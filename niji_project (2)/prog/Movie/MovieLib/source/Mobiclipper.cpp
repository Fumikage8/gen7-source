#if defined(GF_PLATFORM_CTR)

#include "Mobiclipper.h"

#include <mobiclip.h>
#include <nw/snd.h>

#include "helper/helper_Application.h"
#include "helper/helper_AudioDevice.h"
#include "helper/helper_YuvToRgb.h"
#include "heap/include/gfl2_Heap.h"
#include "System/include/HeapDefine.h"
#include <Fade/include/gfl2_FadeManager.h>
#include <System/include/SystemEventManager.h>
#include <Debug/include/gfl2_DebugPrint.h>

//#include "system/gfl_use.h"
//#include "debug/debugwin.h"

#include <util/include/gfl2_GLHeapAllocator.h>
#include "renderer/MobiclipRenderingPipeLine.h"


namespace movie{

static gfl2::heap::HeapBase* s_heap;
bool s_stackInitialized = false;

// ---------------------------------------------------------
// 
// Mobiclip処理の流れ（Playerが内部でやってます）
// 
// 初期化
// ↓
// 再生するMoflex形式の動画ファイルを読み込む
// ↓
// フレームを読み込み、デマクサが映像と音声に分離する
// ↓
// 分離された映像・音声は圧縮されているので、デコーダーが解凍する
// ↓
// 解凍された映像はYUV形式で帰ってくるので、RGB形式に変換
// ↓
// 解凍された映像は、ブロックノイズが気になることがあるので、
// 画質向上のためデブロッキングフィルタをかける
// ↓
// 常に画面に描画しているテクスチャの内容を差し替え、音声フレームも流す
// ↓
// 以下繰り返し
// ↓
// 再生終了
// ↓
// 片付け
// 
// ---------------------------------------------------------


static void* Alloc(size_t size)
{
  return GflHeapAllocMemory(s_heap, size);
}
static void Free(void* ptr)
{
  GflHeapFreeMemory(ptr);
}

// ---------------------------------------------------------

Mobiclipper::Mobiclipper()
{
}

Mobiclipper::~Mobiclipper()
{
}

// ---------------------------------------------------------
void Mobiclipper::Initialize(
          gfl2::heap::NwAllocator* heap_allocator,
          gfl2::heap::NwAllocator* device_allocator,
          gfl2::util::GLHeapAllocator* pGLDeviceAllocator,
          movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine,
          bool enable_sound)
{

  //
 // m_gs = gs;
  m_alloc = device_allocator; //  m_alloc = heap_allocator;    //@ohno
  m_device_alloc = device_allocator;
  m_pGLDeviceAllocator = pGLDeviceAllocator;
  m_renderingPipeLine  = renderingPipeLine;
  m_enable_sound = enable_sound;
  m_stop_seq = 0;
  m_finalize_seq = 0;
  m_presentationInterval = 0;

  // ヒープの確保
  const size_t heap_size = 3*1024*1024;
//  s_heap = gfl2::heap::Manager::CreateHeap(heap_allocator->GetHeapBase(), HEAPID_MOBICLIP, heap_size, gfl::heap::HEAP_TYPE_EXP);

  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  s_heap = GFL_CREATE_LOCAL_HEAP(devHeap,  heap_size, gfl2::heap::HEAP_TYPE_EXP,false);
  
  // mobiclip機能の初期化
//  mw::mo::helper::application::Initialize(); // 便利機能
  mw::mo::filter::Deblocker::Initialize(Alloc, Free); // デブロッキングフィルタ（圧縮した動画はブロックノイズが現れやすい。それを軽減するフィルタ（CPUと引き換えに画質を高めている））
  mw::mo::mobiclip::Decoder::Initialize(Alloc, Free); // Mobiclip形式の動画デコーダー
  mw::mo::imaadpcm::Decoder::Initialize(Alloc, Free); // IMA-ADPCM形式の音声デコーダー
  mw::mo::fastaudio::Decoder::Initialize(Alloc, Free); // FastAudio形式の音声デコーダー
  mw::mo::moflex::Demuxer::Initialize(Alloc, Free); // ムービーを、音部分と映像部分に分ける機能

  if(m_enable_sound)
  {
    nn::snd::SetMasterVolume(1.0f);
    // helper関連の初期化
    mw::mo::helper::audiodevice::Initialize(m_alloc, 1, false);  // オーディオ初期化
    nw::snd::SoundSystem::SetSoundFrameUserCallback(mw::mo::helper::audiodevice::GetSoundCallback(),NULL);
  }

  // helper関連の初期化
  mw::mo::helper::yuvtorgb::Initialize(); // YUVカラーからRGBカラーに変換する機能

  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
}

bool Mobiclipper::Finalize()
{
  switch( m_finalize_seq )
  {
  case 0:
    // helper終了
    mw::mo::helper::yuvtorgb::Finalize();
    ++m_finalize_seq;
    return false;
  case 1:
    mw::mo::moflex::Demuxer::Finalize();
    mw::mo::fastaudio::Decoder::Finalize();
    ++m_finalize_seq;
    return false;
  case 2:
    // Mobiclip機能の終了
    mw::mo::imaadpcm::Decoder::Finalize();
    mw::mo::mobiclip::Decoder::Finalize();
    mw::mo::filter::Deblocker::Finalize();
    // mw::mo::helper::application::Finalize(); // 便利機能
    ++m_finalize_seq;
    return false;
  case 3:
    gfl2::heap::Manager::DeleteHeap(s_heap);
    s_heap = NULL;
    ++m_finalize_seq;
    return false;
  case 4:
    if(m_enable_sound)
    {
      m_audio.StopPlayback();
      nw::snd::SoundSystem::SetSoundFrameUserCallback(NULL,NULL);
      mw::mo::helper::audiodevice::Finalize();
    }
    return true;
  default:
    GFL_ASSERT(0);
    return true;
  }
}

void Mobiclipper::OnSuspendWithSleep( void )     ///< アプリケーション停止処理
{
  GFL_PRINT("Mobi:OnSuspendWithSleep\n");
  m_audio.StopPlayback();
  m_player.StopPlayback();
}

void Mobiclipper::OnResumeWithSleep( void )  ///< アプリケーション復帰処理
{
  GFL_PRINT("Mobi:OnResumeWithHomeMenu\n");
  m_player.StartPlayback();
}

void Mobiclipper::OnSuspendWithHomeMenu( void )
{
  GFL_PRINT("Mobi:OnSuspendWithHomeMenu\n");
  m_audio.StopPlayback();
  m_player.StopPlayback();
}
void Mobiclipper::OnResumeWithHomeMenu( void )
{
 GFL_PRINT("Mobi:OnResumeWithHomeMenu\n");
  m_player.StartPlayback();
}

///電源切断の前にやるべき処理
bool Mobiclipper::OnCloseApplication(void)
{
  while (!Stop()){}
  while (!Finalize()){}
  return true;
}


void Mobiclipper::Play(const Option& opt)
{
  if( m_enable_sound )
  {
    // 指定されたボリュームのセット
    nn::snd::SetMasterVolume(opt.volume);
  }

  m_option = opt;

  // フェードイン開始
  if(opt.fadein_time != 0)
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );

    m_seq = SEQ_FADEIN_WAIT;
    m_fade_timer = 0;
  }
  else
  {
    m_seq = SEQ_MAIN;
  }


  // ファイルのオープン
  nn::Result result = m_file.TryInitialize(opt.filename);
  NN_UTIL_PANIC_IF_FAILED(result);

  // ストリームリーダーの作成
  m_reader.Initialize(&m_file, m_alloc);

  // デマクサの作成
  m_demuxer.Create(m_reader.GetMoflexReader(), m_reader.GetCookie());
  // プレイヤーの作成
  m_player.Create(m_alloc, &m_demuxer, &m_codec, &m_video, &m_audio);

  // 読み込み開始
  mw::mo::helper::PlayerStatus status;
  do
  {
    m_player.Yield();
    status = m_player.GetStatus();
  }
  while(status != mw::mo::helper::PLAYER_ERROR && status != mw::mo::helper::PLAYER_STOPPED);


  if(m_player.GetStatus() != mw::mo::helper::PLAYER_ERROR)
  {
    GFL_PRINT("GetNbAudioStream=%d\n", m_player.GetNbAudioStream());

    switch( opt.screen )
    {
    case gfl2::lyt::DISPLAY_UPPER:
      {
        m_player.SetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_TOP, 0);
        const mw::mo::moflex::VideoStreamInfo* info = m_player.GetVideoStreamInfo(m_player.GetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_TOP));
        m_video.Initialize( m_device_alloc, m_pGLDeviceAllocator, m_renderingPipeLine, info);

        m_presentationInterval = ((s64)info->m_FpsScale * 1000000)/(s64)info->m_FpsRate;
        m_presentationInterval *= m_player.GetNbImageFrames(info);
#if PM_DEBUG
        GFL_PRINT("top\n");
        DebugPrintVideoStreamInfo(info);
#endif
      }
      break;
    case gfl2::lyt::DISPLAY_LOWER:
      {
        m_player.SetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_BOTTOM, 0);
        const mw::mo::moflex::VideoStreamInfo* info = m_player.GetVideoStreamInfo(m_player.GetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_BOTTOM));
        m_video.Initialize( m_device_alloc, m_pGLDeviceAllocator, m_renderingPipeLine, NULL, info);

        m_presentationInterval = ((s64)info->m_FpsScale * 1000000)/(s64)info->m_FpsRate;
        m_presentationInterval *= m_player.GetNbImageFrames(info);
#if PM_DEBUG
        GFL_PRINT("bottom\n");
        DebugPrintVideoStreamInfo(info);
#endif
      }
      break;
    case gfl2::lyt::DISPLAY_BOTH:
      {
        m_player.SetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_TOP, 0);
        m_player.SetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_BOTTOM, 1);

        const mw::mo::moflex::VideoStreamInfo* topInfo = m_player.GetVideoStreamInfo(m_player.GetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_TOP));
        const mw::mo::moflex::VideoStreamInfo* bottomInfo = m_player.GetVideoStreamInfo(m_player.GetCurrentVideoStream(mw::mo::helper::PLAYER_SCREEN_BOTTOM));
        m_video.Initialize( m_device_alloc, m_pGLDeviceAllocator, m_renderingPipeLine, topInfo, bottomInfo);

        m_presentationInterval = ((s64)topInfo->m_FpsScale * 1000000)/(s64)topInfo->m_FpsRate;
        m_presentationInterval *= m_player.GetNbImageFrames(topInfo);
#if PM_DEBUG
        GFL_PRINT("top\n");
        DebugPrintVideoStreamInfo(topInfo);
        GFL_PRINT("bottom\n");
        DebugPrintVideoStreamInfo(bottomInfo);
#endif
      }
      break;
    default:
      GFL_ASSERT_MSG(0,"mobiclip screen type unsupported. screen = %d", opt.screen);
      break;
    }

    m_player.SetCurrentAudioStream(0);

    // オーディオが含まれているなら、オーディオ再生クラスを初期化する
    if(m_player.GetCurrentAudioStream() != -1 && m_enable_sound)
    {
      GFL_PRINT("initialize audio");
      m_audio.Initialize(m_device_alloc, m_player.GetAudioStreamInfo(m_player.GetCurrentAudioStream()));
    }



    m_player.StartPlayback();
    //m_audio.StartPlayback();
  }
}

#if PM_DEBUG
void Mobiclipper::DebugPrintVideoStreamInfo( const mw::mo::moflex::VideoStreamInfo* info )
{
  GFL_PRINT("fpsRate: %d\n", info->m_FpsRate);
  GFL_PRINT("fpsScale: %d\n", info->m_FpsScale);
  GFL_PRINT("Height: %d\n", info->m_Height);
  GFL_PRINT("Width: %d\n", info->m_Width);
  GFL_PRINT("PelRatioRate: %d\n", info->m_PelRatioRate);
  GFL_PRINT("PelRatioScale: %d\n", info->m_PelRatioScale);
  GFL_PRINT("Rotation: %d\n", info->m_Rotation);
  GFL_PRINT("StreamType: %d\n", info->m_StreamType);
  GFL_PRINT("codec: %d\n", info->m_VideoCodec);
}
#endif

bool Mobiclipper::Stop()
{
  switch( m_stop_seq )
  {
  case 0:
    GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);
    m_player.Yield();
    ++m_stop_seq;
    return false;
  case 1:
    m_audio.StopPlayback();
    m_player.StopPlayback();
    ++m_stop_seq;
    return false;
  case 2:
    m_player.Release();
    ++m_stop_seq;
    return false;
  case 3:
    m_video.Finalize(m_renderingPipeLine);
    ++m_stop_seq;
    return false;
  case 4:
    m_audio.Finalize();
    ++m_stop_seq;
    return false;
  case 5:
    m_demuxer.Release();
    ++m_stop_seq;
    return false;
  case 6:
    m_reader.Finalize();
    ++m_stop_seq;
    return false;
  case 7:
    m_file.Finalize();
    return true;
  default:
    GFL_ASSERT(0);
    return true;
  }
}

bool Mobiclipper::Update()
{
  if(m_seq == SEQ_END){return false;}

  //if(m_seq != SEQ_FADEOUT_WAIT)
  {
    // オーディオのデコードが間に合っていない場合、
    // 再度バッファリングする
    if(m_audio.AudioUnderflow())
    {
      m_audio.PausePlayback();
      m_player.PausePlayback();
      m_player.ResumePlayback();
      m_audio.ResumePlayback();
    }


    // 次の映像がまだ間に合ってない場合、
    // 生成を依頼し完了を待つ
    if(!m_player.VideoImageRequestPending())
    {
      s64 presentationElapsed = m_player.VideoPresentationTimeElapsed();
      if( presentationElapsed >= m_presentationInterval )
      {
        GFL_PRINT( "movie frame skiped\n" );
        m_player.VideoImageSkip();
      }
      else 
      {
        nngxWaitCmdlistDone();  // @attention オレンジ箱(拡張アプリ(CTRの3倍速))でムービーが乱れたので、次の描画が始まる前にコマンドリストの完了を待つことにした。ダブルバッファにしていればこれは不要だと思う。
        if(m_player.VideoImageRequestStart())
          m_player.WaitVideoImage();
      }
    }
  }

  // 画面フェードに合わせてMasterVolumeを弄ることで音声もフェードさせる
  switch(m_seq)
  {
  case SEQ_MAIN:
    #if PM_DEBUG
//    if(m_enable_sound)
//      nn::snd::SetMasterVolume(GFL_SINGLETON_INSTANCE(xy_debug::DebugWinWork)->mMovieVolume);
    #endif
    break;
  case SEQ_FADEIN_WAIT:
    ++m_fade_timer;
    if(m_enable_sound && m_option.audioFade)
      nn::snd::SetMasterVolume(m_option.volume * m_fade_timer / m_option.fadein_time);
    if ( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE) )
    {
      if(m_enable_sound && m_option.audioFade)
        nn::snd::SetMasterVolume(m_option.volume);
      m_seq = SEQ_MAIN;
    }
    break;
  case SEQ_FADEOUT_WAIT:
    GFL_PRINT("fadeout %f\n",m_option.volume - m_option.volume * m_fade_timer / m_option.fadeout_time);
    ++m_fade_timer;

    if(m_enable_sound && m_option.audioFade)
      nn::snd::SetMasterVolume(m_option.volume - m_option.volume * m_fade_timer / m_option.fadeout_time);

		if ( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE) )
    {
      m_seq = SEQ_END;
      return false;
    }
    break;
  }

  return true;
}

bool Mobiclipper::IsPlaying()
{
  return m_player.GetStatus() != mw::mo::helper::PLAYER_STOPPED;
}

void Mobiclipper::PlayAgain()
{
  m_audio.StopPlayback();
  m_player.StopPlayback();
  m_player.SetPositionByFileOffset(0);
  m_player.StartPlayback();
}

bool Mobiclipper::Draw()
{
  //デコード等が完了し、描画すべき画像が得られているならば
  bool ret = false;
  if(m_player.VideoImageAvailable())
  {
    // 描画が遅れていれば飛ばす
    if(m_player.VideoPresentationTimeElapsed()>=0)
    {
      // 遅れていないので描画を更新
      m_video.UpdateTexture();
      m_video.Draw();
      ret = true;
    }
  }
  return ret;
}

void Mobiclipper::DrawEnd()
{
  m_player.VideoImageRequestEnd();
  m_audio.StartPlayback();
  mw::mo::helper::PlayerStatus status = m_player.GetStatus();

  m_player.Yield();
  if(status == mw::mo::helper::PLAYER_STOPPED)
  {
    // 最後まで再生が終わったらそのまま止めておく
    m_player.StopPlayback();
  }
}

void Mobiclipper::RequestEnd()
{
  if(m_seq == SEQ_MAIN)
  {
    if(m_option.fadeout_time == 0)
    {
      m_seq = SEQ_END;
    }
    else
    {
      static const gfl2::math::Vector4 scou(0.0f,0.0f,0.0f,0.0f);
      static const gfl2::math::Vector4 ecou(0.0f,0.0f,0.0f,255.0f);
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->FadeManager::RequestOut( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &scou, &ecou, m_option.fadeout_time );

      m_seq = SEQ_FADEOUT_WAIT;
      m_fade_timer = 0;
    }
  }
}

void Mobiclipper::DrawCost()
{
}


void Mobiclipper::EnableDeblocking(bool flag)
{
  m_player.EnableDeblocking(flag);
}

bool Mobiclipper::IsUpdate()
{
  return m_seq == SEQ_MAIN;
}



}// namespace movie
#endif// defined(GF_PLATFORM_CTR)

//=============================================================================
/**
 * @brief  サウンド全体の初期化・更新・後片付け
 * @file   sound_BGM.cpp
 * @author obata
 * @date   2011.06.29
 */
//=============================================================================

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include "Sound/include/Sound.h"
#include "Sound/include/SoundLocal.h"
#include "Sound/include/PMSoundSystem.h"
#include "System/include/HeapDefine.h"


#if SOUND_UPDATE_THREAD_ENABLE
#include "Sound/include/SoundUpdateThread.h"
#endif

#if 0
#ifdef DEBUG_ONLY_FOR_iwasawa
#include "system/main.h"  //テストコード用
#endif
#endif

namespace Sound { 

  static b32 s_is_initialized = false;
  b32 IsInitialized(){return s_is_initialized;}

#if SOUND_UPDATE_THREAD_ENABLE
  static gfl2::thread::CriticalSection s_criticalSection;
  static Sound::SoundUpdateThread* s_soundUpdateThread = NULL;
  gfl2::thread::CriticalSection& GetCriticalSection(){return s_criticalSection;}
#endif

  //---------------------------------------------------------------------------
  /**
   * @brief サウンド初期化
   * @param systemHeap サウンドに使用するシステムヒープ
   * @param deviceHeap サウンドに使用するデバイスヒープ
   */
  //---------------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap )
  {
    DEBUG_SOUND_OFF_RETNONE;

    if(!s_is_initialized)
    {
#if SOUND_UPDATE_THREAD_ENABLE
      s_criticalSection.Initialize();
#endif
      s_is_initialized = true;

      Initialize_SYSTEM( systemHeap, deviceHeap );
      Initialize_BGM( systemHeap, deviceHeap );
      Initialize_SE( systemHeap, deviceHeap );
      Initialize_VOICE( systemHeap, deviceHeap );
      Initialize_PLAYER();

      // 常駐SEを読み込む
      LoadAndPushSESoundData( GROUP_GLOBAL );
      LoadAndPushSESoundData( GROUP_FIELD_GLOBAL );
      LoadAndPushSESoundData( GROUP_APP_GLOBAL );

      //本体設定からサウンドのアウトプットモードを初期化
      SetOutputModeByConfig();

#if SOUND_UPDATE_THREAD_ENABLE
      s_soundUpdateThread = GFL_NEW(deviceHeap) Sound::SoundUpdateThread( deviceHeap );
#endif
    }
  }



  //---------------------------------------------------------------------------
  /**
   * @brief サウンド後片付け
   */
  //---------------------------------------------------------------------------
  void Finalize( void )
  {
    if(s_is_initialized)
    {
#if SOUND_UPDATE_THREAD_ENABLE
      GFL_SAFE_DELETE( s_soundUpdateThread );
#endif

      s_is_initialized = false;

#if 1
      //BTS4850 全階層のSEを破棄 ソフトウェアリセット対策
      PopAndUnloadSESoundDataAll();
#else
      // 常駐SEを破棄
      PopAndUnloadSESoundData( GROUP_APP_GLOBAL );
      PopAndUnloadSESoundData( GROUP_FIELD_GLOBAL ); 
      PopAndUnloadSESoundData( GROUP_GLOBAL ); 
#endif
      Finalize_VOICE();
      Finalize_PLAYER();
      Finalize_SE();
      Finalize_BGM();
      Finalize_SYSTEM();
#if SOUND_UPDATE_THREAD_ENABLE
      s_criticalSection.Finalize();
#endif
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief サウンド後片付け 電源Off&ソフトウェアリセット用
   *
   * @note  BTS6040,BTS6242,BTS6269,BTS6274対策
   *
   * 今鳴っているもの、鳴ろうとしているものを処理してからFinalizeを実行する。
   * 待っている間、この関数から抜けないので注意
   */
  //---------------------------------------------------------------------------
  void FinalizeForGameReset( gfl2::fs::AsyncFileManager* pFileReadManager )
  {
    static const int SLEEP_TIME = (1000/60);				//スリープ時間(ms)

    if(!s_is_initialized){
      return; //そもそも初期化されてないので何もしなくていい
    }

#if SOUND_UPDATE_THREAD_ENABLE
    GFL_SAFE_DELETE( s_soundUpdateThread );
#endif

    //現在なっているBGMとSEを止めて、一度システム状態更新のためにUpdateを回す
    StopAllBGM();
    StopAllSE();
    Update_SYSTEM();
    gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );

#if PM_DEBUG
    int count = 0;
#endif
    //ポケモンボイスが止まるのを待つ
    while( ResetVoiceReq( pFileReadManager ) == false ){
      Update_SYSTEM();
      gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );
#if PM_DEBUG
      count++;
#endif
    };
#if PM_DEBUG
    GFL_PRINT("FinalizeVoiceWait %d\n", count );
    count = 0;
#endif

#if 1
#ifdef DEBUG_ONLY_FOR_iwasawa
    //テストコード 130505 by iwasawa
    if( WaitLoadAndPushSESoundDataAsync() == false ){
      gfl2::heap::HeapBase * p_tmp_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
      Sound::LoadAndPushSESoundDataAsync( p_tmp_heap, GROUP_FIELD_RIDE_KENTAROS );
    }
#endif
#endif
    //非常駐SEグループの非同期読み込みを待つ
    while( WaitLoadAndPushSESoundDataAsync() == false ){
      Update_SYSTEM();
      gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );
#if PM_DEBUG
      count++;
#endif
    }
#if PM_DEBUG
    GFL_PRINT("FinalizeLoadSESoundWait %d\n", count );
    count = 0;
#endif
    //スレッド対策の保険でさらにしばらく待ってみる
    for( int i = 0;i < 15;i++){
      Update_SYSTEM();
      gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );
    }
    Finalize(); //システム解放
  }

  //---------------------------------------------------------------------------
  /**
   * @brief サウンド更新
   */
  //---------------------------------------------------------------------------
  void Update( void )
  {
    if(s_is_initialized)
    {
      SOUND_UPDATE_CRITICALSECTION;

      Update_BGM();
      Update_VOICE();
      Update_PLAYER();
      Update_SYSTEM();
    }
  }

#if SOUND_UPDATE_THREAD_ENABLE
  //---------------------------------------------------------------------------
  /**
   * @brief サウンド更新
   */
  //---------------------------------------------------------------------------
  void UpdateThreadSync( void )
  {
    if(s_is_initialized)
    {
      s_soundUpdateThread->Signal();
    }
  }
#endif

  //---------------------------------------------------------------------------
  // サウンドモード変更
  //--------------------------------------------------------------------------
  void SetOutputModeByConfig( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(Sound::IsInitialized() == false ) return;
    gfl2::sound::SoundSystem::OutputMode mode;

#if defined( GF_PLATFORM_CTR )
    mode = static_cast<gfl2::sound::SoundSystem::OutputMode>(nn::cfg::GetSoundOutputMode());
#elif defined( GF_PLATFORM_WIN32 )
    mode = gfl2::sound::SoundSystem::OUTPUT_MODE_SURROUND;
#endif

    // 初期設定
    switch(mode){
    case gfl2::sound::SoundSystem::OUTPUT_MODE_MONO:
      Sound::SetOutputModeMono();
      break;
    case gfl2::sound::SoundSystem::OUTPUT_MODE_STEREO:
      Sound::SetOutputModeStereo();
      break;
    case gfl2::sound::SoundSystem::OUTPUT_MODE_SURROUND:
      Sound::SetOutputModeSurround();
      break;
    }
  }
  gfl2::sound::SoundSystem::OutputMode GetOutputMode( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

     DEBUG_SOUND_OFF_RET(gfl2::sound::SoundSystem::OUTPUT_MODE_SURROUND);
    return s_PMSoundSystem->GetOutputMode();
  }
  void SetOutputModeMono( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetOutputModeMono();
  }
  void SetOutputModeStereo( void )
  { 
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetOutputModeStereo(); 
  }
  void SetOutputModeSurround( void )
  { 
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetOutputModeSurround(); 
  }

	//--------------------------------------------------------------------------
	// マスターボリューム変更
	//		マスターボリュームスイッチフラグ on = true. off = false
	//--------------------------------------------------------------------------
	void SoundMasterVolumeSW(b32 OnOff)
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SoundMasterVolumeSW(OnOff); 
  }
  

  //---------------------------------------------------------------------------
  // エフェクトの追加
  //  XYでは下記になっている
  //        BUS_A→リバーブ専用
  //        BUS_B→鳴き声の波形を読み取るクラス専用
  //--------------------------------------------------------------------------- 
  /**
   * @brief BUS_Aにリバーブエフェクトを追加する
   *
   * @param reverb_buffer_heap  リバーブバッファー用ヒープ
   * @param param               設定構造体
   *
   * @return true追加できた falseできなかった
   */
#if defined(GF_PLATFORM_CTR)
  b32 AppendReverbEffect( gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    return s_PMSoundSystem->AppendReverbEffect( nw::snd::AUX_BUS_A, reverb_buffer_heap, param );
  }
#else
  b32 AppendReverbEffect( gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverbStd::ReverbStdParam & param )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    return s_PMSoundSystem->AppendReverbEffect( nw::snd::AUX_BUS_A, reverb_buffer_heap, param );
  }
#endif
  /**
   * @brief BUS_Aのリバーブエフェクトを破棄する
   *
   * @param fadeTimes フェードタイム（ミリ秒）
   */
  void RemoveReverbEffect( int fadeTimes )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->RemoveReverbEffect( nw::snd::AUX_BUS_A, fadeTimes );
  }
  /**
   * @brief BUS_Bに鳴き声用の波形読み取りエフェクトを追加する
   *
   * @param effect  読み取りエフェクト
   *
   * @return true追加できた falseできなかった
   */
  b32 AppendEffect( nw::snd::FxBase *effect )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    return s_PMSoundSystem->AppendEffect( nw::snd::AUX_BUS_B, effect );
  }
  /**
   * @brief BUS_Bのエフェクトを破棄する
   *
   * @param fadeTimes フェードタイム（ミリ秒）
   */
  void RemoveEffect( int fadeTimes )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->RemoveEffect( nw::snd::AUX_BUS_B, fadeTimes );
  }

} // namespace Sound

#if defined(GF_PLATFORM_CTR)

//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @author ito_isao
 *
 */
//====================================================================== 

#include "MobiclipScene.h"

#include "MovieProc.h"
//#include "system/main.h"

//#include "app/util/app_util_Common.h"

//#include "heap/gfl_HeapMemory.h"
//#include "debug/debugwin.h"
#include "Sound/include/Sound.h"
#include "Fade/include/gfl2_FadeTypes.h"
#include <Fade/include/gfl2_FadeManager.h>
#include <App/AppEvent/include/Movie/MovieAppParam.h>

#include "DebugMovie.h"

namespace movie{

MobiclipScene::MobiclipScene() : m_decide(RET_EXIT), m_loop(false), m_canSkip(false),m_titleMode(false),m_decidedSE(SMID_NULL),m_seq(0), m_hold(0)
{
}

MobiclipScene::~MobiclipScene()
{
}

void MobiclipScene::Enter(int movieno)
{

	s_mobiclip.Initialize(m_owner->GetAllocator(), m_owner->GetDeviceAllocator(), m_owner->GetGLDeviceAllocator(), m_owner->GetRenderingPipeLine(), true);

  movie::Option opt;
  
  opt.fadein_time = gfl2::Fade::FADE_DEFAULT_SYNC;
  opt.fadeout_time = gfl2::Fade::FADE_DEFAULT_SYNC;
  opt.volume = 1.00f;
  opt.screen = gfl2::lyt::DISPLAY_UPPER;
  m_canSkip = false;
  m_titleMode = false;
  m_decidedSE = SMID_NULL;

  switch(movieno){
  case App::Event::MOVIE_TITLE_JAPAN:
    opt.filename = L"rom:/m/title_jp.moflex";
    break;
  case App::Event::MOVIE_TITLE_ENGLISH:
    opt.filename = L"rom:/m/title_us.moflex";
    break;
  case App::Event::MOVIE_TITLE_FRANCE:
    opt.filename = L"rom:/m/title_fr.moflex";
    break;
  case App::Event::MOVIE_TITLE_ITALY:
    opt.filename = L"rom:/m/title_it.moflex";
    break;
  case App::Event::MOVIE_TITLE_GERMANY:
    opt.filename = L"rom:/m/title_de.moflex";
    break;
  case App::Event::MOVIE_TITLE_SPAIN:
    opt.filename = L"rom:/m/title_es.moflex";
    break;
  case App::Event::MOVIE_TITLE_KOREA:
    opt.filename = L"rom:/m/title_kr.moflex";
    break;
  case App::Event::MOVIE_TITLE_CHINA:
    opt.filename = L"rom:/m/title_chs.moflex";
    break;
  case App::Event::MOVIE_TITLE_TAIWAN:
    opt.filename = L"rom:/m/title_cht.moflex";
    break;
  default:
    GFL_ASSERT(0);
    opt.filename = L"rom:/m/title_jp.moflex";
  }

  opt.screen = gfl2::lyt::DISPLAY_BOTH;
  opt.fadeout_time = 16;
  opt.audioFade = false;
#if PM_VERSION == VERSION_SUN2
  m_decidedSE = SEQ_SE_TITLESTART_SUN;
#else
  m_decidedSE = SEQ_SE_TITLESTART_MOON;
#endif
  m_owner->SetLoopFlag(true);
  m_canSkip = true;
  m_titleMode = true;

#if PM_DEBUG
  //	GFL_SINGLETON_INSTANCE(xy_debug::DebugWinWork)->mMovieVolume = opt.volume;
#endif

  s_mobiclip.EnableDeblocking(false);
  s_mobiclip.Play(opt);

  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );


}


  

int MobiclipScene::Update(int no)
{

	if(!s_mobiclip.IsPlaying())
	{
#if PM_DEBUG
    if( GFL_SINGLETON_INSTANCE(DebugMovie)->IsLoop() || m_owner->GetLoopFlag() )
#else
    if(m_owner->GetLoopFlag())
#endif //PM_DEBUG
    {
      // 再生完了したら再度再生する
      s_mobiclip.PlayAgain();
    }
    else
    {
      // 再生完了していたら抜ける
  		s_mobiclip.RequestEnd();
    }
	}

  // @attention 60FPSのTriggerがおかしい
  // 原因：GflUse で、Mainが2回よばれているため。（整合性をあわせるために無理やりSyncro‥まで読んでる）
  //      影響がでかいので、暫定対処とする。（一回直してみたら、30FPS の Repeat がおかしくなった）
  u32 now = m_owner->GetButton()->GetHold();

  if( s_mobiclip.IsUpdate() )
  {
    //Triggerを取る
    u32 trigger = (~m_hold) & now;

    if( m_canSkip &&
       (trigger & gfl2::ui::BUTTON_A) || 
       (trigger & gfl2::ui::BUTTON_START ) )
    {
      if( m_decidedSE != SMID_NULL ) 
      {
        Sound::PlaySE( m_decidedSE );
      }

      s_mobiclip.RequestEnd();
      m_decide = RET_DECIDE;
    }
    else if( m_titleMode && (m_owner->GetButton()->IsHold(gfl2::ui::BUTTON_UP) &&
       m_owner->GetButton()->IsHold(gfl2::ui::BUTTON_B) && 
       m_owner->GetButton()->IsHold(gfl2::ui::BUTTON_X)) )
    {
      s_mobiclip.RequestEnd();
      m_decide = RET_UPBX;
    }
#if PM_DEBUG
    else if( trigger & gfl2::ui::BUTTON_Y )
    {
      s_mobiclip.RequestEnd();
      m_decide = RET_Y;
    }

    if( GFL_SINGLETON_INSTANCE(DebugMovie)->IsSkip() )
    {
      s_mobiclip.RequestEnd();
      m_decide = RET_DECIDE;
    }
#endif
  }
  m_hold = now;

	if(!s_mobiclip.Update())
	{
    m_seq = 0;
	  return m_decide;
	}

	double time = nn::os::Tick::GetSystemCurrent();
	s_time = time - s_before;
	s_before = time;

	return RET_NONE;
}

bool MobiclipScene::DrawTop(void)
{
	return s_mobiclip.Draw();
//	s_mobiclip.DrawCost();
//	gfl::grp::util::DrawUtil::DrawText(0,200,L"cpu cost: %f", s_time*100.0 / nn::os::Tick::TICKS_PER_SECOND);
}

bool MobiclipScene::DrawBottom(void)
{
  return true;
	//s_mobiclip.Draw();  //Draw内部で上下書き込みしているので、必要ない
}

void MobiclipScene::DrawEnd(void)
{
  s_mobiclip.DrawEnd();
}

bool MobiclipScene::Exit()
{
  enum {
    SEQ_STOP1,
    SEQ_STOP2,
    SEQ_EXIT,
  };

  switch(m_seq)
  {
  case SEQ_STOP1:
    GFL_DELETE m_layout;
    m_layout = NULL;
    ++m_seq;
    // fall throght
  case SEQ_STOP2:
    if( s_mobiclip.Stop() )
    {
      ++m_seq;
    }
    return false;
  case SEQ_EXIT:
	  if( s_mobiclip.Finalize() )
    {
      //内部で nn::snd でVoiceやWaveBufferを確保しているため、サウンドを再初期化する
      nn::snd::SetMasterVolume(1.0);
      return true;
    }
    return false;
  default:
    GFL_ASSERT(0);
    return true;
  }
}


}
#endif //defined(GF_PLATFORM_CTR)

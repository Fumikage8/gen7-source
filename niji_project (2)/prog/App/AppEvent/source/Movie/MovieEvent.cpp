//===================================================================
/**
 *	@file	    MovieEvent.cpp
 *	@brief    ムービーのイベント
 *	@author	  k.ohno
 *	@date		  2015.10.18
 */
//===================================================================

#include "util/include/gfl2_std_string.h"

#include "App/AppEvent/include/Movie/MovieEvent.h"
#include "App/AppEvent/include/Movie/MovieEventListener.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Sound/include/Sound.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"


#include "Movie/MovieLib/source/MovieProc.h"

#include <GameSys/include/DllProc.h>

#if PM_DEBUG
#include "Debug/StartMenu/include/StartLauncher.h"
#endif



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Event )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------
enum
{
  EVSEQ_APP_INIT,
  EVSEQ_WAIT_TEST,
  EVSEQ_END,
};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
 */
//-----------------------------------------------------------------------------

#if PM_DEBUG
bool MovieEvent::mbDebug;
#endif

//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
MovieEvent* MovieEvent::StartEvent( GameSys::GameManager* p_gameman, bool bDebug, u32 return_bgm_id )
#else
MovieEvent* MovieEvent::StartEvent( GameSys::GameManager* p_gameman, u32 return_bgm_id )
#endif
{
#if PM_DEBUG
  mbDebug = bDebug;
#endif

  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  MovieEvent* pEvent =  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, MovieEvent>( p_geventman );
  pEvent->SetReturnBGMID(return_bgm_id);
  return pEvent;
}





//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
MovieEvent::MovieEvent( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap ),
mSeq( 0 )
{
  mSeq = EVSEQ_APP_INIT;
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
MovieEvent::~MovieEvent()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool MovieEvent::BootChk( GameSys::GameManager*  )
{
  return true;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void MovieEvent::InitFunc( GameSys::GameManager*  )
{
}



//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT MovieEvent::MainFunc( GameSys::GameManager* game_manager )
{
  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();
  gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  
#if defined(GF_PLATFORM_CTR)
  switch( mSeq ) {
  case EVSEQ_APP_INIT:
    mpProc = GameSys::CallMovieProc();
    mpProc->SetNo(mAppParam.movieNo);
    mSeq = EVSEQ_WAIT_TEST;
    break;
  case EVSEQ_WAIT_TEST:       // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess(mpProc) == false )
    {
      if(mpMovieEventListener){
        mpMovieEventListener->OnTermination();
      }
      mpProc = NULL;
      mSeq = EVSEQ_END;

#if PM_DEBUG
      if(mbDebug)
      {
        debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
        debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( proc_manager, &m_pLauncherParam );
        launcher->CreateStartListData<debug::startmenu::StartLauncher>();
      }else
#endif  // PM_DEBUG
      {
        Field::FieldSound* pFieldSound = game_manager->GetGameData()->GetFieldSound();
        if( mReturnBGMID == 0 ){
          Sound::StartBGMReq( pFieldSound->GetFieldBGM(), Sound::BGM_FADE_VERY_FAST );
        }else{
          Sound::StartBGMReq( mReturnBGMID, Sound::BGM_FADE_VERY_FAST );
        }
      }
      return GameSys::GMEVENT_RES_FINISH;
    }
    break;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
#else// defined(GF_PLATFORM_CTR)
  return GameSys::GMEVENT_RES_FINISH;
#endif// defined(GF_PLATFORM_CTR)
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void MovieEvent::EndFunc( GameSys::GameManager* )
{
}


//--------------------------------------------------------------
/**
 * @brief   初期化データをセット
 * @param   pMovieEventListener
 */
//--------------------------------------------------------------
void MovieEvent::SetAppParam(const APP_PARAM* pAppParam)
{
  gfl2::std::MemCopy(pAppParam,&mAppParam, sizeof(APP_PARAM));
}



GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

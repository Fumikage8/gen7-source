//===================================================================
/**
 *	@file	  ThreadTest.cpp
 *	@brief  スレッドサンプル
 *	@author	k.ohno
 *	@date		2015.3.13
 *
 */
//===================================================================
#if PM_DEBUG

#include "Test/MenuFrameTest/include/ThreadSampleEvent.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>


#include "Thread/include/gfl2_ThreadStatic.h"

#include "Test/MenuFrameTest/include/ThreadTestProc.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ThreadTest )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------
enum
{
  EVSEQ_INIT,
  EVSEQ_INIT_TEST,
  EVSEQ_WAIT_TEST,
  EVSEQ_JUMP_TITLE,
  EVSEQ_END,
};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------
void ThreadSampleEvent::StartEvent( GameSys::GameManager* p_gameman )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, ThreadSampleEvent>( p_geventman );
}

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
ThreadSampleEvent::ThreadSampleEvent( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap ),
mSeq( 0 )
{
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
ThreadSampleEvent::~ThreadSampleEvent()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool ThreadSampleEvent::BootChk( GameSys::GameManager*  )
{
  return true;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void ThreadSampleEvent::InitFunc( GameSys::GameManager*  )
{
}





//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT ThreadSampleEvent::MainFunc( GameSys::GameManager* game_manager )
{
  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();
//  gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT );

  switch( mSeq ) {
  case EVSEQ_INIT:
    NIJI_PROC_SETNEXT< GameSys::GameProcManager, Test::ThreadTest::Proc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

    mSeq = EVSEQ_INIT_TEST;
    break;
  case EVSEQ_INIT_TEST:
    if( game_manager->IsProcessExists() == false ){  //プロセスが終了する
      mSeq = EVSEQ_WAIT_TEST;
    }
    break;
  case EVSEQ_WAIT_TEST:
    mSeq = EVSEQ_JUMP_TITLE;
    break;
  case EVSEQ_JUMP_TITLE:
#if PM_DEBUG
    { // ランチャー
      debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
      debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( proc_manager, &m_pLauncherParam );
      launcher->CreateStartListData<debug::startmenu::StartLauncher>();
      }
#endif  // PM_DEBUG
    mSeq = EVSEQ_END;
    return GameSys::GMEVENT_RES_FINISH;

  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void ThreadSampleEvent::EndFunc( GameSys::GameManager*  )
{

}



GFL_NAMESPACE_END( ThreadTest )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG

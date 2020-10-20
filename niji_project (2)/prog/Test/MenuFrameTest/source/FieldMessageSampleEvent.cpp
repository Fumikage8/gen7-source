//===================================================================
/**
 *	@file	  FieldMessageSampleEvent.cpp
 *	@brief  フィールドメッセージのサンプル
 *	@author	k.ohno
 *	@date		2015.3.13
 *
 */
//===================================================================
#if PM_DEBUG

#include "Test/MenuFrameTest/include/FieldMessageSampleEvent.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>


#include "Thread/include/gfl2_ThreadStatic.h"

#include "Test/MenuFrameTest/include/FieldMessageSampleProc.h"
#include "FieldMessageSampleFrame.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldMessageTest )

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
void FieldMessageSampleEvent::StartEvent( GameSys::GameManager* p_gameman )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, FieldMessageSampleEvent>( p_geventman );
}

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
FieldMessageSampleEvent::FieldMessageSampleEvent( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap ),
mSeq( 0 )
{
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
FieldMessageSampleEvent::~FieldMessageSampleEvent()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool FieldMessageSampleEvent::BootChk( GameSys::GameManager*  )
{
  return true;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void FieldMessageSampleEvent::InitFunc( GameSys::GameManager*  )
{
}





//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT FieldMessageSampleEvent::MainFunc( GameSys::GameManager* game_manager )
{
  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();
//  gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT );

  switch( mSeq ) {
  case EVSEQ_INIT:
    mpProc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, Test::FieldMessageTest::Proc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
    mpProc->SetAsyncFileManager(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager());
    mSeq = EVSEQ_INIT_TEST;
    break;
  case EVSEQ_INIT_TEST:
    if( game_manager->IsProcessExists() == false ){  //万が一先にプロセスが終了する場合にはセットアップしない
      GFL_ASSERT(0);//とはいえ想定外
      mSeq = EVSEQ_JUMP_TITLE;
    }
    else if(mpProc->GetFrameManager()){
      //起動フレーム作成
      FieldMessageSampleFrame* pFieldMessageSampleFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, FieldMessageSampleFrame>( mpProc->GetFrameManager() );
      //セットアップ
      mAppParam.mpAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      mAppParam.mpAppRenderingManager = mpProc->GetAppRenderingManager();
      mAppParam.mpAppHeap = mpProc->GetAppHeap();
      mAppParam.mpAppFrameManager = mpProc->GetFrameManager();
      pFieldMessageSampleFrame->Setup( &mAppParam );
      mSeq = EVSEQ_WAIT_TEST;
    }
    break;
  case EVSEQ_WAIT_TEST:
    // プロセスの終了待ち
    if( game_manager->IsProcessExists() == false ){
      GFL_PRINT("outno %d\n", mAppParam.outNo);
      mSeq = EVSEQ_JUMP_TITLE;
    }
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
void FieldMessageSampleEvent::EndFunc( GameSys::GameManager*  )
{

}



GFL_NAMESPACE_END( FieldMessageTest )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG

//===================================================================
/**
 *	@file	    CongratsEvent.cpp
 *	@brief    
 *	@author	  k.ohno
 *	@date		  2016.1.19
 */
//===================================================================

#include "App/AppEvent/include/Congrats/CongratsEvent.h"

#include "System/include/HeapDefine.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "GameSys/include/DllProc.h"

#include "App/AppDemo/include/AppDemoProc.h"
#include "App/GameClearSave/include/GameClearSaveProc.h"
#include <App/DendouDemo/include/DendouDemoProc.h>

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Event )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------
namespace{
enum
{
  EVSEQ_APP_START,
  EVSEQ_APP_WAIT,
  EVSEQ_BPART_START,
  EVSEQ_BPART_WAIT,
  EVSEQ_END,
};

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


//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------
CongratsEvent* CongratsEvent::StartEvent( GameSys::GameManager* p_gameman, bool isAlreadyClear )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  CongratsEvent* pEvent =  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, CongratsEvent>( p_geventman );
  pEvent->SetParams(isAlreadyClear);
  return pEvent;
}





//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
CongratsEvent::CongratsEvent( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap ),
mSeq( 0 )
{
  mSeq=EVSEQ_APP_START;
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
CongratsEvent::~CongratsEvent()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool CongratsEvent::BootChk( GameSys::GameManager*  )
{
  return true;
}


//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void CongratsEvent::InitFunc( GameSys::GameManager*  )
{

}



//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT CongratsEvent::MainFunc( GameSys::GameManager* game_manager )
{
  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();
  
  switch( mSeq ) {
  case EVSEQ_APP_START:
    {
      gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      gfl2::heap::HeapBase * heapevent = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
      mpParam = GFL_NEW(heapevent) App::AppDemo::AppDemoProcParam_Congrats;

      mpParam->pHeap = heap;
      mpParam->pPokeParty = game_manager->GetGameData()->GetPlayerParty();
      mpParam->pMyStatus = game_manager->GetGameData()->GetPlayerStatus();
      mpParam->isFirstClear = !mIsAlreadyClear;
      mpParam->defensedCount = game_manager->GetGameData()->GetRecordSaveData()->Get(Savedata::Record::RECID_DENDOU_CNT);
      mpParam->SetGArcId();

      mpProc = GameSys::CallAppDemoProc_Congrats( mpParam );
      mSeq = EVSEQ_APP_WAIT;
    }
    break;
  case EVSEQ_APP_WAIT:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess(mpProc) == false )
    {
      mpProc = NULL;
      GFL_SAFE_DELETE(mpParam);
      mSeq = EVSEQ_BPART_START;
    }
    break;
  case EVSEQ_BPART_START:
    {
      gfl2::heap::HeapBase * heapevent = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
      mpBPartParam = GFL_NEW(heapevent) app::dendou_demo::APP_PARAM();
      mpBPartParam->pPokeParty = game_manager->GetGameData()->GetPlayerParty();
      mpBPartParam->pMyStatus = game_manager->GetGameData()->GetPlayerStatus();
      mpBPartParam->isFirstClear = !mIsAlreadyClear;
      mpBPartParam->defensedCount = game_manager->GetGameData()->GetRecordSaveData()->Get(Savedata::Record::RECID_DENDOU_CNT);

      mpBPartProc = GameSys::CallDendouDemoProc( mpBPartParam );
      mSeq = EVSEQ_BPART_WAIT;
    }
    break;
  case EVSEQ_BPART_WAIT:
    if( proc_manager->IsRegisterProcess(mpBPartProc) == false )
    {
      mpBPartProc = NULL;
      GFL_SAFE_DELETE(mpBPartParam);
      mSeq = EVSEQ_END;
    }
    break;
  case EVSEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  default:
    GFL_ASSERT(0);
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}



//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void CongratsEvent::EndFunc( GameSys::GameManager*  )
{
}





GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

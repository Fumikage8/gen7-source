// ============================================================================
/*
 * @file BattleRoyalResultEvent.cpp
 * @brief バトルロイヤルの結果アプリのイベント
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/AppEvent/include/BattleRoyalResult/BattleRoyalResultEvent.h"
#include "App/BattleRoyalResult/include/BattleRoyalResultProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


  void BattleRoyalResultEvent::StartEvent( GameSys::GameManager* pGameManager, App::BattleRoyalResult::APP_PARAM* pAppParam )
  {
    GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
    BattleRoyalResultEvent* pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, BattleRoyalResultEvent>( pGameEventManager );
    GFL_ASSERT( pEvent );
    pEvent->m_pAppParam = pAppParam;
  }


  BattleRoyalResultEvent::BattleRoyalResultEvent( gfl2::heap::HeapBase* pHeap ) :
    NetEvent::NetEventBase( pHeap )
  {
    _Clear();
  }


  BattleRoyalResultEvent::~BattleRoyalResultEvent()
  {
  }


  bool BattleRoyalResultEvent::BootChk( GameSys::GameManager* pGameManager )
  {
    return true;
  }


  void BattleRoyalResultEvent::InitFunc( GameSys::GameManager* pGameManager )
  {
    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    //  proc生成
    m_pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::BattleRoyalResult::BattleRoyalResultProc, App::BattleRoyalResult::APP_PARAM>( 
              pGameManager->GetProcManager(), m_pAppParam, pGameManager->GetAsyncFileManager() , "BattleRoyalResult.cro" );
  }


  void BattleRoyalResultEvent::EndFunc( GameSys::GameManager* pGameManager )
  {
    m_pAppParam = NULL;
  }


  GameSys::GMEVENT_RESULT BattleRoyalResultEvent::MainFunc( GameSys::GameManager* pGameManager )
  {
    //  procが死んでいるかチェック
    if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
    {
      if( IsError() )
      {
        //  通信失敗
        return GameSys::GMEVENT_RES_FINISH;
      }
      else
      {
        //  正常終了
        return GameSys::GMEVENT_RES_FINISH;
      }
    }

    return GameSys::GMEVENT_RES_CONTINUE;
  }


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

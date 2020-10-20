// ============================================================================
/*
 * @file SuperTrainingEvent.cpp
 * @brief すごい特訓アプリのイベント
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include "App/AppEvent/include/SuperTraining/SuperTrainingEvent.h"
#include "App/SuperTraining/include/SuperTrainingProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


  void SuperTrainingEvent::StartEvent( GameSys::GameManager* pGameManager, App::SuperTraining::APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );

    GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
    SuperTrainingEvent* pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, SuperTrainingEvent>( pGameEventManager );

    pEvent->m_pAppParam = pAppParam;
  }


  SuperTrainingEvent::SuperTrainingEvent( gfl2::heap::HeapBase* pHeap )
    : GameSys::GameEvent( pHeap )
    , m_pAppParam( NULL )
    , m_pProc( NULL )
  {
  }


  SuperTrainingEvent::~SuperTrainingEvent()
  {
  }


  bool SuperTrainingEvent::BootChk( GameSys::GameManager* pGameManager )
  {
    return true;
  }


  void SuperTrainingEvent::InitFunc( GameSys::GameManager* pGameManager )
  {
    // @note : 最初にdllを読み込まないとdll内のクラスをnew出来ない。要注意
    //  proc生成
    m_pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::SuperTraining::SuperTrainingProc, App::SuperTraining::APP_PARAM>( 
              pGameManager->GetProcManager(), m_pAppParam, pGameManager->GetAsyncFileManager() , "SuperTraining.cro" );
    //  上画面のステータス表示に必要
    {
      gfl2::ro::RoManager*  pRoMgr               = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      gfl2::ro::Module* pRaderChartModule   = pRoMgr->LoadModule(pGameManager->GetAsyncFileManager() , "RaderChart.cro");
      gfl2::ro::Module* pStatusUpperModule  = pRoMgr->LoadModule(pGameManager->GetAsyncFileManager() , "StatusUpper.cro");

      pRoMgr->StartModule(pRaderChartModule, true);  // @fix GFNMCat[2297] すごい特訓：CTR版でレーダーチャートが出ない
      pRoMgr->StartModule(pStatusUpperModule, true);

      m_pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
      m_pProc->SetChildModulePtr( 0 , pRaderChartModule );
      m_pProc->SetChildModulePtr( 1 , pStatusUpperModule );
    }
  }


  void SuperTrainingEvent::EndFunc( GameSys::GameManager* pGameManager )
  {
    m_pAppParam = NULL;
  }


  GameSys::GMEVENT_RESULT SuperTrainingEvent::MainFunc( GameSys::GameManager* pGameManager )
  {
    //  procが死んでいるかチェック
    if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
    {
      UCHIDA_PRINT( "%s: m_pAppParam->out.result(%d)\n", __FUNCTION__, m_pAppParam->out.result );
#if PM_DEBUG
      u32 loopNum = GFL_NELEMS( m_pAppParam->out.bPowerUpArray );
      for( u32 i = 0; i < loopNum; ++i )
      {
        UCHIDA_PRINT( "%s: m_pAppParam->out.bPowerUpArray[ %d ] = (%d)\n", __FUNCTION__, i, m_pAppParam->out.bPowerUpArray[ i ] );
      }
#endif

      //  正常終了
      return GameSys::GMEVENT_RES_FINISH;
    }

    return GameSys::GMEVENT_RES_CONTINUE;
  }


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

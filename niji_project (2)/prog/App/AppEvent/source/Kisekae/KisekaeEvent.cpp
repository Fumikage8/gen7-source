// ============================================================================
/*
 * @file KisekaeEvent.cpp
 * @brief きせかえアプリのイベント
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/AppEvent/include/Kisekae/KisekaeEvent.h"
#include "App/Kisekae/include/KisekaeProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


void KisekaeEvent::StartEvent( GameSys::GameManager* pGameManager, App::Kisekae::APP_PARAM* pParam )
{
  GFL_ASSERT( pParam );

  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  KisekaeEvent* pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, KisekaeEvent>( pGameEventManager );
  pEvent->m_pAppParam = pParam;
}


KisekaeEvent::KisekaeEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent( pHeap )
{
  _Clear();
}


KisekaeEvent::~KisekaeEvent()
{
}


bool KisekaeEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void KisekaeEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  GFL_ASSERT( m_pAppParam );

#if PM_DEBUG
  if( m_pAppParam->runType == App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP )
  {
    UCHIDA_PRINT( "APP_PARAM::COLOR_SHOP::shopId %d\n", m_pAppParam->typeData.colorShop.shopId );
    UCHIDA_PRINT( "APP_PARAM::COLOR_SHOP::colorId %d\n", m_pAppParam->typeData.colorShop.colorId );
    UCHIDA_PRINT( "APP_PARAM::COLOR_SHOP::bUseKinomiDiscount %d\n", m_pAppParam->typeData.colorShop.bUseKinomiDiscount );
  }
#endif

  //  proc生成
  m_pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::Kisekae::Proc, App::Kisekae::APP_PARAM>( 
            pGameManager->GetProcManager(), m_pAppParam, pGameManager->GetAsyncFileManager() , "Kisekae.cro" );

  //  きせかえデータ参照に必要
  {
    gfl2::ro::RoManager*  pRoMgr               = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    gfl2::ro::Module*     pKisekaeDataModule   = pRoMgr->LoadModule( pGameManager->GetAsyncFileManager() , "KisekaeData.cro" );

    pRoMgr->StartModule( pKisekaeDataModule, true );
    m_pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 1 );
    m_pProc->SetChildModulePtr( 0, pKisekaeDataModule );
  }
}


void KisekaeEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT KisekaeEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  //  procが死んでいるかチェック
  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
  {
    //  正常終了
    UCHIDA_PRINT( "APP_PARAM::OUT_PUT::bKinomiChangeColor %d\n", m_pAppParam->output.bKinomiChangeColor );
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

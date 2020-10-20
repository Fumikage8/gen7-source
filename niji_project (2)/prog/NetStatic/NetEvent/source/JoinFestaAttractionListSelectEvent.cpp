// ============================================================================
/*
 * @file JoinFestaAttractionListSelectEvent.cpp
 * @brief アトラクション開催参加アプリのイベント
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaAttractionListSelectEvent.h"
#include "NetApp/JoinFestaAttractionListSelect/include/JoinFestaAttractionListSelectProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

//　ホームスリープ制御
#include "System/include/HomeSleepAndPowerSystem.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)


void JoinFestaAttractionListSelectEvent::StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaAttractionListSelect::APP_RESULT* pAppResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaAttractionListSelectEvent* pJoinFestaAttractionListSelectEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaAttractionListSelectEvent>( pGameEventManager );
  pJoinFestaAttractionListSelectEvent->SetAppResult( pAppResult );
}


JoinFestaAttractionListSelectEvent::JoinFestaAttractionListSelectEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pAppResult( NULL ),
 m_pJoinFestaAttractionListSelectProc( NULL )
{
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )//これらはWifiのときだけHomeSleep禁止
  {
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
  }
}


JoinFestaAttractionListSelectEvent::~JoinFestaAttractionListSelectEvent()
{
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
}


bool JoinFestaAttractionListSelectEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaAttractionListSelectEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaAttractionListSelectProc = GameSys::CallJoinFestaAttractionListSelectProc( &m_AppParam );
  m_pJoinFestaAttractionListSelectProc->SetAppResult( m_pAppResult );
}


void JoinFestaAttractionListSelectEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaAttractionListSelectEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaAttractionListSelectProc ) == false )
  {
//    if( IsError() )
//    {
//      result = GameSys::GMEVENT_RES_FINISH;
//    }
    result = GameSys::GMEVENT_RES_FINISH;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetEvent)

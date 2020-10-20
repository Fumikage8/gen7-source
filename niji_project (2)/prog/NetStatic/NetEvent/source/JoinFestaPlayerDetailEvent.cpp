// ============================================================================
/*
 * @file JoinFestaPlayerDetailEvent.cpp
 * @brief 詳細アプリのイベント
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaPlayerDetailEvent.h"
#include "NetApp/JoinFestaPlayerDetail/include/JoinFestaPlayerDetailProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

//　ホームスリープ制御
#include "System/include/HomeSleepAndPowerSystem.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)


void JoinFestaPlayerDetailEvent::StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaPlayerDetail::APP_PARAM& appParam )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaPlayerDetailEvent* pJoinFestaPlayerDetailEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaPlayerDetailEvent>( pGameEventManager );
  pJoinFestaPlayerDetailEvent->SetAppParam( appParam );
}


JoinFestaPlayerDetailEvent::JoinFestaPlayerDetailEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pJoinFestaPlayerDetailProc( NULL )
{
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )//これらはWifiのときだけHomeSleep禁止
  {
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
  }
}


JoinFestaPlayerDetailEvent::~JoinFestaPlayerDetailEvent()
{
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
}


bool JoinFestaPlayerDetailEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaPlayerDetailEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaPlayerDetailProc = GameSys::CallJoinFestaPlayerDetailProc( &m_AppParam );
}


void JoinFestaPlayerDetailEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaPlayerDetailEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaPlayerDetailProc ) == false )
  {
//    if( IsError() )
//    {
//      result = GameSys::GMEVENT_RES_FINISH;
//    }
    result = GameSys::GMEVENT_RES_FINISH;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetEvent)

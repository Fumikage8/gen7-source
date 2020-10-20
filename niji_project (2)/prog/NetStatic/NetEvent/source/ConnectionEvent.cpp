// ============================================================================
/*
 * @file ConnectionEvent.cpp
 * @brief インターネット接続アプリのイベント
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/ConnectionEvent.h"
#include "NetApp/Connection/include/ConnectionProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(Connection)


void ConnectionEvent::StartEvent( GameSys::GameManager* pGameManager, const NetApp::Connection::APP_PARAM& appParam )
{
  StartEvent( pGameManager, appParam, NULL );
}


void ConnectionEvent::StartEvent( GameSys::GameManager* pGameManager, const NetApp::Connection::APP_PARAM& appParam, NetApp::Connection::APP_RESULT* pAppResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  ConnectionEvent* pConnectionEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, ConnectionEvent>( pGameEventManager );
  pConnectionEvent->SetAppParam( appParam );
  pConnectionEvent->SetAppResult( pAppResult );
}


ConnectionEvent::ConnectionEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pAppResult( NULL ),
 m_pConnectionProc( NULL )
{
}


ConnectionEvent::~ConnectionEvent()
{
}


bool ConnectionEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void ConnectionEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pConnectionProc = GameSys::CallConnectionProc( &m_AppParam );
  m_pConnectionProc->SetAppResult( m_pAppResult );
}


void ConnectionEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT ConnectionEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pConnectionProc ) == false )
  {
    if( IsError() )
    {
      if( m_pAppResult )
      {
        m_pAppResult->bIsCanceled = true;
      }
    }
    result = GameSys::GMEVENT_RES_FINISH;
  }

  return result;

}


GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetEvent)

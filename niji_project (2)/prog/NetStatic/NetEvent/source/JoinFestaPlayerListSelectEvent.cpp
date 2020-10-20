// ============================================================================
/*
 * @file JoinFestaPlayerListSelectEvent.cpp
 * @brief プレイヤー単体選択アプリのイベント
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectEvent.h"
#include "NetApp/JoinFestaPlayerListSelect/include/JoinFestaPlayerListSelectProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)


void JoinFestaPlayerListSelectEvent::StartEvent( GameSys::GameManager* pGameManager, const NetApp::JoinFestaPlayerListSelect::APP_PARAM& appParam, NetApp::JoinFestaPlayerListSelect::APP_RESULT* pAppResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaPlayerListSelectEvent* pJoinFestaPlayerListSelectEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaPlayerListSelectEvent>( pGameEventManager );
  pJoinFestaPlayerListSelectEvent->SetAppParam( appParam );
  pJoinFestaPlayerListSelectEvent->SetAppResult( pAppResult );
}


JoinFestaPlayerListSelectEvent::JoinFestaPlayerListSelectEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pAppResult( NULL ),
 m_pJoinFestaPlayerListSelectProc( NULL )
{
}


JoinFestaPlayerListSelectEvent::~JoinFestaPlayerListSelectEvent()
{
}


bool JoinFestaPlayerListSelectEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaPlayerListSelectEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaPlayerListSelectProc = GameSys::CallJoinFestaPlayerListSelectProc( &m_AppParam );
  m_pJoinFestaPlayerListSelectProc->SetAppResult( m_pAppResult );

}


void JoinFestaPlayerListSelectEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaPlayerListSelectEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaPlayerListSelectProc ) == false )
  {
    result = GameSys::GMEVENT_RES_FINISH;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetEvent)

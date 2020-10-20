// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiEvent.cpp
 * @brief プレイヤー複数選択アプリのイベント
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectMultiEvent.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/include/JoinFestaPlayerListSelectMultiProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)


void JoinFestaPlayerListSelectMultiEvent::StartEvent( GameSys::GameManager* pGameManager, const NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM& appParam, NetApp::JoinFestaPlayerListSelectMulti::APP_RESULT* pAppResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaPlayerListSelectMultiEvent* pJoinFestaPlayerListSelectMultiEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaPlayerListSelectMultiEvent>( pGameEventManager );
  pJoinFestaPlayerListSelectMultiEvent->SetAppParam( appParam );
  pJoinFestaPlayerListSelectMultiEvent->SetAppResult( pAppResult );
}


JoinFestaPlayerListSelectMultiEvent::JoinFestaPlayerListSelectMultiEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pAppResult( NULL ),
 m_pJoinFestaPlayerListSelectMultiProc( NULL )
{
}


JoinFestaPlayerListSelectMultiEvent::~JoinFestaPlayerListSelectMultiEvent()
{
}


bool JoinFestaPlayerListSelectMultiEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaPlayerListSelectMultiEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaPlayerListSelectMultiProc = GameSys::CallJoinFestaPlayerListSelectMultiProc( &m_AppParam );
  m_pJoinFestaPlayerListSelectMultiProc->SetAppResult( m_pAppResult );

}


void JoinFestaPlayerListSelectMultiEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaPlayerListSelectMultiEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaPlayerListSelectMultiProc ) == false )
  {
    result = GameSys::GMEVENT_RES_FINISH;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetEvent)

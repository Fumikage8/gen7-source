// ============================================================================
/*
 * @file JoinFestaOpenListEvent.cpp
 * @brief 開催選択アプリのイベント
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaOpenListEvent.h"
#include "NetApp/JoinFestaOpenList/include/JoinFestaOpenListProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


void JoinFestaOpenListEvent::StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaOpenList::APP_RESULT* pAppResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaOpenListEvent* pJoinFestaOpenListEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaOpenListEvent>( pGameEventManager );
  pJoinFestaOpenListEvent->SetAppResult( pAppResult );
}


JoinFestaOpenListEvent::JoinFestaOpenListEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pAppResult( NULL ),
 m_pJoinFestaOpenListProc( NULL )
{
}


JoinFestaOpenListEvent::~JoinFestaOpenListEvent()
{
}


bool JoinFestaOpenListEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaOpenListEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaOpenListProc = GameSys::CallJoinFestaOpenListProc( &m_AppParam );
  m_pJoinFestaOpenListProc->SetAppResult( m_pAppResult );
}


void JoinFestaOpenListEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaOpenListEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaOpenListProc ) == false )
  {
    result = GameSys::GMEVENT_RES_FINISH;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetEvent)

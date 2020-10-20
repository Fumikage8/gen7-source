// ============================================================================
/*
 * @file JoinFestaSelectWordEvent.cpp
 * @brief 挨拶選択アプリのイベント
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaSelectWordEvent.h"
#include "NetApp/JoinFestaSelectWord/include/JoinFestaSelectWordProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)


void JoinFestaSelectWordEvent::StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM eventParam )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  NetEvent::JoinFestaSelectWord::JoinFestaSelectWordEvent* pJoinFestaSelectWordEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaSelectWordEvent>( pGameEventManager );
  pJoinFestaSelectWordEvent->SetEventParam( eventParam );
}


JoinFestaSelectWordEvent::JoinFestaSelectWordEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap ),
 m_EventParam(),
 m_AppParam(),
 m_pJoinFestaSelectWordProc( NULL )
{
}


JoinFestaSelectWordEvent::~JoinFestaSelectWordEvent()
{
}


bool JoinFestaSelectWordEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaSelectWordEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_AppParam.editMenuID = m_EventParam.editMenuID;

  m_pJoinFestaSelectWordProc = GameSys::CallJoinFestaSelectWordProc( &m_AppParam );
}


void JoinFestaSelectWordEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaSelectWordEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaSelectWordProc ) == false )
  {
//    if( IsError() )
//    {
//    }
    result = GameSys::GMEVENT_RES_FINISH;
  }
  return result;
}


GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetEvent)

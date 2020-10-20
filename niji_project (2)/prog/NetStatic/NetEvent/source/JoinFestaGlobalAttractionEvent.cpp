// ============================================================================
/*
 * @file JoinFestaGlobalAttractionEvent.cpp
 * @brief グローバルアトラクションアプリのイベント
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaGlobalAttractionEvent.h"
#include "NetApp/JoinFestaGlobalAttraction/include/JoinFestaGlobalAttractionProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)


void JoinFestaGlobalAttractionEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaGlobalAttractionEvent>( pGameEventManager );
}


JoinFestaGlobalAttractionEvent::JoinFestaGlobalAttractionEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pJoinFestaGlobalAttractionProc( NULL )
{
}


JoinFestaGlobalAttractionEvent::~JoinFestaGlobalAttractionEvent()
{
}


bool JoinFestaGlobalAttractionEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaGlobalAttractionEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaGlobalAttractionProc = GameSys::CallJoinFestaGlobalAttractionProc( &m_AppParam );
}


void JoinFestaGlobalAttractionEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaGlobalAttractionEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaGlobalAttractionProc ) == false )
  {
//    if( IsError() )
//    {
//    }
    result = GameSys::GMEVENT_RES_FINISH;
  }
  return result;
}


GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetEvent)

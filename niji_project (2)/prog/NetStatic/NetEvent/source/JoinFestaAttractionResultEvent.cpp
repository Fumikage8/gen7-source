// ============================================================================
/*
 * @file JoinFestaAttractionResultEvent.cpp
 * @brief アトラクション結果アプリのイベント
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaAttractionResultEvent.h"
#include "NetApp/JoinFestaAttractionResult/include/JoinFestaAttractionResultProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)


void JoinFestaAttractionResultEvent::StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaAttractionResult::APP_PARAM& appParam )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaAttractionResultEvent* pJoinFestaAttractionResultEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaAttractionResultEvent>( pGameEventManager );
  pJoinFestaAttractionResultEvent->SetAppParam( appParam );
}


JoinFestaAttractionResultEvent::JoinFestaAttractionResultEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap ),
 m_AppParam(),
 m_pJoinFestaAttractionResultProc( NULL )
{
}


JoinFestaAttractionResultEvent::~JoinFestaAttractionResultEvent()
{
}


bool JoinFestaAttractionResultEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaAttractionResultEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaAttractionResultProc = GameSys::CallJoinFestaAttractionResultProc( &m_AppParam );
}


void JoinFestaAttractionResultEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaAttractionResultEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaAttractionResultProc ) == false )
  {
    result = GameSys::GMEVENT_RES_FINISH;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetEvent)

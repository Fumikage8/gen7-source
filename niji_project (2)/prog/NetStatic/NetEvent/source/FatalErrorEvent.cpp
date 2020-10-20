// ============================================================================
/*
 * @file FatalErrorEvent.cpp
 * @brief フェイタルエラーアプリのイベント
 * @date 2015.12.25
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"
#include "NetApp/Connection/include/ConnectionProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(FatalError)


void FatalErrorEvent::StartEvent( GameSys::GameManager* pGameManager, const EventParam& eventParam )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  FatalErrorEvent* pFatalErrorEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, FatalErrorEvent>( pGameEventManager );
  pFatalErrorEvent->SetEventParam( eventParam );
}


FatalErrorEvent::FatalErrorEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap ),
 m_EventParam()
{
}


FatalErrorEvent::~FatalErrorEvent()
{
}


bool FatalErrorEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void FatalErrorEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  NetApp::Connection::APP_PARAM appParam;

  appParam.bootMode = NetApp::Connection::BOOT_MODE_FATAL_ERROR;

  NetApp::Connection::ConnectionProc* pConnectionProc = GameSys::CallConnectionProc( &appParam );

  pConnectionProc->SetFatalErrorEventParam( m_EventParam );
}


void FatalErrorEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT FatalErrorEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  return GameSys::GMEVENT_RES_CONTINUE;
}


GFL_NAMESPACE_END(FatalError)
GFL_NAMESPACE_END(NetEvent)

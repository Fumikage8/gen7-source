// ============================================================================
/*
 * @file RegulationSelectEvent.cpp
 * @brief レギュレーション選択イベント
 * @date 2015.06.12
 */
// ============================================================================
#include "NetStatic/NetEvent/include/RegulationSelectEvent.h"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"
#include "NetApp/RegulationSelect/source/RegulationSelectDefine.h"
#include "NetApp/RegulationSelect/include/RegulationSelectProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(RegulationSelect)


void RegulationSelectEvent::StartEvent( GameSys::GameManager* pGameManager, const NetApp::RegulationSelect::APP_PARAM& appParam )
{
  StartEvent( pGameManager, appParam, NULL );
}


void RegulationSelectEvent::StartEvent( GameSys::GameManager* pGameManager, const NetApp::RegulationSelect::APP_PARAM& appParam, NetApp::RegulationSelect::APP_RESULT* pAppResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  RegulationSelectEvent* pRegulationSelectEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, RegulationSelectEvent>( pGameEventManager );
  pRegulationSelectEvent->SetAppParam( appParam );
  pRegulationSelectEvent->SetAppResult( pAppResult );
}


RegulationSelectEvent::RegulationSelectEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pAppResult( NULL ),
 m_AppResultLocal(),
 m_pProc( NULL )
{
}


RegulationSelectEvent::~RegulationSelectEvent()
{
}


bool RegulationSelectEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void RegulationSelectEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_AppResultLocal.bIsFatalError = false;

  m_pProc = GameSys::CallRegulationSelectProc( &m_AppParam );
  m_pProc->SetAppResult( m_pAppResult );
  m_pProc->SetAppResultLocal( &m_AppResultLocal );
}


void RegulationSelectEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT RegulationSelectEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
  {
    if( m_AppResultLocal.bIsFatalError )
    {
      NetEvent::FatalError::EventParam eventParam;

      eventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED;

      NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, eventParam );
    }
    else
    {
      result = GameSys::GMEVENT_RES_FINISH;
    }
  }

  return result;
}


GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetEvent)

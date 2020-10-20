// ============================================================================
/*
 * @file GameSyncEvent.cpp
 * @brief ゲームシンクアプリのイベント
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetEvent/include/GameSyncEvent.h"
#include "NetApp/GameSync/include/GameSyncProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(GameSync)


static const int SEQUENCE_NO_GAME_SYNC_START             = 0;
static const int SEQUENCE_NO_GAME_SYNC_EXECUTE           = 1;

void GameSyncEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, GameSyncEvent>( pGameEventManager );
}


GameSyncEvent::GameSyncEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pGameSyncProc( NULL )
{
}


GameSyncEvent::~GameSyncEvent()
{
}


bool GameSyncEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void GameSyncEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  SetSeqNo( SEQUENCE_NO_GAME_SYNC_START );
}


void GameSyncEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT GameSyncEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  switch( GetSeqNo() )
  {
    case SEQUENCE_NO_GAME_SYNC_START:
    {
      m_pGameSyncProc = GameSys::CallGameSyncProc( &m_AppParam );

      SetSeqNo( SEQUENCE_NO_GAME_SYNC_EXECUTE );
    }
    break;

    case SEQUENCE_NO_GAME_SYNC_EXECUTE:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pGameSyncProc ) == false )
      {
        result = GameSys::GMEVENT_RES_FINISH;
      }
    }
    break;
  }

  return result;
}


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetEvent)

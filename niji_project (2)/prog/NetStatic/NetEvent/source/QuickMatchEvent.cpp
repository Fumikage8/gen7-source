// ============================================================================
/*
 * @file QuickMatchEvent.cpp
 * @brief クイックマッチアプリのイベント
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetStatic/NetEvent/include/QuickMatchEvent.h"
#include "NetApp/QuickMatch/include/QuickMatchProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/Mystatus.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

// エラーマネージャー
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include "NetStatic/NetEvent/include/P2PTradeEvent.h"

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif

// エラーマネージャー
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// FatalErrorEvent
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(QuickMatch)


void QuickMatchEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, QuickMatchEvent>( pGameEventManager );
}

void QuickMatchEvent::StartEventForBattleTree( GameSys::GameManager* pGameManager , EVENT_PARAM* pEventParam )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, QuickMatchEvent>( pGameEventManager )->SetEventParam( pEventParam );
}


QuickMatchEvent::QuickMatchEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_pAppParam( NULL ),
 m_eventSeq(CALL_NONE),
 m_pEventParam(NULL)
 //m_NBREventParam(),
{
  ::std::memset( &m_NBREventParam , 0 , sizeof(m_NBREventParam) );
}


QuickMatchEvent::~QuickMatchEvent()
{
}


bool QuickMatchEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void QuickMatchEvent::InitFunc( GameSys::GameManager* pGameManager )
{
	gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pAppParam = GFL_NEW( pDevHeap ) NetApp::QuickMatch::APP_PARAM;

  // @note : 最初にdllを読み込まないとdll内のクラスをnew出来ない。要注意
  if( IsError() == false )
  {
    if( m_pEventParam != NULL )
    {//バトルツリーモード
      m_pAppParam->m_isBattleTreeMode = true;
      pGameManager->GetGameData()->GetPlayerStatus()->SetQuickMatchType( MATCH_TYPE_TREE_NORMAL );
    }
    
    GameSys::CallQuickMatchProc( m_pAppParam );
  }
  else
  {
    m_pAppParam->m_isEnd = true;
  }
}


void QuickMatchEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  // エラー処理対応
  // memo:このEventは下画面「通信であそぶ」から起動される為、親イベントになる。
  //      ローカルのP2Pしかしない為、エラー対処としてはP2P切断のみでOK。
  //      （P2P強制切断はP2pConnectionManager内部で処理している。）

  GFL_SAFE_DELETE( m_pAppParam );
}


GameSys::GMEVENT_RESULT QuickMatchEvent::MainFunc( GameSys::GameManager* pGameManager )
{
#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
#endif

  if( m_pAppParam->m_isEnd )
  {
    if( m_pEventParam )
    {//バトルツリーモード
      m_pEventParam->isConnected = m_pAppParam->m_isBattleTreeConnected;
      return GameSys::GMEVENT_RES_FINISH;
    }

    if( m_eventSeq == CALL_NONE )
    {
      if( m_pAppParam->m_result == NetApp::QuickMatch::APP_RESULT_BATTLE )
      {
        m_eventSeq = CALL_NBR;

        m_NBREventParam.battleRule = m_pAppParam->m_rule;
#if defined(GF_PLATFORM_CTR)
        if( m_pAppParam->m_isHost )
        {
          m_NBREventParam.hostNetID = pP2pConnectionManager->GetNetID();
        }
        else
        {
          m_NBREventParam.hostNetID = 1 - pP2pConnectionManager->GetNetID();
        }
#endif

        GFL_PRINT("QuickMatchEvent::MainFunc : Next NBR[%d][%d]\n",m_NBREventParam.battleRule,m_NBREventParam.hostNetID);
      }
      else if( m_pAppParam->m_result == NetApp::QuickMatch::APP_RESULT_TRADE )
      {
        m_eventSeq = CALL_TRADE;
      }
      else
      {
        m_eventSeq = RETURN_FIELD;
      }
    }


    NetLib::Error::NijiNetworkErrorManager* pErrorManager = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager);

    if( pErrorManager->IsFatalError() )
    {
      // FatalError呼び出し
      NetEvent::FatalError::EventParam eventParam;
      eventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_NET_ERROR;
      NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, eventParam );

      return GameSys::GMEVENT_RES_CONTINUE;
    }
    else if( IsError() )
    {
      m_eventSeq = RETURN_FIELD;
    }

    switch( m_eventSeq )
    {
    case CALL_NONE:
      GFL_ASSERT( 0 );//@fix
      m_eventSeq = RETURN_FIELD;
    case CALL_NBR:
      NetEvent::NBR::NBREvent::StartEvent( pGameManager , &m_NBREventParam );
      m_eventSeq = RETURN_FIELD;
    break;
    case CALL_TRADE:
      NetEvent::P2PTrade::P2PTradeEvent::StartEvent( pGameManager );
      m_eventSeq = RETURN_FIELD;
    break;
    case RETURN_FIELD:
#if defined(GF_PLATFORM_CTR)
      pP2pConnectionManager->DisconnectStart();
      if( pP2pConnectionManager->IsDisconnectSuccess() == false )
      {
        break;
      }
      pP2pConnectionManager->Terminate();
      NetLib::NijiNetworkSystem::TerminateNetwork();
      // 通信エラークリア
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->Clear();
#endif
      return GameSys::GMEVENT_RES_FINISH;
    }

    return GameSys::GMEVENT_RES_CONTINUE;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}


void QuickMatchEvent::SetEventParam( EVENT_PARAM* pEventParam )
{
  m_pEventParam = pEventParam;
}

GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetEvent)

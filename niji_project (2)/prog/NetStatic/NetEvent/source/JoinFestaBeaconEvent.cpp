// ============================================================================
/*
 * @file JoinFestaBeaconEvent.cpp
 * @brief ビーコンアプリのイベント
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaBeaconEvent.h"

#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "NetStatic/NetEvent/include/P2PTradeEvent.h"

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
// NetGame
#include <gflnet2/source/p2p/gflnet2_NetGameBase.h>
#endif

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)

NetApp::JoinFestaBeacon::APP_PARAM*                       JoinFestaBeaconEvent::m_pAppParam = NULL;

void JoinFestaBeaconEvent::StartEvent( GameSys::GameManager* pGameManager , NetApp::JoinFestaBeacon::APP_PARAM* param )
{
  m_pAppParam = param;
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaBeaconEvent>( pGameEventManager );
}


JoinFestaBeaconEvent::JoinFestaBeaconEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_proc( NULL ),
 m_eventSeq(0)
{
  ::std::memset( &m_NBREventParam , 0 , sizeof(m_NBREventParam) );
}


JoinFestaBeaconEvent::~JoinFestaBeaconEvent()
{
}


bool JoinFestaBeaconEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaBeaconEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  // @note : 最初にdllを読み込まないとdll内のクラスをnew出来ない。要注意
  m_pAppParam->isConnectSuccess = false;//初期化
  m_proc = GameSys::CallJoinFestaBeaconProc( m_pAppParam );
}


void JoinFestaBeaconEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaBeaconEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  enum
  {
    EVENT_SEQ_MATCHING = 0,
    EVENT_SEQ_TRADE,
    EVENT_SEQ_NBR,
    EVENT_SEQ_DISCONNECT
  };

#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
#endif

  switch( m_eventSeq )
  {
  case EVENT_SEQ_MATCHING:
    if( pGameManager->GetProcManager()->IsRegisterProcess( m_proc ) == false )
    {
      if( m_pAppParam->isConnectSuccess && IsError() == false )
      {//マッチング成功
        if( m_pAppParam->type == NetApp::JoinFestaBeacon::TYPE_TRADE )
        {
#if defined(GF_PLATFORM_CTR)
          // @fix NMCat[890]:お誘いパケットのままだとSubscriptionで受け取る事があるのでパケット変える
          {
            NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
            pMyData->SetRequestPlayingP2pTrade();
            GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
          }
#endif

          NetEvent::P2PTrade::P2PTradeEvent::StartEvent( pGameManager );
          m_eventSeq = EVENT_SEQ_TRADE;//交換
        }
        else
        {
          switch( m_pAppParam->type )
          {
          case NetApp::JoinFestaBeacon::TYPE_BATTLE_S:
            m_NBREventParam.battleRule = Regulation::BATTLE_RULE_SINGLE;
          break;
          case NetApp::JoinFestaBeacon::TYPE_BATTLE_D:
            m_NBREventParam.battleRule = Regulation::BATTLE_RULE_DOUBLE;
          break;
          case NetApp::JoinFestaBeacon::TYPE_BATTLE_M:
            m_NBREventParam.battleRule = Regulation::BATTLE_RULE_MULTI;
          break;
          case NetApp::JoinFestaBeacon::TYPE_BATTLE_R:
            m_NBREventParam.battleRule = Regulation::BATTLE_RULE_ROYAL;
          break;
          }

#if defined(GF_PLATFORM_CTR)
          if( m_pAppParam->inviteType == NetApp::JoinFestaBeacon::TYPE_INVITE )
          {
            m_NBREventParam.hostNetID = pP2pConnectionManager->GetNetID();
          }
          else
          {
            m_NBREventParam.hostNetID = 0;
            u64 hostTID = m_pAppParam->m_pSelectedPersonalDataArray[0]->GetTransferedID();
            for( u32 index = 0 ;index < BTL_CLIENT_NUM ; ++index )
            {
              if( pP2pConnectionManager->GetMyStatus(index) )
              {
                if( pP2pConnectionManager->GetMyStatus(index)->GetTransferdID() == hostTID )
                {
                  m_NBREventParam.hostNetID = index;
                  break;
                }
              }
            }
          }


          // @fix NMCat[890]:お誘いパケットのままだとSubscriptionで受け取る事があるのでパケット変える
          {
            NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
            pMyData->SetRequestPlayingP2pBattle( m_NBREventParam.battleRule );
            GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
          }
#endif
          NetEvent::NBR::NBREvent::StartEvent( pGameManager , &m_NBREventParam );
          m_eventSeq = EVENT_SEQ_NBR;//対戦
        }
      }
      else
      {//キャンセル or error
        m_eventSeq = EVENT_SEQ_DISCONNECT;
      }
    }
  break;
  case EVENT_SEQ_TRADE:
    ResetPersonalDataStatus();
    m_eventSeq = EVENT_SEQ_DISCONNECT;
    return GameSys::GMEVENT_RES_CONTINUE;
  case EVENT_SEQ_NBR:
    ResetPersonalDataStatus();
    m_eventSeq = EVENT_SEQ_DISCONNECT;
    return GameSys::GMEVENT_RES_CONTINUE;
  case EVENT_SEQ_DISCONNECT:
    {
#if defined(GF_PLATFORM_CTR)
      pP2pConnectionManager->DisconnectStart();
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        pP2pConnectionManager->Terminate();
        if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
        {
          NetLib::NijiNetworkSystem::TerminateNetwork();
        }
        return GameSys::GMEVENT_RES_FINISH;
      }
#endif
    }
  break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//ゲームステータスを待機状態にもどす
void JoinFestaBeaconEvent::ResetPersonalDataStatus( void )
{
#if defined(GF_PLATFORM_CTR)
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPacketManager* pPacketManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager);
  pPersonalDataManager->GetMyData()->SetRequestUpadateGameStatus();
  pPacketManager->SendMyData();
#endif
}


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetEvent)

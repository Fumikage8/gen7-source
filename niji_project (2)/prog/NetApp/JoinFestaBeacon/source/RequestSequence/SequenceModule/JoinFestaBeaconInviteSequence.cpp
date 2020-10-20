// ============================================================================
/*
 * @file JoinFestaBeaconInviteSequence.cpp
 * @brief 招待を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/RequestSequence/SequenceModule/JoinFestaBeaconInviteSequence.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>
#include "Battle/Regulation/include/Regulation.h" // バトルルール
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ/デストラクタ
*/
//------------------------------------------------------------------
JoinFestaBeaconInviteSequence::JoinFestaBeaconInviteSequence()
: m_pListener( NULL )
{
  memset( &m_requestParam , 0 , sizeof(m_requestParam) );

  m_timeout.TimeOutStart( TIMEOUT_MSEC );
}
JoinFestaBeaconInviteSequence::~JoinFestaBeaconInviteSequence()
{
#ifdef GF_PLATFORM_CTR
  NetLib::NijiNetworkSystem::SetTempBeaconID( 0 );
#endif
}


//------------------------------------------------------------------
/**
* @brief  更新処理。
*
* @note   	メインループ毎にポーリングされる
*           キャンセル操作が行われた後はこのアップデートは呼ばれなくなり、
*           UpdateCancelが呼ばれるようになります。
*/
//------------------------------------------------------------------
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaBeaconInviteSequence::Update( void )
{
#ifdef GF_PLATFORM_CTR
  //LANOFFチェック
  if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() == false )
  {
    if( m_pListener )
    {
      m_pListener->OnWLANSwitchOFF();
    }
    return gflnet2::ServerClient::REQUEST_STATE_FINISH;
  }

  NetAppLib::JoinFesta::JoinFestaPacketManager* pPacketManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager);
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  bool isWifi = NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI;
  u32 connectNum = m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->connectNum;
  JoinFestaFriendKey friendKey[MAX_CLIENT_NUM];
  NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation[MAX_CLIENT_NUM];
  ::std::memset( friendKey , 0 , sizeof(friendKey) );
  ::std::memset( relation , 0 , sizeof(relation) );
   
  u32 sessionHostIndex = 0;//子のうち一番IDが大きい人がセッションホスト
  u64 maxID = 0;
  for( u32 index = 0 ; index < connectNum ; ++index )
  {
    friendKey[index] = m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->m_pSelectedPersonalDataArray[index]->GetJoinFestaFriendKey();
    relation[index] = m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->m_pSelectedPersonalDataArray[index]->GetRelation();

    if( isWifi )
    {
      if( friendKey[index].principalId > maxID )
      {//セッションホスト決定
        maxID = friendKey[index].principalId;
        sessionHostIndex = index;
      }
    }
    else
    {
      if( friendKey[index].localFriendCode > maxID )
      {//セッションホスト決定
        maxID = friendKey[index].localFriendCode;
        sessionHostIndex = index;
      }
    }
  }


  enum
  {
    MAIN_SEQ_INVITE = 0,  //招待コマンド設定
    MAIN_SEQ_INVITE_WAIT, //招待がどうなったかチェック
    MAIN_SEQ_P2P_CONNECT_WAIT,//P2P接続待ち
  };

  switch( GetSequenceNo() )
  {
  case MAIN_SEQ_INVITE:
    {      
      //セッションホストのビーコンをJFパケットマネージャで取れるようにする
      NetLib::NijiNetworkSystem::SetTempBeaconID( m_requestParam.m_pJoinFestaBeaconWork->GetP2PID() );
      if( m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->type == TYPE_TRADE )
      {//交換
        pPersonalDataManager->GetMyData()->SetTradeRequestTargetUser( friendKey[0] , relation[0] , 1 );
        pPersonalDataManager->GetMyData()->SetRequestInvitingTrade();
      }
      else
      {//対戦
        Regulation::BATTLE_RULE rule = m_requestParam.m_pJoinFestaBeaconWork->GetBattleRule();

        pPersonalDataManager->GetMyData()->SetBattleRequestTargetUser( isWifi , friendKey , relation , connectNum , true );
        pPersonalDataManager->GetMyData()->SetRequestInvitingBattle( rule , isWifi );
      }

      pPacketManager->SendMyData();

      SetSequenceNo( MAIN_SEQ_INVITE_WAIT );
    }
  break;
  case MAIN_SEQ_INVITE_WAIT:
    {
      for( u32 index = 0 ; index < connectNum ; ++index )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = pPersonalDataManager->GetPersonalData( friendKey[index] );
        if( pPersonalData == NULL )
        {//居なくなったらキャンセルされた扱いにする（相手が通信NG設定にした場合や押し出し等）
          if( m_pListener )
          {
            m_pListener->OnConnectCanceled( index );
          }

          UpdateBeaconStateExit();

          return gflnet2::ServerClient::REQUEST_STATE_FINISH;
        }

        NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND targetRequestCommand = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_JOIN;
        u32 targetTicketID = 0;
        if( pPersonalData->IsEnablePacketBattleRecieve() )
        {
          targetTicketID = pPersonalData->GetPacketBattleRecieve().ticketID;
        }

        u32 myTicketID = 0;
        if( pPersonalDataManager->GetMyData()->IsEnablePacketBattleInvite() )
        {
          myTicketID = pPersonalDataManager->GetMyData()->GetPacketBattleInvite().ticketID;
        }

        if( m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->type == TYPE_TRADE )
        {//交換だったら
          targetRequestCommand = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_JOIN;
          if( pPersonalData->IsEnablePacketTradeRecieve() )
          {
            targetTicketID = pPersonalData->GetPacketTradeRecieve().ticketID;
          }
          if( pPersonalDataManager->GetMyData()->IsEnablePacketTradeInvite() )
          {
            myTicketID = pPersonalDataManager->GetMyData()->GetPacketTradeInvite().ticketID;
          }
        }

        if( pPersonalData )
        {
          //キャンセルor通信プレイできない状態
          bool isTargetCancel = pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS &&
                                pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_SHOP &&
                                pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_PERSONAL &&
                                pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CANCEL &&      //キャンセルした後はずっとキャンセル状態なので招待可能とする
                                pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CANCEL &&     //キャンセルした後はずっとキャンセル状態なので招待可能とする
                                pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN &&     //キャンセルした後はずっとキャンセル状態なので招待可能とする
                                pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN &&    //キャンセルした後はずっとキャンセル状態なので招待可能とする
                                pPersonalData->GetRequestCommand() != targetRequestCommand;

          //ほかのプレイヤーの招待に応じている
          bool isTargetOtherPlayerJoined = pPersonalData->GetRequestCommand() == targetRequestCommand && targetTicketID != myTicketID;

          //手動キャンセル
          bool isManualCancel = ( pPersonalData->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN ||
                                pPersonalData->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN ) &&
                                targetTicketID == myTicketID;

          if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
          {//Wifiだったら手動しか見ない
            isTargetCancel = false;
            isTargetOtherPlayerJoined = false;
          }

          if( isTargetCancel || isTargetOtherPlayerJoined || isManualCancel )
          {//キャンセルされた扱い
            if( m_pListener )
            {
              m_pListener->OnConnectCanceled( index );
            }

            UpdateBeaconStateExit();

            return gflnet2::ServerClient::REQUEST_STATE_FINISH;
          }
          else
          {//待ち継続
            if( pPersonalData->GetRequestCommand() != targetRequestCommand )
            {//まだ準備完了じゃない
              //タイムアウトチェック
              if( m_timeout.IsTimeOut() )
              {
                if( m_pListener )
                {//キャンセルされた扱いにする
                  m_pListener->OnConnectCanceled( index );
                }

                UpdateBeaconStateExit();

                return gflnet2::ServerClient::REQUEST_STATE_FINISH;
              }
            }


            if( pPersonalData->GetRequestCommand() == targetRequestCommand &&
                index == sessionHostIndex &&
                targetTicketID == myTicketID )
            {//セッションホストが開始した
              NetLib::P2P::ENUM_NIJI_P2P_ID p2pID = m_requestParam.m_pJoinFestaBeaconWork->GetP2PID();
              //接続開始
              if( isWifi )
              {
                u64 hostPID = friendKey[sessionHostIndex].principalId;
                gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
                gflnet2::nex::NexMatchMakingRule rule1( 0, hostPID, hostPID, hostPID );
                list.push_back( &rule1 );
                pP2pConnectionManager->Initialize( p2pID );
                GFL_PRINT("JoinFestaBeaconInviteSequence : matchingID[%llu]\n",hostPID);

                //@fix #4127   インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】
                if( m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->type == TYPE_BATTLE_R )
                {
                  pP2pConnectionManager->SetupMigrationCallBackEnable();
                }

                pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::CHILD , &list );
              }
              else
              {
                NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
                pP2pConnectionManager->Initialize( p2pID );
                pP2pConnectionManager->ConnectStart( gflnet2::p2p::CHILD , friendKey[sessionHostIndex].localFriendCode );
              }

              SetSequenceNo( MAIN_SEQ_P2P_CONNECT_WAIT );
              break;
            }
          }
        }
      }

    }
  break;
  case MAIN_SEQ_P2P_CONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsConnectSuccess() )
      {
        if( m_pListener )
        {
          m_pListener->OnConnectSuccess();
        }

        return gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }
    }
  break;
  }

  return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

#else
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
#endif
}

//------------------------------------------------------------------
/**
* @brief  キャンセル処理の更新
*
* @note   	
*			      リクエストをキャンセルするとUpdateではなくこちらが呼ばれるようになります。
*           キャンセル処理が完了したらREQUEST_STATE_FINISHを返すとリクエストは終了します。
*           キャンセルが行われなかった場合、この関数は呼ばれません。
*/
//------------------------------------------------------------------
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaBeaconInviteSequence::UpdateCancel( void )
{
#ifdef GF_PLATFORM_CTR
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  pP2pConnectionManager->DisconnectStart( true );


  if( m_pListener )
  {
    m_pListener->OnCancelSuccess();
  }

  UpdateBeaconStateExit();

#endif

  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void JoinFestaBeaconInviteSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void JoinFestaBeaconInviteSequence::SetListener( JoinFestaBeaconResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void JoinFestaBeaconInviteSequence::SetRequestParam( JoinFestaBeaconRequestClient::REQUEST_PARAM& pParam )
{
  m_requestParam = pParam;
}


//------------------------------------------------------------------
/**
* @brief ビーコンを終了ステートにする
*/
//------------------------------------------------------------------
void JoinFestaBeaconInviteSequence::UpdateBeaconStateExit( void )
{
#ifdef GF_PLATFORM_CTR
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPacketManager* pPacketManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager);
  if( m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->type == TYPE_TRADE )
  {//交換
    pPersonalDataManager->GetMyData()->SetRequestCancelingTrade();
  }
  else
  {//対戦
    pPersonalDataManager->GetMyData()->SetRequestCancelingBattle();
  }

  pPacketManager->SendMyData();
#endif
}


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

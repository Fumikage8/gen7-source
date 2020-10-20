// ============================================================================
/*
 * @file JoinFestaBeaconJoinSequence.cpp
 * @brief 招待への参加を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/RequestSequence/SequenceModule/JoinFestaBeaconJoinSequence.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <util/include/gfl2_std_string.h>
#include "Battle/Regulation/include/Regulation.h" // バトルルール


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ/デストラクタ
*/
//------------------------------------------------------------------
JoinFestaBeaconJoinSequence::JoinFestaBeaconJoinSequence()
: m_pListener( NULL )
{
  memset( &m_requestParam , 0 , sizeof(m_requestParam) );
#ifdef GF_PLATFORM_CTR

  m_pPacketManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager);
  m_pPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  m_pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  m_ownerTicketID = 0;

  m_isWifi = NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI;

  ::std::memset( m_friendID , 0 , sizeof(m_friendID) );
  ::std::memset( m_friendKey , 0 , sizeof(m_friendKey) );

  m_sessionHostIndex = 0;
  m_pOwnerPersonalData = NULL;
  m_isTrade = false;
  m_connectNum = 0;
  m_isSessionMyHost = false;

#endif
}
JoinFestaBeaconJoinSequence::~JoinFestaBeaconJoinSequence()
{
#ifdef GF_PLATFORM_CTR
  // 成功失敗に関わらず、招待者の保存データはもう必要ない
  m_pPersonalDataManager->ResetInviteFriendKey();

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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaBeaconJoinSequence::Update( void )
{
#ifdef GF_PLATFORM_CTR
  if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() == false )
  {
    if( m_pListener )
    {
      m_pListener->OnWLANSwitchOFF();
    }
    return gflnet2::ServerClient::REQUEST_STATE_FINISH;
  }

  enum
  {
    MAIN_SEQ_SETUP = 0,
    MAIN_SEQ_JOIN,  //参加コマンド設定
    MAIN_SEQ_JOIN_WAIT, //参加がどうなったかチェック
    MAIN_SEQ_CREATE_SESSION_WAIT,//セッション作成待ち
    MAIN_SEQ_P2P_CONNECT_WAIT//P2P接続待ち
  };

  switch( GetSequenceNo() )
  {
  case MAIN_SEQ_SETUP://初期化
    {
      Setup();
    }
  case MAIN_SEQ_JOIN://参加コマンド設定
    {
      if( m_isTrade )
      {//交換
        if( m_pOwnerPersonalData->IsEnablePacketTradeInvite() )
        {
          m_ownerTicketID = m_pOwnerPersonalData->GetPacketTradeInvite().ticketID;
          m_pPersonalDataManager->GetMyData()->SetRequestRecieveTrade( m_pOwnerPersonalData->GetJoinFestaFriendKey() , m_ownerTicketID , true );
        }
      }
      else
      {//対戦
        if( m_pOwnerPersonalData->IsEnablePacketBattleInvite() )
        {
          m_ownerTicketID = m_pOwnerPersonalData->GetPacketBattleInvite().ticketID;
          m_pPersonalDataManager->GetMyData()->SetRequestRecieveBattle( m_pOwnerPersonalData->GetJoinFestaFriendKey() , m_ownerTicketID , true , true, m_pOwnerPersonalData->GetPacketBattleInvite().battle_rule );
        }
      }

      //ブラックリストチェック
      if( CheckBlackList() == true )
      {//キャンセルされた扱いにする
        if( m_pListener )
        {
          m_pListener->OnConnectCanceled(0);
        }

        //キャンセルパケットを飛ばす
        if( m_isTrade )
        {//交換
          m_pPersonalDataManager->GetMyData()->SetRequestRecieveTrade( m_pOwnerPersonalData->GetJoinFestaFriendKey() , m_ownerTicketID , false );
        }
        else
        {//対戦
          m_pPersonalDataManager->GetMyData()->SetRequestRecieveBattle( m_pOwnerPersonalData->GetJoinFestaFriendKey() , m_ownerTicketID , false , true, m_pOwnerPersonalData->GetPacketBattleInvite().battle_rule );
        }

        m_pPacketManager->SendMyData();

        return gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }

      if( m_isSessionMyHost == false )
      {
        m_pPacketManager->SendMyData();//ホストはこのタイミングでビーコン更新しない
      }

      SetSequenceNo( MAIN_SEQ_JOIN_WAIT );
    }
  break;
  case MAIN_SEQ_JOIN_WAIT://参加がどうなったかチェック
    {
      //招待者のキャンセルチェック
      if( CheckOwnerCanceled() == true )
      {//招待者がキャンセルした
        if( m_pListener )
        {
          m_pListener->OnConnectCanceled(0);
        }

        return gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }

      //全メンバーチェック
      bool isAllReady = true;
      for( u32 index = 0 ; index < m_connectNum ; ++index )
      {//参加者の状態をそれぞれ見る
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = m_pPersonalDataManager->GetPersonalData( m_friendKey[index] );
        if( pPersonalData == NULL )
          continue;

        NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND targetRequestCommand = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_JOIN;

        u32 targetTicketID = 0;
        if( pPersonalData->IsEnablePacketBattleRecieve() )
        {
          targetTicketID = pPersonalData->GetPacketBattleRecieve().ticketID;
        }
        if( m_isTrade )
        {//交換だったら
          targetRequestCommand = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_JOIN;
          if( pPersonalData->IsEnablePacketTradeRecieve() )
          {
            targetTicketID = pPersonalData->GetPacketTradeRecieve().ticketID;
          }
        }

        //キャンセルor通信プレイできない状態
        bool isTargetCancel = pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS &&
                              pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_SHOP &&
                              pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_PERSONAL &&
                              pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CANCEL &&           //キャンセルした後はずっとキャンセル状態なので招待可能とする
                              pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CANCEL &&          //キャンセルした後はずっとキャンセル状態なので招待可能とする
                              pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN &&          //キャンセルした後はずっとキャンセル状態なので招待可能とする
                              pPersonalData->GetRequestCommand() != NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN &&         //キャンセルした後はずっとキャンセル状態なので招待可能とする
                              pPersonalData->GetRequestCommand() != targetRequestCommand;
        //ほかのプレイヤーの招待に応じている
        bool isTargetOtherPlayerJoined = pPersonalData->GetRequestCommand() == targetRequestCommand && targetTicketID != m_ownerTicketID;

        //手動キャンセル
        bool isManualCancel = ( pPersonalData->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN ||
                              pPersonalData->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN ) &&
                              targetTicketID == m_ownerTicketID;

        if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
        {//Wifiだったら手動しか見ない
          isTargetCancel = false;
          isTargetOtherPlayerJoined = false;
        }

        //あるクライアントがキャンセルしたか
        if( isTargetCancel || isTargetOtherPlayerJoined || isManualCancel )
        {//キャンセルされた扱い
          if( m_pListener )
          {
            m_pListener->OnConnectCanceled(0);
          }

          return gflnet2::ServerClient::REQUEST_STATE_FINISH;
        }

        //待ち継続
        bool isBusy = pPersonalData->GetRequestCommand() != targetRequestCommand || targetTicketID != m_ownerTicketID;
        if( m_isSessionMyHost )
        {//ホスト側
          if( isBusy )
          {//まだ準備中のメンバーがいる
            isAllReady = false;
          }
        }
        else
        {//クライアント側
          if( index == m_sessionHostIndex && isBusy )
          {//セッションホストが開始してない
            isAllReady = false;
          }
        }
      }

      //全員の準備が完了
      if( isAllReady == true )
      {
        StartP2PConnect();

        if( m_isSessionMyHost && NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
        {//Wifiホストだったらセッション作成待ち
          SetSequenceNo( MAIN_SEQ_CREATE_SESSION_WAIT );
        }
        else
        {//子だったら接続待ち
          SetSequenceNo( MAIN_SEQ_P2P_CONNECT_WAIT );
        }
      }
    }
  break;
  case MAIN_SEQ_CREATE_SESSION_WAIT:
    {
      if( m_pP2pConnectionManager->IsMatchmakingCreateSuccess() )
      {//接続待ちへ
        m_pPacketManager->SendMyData();//ホストはこのタイミングでビーコン更新
        SetSequenceNo( MAIN_SEQ_P2P_CONNECT_WAIT );
      }
    }
  break;
  case MAIN_SEQ_P2P_CONNECT_WAIT://P2P接続待ち
    {
      if( m_pP2pConnectionManager->IsConnectSuccess() )
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
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE JoinFestaBeaconJoinSequence::UpdateCancel( void )
{
#ifdef GF_PLATFORM_CTR
  if( m_pP2pConnectionManager )
  {
    m_pP2pConnectionManager->DisconnectStart( true );
  }


  if( m_pListener )
  {
    m_pListener->OnCancelSuccess();
  }

  if( m_isTrade )
  {//交換
    m_pPersonalDataManager->GetMyData()->SetRequestRecieveTrade( m_pOwnerPersonalData->GetJoinFestaFriendKey() , m_ownerTicketID , false );
  }
  else
  {//対戦
    m_pPersonalDataManager->GetMyData()->SetRequestRecieveBattle( m_pOwnerPersonalData->GetJoinFestaFriendKey() , m_ownerTicketID , false , true, m_pOwnerPersonalData->GetPacketBattleInvite().battle_rule );
  }

  m_pPacketManager->SendMyData();

#endif

  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void JoinFestaBeaconJoinSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void JoinFestaBeaconJoinSequence::SetListener( JoinFestaBeaconResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void JoinFestaBeaconJoinSequence::SetRequestParam( JoinFestaBeaconRequestClient::REQUEST_PARAM& pParam )
{
  m_requestParam = pParam;
}

//招待者がキャンセルしたか
bool JoinFestaBeaconJoinSequence::CheckOwnerCanceled( void )
{
#ifdef GF_PLATFORM_CTR
  NetAppLib::JoinFesta::JoinFestaPersonalData* pNowOwnerPersonalData = m_pPersonalDataManager->GetPersonalData( m_pOwnerPersonalData->GetJoinFestaFriendKey() );
  if( pNowOwnerPersonalData == NULL )
  {//キャンセルした扱い
    return true;
  }

  NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND targetHostRequestCommand = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE;
  if( m_isTrade )
    targetHostRequestCommand = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE;
      
  if( pNowOwnerPersonalData )
  {
    if( pNowOwnerPersonalData->GetRequestCommand() != targetHostRequestCommand )
    {//招待者がキャンセルした
      return true;
    }
    // チケットIDが違っていた
    if( m_pPersonalDataManager->GetInviteTicketId() != m_ownerTicketID )
    {
      return true;
    }
  }
#endif
  return false;
}


//接続開始
void JoinFestaBeaconJoinSequence::StartP2PConnect( void )
{
#ifdef GF_PLATFORM_CTR
  NetLib::P2P::ENUM_NIJI_P2P_ID p2pID = m_requestParam.m_pJoinFestaBeaconWork->GetP2PID();
  gflnet2::p2p::CONNECT_TYPE connectType = gflnet2::p2p::CHILD;
  if( m_isSessionMyHost )
  {
    connectType = gflnet2::p2p::PARENT;
  }

  u64 hostID = m_friendID[m_sessionHostIndex];//WifiだとPID
  if( m_isWifi )
  {
    gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
    gflnet2::nex::NexMatchMakingRule rule1( 0, hostID, hostID, hostID );
    list.push_back( &rule1 );
    m_pP2pConnectionManager->Initialize( p2pID );
    GFL_PRINT("JoinFestaBeaconJoinSequence : matchingID[%llu]\n",hostID);

    //@fix #4127   インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】
    if( m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->type == TYPE_BATTLE_R )
    {
      m_pP2pConnectionManager->SetupMigrationCallBackEnable();
    }

    m_pP2pConnectionManager->ConnectStartWifi( connectType , &list );
  }
  else
  {
    NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
    m_pP2pConnectionManager->Initialize( p2pID );
    //ローカル接続用のパケット設定
    if( m_isSessionMyHost )
    {
      NetAppLib::JoinFesta::JoinFestaPacketData packet;
      ::std::memset( &packet , 0 , sizeof(packet) );
      u32 size = m_pPersonalDataManager->GetMyData()->GetPacketDataSize();
      m_pPersonalDataManager->GetMyData()->OutputPacketData( &packet );

      m_pP2pConnectionManager->SetLocalP2PMasterBeacon( &packet , size );
    }

    m_pP2pConnectionManager->ConnectStart( connectType , hostID );
  }
#endif
}

//初期化
void JoinFestaBeaconJoinSequence::Setup( void )
{
#ifdef GF_PLATFORM_CTR
  m_pOwnerPersonalData = m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->m_pSelectedPersonalDataArray[0];
  m_isTrade = m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->type == TYPE_TRADE;
  m_connectNum = m_requestParam.m_pJoinFestaBeaconWork->GetAppParam()->connectNum;

  u64 maxID = 0;
  for( u32 index = 0 ; index < m_connectNum ; ++index )
  {
    //マッチング用ID作成
    if( m_isTrade )
    {
      if( m_pOwnerPersonalData->IsEnablePacketTradeInvite() )
      {
        if( m_isWifi )
          m_friendID[index] = m_pOwnerPersonalData->GetPacketTradeInvite().guestPrincipalId;
        else
          m_friendID[index] = m_pOwnerPersonalData->GetPacketTradeInvite().guestLocalFriendCode;
      }
    }
    else
    {
      if( m_pOwnerPersonalData->IsEnablePacketBattleInvite() )
      {
        m_friendID[index] = m_pOwnerPersonalData->GetPacketBattleInvite().guestFriendId[index];
      }
    }

    //フレンドキー作成
    if( m_isWifi )
      m_friendKey[index].initialize( static_cast<nn::friends::PrincipalId>( m_friendID[index] ) );
    else
      m_friendKey[index].initialize( static_cast<nn::friends::LocalFriendCode>( m_friendID[index] ) );
    

    if( m_friendID[index] > maxID )
    {//セッションホスト決定
      maxID = m_friendID[index];
      m_sessionHostIndex = index;
    }
  }

  m_isSessionMyHost = m_pPersonalDataManager->GetMyData()->IsSameFriendKey( m_friendKey[m_sessionHostIndex] );

  //セッションホストのビーコンをJFパケットマネージャで取れるようにする
  NetLib::NijiNetworkSystem::SetTempBeaconID( m_requestParam.m_pJoinFestaBeaconWork->GetP2PID() );

#endif
}

//ブラックリストチェック
bool JoinFestaBeaconJoinSequence::CheckBlackList( void )
{
  bool ret = false;
#ifdef GF_PLATFORM_CTR

  for( u32 index = 0 ; index < m_connectNum ; ++index )
  {
    GFL_PRINT("CheckBlackList : m_friendKey[%u][%llu]",m_friendKey[index].principalId,m_friendKey[index].localFriendCode);
    if( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetJoinFestaData()->IsBlackList( m_friendKey[index] ) )
    {
      ret = true;
    }
  }

#endif
  return ret;
}

GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

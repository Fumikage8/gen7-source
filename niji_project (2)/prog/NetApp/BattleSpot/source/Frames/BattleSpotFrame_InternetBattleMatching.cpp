//==============================================================================
/**
 * @file   BattleSpotFrame_InternetBattleMatching.cpp
 * @date   2015/10/02 Fri. 13:32:57
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../System/BattleSpotWork.h"  

#include  "../BattleSpotResource.h"
#include  "../Views/BattleSpotViewContainer.h"

#include  "./BattleSpotFrame_InternetBattleMatching.h"
#include  "GameSys/include/GameData.h"

#include <System/include/SystemEventManager.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"

#include "NetStatic/NetEvent/include/BattleSpotEvent.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_GetPlayStatus,
    STATE_UpdateRank,
    STATE_Matching,

    STATE_Ready,
    STATE_Timeout,
    STATE_MessageWait,
    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_InternetBattleMatching
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_InternetBattleMatching::Frame_InternetBattleMatching(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  , m_matchingState(0)
  , m_pP2pPacketTransfer(NULL)
  , m_seq(0)
  , m_CountDownTimer()
  , m_matchingCount(0)
  , m_isRetryExists(false)
{
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
  ::std::memset( &m_syncData , 0 , sizeof(m_syncData) );
#if defined(GF_PLATFORM_CTR)
  // ランキングクライアント作成
  m_rankingClient = GFL_NEW( m_pWork->GetDeviceHeap() ) gflnet2::nex::NexRankingClient();
  m_rankingClient->RegistRankingClientListener( this );

  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::MyStatus* myst = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  m_pglConnector = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Pgl::PGLConnector();
  m_pglConnector->Initialize( m_pWork->GetDeviceHeap(), myst->GetNexUniqueID() , nexMgr->GetAuthenticationToken(), static_cast<nn::cfg::CTR::CfgRegionCode>( myst->GetRegionCode() ) );
  m_pglConnector->SetPGLConnectorListener( this );
#endif
}


//------------------------------------------------------------------
/**
  *  @func     Frame_InternetBattleMatching
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_InternetBattleMatching::~Frame_InternetBattleMatching()
{
#if defined(GF_PLATFORM_CTR)
  if( m_rankingClient )
  {
    m_rankingClient->UnregistRankingClientListener( this );
    GFL_SAFE_DELETE( m_rankingClient );
  }

  if( m_pglConnector )
  {
    m_pglConnector->RemovePGLConnectorListener();
    m_pglConnector->Finalize();
    GFL_SAFE_DELETE( m_pglConnector );
  }

#endif

  GFL_SAFE_DELETE( m_pP2pPacketTransfer );

  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);

  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->P2pDisconnectSpanOff();
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_InternetBattleMatching::startup()
{
  m_pViewContainer->SetAllInputEnabled( true );

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Card;

    m_pViewContainer->m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_InternetMatch_Matching);

    gfl2::str::StrBuf* name1 = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetTextParam( Regulation::CUPNAME1 );
    gfl2::str::StrBuf* name2 = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetTextParam( Regulation::CUPNAME2 );
    m_pUpperView->SetupCupTitle( name1->GetPtr() , name2->GetPtr() );

    GFL_SAFE_DELETE( name1 );
    GFL_SAFE_DELETE( name2 );
  }

  InitializeRanking();

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_InternetBattleMatching::setListener()
{
//  m_pUpperView->SetEventHandler(this);
  m_pLowerView->SetEventHandler(this);
  m_pViewContainer->m_pLowerView_Selection->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_InternetBattleMatching::removeListener()
{
//  m_pUpperView->SetEventHandler(NULL);
  m_pLowerView->SetEventHandler(NULL);
  m_pViewContainer->m_pLowerView_Selection->SetEventHandler(NULL);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_InternetBattleMatching::updateSequence()
{
  switch(m_State)
  {
  case  STATE_Entry:
    {
      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      u32 pid = m_pWork->m_pBattleSpotSave->GetEntryCommunityPid ( Savedata::BattleSpotData::ENTRY_COMMUNITY_NET );
      if( pid == game_data->GetPlayerStatus()->GetPrincipalId() )
      {//PID一致

        NetAppLib::GameSync::GETPLAYSTATUS_PARAM param;
        std::memset( &param , 0 , sizeof(param) );
        param.pWork = m_pWork;
        param.mode = NetAppLib::GameSync::GETPLAYSTATUS_MODE_CUP;

        NetAppLib::GameSync::GameSyncRequestFacade::AddConfirmPlayStatusRequest( this, &param );

        m_State = STATE_GetPlayStatus;
      }
      else
      {//不一致
        m_State = STATE_MessageWait;
        m_pViewContainer->m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Message_DifferentConsole);
      }
    }
    break;
  case  STATE_GetPlayStatus:
    break;
  case  STATE_UpdateRank:
    {
      UpdateRankData();
    }
    break;
    case  STATE_Matching:
      {
        if( UpdateMatching() )
        {//マッチング確定
          Sound::PlaySE( SEQ_SE_SYS_NJ_006 );
          m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_Matched);
          m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_InternetMatch_Matched);


          m_pUpperView->SetupOpponentText();

          m_seq = 0;
          m_State =  STATE_Ready;
        }
      }
    break;
    case STATE_Timeout:
    case STATE_MessageWait:
      break;
    case  STATE_Ready:
        if( ReadyWait() )
        {//バトルへ
          m_pWork->m_pAppParam->nextApp = NEXT_APP_BATTLE;
          m_pWork->m_pAppParam->nextFrame = FRAME_InternetAfterBattle;
          //m_pWork->m_pAppParam->m_bgm = m_pLowerView->GetSelectedBGM();
          m_pWork->DisconnectFlagOn( m_pWork->GetCommonData() );
          //対戦回数＋1
          m_pWork->m_pBattleSpotSave->SetInternetMatchCount( m_pWork->m_pBattleSpotSave->GetInternetMatchCount() + 1 );
          exitFrame(RESULT_InternetBattleMatching_Next);
        }
      break;

    case  STATE_Exit:
      exitFrame(m_Result);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    FinalizeRanking
  */
//------------------------------------------------------------------
bool  Frame_InternetBattleMatching::cleanup()
{
  bool  bAllDone = true;

  if( FinalizeRanking() == false )
  {
    return false;
  }

#if defined(GF_PLATFORM_CTR)
  if( m_pglConnector )
  {
    m_pglConnector->CancelConnecting();
    if( m_pglConnector->IsConnecting() )
    {
      bAllDone = false;
    }
    else
    {
      m_pglConnector->RemovePGLConnectorListener();
      m_pglConnector->Finalize();
      GFL_SAFE_DELETE( m_pglConnector );
    }
  }
#endif

  return  bAllDone;
}




void  Frame_InternetBattleMatching::LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode)
{
  switch( eventCode )
  {
  case LowerView_Card::IEventHandler::EVENT_Message_EOM:
      if( m_State == STATE_Timeout )
      {
        ExitSelf(RESULT_InternetBattleMatching_Timeout);
      }
   break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_InternetBattleMatching::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
  case LowerView_Selection::IEventHandler::EVENT_Message_EOM:
    {
       ExitSelf(RESULT_InternetBattleMatching_InvalidPid);
    }
  break;
  }
}



void Frame_InternetBattleMatching::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
#if defined(GF_PLATFORM_CTR)

  if( m_matchingState == DISCONNECT_WAIT )
  {//キャンセル後
    GFL_PRINT("Frame_InternetBattleMatching::OnPacketTransferSuccess : skip\n");
    return;
  }


  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();

  GFL_PRINT("Frame_InternetBattleMatching::OnPacketTransferSuccess : data count[%d]\n",receivedDataCount);

  int myID = pP2pConnectionManager->GetNetID();

  if( myID > 0 && myID < BTL_CLIENT_NUM )
  {//自分のパーティを正しい位置に移動
    m_pWork->m_pAppParam->m_pPersonalData[myID]->pPokeParty->CopyFrom( *m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
  }

  for( u32 i = 0; i < receivedDataCount; ++i )
  {
    int netID = 0;
    ::std::memset( &m_syncData , 0 , sizeof(m_syncData) );
    bool bResult = pP2pPacketTransfer->GetReceivedData( i, SEND_COMMAND, netID, &m_syncData, sizeof(m_syncData) );
    
    if( bResult )
    {
      if( netID < BTL_CLIENT_NUM )
      {
        m_pWork->m_pAppParam->m_pPersonalData[netID]->pPokeParty->DeserializeCore( &m_syncData.m_serializeBuffer );


        u32 rate = m_syncData.m_rate;

        m_pWork->SetOpponentRate( rate );

        bool ret = m_pWork->CheckPokeParty( netID , NULL , &m_syncData.regulationComp );
        if( ret == false )
        {//不正かレギュレーションエラー
          GFL_PRINT("invalid pokemon \n");
          pP2pConnectionManager->DisconnectStart(true);
          m_matchingState = DISCONNECT_WAIT;

          if( m_isRetryExists == false )
          {
            m_isRetryExists = true;
            m_matchingCount = 0;//レート幅初期化
          }
          else
          {
            m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
          }

          return;
        }
      }
    }
  }


  //相手のランキング取得

  // ランキングモードを自分のランキングデータに指定
  nn::nex::RankingConstants::RankingMode rankingMode = nn::nex::RankingConstants::RANKING_MODE_USER;

  // 順位計算パラメータの設定
  nn::nex::RankingOrderParam orderParam;
  {
    orderParam.SetOrderCalculation(nn::nex::RankingConstants::ORDER_CALCULATION_113); 
    orderParam.SetFilterGroupIndex(nn::nex::RankingConstants::FILTER_GROUP_INDEX_NONE);
    orderParam.SetTimeScope(nn::nex::RankingConstants::TIME_SCOPE_ALL);
  }
  // カテゴリID = コミュニティID
  u32 category_id = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
  u32 otherPrincipalID = pP2pConnectionManager->GetMyStatus( 1 - myID )->GetPrincipalId();
  u64 otherNUID = pP2pConnectionManager->GetMyStatus( 1 - myID )->GetNexUniqueID();

  // ランキング取得
  bool ret = m_rankingClient->GetRankingList( rankingMode, category_id, orderParam, otherNUID, otherPrincipalID );
  ICHI_PRINT("GetRankingList category %d\n", category_id );
  if( ret == false )
  {
    pP2pConnectionManager->DisconnectStart(true);
    m_matchingState = DISCONNECT_WAIT;
    if( m_isRetryExists == false )
    {
      m_isRetryExists = true;
      m_matchingCount = 0;//レート幅初期化
    }
    else
    {
      m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
    }
  }
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_InternetBattleMatching::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}



//------------------------------------------------------------------
/**
  *  @func     //マッチング更新
  *  @brief    
  */
//------------------------------------------------------------------
bool Frame_InternetBattleMatching::UpdateMatching()
{
#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  if( m_pP2pPacketTransfer )
  {
    if( pP2pConnectionManager->IsConnectSuccess() )
    {
      m_pP2pPacketTransfer->Update();
    }
  }

  switch( m_matchingState )
  {
  case MATCHING_INIT:
    {
      u32 battleCount = m_matchingCount;
      // マッチング用のレートのRange設定を求める
      u32 range = m_pWork->GetRateRange( m_pWork->GetRankData().rate, battleCount );
      GFL_PRINT("Frame_FriendBattleMatching : range = %d\n", range );

      // レートを設定
      int rate      = m_pWork->GetRankData().rate;
      int rate_min  = m_pWork->GetRankData().rate - range;
      int rate_max  = m_pWork->GetRankData().rate + range;

      // マイナスチェック
      if( rate < 0 ){ rate = 0; }
      if( rate_min < 0 ){ rate_min = 0; }
      if( rate_max < 0 ){ rate_max = 0; }

      int cut_per = m_pWork->GetDisconnectRate( &m_pWork->GetRankData() );
      int cut_per_min = cut_per - BattleSpotWork::CUT_RANGE;
      int cut_per_max = cut_per + BattleSpotWork::CUT_RANGE;

      // マイナスチェック
      if( cut_per < 0 ){ cut_per = 0; }
      if( cut_per_min < 0 ){ cut_per_min = 0; }
      if( cut_per_max < 0 ){ cut_per_max = 0; }

      GFL_PRINT("Frame_InternetBattleMatching : CutRate : %d %d %d \n", cut_per, cut_per_min, cut_per_max );

      NetLib::P2P::ENUM_NIJI_P2P_ID p2pID = NetLib::P2P::NIJI_P2P_WIFI_CUP;
      u32 category_id = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );

      // WiFiマッチング用条件
      gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
      gflnet2::nex::NexMatchMakingRule rule0( 0 , rate, rate_min , rate_max );
      gflnet2::nex::NexMatchMakingRule rule1( 1 , cut_per, cut_per_min , cut_per_max );
      gflnet2::nex::NexMatchMakingRule rule2( 2 , category_id , category_id , category_id );
      list.push_back( &rule0 );
      list.push_back( &rule1 );
      list.push_back( &rule2 );

      pP2pConnectionManager->Initialize( p2pID );
      pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::AUTO , &list );
      NetEvent::BattleSpot::BattleSpotEvent::SetSessionLeftFlag( false );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->P2pDisconnectSpanOn();
      m_CountDownTimer.TimeOutStart( MATCHING_TIMEOUT_MSEC );

      m_matchingState = MATCHING_WAIT;
    }
  break;
  case MATCHING_WAIT:
    {
      if( pP2pConnectionManager->IsConnectSuccess() )
      {
        //アイコンセットアップ開始
        m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_InternetMatch_Matched);

        GFL_SAFE_DELETE( m_pP2pPacketTransfer );
        gfl2::heap::HeapBase* pDeviceHeap = m_pWork->GetDeviceHeap()->GetLowerHandle();
        m_pP2pPacketTransfer = GFL_NEW( pDeviceHeap ) NetLib::P2P::P2pPacketTransfer( pDeviceHeap, this );
        ::std::memset( &m_syncData , 0 , sizeof(m_syncData) );
        m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->SerializeCore( &m_syncData.m_serializeBuffer );

        m_syncData.m_rate = m_pWork->GetRankData().rate;

        m_pWork->SetupRegulationComp( &m_syncData.regulationComp );

        m_pP2pPacketTransfer->SendDataCommand( &m_syncData , sizeof(m_syncData) , SEND_COMMAND );
        m_matchingState = SEND_RECV_WAIT;
      }
      else if( m_CountDownTimer.IsTimeOut() )
      {
        pP2pConnectionManager->DisconnectStart(true);
        m_matchingState = DISCONNECT_WAIT;
      }
    }
  break;
  case DISCONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        //パーティの状態を元に戻しておく
        {//0番に自分のパーティ設定
          m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->CopyFrom( *m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.pPokeParty );
        }

        GFL_SAFE_DELETE( m_pP2pPacketTransfer );
        pP2pConnectionManager->Terminate();
        m_matchingCount++;
        if( m_matchingCount < MAX_MATCHING_TIMEOUT )
        {
          m_matchingState = MATCHING_INIT;
        }
        else
        {
          m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_MatchingFailed);

          m_State = STATE_Timeout;
        }
      }
    }
    break;
  case SEND_RECV_WAIT:
    {
      if( NetEvent::BattleSpot::BattleSpotEvent::IsSessionLeftFlag() )
      {
        pP2pConnectionManager->DisconnectStart(true);
        m_matchingState = DISCONNECT_WAIT;
        break;
      }
    }
  break;
  case SEND_RECV_END:
    {
      if( NetEvent::BattleSpot::BattleSpotEvent::IsSessionLeftFlag() )
      {
        pP2pConnectionManager->DisconnectStart(true);
        m_matchingState = DISCONNECT_WAIT;
        break;
      }

      if( m_pUpperView->IsIconSetupComplete() == false )
      {//アイコン待ち
        break;
      }

      if( pP2pConnectionManager->IsTimingEnd( BattleSpotWork::CONNECT_COMPLETE_TIMING ) == false )
      {
        break;
      }

      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->P2pDisconnectSpanOff();

      m_matchingState = MATCHING_INIT;
    }
  return true;
  case MATCHING_ERROR:
  default:
    m_matchingState = MATCHING_INIT;
  return true;
  }
#endif

  return false;
}

static u32 s_wait = 0;

bool Frame_InternetBattleMatching::ReadyWait()
{
  if( s_wait++ > 30 )
  {
    s_wait = 0;
    return true;
  }

  return false;
}


void Frame_InternetBattleMatching::UpdateRankData()
{
#if defined(GF_PLATFORM_CTR)
  switch( m_seq ){
  case RANK_SEQ_RATE_DATA_GET:
    {
      // ランキングモードを自分のランキングデータに指定
      nn::nex::RankingConstants::RankingMode rankingMode = nn::nex::RankingConstants::RANKING_MODE_USER;

      // 順位計算パラメータの設定
      nn::nex::RankingOrderParam orderParam;
      {
        orderParam.SetOrderCalculation(nn::nex::RankingConstants::ORDER_CALCULATION_113); 
        orderParam.SetFilterGroupIndex(nn::nex::RankingConstants::FILTER_GROUP_INDEX_NONE);
        orderParam.SetTimeScope(nn::nex::RankingConstants::TIME_SCOPE_ALL);
      }
      // カテゴリID = コミュニティID
      u32 category_id = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );

      #if PM_DEBUG
      //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"RankingClient::GetRanking(nexUniq:%llu) \n", m_nexUniqueId );
      #endif

      // ランキング取得
      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      bool ret = m_rankingClient->GetRankingList( rankingMode, category_id, orderParam, game_data->GetPlayerStatus()->GetNexUniqueID() );
      GFL_PRINT("GetRankingList category %d\n", category_id );
      GFL_ASSERT(ret); // @check
      m_seq = RANK_SEQ_WAIT;
    }
    break;

  case RANK_SEQ_DATA_GET_NOT_FOUND:///< レートデータなし
    {
      // レートデータ初期化
      RANK_DATA rank_data;
      ::std::memset( &rank_data, 0 , sizeof(RANK_DATA));
      rank_data.rate        = BattleSpotWork::DEFAULT_RATE;
      rank_data.win         = 0;
      rank_data.lose        = 0;
      rank_data.draw        = 0;
      rank_data.disconnect  = 0;
      m_pWork->SetRankData( rank_data );

      m_seq = RANK_SEQ_COMMON_DATA_GET;
    }
    break;

  ///< レートデータ取得
  case RANK_SEQ_RATE_DATA_GET_SUCCESS:
    m_seq = RANK_SEQ_COMMON_DATA_GET;
    /* break through */

  ///< 共通データ取得
  case RANK_SEQ_COMMON_DATA_GET:
    {
      #if PM_DEBUG
      //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"RankingClient::GetCommonData(nexUniq:%llu) \n", m_nexUniqueId );
      #endif

      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      bool ret = m_rankingClient->GetCommonData( game_data->GetPlayerStatus()->GetNexUniqueID() );
      GFL_PRINT("GetCommonData\n");
      if( ret == false ){
        m_seq = RANK_SEQ_ERROR_RETURN;
      } else {
        m_seq = RANK_SEQ_WAIT;
      }
    }
    break;

  ///< 共通データリセット
  case RANK_SEQ_COMMON_DATA_RESET:
    {
      COMMON_DATA common_data;
      ::std::memset( &common_data , 0 , sizeof( common_data ) );
      common_data.disconnect_bit = 0;
      common_data.disconnect_flag  = 0;
      common_data.first_flag = 1;
      
      m_pWork->SetCommonData( common_data );

      #if PM_DEBUG
      //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"RankingClient::UploadCommonData(nexUniq:%llu) \n", m_nexUniqueId );
      #endif

      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      bool ret = m_rankingClient->UploadCommonData( &common_data, sizeof(COMMON_DATA), game_data->GetPlayerStatus()->GetNexUniqueID() );
      GFL_PRINT("UploadCommonData\n");
      if( ret == false ){
        m_seq = RANK_SEQ_ERROR_RETURN;
      } else {
        m_seq = RANK_SEQ_WAIT;
      }
    }
    break;

  case RANK_SEQ_WAIT:
    // リスナー通知待ち
    break;

  case RANK_SEQ_ERROR_RETURN:         ///< エラー
    // つうしん　エラーが　はっせいしました
    //m_view_lower->StartMessage( battle_match_guide_17, gfl::str::StrWin::FINISH_USER );
    //m_view->StopTimeIcon();
    m_seq = RANK_SEQ_ERROR_WAIT;
    break;

  case RANK_SEQ_ERROR_WAIT:           ///< エラーメッセージ待ち
    // 戻る場合はメッセージ待ち
    //if( m_view_lower->UpdateMessage() )
    //{
    //  // TOPへもどる
    //  SetFinish( NEXT_SELECT_MATCH );
    //  SetSwitchType( SWITCH_RETURN );
   // }
    break;

  case RANK_SEQ_EXIT:               ///< 破棄
    m_seq = RANK_SEQ_END;
    break;

  case RANK_SEQ_END:                ///< 終了
    // インターネット大会
    /*if( m_common_data->select_mode == SELECT_INTERNET_CUP )
    {
      // BTS[6442]:【シーケンス：インターネット大会の対戦数がセーブされていない】
      // サーバから取得した対戦回数がセーブしてある対戦回数を上回っていると書き換える
      {
        savedata::BattleMatch *sv_battle_match = m_game_data->GetBattleMatch();
        Ranking::RANK_DATA* rank = m_net->GetRankingRankData();
        u8 battle_count = ( rank->win + rank->lose + rank->draw );

        if( battle_count > sv_battle_match->GetInternetMatchCount() )
        {
          sv_battle_match->SetInternetMatchCount( battle_count );
          GFL_PRINT("対戦回数を%dに上書きました！\n", battle_count );
        }
      }
    
      // 対戦相手探す
      SetFinish( NEXT_SEARCH );
    }
    // フリー・レート
    else*/
    {
      //SetFinish( NEXT_CHALLENGE );  // このポケモンで戦いますか？
    }

    //m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatch_Matching);
    m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatch_Matching);
    m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_Matching);
    m_State =  STATE_Matching;

    break;
  }
#endif
}



//--------------------------------------------------------------------------
/**
 * @brief   ランキングクライアント初期化
 * @return  trueで成功
 */
//--------------------------------------------------------------------------
bool Frame_InternetBattleMatching::InitializeRanking( void )
{
#if defined(GF_PLATFORM_CTR)
  gflnet2::nex::NexManager* nexMgr;
  nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  
  nn::nex::Credentials* credential = nexMgr->GetCredentials();
  
  bool ret = m_rankingClient->Initialize( credential );
  GFL_PRINT("ランキングクライアント初期化 %d\n", ret);
  return ret;
#else
  return true;
#endif
}

//--------------------------------------------------------------------------
/**
 * @brief   ランキングクライアント破棄
 * @return  trueで成功
 */
//--------------------------------------------------------------------------
bool Frame_InternetBattleMatching::FinalizeRanking( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_rankingClient )
  {
    if( m_rankingClient->IsConnecting() )
    {
      GFL_PRINT("ランキングクライアント通信中\n");
      m_rankingClient->CancelConnecting();
      return false;
    }
    GFL_PRINT("ランキングクライアント終了\n");
    m_rankingClient->Finalize();
    m_rankingClient->UnregistRankingClientListener( this );
    GFL_SAFE_DELETE( m_rankingClient );
  }
#endif
  return true;
}

#if defined(GF_PLATFORM_CTR)
//==========================================================================
/**
 *					RankingClietnリスナー
 */
//==========================================================================

//------------------------------------------------------------------
/**
 * @brief スコアアップロードの成功通知
 *
 * @param[in] category  対象カテゴリID
 */
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnNexRankingUploadScoreSuccess( const u32 category )
{
  GFL_PRINT("OnNexRankingUploadScoreSuccess %d \n", category );

  // アップロードしたらどのモードでも終了
  m_seq = RANK_SEQ_EXIT;
}

//------------------------------------------------------------------
/**
 * @brief スコア削除の成功通知
 *
 * @param[in] category  対象カテゴリID
 */
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnNexRankingDeleteScoreSuccess( const u32 category )
{
  GFL_PRINT("OnNexRankingDeleteScoreSuccess %d \n", category );

  // アップロードしたらどのモードでも終了
  m_seq = RANK_SEQ_EXIT;
}

//------------------------------------------------------------------
/**
 * @brief 共通情報アップロードの成功通知
 */
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnNexRankingUploadCommonDataSuccess( void )
{
  GFL_PRINT("OnNexRankingUploadCommonDataSuccess\n");
  
  // アップロードしたらどのモードでも終了
  m_seq = RANK_SEQ_EXIT;
}


//------------------------------------------------------------------
/**
 * @brief ランキング共通データの取得成功通知
 *
 * @param[in] data ランキング共通データ
 */
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnNexRankingGetCommonDataSuccess( const nn::nex::qVector<qByte>& data )
{
  GFL_PRINT("OnNexRankingGetCommonDataSuccess\n");

  GFL_PRINT("data %d\n", data.size());

  // 
  // 一度も登録したことない場合はサイズが0
  // 
  if( data.size() != 0 )
  {
    COMMON_DATA commonData;
    ::std::memcpy( &commonData , &data[0] , sizeof( commonData ) );

    GFL_PRINT("disconnect_bit %d\n", commonData.disconnect_bit );
    GFL_PRINT("disconnect_flag %x\n", commonData.disconnect_flag );
    GFL_PRINT("first_flag %d\n", commonData.first_flag );

    m_pWork->SetCommonData( commonData );

    m_seq = RANK_SEQ_EXIT;
  }
  else
  {
    // 共通データの初期化
    m_seq = RANK_SEQ_COMMON_DATA_RESET;
    GFL_PRINT("共通データが存在しないので初期化！\n");
  }
}


//------------------------------------------------------------------
/**
 * @brief ランキングリストデータの取得成功通知
 *
 * @param[in] data ランキングリストデータ
 */
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnNexRankingGetListDataSuccess( const nn::nex::RatingRankData& data )
{
  GFL_PRINT("OnNexRankingGetListDataSuccess\n");

  GFL_PRINT("** principalId    = %d\n", data.GetPrincipalID());
  GFL_PRINT("** nexUniqueId    = %llu\n", data.GetUniqueId());
  //GFL_PRINT("** order          = %d\n", data.GetOrder());
  GFL_PRINT("** category       = %d\n", data.GetCategory());
  GFL_PRINT("** rate           = %d\n", data.GetRate());
  GFL_PRINT("** win            = %d\n", data.GetWinningCount());
  GFL_PRINT("** lose           = %d\n", data.GetLosingCount());
  GFL_PRINT("** draw           = %d\n", data.GetDrawCount());
  GFL_PRINT("** disconnect     = %d\n", data.GetDisconnectedCount());
  GFL_PRINT("** commonDataSize = %d\n", data.GetCommonData().size());

  if( m_matchingState == SEND_RECV_WAIT )
  {//相手の成績

    //時間確認
    //開催時間チェック
    gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
    nn::nex::DateTime dateTime;
    if( nexMgr->GetNgsFacade()->GetGameServerTime( &dateTime ) == false )
    {//エラー
      ExitSelf();
      return;
    }

    GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , dateTime.GetYear(),dateTime.GetMonth(),dateTime.GetDay(),dateTime.GetHour() );

    nn::nex::DateTime* pComStartTime = m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_START );
    nn::nex::DateTime* pComEndTime = m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_END );


    //今日のリミット
    u64 cupDayTotal = (dateTime - (*pComStartTime)) / ( 1000 * 60 * 60 * 24 );
    if( dateTime < (*pComStartTime) )
    {//開始前
      cupDayTotal = 0;
    }

    u32 battleCount = data.GetWinningCount() + data.GetLosingCount() + data.GetDrawCount();
    u32 todayLimitNum = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::BATTLE_NUM_LIMIT) * (cupDayTotal+1);
    if( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::BATTLE_NUM_LIMIT) == 0 )
    {//0だったら無制限
      todayLimitNum = 0xFFFFFFFF;
    }

    NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

    if( battleCount >= todayLimitNum || 
      battleCount >= m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::BATTLE_NUM) )
    {//対戦回数オーバー
      GFL_PRINT("相手の対戦回数違反\n");
      
      pP2pConnectionManager->DisconnectStart(true);
      m_matchingState = DISCONNECT_WAIT;
      if( m_isRetryExists == false )
      {
        m_isRetryExists = true;
        m_matchingCount = 0;//レート幅初期化
      }
      else
      {
        m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
      }
    }
    else
    {
      u64 otherNUID = pP2pConnectionManager->GetMyStatus( 1 - pP2pConnectionManager->GetNetID() )->GetNexUniqueID();
      bool ret = m_pglConnector->DownloadBattleBox( otherNUID , NetLib::Pgl::PGLConnector::BATTLE_TYPE_INTERNET );
      if( ret == false )
      {
        pP2pConnectionManager->DisconnectStart(true);
        m_matchingState = DISCONNECT_WAIT;
        if( m_isRetryExists == false )
        {
          m_isRetryExists = true;
          m_matchingCount = 0;//レート幅初期化
        }
        else
        {
          m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
        }
      }
    }
  }
  else
  {//自分の成績


    // 成績をセット
    RANK_DATA rank_data;
    ::std::memset( &rank_data , 0 , sizeof(rank_data) );
    rank_data.rate      = data.GetRate() / 1000;// サーバー側で1000倍されている
    rank_data.win       = data.GetWinningCount();
    rank_data.lose      = data.GetLosingCount();
    rank_data.draw      = data.GetDrawCount();   // 引き分け数
    rank_data.disconnect= data.GetDisconnectedCount(); // 切断数
    m_pWork->SetRankData( rank_data );

    u32 battleCount = rank_data.win + rank_data.lose + rank_data.draw;
    if( battleCount > m_pWork->m_pBattleSpotSave->GetInternetMatchCount() )
    {
      m_pWork->m_pBattleSpotSave->SetInternetMatchCount( battleCount );//対戦回数保存
    }

    m_seq = RANK_SEQ_RATE_DATA_GET_SUCCESS;
  }
}

//------------------------------------------------------------------
/**
 * @brief NexGlobalTradeStationClient通信のエラー通知
 *
 * @param[in] result     実行結果
 * @param[in] errorCode  エラーアプレット用エラーコード
 */
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnNexRankingClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("OnNexRankingClientError %d\n", errorCode );

  // データが存在しません
  // = データ初期化しようとしてない場合なので無視してよい
  if( result == QRESULT_ERROR( Ranking, NotFound ) || result == QRESULT_ERROR( Rating, NotFound ) )
  {
    GFL_PRINT("データが存在しません\n");
    if( m_matchingState == SEND_RECV_WAIT )
    {//存在してなかったら0回なのでOK扱い
      NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
      u64 otherNUID = pP2pConnectionManager->GetMyStatus( 1 - pP2pConnectionManager->GetNetID() )->GetNexUniqueID();
      bool ret = m_pglConnector->DownloadBattleBox( otherNUID , NetLib::Pgl::PGLConnector::BATTLE_TYPE_INTERNET );
      if( ret == false )
      {
        pP2pConnectionManager->DisconnectStart(true);
        m_matchingState = DISCONNECT_WAIT;
        if( m_isRetryExists == false )
        {
          m_isRetryExists = true;
          m_matchingCount = 0;//レート幅初期化
        }
        else
        {
          m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
        }
      }
    }
    else
    {
      m_seq = RANK_SEQ_DATA_GET_NOT_FOUND;
    }
  }
  else
  {
    // エラーアプレット呼び出し
    NetLib::Error::NijiNetworkError error;
    error.SetResultNex( result );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    if( m_matchingState == SEND_RECV_WAIT )
    {
      NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
      pP2pConnectionManager->DisconnectStart(true);
      m_matchingState = DISCONNECT_WAIT;
      if( m_isRetryExists == false )
      {
        m_isRetryExists = true;
        m_matchingCount = 0;//レート幅初期化
      }
      else
      {
        m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
      }
    }
    else
    {
      m_matchingState = MATCHING_ERROR;
      m_seq = RANK_SEQ_DATA_GET_NOT_FOUND;
    }
  }
}


//------------------------------------------------------------------
/**
* @brief PGLバトルボックス登録通信の完了イベントを検知
*
* @param[in]  boxData   ボックスデータ
* @param[in]  dataSize  ボックスデータサイズ
*/
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnPGLDownloadBattleBoxSuccess( const u8* boxData, u32 dataSize )
{
  GFL_PRINT("OnPGLDownloadBattleBoxSuccess %d\n", dataSize );

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  int myID = pP2pConnectionManager->GetNetID();

  u32 partydataSize   = ( pml::pokepara::CoreParam::DATASIZE * pml::PokeParty::MAX_MEMBERS ) + sizeof(u32);
  u8* pSerializeBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[partydataSize];

  u32 pokeNum = m_pWork->m_pAppParam->m_pPersonalData[ 1 - myID ]->pPokeParty->GetMemberCount();
  u8* pSerializePtr = pSerializeBuffer;
  for( u32 index = 0 ; index < pokeNum ; ++index )
  {
    m_pWork->m_pAppParam->m_pPersonalData[ 1 - myID ]->pPokeParty->GetMemberPointer( index )->Serialize_Core( pSerializePtr );
    pSerializePtr += pml::pokepara::CoreParam::DATASIZE;
  }

  u32* lockFlagPtr = reinterpret_cast<u32*>( &pSerializeBuffer[pml::pokepara::CoreParam::DATASIZE * pml::PokeParty::MAX_MEMBERS] );
  *lockFlagPtr = 1;//ロックフラグ kujira/sango時代とフォーマットを合わせる為
  
  bool isCompare = false;
  if( partydataSize == dataSize )
  {
    GFL_PRINT("battle box crc server[0x%x]\n",gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(boxData), partydataSize ) );
    GFL_PRINT("battle box crc other[0x%x]\n",gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(pSerializeBuffer), partydataSize ) );

    if( std::memcmp( boxData , pSerializeBuffer , dataSize ) == 0 )
    {//一致
      isCompare = true;
    }
    else
    {
      // @fix MMCat[127]:メモリ比較で一致しなかった場合、リボンを除外した一致を改めて行う
      //                 施設でリボンが取得出来る為、大会中にリボンが変わる可能性がある。
      isCompare = m_pWork->BattleTeamMatchCheckIgnoreRibon( boxData, pSerializeBuffer, pokeNum );
    }
  }
  else
  {//サイズ不一致
    GFL_ASSERT(0);//@fix
  }

  GFL_SAFE_DELETE_ARRAY( pSerializeBuffer );

  if( isCompare == true )
  {//一致
    pP2pConnectionManager->TimingStart( BattleSpotWork::CONNECT_COMPLETE_TIMING , true );

    m_matchingState = SEND_RECV_END;
  }
  else
  {//不一致
    pP2pConnectionManager->DisconnectStart(true);
    m_matchingState = DISCONNECT_WAIT;
    if( m_isRetryExists == false )
    {
      m_isRetryExists = true;
      m_matchingCount = 0;//レート幅初期化
    }
    else
    {
      m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
    }
  }
}

  //------------------------------------------------------------------
/**
* @brief PGL通信のエラーイベントを検知
*
* @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
*
* @param[in] commandType 完了したPGL通信コマンド
* @param[in] httpStatus サーバから返却されたHTTPステータスコード
*/
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultPGL5xx( httpStatus );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  pP2pConnectionManager->DisconnectStart(true);
  m_matchingState = DISCONNECT_WAIT;
  if( m_isRetryExists == false )
  {
    m_isRetryExists = true;
    m_matchingCount = 0;//レート幅初期化
  }
  else
  {
    m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
  }
}

//------------------------------------------------------------------
/**
* @brief PGL通信のレスポンスデータのエラーコードを検知
*
* @param[in]  commandType  完了したPGL通信コマンド
* @param[in]  resultCode   PGLサーバから受信したレスポンスエラーコード
*/
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultPGL( resultCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  pP2pConnectionManager->DisconnectStart(true);
  m_matchingState = DISCONNECT_WAIT;
  if( m_isRetryExists == false )
  {
    m_isRetryExists = true;
    m_matchingCount = 0;//レート幅初期化
  }
  else
  {
    m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
  }
}

//------------------------------------------------------------------
/**
* @brief PGL通信のエラーイベントを検知
*
* @param[in]  commandType  完了したPGL通信コマンド
* @param[in]  result       エラー内容
* @param[in]  errorCode    ネットワークエラーコード
*/
//------------------------------------------------------------------
void Frame_InternetBattleMatching::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  pP2pConnectionManager->DisconnectStart(true);
  m_matchingState = DISCONNECT_WAIT;
  if( m_isRetryExists == false )
  {
    m_isRetryExists = true;
    m_matchingCount = 0;//レート幅初期化
  }
  else
  {
    m_matchingCount = MAX_MATCHING_TIMEOUT;//終了させる
  }
}


#endif

//------------------------------------------------------------------
/**
  * @brief   ホーム、スリープ、ゲーム終了処理発生時イベント
  *
  * @retval  true    終了処理完了
  * @retval  false   終了処理中
  *
  * @note    適切な終了処理を記述してください。
  * @attention  On～という名前なのに他のメンバとふるまいが違うので注意
  * @attention  Observerパターンとして名前が適切なのかは疑問の余地あり
  */
//------------------------------------------------------------------
bool Frame_InternetBattleMatching::OnSuspendWithHomeSleepAndPower( void )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
#endif
  return cleanup();
}

void Frame_InternetBattleMatching::OnConfirmPlayStatusSuccess()
{
#if defined(GF_PLATFORM_CTR)
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  if( game_data->GetSaveData()->GetGameSync()->IsSignedPGL() )
  {//サインインずみ

    //時間確認
    //開催時間チェック
    gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
    nn::nex::DateTime dateTime;
    if( nexMgr->GetNgsFacade()->GetGameServerTime( &dateTime ) == false )
    {//エラー
      ExitSelf();
      return;
    }

    GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , dateTime.GetYear(),dateTime.GetMonth(),dateTime.GetDay(),dateTime.GetHour() );

    nn::nex::DateTime* pComStartTime = m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_START );
    nn::nex::DateTime* pComEndTime = m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_END );


    //今日のリミット
    u64 cupDayTotal = (dateTime - (*pComStartTime)) / ( 1000 * 60 * 60 * 24 );
    if( dateTime < (*pComStartTime) )
    {//開始前
      cupDayTotal = 0;
    }

    u32 todayLimitNum = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::BATTLE_NUM_LIMIT) * (cupDayTotal+1);
    if( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::BATTLE_NUM_LIMIT) == 0 )
    {//0だったら無制限
      todayLimitNum = 0xFFFFFFFF;
    }

    GFL_PRINT("サーバー時刻:%d/%d/%d %d:%d \n",
      dateTime.GetYear(),
      dateTime.GetMonth(),
      dateTime.GetDay(),
      dateTime.GetHour(),
      dateTime.GetMinute());
    GFL_PRINT("タイムゾーン:%d\n", m_pWork->m_pBattleSpotSave->GetInternetTimeZone() );

    s8  now_hour = dateTime.GetHour();
    // タイムゾーン適用
    {
      now_hour += (m_pWork->m_pBattleSpotSave->GetInternetTimeZone() / 2);

      // 余りがある = +-30分
      if( (m_pWork->m_pBattleSpotSave->GetInternetTimeZone() % 2) )
      {
        if( m_pWork->m_pBattleSpotSave->GetInternetTimeZone() > 0 )
        {
          // 30分以上あれば 1時間プラスする
          if( dateTime.GetMinute() >= 30 )
          {
            now_hour += 1;
          }
        }
        else
        {
          // 30分未満であれば 1時間マイナスする
          if( dateTime.GetMinute() < 30 )
          {
            now_hour -= 1;
          }
        }
      }
    }

    // 0?23の値にする
    // 0未満だったら+24する。
    // 23をこえたら-24する。
    if( now_hour < 0 ){
      now_hour += 24;
      GFL_PRINT("0未満なので+24\n");
    }
    else if( now_hour >= 24 )
    {
      now_hour -= 24;
      GFL_PRINT("23以上なので24\n");
    }

    GFL_PRINT("タイムゾーン適用後時間:%d \n", now_hour );

    GFL_PRINT("cupDayTotal[%llu]todayLimitNum[%u]\n",cupDayTotal,todayLimitNum);

    if( *(m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_END )) < dateTime )
    {//終了済み
      m_pWork->m_retireRequest = true;
      ExitSelf( RESULT_InternetBattleMatching_CupIsOver );
    }
    else if( m_pWork->IsRegulationTimeOut( now_hour ) )
    {//対戦できない時間
      m_pViewContainer->m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Message_NotAvailableTime);
      m_State = STATE_MessageWait;
    }
    else if( m_pWork->m_pBattleSpotSave->GetInternetMatchCount() >= todayLimitNum || 
             m_pWork->m_pBattleSpotSave->GetInternetMatchCount() >= m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::BATTLE_NUM) )
    {//対戦回数オーバー
      m_pViewContainer->m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Message_PrescribedMatches);
      m_State = STATE_MessageWait;
    }
    else
    {//対戦可能
      m_State = STATE_UpdateRank;
    }
  }
  else
  {//サインインしてない
    GFL_ASSERT(0);//@fix
    ExitSelf();
  }
#endif
}

void Frame_InternetBattleMatching::OnConfirmPlayStatusError( u32 errorCode )
{
  ExitSelf();
}


} /*  namespace Live    */
} /*  namespace NetApp  */



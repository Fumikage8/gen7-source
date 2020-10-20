//==============================================================================
/**
 * @file   BattleSpotFrame_RatingBattleMatching.cpp
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

#include  "./BattleSpotFrame_RatingBattleMatching.h"
#include  "GameSys/include/GameData.h"

#include <System/include/SystemEventManager.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#endif
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"

#include "NetStatic/NetEvent/include/BattleSpotEvent.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_GetPlayStatus = 0,
    STATE_Entry,
    STATE_Matching,

    STATE_Ready,
    STATE_Timeout,
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
  *  @func     Frame_RatingBattleMatching
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_RatingBattleMatching::Frame_RatingBattleMatching(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_GetPlayStatus)
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
#endif

}


//------------------------------------------------------------------
/**
  *  @func     Frame_RatingBattleMatching
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_RatingBattleMatching::~Frame_RatingBattleMatching()
{
#if defined(GF_PLATFORM_CTR)
  if( m_rankingClient )
  {
    m_rankingClient->UnregistRankingClientListener( this );
    GFL_SAFE_DELETE( m_rankingClient );
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
bool  Frame_RatingBattleMatching::startup()
{
  m_pViewContainer->SetAllInputEnabled( true );
  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_RatingBattleEntry);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Card;

    m_pViewContainer->m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
    
    switch( m_pWork->GetRule() )
    {
    case BattleSpotWork::RULE_RATE_DOUBLE:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matching_Double);
    break;
    case BattleSpotWork::RULE_RATE_SPECIAL:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matching_Special);
    break;
    case BattleSpotWork::RULE_RATE_WCS:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matching_WCS);
    break;
    case BattleSpotWork::RULE_RATE_SINGLE:
    default:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matching_Single);
    break;
    }
  }

  InitializeRanking();

  NetAppLib::GameSync::GETPLAYSTATUS_PARAM param;
  std::memset( &param , 0 , sizeof(param) );
  param.pWork = m_pWork;
  param.mode = NetAppLib::GameSync::GETPLAYSTATUS_MODE_RATING;

  NetAppLib::GameSync::GameSyncRequestFacade::AddConfirmPlayStatusRequest( this, &param );


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
void  Frame_RatingBattleMatching::setListener()
{
//  m_pUpperView->SetEventHandler(this);
  m_pLowerView->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_RatingBattleMatching::removeListener()
{
//  m_pUpperView->SetEventHandler(NULL);
  m_pLowerView->SetEventHandler(NULL);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_RatingBattleMatching::updateSequence()
{
  switch(m_State)
  {
    case  STATE_GetPlayStatus:
      break;
    case  STATE_Entry:
      {
        UpdateRankData();
      }
      break;
    case  STATE_Matching:
      {
        if( UpdateMatching() )
        {//マッチング確定
          Sound::PlaySE( SEQ_SE_SYS_NJ_006 );
          m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_RatingBattle_Matched);

          switch( m_pWork->GetRule() )
          {
          case BattleSpotWork::RULE_RATE_DOUBLE:
            m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matched_Double);
          break;
          case BattleSpotWork::RULE_RATE_SPECIAL:
            m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matched_Special);
          break;
          case BattleSpotWork::RULE_RATE_WCS:
            m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matched_WCS);
          break;
          case BattleSpotWork::RULE_RATE_SINGLE:
          default:
            m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_Matched_Single);
          break;
          }

          m_pUpperView->SetupOpponentText();

          m_seq = 0;
          m_State =  STATE_Ready;
        }
      }
    break;
    case STATE_Timeout:
      break;
    case  STATE_Ready:
        if( ReadyWait() )
        {//バトルへ
          m_pWork->m_pAppParam->nextApp = NEXT_APP_BATTLE;
          m_pWork->m_pAppParam->nextFrame = FRAME_RatingAfterBattle;

          m_pWork->DisconnectFlagOn( m_pWork->GetCommonData() );
          ExitSelf(RESULT_RatingBattleMatching_Next);
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
bool  Frame_RatingBattleMatching::cleanup()
{
  bool  bAllDone = false;

  if( FinalizeRanking() == false )
  {
    return false;
  }

  bAllDone  = true;


  return  bAllDone;
}




void  Frame_RatingBattleMatching::LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode)
{
  switch( eventCode )
  {
  case LowerView_Card::IEventHandler::EVENT_Message_EOM:
      if( m_State == STATE_Timeout )
      {
        ExitSelf(RESULT_RatingBattleMatching_Timeout);
      }
   break;
  }
}




void Frame_RatingBattleMatching::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
#if defined(GF_PLATFORM_CTR)
  if( m_matchingState == DISCONNECT_WAIT )
  {//キャンセル後
    GFL_PRINT("Frame_InternetBattleMatching::OnPacketTransferSuccess : skip\n");
    return;
  }

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();

  GFL_PRINT("Frame_RatingBattleMatching::OnPacketTransferSuccess : data count[%d]\n",receivedDataCount);

  int myID = pP2pConnectionManager->GetNetID();
  if( myID < 0 )
  {
    return;
  }

  if( myID > 0 && myID < BTL_CLIENT_NUM )
  {//自分のパーティを正しい位置に移動
    m_pWork->m_pAppParam->m_pPersonalData[myID]->pPokeParty->CopyFrom( *m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
  }

  m_pWork->m_pAppParam->m_pPersonalData[myID]->qrTeam = m_pWork->m_pAppParam->m_isQRTeam;//自分のQRチーム状態を設定

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
        m_pWork->m_pAppParam->m_pPersonalData[netID]->qrTeam = m_syncData.m_isQRTeam;

        u32 rate = m_syncData.m_rate;

        m_pWork->SetOpponentRate( rate );

        bool ret = m_pWork->CheckPokeParty( netID , m_syncData.m_signatureBuffer , &m_syncData.regulationComp );
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

  pP2pConnectionManager->TimingStart( BattleSpotWork::CONNECT_COMPLETE_TIMING , true );

  m_matchingState = SEND_RECV_END;
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_RatingBattleMatching::ExitSelf(const FrameResult frameResult)
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
bool Frame_RatingBattleMatching::UpdateMatching()
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
      NetLib::P2P::ENUM_NIJI_P2P_ID p2pID = NetLib::P2P::NIJI_P2P_RNDRATE_S;
      switch( m_pWork->GetRule() )
      {
      case BattleSpotWork::RULE_RATE_DOUBLE:
        p2pID = NetLib::P2P::NIJI_P2P_RNDRATE_D;
      break;
      case BattleSpotWork::RULE_RATE_SPECIAL:
        p2pID = NetLib::P2P::NIJI_P2P_RNDRATE_SP;
      break;
      case BattleSpotWork::RULE_RATE_WCS:
        p2pID = NetLib::P2P::NIJI_P2P_RNDRATE_WCS;
      break;
      case BattleSpotWork::RULE_RATE_SINGLE:
      default:
        p2pID = NetLib::P2P::NIJI_P2P_RNDRATE_S;
      break;
      }

      // マッチング用のレートのRange設定を求める
      u32 battleCount = m_pWork->GetRankData().win + m_pWork->GetRankData().lose + m_pWork->GetRankData().draw;
      u32 battleRate = m_pWork->GetRankData().rate;
      if( battleCount <= BATTLE_INIT_COUNT )
      {//10戦以内だったら補正
        int winlose = m_pWork->GetRankData().win - m_pWork->GetRankData().lose;

        float addParam = ( (float)winlose / (float)battleCount );

        float addResult = addParam * ( (BATTLE_INIT_COUNT+1) - battleCount ) * (BATTLE_INIT_COUNT-1);

        GFL_PRINT("%f = (%d - %d) / %d * ((11-%d)) * 9\n",addResult,m_pWork->GetRankData().win,m_pWork->GetRankData().lose
          ,battleCount,battleCount);

        battleRate += (int)addResult;
      }

      u32 battleRetryCount = m_matchingCount;
      u32 range = m_pWork->GetRateRange( battleRate , battleRetryCount );
      GFL_PRINT("Frame_FriendBattleMatching : range = %d rate = %d retrycount = %d\n", range ,battleRate,battleRetryCount);

      // レートを設定
      int rate      = battleRate;
      int rate_min  = battleRate - range;
      int rate_max  = battleRate + range;

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

      GFL_PRINT("Frame_RatingBattleMatching : CutRate : %d %d %d \n", cut_per, cut_per_min, cut_per_max );

      // WiFiマッチング用条件
      gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
      gflnet2::nex::NexMatchMakingRule rule0( 0 , rate, rate_min , rate_max );
      gflnet2::nex::NexMatchMakingRule rule1( 1 , cut_per, cut_per_min , cut_per_max );
      list.push_back( &rule0 );
      list.push_back( &rule1 );

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
        switch( m_pWork->GetRule() )
        {
        case BattleSpotWork::RULE_RATE_DOUBLE:
          m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_RatingBattle_Matched_Double);
          break;
        case BattleSpotWork::RULE_RATE_SPECIAL:
          m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_RatingBattle_Matched_Special);
          break;
        case BattleSpotWork::RULE_RATE_WCS:
          m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_RatingBattle_Matched_WCS);
          break;
        case BattleSpotWork::RULE_RATE_SINGLE:
        default:
          m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_RatingBattle_Matched_Single);
          break;
        }
        
        GFL_SAFE_DELETE( m_pP2pPacketTransfer );
        gfl2::heap::HeapBase* pDeviceHeap = m_pWork->GetDeviceHeap()->GetLowerHandle();
        m_pP2pPacketTransfer = GFL_NEW( pDeviceHeap ) NetLib::P2P::P2pPacketTransfer( pDeviceHeap, this );
        ::std::memset( &m_syncData , 0 , sizeof(m_syncData) );
        m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->SerializeCore( &m_syncData.m_serializeBuffer );

        m_syncData.m_rate = m_pWork->GetRankData().rate;
        m_syncData.m_isQRTeam = m_pWork->m_pAppParam->m_isQRTeam;

        GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        std::memcpy( m_syncData.m_signatureBuffer , game_data->GetSignatureConst() , sizeof(m_syncData.m_signatureBuffer) );

        m_pWork->SetupRegulationComp( &m_syncData.regulationComp );

        m_pP2pPacketTransfer->SendDataCommand( &m_syncData , sizeof(m_syncData) , SEND_COMMAND );
        m_matchingState = SEND_RECV_WAIT;
      }
      else if( m_CountDownTimer.IsTimeOut() )
      {
        GFL_PRINT("TimeOut!!\n");
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
          m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_RatingBattle_MatchingFailed);

          m_State = STATE_Timeout;
        }
      }
    }
    break;
  case SEND_RECV_WAIT:
    {
      if( NetEvent::BattleSpot::BattleSpotEvent::IsSessionLeftFlag() )
      {
        GFL_PRINT("SEND_RECV_WAIT disconnect\n");
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
        GFL_PRINT("SEND_RECV_END disconnect\n");
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

bool Frame_RatingBattleMatching::ReadyWait()
{
  if( s_wait++ > 30 )
  {
    s_wait = 0;
    return true;
  }

  return false;
}


void Frame_RatingBattleMatching::UpdateRankData()
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

    m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatch_Matching);
    m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_RatingBattle_Matching);
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
bool Frame_RatingBattleMatching::InitializeRanking( void )
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
bool Frame_RatingBattleMatching::FinalizeRanking( void )
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
void Frame_RatingBattleMatching::OnNexRankingUploadScoreSuccess( const u32 category )
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
void Frame_RatingBattleMatching::OnNexRankingDeleteScoreSuccess( const u32 category )
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
void Frame_RatingBattleMatching::OnNexRankingUploadCommonDataSuccess( void )
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
void Frame_RatingBattleMatching::OnNexRankingGetCommonDataSuccess( const nn::nex::qVector<qByte>& data )
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
void Frame_RatingBattleMatching::OnNexRankingGetListDataSuccess( const nn::nex::RatingRankData& data )
{
  GFL_PRINT("OnNexRankingGetListDataSuccess\n");

  {
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

    // 成績をセット
    RANK_DATA rank_data;
    ::std::memset( &rank_data , 0 , sizeof(rank_data) );
    rank_data.rate      = data.GetRate() / 1000;// サーバー側で1000倍されている
    rank_data.win       = data.GetWinningCount();
    rank_data.lose      = data.GetLosingCount();
    rank_data.draw      = data.GetDrawCount();   // 引き分け数
    rank_data.disconnect= data.GetDisconnectedCount(); // 切断数
    m_pWork->SetRankData( rank_data );
  }

  m_seq = RANK_SEQ_RATE_DATA_GET_SUCCESS;
}

//------------------------------------------------------------------
/**
 * @brief NexGlobalTradeStationClient通信のエラー通知
 *
 * @param[in] result     実行結果
 * @param[in] errorCode  エラーアプレット用エラーコード
 */
//------------------------------------------------------------------
void Frame_RatingBattleMatching::OnNexRankingClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("OnNexRankingClientError %d\n", errorCode );

  // データが存在しません
  // = データ初期化しようとしてない場合なので無視してよい
  if( result == QRESULT_ERROR( Ranking, NotFound ) || result == QRESULT_ERROR( Rating, NotFound ) )
  {
    GFL_PRINT("データが存在しません\n");
    m_seq = RANK_SEQ_DATA_GET_NOT_FOUND;
  }
  else
  {
    // エラーアプレット呼び出し
    NetLib::Error::NijiNetworkError error;
    error.SetResultNex( result );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
    m_matchingState = MATCHING_ERROR;
    m_seq = 0;
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
bool Frame_RatingBattleMatching::OnSuspendWithHomeSleepAndPower( void )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
#endif
  return cleanup();
}


void Frame_RatingBattleMatching::OnConfirmPlayStatusSuccess()
{
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  if( game_data->GetSaveData()->GetGameSync()->IsSignedPGL() )
  {//サインインずみ
    m_State = STATE_Entry;
  }
  else
  {//サインインしてない
    GFL_ASSERT(0);//@fix
    ExitSelf();
  }
}

void Frame_RatingBattleMatching::OnConfirmPlayStatusError( u32 errorCode )
{
  ExitSelf();
}

} /*  namespace Live    */
} /*  namespace NetApp  */



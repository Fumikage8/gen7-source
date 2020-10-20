//==============================================================================
/**
 * @file   BattleSpotFrame_FreeBattleMatching.cpp
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

#include  "./BattleSpotFrame_FreeBattleMatching.h"
#include  "GameSys/include/GameData.h"
#include <Savedata/include/MyStatus.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include "NetStatic/NetEvent/include/BattleSpotEvent.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_Matching,

    //@fix NMCat[#4104 フリーバトルでマッチング直後に通信切断すると切断フラグが増加しない]
    STATE_DisconnectIncWait,

    STATE_Ready,
    STATE_Exit,
    STATE_Timeout,

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
  *  @func     Frame_FreeBattleMatching
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_FreeBattleMatching::Frame_FreeBattleMatching(BattleSpotWork* pWork)
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
  , m_isRetryExists(false)
  , m_isRetryRequest(false)
{
  ::std::memset( &m_syncData , 0 , sizeof(m_syncData) );
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
#if defined(GF_PLATFORM_CTR)
  // ランキングクライアント作成
  m_rankingClient = GFL_NEW( m_pWork->GetDeviceHeap() ) gflnet2::nex::NexRankingClient();
  m_rankingClient->RegistRankingClientListener( this );
#endif

}


//------------------------------------------------------------------
/**
  *  @func     Frame_FreeBattleMatching
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_FreeBattleMatching::~Frame_FreeBattleMatching()
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
bool  Frame_FreeBattleMatching::startup()
{
  m_pViewContainer->SetAllInputEnabled( true );
  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_FreeBattleEntry);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Card;

    m_pViewContainer->m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
    
    switch( m_pWork->GetRule() )
    {
    case BattleSpotWork::RULE_FREE_DOUBLE:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FreeBattle_Matching_Double);
    break;
    case BattleSpotWork::RULE_FREE_ROYAL:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FreeBattle_Matching_BattleRoyal);
    break;
    case BattleSpotWork::RULE_FREE_SINGLE:
    default:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FreeBattle_Matching_Single);
    break;
    }
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
void  Frame_FreeBattleMatching::setListener()
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
void  Frame_FreeBattleMatching::removeListener()
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
void  Frame_FreeBattleMatching::updateSequence()
{
  switch(m_State)
  {
  case  STATE_Entry:
    {
      UpdateRankData();
    }
    break;
  case  STATE_Matching:
    {
      if( UpdateMatching() )
      {//マッチング確定
#if defined(GF_PLATFORM_CTR)
        //@fix NMCat[#4104 フリーバトルでマッチング直後に通信切断すると切断フラグが増加しない]
        m_pWork->DisconnectFlagOn( m_pWork->GetCommonData() );
        GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        bool ret = m_rankingClient->UploadCommonData( &m_pWork->GetCommonData(), sizeof(COMMON_DATA), game_data->GetPlayerStatus()->GetNexUniqueID() );
        GFL_PRINT("UploadCommonData\n");
        if( ret == false ){
          GFL_ASSERT( 0 );//@fix
          ExitSelf(RESULT_FreeBattleMatching_Timeout);
        } else {
          m_State = STATE_DisconnectIncWait;
        }
#endif
      }
    }
    break;
  case  STATE_DisconnectIncWait:
    {
    }
    break;
  case  STATE_Ready:
      if( ReadyWait() )
      {
        m_pWork->m_pAppParam->nextApp = NEXT_APP_BATTLE;
        m_pWork->m_pAppParam->nextFrame = FRAME_FreeAfterBattle;

        exitFrame(RESULT_FreeBattleMatching_Next);
      }
    break;
  case STATE_Timeout:
    break;
  case  STATE_Exit:
    exitFrame(m_Result);
    break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_FreeBattleMatching::cleanup()
{
  bool  bAllDone = false;

  if( FinalizeRanking() == false )
  {
    return false;
  }

  bAllDone  = true;


  return  bAllDone;
}


void  Frame_FreeBattleMatching::LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode)
{
  switch( eventCode )
  {
  case LowerView_Card::IEventHandler::EVENT_Message_EOM:
    if( m_State == STATE_Timeout )
    {
      ExitSelf(RESULT_FreeBattleMatching_Timeout);
    }
  break;
  }
}


void Frame_FreeBattleMatching::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
#if defined(GF_PLATFORM_CTR)
  if( m_matchingState == DISCONNECT_WAIT )
  {//キャンセル後
    GFL_PRINT("Frame_InternetBattleMatching::OnPacketTransferSuccess : skip\n");
    return;
  }

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();

  GFL_PRINT("Frame_FreeBattleMatching::OnPacketTransferSuccess : data count[%d]\n",receivedDataCount);

  int myID = pP2pConnectionManager->GetNetID();
  if( myID < 0 )
    return;

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

        bool ret = m_pWork->CheckPokeParty( netID , m_syncData.m_signatureBuffer , &m_syncData.regulationComp );
        if( ret == false )
        {
          GFL_PRINT("invalid pokemon \n");
          pP2pConnectionManager->DisconnectStart(true);
          m_matchingState = DISCONNECT_WAIT;

          if( m_isRetryExists == false )
          {
            m_isRetryExists = true;
            m_isRetryRequest = true;
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
void  Frame_FreeBattleMatching::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}



bool Frame_FreeBattleMatching::UpdateMatching()
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
      NetLib::P2P::ENUM_NIJI_P2P_ID p2pID = NetLib::P2P::NIJI_P2P_RNDFREE_S;
      switch( m_pWork->GetRule() )
      {
      case BattleSpotWork::RULE_FREE_DOUBLE:
        if( m_pWork->GetLegendEnable() )
          p2pID = NetLib::P2P::NIJI_P2P_RNDFREE_D_LEGEND;
        else
          p2pID = NetLib::P2P::NIJI_P2P_RNDFREE_D;
      break;
      case BattleSpotWork::RULE_FREE_ROYAL:
        if( m_pWork->GetLegendEnable() )
          p2pID = NetLib::P2P::NIJI_P2P_RNDFREE_B_LEGEND;
        else
          p2pID = NetLib::P2P::NIJI_P2P_RNDFREE_B;
      break;
      case BattleSpotWork::RULE_FREE_SINGLE:
      default:
        if( m_pWork->GetLegendEnable() )
          p2pID = NetLib::P2P::NIJI_P2P_RNDFREE_S_LEGEND;
        else
          p2pID = NetLib::P2P::NIJI_P2P_RNDFREE_S;
      break;
      }

      // 切断率
      int cut_per = m_pWork->GetDisconnectRate( &m_pWork->GetCommonData() );
      int cut_per_min = cut_per - BattleSpotWork::CUT_RANGE;
      int cut_per_max = cut_per + BattleSpotWork::CUT_RANGE;

      // マイナスチェック
      if( cut_per < 0 ){ cut_per = 0; }
      if( cut_per_min < 0 ){ cut_per_min = 0; }
      if( cut_per_max < 0 ){ cut_per_max = 0; }

      GFL_PRINT("Frame_FreeBattleMatching : CutRate : %d %d %d \n", cut_per, cut_per_min, cut_per_max );

      // WiFiマッチング用条件
      gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
      gflnet2::nex::NexMatchMakingRule rule1( 0, cut_per, cut_per_min, cut_per_max );
      list.push_back( &rule1 );

      pP2pConnectionManager->Initialize( p2pID );

      //@fix #4127   インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】
      if( m_pWork->GetRule() == BattleSpotWork::RULE_FREE_ROYAL )
      {
        pP2pConnectionManager->SetupMigrationCallBackEnable();
      }

      pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::AUTO , &list );
      NetEvent::BattleSpot::BattleSpotEvent::SetSessionLeftFlag( false );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->P2pDisconnectSpanOn();

      m_CountDownTimer.TimeOutStart( MATCHING_TIMEOUT_MSEC_FREE );

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
        case BattleSpotWork::RULE_FREE_DOUBLE:
          m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_FreeBattle_Matched_Double);
          break;
        case BattleSpotWork::RULE_FREE_ROYAL:
          m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_FreeBattle_Matched_BattleRoyal);
          break;
        case BattleSpotWork::RULE_FREE_SINGLE:
        default:
          m_pUpperView->StartIconSetup(UpperView_Information::VIEWMODE_FreeBattle_Matched_Single);
          break;
        }

        GFL_SAFE_DELETE( m_pP2pPacketTransfer );
        gfl2::heap::HeapBase* pDeviceHeap = m_pWork->GetDeviceHeap()->GetLowerHandle();
        m_pP2pPacketTransfer = GFL_NEW( pDeviceHeap ) NetLib::P2P::P2pPacketTransfer( pDeviceHeap, this );
        ::std::memset( &m_syncData , 0 , sizeof(m_syncData) );
        m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->SerializeCore( &m_syncData.m_serializeBuffer );

        m_syncData.m_isQRTeam = m_pWork->m_pAppParam->m_isQRTeam;

        GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        std::memcpy( m_syncData.m_signatureBuffer , game_data->GetSignatureConst() , sizeof(m_syncData.m_signatureBuffer) );

        m_pWork->SetupRegulationComp( &m_syncData.regulationComp );
        
        m_pP2pPacketTransfer->SendDataCommand( &m_syncData , sizeof(m_syncData) , SEND_COMMAND );
        m_matchingState = SEND_RECV_WAIT;
      }
      else if( m_CountDownTimer.IsTimeOut() )
      {//タイムアウト
        pP2pConnectionManager->DisconnectStart(true);
        m_matchingState = DISCONNECT_WAIT;
      }
    }
  break;
  case DISCONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        GFL_SAFE_DELETE( m_pP2pPacketTransfer );
        pP2pConnectionManager->Terminate();
        
        if( m_isRetryRequest == true )
        {
          m_isRetryRequest = false;
          m_matchingState = MATCHING_INIT;
        }
        else
        {
          m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_FreeBattle_MatchingFailed);

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

bool Frame_FreeBattleMatching::ReadyWait()
{
  if( s_wait++ > 60 )
  {
    s_wait = 0;
    return true;
  }

  return false;
}

void Frame_FreeBattleMatching::UpdateRankData()
{
#if defined(GF_PLATFORM_CTR)
  switch( m_seq ){
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
    m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_FreeBattle_Matching);
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
bool Frame_FreeBattleMatching::InitializeRanking( void )
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
bool Frame_FreeBattleMatching::FinalizeRanking( void )
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
 * @brief 共通情報アップロードの成功通知
 */
//------------------------------------------------------------------
void Frame_FreeBattleMatching::OnNexRankingUploadCommonDataSuccess( void )
{
  GFL_PRINT("OnNexRankingUploadCommonDataSuccess\n");
  
  //@fix NMCat[#4104 フリーバトルでマッチング直後に通信切断すると切断フラグが増加しない]
  if( m_State == STATE_DisconnectIncWait )
  {//切断カウント増やすとき
    Sound::PlaySE( SEQ_SE_SYS_NJ_006 );
    m_pLowerView->SetViewMode(LowerView_Card::VIEWMODE_FreeBattle_Matched);

    switch( m_pWork->GetRule() )
    {
    case BattleSpotWork::RULE_FREE_DOUBLE:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FreeBattle_Matched_Double);
      m_pUpperView->SetupOpponentText();
      break;
    case BattleSpotWork::RULE_FREE_ROYAL:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FreeBattle_Matched_BattleRoyal);
      m_pUpperView->SetupOpponentTextRoyal();
      break;
    case BattleSpotWork::RULE_FREE_SINGLE:
    default:
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FreeBattle_Matched_Single);
      m_pUpperView->SetupOpponentText();
      break;
    }

    m_State =  STATE_Ready;
  }
  else
  {
    // アップロードしたらどのモードでも終了
    m_seq = RANK_SEQ_EXIT;
  }
}

//------------------------------------------------------------------
/**
 * @brief ランキング共通データの取得成功通知
 *
 * @param[in] data ランキング共通データ
 */
//------------------------------------------------------------------
void Frame_FreeBattleMatching::OnNexRankingGetCommonDataSuccess( const nn::nex::qVector<qByte>& data )
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
 * @brief NexGlobalTradeStationClient通信のエラー通知
 *
 * @param[in] result     実行結果
 * @param[in] errorCode  エラーアプレット用エラーコード
 */
//------------------------------------------------------------------
void Frame_FreeBattleMatching::OnNexRankingClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("OnNexRankingClientError %d\n", errorCode );

  //@fix NMCat[#4104 フリーバトルでマッチング直後に通信切断すると切断フラグが増加しない]
  if( m_State == STATE_DisconnectIncWait )
  {//切断カウント増やすとき
    // エラーアプレット呼び出し
    NetLib::Error::NijiNetworkError error;
    error.SetResultNex( result );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    ExitSelf(RESULT_FreeBattleMatching_Timeout);
  }
  else
  {
    // データが存在しません
    // = データ初期化しようとしてない場合なので無視してよい
    if( result == QRESULT_ERROR( Ranking, NotFound ) || result == QRESULT_ERROR( Rating, NotFound ) )
    {
      GFL_PRINT("データが存在しません\n");
      m_seq = RANK_SEQ_EXIT;
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
bool Frame_FreeBattleMatching::OnSuspendWithHomeSleepAndPower( void )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
#endif
  return cleanup();
}

} /*  namespace Live    */
} /*  namespace NetApp  */



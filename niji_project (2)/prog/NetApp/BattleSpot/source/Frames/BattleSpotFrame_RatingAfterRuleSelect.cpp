//==============================================================================
/**
 * @file   BattleSpotFrame_RatingAfterRuleSelect.cpp
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

#include  "./BattleSpotFrame_RatingAfterRuleSelect.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include  "GameSys/include/GameData.h"
#include  <Savedata/include/BattleSpotSave.h>
#include <System/include/SystemEventManager.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Selection,
    STATE_Save,
    STATE_SaveWait,
    STATE_Reset,
    STATE_Exit,
    STATE_MessageWait,
    STATE_MessageWaitEnd,

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
  *  @func     Frame_RatingAfterRuleSelect
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_RatingAfterRuleSelect::Frame_RatingAfterRuleSelect(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
//  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  , m_resetRateSeq(0)
{
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
#if defined(GF_PLATFORM_CTR)
  // ランキングクライアント作成
  m_rankingClient = GFL_NEW( m_pWork->GetDeviceHeap() ) gflnet2::nex::NexRankingClient();
  m_rankingClient->RegistRankingClientListener( this );
#endif
}


//------------------------------------------------------------------
/**
  *  @func     Frame_RatingAfterRuleSelect
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_RatingAfterRuleSelect::~Frame_RatingAfterRuleSelect()
{
#if defined(GF_PLATFORM_CTR)
  if( m_rankingClient )
  {
    m_rankingClient->UnregistRankingClientListener( this );
    GFL_SAFE_DELETE( m_rankingClient );
  }

#endif
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_RatingAfterRuleSelect::startup()
{
  m_pViewContainer->SetAllInputEnabled( true );
  //m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_RatingBattleEntry);

  /*  views  */
  {
//    m_pUpperView  = m_pViewContainer->m_pUpperView_Demo;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
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
void  Frame_RatingAfterRuleSelect::setListener()
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
void  Frame_RatingAfterRuleSelect::removeListener()
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
void  Frame_RatingAfterRuleSelect::updateSequence()
{
#if defined(GF_PLATFORM_CTR)
  switch(m_State)
  {
    case  STATE_Entry:
      {
        if( UpdateNetwork() )
        {
          //時間確認
          //開催時間チェック
          gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
          nn::nex::DateTime dateTime;
          if( nexMgr->GetNgsFacade()->GetGameServerTime( &dateTime ) == false )
          {//エラー
            ExitSelf(RESULT_RatingAfterRuleSelect_Back);
            return;
          }

          GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , dateTime.GetYear(),dateTime.GetMonth(),dateTime.GetDay(),dateTime.GetHour() );

          if( *(m_pWork->GetCommunityEndDateTime( m_pWork->GetRule() - BattleSpotWork::RULE_RATE_SINGLE )) < dateTime )
          {//終了済み
            m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_SeasonIsOver );
            m_State = STATE_MessageWaitEnd;
            return;
          }

          //コミュニティ、PIDチェック
          if( CheckCommunityID() )
          {//OK
            if( CheckPid() )
            {//OK
              exitFrame(RESULT_RatingAfterRuleSelect_Next);
            }
            else
            {//本体変わった
              m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_ResetRateConfirm);
              m_State = STATE_Selection;
            }
          }
          else
          {//未登録
            m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_RegistPIDConfirm);
            m_State = STATE_Selection;
          }
        }
      }
      break;

    case  STATE_Selection:
      break;
    case  STATE_Save:
      {
        SetNewCommunityId();
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_RegistPIDWait);
        UpdateSave();
        m_State = STATE_SaveWait;
      }
      //break;
    case  STATE_SaveWait:
      {
        if( UpdateSave() == false )
        {
          break;
        }

        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_RegistedPID);
        m_State = STATE_MessageWait;
      }
      break;
    case  STATE_Reset:
      {
        if( UpdateResetRate() )
        {//完了
          m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_ResetRate );
          m_State = STATE_MessageWait;
        }
      } 
    break;
    case  STATE_MessageWait:
    case STATE_MessageWaitEnd:
      break;
    case  STATE_Exit:
      exitFrame(m_Result);
      break;
  }
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_RatingAfterRuleSelect::cleanup()
{
  bool  bAllDone = false;

  if( FinalizeRanking() == false )
  {
    return false;
  }

  bAllDone  = true;


  return  bAllDone;
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
void  Frame_RatingAfterRuleSelect::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_RegistPID_Yes:             
      {
        m_State = STATE_Save;
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_RegistPID_No:             
      {
        ExitSelf(RESULT_RatingAfterRuleSelect_Back);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_ResetRate_Yes:             
      {
        m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_ResetRateWait );
        m_State = STATE_Reset;
      }
      break;
    case  LowerView_Selection::IEventHandler::EVENT_ResetRate_No:             
      {
        ExitSelf(RESULT_RatingAfterRuleSelect_Back);
      }
      break;
    case LowerView_Selection::IEventHandler::EVENT_Message_EOM:
      {
        if( m_State == STATE_MessageWait )
        {
          ExitSelf(RESULT_RatingAfterRuleSelect_Next);
        }
        else if( m_State == STATE_MessageWaitEnd )
        {
          ExitSelf(RESULT_RatingAfterRuleSelect_Back);
        }
      }
    break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_RatingAfterRuleSelect::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}

static u32 s_wait = 0;

bool Frame_RatingAfterRuleSelect::UpdateNetwork()
{
#if defined(GF_PLATFORM_CTR)
  if( s_wait++ > 90 )
  {
    s_wait = 0;
    return true;
  }
#endif

  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief   コミュニティIDの確認
 * @return  trueで一致した  falseで不一致
 */
//--------------------------------------------------------------------------
bool Frame_RatingAfterRuleSelect::CheckCommunityID( void )
{
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::BattleSpotData *sv_battle_match = game_data->GetBattleSpotData();

  u32 community_id1 = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
  u32 community_id2 = sv_battle_match->GetEntryCommunityId( static_cast<Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE>(m_pWork->GetRule()-2) );
  GFL_PRINT(" community_id1 %d \n", community_id1 );
  GFL_PRINT(" community_id2 %d \n", community_id2 );
  
  if( community_id1 == community_id2 )
  {
    return true;
  }
  else
  {
    return false;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   プリンシパルIDの確認
 * @return  trueで一致した  falseで不一致
 */
//--------------------------------------------------------------------------
bool Frame_RatingAfterRuleSelect::CheckPid( void )
{
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::BattleSpotData *sv_battle_match = game_data->GetBattleSpotData();
  Savedata::MyStatus    *sv_mystatus = game_data->GetPlayerStatus();


  Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE type = m_pWork->GetCategoryID();

  u32 pid1 = sv_battle_match->GetEntryCommunityPid( static_cast<Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE>( type ) );
  u32 pid2 = sv_mystatus->GetPrincipalId();
  GFL_PRINT(" pid1 %d \n", pid1 );
  GFL_PRINT(" pid2 %d \n", pid2 );
  
  if( pid1 == pid2 )
  {
    return true;
  }
  else
  {
    return false;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   セーブに最新のコミュニティIDとプリンシパルIDをセット
 */
//--------------------------------------------------------------------------
void Frame_RatingAfterRuleSelect::SetNewCommunityId( void )
{
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::BattleSpotData *sv_battle_match = game_data->GetBattleSpotData();
  Savedata::MyStatus    *sv_mystatus = game_data->GetPlayerStatus();

  // 最新のコミュニティID
  u32 community_id = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
  // 自分のプリンシパルID
  u32 pid = sv_mystatus->GetPrincipalId();

  // セーブするデータをセット
  Savedata::BattleSpotData::ENTRY_COMMUNITY entry_community;
  entry_community.community_id = community_id;
  entry_community.principalId = pid;

  Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE type = m_pWork->GetCategoryID();

  // セーブにデータセット
  sv_battle_match->SetEntryCommunity( entry_community, type );
}



//--------------------------------------------------------------------------
/**
 * @brief   レートの初期化処理
 */
//--------------------------------------------------------------------------
bool Frame_RatingAfterRuleSelect::UpdateResetRate( void )
{
#if defined(GF_PLATFORM_CTR)

  switch( m_resetRateSeq )
  {
  case RESET_RATE_INIT:
    {
      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();

      // スコアデータ削除
      u32 category_id = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
      bool ret = m_rankingClient->DeleteScore( category_id, game_data->GetPlayerStatus()->GetNexUniqueID() );
      GFL_PRINT("DeleteScore category %d\n", category_id );
      if( ret == true )
      {
        m_resetRateSeq = RESET_RATE_WAIT;
      } 
      else 
      {
        GFL_ASSERT( 0 );//@fix
        ExitSelf(RESULT_RatingAfterRuleSelect_Back);
        return true;
      }
    }
  break;
  case RESET_RATE_WAIT:
  break;
  case RESET_RATE_END:
    {
      bool ret = UpdateSave();
      if( ret )
      {//セーブ終わり
        return true;
      }
    }
  break;
  }

#endif

  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief   ランキングクライアント初期化
 * @return  trueで成功
 */
//--------------------------------------------------------------------------
bool Frame_RatingAfterRuleSelect::InitializeRanking( void )
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
bool Frame_RatingAfterRuleSelect::FinalizeRanking( void )
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
* @brief スコア削除の成功通知
*
* @param[in] category  対象カテゴリID
*/
//------------------------------------------------------------------
void Frame_RatingAfterRuleSelect::OnNexRankingDeleteScoreSuccess( const u32 category )
{
  GFL_PRINT("OnNexRankingDeleteScoreSuccess[%d]\n",category);
  m_resetRateSeq = RESET_RATE_END;

  SetNewCommunityId();
  UpdateSave();
}

//------------------------------------------------------------------
/**
 * @brief NexGlobalTradeStationClient通信のエラー通知
 *
 * @param[in] result     実行結果
 * @param[in] errorCode  エラーアプレット用エラーコード
 */
//------------------------------------------------------------------
void Frame_RatingAfterRuleSelect::OnNexRankingClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("OnNexRankingClientError %d\n", errorCode );

  // データが存在しません
  // = データ初期化しようとしてない場合なので無視してよい
  if( result == QRESULT_ERROR( Ranking, NotFound ) || result == QRESULT_ERROR( Rating, NotFound ) )
  {
    GFL_PRINT("データが存在しません\n");
    SetNewCommunityId();
    UpdateSave();
    m_resetRateSeq = RESET_RATE_END;
  }
  else
  {
    // エラーアプレット呼び出し
    NetLib::Error::NijiNetworkError error;
    error.SetResultNex( result );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
    ExitSelf(RESULT_RatingAfterRuleSelect_Back);
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
bool Frame_RatingAfterRuleSelect::OnSuspendWithHomeSleepAndPower( void )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
#endif
  return cleanup();
}

} /*  namespace Live    */
} /*  namespace NetApp  */



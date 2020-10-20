//==============================================================================
/**
 * @file   BattleSpotFrame_UpdateDisonnectCount.cpp
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

#include  "BattleSpotFrame_UpdateDisconnectCount.h"
#include  "GameSys/include/GameData.h"
#include <Savedata/include/MyStatus.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_UpdateRank,
    STATE_Ready,
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
  *  @func     Frame_UpdateDisconnectCount
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_UpdateDisconnectCount::Frame_UpdateDisconnectCount(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_seq(0)
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
  *  @func     Frame_UpdateDisconnectCount
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_UpdateDisconnectCount::~Frame_UpdateDisconnectCount()
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
bool  Frame_UpdateDisconnectCount::startup()
{
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
void  Frame_UpdateDisconnectCount::setListener()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_UpdateDisconnectCount::removeListener()
{
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_UpdateDisconnectCount::updateSequence()
{
  switch(m_State)
  {
  case  STATE_Entry:
    {
      if( UpdateSave() )
      {
        if( m_pWork->GetRule() == BattleSpotWork::RULE_FREE_SINGLE || 
            m_pWork->GetRule() == BattleSpotWork::RULE_FREE_DOUBLE || 
            m_pWork->GetRule() == BattleSpotWork::RULE_FREE_ROYAL
          )
        {//フリー
          //@fix NMCat[#4104 フリーバトルでマッチング直後に通信切断すると切断フラグが増加しない]
          if( m_pWork->m_pAppParam->nextApp != NEXT_APP_BATTLE )
          {
            m_State = STATE_UpdateRank;
          }
          else
          {//バトル前は切断フラグ送信をここより前にやっている
            m_State = STATE_Ready;
          }
        }
        else
        {//フリー以外
          m_State = STATE_Ready;
        }
      }
    }
    break;
  case STATE_UpdateRank:
  {
    UpdateRankData();
  }
    break;
    case  STATE_Ready:
        exitFrame(RESULT_UpdateDisconnectCount_Next);
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
bool  Frame_UpdateDisconnectCount::cleanup()
{
  bool  bAllDone = false;

  if( FinalizeRanking() == false )
  {
    return false;
  }

  bAllDone  = true;

  m_pWork->m_pAppParam->m_bgm = m_pWork->m_pViewContainer->m_pLowerView_Card->GetSelectedBGM();//BGM設定
  
  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_UpdateDisconnectCount::ExitSelf(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}



void Frame_UpdateDisconnectCount::UpdateRankData()
{
#if defined(GF_PLATFORM_CTR)
  switch( m_seq ){
  ///< 共通データ更新
  case RANK_SEQ_COMMON_DATA_UPDATE:
    {

      #if PM_DEBUG
      //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"RankingClient::UploadCommonData(nexUniq:%llu) \n", m_nexUniqueId );
      #endif

      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      bool ret = m_rankingClient->UploadCommonData( &m_pWork->GetCommonData(), sizeof(COMMON_DATA), game_data->GetPlayerStatus()->GetNexUniqueID() );
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
    m_seq = RANK_SEQ_ERROR_WAIT;
    break;

  case RANK_SEQ_ERROR_WAIT:           ///< エラーメッセージ待ち
    break;

  case RANK_SEQ_EXIT:               ///< 破棄
    m_seq = RANK_SEQ_END;
    break;

  case RANK_SEQ_END:                ///< 終了

    m_State =  STATE_Ready;

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
bool Frame_UpdateDisconnectCount::InitializeRanking( void )
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
bool Frame_UpdateDisconnectCount::FinalizeRanking( void )
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
void Frame_UpdateDisconnectCount::OnNexRankingUploadCommonDataSuccess( void )
{
  GFL_PRINT("OnNexRankingUploadCommonDataSuccess\n");
  
  // アップロードしたらどのモードでも終了
  m_seq = RANK_SEQ_EXIT;
}



//------------------------------------------------------------------
/**
 * @brief NexGlobalTradeStationClient通信のエラー通知
 *
 * @param[in] result     実行結果
 * @param[in] errorCode  エラーアプレット用エラーコード
 */
//------------------------------------------------------------------
void Frame_UpdateDisconnectCount::OnNexRankingClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("OnNexRankingClientError %d\n", errorCode );

  {
    // エラーアプレット呼び出し
    NetLib::Error::NijiNetworkError error;
    error.SetResultNex( result );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  }

  m_seq = RANK_SEQ_EXIT;
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
bool Frame_UpdateDisconnectCount::OnSuspendWithHomeSleepAndPower( void )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
#endif
  return cleanup();
}

} /*  namespace Live    */
} /*  namespace NetApp  */



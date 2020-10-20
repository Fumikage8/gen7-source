#if !defined(BATTLESPOTFRAME_RATINGBATTLEMATCHING_H_INCLUDED)
#define BATTLESPOTFRAME_RATINGBATTLEMATCHING_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_RatingBattleMatching.h
 * @date   2015/10/02 Fri. 13:31:47
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include <gflnet2/include/nex/gflnet2_NexRankingClient.h>
#include <gflnet2/include/nex/gflnet2_NexRankingClientListener.h>

#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../BattleSpotFrameDef.h"
#include  "../Views/BattleSpotViewContainer.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransfer.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransferListener.h"
#include <System/include/SystemEventListener.h>
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusResponseListener.h"

namespace NetApp    {
namespace BattleSpot {
  class BattleSpotWork;
  class BattleSpotViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief
  */
//==============================================================================
class Frame_RatingBattleMatching
  : public    NetAppLib::System::NetAppFrameBase
  , public    System::SystemEventListener
  , public    NetLib::P2P::P2pPacketTransferListener
  , public NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener
#if defined(GF_PLATFORM_CTR)
  , public    gflnet2::nex::NexRankingClientListener
#endif
  , protected LowerView_Card::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_RatingBattleMatching );
public:

  enum
  {
    MATCHING_INIT = 0,
    MATCHING_WAIT,
    DISCONNECT_WAIT,
    SEND_RECV_WAIT,
    SEND_RECV_END,
    MATCHING_ERROR,
  };

  enum{
    // 共通データ取得
    RANK_SEQ_RATE_DATA_GET = 0,        ///< レートデータ取得
    RANK_SEQ_RATE_DATA_GET_SUCCESS,///< レートデータ取得
    RANK_SEQ_DATA_GET_NOT_FOUND,   ///< データなし

    RANK_SEQ_COMMON_DATA_GET,      ///< 共通データ取得
    RANK_SEQ_COMMON_DATA_RESET,    ///< 共通データリセット

    RANK_SEQ_COMMON_DATA_UPLOAD,   ///< 共通データ登録

    RANK_SEQ_RANK_DATA_INITIALIZATION, ///< 成績データ初期化

    RANK_SEQ_WAIT,                 ///< 通信待ち
    RANK_SEQ_EXIT,                 ///< 破棄

    RANK_SEQ_SAVE,                 ///< セーブ
    RANK_SEQ_SAVE_WAIT,            ///< セーブ待ち

    RANK_SEQ_ERROR_RETURN,         ///< エラー
    RANK_SEQ_ERROR_WAIT,           ///< エラーメッセージ待ち

    RANK_SEQ_END,                  ///< 終了
  };

  enum
  {
    SEND_COMMAND = 20,
    BATTLE_INIT_COUNT = 10
  };

public:
  Frame_RatingBattleMatching(BattleSpotWork* pWork);
  virtual ~Frame_RatingBattleMatching();

  /*  implement FrameCellSuper I/F  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
//  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*  end implement FrameCellSuper I/F  */

protected:
//  virtual void  exitFrame(NetAppLib::System::FrameResult result );

private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  implement LowerView_Card_OnEvent::IEventHandler  */
  virtual void  LowerView_Card_OnEvent(LowerView_Card* pSender, const EventCode eventCode);

  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer );

  virtual void OnConfirmPlayStatusSuccess();
  virtual void OnConfirmPlayStatusError( u32 errorCode );

#if defined(GF_PLATFORM_CTR)
  virtual void OnConfirmPlayStatusError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult ){}
#endif // defined(GF_PLATFORM_CTR)

  void  ExitSelf(const FrameResult frameResult = RESULT_RatingBattleEntry_Back);

  bool UpdateMatching();
  bool ReadyWait();

  void UpdateRankData();

  bool InitializeRanking();
  bool FinalizeRanking();

public:

#if defined(GF_PLATFORM_CTR)

  //------------------------------------------------------------------
  /**
  * @brief スコアアップロードの成功通知
  *
  * @param[in] category  対象カテゴリID
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingUploadScoreSuccess( const u32 category );

  //------------------------------------------------------------------
  /**
  * @brief スコア削除の成功通知
  *
  * @param[in] category  対象カテゴリID
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingDeleteScoreSuccess( const u32 category );

  //------------------------------------------------------------------
  /**
  * @brief 共通情報アップロードの成功通知
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingUploadCommonDataSuccess( void );


  //------------------------------------------------------------------
  /**
  * @brief ランキング共通データの取得成功通知
  *
  * @param[in] data ランキング共通データ
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingGetCommonDataSuccess( const nn::nex::qVector<qByte>& data );

  virtual void OnNexRankingGetListDataSuccess( const nn::nex::RatingRankData& data );
  virtual void OnNexRankingClientError( const nn::nex::qResult& result, u32 errorCode );
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
  virtual bool OnSuspendWithHomeSleepAndPower( void );

private:

  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  /*  views  */
  UpperView_Information*           m_pUpperView;
  LowerView_Card*      m_pLowerView;

  u32 m_matchingState;

  NetLib::P2P::P2pPacketTransfer* m_pP2pPacketTransfer;

  BattleSpotWork::SYNC_DATA m_syncData;


#if defined(GF_PLATFORM_CTR)
  gflnet2::nex::NexRankingClient* m_rankingClient;
#endif
  u32 m_seq;//ランキングseq

  NetAppLib::Util::NetAppTimeout                                m_CountDownTimer;

  u32 m_matchingCount;
  bool m_isRetryExists;
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_RATINGBATTLEMATCHING_H_INCLUDED)  */

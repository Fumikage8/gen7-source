#if !defined(BATTLESPOTFRAME_UPDATEDISCONNECTCOUNT_H_INCLUDED)
#define BATTLESPOTFRAME_UPDATEDISCONNECTCOUNT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_UpdateDisonnectCount.h
 * @date   2015/10/02 Fri. 13:32:33
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
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../BattleSpotFrameDef.h"
#include  "../Views/BattleSpotViewContainer.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransfer.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransferListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

#include <gflnet2/include/nex/gflnet2_NexRankingClient.h>
#include <gflnet2/include/nex/gflnet2_NexRankingClientListener.h>

#include <System/include/SystemEventListener.h>

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
class Frame_UpdateDisconnectCount
  : public    NetAppLib::System::NetAppFrameBase
  , public    System::SystemEventListener
#if defined(GF_PLATFORM_CTR)
  , public    gflnet2::nex::NexRankingClientListener
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_UpdateDisconnectCount );

private:

  enum
  {
    MATCHING_INIT = 0,
    MATCHING_WAIT,
    SEND_RECV_WAIT,
    SEND_RECV_END,
    MATCHING_ERROR,
    DISCONNECT_WAIT,
  };

  enum{
    RANK_SEQ_COMMON_DATA_UPDATE = 0,    ///< 共通データ更新

    RANK_SEQ_WAIT,                 ///< 通信待ち
    RANK_SEQ_EXIT,                 ///< 破棄

    RANK_SEQ_ERROR_RETURN,         ///< エラー
    RANK_SEQ_ERROR_WAIT,           ///< エラーメッセージ待ち

    RANK_SEQ_END,                  ///< 終了
  };


public:
  Frame_UpdateDisconnectCount(BattleSpotWork* pWork);
  virtual ~Frame_UpdateDisconnectCount();

protected:
//  virtual void  exitFrame(NetAppLib::System::FrameResult result );

private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();


  void  ExitSelf(const FrameResult frameResult = RESULT_FreeBattleEntry_Back);

  void UpdateRankData();

  bool InitializeRanking();
  bool FinalizeRanking();

#if defined(GF_PLATFORM_CTR)



  //------------------------------------------------------------------
  /**
  * @brief 共通情報アップロードの成功通知
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingUploadCommonDataSuccess( void );


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

  u32                       m_State;
  FrameResult               m_Result;
  u32                       m_seq;

#if defined(GF_PLATFORM_CTR)
  gflnet2::nex::NexRankingClient* m_rankingClient;
#endif

};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_UPDATEDISCONNECTCOUNT_H_INCLUDED)  */

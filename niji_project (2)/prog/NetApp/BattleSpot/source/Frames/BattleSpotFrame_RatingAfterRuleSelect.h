#if !defined(BATTLESPOTFRAME_RATINGAFTERRULESELECT_H_INCLUDED)
#define BATTLESPOTFRAME_RATINGAFTERRULESELECT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_RatingAfterRuleSelect.h
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
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../BattleSpotFrameDef.h"
#include  "../Views/BattleSpotViewContainer.h"

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
class Frame_RatingAfterRuleSelect
  : public    NetAppLib::System::NetAppFrameBase
  , public    System::SystemEventListener
#if defined(GF_PLATFORM_CTR)
  , public    gflnet2::nex::NexRankingClientListener
#endif
  , protected LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_RatingAfterRuleSelect );

  enum
  {
    RESET_RATE_INIT = 0,
    RESET_RATE_WAIT,
    RESET_RATE_END,
  };

public:
  Frame_RatingAfterRuleSelect(BattleSpotWork* pWork);
  virtual ~Frame_RatingAfterRuleSelect();

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

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode);

  void  ExitSelf(const FrameResult frameResult = RESULT_RatingBattleEntry_Back);

  bool UpdateNetwork();

  //--------------------------------------------------------------------------
  /**
   * @brief   コミュニティIDの確認
   * @return  trueで一致した  falseで不一致
   */
  //--------------------------------------------------------------------------
  bool CheckCommunityID( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   プリンシパルIDの確認
   * @return  trueで一致した  falseで不一致
   */
  //--------------------------------------------------------------------------
  bool CheckPid( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   セーブに最新のコミュニティIDとプリンシパルIDをセット
   */
  //--------------------------------------------------------------------------
  void SetNewCommunityId( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   レートの初期化処理
   */
  //--------------------------------------------------------------------------
  bool UpdateResetRate( void );

  bool InitializeRanking();
  bool FinalizeRanking();

#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
  * @brief スコア削除の成功通知
  *
  * @param[in] category  対象カテゴリID
  */
  //------------------------------------------------------------------
  virtual void OnNexRankingDeleteScoreSuccess( const u32 category );
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

  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  /*  views  */
//  UpperView_Demo*           m_pUpperView;
  LowerView_Selection*      m_pLowerView;
  u32 m_resetRateSeq;

#if defined(GF_PLATFORM_CTR)
  gflnet2::nex::NexRankingClient* m_rankingClient;
#endif
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_RATINGAFTERRULESELECT_H_INCLUDED)  */

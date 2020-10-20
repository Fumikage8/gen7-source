// ============================================================================
/*
 * @file NBRRuleSelectorDecideFrame.h
 * @brief 通信対戦受付アプリのルール選択する人を決定プロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECTOR_DECIDE
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_RULESELECTORDECIDE_FRAME_H_INCLUDE )
#define NBR_RULESELECTORDECIDE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoSimple.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerView2Button.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerView2ButtonListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRRuleSelectorDecideFrame :
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerView2ButtonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRRuleSelectorDecideFrame );
public:
  // コンストラクタ
  NBRRuleSelectorDecideFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                              View::NBRParentView* pParentView,
                              NBRNet* pNet );
  // デストラクタ
  virtual ~NBRRuleSelectorDecideFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();
  virtual bool cleanup();

public:
  //------------------------------------------------------------------
  //------------------------------------------------------------------
  // NetRecvListener
  //------------------------------------------------------------------
  /**
   * @brief  通信終了通知
   */
  virtual void OnNBRNetRecvEnd();

  /**
   * @brief     ルール選択者通知
   * @param[in] ルール選択者通知コマンド
   */
  virtual void OnNBRNetRecvRuleSelector( const NBRNetDefine::RuleSelectorCommand command );

  //------------------------------------------------------------------
  /**
   * @brief  ルール選択者が送信者になった通知
   * @param[in]   netId        送信者のNetId
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRuleSelectorIsMe( int netId );

  //------------------------------------------------------------------
  //------------------------------------------------------------------
  // NBRLowerViewReturnButtonListener
  //------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   * @param[out]  buttonId    選んだボタンのボタンID
   */
  virtual void On2ButtonAction( int buttonId );

  /**
   * @brief   戻るボタン通知
   */
  virtual void OnBackButtonAction();

  //------------------------------------------------------------------
  // NBRLowerViewMessageMenuListener
  //------------------------------------------------------------------
  /**
   * @brief   はいボタン通知
   */
  virtual void OnYesButtonAction();

  /**
   * @brief   いいえボタン通知
   */
  virtual void OnNoButtonAction();

private:

  //=================================
  //! シーケンス定義
  //=================================
  enum{
    SEQ_RULE_SELECTOR_SEND,       //! ルール選択者送信
    SEQ_RULE_SELECTOR_TIMING_LOOP,//! ルール選択者決定同期
    SEQ_MESSAGE_WAIT,             //! 最低ウェイト
    SEQ_FIRST,                //! 初期シーケンス
    SEQ_RECV_WAIT,            //! ルール選択者受信待ち
    SEQ_TUTORIAL,             //! ルール選ぶ人への説明
    SEQ_TUTORIAL_WAIT,        //! ルール選ぶ人への説明待ち
    SEQ_RULE_SELECTOR_WAIT,   //! ルール選ぶ人ボタン押下待ち
    SEQ_TIMING_LOOP,          //! 同期待ち
  };

private:

  // 自分がルール選ぶ人を決めるのか？
  bool IsRuleSelector() const;

private:
  // SubView
  View::NBRLowerView2Button*            m_pLowerView;   //! 下画面
  View::NBRUpperViewPlayerInfoSimple*   m_pUpperView;   //! 上画面
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_RULESELECTORDECIDE_FRAME_H_INCLUDE

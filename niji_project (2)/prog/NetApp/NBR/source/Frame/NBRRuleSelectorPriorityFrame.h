// ============================================================================
/*
 * @file NBRRuleSelectorPriorityFrame.h
 * @brief 通信対戦受付アプリのルール選択する人の優先順位決めプロセスです。
          フレームID：NBR_FRAME_ID_RULE_SELECTOR_PRIORITY
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_RULESELECTORPRIORITY_FRAME_H_INCLUDE )
#define NBR_RULESELECTORPRIORITY_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoSimple.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRRuleSelectorPriorityFrame :
  public NBRFrameBase,
  public NBRNetRecvListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRRuleSelectorPriorityFrame );

private:

  //=================================
  //! シーケンス定義
  //=================================
  enum{
    SEQ_NET_INITIALIZE,       //! NBRNetの初期化
    SEQ_NET_INITIALIZE_WAIT,  //! NBRNetの初期化待ち
    SEQ_SEND_PRIORITY,        //! 優先順位送信（親）
    SEQ_RECV_PRIORITY_WAIT,   //! 優先順位受信待ち（子）
    SEQ_TIMING_LOOP,          //! 同期待ち
  };

public:
  // コンストラクタ
  NBRRuleSelectorPriorityFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                                View::NBRParentView* pParentView,
                                NBRNet* pNet );
  // デストラクタ
  virtual ~NBRRuleSelectorPriorityFrame();

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
   * @brief     ルール選ぶ人の候補者優先順位通知（子のみ）
   * @param[in] ルール選ぶ人の候補者優先順位（NetIdが要素で優先度が格納されている）
   */
  virtual void OnNBRNetRecvPriority( const u8 priority[NBRNetDefine::NET_NUM_MAX] );

  //------------------------------------------------------------------
  // NBRLowerViewMessageMenuListener
  //------------------------------------------------------------------
  /**
   * @brief   はいボタン通知
   */
  virtual void OnYesButtonAction(){};

  /**
   * @brief   いいえボタン通知
   */
  virtual void OnNoButtonAction(){};

private:

  // SubView
  View::NBRUpperViewPlayerInfoSimple*   m_pUpperView;   //! 上画面
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_RULESELECTORPRIORITY_FRAME_H_INCLUDE

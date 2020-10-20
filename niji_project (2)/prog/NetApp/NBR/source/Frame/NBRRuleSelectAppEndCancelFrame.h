// ============================================================================
/*
 * @file NBRRuleSelectAppEndCancelFrame.h
 * @brief 通信対戦受付アプリのルール選択アプリでキャンセル戻りしたプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_RULESELECTAPPENDCANCEL_FRAME_H_INCLUDE )
#define NBR_RULESELECTAPPENDCANCEL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRRuleSelectAppEndCancelFrame :
  public NBRFrameBase,
  public NBRNetRecvListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRRuleSelectAppEndCancelFrame );
public:
  // コンストラクタ
  NBRRuleSelectAppEndCancelFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                            View::NBRParentView* pParentView,
                            NBRNet* pNet );
  // デストラクタ
  virtual ~NBRRuleSelectAppEndCancelFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

public:
  //------------------------------------------------------------------
  //------------------------------------------------------------------
  // NetRecvListener
  //------------------------------------------------------------------
  /**
   * @brief  通信終了通知
   */
  virtual void OnNBRNetRecvEnd();

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

  enum {
    SEQ_BUTTON_WAIT,
    SEQ_SEND,
    SEQ_END,
  };

  NetAppLib::System::FrameResult m_result;
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_RULESELECTAPPENDCANCEL_FRAME_H_INCLUDE

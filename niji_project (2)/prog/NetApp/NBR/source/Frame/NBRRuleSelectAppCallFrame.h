// ============================================================================
/*
 * @file NBRRuleSelectAppCallFrame.h
 * @brief 通信対戦受付アプリのルール選択アプリ呼び出しプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_APP_CALL
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_RULESELECTAPPCALL_FRAME_H_INCLUDE )
#define NBR_RULESELECTAPPCALL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRRuleSelectAppCallFrame : public NBRFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRRuleSelectAppCallFrame );
public:
  // コンストラクタ
  NBRRuleSelectAppCallFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                             View::NBRParentView* pParentView,
                             NBRNet* pNet );
  // デストラクタ
  virtual ~NBRRuleSelectAppCallFrame();

private:
  //------------------------------------------------------------------
  // NBRLowerViewMessageMenuListener
  //------------------------------------------------------------------
  // 使わない
  virtual void OnYesButtonAction(){}
  virtual void OnNoButtonAction(){}

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void updateSequence();

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_RULESELECTAPPCALL_FRAME_H_INCLUDE

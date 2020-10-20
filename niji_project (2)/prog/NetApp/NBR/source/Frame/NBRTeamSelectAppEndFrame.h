// ============================================================================
/*
 * @file NBRTeamSelectAppEndFrame.h
 * @brief 通信対戦受付アプリのチーム選択アプリ終了プロセスです。
 *        フレームID:NBR_FRAME_ID_TEAM_SELECT_APP_END
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_TEAMSELECTAPPEND_FRAME_H_INCLUDE )
#define NBR_TEAMSELECTAPPEND_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRTeamSelectAppEndFrame : public NBRFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRTeamSelectAppEndFrame );
public:
  // コンストラクタ
  NBRTeamSelectAppEndFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                            View::NBRParentView* pParentView,
                            NBRNet* pNet );
  // デストラクタ
  virtual ~NBRTeamSelectAppEndFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

public:
  //------------------------------------------------------------------
  // NBRLowerViewMessageMenuListener
  //------------------------------------------------------------------
  /**
   * @brief   はいボタン通知
   */
  virtual void OnYesButtonAction(){}

  /**
   * @brief   いいえボタン通知
   */
  virtual void OnNoButtonAction(){}
private:

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_TEAMSELECTAPPEND_FRAME_H_INCLUDE

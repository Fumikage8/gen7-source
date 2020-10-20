// ============================================================================
/*
 * @file NBRBattleStartFrame.h
 * @brief 通信対戦受付アプリのバトルへプロセスです。
 *        フレームID:NBR_FRAME_ID_BATTLE_START
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_BATTLESTART_FRAME_H_INCLUDE )
#define NBR_BATTLESTART_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRBattleStartFrame : public NBRFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRBattleStartFrame );
private:

  //! 自動で次に進むまでの時間
  static const u32 AUTO_END_WAIT = 3000; // 3秒

public:
  // コンストラクタ
  NBRBattleStartFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                       View::NBRParentView* pParentView,
                       NBRNet* pNet );
  // デストラクタ
  virtual ~NBRBattleStartFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
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

#endif // NBR_BATTLESTART_FRAME_H_INCLUDE

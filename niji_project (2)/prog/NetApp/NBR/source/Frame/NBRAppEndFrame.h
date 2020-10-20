// ============================================================================
/*
 * @file NBRAppEndFrame.h
 * @brief 通信対戦受付アプリのアプリ終了時フレーム
          フレームID：NBR_FRAME_ID_APP_END
 * @date 2015.07.25
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_APPEND_FRAME_H_INCLUDE )
#define NBR_APPEND_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRAppEndFrame :
  public NBRFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRAppEndFrame );

public:
  // コンストラクタ
  NBRAppEndFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                                View::NBRParentView* pParentView,
                                NBRNet* pNet );
  // デストラクタ
  virtual ~NBRAppEndFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void updateSequence();
  virtual bool cleanup();

  // 終了タイプ別行進処理
  void UpdateTypeOneself();
  void UpdateTypeOpponent();

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

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_APPEND_FRAME_H_INCLUDE

// ============================================================================
/*
 * @file NBRLowerViewMessageMenuListener.h
 * @brief 通信対戦受付アプリ　下画面ビューメッセージメニューリスナー
 * @date 2015.07.28
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_MESSAGEMENU_LISTENER_H_INCLUDE )
#define NBR_LOWER_VIEW_MESSAGEMENU_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRLowerViewMessageMenuListener
{
public:

  //--------------------------------------------------------------------------
  /**
   * @brief   はいボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnYesButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   いいえボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnNoButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   マルチ用プレイヤー選択ボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnMutliButtonAction( u32 buttonIndex ){}

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_LOWER_VIEW_MESSAGEMENU_LISTENER_H_INCLUDE

// ============================================================================
/*
 * @file NBRLowerViewReturnButtonListener.h
 * @brief 通信対戦受付アプリ 下画面：戻るボタンのみ　イベントリスナ－
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_RETURNBUTTON_LISTENER_H_INCLUDE )
#define NBR_LOWER_VIEW_RETURNBUTTON_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRLowerViewReturnButtonListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   戻るボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnBackButtonAction() = 0;
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_LOWER_VIEW_RETURNBUTTON_LISTENER_H_INCLUDE

// ============================================================================
/*
 * @file NBRLowerView2ButtonListener.h
 * @brief 通信対戦受付アプリ 下画面：ルール誰が選ぶか画面　イベントリスナ－
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_2BUTTON_LISTENER_H_INCLUDE )
#define NBR_LOWER_VIEW_2BUTTON_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRLowerView2ButtonListener
{
public:

  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   * @param[out]  buttonId    選んだボタンのボタンID
   */
  //--------------------------------------------------------------------------
  virtual void On2ButtonAction( int buttonId ) = 0;

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

#endif // NBR_LOWER_VIEW_2BUTTON_LISTENER_H_INCLUDE

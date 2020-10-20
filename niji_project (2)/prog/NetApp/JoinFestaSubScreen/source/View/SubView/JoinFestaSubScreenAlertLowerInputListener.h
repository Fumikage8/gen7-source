// ============================================================================
/*
 * @file JoinFestaSubScreenAlertLowerInputListener.h
 * @brief ジョインフェスタ下画面のアラート画面の入力イベントを受け取るリスナーです。
 * @date 2016.01.07
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_ALERT_LOWER_INPUTLISTENER_H_INCLUDE )
#define JOINFESTASUBSCREEN_ALERT_LOWER_INPUTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenAlertLowerInputListener
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
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_ALERT_LOWER_INPUTLISTENER_H_INCLUDE

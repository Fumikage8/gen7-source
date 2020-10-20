// ============================================================================
/*
 * @file JoinFestaSubScreenAttractionLowerInputListener.h
 * @brief ジョインフェスタ下画面のAttraction中画面の入力イベントを受け取るリスナーです。
 * @date 2015.11.16
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_ATTRACTION_LOWER_INPUTLISTENER_H_INCLUDE )
#define JOINFESTASUBSCREEN_ATTRACTION_LOWER_INPUTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenAttractionLowerInputListener
{
public:

  //--------------------------------------------------------------------------
  /**
   * @brief   中止ボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnStopButtonAction() = 0;
  
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_ATTRACTION_LOWER_INPUTLISTENER_H_INCLUDE

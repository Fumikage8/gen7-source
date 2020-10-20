// ============================================================================
/*
 * @file JoinFestaSubScreenInvitationLowerInputListener.h
 * @brief ジョインフェスタ下画面のお誘い画面の入力イベントを受け取るリスナーです。
 * @date 2015.10.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_INVITATE_LOWER_INPUTLISTENER_H_INCLUDE )
#define JOINFESTASUBSCREEN_INVITATE_LOWER_INPUTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenInvitationLowerInputListener
{
public:

  //--------------------------------------------------------------------------
  /**
   * @brief   申し込みOKボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnOkButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   申し込みNGボタン（戻るボタン）通知
   */
  //--------------------------------------------------------------------------
  virtual void OnNgButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   お断りリストボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnBlackListButtonAction() = 0;
  
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_INVITATE_LOWER_INPUTLISTENER_H_INCLUDE

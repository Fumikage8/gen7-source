// ============================================================================
/*
 * @file JoinFestaSubScreenSelectLowerInputListener.h
 * @brief ジョインフェスタ下画面の選択画面の入力イベントを受け取るリスナーです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_SELECT_LOWER_INPUTLISTENER_H_INCLUDE )
#define JOINFESTASUBSCREEN_SELECT_LOWER_INPUTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenSelectLowerInputListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   * @param[out]  buttonId    選んだボタンのボタンID
   */
  //--------------------------------------------------------------------------
  virtual void OnButtonAction( int buttonId ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   戻るボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnBackButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   「おことわりリストを　こうしんしました」メッセージ終了通知
   */
  //--------------------------------------------------------------------------
  virtual void OnBlackListUpdateMessageFinish(){}

  //--------------------------------------------------------------------------
  /**
   * @brief   「つうしんが　ちゅうだんされました」メッセージ終了通知
   */
  //--------------------------------------------------------------------------
  virtual void OnNetworkErrorMessageFinish(){}
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_SELECT_LOWER_INPUTLISTENER_H_INCLUDE

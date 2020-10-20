// ============================================================================
/*
 * @file NBRLowerViewTeamSelectListener.h
 * @brief 通信対戦受付アプリ 下画面：チーム選択画面　イベントリスナ－
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_TEAMSELECT_LISTENER_H_INCLUDE )
#define NBR_LOWER_VIEW_TEAMSELECT_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRLowerViewTeamSelectListener
{
public:
  
  //--------------------------------------------------------------------------
  /**
   * @brief   対戦チームボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnTeamButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   参加できないボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnCannnotJoinButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   しょうぶかいしボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnBattleStartButtonAction() = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   BGM選択ボタン押下通知（右左）
   */
  //--------------------------------------------------------------------------
  virtual void OnBgmLButtonAction() = 0;
  virtual void OnBgmRButtonAction() = 0;

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

#endif // NBR_LOWER_VIEW_TEAMSELECT_LISTENER_H_INCLUDE

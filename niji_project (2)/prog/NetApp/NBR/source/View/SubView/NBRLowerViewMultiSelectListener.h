// ============================================================================
/*
 * @file NBRLowerViewMultiSelectListener.h
 * @brief 通信対戦受付アプリ 下画面：マルチのチーム割り振り画面　イベントリスナ－
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_MULTISELECT_LISTENER_H_INCLUDE )
#define NBR_LOWER_VIEW_MULTISELECT_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRLowerViewMultiSelectListener
{
public:
  
  //--------------------------------------------------------------------------
  /**
   * @brief     指定番目のプレイヤー選択ボタン押下通知
   * @param[in] 1～4番目
   */
  //--------------------------------------------------------------------------
  virtual void OnPlayerSelectButtonAction( int no ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   決定ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnDecideButtonAction() = 0;

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

#endif // NBR_LOWER_VIEW_MULTISELECT_LISTENER_H_INCLUDE

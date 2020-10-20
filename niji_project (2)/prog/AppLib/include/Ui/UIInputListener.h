//=============================================================================
/**
 * @file   UIInputListener.h
 * @brief  デバイスインプットイベントリスナークラス
 * @author h.suzuki
 */
//=============================================================================

#ifndef __XY_PROJECT_APP_UI_INPUT_LISTENER_H__
#define __XY_PROJECT_APP_UI_INPUT_LISTENER_H__
#pragma once

#include <ui/include/gfl2_UI.h>

namespace app {
namespace ui {



/************************************************************************************************
*
*   @brief ＵＩイベントリスナ用インターフェースクラス
*
*         UIResponderから通知される入力イベントを検知するためのイベントクラス。
*         各種イベント関数の返却値は、同フレーム内での他の入力イベントを排他制御する為に
*         用いられる。
*
*
************************************************************************************************/
class UIInputListener
{

public:
  enum ListenerResult{
    DISABLE_ACCESS = 0,         //False
    ENABLE_ACCESS,           //True
    MYSUBVIEW_ONLY_ACCESS  // 新しい戻り値 他のUIVIEWのキーを禁止
  };

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タッチパネルイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
  *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch ){ return ENABLE_ACCESS; }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   タッチパネルのダブルクリックイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual ListenerResult OnDoubleClickEvent( gfl2::ui::TouchPanel * pTouchPanel ){ return ENABLE_ACCESS; }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId ){ return ENABLE_ACCESS; }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペイン長押しイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual ListenerResult OnLayoutPaneLongPressEvent( const u32 painId ){ return ENABLE_ACCESS; }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペイン選択アニメーションの開始を検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return none
  */
  //--------------------------------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 painId ){}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return none
  * 
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  * 
  *                 TRIGER判定として使いたい場合は、ここを使わずに
  *                 ボタンマネージャーモードをTRIGGERにして
  *                 OnLayoutPaneEventを使うべき。
  */
  //--------------------------------------------------------------------------------------------
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 paneId ){}

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  キー入力の検知
   *         ※タッチパネルの入力が優先される
   *
   * @param  pButton  ボタン ( A,B,X,Y,L,R... )
   * @param  pKey     十字キー
   * @param  pStick   アナログスティック
   *
   * @return 同フレーム内での他入力イベントを許可するならtrue、
   *         他のイベントを排除するならfalseを返却すること。
   */
  //--------------------------------------------------------------------------------------------
  virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ){ return ENABLE_ACCESS; }
};


}	// ui
}	// app


#endif // __XY_PROJECT_APP_UI_INPUT_LISTENER_H__

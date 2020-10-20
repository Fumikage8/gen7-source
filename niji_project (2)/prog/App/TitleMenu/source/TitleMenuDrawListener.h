#ifndef NIJI_PROJECT_APP_TITLE_MENU_DRAW_LISTENER_H_INCLUDED
#define NIJI_PROJECT_APP_TITLE_MENU_DRAW_LISTENER_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    TitleMenuDrawListener.h
 * @date    2015/07/21 14:51:58
 * @author  fukushima_yuya
 * @brief   タイトルメニュー画面リスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/TitleMenu/include/TitleMenuAppParam.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TitleMenu)

//------------------------------------------------------------------------------
// @brief   DrawListenerクラス
//------------------------------------------------------------------------------
class TitleMenuDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   タイトルメニュー画面の終了通知
   */
  //--------------------------------------------------------------------------
  virtual void EndTitleMenu( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   終了モードのセット
   * @param   endMode   終了モード
   */
  //--------------------------------------------------------------------------
  virtual void SetEndMode( EndMode endMode ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   セーブデータフォーマット開始を通知
   */
  //--------------------------------------------------------------------------
  virtual void OnSavedataFormat( void ) = 0;
};

GFL_NAMESPACE_END(TitleMenu)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_TITLE_MENU_DRAW_LISTENER_H_INCLUDED
#ifndef NIJI_PROJECT_APP_LANG_SELECT_DRAW_LISTENER_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_DRAW_LISTENER_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectDrawListener.h
 * @date 2015/07/23 15:24:41
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include "App/LangSelect/include/LangSelectAppParam.h"
#include "App/LangSelect/source/LangSelectConst.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(LangSelect)

//------------------------------------------------------------------------------
// @brief   DrawListenerクラス
//------------------------------------------------------------------------------
class LangSelectDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   言語選択の開始
   */
  //--------------------------------------------------------------------------
  virtual void StartLangSelect( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   画面モードの設定
   * @param   dispMode    画面モード
   */
  //--------------------------------------------------------------------------
  virtual void SetDispMode( DispMode dispMode ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   言語選択画面の終了通知
   */
  //--------------------------------------------------------------------------
  virtual void EndLangSelect( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   選択した言語
   * @param   langID    言語ID
   * @param   kanji     漢字フラグ
   */
  //--------------------------------------------------------------------------
  virtual void SetLangID( u8 langID, bool kanji ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   セーブデータ削除コマンドの入力
   */
  //--------------------------------------------------------------------------
  virtual void InputSavedataDeleteCommand( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   Aボタン入力
   */
  //--------------------------------------------------------------------------
  virtual void InputDecide( bool input ) { ; }
};

GFL_NAMESPACE_END(LangSelect)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_LANG_SELECT_DRAW_LISTENER_H_INCLUDED
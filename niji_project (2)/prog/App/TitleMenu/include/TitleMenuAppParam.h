#ifndef NIJI_PROJECT_APP_TITLE_MENU_APP_PARAM_H_INCLUDED
#define NIJI_PROJECT_APP_TITLE_MENU_APP_PARAM_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file TitleMenuAppParam.h
 * @date 2015/07/22 12:27:18
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TitleMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//! @brief  初期化モード
enum InitMode
{
  INIT_MODE_NONE,               //!< 初期化モードなし。通常タイトルメニューへ
  INIT_MODE_TITLE,              //!< 初期化モードあり。タイトルから呼び出し
  INIT_MODE_SAVEDATA_BROKEN,    //!< 初期化モードあり。セーブデータチェックから呼び出し
  INIT_MODE_NUM,
};

//! @brief  終了モード
enum EndMode
{
  END_MODE_SELECT_CONTINUE,     //!< つづきからを選択
  END_MODE_SELECT_PRESENT,      //!< ふしぎな贈り物を選択
  END_MODE_SELECT_LIVE_CUP,     //!< ライブ大会を選択
  END_MODE_SELECT_DLC_SHOP,     //!< DLCショップを選択
  END_MODE_CANCEL,              //!< キャンセル
  END_MODE_INIT_SAVE_CANCEL,    //!< セーブデータ初期化キャンセル
  END_MODE_INIT_SAVE_DECIDE,    //!< セーブデータ初期化
  END_MODE_NUM,
};

//------------------------------------------------------------------------------
// @brief   外部設定パラメータ
//------------------------------------------------------------------------------
struct APP_PARAM
{
  // [in]
  InitMode initMode;    //!< 初期化モード

  // [out]
  EndMode   endMode;    //!< 終了モード

  APP_PARAM( void )
    : initMode( INIT_MODE_NONE )
    , endMode( END_MODE_NUM )
  { ; }
};

GFL_NAMESPACE_END(TitleMenu)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_TITLE_MENU_APP_PARAM_H_INCLUDED
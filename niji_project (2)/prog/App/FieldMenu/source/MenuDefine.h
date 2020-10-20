//=============================================================================
/**
 * @file    MenuDefine.h
 * @brief   フィールドメニュー共通定義
 * @author  Hiroyuki Nakamura
 * @date    2015.09.29
 */
//=============================================================================
#if !defined( MENU_DEFINE_H_INCLUDED )
#define MENU_DEFINE_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//! 描画プライオリティ
enum DrawPriority
{
  DRAW_PRI_UPPER_BAR = 0,         //!< 上画面メニューバー
  DRAW_PRI_LOWER_BG,              //!< 下画面背景

  DRAW_PRI_LOWER_NAVI,            //!< ナビ画面マップ
  DRAW_PRI_LOWER_MENU_ICON,       //!< メニューアイコン

  DRAW_PRI_LOWER_COVER,           //!< ロトムカバー

  DRAW_PRI_LOWER_CURSOR,          //!< メニューカーソル

  DRAW_PRI_APP = 128,             //!< 個別アプリ
};

//! 表示モード
enum DispMode
{
  DISP_MODE_NORMAL = 0,   //!< 通常モード
  DISP_MODE_FRAME_OFF,    //!< 枠なし
  DISP_MODE_SLEEP,        //!< スリープモード
};

GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // MENU_DEFINE_H_INCLUDED

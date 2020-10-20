#ifndef NIJI_PROJECT_APP_HERO_SELECT_CONST_H_INCLUDED
#define NIJI_PROJECT_APP_HERO_SELECT_CONST_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file HeroSelectConst.h
 * @date 2015/08/26 14:49:02
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

static const u32 ROW_MAX = 4;   //!< 列
static const u32 COL_MAX = 2;   //!< 行
static const u32 CHARA_MAX = ROW_MAX * COL_MAX;   //!< 選択項目数

//! 画面モード
enum {
  DISP_MODE_TALK,
  DISP_MODE_HERO_SELECT,
  DISP_MODE_YESNO,
  DISP_MODE_NUM,
};

//! YESNOウィンドウの結果
enum WndRes
{
  WND_RES_NONE,   //!< 入力待ち
  WND_RES_YES,    //!< はい
  WND_RES_NO,     //!< いいえ
  WND_RES_NUM,
};

//! ARCID
enum {
  ARC_HERO_SELECT,
  ARC_FIELDWINDOW,
  ARC_MAX,
};

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_HERO_SELECT_CONST_H_INCLUDED
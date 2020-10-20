//======================================================================
/**
 * @file    LayoutDefaultFont.h
 * @brief   レイアウトエディタで指定されるデフォルトフォント
 * @author  ariizumi
 * @author  Hiroyuki Nakamura
 * @date    12/07/20
 *
 * @li  sangoのlayout_default_font.hを移植
 */
//======================================================================
#if !defined( LAYOUT_DEFAULT_FONT_H_INCLUDED )
#define LAYOUT_DEFAULT_FONT_H_INCLUDED

#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(print)

//-----------------------------------------------------------------------------
/**
 *    LayoutSystem　デフォルトフォントの設定インデックス
 */
//-----------------------------------------------------------------------------
#include <niji_reference_files/font/LayoutDefaultFontDefine.h>

extern const char *const LAYOUT_DEFAULT_FONT;
extern const char *const LAYOUT_NUM_FONT;
extern const char *const LAYOUT_BATTLE_FONT;
extern const char *const LAYOUT_TWHK_FONT;
//extern const char *const LAYOUT_TENJI_FONT;

GFL_NAMESPACE_END(print)

#endif  // LAYOUT_DEFAULT_FONT_H_INCLUDED

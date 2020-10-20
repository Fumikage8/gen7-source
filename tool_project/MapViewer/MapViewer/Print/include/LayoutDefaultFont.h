//======================================================================
/**
 * @file    LayoutDefaultFont.h
 * @brief   ���C�A�E�g�G�f�B�^�Ŏw�肳���f�t�H���g�t�H���g
 * @author  ariizumi
 * @author  Hiroyuki Nakamura
 * @date    12/07/20
 *
 * @li  sango��layout_default_font.h���ڐA
 */
//======================================================================
#if !defined( LAYOUT_DEFAULT_FONT_H_INCLUDED )
#define LAYOUT_DEFAULT_FONT_H_INCLUDED

#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(print)

//-----------------------------------------------------------------------------
/**
 *    LayoutSystem�@�f�t�H���g�t�H���g�̐ݒ�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
#include <niji_reference_files/font/LayoutDefaultFontDefine.h>

extern const char *const LAYOUT_DEFAULT_FONT;
extern const char *const LAYOUT_NUM_FONT;
extern const char *const LAYOUT_BATTLE_FONT;
//extern const char *const LAYOUT_TENJI_FONT;

GFL_NAMESPACE_END(print)

#endif  // LAYOUT_DEFAULT_FONT_H_INCLUDED

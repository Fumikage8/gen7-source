#if !defined( __NUMBER_INPUT_DEF_H_INCLUDED__ )
#define __NUMBER_INPUT_DEF_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    NumberInputDef.h
 * @date    2015/11/24 12:40:50
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：定数定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/NumberInput/include/NumberInputAppParam.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(NumberInput)

static const u32 INPUT_PART_CODE_MAX  = ALPHABET_INPUT_MAX;                     //!< 入力コード数(１パート)
static const u32 INPUT_PART_MAX       = PART_MAX;                               //!< 入力パート数
static const u32 INPUT_MAX            = INPUT_PART_CODE_MAX * INPUT_PART_MAX;   //!< 全体の入力総数(アルファベットモード)
static const u32 INPUT_NUM_MAX        = INPUT_MAX - 2;                          //!< 全体の入力総数(アルファベットモード)

static const s32 INPUT_DEFAULT_CODE   = -1;       //!< 入力コードなし

static const u32 CODE_BUTTON_MAX      = 36;                         //!< 文字のボタン数
static const u32 BUTTON_DATA_MAX      = CODE_BUTTON_MAX + 2;        //!< ボタンデータの数
static const u32 CURSOR_DATA_MAX      = CODE_BUTTON_MAX + 2 * 5;    //!< カーソルデータの数

GFL_NAMESPACE_END(NumberInput)
GFL_NAMESPACE_END(App)

#endif // __NUMBER_INPUT_DEF_H_INCLUDED__

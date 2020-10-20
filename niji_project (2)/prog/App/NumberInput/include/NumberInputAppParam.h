#if !defined( __NUMBER_INPUT_APP_PARAM_H_INCLUDED__ )
#define __NUMBER_INPUT_APP_PARAM_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    NumberInputAppParam.h
 * @date    2015/11/24 11:53:00
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：外部設定パラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_Str.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( NumberInput )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 ALPHABET_INPUT_MAX = 4;    //!< アルファベット入力数(1パート)
static const u32 PART_MAX = 4;              //!< 入力パート数

enum {
  CALL_MODE_NUMBER = 0,             //!< モード：数字のみ @note nijiでは使用しない。使用する場合は[0]埋めの対応を見直さないとSetEndParamで不具合が発生するので注意。
  CALL_MODE_ALPHABET,               //!< モード：アルファベット込み
  CALL_MODE_NUM,

  CALL_TYPE_BATTLE_RECORDER = 0,    //!< タイプ：バトルレコーダー
  CALL_TYPE_WONDER_GIFT,            //!< タイプ：不思議な贈り物
  CALL_TYPE_NUM,
};

//------------------------------------------------------------------------------
// @brief   外部設定パラメータ構造体
//------------------------------------------------------------------------------
struct APP_PARAM
{
  // [in]
  u16     callMode;     //!< 呼び出しモード(数字のみ or アルファベット込み)
  u16     callType;     //!< 呼び出しタイプ(場面)

  // [in / out]
  u64     inputValue;   //!< 入力された数値

  gfl2::str::STRCODE inputStr[PART_MAX][ALPHABET_INPUT_MAX + 1];   //!< 入力された文字コード

  APP_PARAM( void )
    : callMode( CALL_MODE_ALPHABET )
    , callType( CALL_TYPE_NUM )
    , inputValue( 0 )
  { ; }
};

GFL_NAMESPACE_END( NumberInput )
GFL_NAMESPACE_END( App )

#endif // __NUMBER_INPUT_APP_PARAM_H_INCLUDED__
#ifndef NIJI_PROJECT_APP_STRINPUT_DEF_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_DEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputDef.h
 * @brief   文字入力画面共通定義
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_def.hから移植
 */
//==============================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  App::StrInput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


static const u32 INPUT_MAX = 16;                //!< 入力文字数
static const u32 INPUT_AREA_MAX = INPUT_MAX+1;  //!< 入力エリア数(+挿入位置分)


//! @brief  入力モード
enum {
  INPUT_MODE_AIUEO = 0,     //!< 50音入力
  INPUT_MODE_KEYBOARD,      //!< キーボード入力
  INPUT_MODE_MAX
};

//! @brief  入力タイプ
enum {
  INPUT_TYPE_HIRAGANA = 0,    //!< かな
  INPUT_TYPE_KATAKANA,        //!< カナ
  INPUT_TYPE_ABC,             //!< ABC
  INPUT_TYPE_MARK,            //!< 記号
  INPUT_TYPE_MAX,             //!< 50音入力最大数

  INPUT_TYPE_KEYBOARD_NORMAL = 0,     //!< 通常キーボード
  INPUT_TYPE_KEYBOARD_SHIFT,          //!< シフトキーボード
  INPUT_TYPE_KEYBOARD_HANGUL,         //!< ハングル通常キーボード
  INPUT_TYPE_KEYBOARD_HANGUL_SHIFT,   //!< ハングルシフトキーボード
  INPUT_TYPE_KEYBOARD_MAX,            //!< キーボードタイプ最大数

  // ローカライズ用
  INPUT_TYPE_CAPITAL = INPUT_TYPE_HIRAGANA,   //!< 大文字
  INPUT_TYPE_SMALL = INPUT_TYPE_KATAKANA,     //!< 小文字

  INPUT_TYPE_HANGUL = INPUT_TYPE_HIRAGANA     //!< ハングル
};


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_DEF_H_INCLUDED

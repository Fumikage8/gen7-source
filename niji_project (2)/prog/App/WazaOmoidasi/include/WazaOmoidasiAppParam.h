//=============================================================================
/**
 * @file    WazaOmoidasiAppParam.h
 * @brief   技思い出し画面外部設定パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2015.12.08
 */
//=============================================================================
#if !defined( WAZA_OMOIDASI_APP_PARAM_H_INCLUDED )
#define WAZA_OMOIDASI_APP_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pmlib
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(WazaOmoidasi)

//! 終了モード
enum EndMode
{
  END_DECIDE = 0,   //!< 技を思い出した
  END_CANCEL,       //!< キャンセル
  END_CHANGE,       //!< 技忘れへ
};

//! 外部設定パラメータ
struct APP_PARAM
{
  // [in]
  pml::pokepara::PokemonParam * pp;   //!< ポケモンパラメータ
  u32 delete_pos;                     //!< 忘れる技の位置

  // [in/out]
  u16 list_pos;       //!< リストのカーソル位置
  u16 list_scroll;    //!< リストのスクロール値

  // [out]
  EndMode end_mode;   //!< 終了モード
  WazaNo select_no;   //!< 選択した技（キャンセル時はWAZANO_NULL）
};

GFL_NAMESPACE_END(WazaOmoidasi)
GFL_NAMESPACE_END(App)

#endif  // WAZA_OMOIDASI_APP_PARAM_H_INCLUDED

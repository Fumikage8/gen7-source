//=============================================================================
/**
 * @file    ParurePokeListAppParam.h
 * @brief   ポケパルレポケモン選択画面外部設定パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2015.11.12
 */
//=============================================================================
#if !defined( PARURE_POKELIST_APP_PARAM_H_INCLUDED )
#define PARURE_POKELIST_APP_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(PokeParure)

//! 外部設定パラメータ
struct APP_PARAM
{
  // [in/out]
  u32 select_pos;   //!< IN: 選択中の位置 OUT: 選択位置（キャンセル時はpml::PokeParty::MAX_MEMBERS）
};

GFL_NAMESPACE_END(PokeParure)
GFL_NAMESPACE_END(App)

#endif  // PARURE_POKELIST_APP_PARAM_H_INCLUDED

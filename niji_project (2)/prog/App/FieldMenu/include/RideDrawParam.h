//=============================================================================
/**
 * @file    RideDrawParam.h
 * @brief   ライドポケモン選択画面パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2016.03.30
 */
//=============================================================================
#if !defined( RIDE_DRAW_PARAM_H_INCLUDED )
#define RIDE_DRAW_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/FieldMenuSave.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//! 外部設定パラメータ
struct RIDE_PARAM
{
  Savedata::FieldMenu::RideID on_id;  //!< 使用中のライド
  Field::RIDE_CHECK_RESULT check[Savedata::FieldMenu::RIDE_ID_MAX];   //!< 各ライドの使用可否
};

GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // RIDE_DRAW_PARAM_H_INCLUDED

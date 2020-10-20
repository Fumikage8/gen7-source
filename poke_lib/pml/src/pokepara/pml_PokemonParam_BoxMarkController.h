//=============================================================================
/**
 * @file   pml_PokemonParam_BoxMarkController.h
 * @date   2015/09/15 18:55:08
 * @author obata_toshihiro
 * @brief  ポケモンパラメータの「ボックスマーク」フィールドの解釈
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined PML_POKEMONPARAM_BOXMARKCONTROLLER_H_INCLUDED
#define PML_POKEMONPARAM_BOXMARKCONTROLLER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "pml/include/pokepara/pml_PokemonParamType.h"

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )


  /**
   * @class BoxMarkController
   * @brief ポケモンパラメータの「ボックスマーク」フィールドの解釈を行うクラス
   */
  class BoxMarkController
  {
    GFL_FORBID_COPY_AND_ASSIGN( BoxMarkController );

  public:

    /**
     * @brief ボックスマークの色を取得する
     * @param value  ポケモンパラメータの「ボックスマーク」フィールドの値
     * @param mark   取得対象ボックスマークの種類
     */
    static BoxMarkColor GetBoxMarkColor( u16 value, BoxMark mark );

    /**
     * @brief ボックスマークの色を設定する
     * @param value  設定前のポケモンパラメータの「ボックスマーク」フィールドの値
     * @param mark   設定対象ボックスマークの種類
     * @param color  設定する色
     * @return  設定後のポケモンパラメータの「ボックスマーク」フィールドの値
     */
    static u16 SetBoxMarkColor( u16 value, BoxMark mark, BoxMarkColor color );

  };


GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )

#endif // PML_POKEMONPARAM_BOXMARKCONTROLLER_H_INCLUDED
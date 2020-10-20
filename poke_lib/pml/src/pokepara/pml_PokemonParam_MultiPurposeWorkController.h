//=============================================================================
/**
 * @file   pml_PokemonParam_MultiPurposeWorkController.h
 * @date   2015/09/15 12:27:37
 * @author obata_toshihiro
 * @brief  ポケモンパラメータの「多目的ワーク」の解釈を行うクラス
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined PML_POKEMONPARAM_MULTIPURPOSEWORKCONTROLLER_H_INCLUDED
#define PML_POKEMONPARAM_MULTIPURPOSEWORKCONTROLLER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )


  /**
   * @class MultiPurposeWorkController
   * @brief ポケモンパラメータの「多目的ワーク」の解釈を行うクラス
   */
  class MultiPurposeWorkController
  {
    GFL_FORBID_COPY_AND_ASSIGN( MultiPurposeWorkController );

  public:

    // トリミアン専用
    
    static u8 TORIMIAN_GetMaxKeepDayCount( u32 work );  // 経過日数カウントの最大値を取得する
    static u8 TORIMIAN_GetKeepDayCount( u32 work );     // 経過日数カウントを取得する
    static u8 TORIMIAN_GetBodyHairCount( u32 work );    // 体毛カウントを取得する

    static u32 TORIMIAN_SetMaxKeepDayCount( u32 work, u8 maxKeepDayCount );  // 経過日数カウントの最大値を設定する
    static u32 TORIMIAN_SetKeepDayCount( u32 work, u8 keepDayCount );        // 経過日数カウントを設定する
    static u32 TORIMIAN_SetBodyHairCount( u32 work, u8 bodyHairCount );      // 体毛カウントを設定する

  };


GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )

#endif // PML_POKEMONPARAM_MULTIPURPOSEWORKCONTROLLER_H_INCLUDED
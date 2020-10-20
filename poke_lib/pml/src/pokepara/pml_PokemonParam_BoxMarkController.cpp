//=============================================================================
/**
 * @file   pml_PokemonParam_BoxMarkController.cpp
 * @date   2015/09/15 18:55:08
 * @author obata_toshihiro
 * @brief  ポケモンパラメータの「ボックスマーク」フィールドの解釈
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./pml_PokemonParam_BoxMarkController.h"

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )


namespace
{
  static const u16 BOXMARK_UNIT_MASK      = 0x0003; // １つのボックスマークをマスクするためのビットフラグ
  static const u16 BOXMARK_UNIT_BIT_COUNT = 2;      // １つのボックスマークを表すデータのビット数

} // namespace



//-----------------------------------------------------------------------------
/**
 * @brief ボックスマークの色を取得する
 * @param value  ポケモンパラメータの「ボックスマーク」フィールドの値
 * @param mark   取得対象ボックスマークの種類
 */
 //-----------------------------------------------------------------------------
BoxMarkColor BoxMarkController::GetBoxMarkColor( u16 value, BoxMark mark )
{
  const u16 shift_width   = mark * BOXMARK_UNIT_BIT_COUNT;
  const u16 mask          = BOXMARK_UNIT_MASK << shift_width;
  const u16 masked_value  = ( value & mask );
  const u16 shifted_value = ( masked_value >> shift_width );
  return static_cast<BoxMarkColor>( shifted_value );
}

//-----------------------------------------------------------------------------
/**
 * @brief ボックスマークの色を設定する
 * @param value  設定前のポケモンパラメータの「ボックスマーク」フィールドの値
 * @param mark   設定対象ボックスマークの種類
 * @param color  設定する色
 * @return  設定後のポケモンパラメータの「ボックスマーク」フィールドの値
 */
//-----------------------------------------------------------------------------
u16 BoxMarkController::SetBoxMarkColor( u16 value, BoxMark mark, BoxMarkColor color )
{
  u16 shift_width   = mark * BOXMARK_UNIT_BIT_COUNT;
  u16 add_bit       = static_cast<u16>( color ) << shift_width;
  u16 target_bit    = BOXMARK_UNIT_MASK << shift_width;
  u16 cleared_value = ( value & ~target_bit );
  u32 added_value   = cleared_value | add_bit;
  return added_value;
}



GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )
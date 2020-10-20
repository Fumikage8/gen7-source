//=============================================================================
/**
 * @file   pml_PokemonParam_MultiPurposeWorkController.cpp
 * @date   2015/09/15 12:27:37
 * @author obata_toshihiro
 * @brief  ポケモンパラメータの「多目的ワーク」の解釈を行うクラス
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./pml_PokemonParam_MultiPurposeWorkController.h"

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )


namespace
{

  // トリミアン専用データをマスクするためのビットフラグ
  static const u32 TORIMIAN_MASK_MAX_KEEP_DAY_COUNT = 0x00ff0000; // 経過日数カウントの最大値
  static const u32 TORIMIAN_MASK_KEEP_DAY_COUNT     = 0x0000ff00; // 経過日数カウント
  static const u32 TORIMIAN_MASK_BODY_HAIR_COUNT    = 0x000000ff; // 体毛カウント


  /**
   * @brief マスクした値を正しく解釈するためのシフト幅を取得する
   * @param マスクするためのビットフラグ
   */
  u32 GetShiftWidthByMask( u32 mask )
  {
    for( u32 i=0; i<32; ++i )
    {
      const u32 bit = ( 1 << i );
      if( ( mask & bit ) != 0 )
      {
        return i;
      }
    }

    return 0;
  }

  /**
   * @brief マスクした値を u8 で解釈した値を取得する
   * @param value  マスクする前の値
   * @param mask   適用するマスク
   */
  u8 GetMaskedValue_u8( u32 value, u32 mask )
  {    
    const u32 shiftWidth   = GetShiftWidthByMask( mask );
    const u32 maskedValue  = ( value & mask ); 
    const u32 shiftedValue = ( maskedValue >> shiftWidth );
    return static_cast<u8>( shiftedValue );
  }

  /**
   * @brief u8 の値を、指定したマスクの位置に設定する
   * @param oldValue  設定する前の値
   * @param mask      適用する値の格納位置を表すマスク
   * @param setValue  設定するする値
   * @return  設定後の値
   */
  u32 SetMaskedValue_u8( u32 oldValue, u32 mask, u8 setValue )
  {
    const u32 shiftWidth      = GetShiftWidthByMask( mask );
    const u32 shiftedSetValue = ( setValue << shiftWidth );
    const u32 maskedSetValue  = ( shiftedSetValue & mask );
    const u32 clearedOldValue = oldValue & ( ~mask );
    const u32 newValue        = ( clearedOldValue | maskedSetValue );
    return newValue;
  }


} // namespace



//-----------------------------------------------------------------------------
/**
 * @brief トリミアン専用：経過日数カウントの最大値を取得する
 * @param work  多目的ワーク
 */
//-----------------------------------------------------------------------------
u8 MultiPurposeWorkController::TORIMIAN_GetMaxKeepDayCount( u32 work )
{
  return GetMaskedValue_u8( work, TORIMIAN_MASK_MAX_KEEP_DAY_COUNT );
}

//-----------------------------------------------------------------------------
/**
 * @brief トリミアン専用：経過日数カウントの最大値を設定する
 * @param work             設定前の多目的ワーク
 * @param maxKeepDayCount  設定する経過日数カウントの最大値
 * @return  設定後の多目的ワーク
 */
//-----------------------------------------------------------------------------
u32 MultiPurposeWorkController::TORIMIAN_SetMaxKeepDayCount( u32 work, u8 maxKeepDayCount )
{
  return SetMaskedValue_u8( work, TORIMIAN_MASK_MAX_KEEP_DAY_COUNT, maxKeepDayCount );
}

//-----------------------------------------------------------------------------
/**
 * @brief トリミアン専用：経過日数カウントを取得する
 * @param work  多目的ワーク
 */
//-----------------------------------------------------------------------------
u8 MultiPurposeWorkController::TORIMIAN_GetKeepDayCount( u32 work )
{
  return GetMaskedValue_u8( work, TORIMIAN_MASK_KEEP_DAY_COUNT );
}

//-----------------------------------------------------------------------------
/**
 * @brief トリミアン専用：経過日数カウントを設定する
 * @param work          設定前の多目的ワーク
 * @param keepDayCount  設定する経過日数カウント
 * @return  設定後の多目的ワーク
 */
//-----------------------------------------------------------------------------
u32 MultiPurposeWorkController::TORIMIAN_SetKeepDayCount( u32 work, u8 keepDayCount )
{
  return SetMaskedValue_u8( work, TORIMIAN_MASK_KEEP_DAY_COUNT, keepDayCount );
}

//-----------------------------------------------------------------------------
/**
 * @brief トリミアン専用：体毛カウントを取得する
 * @param work  多目的ワーク
 */
//-----------------------------------------------------------------------------
u8 MultiPurposeWorkController::TORIMIAN_GetBodyHairCount( u32 work )
{
  return GetMaskedValue_u8( work, TORIMIAN_MASK_BODY_HAIR_COUNT );
}

//-----------------------------------------------------------------------------
/**
 * @brief トリミアン専用：体毛カウントを設定する
 * @param work           設定前の多目的ワーク
 * @param bodyHairCount  設定する体毛カウント
 * @return  設定後の多目的ワーク
 */
//-----------------------------------------------------------------------------
u32 MultiPurposeWorkController::TORIMIAN_SetBodyHairCount( u32 work, u8 bodyHairCount )
{
  return SetMaskedValue_u8( work, TORIMIAN_MASK_BODY_HAIR_COUNT, bodyHairCount );
}


GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )
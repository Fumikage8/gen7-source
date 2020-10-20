/**
* @brief  フィールドの地形ブロック関連の定義
* @file   FieldTerrainBlockTypes.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINBLOCKTYPES_H_INCLUDED__ )
#define __FIELDTERRAINBLOCKTYPES_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );
GFL_NAMESPACE_BEGIN( Block );

/** @brief  地形ブロックのアニメーションスロット用途 */
struct AnimationSlotNo
{
  enum Value
  {
    LOCAL = 0,  // ローカルループアニメーション
    ONE_DAY,    // 1日同期アニメーション
    MAX
  };
};


GFL_NAMESPACE_END( Block );
GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINBLOCKTYPES_H_INCLUDED__
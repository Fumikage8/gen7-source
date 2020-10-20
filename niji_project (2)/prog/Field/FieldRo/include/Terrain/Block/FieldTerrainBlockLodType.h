/**
* @brief  フィールド地形ブロックのLOD識別子定義
* @file   FieldTerrainBlockLodType.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINLODTYPE_H_INCLUDED__ )
#define __FIELDTERRAINLODTYPE_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );
GFL_NAMESPACE_BEGIN( Block );

/** @brief  地形ブロックのLOD識別子 */
struct LodType
{
  enum Value
  {
    HIGH = 0,
    MIDDLE,
    LOW,
    MAX
  };
};

GFL_NAMESPACE_END( Block );
GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINLODTYPE_H_INCLUDED__
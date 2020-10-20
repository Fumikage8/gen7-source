/**
* @brief  フィールドの地形関連の定義
* @file   FieldTerrainTypes.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINTYPES_H_INCLUDED__ )
#define __FIELDTERRAINTYPES_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

// エリアデータ内部のデータ構成定義
struct AreaResourceDataIndex
{
  enum Value
  {
    TEXTURE,                // テクスチャ  ※highモデルを含む、全ての地形ブロックと遠景モデルが使用するテクスチャがパックされている
    SHADER,                 // シェーダー  ※highモデルを含む、全ての地形ブロックと遠景モデルが使用するシェーダーがパックされている
    MODEL_MIDDLE,           // 地形ブロック、middleモデルデータ
    MODEL_LOW,              // 地形ブロック、lowモデルデータ
    MODEL_DISTANT_VIEW,     // 遠景モデルデータ
    ANIME_LOCAL,            // ローカルループアニメーション
    ANIME_ONE_DAY,          // 1日同期アニメーション 
    MAX
  };
};

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINTYPES_H_INCLUDED__
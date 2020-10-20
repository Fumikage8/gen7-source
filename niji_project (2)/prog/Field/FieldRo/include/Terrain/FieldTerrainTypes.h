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
    SHADE_AREA,             // 影領域データ
    MAX
  };
};

// 壁のバイナリ種別 @note バイナリデータの格納順になっているので注意
// この定義を追加する場合はあわせて下記を行う
// ・地形ブロックのコンバーターのファイル収集処理に対応するファイル命名規則を追加
struct WallBinIndex
{
  enum Value
  {
    NORMAL = 0,             // 通常の壁
    NAMINORI,               // 波乗り壁
    RIDE,                   // ライドに乗っていないときのみ通れる壁
    ROBA2,                  // 地面馬のみが通れる壁
    MAX
  };
};

// 壁の種別 
// この定義を追加する場合はあわせて下記を行う
// ・元となるバイナリデータの設定の追加(後述のsc_WallIndexToBinIndex)
// ・登録するシーン設定の追加(地形マネージャの初期化設定)
struct WallIndex
{
  enum Value
  {
    NORMAL = 0,             // 通常の壁
    OSHIKAESHI_NAMINORI,    // 波乗り壁(押し返し)
    EVENT_NAMINORI,         // 波乗り壁(イベント)
    OSHIKAESHI_ROBA2,       // 壁(地面馬のみが通れる壁)
    OSHIKAESHI_RIDE,        // ライドに乗っていないときのみ通れる壁
    MAX
  };
};

// 「壁」と「元になるバイナリデータ」を結びつけるテーブル
static const WallBinIndex::Value sc_WallIndexToBinIndex[ WallIndex::MAX ] = 
{
  WallBinIndex::NORMAL,     // WallIndex::NORMAL
  WallBinIndex::NAMINORI,   // WallIndex::OSHIKAESHI_NAMINORI
  WallBinIndex::NAMINORI,   // WallIndex::EVENT_NAMINORI
  WallBinIndex::ROBA2,       // WallIndex::OSHIKAESHI_ROBA2
  WallBinIndex::RIDE        // WallIndex::OSHIKAESHI_RIDE
};

// 地形ブロックのメモリ割り当てモード
struct MemoryMode
{
  enum Value
  {
    NORMAL = 0,   // 通常サイズ
    LOW,          // 軽減サイズ
    
    // 以下、地形マネージャのみ使用可
    NORMAL_2X,    // 通常サイズ(2倍モード)
    NORMAL_4X,    // 通常サイズ(4倍モード)
  };
};


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINTYPES_H_INCLUDED__
//======================================================================
/**
 * @file FinderStudioPokemonDataTable.h
 * @date 2017/02/09
 * @author harada_takumi
 * @brief ファインダースタジオポケモンデータテーブルアクセサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_POKEMON_DATA_TABLE_H_INCLUDED__
#define __FINDER_STUDIO_POKEMON_DATA_TABLE_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

// PokeTool
#include "PokeTool/include/PokeTool.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

// ポケモンAABB情報
class PokemonDataTable
{
public:
  PokemonDataTable();
  virtual ~PokemonDataTable(){}

  /// @brief アーカイブからの読み込み
  /// @param pHeap ヒープ
  /// @param pWorkHeap 一時ヒープ
  void StartLoadData(gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pWorkHeap);
  bool IsFinishLoadData();

  /// @brief 削除処理
  void DeleteData();

  // データテーブル構造体
  typedef struct
  {
    u32 monsNo : 16; /// 図鑑No
    u32 formNo : 8;  /// フォルムNo
    u32 zoomComposition1 : 5;    /// 構図１ズーム値
    u32 canUseComposition1 : 1;  /// 構図１使用可能フラグ
    u32 padding1 : 2;

    u32 zoomComposition2 : 5;    /// 構図２ズーム値
    u32 canUseComposition2 : 1;  /// 構図２使用可能フラグ
    u32 padding2 : 2;
    u32 distanceComposition1 : 8;    /// 構図１トレーナーとの距離
    u32 distanceComposition2 : 8;    /// 構図２トレーナーとの距離
    u32 canUseMotion1 : 8;   // 25種のモーションの使用可否

    u32 canUseMotion2 : 8;   // 25種のモーションの使用可否
    u32 canUseMotion3 : 8;   // 25種のモーションの使用可否
    u32 canUseMotion4 : 8;   // 25種のモーションの使用可否
    u32 padding3 : 8;

  } PokemonData;

  /// @brief データの取得
  /// @param monsNo 図鑑No
  /// @param formNo フォルムNo
  const PokemonData* GetData(u32 monsNo,u32 formNo);

  /// @brief 読み込み済みか
  bool IsLoaded() const { return m_IsLoaded; }

  /// @brief モーション使用可否判定
  /// @param monsNo 図鑑No
  /// @param formNo フォルムNo
  /// @param motionType モーションタイプ
  /// @param motionNo モーションNo
  bool IsCanUseMotion(u32 monsNo,u32 formNo,PokeMotionType motionType,u32 motionNo);

private:
  /// @brief データの取得
  /// @param インデックス
  const PokemonData* getData(u32 index) const;

  /// @brief インデックスの取得
  /// @param monsNo 図鑑No formNo フォルムNo
  /// @return データのインデックス
  u32 getIndex(u32 monsNo,u32 formNo);

  // バッファ
  void* m_pBuffer;

  // 読み込み済みか
  bool m_IsLoaded;
  // データサイズ
  size_t m_DataSize;
  // データ個数
  u32 m_DataNum;

  // 最後に読み込んだMonsNo
  u32 m_LastCheckMonsNo;
  // 最後に読み込んだFormNo
  u32 m_LastCheckFormNo;
  // 最後に算出したインデックス
  u32 m_LastCalcIndex;

  // 使用可否判定モーション数
  // 待機モーション2種 + バトル使用最大モーション数 + かわいがり使用最大モーション数 + 眠りモーション
  static const u32 NUM_MOTION_CHECK = 2 + 10 + 12 + 1;

  static const u32 MOTION_TYPE_NO_TABLE[NUM_MOTION_CHECK][2];
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_POKEMON_DATA_TABLE_H_INCLUDED__
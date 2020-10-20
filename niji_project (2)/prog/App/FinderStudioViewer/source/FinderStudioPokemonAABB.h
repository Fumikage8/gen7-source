//======================================================================
/**
 * @file FinderStudioPokemonAABB.h
 * @date 2016/12/14
 * @author harada_takumi
 * @brief ファインダースタジオポケモンAABB
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_POKEMON_AABB_H_INCLUDED__
#define __FINDER_STUDIO_POKEMON_AABB_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

GFL_NAMESPACE_BEGIN(PokeTool)
class PokeModelSystem;
class PokeModel;
struct SimpleParam;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

// ポケモンAABB情報
class PokemonAABB
{
public:
  PokemonAABB();
  virtual ~PokemonAABB(){}

  /// @brief アーカイブからの読み込み
  /// @param pHeap ヒープ
  /// @param pWorkHeap 一時ヒープ
  /// @param simpleParam ポケモン見た目データ
  /// @param pPokeModel ポケモンモデル
  /// @param pPokeModelSystem ポケモンモデルシステム（データインデックス算出に使用）
  void StartLoadData(gfl2::heap::HeapBase* pHeap,
    gfl2::heap::HeapBase* pWorkHeap,
    const PokeTool::SimpleParam& simpleParam,
    PokeTool::PokeModel* pPokeModel,
    PokeTool::PokeModelSystem* pPokeModelSystem);
  bool IsFinishLoadData();

  /// @brief アンロード処理
  void UnloadData();

#if PM_DEBUG
  /// @brief データ決めうち用関数
  void DebugSetData(u32 monsNo,u32 formNo);
#endif

  /// @brief AABBデータの取得
  /// @param motionType バトルかかわいがりか
  /// @param motionNo アニメーションNo
  /// @return パラメータに応じたAABB
  gfl2::math::AABB GetData(PokeMotionType motionType,u32 motionNo) const;

  /// @brief ファインダースタジオで使用可能なモーションか判定
  /// @param motionType バトルかかわいがりか
  /// @param motionNo アニメーションNo
  static bool IsCanUseMotion(PokeMotionType motionType,u32 motionNo);

private:
  // 待機モーション2種 + バトル使用最大モーション数 + かわいがり使用最大モーション数 + 眠りモーション
  static const u32 NUM_MOTION_AABB = 2 + 10 + 12 + 3;

  static const u32 MOTION_TYPE_NO_TABLE[NUM_MOTION_AABB][2];

  gfl2::math::AABB m_AABBData[NUM_MOTION_AABB];
  void* m_pBuffer;

  // ポケモンモデル（アジャストスケール取得時に使用）
  PokeTool::PokeModel* m_pPokeModel;

  gfl2::math::AABB getData(PokeMotionType motionType,u32 motionNo) const;

  /// @brief 指定したサイズ以下のとき、そのサイズに矯正する
  /// @param minSize 最低サイズ
  /// @param pAABB 矯正するAABB
  /// @return 矯正したかどうか
  bool correctAABB(f32 minSize,gfl2::math::AABB* pAABB) const;
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_POKEMON_AABB_H_INCLUDED__
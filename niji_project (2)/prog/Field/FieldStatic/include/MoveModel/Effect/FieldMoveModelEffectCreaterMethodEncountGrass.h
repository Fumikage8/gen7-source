//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelEffectCreaterMethodEncountGrass.h
 *	@brief  エンカウント草エフェクトの生成
 *	@author	tomoya takahashi
 *	@date		2015.06.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_MOVEMODEL_EFFECT_CREATER_METHOD_ENCOUNT_GRASS_H__ )
#define __FIELD_MOVEMODEL_EFFECT_CREATER_METHOD_ENCOUNT_GRASS_H__
#pragma once

#include <math/include/gfl2_math.h>

#include "FieldMoveModelEffectCreaterBase.h"

GFL_NAMESPACE_BEGIN(Field);

GFL_NAMESPACE_BEGIN(Terrain);
class TerrainManager;
GFL_NAMESPACE_END(Terrain);

GFL_NAMESPACE_BEGIN(Effect);
class IEffectBase;
class EffectManager;
GFL_NAMESPACE_END(Effect);

GFL_NAMESPACE_BEGIN(MoveModel);

class FieldMoveModel;

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 *  @class エンカウント草の生成処理
 */
class FieldMoveModelEffectCreaterMethodEncountGrass : public IFieldMoveModelEffectCreaterMethodBase
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldMoveModelEffectCreaterMethodEncountGrass );
public:

  /**
   *  @brief  コンストラクタ
   */
  FieldMoveModelEffectCreaterMethodEncountGrass();

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveModelEffectCreaterMethodEncountGrass();

  /**
   *  @brief  初期化
   */
  void Initialize( Terrain::TerrainManager* pTerrainManager, Effect::EffectManager* pEffectManager, MoveModel::FieldMoveModel* pPlayer );

  /**
   *  @brief  破棄処理
   */
  void Terminate();

  /** 
   *  @brief  ジャンプしたタイミングのエフェクト生成
   */
  virtual void CreateTimingJump();

  /** 
   *  @brief  移動したタイミングのエフェクト生成
   */
  virtual void CreateTimingMove();

  /** 
   *  @brief  ターンしたタイミングのエフェクト生成
   */
  virtual void CreateTimingTurn();

  /** 
   *  @brief  現座標でのブロックとグリッド座標を更新
   */
  virtual void UpdateBlockAndGridPos();

private:

  // グリッド座標、ブロック座標の取得
  bool GetBlockAndGridPos( s32* pBlockX, s32* pBlockZ, s32* pGridX, s32* pGridZ, s32* pFloorIndex );

  // グリッド座標、ブロック座標が変わっているかチェック
  bool IsBlockAndGridChange( const s32& BlockX, const s32& BlockZ, const s32& GridX, const s32& GridZ, const s32& FloorIndex ) const;

  // アトリビュートが草かチェック
  bool IsAttrGrass( void ) const;

  // エフェクトのタイプを取得
  u32 GetEffectType(void) const;

  // エフェクト生成
  void CreateEffect( const s32& BlockX, const s32& BlockZ, const s32& GridX, const s32& GridZ, const s32& FloorIndex );

  // 不要エフェクトの削除
  void DeleteEffect();

  // ブロックとグリッド座標を更新
  void UpdateBlockAndGridPos( const s32& BlockX, const s32& BlockZ, const s32& GridX, const s32& GridZ, const s32& FloorIndex );

private:
  static const u32 EFFECT_MAX = 6;

  Terrain::TerrainManager* m_pTerrainManager; ///< 地形

  MoveModel::FieldMoveModel* m_pPlayer;     ///< 主人公のInstance
  Effect::EffectManager* m_pEffectManager;  ///< エフェクトマネージャー

  gfl2::math::Vector3 m_OldPos; ///< 古い座標

  s32 m_OldBlockX;              ///< 古いブロック座標X
  s32 m_OldBlockZ;              ///< 古いブロック座標Z
  s32 m_OldGridX;               ///< 古いブロック内グリッド座標X
  s32 m_OldGridZ;               ///< 古いブロック内グリッド座標Z
  s32 m_OldFloorIndex;          ///< 古いフロアインデックス

  Effect::IEffectBase* m_pEffectArray[EFFECT_MAX];  ///< エフェクト配列
};

GFL_NAMESPACE_END(MoveModel);
GFL_NAMESPACE_END(Field);

#endif	// __FIELD_MOVEMODEL_EFFECT_CREATER_METHOD_ENCOUNT_GRASS_H__


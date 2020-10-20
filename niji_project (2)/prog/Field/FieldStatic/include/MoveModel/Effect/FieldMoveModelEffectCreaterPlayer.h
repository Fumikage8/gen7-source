//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelEffectCreaterPlayer.h
 *	@brief  プレイヤー用エフェクト生成処理
 *	@author	tomoya takahashi
 *	@date		2015.06.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_MOVEMODEL_EFFECT_CREATER_PLAYER_H__ )
#define __FIELD_MOVEMODEL_EFFECT_CREATER_PLAYER_H__
#pragma once

#include "FieldMoveModelEffectCreaterBase.h"
#include "FieldMoveModelEffectCreaterMethodEncountGrass.h"


GFL_NAMESPACE_BEGIN(Field);

GFL_NAMESPACE_BEGIN(Effect);
class EffectManager;
GFL_NAMESPACE_END(Effect);

GFL_NAMESPACE_BEGIN(Terrain);
class TerrainManager;
GFL_NAMESPACE_END(Terrain);

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
class FieldMoveModelEffectCreaterPlayer : public IFieldMoveModelEffectCreaterBase
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldMoveModelEffectCreaterPlayer );

public:
  struct Description
  {
    Effect::EffectManager* pEffectManager;
    Terrain::TerrainManager* pTerrainManager;
    MoveModel::FieldMoveModel* pPlayer;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldMoveModelEffectCreaterPlayer();

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveModelEffectCreaterPlayer();

  /**
   *  @brief  初期化
   */
  void Initialize( const Description& desc );

  /**
   *  @brief  破棄処理
   */
  virtual void Terminate();

private:

  enum
  {
    PLAYER_CREATER_METHOD_ENCOUNTGRASS = 0,
    PLAYER_CREATER_METHOD_NUM
  };

private:

  // 生成メソッド配列
  IFieldMoveModelEffectCreaterMethodBase* m_pCreaterMethodArray[PLAYER_CREATER_METHOD_NUM];

  // 各種生成モジュール
  FieldMoveModelEffectCreaterMethodEncountGrass m_EncountGrass;
};

GFL_NAMESPACE_END(MoveModel);
GFL_NAMESPACE_END(Field);

#endif	// __FIELD_MOVEMODEL_EFFECT_CREATER_PLAYER_H__


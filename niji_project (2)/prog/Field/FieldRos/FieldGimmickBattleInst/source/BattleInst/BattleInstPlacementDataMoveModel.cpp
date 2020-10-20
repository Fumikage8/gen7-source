//==============================================================================
/**
 * @file	BattleInstPlacementDataMoveModel.cpp
 * @brief	バトル施設の動的動作モデル配置情報
 * @author	onoue_masayuki
 * @date	2016/01/27
 */
// =============================================================================

#include <util/include/gfl2_std_string.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// mine
#include "Field/FieldRos/FieldGimmickBattleInst/include/BattleInst/BattleInstPlacementDataMoveModel.h"

GFL_NAMESPACE_BEGIN(BattleInst)


// コンストラクタ
BattleInstPlacementDataMoveModel::BattleInstPlacementDataMoveModel()
{
  m_type[0] = 1;  // CollistionDataHeader.num
  m_type[1] = Field::FieldCollisionAccessor::CYLINDER; // CollistionDataHeader.type[0]
  m_pDressUpParam = NULL;
  m_pDressUpModelResourceManager = NULL;
}

/**
 *  @brief 話しかけコリジョン情報の取得
 */
const void * BattleInstPlacementDataMoveModel::GetTalkCollisionData( const Field::FieldMoveModelAccessor::MoveModelData & data ) const
{
  Field::FieldCollisionAccessor::Cylinder cylinder;
  cylinder.center[0] = data.position[0];
  cylinder.center[1] = data.position[1];
  cylinder.center[2] = data.position[2];
  cylinder.radius = 45.5f;
  cylinder.height = 10.0f;
  gfl2::std::MemCopy( &cylinder, (void*)&m_type[2], sizeof(Field::FieldCollisionAccessor::Cylinder) );

  uptr addr = reinterpret_cast<uptr>(m_type);
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
const void * BattleInstPlacementDataMoveModel::GetCollisionData( const Field::FieldMoveModelAccessor::MoveModelData & data ) const
{
  Field::FieldCollisionAccessor::Cylinder cylinder;
  cylinder.center[0] = data.position[0];
  cylinder.center[1] = data.position[1];
  cylinder.center[2] = data.position[2];
  cylinder.radius = 37.5f;
  cylinder.height = 10.0f;
  gfl2::std::MemCopy( &cylinder, (void*)&m_type[2], sizeof(Field::FieldCollisionAccessor::Cylinder) );

  uptr addr = reinterpret_cast<uptr>(m_type);
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief 着せ替え用パラメータの取得
 *
 *  @note  動作モデルに着せ替えを適用させる場合に使用する。
 */
poke_3d::model::DressUpParam * BattleInstPlacementDataMoveModel::GetDressupParam() const
{
  return m_pDressUpParam;
}

/**
 *  @brief 着せ替え用リソースマネージャの取得
 *
 *  @note  動作モデルに着せ替えを適用させる場合に使用する。
 */
poke_3d::model::DressUpModelResourceManager * BattleInstPlacementDataMoveModel::GetDressUpModelResourceManager() const
{
  return m_pDressUpModelResourceManager;
}
  
/**
 *  @brief 着せ替えパラメータの設定
 *  @param  pDressupParam    着せ替えパラメータ
 */
void BattleInstPlacementDataMoveModel::SetDressUpParam( poke_3d::model::DressUpParam* pDressupParam )
{
  m_pDressUpParam = pDressupParam; 
}

/**
 *  @brief 着せ替えリソースマネージャの設定
 *  @param  pDressUpModelResourceManager    着せ替えパラメータ
 */
void BattleInstPlacementDataMoveModel::SetDressUpModelResourceManager( poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager )
{
  m_pDressUpModelResourceManager = pDressUpModelResourceManager;
}


GFL_NAMESPACE_END(BattleInst)
  
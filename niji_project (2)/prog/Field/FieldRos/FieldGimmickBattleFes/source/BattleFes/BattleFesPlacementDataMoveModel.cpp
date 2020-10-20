//==============================================================================
/**
* @file	BattleFesPlacementDataMoveModel.cpp
* @brief	バトルフェスの動的動作モデル配置情報  ※BattleInstから移植
* @author	munakata_kai
* @date	2016/11/26
*/
// =============================================================================

#include <util/include/gfl2_std_string.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// mine
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesPlacementDataMoveModel.h"

GFL_NAMESPACE_BEGIN( BattleFes )

// コンストラクタ
BattleFesPlacementDataMoveModel::BattleFesPlacementDataMoveModel()
{
  m_type[0] = 1;  // CollistionDataHeader.num
  m_type[1] = Field::FieldCollisionAccessor::CYLINDER; // CollistionDataHeader.type[0]
  m_pDressUpParam = NULL;
  m_pDressUpModelResourceManager = NULL;
}

/**
 *  @brief 話しかけコリジョン情報の取得
 */
const void * BattleFesPlacementDataMoveModel::GetTalkCollisionData( const Field::FieldMoveModelAccessor::MoveModelData & data ) const
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
const void * BattleFesPlacementDataMoveModel::GetCollisionData( const Field::FieldMoveModelAccessor::MoveModelData & data ) const
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
poke_3d::model::DressUpParam * BattleFesPlacementDataMoveModel::GetDressupParam() const
{
  return m_pDressUpParam;
}

/**
 *  @brief 着せ替え用リソースマネージャの取得
 *
 *  @note  動作モデルに着せ替えを適用させる場合に使用する。
 */
poke_3d::model::DressUpModelResourceManager * BattleFesPlacementDataMoveModel::GetDressUpModelResourceManager() const
{
  return m_pDressUpModelResourceManager;
}
  
/**
 *  @brief 着せ替えパラメータの設定
 *  @param  pDressupParam    着せ替えパラメータ
 */
void BattleFesPlacementDataMoveModel::SetDressUpParam( poke_3d::model::DressUpParam* pDressupParam )
{
  m_pDressUpParam = pDressupParam; 
}

/**
 *  @brief 着せ替えリソースマネージャの設定
 *  @param  pDressUpModelResourceManager    着せ替えパラメータ
 */
void BattleFesPlacementDataMoveModel::SetDressUpModelResourceManager( poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager )
{
  m_pDressUpModelResourceManager = pDressUpModelResourceManager;
}

GFL_NAMESPACE_END( BattleFes )

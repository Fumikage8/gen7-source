//==============================================================================
/**
 * @file	JFFieldPlacementDataMoveModel.cpp
 * @brief	ジョインフェスタの動的動作モデル配置情報
 * @author	ichiraku_katsuhiko
 * @date	2015/06/03 18:33:05
 */
// =============================================================================

#include <util/include/gfl2_std_string.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// mine
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFFieldPlacementDataMoveModel.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)


// コンストラクタ
JFFieldPlacementDataMoveModel::JFFieldPlacementDataMoveModel() :
  m_pDressUpParam(NULL)
  ,m_pDressUpModelResourceManager(NULL)
  ,m_facilitiesRotationRadY(0.0f)
  ,m_talkCollisionDistance(0.0f)
{
  m_type[0] = 1;  // CollistionDataHeader.num
  m_type[1] = Field::FieldCollisionAccessor::CYLINDER; // CollistionDataHeader.type[0]
}

/**
 *  @brief 話しかけコリジョン情報の取得
 */
const void * JFFieldPlacementDataMoveModel::GetTalkCollisionData( const Field::FieldMoveModelAccessor::MoveModelData & data ) const
{
  Field::FieldCollisionAccessor::Cylinder cylinder;
  cylinder.center[0] = data.position[0];
  cylinder.center[1] = data.position[1];
  cylinder.center[2] = data.position[2];
  cylinder.radius = 80.0f;
  cylinder.height = 150.0f;
  // オフセット
  if( m_talkCollisionDistance != 0.0f )
  {
    // 位置計算
    {
      gfl2::math::Vector3 position;
      gfl2::math::Vector3 centerPos( 4200.0f, 0.0f, 4200.0f );
      gfl2::math::Vector3 basePos( 0.0f, 0.0f, 0.0f );

      // オフセット値
      basePos.SetZ( m_talkCollisionDistance );

      gfl2::math::Matrix34 transMat( gfl2::math::Matrix34::GetTranslation( basePos ) );
      gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationY( m_facilitiesRotationRadY ) );
      gfl2::math::Matrix34 mat = rotMat * transMat;
      position = centerPos + mat.GetElemPosition();

      // 上書き
      cylinder.center[0] = position.x;
      cylinder.center[1] = position.y;
      cylinder.center[2] = position.z;
    }
  }
  gfl2::std::MemCopy( &cylinder, (void*)&m_type[2], sizeof(Field::FieldCollisionAccessor::Cylinder) );

  uptr addr = reinterpret_cast<uptr>(m_type);
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
const void * JFFieldPlacementDataMoveModel::GetCollisionData( const Field::FieldMoveModelAccessor::MoveModelData & data ) const
{
  Field::FieldCollisionAccessor::Cylinder cylinder;
  cylinder.center[0] = data.position[0];
  cylinder.center[1] = data.position[1];
  cylinder.center[2] = data.position[2];
  cylinder.radius = 40.0f;
  cylinder.height = 200.0f;
  gfl2::std::MemCopy( &cylinder, (void*)&m_type[2], sizeof(Field::FieldCollisionAccessor::Cylinder) );

  uptr addr = reinterpret_cast<uptr>(m_type);
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief 着せ替え用パラメータの取得
 *
 *  @note  動作モデルに着せ替えを適用させる場合に使用する。現状ジョインフェスタしかないはず。 ichiraku
 */
poke_3d::model::DressUpParam * JFFieldPlacementDataMoveModel::GetDressupParam() const
{
  return m_pDressUpParam;
}

/**
 *  @brief 着せ替え用リソースマネージャの取得
 *
 *  @note  動作モデルに着せ替えを適用させる場合に使用する。現状ジョインフェスタしかないはず。 ichiraku
 */
poke_3d::model::DressUpModelResourceManager * JFFieldPlacementDataMoveModel::GetDressUpModelResourceManager() const
{
  return m_pDressUpModelResourceManager;
}
  
/**
 *  @brief 着せ替えパラメータの設定
 *  @param  pDressupParam    着せ替えパラメータ
 */
void JFFieldPlacementDataMoveModel::SetDressUpParam( poke_3d::model::DressUpParam* pDressupParam )
{
  m_pDressUpParam = pDressupParam; 
}

/**
 *  @brief 着せ替えリソースマネージャの設定
 *  @param  pDressUpModelResourceManager    着せ替えパラメータ
 */
void JFFieldPlacementDataMoveModel::SetDressUpModelResourceManager( poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager )
{
  m_pDressUpModelResourceManager = pDressUpModelResourceManager;
}


/**
 *  @brief 施設店員の話しかけコリジョンオフセット
 *  @param  facilitiesRotationRadY  店員の回転Y
 *  @param  talkCollisionDistance   話しかけコリジョンオフセットずらす距離（中心から）
 */
void JFFieldPlacementDataMoveModel::SetFacilitiesTalkCollisionOffset( f32 facilitiesRotationRadY, f32 talkCollisionDistance )
{
  m_facilitiesRotationRadY  = facilitiesRotationRadY;
  m_talkCollisionDistance   = talkCollisionDistance;
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)
  
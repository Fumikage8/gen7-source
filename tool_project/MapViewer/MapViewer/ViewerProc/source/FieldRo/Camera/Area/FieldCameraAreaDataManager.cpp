/**
* @brief  フィールドカメラ、カメラエリアデータ管理
* @file   FieldCameraAreaDataManager.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// field
#include "ViewerProc/include/FieldRo/Camera/Area/FieldCameraAreaDataManager.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );



// @todo 仮
struct BIN_AREA_DATA
{
  u32                           dataType;
  u32                           interpolationFrame;
  s32                           priority;   

  u32                           easeFunc;


  POINT_DATA          pointA;
  b32                 defaultFlagA;
  POINT_DATA          pointB;
  b32                 defaultFlagB;
};

struct BIN_DATA
{
  u32 dataNum;
  BIN_AREA_DATA areaData;
};

static BIN_DATA BIN_DATA_DUMMY =
{
  0,
};


void ConvertDegToRad( gfl2::math::Vector3* pOut )
{
  pOut->x = gfl2::math::ConvDegToRad( pOut->x );
  pOut->y = gfl2::math::ConvDegToRad( pOut->y );
  pOut->z = gfl2::math::ConvDegToRad( pOut->z );
}

void ConvertDegToRad( POINT_DATA* pOut )
{
  pOut->fovY = gfl2::math::ConvDegToRad( pOut->fovY );
  ConvertDegToRad( &pOut->rotation );
}


/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
AreaDataManager::AreaDataManager( const InitializeDescription& rInitDesc ) : m_pBinData( &BIN_DATA_DUMMY )
  // 内部制御
  // m_DefaultData
{


  m_DefaultData.offset = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
  m_DefaultData.rotation = gfl2::math::Vector3( gfl2::math::ConvDegToRad( -34.637991f ), 0.0f, 0.0f );
  m_DefaultData.distance = 579.0f;
  m_DefaultData.fovY = gfl2::math::ConvDegToRad( 35.0f );

  // @note 開発中は無いこともある
  if( rInitDesc.m_pData )
  {
    m_pBinData = static_cast<BIN_DATA*>( rInitDesc.m_pData );
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
AreaDataManager::~AreaDataManager( void )
{

}

/**
* @brief エリアのデフォルト設定を取得
*
* @return 成否(falseはありえない)
*/
b32 AreaDataManager::GetDefaultData( DATA* pOut ) const
{
  // 仮
  pOut->dataType = DataType::FOLLOW;
  pOut->interpolationFrame = 5;
  pOut->pointA = m_DefaultData;
  return true;
}

// @todo 仮
b32 AreaDataManager::GetData( DATA* pOut, s32 areaId, s32 prevAreaId ) const
{
  if( areaId >= m_pBinData->dataNum || areaId == -1 )
  {
    return GetDefaultData( pOut );
  }

  BIN_AREA_DATA* pBinAreaData = static_cast<BIN_AREA_DATA*>( ((&m_pBinData->areaData) + areaId ) );

  switch( pBinAreaData->dataType )
  {
  case DataType::FOLLOW:
    {
      pOut->dataType = static_cast< DataType::Value > ( pBinAreaData->dataType );

      if( pBinAreaData->defaultFlagA )
      {
        pOut->pointA = m_DefaultData;
      }
      else
      {
        pOut->pointA = pBinAreaData->pointA;
        ConvertDegToRad( &pOut->pointA );
      }

      pOut->interpolationFrame = pBinAreaData->interpolationFrame;
      return true;
    }

  case DataType::FOLLOW_TRANS:
    {
      pOut->dataType = static_cast< DataType::Value > ( pBinAreaData->dataType );

      if( pBinAreaData->defaultFlagA )
      {
        pOut->pointA = m_DefaultData;
      }
      else
      {
        pOut->pointA = pBinAreaData->pointA;
        ConvertDegToRad( &pOut->pointA );
      }

      if( pBinAreaData->defaultFlagB )
      {
        pOut->pointB = m_DefaultData;
      }
      else
      {
        pOut->pointB = pBinAreaData->pointB;
        ConvertDegToRad( &pOut->pointB );
      }

      pOut->interpolationFrame = pBinAreaData->interpolationFrame;
      pOut->easeFunc = static_cast< gfl2::math::Easing::EaseFunc > ( pBinAreaData->easeFunc );
      return true;
    }

  default:
    {
      return false;
    }
  }


  if( areaId  == 0 )
  {
    pOut->dataType = DataType::FOLLOW;
    pOut->interpolationFrame = 5;
    pOut->pointA = m_DefaultData;
    return true;
  }
  else
  {
#if 0
    pOut->dataType = DataType::FULL;
    pOut->interpolationFrame = 25;
    pOut->pointA = m_DefaultData;
    pOut->pointA.distance = 300.0f;
    pOut->pointA.rotation = gfl2::math::Vector3( gfl2::math::ConvDegToRad( -24.637991f ), gfl2::math::ConvDegToRad( -24.637991f ), 0.0f );
#endif
    pOut->dataType = DataType::FOLLOW_TRANS;
    pOut->interpolationFrame = 0;
    pOut->pointA = m_DefaultData;
    pOut->pointA.distance = 300.0f;
    pOut->pointA.rotation = gfl2::math::Vector3( gfl2::math::ConvDegToRad( -24.637991f ), gfl2::math::ConvDegToRad( 0.0f ), 0.0f );
    pOut->pointB = m_DefaultData;
    pOut->pointB.distance = 300.0f;
    pOut->pointB.rotation = gfl2::math::Vector3( gfl2::math::ConvDegToRad( -44.637991f ), gfl2::math::ConvDegToRad( -24.637991f ), 0.0f );
    pOut->easeFunc = gfl2::math::Easing::LINEAR;
    return true;
  }
}

s32 AreaDataManager::GetPriority( s32 areaId ) const
{
  if( areaId >= m_pBinData->dataNum || areaId == -1 )
  {
    return -1;
  }

  BIN_AREA_DATA* pBinAreaData = static_cast<BIN_AREA_DATA*>( ((&m_pBinData->areaData) + areaId ) );
  return pBinAreaData->priority;
}

void AreaDataManager::Lerp( POINT_DATA* pOut, const POINT_DATA& r1, const POINT_DATA& r2, f32 t ) const
{
  pOut->fovY = gfl2::math::Lerp( r1.fovY, r2.fovY, t );
  pOut->distance = gfl2::math::Lerp( r1.distance, r2.distance, t );
  pOut->offset = r1.offset;
  pOut->offset.Lerp( r2.offset, t );

  gfl2::math::EulerRotation rot1;
  gfl2::math::EulerRotation rot2;

  gfl2::math::Quaternion q1;
  gfl2::math::Quaternion q2;

  rot1.Set( r1.rotation, gfl2::math::EulerRotation::RotationOrderZYX );
  rot2.Set( r2.rotation, gfl2::math::EulerRotation::RotationOrderZYX );

  q1 = rot1.ToQuaternion();
  q2 = rot2.ToQuaternion();

  q1.Slerp( q2, t );
  q1.QuaternionToRadian( &pOut->rotation.x, &pOut->rotation.y, &pOut->rotation.z );
}



GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );
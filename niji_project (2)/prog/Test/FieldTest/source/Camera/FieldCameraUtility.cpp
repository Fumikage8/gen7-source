/**
* @file    FieldCameraUtility.cpp
* @brief   フィールド上のカメラ便利関数群
* @author  ikuta_junya
* @date    2014.09.26
*/

#include "test/FieldTest/include/Camera/FieldCameraUtility.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

void ConvertDegreeToRadian( gfl2::math::Vector* pVec )
{
  pVec->x = gfl2::math::ConvDegToRad( pVec->x );
  pVec->y = gfl2::math::ConvDegToRad( pVec->y );
  pVec->z = gfl2::math::ConvDegToRad( pVec->z );
}

void ConvertRadianToDegree( gfl2::math::Vector* pVec )
{
  pVec->x = gfl2::math::ConvRadToDeg( pVec->x );
  pVec->y = gfl2::math::ConvRadToDeg( pVec->y );
  pVec->z = gfl2::math::ConvRadToDeg( pVec->z );
}

u32 GetLimitLineRotate( const gfl2::math::Vector& rCenter, f32 radius, f32 x, f32* pOut1, f32* pOut2 )
{
  if( rCenter.x - radius >= x  || rCenter.x + radius <= x )
  {
    return 0;
  }

  // X
  // ↑ 
  //   → Z
  // 上記平面で逆三角関数を使う

  f32 sinHit( ( x - rCenter.x ) / radius );
  f32 angle = asinf( sinHit );

  *pOut1 = angle;
  if( angle > 0 )
  {
    *pOut2 = PI - angle;
  }
  else
  {
    *pOut2 = -PI - angle;
  }
  return 2;
}

const c8* const COORDINATE_SYSTEM_TABLE[]= 
{
 "TYPE_WORLD",
 "TYPE_LOCAL",
};

const c8* GetLabel( CoordinateSystemType type )
{
  if( type >= GFL_NELEMS( COORDINATE_SYSTEM_TABLE ) )
  {
    return 0;
  }

  return COORDINATE_SYSTEM_TABLE[ type ];
}

/**
* @brief   デフォルトカメラの設定データを生成する
*
* @param   pHeap                      生成に使用するヒープ
*
* @return  無し
*/
Field::Camera::CAMERA_DATA* CreateDefaultCameraData( gfl2::heap::HeapBase* pHeap )
{
  u32 defaultInterpolationFrame = 0; 
  f32 defaultAngleOfView = 35.0f;

  Field::Camera::CAMERA_DATA* pCameraData = GFL_NEW( pHeap ) Field::Camera::CAMERA_DATA;

  // ヘッダー部分
  pCameraData->id = 0;
  pCameraData->type = Field::Camera::TYPE_RELATIVE_ROTATE;
  // strcpy( &pCameraData->name[ 0 ], "Default" );
  pCameraData->name[ 0 ] = '\0'; 

  // 補間情報
  pCameraData->interpolationData.defaultFrame = defaultInterpolationFrame;
  pCameraData->interpolationData.pUniqueInterpolationDataTable = NULL;
  pCameraData->interpolationData.uniqueInterpolationDataSize = 0;

  // 射影設定
  pCameraData->projectionData.aov = defaultAngleOfView;
  pCameraData->projectionData.nearZ = 1.0f;
  pCameraData->projectionData.farZ = 100000.0f;
  pCameraData->priority = 0;

  // タイプ別データ部分
  Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = GFL_NEW( pHeap ) Field::Camera::CAMERA_DATA_RELATIVE_ROTATE;

  InitializeLimitData( &pData->positionLimitData );
  InitializeLimitData( &pData->positionLimitDataEx );
  InitializeLimitData( &pData->rotateLimitData );
  pData->rotateLimitCoordinateSystemType = Field::Camera::COORDINATE_SYSTEM_TYPE_LOCAL;

  pData->rotate = gfl2::math::Vector( -34.637991, 0.0f, 0.0f );
  Field::Camera::ConvertDegreeToRadian( &pData->rotate );
  pData->distance = 579;

  pData->targetOffsetType = Field::Camera::COORDINATE_SYSTEM_TYPE_LOCAL;
  pData->targetOffset = gfl2::math::Vector( 0.0f, 0.0f, 0.0f );

  pCameraData->pData = static_cast<void*>( pData );


  return pCameraData;
}



GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );
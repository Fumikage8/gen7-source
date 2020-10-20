#if defined(GF_PLATFORM_WIN32)

/**
* @file    SkeltonCameraUtility.cpp
* @brief   フィールド上のカメラ便利関数群
* @author  ikuta_junya
* @date    2014.09.26
*/

#include "Test/Skelton/include/Field/Camera/SkeltonCameraUtility.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

void ConvertDegreeToRadian( gfl2::math::Vector4* pVec )
{
  pVec->x = gfl2::math::ConvDegToRad( pVec->x );
  pVec->y = gfl2::math::ConvDegToRad( pVec->y );
  pVec->z = gfl2::math::ConvDegToRad( pVec->z );
}

void ConvertRadianToDegree( gfl2::math::Vector4* pVec )
{
  pVec->x = gfl2::math::ConvRadToDeg( pVec->x );
  pVec->y = gfl2::math::ConvRadToDeg( pVec->y );
  pVec->z = gfl2::math::ConvRadToDeg( pVec->z );
}

u32 GetLimitLineRotate( const gfl2::math::Vector4& rCenter, f32 radius, f32 x, f32* pOut1, f32* pOut2 )
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
 "WORLD",
 "LOCAL",
};

const c8* GetLabel( CoordinateSystemType type )
{
  if( type >= GFL_NELEMS( COORDINATE_SYSTEM_TABLE ) )
  {
    return NULL;
  }

  return COORDINATE_SYSTEM_TABLE[ type ];
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32

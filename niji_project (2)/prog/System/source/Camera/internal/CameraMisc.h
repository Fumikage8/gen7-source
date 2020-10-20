
//=============================================================================
/**
 * @file   CameraMisc.h
 * @brief  ModelInFrame移植時に足りなかったもの
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#if !defined( __CAMERA_MISC_H__ )
#define __CAMERA_MISC_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <math/include/gfl2_MathAABB.h>

namespace System { namespace Camera { namespace internal
{

  extern const f32 F_PI;
  extern const f32 F_PIx2;
  extern const f32 F_PI_2;

  extern const gfl2::math::VEC2  VEC2_ZERO;
  extern const gfl2::math::VEC3  VEC3_ZERO;
  extern const gfl2::math::VEC3  VEC3_UNIT_SCALE;
  extern const gfl2::math::VEC3  VEC3_UNIT_X;
  extern const gfl2::math::VEC3  VEC3_UNIT_Y;
  extern const gfl2::math::VEC3  VEC3_UNIT_Z;
  extern const gfl2::math::MTX33 MTX33_IDENTITY;
  extern const gfl2::math::MTX34 MTX34_IDENTITY;
  extern const gfl2::math::MTX44 MTX44_IDENTITY;

  extern bool IsAABBVolumeZero(const gfl2::math::AABB& aabb, const float epsilon = 0.0f);

  extern gfl2::math::VEC3* VEC3TransformCoord(gfl2::math::VEC3* pOut, const gfl2::math::MTX44* pM, const gfl2::math::VEC3* pV);

} // namespace internal
} } // namespace System namespace Camera

#endif // __CAMERA_MISC_H__

/*  EOF  */







//=============================================================================
/**
 * @file   CameraMisc.cpp
 * @brief  ModelInFrame移植時に足りなかったもの
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#include "CameraMisc.h"

namespace System { namespace Camera { namespace internal
{

  const f32 F_PI    = 3.141592653589793f;//nn::math::F_PI;
  const f32 F_PIx2  = F_PI*2.0f;
  const f32 F_PI_2  = F_PI*0.5f;



  const gfl2::math::VEC2  VEC2_ZERO       = gfl2::math::VEC2(0.0f, 0.0f);
  const gfl2::math::VEC3  VEC3_ZERO       = gfl2::math::VEC3(0.0f, 0.0f, 0.0f);
  const gfl2::math::VEC3  VEC3_UNIT_SCALE = gfl2::math::VEC3(1.0f, 1.0f, 1.0f);
  const gfl2::math::VEC3  VEC3_UNIT_X     = gfl2::math::VEC3(1.0f, 0.0f, 0.0f);
  const gfl2::math::VEC3  VEC3_UNIT_Y     = gfl2::math::VEC3(0.0f, 1.0f, 0.0f);
  const gfl2::math::VEC3  VEC3_UNIT_Z     = gfl2::math::VEC3(0.0f, 0.0f, 1.0f);
  const gfl2::math::MTX33 MTX33_IDENTITY  = gfl2::math::MTX33(
                                             1.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f,
                                             0.0f, 0.0f, 1.0f
                                           );
  const gfl2::math::MTX34 MTX34_IDENTITY = gfl2::math::MTX34(
                                             1.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 1.0f, 0.0f
                                           );
  const gfl2::math::MTX44 MTX44_IDENTITY = gfl2::math::MTX44(
                                             1.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 1.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 1.0f
                                           );



  bool IsAABBVolumeZero(const gfl2::math::AABB& aabb, const float epsilon)
  {
    return ( (aabb.GetMax().x - aabb.GetMin().x) <= epsilon || (aabb.GetMax().y - aabb.GetMin().y) <= epsilon || (aabb.GetMax().z - aabb.GetMin().z) <= epsilon );
  }



  gfl2::math::VEC3* VEC3TransformCoord(gfl2::math::VEC3* pOut, const gfl2::math::MTX44* pM, const gfl2::math::VEC3* pV)
  {
    gfl2::math::VEC3 tmp;
    f32 rw;
    
    tmp.x = pM->f._00 * pV->x + pM->f._01 * pV->y + pM->f._02 * pV->z + pM->f._03;
    tmp.y = pM->f._10 * pV->x + pM->f._11 * pV->y + pM->f._12 * pV->z + pM->f._13;
    tmp.z = pM->f._20 * pV->x + pM->f._21 * pV->y + pM->f._22 * pV->z + pM->f._23;
       rw = pM->f._30 * pV->x + pM->f._31 * pV->y + pM->f._32 * pV->z + pM->f._33;
    rw = 1.f / rw;

    pOut->x = rw * tmp.x;
    pOut->y = rw * tmp.y;
    pOut->z = rw * tmp.z;

    return pOut;
  }

} // namespace internal
} } // namespace System namespace Camera

/*  EOF  */






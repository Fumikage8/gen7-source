﻿
//=============================================================================
/**
 * @file   gfl_MathPlane.cpp
 * @brief  平面制御クラス
 * @author 池田 哲平
 * @date   2012.08.25
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.12.03
 */
//=============================================================================

#include "MathPlane.h"

namespace System { namespace Camera { namespace internal
{
  //-----------------------------------------------------------------------------
  // 線との衝突判定を行う
  //-----------------------------------------------------------------------------
  bool Plane::IsHitLine(gfl2::math::VEC3* hitPosi, const gfl2::math::VEC3& srcPosi, const gfl2::math::VEC3& destPosi) const
  {
    // 交点 P = xyz0*t + xyz1*(1-t)と考えると平面方程式は
    // A*(x0*t + x1*(1-t)) + B*(y0*t + y1*(1-t)) + C*(z0*t + z1*(1-t)) + D = 0 となり、これは
    // t = -(A*x1 + B*y1 + C*z1 + D)/(A*(x0-x1) + B*(y0-y1) + C*(z0-z1)と変換できる。
    const float fSrcDot3 = gfl2::math::VEC3Dot(reinterpret_cast<const gfl2::math::VEC3* >(&abcd), &srcPosi);
    const float fSrcDot4 = fSrcDot3 + abcd.w;
    if( fSrcDot4 >= 0.0f )
    {
      const float fDestDot3 = gfl2::math::VEC3Dot(reinterpret_cast<const gfl2::math::VEC3* >(&abcd), &destPosi);
      const float fDestDot4 = fDestDot3 + abcd.w;
      if( fDestDot4 <= 0.0f ) // fSrcDot4*fDestDot4 <= 0.0f 法線の向きを考慮に入れないときの判定
      {
        const float fDenom = fSrcDot3 - fDestDot3;
        if( fDenom != 0.0f )
        {
          const float fT = -fDestDot4/fDenom;
          if( fT >= 0.0f && fT <= 1.0f )
          {
            gfl2::math::VEC3 inAdd;
            gfl2::math::VEC3Sub(&inAdd, &srcPosi, &destPosi);
            gfl2::math::VEC3Scale(&inAdd, &inAdd, fT);
            gfl2::math::VEC3Add(hitPosi, &destPosi, &inAdd);
            return true;
          }
        }
      }
    }

    return false;
  }

} } } // namespace System::Camera::internal

/*  EOF  */






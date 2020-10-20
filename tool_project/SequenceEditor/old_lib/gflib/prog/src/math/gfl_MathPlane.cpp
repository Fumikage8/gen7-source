
//=============================================================================
/**
 * @file   gfl_MathPlane.cpp
 * @brief  ���ʐ���N���X
 * @author �r�c �N��
 * @date   2012.08.25
 */
//=============================================================================

#include "math/gfl_MathPlane.h"

namespace gfl { namespace math
{
  //-----------------------------------------------------------------------------
  // ���Ƃ̏Փ˔�����s��
  //-----------------------------------------------------------------------------
  bool Plane::IsHitLine(VEC3* hitPosi, const VEC3& srcPosi, const VEC3& destPosi) const
  {
    // ��_ P = xyz0*t + xyz1*(1-t)�ƍl����ƕ��ʕ�������
    // A*(x0*t + x1*(1-t)) + B*(y0*t + y1*(1-t)) + C*(z0*t + z1*(1-t)) + D = 0 �ƂȂ�A�����
    // t = -(A*x1 + B*y1 + C*z1 + D)/(A*(x0-x1) + B*(y0-y1) + C*(z0-z1)�ƕϊ��ł���B
    const float fSrcDot3 = VEC3Dot(reinterpret_cast<const gfl::math::VEC3* >(&abcd), &srcPosi);
    const float fSrcDot4 = fSrcDot3 + abcd.w;
    if( fSrcDot4 >= 0.0f )
    {
      const float fDestDot3 = gfl::math::VEC3Dot(reinterpret_cast<const gfl::math::VEC3* >(&abcd), &destPosi);
      const float fDestDot4 = fDestDot3 + abcd.w;
      if( fDestDot4 <= 0.0f ) // fSrcDot4*fDestDot4 <= 0.0f �@���̌������l���ɓ���Ȃ��Ƃ��̔���
      {
        const float fDenom = fSrcDot3 - fDestDot3;
        if( fDenom != 0.0f )
        {
          const float fT = -fDestDot4/fDenom;
          if( fT >= 0.0f && fT <= 1.0f )
          {
            VEC3 inAdd;
            VEC3Sub(&inAdd, &srcPosi, &destPosi);
            VEC3Scale(&inAdd, &inAdd, fT);
            VEC3Add(hitPosi, &destPosi, &inAdd);
            return true;
          }
        }
      }
    }

    return false;
  }

} } // namespace gfl::math

/*  EOF  */






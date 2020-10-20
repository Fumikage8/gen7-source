
//=============================================================================
/**
 * @file   gfl_MathSphere.h
 * @brief  ������N���X�w�b�_
 * @author �r�c �N��
 * @date   2012.09.14
 */
//=============================================================================

#ifndef __GFL_MATH_SPHERE_H__
#define __GFL_MATH_SPHERE_H__
#pragma once

namespace gfl { namespace math
{
  //-----------------------------------------------------------------------------
  /**
   * @brief ������N���X
   */
  //-----------------------------------------------------------------------------
  class Sphere
  {
  public:
    VEC3  center; ///< ���S�ʒu
    float radius; ///< ���a

  public:
    Sphere() { return ; } ///< �R���X�g���N�^�P
    Sphere(const VEC3& __center__, const float __radius__) : center(__center__) , radius(__radius__) { return ; } ///< �R���X�g���N�^�Q

    inline void Set(const VEC3& __center__, const float __radius__) { center = __center__, radius = __radius__; } ///< �l�ݒ�P
  };

} } // namespace gfl::math

#endif // __GFL_MATH_SPHERE_H__

/*  EOF  */






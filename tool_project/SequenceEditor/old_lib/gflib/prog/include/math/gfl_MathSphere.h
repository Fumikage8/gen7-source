
//=============================================================================
/**
 * @file   gfl_MathSphere.h
 * @brief  球制御クラスヘッダ
 * @author 池田 哲平
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
   * @brief 球制御クラス
   */
  //-----------------------------------------------------------------------------
  class Sphere
  {
  public:
    VEC3  center; ///< 中心位置
    float radius; ///< 半径

  public:
    Sphere() { return ; } ///< コンストラクタ１
    Sphere(const VEC3& __center__, const float __radius__) : center(__center__) , radius(__radius__) { return ; } ///< コンストラクタ２

    inline void Set(const VEC3& __center__, const float __radius__) { center = __center__, radius = __radius__; } ///< 値設定１
  };

} } // namespace gfl::math

#endif // __GFL_MATH_SPHERE_H__

/*  EOF  */






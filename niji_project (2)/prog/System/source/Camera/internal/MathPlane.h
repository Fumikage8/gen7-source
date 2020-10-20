
//=============================================================================
/**
 * @file   MathPlane.h
 * @brief  平面制御クラスヘッダ
 * @author 池田 哲平
 * @date   2012.08.25
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.12.03
 */
//=============================================================================

#ifndef __MATH_PLANE_H__
#define __MATH_PLANE_H__
#pragma once

#include <math/include/gfl2_math.h>

namespace System { namespace Camera { namespace internal
{
  //-----------------------------------------------------------------------------
  /**
   * @brief 平面制御クラス
   */
  //-----------------------------------------------------------------------------
  class Plane
  {
  public:
    gfl2::math::VEC4 abcd; ///< ax + by + cz + d = 0

  public:
    Plane() { return ; } ///< コンストラクタ１
    Plane(const gfl2::math::VEC4& __abcd__) : abcd(__abcd__) { return ; } ///< コンストラクタ２
    Plane(const float a, const float b, const float c, const float d) : abcd(a, b, c, d) { return ; } ///< コンストラクタ３
    Plane(const gfl2::math::VEC3& normVec, const gfl2::math::VEC3& posi) { Set(normVec, posi); } ///< コンストラクタ４

    inline void Set(const gfl2::math::VEC4& __abcd__) { abcd = __abcd__; }
    inline void Set(const float a, const float b, const float c, const float d) { abcd = gfl2::math::VEC4(a, b, c, d); } ///< 値設定１
    //-----------------------------------------------------------------------------
    /**
     * @brief 値設定２
     *
     * @param normVec 法線ベクタ(正規化されている必要はない)
     * @param posi    平面に含まれる任意の位置
     */
    //-----------------------------------------------------------------------------
    inline void Set(const gfl2::math::VEC3& normVec, const gfl2::math::VEC3& posi) { abcd = gfl2::math::VEC4(normVec.x, normVec.y, normVec.z, -gfl2::math::VEC3Dot(&normVec, &posi)); }

    inline void AddD(const float d) { abcd.w += d; } ///< D値を加算する
    inline void Normalize() ///< 正規化する
    {
      const float len = gfl2::math::VEC3Len(reinterpret_cast<gfl2::math::VEC3* >(&abcd));
      gfl2::math::VEC4Scale(&abcd, &abcd, 1.0f/len);
    }

    inline const gfl2::math::VEC4& GetPlaneVector() const { return abcd; } ///< 平面ベクタを取得する
    inline void GetNormalVector(gfl2::math::VEC3* v) const { v->x = abcd.x, v->y = abcd.y, v->z = abcd.z; } ///< 法線ベクタを取得する(正規化されている保証はない)

    /// 入力位置から平面までの最短距離を取得する
    inline float GetDistance(const gfl2::math::VEC3& posi) const { return gfl2::math::VEC3Dot(reinterpret_cast<const gfl2::math::VEC3* >(&abcd), &posi) + abcd.w; }

    //-----------------------------------------------------------------------------
    /**
     * @brief 点との衝突判定を行う
     *
     * @param posi    点の位置
     * @param epsilon 衝突していると判断してよいイプシロン値
     *
     * @retval true  衝突している
     * @retval false 衝突していない
     */
    //-----------------------------------------------------------------------------
    inline bool IsHitPoint(const gfl2::math::VEC3& posi, const float epsilon = 0.001f) const { return (gfl2::math::FAbs(GetDistance(posi)) <= epsilon); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 線との衝突判定を行う(注：重なっている時はfalseを返す)
     *
     * @param hitPosi  衝突位置
     * @param srcPosi  始点
     * @param destPosi 終点
     *
     * @retval true  衝突している
     * @retval false 衝突していない
     */
    //-----------------------------------------------------------------------------
    bool IsHitLine(gfl2::math::VEC3* hitPosi, const gfl2::math::VEC3& srcPosi, const gfl2::math::VEC3& destPosi) const ;
  };

} } } // namespace System::Camera::internal

#endif // __MATH_PLANE_H__

/*  EOF  */






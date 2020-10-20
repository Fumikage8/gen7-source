
//=============================================================================
/**
 * @file   gfl_MathPlane.h
 * @brief  平面制御クラスヘッダ
 * @author 池田 哲平
 * @date   2012.08.25
 */
//=============================================================================

#ifndef __GFL_MATH_PLANE_H__
#define __GFL_MATH_PLANE_H__
#pragma once

namespace gfl { namespace math
{
  //-----------------------------------------------------------------------------
  /**
   * @brief 平面制御クラス
   */
  //-----------------------------------------------------------------------------
  class Plane
  {
  public:
    VEC4 abcd; ///< ax + by + cz + d = 0

  public:
    Plane() { return ; } ///< コンストラクタ１
    Plane(const VEC4& __abcd__) : abcd(__abcd__) { return ; } ///< コンストラクタ２
    Plane(const float a, const float b, const float c, const float d) : abcd(a, b, c, d) { return ; } ///< コンストラクタ３
    Plane(const VEC3& normVec, const VEC3& posi) { Set(normVec, posi); } ///< コンストラクタ４

    inline void Set(const VEC4& __abcd__) { abcd = __abcd__; }
    inline void Set(const float a, const float b, const float c, const float d) { abcd = VEC4(a, b, c, d); } ///< 値設定１
    //-----------------------------------------------------------------------------
    /**
     * @brief 値設定２
     *
     * @param normVec 法線ベクタ(正規化されている必要はない)
     * @param posi    平面に含まれる任意の位置
     */
    //-----------------------------------------------------------------------------
    inline void Set(const VEC3& normVec, const VEC3& posi) { abcd = VEC4(normVec.x, normVec.y, normVec.z, -VEC3Dot(&normVec, &posi)); }

    inline void AddD(const float d) { abcd.w += d; } ///< D値を加算する
    inline void Normalize() ///< 正規化する
    {
      const float len = VEC3Len(reinterpret_cast<VEC3* >(&abcd));
      VEC4Scale(&abcd, &abcd, 1.0f/len);
    }

    inline const gfl::math::VEC4& GetPlaneVector() const { return abcd; } ///< 平面ベクタを取得する
    inline void GetNormalVector(VEC3* v) const { v->x = abcd.x, v->y = abcd.y, v->z = abcd.z; } ///< 法線ベクタを取得する(正規化されている保証はない)

    /// 入力位置から平面までの最短距離を取得する
    inline float GetDistance(const VEC3& posi) const { return VEC3Dot(reinterpret_cast<const VEC3* >(&abcd), &posi) + abcd.w; }

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
    inline bool IsHitPoint(const VEC3& posi, const float epsilon = 0.001f) const { return (FAbs(GetDistance(posi)) <= epsilon); }

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
    bool IsHitLine(VEC3* hitPosi, const VEC3& srcPosi, const VEC3& destPosi) const ;
  };

} } // namespace gfl::math

#endif // __GFL_MATH_PLANE_H__

/*  EOF  */






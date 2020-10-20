//*************************************************************************************************
/**
 @file    gfl2_MathUtil.h
 @brief   算術系のユーティリティクラス
 @date    2015.12.03
 @author  Naoya Uematsu
 */
//*************************************************************************************************

#if !defined(GFLIB2_MATH_MATHUTIL_H_INCLUDED)
#define GFLIB2_MATH_MATHUTIL_H_INCLUDED
#pragma once

#include <math/include/gfl2_Vector2.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Matrix44.h>

namespace gfl2 { namespace math {

class MathUtil
{
public:
  /**
   * @brief レイを飛ばしてスクリーン上で交差したベクトルを計算
   * @param viewMatrix ビュー行列
   * @param projectionMatrix プロジェクション行列
   * @param x スクリーン座標x(-1.0f～1.0f)
   * @param y スクリーン座標y(-1.0f～1.0f)
   * @return gfl2::math::Vector4 レイとスクリーンの交点
   */
  static inline gfl2::math::Vector4 CalculateNearPlaneVectorPosition(const gfl2::math::Matrix34& viewMatrix, const gfl2::math::Matrix44& projectionMatrix, f32 x, f32 y)
  {
    gfl2::math::Matrix44 viewInvMatrix = viewMatrix.Inverse();
    gfl2::math::Matrix44 projectionInvMatrix = projectionMatrix.Inverse();
    gfl2::math::Matrix44 viewProjectionInvMatrix = viewInvMatrix * projectionInvMatrix;
  
    gfl2::math::Vector4 screenPosition(x, y, 0.0f);           // 前方クリップ面
    gfl2::math::Vector4 viewCameraPosition(0.0f, 0.0f, 0.0f); // ビュー空間でのカメラの位置
  
    gfl2::math::Vector4 worldPosition;
    worldPosition = viewProjectionInvMatrix.TransformVec(screenPosition);
    gfl2::math::Vector4 worldPerspectivePosition = worldPosition / worldPosition.w;

    worldPosition = viewInvMatrix * viewCameraPosition;
    gfl2::math::Vector4 worldPerspectiveCameraPosition = worldPosition / worldPosition.w;

    gfl2::math::Vector4 ray = worldPerspectivePosition - worldPerspectiveCameraPosition;
  
    return ray.Normalize();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 線分とポリゴンとの当たり判定
   * @param pOutPos   交点
   * @param ray_pos   線分位置
   * @param ray_vec   線分ベクトル
   * @param vertex0   ポリゴン頂点０（右回り）
   * @param vertex1   ポリゴン頂点１（右回り）
   * @param vertex2   ポリゴン頂点２（右回り）
   * @return          交差したか
   */
  //---------------------------------------------------------------------------
  static b32 RayPickPolygon( gfl2::math::Vector4 *pOutPos, const gfl2::math::Vector4 &ray_pos, const gfl2::math::Vector4 &ray_vec, gfl2::math::Vector4 vertex0, gfl2::math::Vector4 vertex1, gfl2::math::Vector4 vertex2 );

private:
  static b32 RayPick( gfl2::math::Vector4 *cross, const gfl2::math::Vector4 &pos1, const gfl2::math::Vector4 &vec1, const gfl2::math::Vector4 &pos2, const gfl2::math::Vector4 &vec2 );
  static b32 CheckInnerPoint( const gfl2::math::Vector4 &pos0, const gfl2::math::Vector4 &pos1, const gfl2::math::Vector4 &pos2, const gfl2::math::Vector4 &point, const gfl2::math::Vector4 &normal );
};

}}

#endif
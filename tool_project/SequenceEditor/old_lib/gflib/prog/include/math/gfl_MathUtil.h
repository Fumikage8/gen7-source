#ifndef __GFL_MATH_UTIL_H__
#define __GFL_MATH_UTIL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MathUtil.h
 *	@brief  算術ユーティリティ
 *	@author	masateru ishiguro
 *	@date		2011.04.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <nn/math.h>

#include <math/gfl_Vec.h>
#include <math/gfl_Mtx.h>
#include <math/gfl_MathType.h>

namespace gfl {
namespace math {

class MathUtil
{
public:

  //---------------------------------------------------------------------------
  /**
   * @brief ニアクリップ面の３Ｄワールド座標取得
   * @param p_view_matrix         ビューマトリックス
   * @param p_projection_matrix   プロジェクションマトリックス
   * @param x                     スクリーン座標Ｘ(-1.0f ? 1.0f)3DSは縦横が逆転している場合があるので注意してください
   * @param y                     スクリーン座標Ｘ(-1.0f ? 1.0f)3DSは縦横が逆転している場合があるので注意してください
   * @return                      ニアクリップ面の３Ｄワールド座標
   */
  //---------------------------------------------------------------------------
  static gfl::math::VEC4 GetNearPlanePosition( gfl::math::MTX34 *p_view_matrix, gfl::math::MTX44 *p_projection_matrix, f32 x, f32 y );
  
  //---------------------------------------------------------------------------
  /**
   * @brief カメラからのレイ取得
   * @param p_view_matrix         ビューマトリックス
   * @param p_projection_matrix   プロジェクションマトリックス
   * @param x                     スクリーン座標Ｘ(-1.0f ? 1.0f)3DSは縦横が逆転している場合があるので注意してください
   * @param y                     スクリーン座標Ｘ(-1.0f ? 1.0f)3DSは縦横が逆転している場合があるので注意してください
   * @return                      カメラからのレイ（長さ１のベクトル）
   */
  //---------------------------------------------------------------------------
  static gfl::math::VEC4 GetRayVector( gfl::math::MTX34 *p_view_matrix, gfl::math::MTX44 *p_projection_matrix, f32 x, f32 y );

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
  static b32 RayPickPolygon( gfl::math::VEC4 *pOutPos, const gfl::math::VEC4 &ray_pos, const gfl::math::VEC4 &ray_vec, gfl::math::VEC4 vertex0, gfl::math::VEC4 vertex1, gfl::math::VEC4 vertex2 );
  
private:
  MathUtil(){}
  ~MathUtil(){}

  static b32 RayPick( gfl::math::VEC4 *cross, const gfl::math::VEC4 &pos1, const gfl::math::VEC4 &vec1, const gfl::math::VEC4 &pos2, const gfl::math::VEC4 &vec2 );
  static b32 CheckInnerPoint( const gfl::math::VEC4 &pos0, const gfl::math::VEC4 &pos1, const gfl::math::VEC4 &pos2, const gfl::math::VEC4 &point, const gfl::math::VEC4 &normal );
  
};


} /* end of namespace math */
} /* end of namespace gfl */

#endif // __GFL_MATH_UTIL_H__

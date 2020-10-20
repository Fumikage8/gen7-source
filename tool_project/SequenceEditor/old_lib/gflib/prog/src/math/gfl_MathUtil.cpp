//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MathUtil.cpp
 *	@brief  算術ユーティリティ
 *	@author	masateru ishiguro
 *	@date		2011.04.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nn/math.h>
#include <math/gfl_MathUtil.h>

namespace gfl{
namespace math{

const f32 F_PI   = nn::math::F_PI;
const f32 F_PIx2 = F_PI*2.0f;
const f32 F_PI_2 = F_PI*0.5f;
const f32 F_MAX  = nn::math::F_MAX;
const f32 F_MIN  = nn::math::F_MIN;

const VEC2 VEC2_ZERO      (0.0f, 0.0f);
const VEC2 VEC2_UNIT_SCALE(1.0f, 1.0f);
const VEC3 VEC3_ZERO      (0.0f, 0.0f, 0.0f);
const VEC3 VEC3_UNIT_X    (1.0f, 0.0f, 0.0f);
const VEC3 VEC3_UNIT_Y    (0.0f, 1.0f, 0.0f);
const VEC3 VEC3_UNIT_Z    (0.0f, 0.0f, 1.0f);
const VEC3 VEC3_UNIT_SCALE(1.0f, 1.0f, 1.0f);
const VEC4 VEC4_UNIT_Y    (0.0f, 1.0f, 0.0f, 0.0f);

const MTX33 MTX33_IDENTITY(1.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 1.0f);
const MTX34 MTX34_IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f);
const MTX44 MTX44_IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f);

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
gfl::math::VEC4 MathUtil::GetNearPlanePosition( gfl::math::MTX34 *p_view_matrix, gfl::math::MTX44 *p_projection_matrix, f32 x, f32 y )
{
  gfl::math::MTX44      view_inv_matrix = gfl::math::MTX44( *p_view_matrix );
  gfl::math::MTX44      projection_inv_matrix;
  gfl::math::MTX44      view_projection_inv_matrix;
  
  gfl::math::VEC3       screan_pos( x, y, 0.0f );// 前方クリップ面
  gfl::math::VEC4       world_pos;
  gfl::math::VEC4       world_pers_pos;
  
  gfl::math::MTX44Inverse( &view_inv_matrix, &view_inv_matrix );
  gfl::math::MTX44Inverse( &projection_inv_matrix, p_projection_matrix );
  gfl::math::MTX44Mult( &view_projection_inv_matrix, &view_inv_matrix, &projection_inv_matrix );
  
  nn::math::VEC3Transform( &world_pos, &view_projection_inv_matrix, &screan_pos ); 
  gfl::math::VEC4Scale( &world_pers_pos, &world_pos, 1.0f / world_pos.w );
  
  return world_pers_pos;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
gfl::math::VEC4 MathUtil::GetRayVector( gfl::math::MTX34 *p_view_matrix, gfl::math::MTX44 *p_projection_matrix, f32 x, f32 y )
{
  gfl::math::MTX44      view_inv_matrix = gfl::math::MTX44( *p_view_matrix );
  gfl::math::MTX44      projection_inv_matrix;
  gfl::math::MTX44      view_projection_inv_matrix;
  
  gfl::math::VEC3       screan_pos( x, y, 0.0f );// 前方クリップ面
  gfl::math::VEC3       view_cam_pos( 0.0f, 0.0f, 0.0f );// ビュー空間でのカメラの位置
  gfl::math::VEC4       world_pos;
  gfl::math::VEC4       world_pers_pos;
  gfl::math::VEC4       world_pers_cam_pos;
  gfl::math::VEC4       ray;
  
  gfl::math::MTX44Inverse( &view_inv_matrix, &view_inv_matrix );
  gfl::math::MTX44Inverse( &projection_inv_matrix, p_projection_matrix );
  gfl::math::MTX44Mult( &view_projection_inv_matrix, &view_inv_matrix, &projection_inv_matrix );
  
  nn::math::VEC3Transform( &world_pos, &view_projection_inv_matrix, &screan_pos ); 
  gfl::math::VEC4Scale( &world_pers_pos, &world_pos, 1.0f / world_pos.w );
  
  nn::math::VEC3Transform( &world_pos, &view_inv_matrix, &view_cam_pos ); 
  gfl::math::VEC4Scale( &world_pers_cam_pos, &world_pos, 1.0f / world_pos.w );
  
  ray = world_pers_pos - world_pers_cam_pos;
  
  return ray.Normalize();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
b32 MathUtil::RayPickPolygon( gfl::math::VEC4 *pOutPos, const gfl::math::VEC4 &ray_pos, const gfl::math::VEC4 &ray_vec, gfl::math::VEC4 vertex0, gfl::math::VEC4 vertex1, gfl::math::VEC4 vertex2 )
{
  gfl::math::VEC4			nor;
	gfl::math::VEC4			cross;
	gfl::math::VEC4			lVec;
	gfl::math::VEC4			rVec;
  gfl::math::VEC4			center;
  
  lVec = vertex1 - vertex0;
  rVec = vertex2 - vertex0;
  nor.Cross( lVec, rVec );
  nor = nor.Normalize();
  
  if ( RayPick( &cross, ray_pos, ray_vec, vertex0, nor ) == false )
  {//平面に届かないのでスキップ
    return false;
  }
  
  //float誤差で抜け落ちないようにポリゴンを少し広げる
  center = vertex0 + vertex1 + vertex2;
  center /= 3;
  
  vertex0 += (vertex0 - center) * 0.01f;
  vertex1 += (vertex1 - center) * 0.01f;
  vertex2 += (vertex2 - center) * 0.01f;


  if ( CheckInnerPoint( vertex0, vertex1, vertex2, cross, nor ) == false )
  {//ポリゴンの外側だった
    return false;
  }

  *pOutPos = cross;

  return true;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
b32 MathUtil::CheckInnerPoint( const gfl::math::VEC4 &pos0, const gfl::math::VEC4 &pos1, const gfl::math::VEC4 &pos2, const gfl::math::VEC4 &point, const gfl::math::VEC4 &normal )
{
	gfl::math::VEC4			lVec;
	gfl::math::VEC4			rVec;
	gfl::math::VEC4			nor;

	lVec = pos0 - point;
	rVec = pos1 - point;
	nor.Cross( lVec, rVec);

	if ( normal.Dot( nor ) < 0.0f )
		return false;

	lVec = pos1 - point;
	rVec = pos2 - point;
	nor.Cross( lVec, rVec);

	if ( normal.Dot( nor ) < 0.0f )
		return false;

	lVec = pos2 - point;
	rVec = pos0 - point;
	nor.Cross( lVec, rVec);

	if ( normal.Dot( nor ) < 0.0f )
		return false;

	return ( true );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
b32 MathUtil::RayPick( gfl::math::VEC4 *cross, const gfl::math::VEC4 &pos1, const gfl::math::VEC4 &vec1, const gfl::math::VEC4 &pos2, const gfl::math::VEC4 &vec2 )
{
  f32		a, b, t;
  gfl::math::VEC4	vec;

  vec = pos2 - pos1;

  a = vec.Dot( vec2 );
  b = vec1.Dot( vec2 );

  //平行なので交点なし
  if( b == 0.0f )
      return ( false );

  //媒介変数ｔを求める
  t = a / b;

  if ( t < 0.0f || t > 1.0f )
    return ( false );

  //直線の方程式に代入し、交点を求める
  cross->x = pos1.x + vec1.x * t;
  cross->y = pos1.y + vec1.y * t;
  cross->z = pos1.z + vec1.z * t;

  //交点あり
  return ( true );
}



}
}

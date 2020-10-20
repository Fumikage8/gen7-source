//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MathUtil.cpp
 *	@brief  算術ユーティリティ
 *	@author	masateru ishiguro
 *	@date		2015.12.3
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <math/include/gfl2_MathUtil.h>

namespace gfl2 { namespace math {

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
b32 MathUtil::RayPickPolygon( gfl2::math::Vector4 *pOutPos, const gfl2::math::Vector4 &ray_pos, const gfl2::math::Vector4 &ray_vec, gfl2::math::Vector4 vertex0, gfl2::math::Vector4 vertex1, gfl2::math::Vector4 vertex2 )
{
  gfl2::math::Vector4			nor;
	gfl2::math::Vector4			cross;
	gfl2::math::Vector4			lVec;
	gfl2::math::Vector4			rVec;
  gfl2::math::Vector4			center;
  
  lVec = vertex1 - vertex0;
  rVec = vertex2 - vertex0;
  nor = lVec.Cross( rVec );
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
b32 MathUtil::CheckInnerPoint( const gfl2::math::Vector4 &pos0, const gfl2::math::Vector4 &pos1, const gfl2::math::Vector4 &pos2, const gfl2::math::Vector4 &point, const gfl2::math::Vector4 &normal )
{
	gfl2::math::Vector4			lVec;
	gfl2::math::Vector4			rVec;
	gfl2::math::Vector4			nor;

	lVec = pos0 - point;
	rVec = pos1 - point;
	nor = lVec.Cross( rVec );

	if ( normal.Dot( nor ) < 0.0f )
		return false;

	lVec = pos1 - point;
	rVec = pos2 - point;
	nor = lVec.Cross( rVec );

	if ( normal.Dot( nor ) < 0.0f )
		return false;

	lVec = pos2 - point;
	rVec = pos0 - point;
	nor = lVec.Cross( rVec );

	if ( normal.Dot( nor ) < 0.0f )
		return false;

	return ( true );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
b32 MathUtil::RayPick( gfl2::math::Vector4 *cross, const gfl2::math::Vector4 &pos1, const gfl2::math::Vector4 &vec1, const gfl2::math::Vector4 &pos2, const gfl2::math::Vector4 &vec2 )
{
  f32		a, b, t;
  gfl2::math::Vector4	vec;

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

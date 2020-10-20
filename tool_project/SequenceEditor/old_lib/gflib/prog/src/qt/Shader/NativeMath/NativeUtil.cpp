#include <math.h>
#include "qt/Shader/NativeMath/NativeUtil.h"

// @todo 完成したらcppは削除してヘッダーだけの単なるインライン関数にすべて変更する予定。ので将来的にはNativeMathはライブラリではなくなる。

namespace gfl { namespace maya { namespace Math {

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
bool Util::RayPickPolygon( Vector *pOutPos, const Vector &ray_pos, const Vector &ray_vec, Vector vertex0, Vector vertex1, Vector vertex2 )
{
  Vector			nor;
	Vector			cross;
	Vector			lVec;
	Vector			rVec;
  Vector			center;
  
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
bool Util::CheckInnerPoint( const Vector &pos0, const Vector &pos1, const Vector &pos2, const Vector &point, const Vector &normal )
{
	Vector			lVec;
	Vector			rVec;
	Vector			nor;

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
bool Util::RayPick( Vector *cross, const Vector &pos1, const Vector &vec1, const Vector &pos2, const Vector &vec2 )
{
  float		a, b, t;
  Vector	vec;

  vec = pos2 - pos1;

  a = vec.Dot( vec2 );
  b = vec1.Dot( vec2 );

  //平行なので交点なし
  if( b == 0.0f )
      return ( false );

  //媒介変数ｔを求める
  t = a / b;

  if ( t < 0.0f )
    return ( false );

  //直線の方程式に代入し、交点を求める
  cross->x = pos1.x + vec1.x * t;
  cross->y = pos1.y + vec1.y * t;
  cross->z = pos1.z + vec1.z * t;

  //交点あり
  return ( true );
}

}}}
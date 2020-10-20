#include <math.h>
#include "qt/Shader/NativeMath/NativeUtil.h"

// @todo ����������cpp�͍폜���ăw�b�_�[�����̒P�Ȃ�C�����C���֐��ɂ��ׂĕύX����\��B�̂ŏ����I�ɂ�NativeMath�̓��C�u�����ł͂Ȃ��Ȃ�B

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
  {//���ʂɓ͂��Ȃ��̂ŃX�L�b�v
    return false;
  }
  
  //float�덷�Ŕ��������Ȃ��悤�Ƀ|���S���������L����
  center = vertex0 + vertex1 + vertex2;
  center /= 3;
  
  vertex0 += (vertex0 - center) * 0.01f;
  vertex1 += (vertex1 - center) * 0.01f;
  vertex2 += (vertex2 - center) * 0.01f;


  if ( CheckInnerPoint( vertex0, vertex1, vertex2, cross, nor ) == false )
  {//�|���S���̊O��������
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

  //���s�Ȃ̂Ō�_�Ȃ�
  if( b == 0.0f )
      return ( false );

  //�}��ϐ��������߂�
  t = a / b;

  if ( t < 0.0f )
    return ( false );

  //�����̕������ɑ�����A��_�����߂�
  cross->x = pos1.x + vec1.x * t;
  cross->y = pos1.y + vec1.y * t;
  cross->z = pos1.z + vec1.z * t;

  //��_����
  return ( true );
}

}}}
// マトリックス演算系　※汎用性重視で各ハードウェアに対して最適化はしていません

#include "core/gfl_Math.h"


#include "core/gfl_Tuple.h"
#include "core/gfl_Vector.h"
#include "core/gfl_Matrix.h"

// =================================================================
// BaseMatrix44
// =================================================================
// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Set( const BaseMatrix44& m )
{
#if 0 // べたに書くとこうなる
	_m00 = m._m00;
	_m01 = m._m01;
	_m02 = m._m02;
	_m03 = m._m03;
	_m10 = m._m10;
	_m11 = m._m11;
	_m12 = m._m12;
	_m13 = m._m13;
	_m20 = m._m20;
	_m21 = m._m21;
	_m22 = m._m22;
	_m23 = m._m23;
	_m30 = m._m30;
	_m31 = m._m31;
	_m32 = m._m32;
	_m33 = m._m33;
#else // まとめて転送可能な場合にはこちらが高速
	SetRow( 0, *m.GetRow( 0 ) );
	SetRow( 1, *m.GetRow( 1 ) );
	SetRow( 2, *m.GetRow( 2 ) );
	SetRow( 3, *m.GetRow( 3 ) );
#endif
}

// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Set( f32 m0, f32 m1, f32 m2, f32 m3,
								   f32 m4, f32 m5, f32 m6, f32 m7,
								   f32 m8, f32 m9, f32 m10, f32 m11,
								   f32 m12, f32 m13, f32 m14, f32 m15 )
{
	_m00 = m0;
	_m01 = m1;
	_m02 = m2;
	_m03 = m3;
	_m10 = m4;
	_m11 = m5;
	_m12 = m6;
	_m13 = m7;
	_m20 = m8;
	_m21 = m9;
	_m22 = m10;
	_m23 = m11;
	_m30 = m12;
	_m31 = m13;
	_m32 = m14;
	_m33 = m15;
}

// -----------------------------------------------------------------
// Set33
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Set33( f32 m0, f32 m1, f32 m2,
									 f32 m3, f32 m4, f32 m5,
									 f32 m6, f32 m7, f32 m8 )
{
	_m00 = m0;
	_m01 = m1;
	_m02 = m2;
	_m10 = m3;
	_m11 = m4;
	_m12 = m5;
	_m20 = m6;
	_m21 = m7;
	_m22 = m8;
}

// -----------------------------------------------------------------
// Set33
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Set33( const Matrix& m )
{
	_m00 = m._m00;
	_m01 = m._m01;
	_m02 = m._m02;
	_m10 = m._m10;
	_m11 = m._m11;
	_m12 = m._m12;
	_m20 = m._m20;
	_m21 = m._m21;
	_m22 = m._m22;
}

// -----------------------------------------------------------------
// SetUnit
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::SetUnit( void )
{
#if 0
	_m0, 1, 0.0f );
	_m0, 2, 0.0f );
	_m0, 3, 0.0f );
	_m1, 0, 0.0f );
	_m1, 2, 0.0f );
	_m1, 3, 0.0f );
	_m2, 0, 0.0f );
	_m2, 1, 0.0f );
	_m2, 3, 0.0f );
	_m3, 0, 0.0f );
	_m3, 1, 0.0f );
	_m3, 2, 0.0f );

	_m0, 0, 1.0f );
	_m1, 1, 1.0f );
	_m2, 2, 1.0f );
	_m3, 3, 1.0f );
#else
	SetRow( 0, Vector4::sX1 );
	SetRow( 1, Vector4::sY1 );
	SetRow( 2, Vector4::sZ1 );
	SetRow( 3, Vector4::sW1 );
#endif
}

// -----------------------------------------------------------------
// SetUnit33
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::SetUnit33( void )
{
	SetRow( 0, Vector4::sX1 );
	SetRow( 1, Vector4::sY1 );
	SetRow( 2, Vector4::sZ1 );
}

// -----------------------------------------------------------------
// Transpose33
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Transpose33( const BaseMatrix44& m )
{
	_m00 = m._m00;
	_m01 = m._m10;
	_m02 = m._m20;
	_m10 = m._m01;
	_m11 = m._m11;
	_m12 = m._m21;
	_m20 = m._m02;
	_m21 = m._m12;
	_m22 = m._m22;
}

// -----------------------------------------------------------------
// Transpose33
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Transpose33( void )
{
	Math::Swap( _m01, _m10 );
	Math::Swap( _m02, _m20 );
	Math::Swap( _m12, _m21 );
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Transpose( const BaseMatrix44& m )
{
	_m00 = m._m00;
	_m01 = m._m10;
	_m02 = m._m20;
	_m03 = m._m30;
	_m10 = m._m01;
	_m11 = m._m11;
	_m12 = m._m21;
	_m13 = m._m31;
	_m20 = m._m02;
	_m21 = m._m12;
	_m22 = m._m22;
	_m23 = m._m32;
	_m30 = m._m03;
	_m31 = m._m13;
	_m32 = m._m23;
	_m33 = m._m33;
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Transpose( void )
{
	Math::Swap( _m01, _m10 );
	Math::Swap( _m02, _m20 );
	Math::Swap( _m03, _m30 );
	Math::Swap( _m12, _m21 );
	Math::Swap( _m13, _m31 );
	Math::Swap( _m23, _m32 );
}

// -----------------------------------------------------------------
// operator ==
// -----------------------------------------------------------------
bool gfl::core::BaseMatrix44::operator==( const BaseMatrix44& m0 )
{
	return *GetRow( 0 ) == *m0.GetRow( 0 ) &&
		*GetRow( 1 ) == *m0.GetRow( 1 ) &&
		*GetRow( 2 ) == *m0.GetRow( 2 ) &&
		*GetRow( 3 ) == *m0.GetRow( 3 );
}

// -----------------------------------------------------------------
// operator !=
// -----------------------------------------------------------------
bool gfl::core::BaseMatrix44::operator!=( const BaseMatrix44& m0 )
{
	return *GetRow( 0 ) != *m0.GetRow( 0 ) ||
		*GetRow( 1 ) != *m0.GetRow( 1 ) ||
		*GetRow( 2 ) != *m0.GetRow( 2 ) ||
		*GetRow( 3 ) != *m0.GetRow( 3 );
}

// -----------------------------------------------------------------
// IsSimilar
// -----------------------------------------------------------------
bool gfl::core::BaseMatrix44::IsSimilar( const BaseMatrix44& m0, const f32 rot_scale_range, const f32 trans_range ) const
{
	return GetRow( 0 )->IsSimilar( *m0.GetRow( 0 ), rot_scale_range ) &&
		GetRow( 1 )->IsSimilar( *m0.GetRow( 1 ), rot_scale_range ) &&
		GetRow( 2 )->IsSimilar( *m0.GetRow( 2 ), rot_scale_range ) &&
		GetRow( 3 )->IsSimilar( *m0.GetRow( 3 ), trans_range );
}

// -----------------------------------------------------------------
// Normalize
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::Normalize( void )
{
	gfl::core::Math::ClampAbs( &_m00, 1.0f );
	gfl::core::Math::ClampAbs( &_m01, 1.0f );
	gfl::core::Math::ClampAbs( &_m02, 1.0f );
	gfl::core::Math::ClampAbs( &_m10, 1.0f );
	gfl::core::Math::ClampAbs( &_m11, 1.0f );
	gfl::core::Math::ClampAbs( &_m12, 1.0f );
	gfl::core::Math::ClampAbs( &_m20, 1.0f );
	gfl::core::Math::ClampAbs( &_m21, 1.0f );
	gfl::core::Math::ClampAbs( &_m22, 1.0f );
}

// -----------------------------------------------------------------
// Print
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::PrintConsole( void ) const
{
	Debug::PrintConsole( "%ff, %ff, %ff, %ff,\n%ff, %ff, %ff, %ff,\n%ff, %ff, %ff, %ff,\n%ff, %ff, %ff, %ff\n",
						 _m00, _m01, _m02, _m03,
						 _m10, _m11, _m12, _m13,
						 _m20, _m21, _m22, _m23,
						 _m30, _m31, _m32, _m33 );
}

// -----------------------------------------------------------------
// Print3x3
// -----------------------------------------------------------------
void gfl::core::BaseMatrix44::PrintConsole33( void ) const
{
	Debug::PrintConsole( "%ff, %ff, %ff,\n%ff, %ff, %ff,\n%ff, %ff, %ff\n",
						 _m00, _m01, _m02,
						 _m10, _m11, _m12,
						 _m20, _m21, _m22 );
}

// =================================================================
// Matrix44AOS
// =================================================================
// ---------------------------------------------------------------------------------
// LookAtMatrix生成
// ---------------------------------------------------------------------------------
gfl::core::Matrix44AOS gfl::core::Matrix44AOS::GetLookAt( const Vector4& eye, const Vector4& aim, const Vector4& up )
{
	return GetLookAtGL( eye, aim, up );
}

// ---------------------------------------------------------------------------------
// OpenGL互換 LookAtMatrix生成
// ---------------------------------------------------------------------------------
gfl::core::Matrix44AOS gfl::core::Matrix44AOS::GetLookAtGL( const Vector4& eye, const Vector4& aim, const Vector4& up )
{
	Vector4 d = ( aim - eye );
	d.Normalize( );

	Vector4 s = d;
	s.Cross( up );
	s.Normalize( );

	Vector4 r = s;
	r.Cross( d );

	gfl::core::Matrix44AOS mat;

	mat.Set( s._x, r._x, -d._x, 0.0f,
			 s._y, r._y, -d._y, 0.0f,
			 s._z, r._z, -d._z, 0.0f,
			 -eye.Dot( s ), -eye.Dot( r ), eye.Dot( d ), 1.0f );

	//mat.Inverse();

	return mat;
}

// ---------------------------------------------------------------------------------
// PerspectiveMatrix生成
// ---------------------------------------------------------------------------------
gfl::core::Matrix44AOS gfl::core::Matrix44AOS::GetPerspective( float fovy, float aspect, float zNear, float zFar )
{
	return GetPerspectiveGL( fovy, aspect, zNear, zFar );
}

// ---------------------------------------------------------------------------------
// OpenGL互換 PerspectiveMatrix生成
// ---------------------------------------------------------------------------------
// Perspective projection matrix:
// cot(fovy/2)/aspect        0            0                     0
//      0             cot(fovy/2)         0                     0
//      0                    0  (zFar+zNear)/(zNear-zFar) 2*zFar*zNear/(zNear-zFar)
//      0                    0           -1                     0
//! Perspective OpenGL互換(Depth=-1.0〜+1.0に射影)
gfl::core::Matrix44AOS gfl::core::Matrix44AOS::GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar )
{
	f32 f, rangeInv;

	f = tanf( (gfl::core::Math::ONE_PI * 0.5f) - fovy * 0.5f );
	//f = gfl::core::Math::FCos( fovy * 0.5f ); // これじゃ駄目な場合もあるらしい
	rangeInv = 1.0f / ( zNear - zFar );

	gfl::core::Matrix44AOS mat;

	mat.Set( f / aspect, 0.0f, 0.0f, 0.0f,
			 0.0f, f, 0.0f, 0.0f,
			 0.0f, 0.0f, ( zNear + zFar ) * rangeInv, -1.0f,
			 0.0f, 0.0f, zNear * zFar * rangeInv * 2.0f, 0.0f );

	return mat;
}

// -----------------------------------------------------------------
// Set Rotate X
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::SetRotateX( const f32 s, const f32 c )
{
	SetRow( 0, Vector4::sX1 );

	_m10 = 0.0f;
	_m11 = c;
	_m12 = s;
	_m13 = 0.0f;

	_m20 = 0.0f;
	_m21 = -s;
	_m22 = c;
	_m23 = 0.0f;
}

// -----------------------------------------------------------------
// Set Rotate Y
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::SetRotateY( const f32 s, const f32 c )
{
	_m00 = c;
	_m01 = 0.0f;
	_m02 = -s;
	_m03 = 0.0f;

	SetRow( 1, Vector4::sY1 );

	_m20 = s;
	_m21 = 0.0f;
	_m22 = c;
	_m23 = 0.0f;
}

// -----------------------------------------------------------------
// Set Rotate Z
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::SetRotateZ( const f32 s, const f32 c )
{
	_m00 = c;
	_m01 = s;
	_m02 = 0.0f;
	_m03 = 0.0f;

	_m10 = -s;
	_m11 = c;
	_m12 = 0.0f;
	_m13 = 0.0f;

	SetRow( 2, Vector4::sZ1 );
}

// -----------------------------------------------------------------
// Set Scale
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::SetScale( const f32 x, const f32 y, const f32 z )
{
	_m01 =
		_m02 =
		_m03 =
		_m10 =
		_m12 =
		_m13 =
		_m20 =
		_m21 =
		_m23 = 0.0f;
	_m00 = x;
	_m11 = y;
	_m22 = z;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateX( const Matrix44AOS& m0, const f32 s, const f32 c )
{
	_m00 = m0._m00;
	_m01 = m0._m01;
	_m02 = m0._m02;
	_m03 = m0._m03;
	_m30 = m0._m30;
	_m31 = m0._m31;
	_m32 = m0._m32;
	_m33 = m0._m33;
	_m13 = m0._m13;
	_m23 = m0._m23;
	_m10 = m0._m20 * s + m0._m10 * c;
	_m11 = m0._m21 * s + m0._m11 * c;
	_m12 = m0._m22 * s + m0._m12 * c;
	_m20 = m0._m20 * c - m0._m10 * s;
	_m21 = m0._m21 * c - m0._m11 * s;
	_m22 = m0._m22 * c - m0._m12 * s;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateX( const f32 s, const f32 c )
{
	const f32 m10 = _m10, m20 = _m20;
	const f32 m11 = _m11, m21 = _m21;
	const f32 m12 = _m12, m22 = _m22;

	_m10 = m20 * s + m10 * c;
	_m11 = m21 * s + m11 * c;
	_m12 = m22 * s + m12 * c;
	_m20 = m20 * c - m10 * s;
	_m21 = m21 * c - m11 * s;
	_m22 = m22 * c - m12 * s;
}

// -----------------------------------------------------------------
// RotateX Reverse
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateX_Reverse( const f32 s, const f32 c )
{
	const f32 m01 = _m01, m02 = _m02;
	const f32 m11 = _m11, m12 = _m12;
	const f32 m21 = _m21, m22 = _m22;

	_m01 = m01 * c - m02 * s;
	_m11 = m11 * c - m12 * s;
	_m21 = m21 * c - m22 * s;
	_m02 = m02 * c + m01 * s;
	_m12 = m12 * c + m11 * s;
	_m22 = m22 * c + m21 * s;
}

// -----------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateY( const Matrix44AOS& m0, const f32 s, const f32 c )
{
	_m10 = m0._m10;
	_m11 = m0._m11;
	_m12 = m0._m12;
	_m13 = m0._m13;
	_m30 = m0._m30;
	_m31 = m0._m31;
	_m32 = m0._m32;
	_m33 = m0._m33;
	_m03 = m0._m03;
	_m23 = m0._m23;
	_m20 = m0._m00 * c + m0._m20 * s;
	_m21 = m0._m01 * c + m0._m21 * s;
	_m22 = m0._m02 * c + m0._m22 * s;
	_m00 = m0._m00 * c - m0._m20 * s;
	_m01 = m0._m01 * c - m0._m21 * s;
	_m02 = m0._m02 * c - m0._m22 * s;
}

// -----------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateY( const f32 s, const f32 c )
{
	const f32 m00 = _m00, m20 = _m20;
	const f32 m01 = _m01, m21 = _m21;
	const f32 m02 = _m02, m22 = _m22;

	_m20 = m00 * s + m20 * c;
	_m21 = m01 * s + m21 * c;
	_m22 = m02 * s + m22 * c;
	_m00 = m00 * c - m20 * s;
	_m01 = m01 * c - m21 * s;
	_m02 = m02 * c - m22 * s;
}

// -----------------------------------------------------------------
// Rotate Y Reverse
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateY_Reverse( const Matrix44AOS& m0, const f32 s, const f32 c )
{
	const f32 m00 = m0._m00, m02 = m0._m02;
	const f32 m10 = m0._m10, m12 = m0._m12;
	const f32 m20 = m0._m20, m22 = m0._m22;

	_m01 = m0._m01;
	_m03 = m0._m03;
	_m11 = m0._m11;
	_m13 = m0._m13;
	_m21 = m0._m21;
	_m23 = m0._m23;
	_m31 = m0._m31;
	_m33 = m0._m33;

	_m00 = m00 * c + m02 * s;
	_m10 = m10 * c + m12 * s;
	_m20 = m20 * c + m22 * s;
	_m02 = m02 * c - m00 * s;
	_m12 = m12 * c - m10 * s;
	_m22 = m22 * c - m20 * s;

	const f32 x = m0._m30;
	const f32 z = m0._m32;
	_m30 = Math::GetX_RotateY_SinCosXZ( s, c, x, z );
	_m32 = Math::GetZ_RotateY_SinCosXZ( s, c, x, z );
}

// -----------------------------------------------------------------
// Rotate Y Reverse
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateY_Reverse( const f32 s, const f32 c )
{
	const f32 m00 = _m00, m02 = _m02;
	const f32 m10 = _m10, m12 = _m12;
	const f32 m20 = _m20, m22 = _m22;

	_m00 = m00 * c + m02 * s;
	_m10 = m10 * c + m12 * s;
	_m20 = m20 * c + m22 * s;
	_m02 = m02 * c - m00 * s;
	_m12 = m12 * c - m10 * s;
	_m22 = m22 * c - m20 * s;
}

// -----------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateZ( const Matrix44AOS& m0, const f32 s, const f32 c )
{
	_m20 = m0._m20;
	_m21 = m0._m21;
	_m22 = m0._m22;
	_m23 = m0._m23;
	_m30 = m0._m30;
	_m31 = m0._m31;
	_m32 = m0._m32;
	_m33 = m0._m33;
	_m03 = m0._m03;
	_m13 = m0._m13;
	_m00 = m0._m10 * s + m0._m00 * c;
	_m01 = m0._m11 * s + m0._m01 * c;
	_m02 = m0._m12 * s + m0._m02 * c;
	_m10 = m0._m10 * c - m0._m00 * s;
	_m11 = m0._m11 * c - m0._m01 * s;
	_m12 = m0._m12 * c - m0._m02 * s;
}

// -----------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateZ( const f32 s, const f32 c )
{
	const f32 m00 = _m00, m10 = _m10;
	const f32 m01 = _m01, m11 = _m11;
	const f32 m02 = _m02, m12 = _m12;

	_m00 = m10 * s + m00 * c;
	_m01 = m11 * s + m01 * c;
	_m02 = m12 * s + m02 * c;
	_m10 = m10 * c - m00 * s;
	_m11 = m11 * c - m01 * s;
	_m12 = m12 * c - m02 * s;
}

// -----------------------------------------------------------------
// Rotate Z Reverese
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::RotateZ_Reverse( const f32 s, const f32 c )
{
	const f32 m00 = _m00, m01 = _m01;
	const f32 m10 = _m10, m11 = _m11;
	const f32 m20 = _m20, m21 = _m21;

	_m00 = m00 * c - m01 * s;
	_m10 = m10 * c - m11 * s;
	_m20 = m20 * c - m21 * s;
	_m01 = m00 * s + m01 * c;
	_m11 = m10 * s + m11 * c;
	_m12 = m20 * s + m21 * c;
}

// -----------------------------------------------------------------
// Mul33
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::Mul33( const Matrix44AOS& m0, const Matrix44AOS& m1 )
{
	const f32 l_00 = m0._m00, l_01 = m0._m01, l_02 = m0._m02;
	const f32 l_10 = m0._m10, l_11 = m0._m11, l_12 = m0._m12;
	const f32 l_20 = m0._m20, l_21 = m0._m21, l_22 = m0._m22;
	const f32 r_00 = m1._m00, r_01 = m1._m01, r_02 = m1._m02;

	_m00 = l_00 * r_00 + l_10 * r_01 + l_20 * r_02;
	_m01 = l_01 * r_00 + l_11 * r_01 + l_21 * r_02;
	_m02 = l_02 * r_00 + l_12 * r_01 + l_22 * r_02;

	const f32 r_10 = m1._m10, r_11 = m1._m11, r_12 = m1._m12;

	_m10 = l_00 * r_10 + l_10 * r_11 + l_20 * r_12;
	_m11 = l_01 * r_10 + l_11 * r_11 + l_21 * r_12;
	_m12 = l_02 * r_10 + l_12 * r_11 + l_22 * r_12;

	const f32 r_20 = m1._m20, r_21 = m1._m21, r_22 = m1._m22;

	_m20 = l_00 * r_20 + l_10 * r_21 + l_20 * r_22;
	_m21 = l_01 * r_20 + l_11 * r_21 + l_21 * r_22;
	_m22 = l_02 * r_20 + l_12 * r_21 + l_22 * r_22;
}

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::Mul( const Matrix44AOS& m0, const Matrix44AOS& m1 )
{
	const f32 l_00 = m0._m00, l_01 = m0._m01, l_02 = m0._m02, l_03 = m0._m03;
	const f32 l_10 = m0._m10, l_11 = m0._m11, l_12 = m0._m12, l_13 = m0._m13;
	const f32 l_20 = m0._m20, l_21 = m0._m21, l_22 = m0._m22, l_23 = m0._m23;
	const f32 l_30 = m0._m30, l_31 = m0._m31, l_32 = m0._m32, l_33 = m0._m33;
	const f32 r_00 = m1._m00, r_01 = m1._m01, r_02 = m1._m02, r_03 = m1._m03;

	_m00 = l_00 * r_00 + l_10 * r_01 + l_20 * r_02 + l_30 * r_03;
	_m01 = l_01 * r_00 + l_11 * r_01 + l_21 * r_02 + l_31 * r_03;
	_m02 = l_02 * r_00 + l_12 * r_01 + l_22 * r_02 + l_32 * r_03;
	_m03 = l_03 * r_00 + l_13 * r_01 + l_23 * r_02 + l_33 * r_03;

	const f32 r_10 = m1._m10, r_11 = m1._m11, r_12 = m1._m12, r_13 = m1._m13;

	_m10 = l_00 * r_10 + l_10 * r_11 + l_20 * r_12 + l_30 * r_13;
	_m11 = l_01 * r_10 + l_11 * r_11 + l_21 * r_12 + l_31 * r_13;
	_m12 = l_02 * r_10 + l_12 * r_11 + l_22 * r_12 + l_32 * r_13;
	_m13 = l_03 * r_10 + l_13 * r_11 + l_23 * r_12 + l_33 * r_13;

	const f32 r_20 = m1._m20, r_21 = m1._m21, r_22 = m1._m22, r_23 = m1._m23;

	_m20 = l_00 * r_20 + l_10 * r_21 + l_20 * r_22 + l_30 * r_23;
	_m21 = l_01 * r_20 + l_11 * r_21 + l_21 * r_22 + l_31 * r_23;
	_m22 = l_02 * r_20 + l_12 * r_21 + l_22 * r_22 + l_32 * r_23;
	_m23 = l_03 * r_20 + l_13 * r_21 + l_23 * r_22 + l_33 * r_23;

	const f32 r_30 = m1._m30, r_31 = m1._m31, r_32 = m1._m32, r_33 = m1._m33;

	_m30 = l_00 * r_30 + l_10 * r_31 + l_20 * r_32 + l_30 * r_33;
	_m31 = l_01 * r_30 + l_11 * r_31 + l_21 * r_32 + l_31 * r_33;
	_m32 = l_02 * r_30 + l_12 * r_31 + l_22 * r_32 + l_32 * r_33;
	_m33 = l_03 * r_30 + l_13 * r_31 + l_23 * r_32 + l_33 * r_33;
}

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::Mul( const Quaternion& q0 )
{
	Matrix mat;
	mat.QuaternionToMatrix( q0 );
	Mul33( mat );
}

// -----------------------------------------------------------------
// Determinant33
// -----------------------------------------------------------------
f32 gfl::core::Matrix44AOS::Determinant33( void ) const
{
	return _m00 * _m11 * _m22 + _m01 * _m12 * _m20 + _m02 * _m10 * _m21
		- _m00 * _m12 * _m21 - _m01 * _m10 * _m22 - _m02 * _m11 * _m20;
}

// -----------------------------------------------------------------
// Determinant
// -----------------------------------------------------------------
f32 gfl::core::Matrix44AOS::Determinant( void ) const
{
	return(_m00 * _m11 - _m10 * _m01 )*( _m22 * _m33 - _m32 * _m23 )
		-( _m00 * _m21 - _m20 * _m01 )*( _m12 * _m33 - _m32 * _m13 )
		+( _m00 * _m31 - _m30 * _m01 )*( _m12 * _m23 - _m22 * _m13 )
		+( _m10 * _m21 - _m20 * _m11 )*( _m02 * _m33 - _m32 * _m03 )
		-( _m10 * _m31 - _m30 * _m11 )*( _m02 * _m23 - _m22 * _m03 )
		+( _m20 * _m31 - _m30 * _m21 )*( _m02 * _m13 - _m12 * _m03 );
}

// -----------------------------------------------------------------
// Inverse33
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::Inverse33( const Matrix44AOS& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set33(
		 ( m0._m11 * m0._m22 - m0._m21 * m0._m12 ) * det,
		 ( m0._m21 * m0._m02 - m0._m01 * m0._m22 ) * det,
		 ( m0._m01 * m0._m12 - m0._m11 * m0._m02 ) * det,

		 ( m0._m12 * m0._m20 - m0._m22 * m0._m10 ) * det,
		 ( m0._m22 * m0._m00 - m0._m02 * m0._m20 ) * det,
		 ( m0._m02 * m0._m10 - m0._m12 * m0._m00 ) * det,

		 ( m0._m10 * m0._m21 - m0._m20 * m0._m11 ) * det,
		 ( m0._m20 * m0._m01 - m0._m00 * m0._m21 ) * det,
		 ( m0._m00 * m0._m11 - m0._m10 * m0._m01 ) * det
		 );
}

// -----------------------------------------------------------------
// Inverse
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::Inverse( const Matrix44AOS& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set(
		 m0._m11 * ( m0._m22 * m0._m33 - m0._m32 * m0._m23 ) + m0._m21 * ( m0._m32 * m0._m13 - m0._m12 * m0._m33 ) + m0._m31 * ( m0._m12 * m0._m23 - m0._m22 * m0._m13 ),
		 m0._m21 * ( m0._m02 * m0._m33 - m0._m32 * m0._m03 ) + m0._m31 * ( m0._m22 * m0._m03 - m0._m02 * m0._m23 ) + m0._m01 * ( m0._m32 * m0._m23 - m0._m22 * m0._m33 ),
		 m0._m31 * ( m0._m02 * m0._m13 - m0._m12 * m0._m03 ) + m0._m01 * ( m0._m12 * m0._m33 - m0._m32 * m0._m13 ) + m0._m11 * ( m0._m32 * m0._m03 - m0._m02 * m0._m33 ),
		 m0._m01 * ( m0._m22 * m0._m13 - m0._m12 * m0._m23 ) + m0._m11 * ( m0._m02 * m0._m23 - m0._m22 * m0._m03 ) + m0._m21 * ( m0._m12 * m0._m03 - m0._m02 * m0._m13 ),

		 m0._m12 * ( m0._m20 * m0._m33 - m0._m30 * m0._m23 ) + m0._m22 * ( m0._m30 * m0._m13 - m0._m10 * m0._m33 ) + m0._m32 * ( m0._m10 * m0._m23 - m0._m20 * m0._m13 ),
		 m0._m22 * ( m0._m00 * m0._m33 - m0._m30 * m0._m03 ) + m0._m32 * ( m0._m20 * m0._m03 - m0._m00 * m0._m23 ) + m0._m02 * ( m0._m30 * m0._m23 - m0._m20 * m0._m33 ),
		 m0._m32 * ( m0._m00 * m0._m13 - m0._m10 * m0._m03 ) + m0._m02 * ( m0._m10 * m0._m33 - m0._m30 * m0._m13 ) + m0._m12 * ( m0._m30 * m0._m03 - m0._m00 * m0._m33 ),
		 m0._m02 * ( m0._m20 * m0._m13 - m0._m10 * m0._m23 ) + m0._m12 * ( m0._m00 * m0._m23 - m0._m20 * m0._m03 ) + m0._m22 * ( m0._m10 * m0._m03 - m0._m00 * m0._m13 ),

		 m0._m13 * ( m0._m20 * m0._m31 - m0._m30 * m0._m21 ) + m0._m23 * ( m0._m30 * m0._m11 - m0._m10 * m0._m31 ) + m0._m33 * ( m0._m10 * m0._m21 - m0._m20 * m0._m11 ),
		 m0._m23 * ( m0._m00 * m0._m31 - m0._m30 * m0._m01 ) + m0._m33 * ( m0._m20 * m0._m01 - m0._m00 * m0._m21 ) + m0._m03 * ( m0._m30 * m0._m21 - m0._m20 * m0._m31 ),
		 m0._m33 * ( m0._m00 * m0._m11 - m0._m10 * m0._m01 ) + m0._m03 * ( m0._m10 * m0._m31 - m0._m30 * m0._m11 ) + m0._m13 * ( m0._m30 * m0._m01 - m0._m00 * m0._m31 ),
		 m0._m03 * ( m0._m20 * m0._m11 - m0._m10 * m0._m21 ) + m0._m13 * ( m0._m00 * m0._m21 - m0._m20 * m0._m01 ) + m0._m23 * ( m0._m10 * m0._m01 - m0._m00 * m0._m11 ),

		 m0._m10 * ( m0._m31 * m0._m22 - m0._m21 * m0._m32 ) + m0._m20 * ( m0._m11 * m0._m32 - m0._m31 * m0._m12 ) + m0._m30 * ( m0._m21 * m0._m12 - m0._m11 * m0._m22 ),
		 m0._m20 * ( m0._m31 * m0._m02 - m0._m01 * m0._m32 ) + m0._m30 * ( m0._m01 * m0._m22 - m0._m21 * m0._m02 ) + m0._m00 * ( m0._m21 * m0._m32 - m0._m31 * m0._m22 ),
		 m0._m30 * ( m0._m11 * m0._m02 - m0._m01 * m0._m12 ) + m0._m00 * ( m0._m31 * m0._m12 - m0._m11 * m0._m32 ) + m0._m10 * ( m0._m01 * m0._m32 - m0._m31 * m0._m02 ),
		 m0._m00 * ( m0._m11 * m0._m22 - m0._m21 * m0._m12 ) + m0._m10 * ( m0._m21 * m0._m02 - m0._m01 * m0._m22 ) + m0._m20 * ( m0._m01 * m0._m12 - m0._m11 * m0._m02 )
		 );
	Scale( det );
}

// -----------------------------------------------------------------
// Matrix To Quaternion
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::MatrixToQuaternion( Quaternion* q ) const
{
	Matrix44AOS mat(*this);
	mat.Normalize();
	
	const f32 tr = mat._m00 + mat._m11 + mat._m22;
	f32 ww, max;
	s32 index;

	if( tr >= 0.0f ){
		ww = Math::FSqrt( tr + 1.0f );
		q->SetW( ww * 0.5f );
		ww = -0.5f / ww;

		q->SetX( ( mat._m21 - mat._m12 ) * ww );
		q->SetY( ( mat._m02 - mat._m20 ) * ww );
		q->SetZ( ( mat._m10 - mat._m01 ) * ww );
		//		q->Normalize();
		return;
	}

	index = 0;
	max = mat._m00;
	if( mat._m11 > max ){
		index = 1;
		max = mat._m11;
	}
	if( mat._m22 > max ){
		index = 2;
	}

	switch( index ){
	case 0:
		ww = Math::FSqrt( mat._m00 - mat._m11 - mat._m22 + 1.0f );
		q->SetX( ww * 0.5f );
		ww = 0.5f / ww;
		q->SetY( ( mat._m01 + mat._m10 ) * ww );
		q->SetZ( ( mat._m20 + mat._m02 ) * ww );
		q->SetW( ( mat._m12 - mat._m21 ) * ww );
		break;
	case 1:
		ww = Math::FSqrt( mat._m11 - mat._m00 - mat._m22 + 1.0f );
		q->SetY( ww * 0.5f );
		ww = 0.5f / ww;
		q->SetZ( ( mat._m12 + mat._m21 ) * ww );
		q->SetX( ( mat._m01 + mat._m10 ) * ww );
		q->SetW( ( mat._m20 - mat._m02 ) * ww );
		break;
	case 2:
		ww = Math::FSqrt( mat._m22 - mat._m00 - mat._m11 + 1.0f );
		q->SetZ( ww * 0.5f );
		ww = 0.5f / ww;
		q->SetX( ( mat._m20 + mat._m02 ) * ww );
		q->SetY( ( mat._m12 + mat._m21 ) * ww );
		q->SetW( ( mat._m01 - mat._m10 ) * ww );
	}
	//	q->Normalize();
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
gfl::core::Quaternion gfl::core::Matrix44AOS::MatrixToQuaternion( void ) const
{
	Quaternion qt;
	MatrixToQuaternion( &qt );
	return qt;
}

// -----------------------------------------------------------------
// Quaternion To Matrix
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::QuaternionToMatrix( const Quaternion& q )
{
	gfl::core::Quaternion qt;
//	qt.Normalize( q );
	const f32 scl = qt.NormalizeRet( q );

	const f32 x = qt.GetX( );
	const f32 y = qt.GetY( );
	const f32 z = qt.GetZ( );
	const f32 w = qt.GetW( );

	const f32 xx = x * x;
	const f32 yy = y * y;
	const f32 zz = z * z;
	const f32 xy = x * y;
	const f32 xz = x * z;
	const f32 yz = y * z;
	const f32 wx = w * x;
	const f32 wy = w * y;
	const f32 wz = w * z;

	_m00 = 1.0f - 2.0f * ( yy + zz );
	_m10 = 2.0f * ( xy - wz );
	_m20 = 2.0f * ( xz + wy );

	_m01 = 2.0f * ( xy + wz );
	_m11 = 1.0f - 2.0f * ( xx + zz );
	_m21 = 2.0f * ( yz - wx );

	_m02 = 2.0f * ( xz - wy );
	_m12 = 2.0f * ( yz + wx );
	_m22 = 1.0f - 2.0f * ( xx + yy );
	
	if( !gfl::core::Math::IsSimilar(scl,1.0f,0.01f) ){
		Scale( scl );
	}
}

// -----------------------------------------------------------------
// GetRadian
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::GetRadian( f32* x, f32* y, f32* z ) const
{
	f32 sy = _m02;
	if( 1.0f <= sy ){
		*x = Math::FATan2( -_m10, _m11 );
		*y = -Math::HALF_PI;
		*z = 0.0f;
	} else if( sy <= -1.0f ){
		*x = Math::FATan2( _m10, _m11 );
		*y = Math::HALF_PI;
		*z = 0.0f;
#if 0
	} else if( sy == 0.0f ){
		*x = Math::FATan2( _m20, -_m10 );
		*y = 0.0f;
		*z = Math::FATan2( -_m10, _m11 );
#endif
	} else {
#if 0 // ジンバルロック対策？
		f32 ay = Math::FASin( -sy );
		static const f32 lim = gfl::core::Math::HALF_PI * 0.9f;

		if( lim < gfl::core::Math::FAbs( ay ) ){
			ay = ( 0.0f < ay ) ? lim : -lim;
		}
		f32 r_cy = gfl::core::Math::FRecip( gfl::core::Math::FCos( ay ) );

		*x = Math::FATan2( _m12 * r_cy, _m22 * r_cy );
		*y = ay;
		*z = Math::FATan2( _m01 * r_cy, _m00 * r_cy );
#else
		// 本当は _m12/GetScaleY(), _m22/GetScaleZ() だけど…
		*x = Math::FATan2( _m12, _m22 );
		*y = Math::FASin( -sy/GetScaleX() );
		*z = Math::FATan2( _m01, _m00 );
#endif
	}
	//	gfl::core::Math::OptimizeRadian( x, y, z );
}

// -----------------------------------------------------------------
// GetRadian
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::GetRadianYXZ( f32* x, f32* y, f32* z ) const
{
#if 0
	f32 sx = _m2, 1 );
	if( 1.0f <= sx ){
		_m2, 1, 1.0f );
	} else if( sx <= -1.0f ){
		_m2, 1, -1.0f );
	}
	if( x ){
		*x = Math::FASin( -sx );
	}
	*y = Math::FATan2( _m20, _m22 );
	if( z ){
		*z = Math::FATan2( _m01, _m11 );
		//		gfl::core::Math::OptimizeRadian( x, y, z );
	}
#else
	f32 sx = _m21;
	if( 1.0f <= sx ){
		if( x ){
			*x = -Math::HALF_PI;
		}
		*y = 0.0f;
		if( z ){
			*z = Math::FATan2( -_m02, _m00 );
		}
	} else if( sx <= -1.0f ){
		if( x ){
			*x = Math::HALF_PI;
		}
		*y = 0.0f;
		if( z ){
			*z = Math::FATan2( _m02, _m00 );
		}
	} else {
		if( x ){
			*x = Math::FASin( -sx/GetScaleZ() );
		}
		*y = Math::FATan2( _m20, _m22 );
		if( z ){
			*z = Math::FATan2( _m01, _m11 );
		}
	}
#endif
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimZY( const T& tag, f32* zsin, f32* zcos, f32* ysin, f32* ycos ) const
{
	gfl::core::Vector loc;

	GlobalToLocal( &loc, tag );

	const f32 lx = loc.GetX( );
	const f32 ly = loc.GetY( );
	const f32 lz = loc.GetZ( );

	f32 tmp_0 = lx * lx + ly * ly;
	f32 tmp_1 = tmp_0 + lz * lz;
	tmp_0 = gfl::core::Math::FSqrt( tmp_0 );
	f32 tmp_2 = gfl::core::Math::FRecip( tmp_0 );
	tmp_1 = gfl::core::Math::FRecip( gfl::core::Math::FSqrt( tmp_1 ) );

	*zsin = ly * tmp_2;
	*zcos = lx * tmp_2;
	*ysin = -lz * tmp_1;
	*ycos = tmp_0 * tmp_1;
}

template void gfl::core::Matrix44AOS::AimZY( const gfl::core::Vector3& tag, f32* zsin, f32* zcos, f32* ysin, f32* ycos ) const;
template void gfl::core::Matrix44AOS::AimZY( const gfl::core::Vector4& tag, f32* zsin, f32* zcos, f32* ysin, f32* ycos ) const;

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimYZ( const T& tag, f32* ysin, f32* ycos, f32* zsin, f32* zcos, const bool reverse ) const
{
	gfl::core::Vector3 loc;

	GlobalToLocal( &loc, tag );
//	tag.PrintConsole( "tag" );
//	loc.PrintConsole( "loc" );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GetX( );
	const f32 ly = loc.GetY( );
	const f32 lz = loc.GetZ( );

	f32 tmp_0 = lx * lx + lz * lz;
	f32 tmp_1 = tmp_0 + ly * ly;
	tmp_0 = gfl::core::Math::FSqrt( tmp_0 );
	f32 tmp_2 = gfl::core::Math::FRecip( tmp_0 );
	tmp_1 = gfl::core::Math::FRecip( gfl::core::Math::FSqrt( tmp_1 ) );

	*ysin = -lz * tmp_2;
	*ycos = lx * tmp_2;
	*zsin = ly * tmp_1;
//	*zsin = ( ( 0.0f <= lx ) ? ly : -ly ) * tmp_1;
	*zcos = tmp_0 * tmp_1;

//	gfl::core::Debug::PrintConsole( "sc %f %f %f %f\n", *ysin, *ycos, *zsin, *zcos );
}

template void gfl::core::Matrix44AOS::AimYZ( const gfl::core::Vector3& tag, f32* ysin, f32* ycos, f32* zsin, f32* zcos, const bool reverse ) const;
template void gfl::core::Matrix44AOS::AimYZ( const gfl::core::Vector4& tag, f32* ysin, f32* ycos, f32* zsin, f32* zcos, const bool reverse ) const;

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimYX( const T& tag, f32* ysin, f32* ycos, f32* xsin, f32* xcos, const bool reverse ) const
{
	gfl::core::Vector loc;

	GlobalToLocal( &loc, tag );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GetX( );
	const f32 ly = loc.GetY( );
	const f32 lz = loc.GetZ( );

	f32 tmp_0 = lx * lx + lz * lz;
	f32 tmp_1 = tmp_0 + ly * ly;
	tmp_0 = gfl::core::Math::FSqrt( tmp_0 );
	f32 tmp_2 = gfl::core::Math::FRecip( tmp_0 );
	tmp_1 = gfl::core::Math::FRecip( gfl::core::Math::FSqrt( tmp_1 ) );

	*ysin = lx * tmp_2;
	*ycos = lz * tmp_2;
	*xsin = -ly * tmp_1;
	*xcos = tmp_0 * tmp_1;
}

template void gfl::core::Matrix44AOS::AimYX( const gfl::core::Vector3& tag, f32* ysin, f32* ycos, f32* xsin, f32* xcos, const bool reverse ) const;
template void gfl::core::Matrix44AOS::AimYX( const gfl::core::Vector4& tag, f32* ysin, f32* ycos, f32* xsin, f32* xcos, const bool reverse ) const;

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimXZ( const T& tag, f32* xsin, f32* xcos, f32* zsin, f32* zcos, const bool reverse ) const
{
	gfl::core::Vector loc;

	GlobalToLocal( &loc, tag );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GetX( );
	const f32 ly = loc.GetY( );
	const f32 lz = loc.GetZ( );

	f32 tmp_0 = ly * ly + lz * lz;
	f32 tmp_1 = tmp_0 + lx * lx;
	tmp_0 = gfl::core::Math::FSqrt( tmp_0 );
	f32 tmp_2 = gfl::core::Math::FRecip( tmp_0 );
	tmp_1 = gfl::core::Math::FRecip( gfl::core::Math::FSqrt( tmp_1 ) );

	*xsin = lz * tmp_2;
	*xcos = ly * tmp_2;
	*zsin = -lx * tmp_1;
	*zcos = tmp_0 * tmp_1;
}

template void gfl::core::Matrix44AOS::AimXZ( const gfl::core::Vector3& tag, f32* ysin, f32* ycos, f32* xsin, f32* xcos, const bool reverse ) const;
template void gfl::core::Matrix44AOS::AimXZ( const gfl::core::Vector4& tag, f32* ysin, f32* ycos, f32* xsin, f32* xcos, const bool reverse ) const;

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimXY( const T& tag, f32* xsin, f32* xcos, f32* zsin, f32* zcos, const bool reverse ) const
{
	gfl::core::Vector loc;

	GlobalToLocal( &loc, tag );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GetX( );
	const f32 ly = loc.GetY( );
	const f32 lz = loc.GetZ( );

	f32 tmp_0 = ly * ly + lz * lz;
	f32 tmp_1 = tmp_0 + lx * lx;
	tmp_0 = gfl::core::Math::FSqrt( tmp_0 );
	f32 tmp_2 = gfl::core::Math::FRecip( tmp_0 );
	tmp_1 = gfl::core::Math::FRecip( gfl::core::Math::FSqrt( tmp_1 ) );

	*xsin = -ly * tmp_2;
	*xcos = lz * tmp_2;
	*zsin = lx * tmp_1;
	*zcos = tmp_0 * tmp_1;
}

template void gfl::core::Matrix44AOS::AimXY( const gfl::core::Vector3& tag, f32* ysin, f32* ycos, f32* xsin, f32* xcos, const bool reverse ) const;
template void gfl::core::Matrix44AOS::AimXY( const gfl::core::Vector4& tag, f32* ysin, f32* ycos, f32* xsin, f32* xcos, const bool reverse ) const;

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimZY( const T& tag )
{
	f32 zsin, zcos, ysin, ycos;

	AimZY( tag, &zsin, &zcos, &ysin, &ycos );

	RotateZ( zsin, zcos );
	RotateY( ysin, ycos );
}

template void gfl::core::Matrix44AOS::AimZY( const gfl::core::Vector3& tag );
template void gfl::core::Matrix44AOS::AimZY( const gfl::core::Vector4& tag );

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimYZ( const T& tag, const bool reverse, const f32 rate )
{
	f32 zsin, zcos, ysin, ycos;

	AimYZ( tag, &ysin, &ycos, &zsin, &zcos, reverse );

	if( rate == 1.0f ){
		RotateY( ysin, ycos );
		RotateZ( zsin, zcos );
	} else {
		RotateY( gfl::core::Math::FATan2( ysin, ycos ) * rate );
		RotateZ( gfl::core::Math::FATan2( zsin, zcos ) * rate );
	}
}

template void gfl::core::Matrix44AOS::AimYZ( const Vector3& tag, const bool reverse, const f32 rate );
template void gfl::core::Matrix44AOS::AimYZ( const Vector4& tag, const bool reverse, const f32 rate );

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimYX( const T& tag, const bool reverse, const f32 rate )
{
	f32 xsin, xcos, ysin, ycos;

	AimYX( tag, &ysin, &ycos, &xsin, &xcos, reverse );

	if( rate == 1.0f ){
		RotateY( ysin, ycos );
		RotateX( xsin, xcos );
	} else {
		RotateY( gfl::core::Math::FATan2( ysin, ycos ) * rate );
		RotateX( gfl::core::Math::FATan2( xsin, xcos ) * rate );
	}
}

template void gfl::core::Matrix44AOS::AimYX( const gfl::core::Vector3& tag, const bool reverse, const f32 rate );
template void gfl::core::Matrix44AOS::AimYX( const gfl::core::Vector4& tag, const bool reverse, const f32 rate );

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimXZ( const T& tag, const bool reverse, const f32 rate )
{
	f32 xsin, xcos, zsin, zcos;

	AimXZ( tag, &xsin, &xcos, &zsin, &zcos, reverse );

	if( rate == 1.0f ){
		RotateX( xsin, xcos );
		RotateZ( zsin, zcos );
	} else {
		RotateX( gfl::core::Math::FATan2( xsin, xcos ) * rate );
		RotateZ( gfl::core::Math::FATan2( zsin, zcos ) * rate );
	}
}

template void gfl::core::Matrix44AOS::AimXZ( const gfl::core::Vector3& tag, const bool reverse, const f32 rate );
template void gfl::core::Matrix44AOS::AimXZ( const gfl::core::Vector4& tag, const bool reverse, const f32 rate );

// -----------------------------------------------------------------
// -----------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::AimXY( const T& tag, const bool reverse, const f32 rate )
{
	f32 xsin, xcos, zsin, zcos;

	AimXY( tag, &xsin, &xcos, &zsin, &zcos, reverse );

	if( rate == 1.0f ){
		RotateX( xsin, xcos );
		RotateY( zsin, zcos );
	} else {
		RotateX( gfl::core::Math::FATan2( xsin, xcos ) * rate );
		RotateY( gfl::core::Math::FATan2( zsin, zcos ) * rate );
	}
}

template void gfl::core::Matrix44AOS::AimXY( const gfl::core::Vector3& tag, const bool reverse, const f32 rate );
template void gfl::core::Matrix44AOS::AimXY( const gfl::core::Vector4& tag, const bool reverse, const f32 rate );

// -----------------------------------------------------------------
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::AimX( f32* xsin, f32* xcos )
{
	const f32 x = _m20;
	const f32 y = _m21;
	const f32 z = _m22;

	f32 xz2 = x * x + z*z;
	//	f32 r_1 = gfl::core::Math::FRecip( gfl::core::Math::FSqrt(xz2 + y * y) );
	//	f32 r_1 = gfl::core::Math::FRecip( xz2 + y * y );

	*xsin = y; //*r_1;
	*xcos = gfl::core::Math::FSqrt( xz2 ); // * r_1;
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
void gfl::core::Matrix44AOS::AimX( void )
{
	f32 xsin, xcos;
	AimX( &xsin, &xcos );
	RotateX( xsin, xcos );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#if 0 //
void gfl::core::Matrix44AOS::Clamp( const Matrix44AOS& parent, const Vector3& low, const Vector3& high )
{
	gfl::core::Vector3 ang;
	gfl::core::Matrix loc;
	loc.Mul33( parent.GetTranspose33( ), *this );
	loc.GetRadian( &ang );

	ang.Clamp( low, high );

	Set33( parent );
	RotateZYX( ang );
}
#endif
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
template <typename T>
void gfl::core::Matrix44AOS::GetPositionByLowestY( T* res, const f32 y0, const f32 r ) const
{
	// 元々の骨の向きの単位ベクトルを求める
	gfl::core::Vector3 v;
	ApplyX_1( &v );

	const f32 y = GetTransY( ) - y0;
	f32 r_y2 = ( r * r - y * y );
	// 半径より高さが高い場合
	if( r_y2 < 0.0f ){
		r_y2 = 0.0f;
	}
	const f32 l = v._x;
	const f32 n = v._z;
	const f32 l_2 = l*l;
	// ローカルX座標
	f32 xl = gfl::core::Math::FSqrt( ( l_2 * r_y2 ) / ( l_2 + n * n ) );
	f32 zl = ( n * xl ) / l;

	// 内積が負の場合は別解
	if( xl * l + zl * n < 0.0f ){
		xl = -xl;
		zl = -zl;
	}

	res->SetX( xl + GetTransX( ) );
	//	res->SetY( y0 );
	res->SetZ( zl + GetTransZ( ) );
}

template void gfl::core::Matrix44AOS::GetPositionByLowestY( gfl::core::Vector3* res, const f32 y0, const f32 r ) const;
template void gfl::core::Matrix44AOS::GetPositionByLowestY( gfl::core::Vector4* res, const f32 y0, const f32 r ) const;

#ifdef GFL_PLATFORM_3DS
//==============================================================================
//==============================================================================
void gfl::core::Matrix44AOS::FromSOA( const gfl::math::MTX33& src )
{
	_m00 = src.m[0][0];
	_m10 = src.m[0][1];
	_m20 = src.m[0][2];
	_m01 = src.m[1][0];
	_m11 = src.m[1][1];
	_m21 = src.m[1][2];
	_m02 = src.m[2][0];
	_m12 = src.m[2][1];
	_m22 = src.m[2][2];
	_m03 = _m13 = _m23 = _m30 = _m31 = _m32 = 0.0f;
	_m33 = 1.0f;
}
//==============================================================================
//==============================================================================
void gfl::core::Matrix44AOS::ToSOA( gfl::math::MTX33* dst ) const
{
	dst->m[0][0] = _m00;
	dst->m[0][1] = _m10;
	dst->m[0][2] = _m20;
	dst->m[1][0] = _m01;
	dst->m[1][1] = _m11;
	dst->m[1][2] = _m21;
	dst->m[2][0] = _m02;
	dst->m[2][1] = _m12;
	dst->m[2][2] = _m22;
}
//==============================================================================
//==============================================================================
void gfl::core::Matrix44AOS::FromSOA( const gfl::math::MTX34& src )
{
	_m00 = src.m[0][0];
	_m10 = src.m[0][1];
	_m20 = src.m[0][2];
	_m30 = src.m[0][3];
	_m01 = src.m[1][0];
	_m11 = src.m[1][1];
	_m21 = src.m[1][2];
	_m31 = src.m[1][3];
	_m02 = src.m[2][0];
	_m12 = src.m[2][1];
	_m22 = src.m[2][2];
	_m32 = src.m[2][3];
	_m03 = _m13 = _m23 = 0.0f;
	_m33 = 1.0f;
}
//==============================================================================
//==============================================================================
void gfl::core::Matrix44AOS::ToSOA( gfl::math::MTX34* dst ) const
{
	dst->m[0][0] = _m00;
	dst->m[0][1] = _m10;
	dst->m[0][2] = _m20;
	dst->m[0][3] = _m30;
	dst->m[1][0] = _m01;
	dst->m[1][1] = _m11;
	dst->m[1][2] = _m21;
	dst->m[1][3] = _m31;
	dst->m[2][0] = _m02;
	dst->m[2][1] = _m12;
	dst->m[2][2] = _m22;
	dst->m[2][3] = _m32;
}
#endif

//==============================================================================
//==============================================================================
gfl::core::Matrix44AOS& gfl::core::Matrix44AOS::Slerp(const Matrix44AOS& m0, const Matrix44AOS& m1, const f32 rate)
{
	gfl::core::Quaternion res;
//	m0.MatrixToQuaternion().PrintConsole("m0");
//	m1.MatrixToQuaternion().PrintConsole("m1");
	res.Slerp(m0.MatrixToQuaternion(),m1.MatrixToQuaternion(),rate);
//	res.PrintConsole("q");
	res.QuaternionToMatrix(this);
	return *this;
}

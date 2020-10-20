// マトリックス演算系　※汎用性重視で各ハードウェアに対して最適化はしていません

namespace gfl {
namespace core {

// =================================================================
// Matrix44Aos
// =================================================================
// ---------------------------------------------------------------------------------
// LookAtMatrix生成
// ---------------------------------------------------------------------------------
MatrixAos44 MatrixAos44::GetLookAt( const Vector& eye, const Vector& aim, const Vector& up )
{
	return GetLookAtGL( eye, aim, up );
}

// ---------------------------------------------------------------------------------
// OpenGL互換 LookAtMatrix生成
// ---------------------------------------------------------------------------------
MatrixAos44 MatrixAos44::GetLookAtGL( const Vector& eye, const Vector& aim, const Vector& up )
{
	Vector d( aim - eye );
	d.Normalize( );

	Vector s( d );
	s.Cross( up );
	s.Normalize( );

	Vector r( s );
	r.Cross( d );

	MatrixAos44 mat;

	mat.Set( s.GFL_VECTOR_X, r.GFL_VECTOR_X, -d.GFL_VECTOR_X, 0.0f,
			 s.GFL_VECTOR_Y, r.GFL_VECTOR_Y, -d.GFL_VECTOR_Y, 0.0f,
			 s.GFL_VECTOR_Z, r.GFL_VECTOR_Z, -d.GFL_VECTOR_Z, 0.0f,
			 -eye.Dot( s ), -eye.Dot( r ), eye.Dot( d ), 1.0f );

	//mat.Inverse();

	return mat;
}

// ---------------------------------------------------------------------------------
// PerspectiveMatrix生成
// ---------------------------------------------------------------------------------
MatrixAos44 MatrixAos44::GetPerspective( float fovy, float aspect, float zNear, float zFar )
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
MatrixAos44 MatrixAos44::GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar )
{
	f32 f, rangeInv;

	f = tanf( ( Math::HALF_PI ) - fovy * 0.5f );
	//f = Math::FCos( fovy * 0.5f ); // これじゃ駄目な場合もあるらしい
	rangeInv = 1.0f / ( zNear - zFar );

	MatrixAos44 mat;

	mat.Set( f / aspect, 0.0f, 0.0f, 0.0f,
			 0.0f, f, 0.0f, 0.0f,
			 0.0f, 0.0f, ( zNear + zFar ) * rangeInv, -1.0f,
			 0.0f, 0.0f, zNear * zFar * rangeInv * 2.0f, 0.0f );

	return mat;
}

// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void MatrixAos44::Set( const MatrixAos44& m )
{
#if 0 // べたに書くとこうなる
	GFL_MTX_00 = m.GFL_MTX_00;
	GFL_MTX_01 = m.GFL_MTX_01;
	GFL_MTX_02 = m.GFL_MTX_02;
	GFL_MTX_03 = m.GFL_MTX_03;
	GFL_MTX_10 = m.GFL_MTX_10;
	GFL_MTX_11 = m.GFL_MTX_11;
	GFL_MTX_12 = m.GFL_MTX_12;
	GFL_MTX_13 = m.GFL_MTX_13;
	GFL_MTX_20 = m.GFL_MTX_20;
	GFL_MTX_21 = m.GFL_MTX_21;
	GFL_MTX_22 = m.GFL_MTX_22;
	GFL_MTX_23 = m.GFL_MTX_23;
	GFL_MTX_30 = m.GFL_MTX_30;
	GFL_MTX_31 = m.GFL_MTX_31;
	GFL_MTX_32 = m.GFL_MTX_32;
	GFL_MTX_33 = m.GFL_MTX_33;
#else // まとめて転送可能な場合にはこちらが高速
	SetRow( 0, m.GetRow( 0 ) );
	SetRow( 1, m.GetRow( 1 ) );
	SetRow( 2, m.GetRow( 2 ) );
	SetRow( 3, m.GetRow( 3 ) );
#endif
}
// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void MatrixAos44::Set( const MatrixAos43& m )
{
#if 0 // べたに書くとこうなる
	GFL_MTX_00 = m.GFL_MTX_00;
	GFL_MTX_01 = m.GFL_MTX_01;
	GFL_MTX_02 = m.GFL_MTX_02;
	GFL_MTX_03 = m.GFL_MTX_03;
	GFL_MTX_10 = m.GFL_MTX_10;
	GFL_MTX_11 = m.GFL_MTX_11;
	GFL_MTX_12 = m.GFL_MTX_12;
	GFL_MTX_13 = m.GFL_MTX_13;
	GFL_MTX_20 = m.GFL_MTX_20;
	GFL_MTX_21 = m.GFL_MTX_21;
	GFL_MTX_22 = m.GFL_MTX_22;
	GFL_MTX_23 = m.GFL_MTX_23;
	GFL_MTX_30 = m.GFL_MTX_30;
	GFL_MTX_31 = m.GFL_MTX_31;
	GFL_MTX_32 = m.GFL_MTX_32;
	GFL_MTX_33 = m.GFL_MTX_33;
#else // まとめて転送可能な場合にはこちらが高速
	SetRow( 0, m.GetRow( 0 ), 0.0f );
	SetRow( 1, m.GetRow( 1 ), 0.0f );
	SetRow( 2, m.GetRow( 2 ), 0.0f );
	SetRow( 3, m.GetRow( 3 ), 1.0f );
#endif
}

// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void MatrixAos44::Set( f32 m0, f32 m1, f32 m2, f32 m3,
					   f32 m4, f32 m5, f32 m6, f32 m7,
					   f32 m8, f32 m9, f32 m10, f32 m11,
					   f32 m12, f32 m13, f32 m14, f32 m15 )
{
	GFL_MTX_00 = m0;
	GFL_MTX_01 = m1;
	GFL_MTX_02 = m2;
	GFL_MTX_03 = m3;
	GFL_MTX_10 = m4;
	GFL_MTX_11 = m5;
	GFL_MTX_12 = m6;
	GFL_MTX_13 = m7;
	GFL_MTX_20 = m8;
	GFL_MTX_21 = m9;
	GFL_MTX_22 = m10;
	GFL_MTX_23 = m11;
	GFL_MTX_30 = m12;
	GFL_MTX_31 = m13;
	GFL_MTX_32 = m14;
	GFL_MTX_33 = m15;
}

// -----------------------------------------------------------------
// SetUnit
// -----------------------------------------------------------------
void MatrixAos44::SetUnit( void )
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
	SetRow( 0, Vector4::GetX1( ) );
	SetRow( 1, Vector4::GetY1( ) );
	SetRow( 2, Vector4::GetZ1( ) );
	SetRow( 3, Vector4::GetW1( ) );
#endif
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void MatrixAos44::Transpose( const MatrixAos44& m )
{
	GFL_MTX_00 = m.GFL_MTX_00;
	GFL_MTX_01 = m.GFL_MTX_10;
	GFL_MTX_02 = m.GFL_MTX_20;
	GFL_MTX_03 = m.GFL_MTX_30;
	GFL_MTX_10 = m.GFL_MTX_01;
	GFL_MTX_11 = m.GFL_MTX_11;
	GFL_MTX_12 = m.GFL_MTX_21;
	GFL_MTX_13 = m.GFL_MTX_31;
	GFL_MTX_20 = m.GFL_MTX_02;
	GFL_MTX_21 = m.GFL_MTX_12;
	GFL_MTX_22 = m.GFL_MTX_22;
	GFL_MTX_23 = m.GFL_MTX_32;
	GFL_MTX_30 = m.GFL_MTX_03;
	GFL_MTX_31 = m.GFL_MTX_13;
	GFL_MTX_32 = m.GFL_MTX_23;
	GFL_MTX_33 = m.GFL_MTX_33;
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void MatrixAos44::Transpose( void )
{
	Math::Swap( GFL_MTX_01, GFL_MTX_10 );
	Math::Swap( GFL_MTX_02, GFL_MTX_20 );
	Math::Swap( GFL_MTX_03, GFL_MTX_30 );
	Math::Swap( GFL_MTX_12, GFL_MTX_21 );
	Math::Swap( GFL_MTX_13, GFL_MTX_31 );
	Math::Swap( GFL_MTX_23, GFL_MTX_32 );
}

// -----------------------------------------------------------------
// Normalize
// -----------------------------------------------------------------
void MatrixAos44::Normalize( VectorNative* scl )
{
	gfl::core::Vector4 vx, vy, vz;

	vx = GetRow( 0 );
	vy = GetRow( 1 );
	vz = GetRow( 2 );

	if( !scl ){
		vx.Normalize( );
		vy.Normalize( );
		vz.Normalize( );
	} else {
		scl->Set( vx.NormalizeRet( ),
				 vy.NormalizeRet( ),
				 vz.NormalizeRet( ) );
	}

	SetRow( 0, vx );
	SetRow( 1, vy );
	SetRow( 2, vz );
}

// -----------------------------------------------------------------
// Print
// -----------------------------------------------------------------
void MatrixAos44::PrintConsole( void ) const
{
	Debug::PrintConsole( "%ff, %ff, %ff, %ff,\n%ff, %ff, %ff, %ff,\n%ff, %ff, %ff, %ff,\n%ff, %ff, %ff, %ff\n",
						 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, GFL_MTX_03,
						 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, GFL_MTX_13,
						 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, GFL_MTX_23,
						 GFL_MTX_30, GFL_MTX_31, GFL_MTX_32, GFL_MTX_33 );
}

// -----------------------------------------------------------------
// Set Rotate X
// -----------------------------------------------------------------
void MatrixAos44::SetRotateX( const f32 s, const f32 c )
{
	SetRow( 0, Vector4::s_X1 );

	GFL_MTX_10 = 0.0f;
	GFL_MTX_11 = c;
	GFL_MTX_12 = s;
	GFL_MTX_13 = 0.0f;

	GFL_MTX_20 = 0.0f;
	GFL_MTX_21 = -s;
	GFL_MTX_22 = c;
	GFL_MTX_23 = 0.0f;
}

// -----------------------------------------------------------------
// Set Rotate Y
// -----------------------------------------------------------------
void MatrixAos44::SetRotateY( const f32 s, const f32 c )
{
	GFL_MTX_00 = c;
	GFL_MTX_01 = 0.0f;
	GFL_MTX_02 = -s;
	GFL_MTX_03 = 0.0f;

	SetRow( 1, Vector4::s_Y1 );

	GFL_MTX_20 = s;
	GFL_MTX_21 = 0.0f;
	GFL_MTX_22 = c;
	GFL_MTX_23 = 0.0f;
}

// -----------------------------------------------------------------
// Set Rotate Z
// -----------------------------------------------------------------
void MatrixAos44::SetRotateZ( const f32 s, const f32 c )
{
	GFL_MTX_00 = c;
	GFL_MTX_01 = s;
	GFL_MTX_02 = 0.0f;
	GFL_MTX_03 = 0.0f;

	GFL_MTX_10 = -s;
	GFL_MTX_11 = c;
	GFL_MTX_12 = 0.0f;
	GFL_MTX_13 = 0.0f;

	SetRow( 2, Vector4::s_Z1 );
}

// -----------------------------------------------------------------
// Set Scale
// -----------------------------------------------------------------
void MatrixAos44::SetScale( const f32 x, const f32 y, const f32 z )
{
	GFL_MTX_01 =
		GFL_MTX_02 =
		GFL_MTX_03 =
		GFL_MTX_10 =
		GFL_MTX_12 =
		GFL_MTX_13 =
		GFL_MTX_20 =
		GFL_MTX_21 =
		GFL_MTX_23 = 0.0f;
	GFL_MTX_00 = x;
	GFL_MTX_11 = y;
	GFL_MTX_22 = z;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void MatrixAos44::RotateX( const MatrixAos44& m0, const f32 s, const f32 c )
{
	GFL_MTX_00 = m0.GFL_MTX_00;
	GFL_MTX_01 = m0.GFL_MTX_01;
	GFL_MTX_02 = m0.GFL_MTX_02;
	GFL_MTX_03 = m0.GFL_MTX_03;
	GFL_MTX_30 = m0.GFL_MTX_30;
	GFL_MTX_31 = m0.GFL_MTX_31;
	GFL_MTX_32 = m0.GFL_MTX_32;
	GFL_MTX_33 = m0.GFL_MTX_33;
	GFL_MTX_13 = m0.GFL_MTX_13;
	GFL_MTX_23 = m0.GFL_MTX_23;
	GFL_MTX_10 = m0.GFL_MTX_20 * s + m0.GFL_MTX_10 * c;
	GFL_MTX_11 = m0.GFL_MTX_21 * s + m0.GFL_MTX_11 * c;
	GFL_MTX_12 = m0.GFL_MTX_22 * s + m0.GFL_MTX_12 * c;
	GFL_MTX_20 = m0.GFL_MTX_20 * c - m0.GFL_MTX_10 * s;
	GFL_MTX_21 = m0.GFL_MTX_21 * c - m0.GFL_MTX_11 * s;
	GFL_MTX_22 = m0.GFL_MTX_22 * c - m0.GFL_MTX_12 * s;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void MatrixAos44::RotateX( const f32 s, const f32 c )
{
	const f32 m10 = GFL_MTX_10, m20 = GFL_MTX_20;
	const f32 m11 = GFL_MTX_11, m21 = GFL_MTX_21;
	const f32 m12 = GFL_MTX_12, m22 = GFL_MTX_22;

	GFL_MTX_10 = m20 * s + m10 * c;
	GFL_MTX_11 = m21 * s + m11 * c;
	GFL_MTX_12 = m22 * s + m12 * c;
	GFL_MTX_20 = m20 * c - m10 * s;
	GFL_MTX_21 = m21 * c - m11 * s;
	GFL_MTX_22 = m22 * c - m12 * s;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void MatrixAos44::RotateX_HalfPi( void )
{
	const f32 m10 = GFL_MTX_10, m20 = GFL_MTX_20;
	const f32 m11 = GFL_MTX_11, m21 = GFL_MTX_21;
	const f32 m12 = GFL_MTX_12, m22 = GFL_MTX_22;

	GFL_MTX_10 = m20;
	GFL_MTX_11 = m21;
	GFL_MTX_12 = m22;
	GFL_MTX_20 = -m10;
	GFL_MTX_21 = -m11;
	GFL_MTX_22 = -m12;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void MatrixAos44::RotateX_MinusHalfPi( void )
{
	const f32 m10 = GFL_MTX_10, m20 = GFL_MTX_20;
	const f32 m11 = GFL_MTX_11, m21 = GFL_MTX_21;
	const f32 m12 = GFL_MTX_12, m22 = GFL_MTX_22;

	GFL_MTX_10 = -m20;
	GFL_MTX_11 = -m21;
	GFL_MTX_12 = -m22;
	GFL_MTX_20 = m10;
	GFL_MTX_21 = m11;
	GFL_MTX_22 = m12;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void MatrixAos44::RotateX_OnePi( void )
{
	const f32 m10 = GFL_MTX_10, m20 = GFL_MTX_20;
	const f32 m11 = GFL_MTX_11, m21 = GFL_MTX_21;
	const f32 m12 = GFL_MTX_12, m22 = GFL_MTX_22;

	GFL_MTX_10 = -m10;
	GFL_MTX_11 = -m11;
	GFL_MTX_12 = -m12;
	GFL_MTX_20 = -m20;
	GFL_MTX_21 = -m21;
	GFL_MTX_22 = -m22;
}

// -----------------------------------------------------------------
// RotateX Reverse
// -----------------------------------------------------------------
void MatrixAos44::RotateX_Reverse( const f32 s, const f32 c )
{
	const f32 m01 = GFL_MTX_01, m02 = GFL_MTX_02;
	const f32 m11 = GFL_MTX_11, m12 = GFL_MTX_12;
	const f32 m21 = GFL_MTX_21, m22 = GFL_MTX_22;

	GFL_MTX_01 = m01 * c - m02 * s;
	GFL_MTX_11 = m11 * c - m12 * s;
	GFL_MTX_21 = m21 * c - m22 * s;
	GFL_MTX_02 = m02 * c + m01 * s;
	GFL_MTX_12 = m12 * c + m11 * s;
	GFL_MTX_22 = m22 * c + m21 * s;
}

// -----------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------
void MatrixAos44::RotateY( const MatrixAos44& m0, const f32 s, const f32 c )
{
	GFL_MTX_10 = m0.GFL_MTX_10;
	GFL_MTX_11 = m0.GFL_MTX_11;
	GFL_MTX_12 = m0.GFL_MTX_12;
	GFL_MTX_13 = m0.GFL_MTX_13;
	GFL_MTX_30 = m0.GFL_MTX_30;
	GFL_MTX_31 = m0.GFL_MTX_31;
	GFL_MTX_32 = m0.GFL_MTX_32;
	GFL_MTX_33 = m0.GFL_MTX_33;
	GFL_MTX_03 = m0.GFL_MTX_03;
	GFL_MTX_23 = m0.GFL_MTX_23;
	GFL_MTX_20 = m0.GFL_MTX_00 * c + m0.GFL_MTX_20 * s;
	GFL_MTX_21 = m0.GFL_MTX_01 * c + m0.GFL_MTX_21 * s;
	GFL_MTX_22 = m0.GFL_MTX_02 * c + m0.GFL_MTX_22 * s;
	GFL_MTX_00 = m0.GFL_MTX_00 * c - m0.GFL_MTX_20 * s;
	GFL_MTX_01 = m0.GFL_MTX_01 * c - m0.GFL_MTX_21 * s;
	GFL_MTX_02 = m0.GFL_MTX_02 * c - m0.GFL_MTX_22 * s;
}

// -----------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------
void MatrixAos44::RotateY( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_20;
	const f32 m01 = GFL_MTX_01, m21 = GFL_MTX_21;
	const f32 m02 = GFL_MTX_02, m22 = GFL_MTX_22;

	GFL_MTX_20 = m00 * s + m20 * c;
	GFL_MTX_21 = m01 * s + m21 * c;
	GFL_MTX_22 = m02 * s + m22 * c;
	GFL_MTX_00 = m00 * c - m20 * s;
	GFL_MTX_01 = m01 * c - m21 * s;
	GFL_MTX_02 = m02 * c - m22 * s;
}

// -----------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------
void MatrixAos44::RotateY_HalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_20;
	const f32 m01 = GFL_MTX_01, m21 = GFL_MTX_21;
	const f32 m02 = GFL_MTX_02, m22 = GFL_MTX_22;

	GFL_MTX_20 = m00;
	GFL_MTX_21 = m01;
	GFL_MTX_22 = m02;
	GFL_MTX_00 = -m20;
	GFL_MTX_01 = -m21;
	GFL_MTX_02 = -m22;
}

// -----------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------
void MatrixAos44::RotateY_MinusHalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_20;
	const f32 m01 = GFL_MTX_01, m21 = GFL_MTX_21;
	const f32 m02 = GFL_MTX_02, m22 = GFL_MTX_22;

	GFL_MTX_20 = -m00;
	GFL_MTX_21 = -m01;
	GFL_MTX_22 = -m02;
	GFL_MTX_00 = m20;
	GFL_MTX_01 = m21;
	GFL_MTX_02 = m22;
}

// -----------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------
void MatrixAos44::RotateY_OnePi( void )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_20;
	const f32 m01 = GFL_MTX_01, m21 = GFL_MTX_21;
	const f32 m02 = GFL_MTX_02, m22 = GFL_MTX_22;

	GFL_MTX_20 = -m20;
	GFL_MTX_21 = -m21;
	GFL_MTX_22 = -m22;
	GFL_MTX_00 = -m00;
	GFL_MTX_01 = -m01;
	GFL_MTX_02 = -m02;
}

// -----------------------------------------------------------------
// Rotate Y Reverse
// -----------------------------------------------------------------
void MatrixAos44::RotateY_Reverse( const MatrixAos44& m0, const f32 s, const f32 c )
{
	const f32 m00 = m0.GFL_MTX_00, m02 = m0.GFL_MTX_02;
	const f32 m10 = m0.GFL_MTX_10, m12 = m0.GFL_MTX_12;
	const f32 m20 = m0.GFL_MTX_20, m22 = m0.GFL_MTX_22;

	GFL_MTX_01 = m0.GFL_MTX_01;
	GFL_MTX_03 = m0.GFL_MTX_03;
	GFL_MTX_11 = m0.GFL_MTX_11;
	GFL_MTX_13 = m0.GFL_MTX_13;
	GFL_MTX_21 = m0.GFL_MTX_21;
	GFL_MTX_23 = m0.GFL_MTX_23;
	GFL_MTX_31 = m0.GFL_MTX_31;
	GFL_MTX_33 = m0.GFL_MTX_33;

	GFL_MTX_00 = m00 * c + m02 * s;
	GFL_MTX_10 = m10 * c + m12 * s;
	GFL_MTX_20 = m20 * c + m22 * s;
	GFL_MTX_02 = m02 * c - m00 * s;
	GFL_MTX_12 = m12 * c - m10 * s;
	GFL_MTX_22 = m22 * c - m20 * s;

	const f32 x = m0.GFL_MTX_30;
	const f32 z = m0.GFL_MTX_32;
	GFL_MTX_30 = Math::GetX_RotateY_SinCosXZ( s, c, x, z );
	GFL_MTX_32 = Math::GetZ_RotateY_SinCosXZ( s, c, x, z );
}

// -----------------------------------------------------------------
// Rotate Y Reverse
// -----------------------------------------------------------------
void MatrixAos44::RotateY_Reverse( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m02 = GFL_MTX_02;
	const f32 m10 = GFL_MTX_10, m12 = GFL_MTX_12;
	const f32 m20 = GFL_MTX_20, m22 = GFL_MTX_22;

	GFL_MTX_00 = m00 * c + m02 * s;
	GFL_MTX_10 = m10 * c + m12 * s;
	GFL_MTX_20 = m20 * c + m22 * s;
	GFL_MTX_02 = m02 * c - m00 * s;
	GFL_MTX_12 = m12 * c - m10 * s;
	GFL_MTX_22 = m22 * c - m20 * s;
}

// -----------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------
void MatrixAos44::RotateZ( const MatrixAos44& m0, const f32 s, const f32 c )
{
	GFL_MTX_20 = m0.GFL_MTX_20;
	GFL_MTX_21 = m0.GFL_MTX_21;
	GFL_MTX_22 = m0.GFL_MTX_22;
	GFL_MTX_23 = m0.GFL_MTX_23;
	GFL_MTX_30 = m0.GFL_MTX_30;
	GFL_MTX_31 = m0.GFL_MTX_31;
	GFL_MTX_32 = m0.GFL_MTX_32;
	GFL_MTX_33 = m0.GFL_MTX_33;
	GFL_MTX_03 = m0.GFL_MTX_03;
	GFL_MTX_13 = m0.GFL_MTX_13;
	GFL_MTX_00 = m0.GFL_MTX_10 * s + m0.GFL_MTX_00 * c;
	GFL_MTX_01 = m0.GFL_MTX_11 * s + m0.GFL_MTX_01 * c;
	GFL_MTX_02 = m0.GFL_MTX_12 * s + m0.GFL_MTX_02 * c;
	GFL_MTX_10 = m0.GFL_MTX_10 * c - m0.GFL_MTX_00 * s;
	GFL_MTX_11 = m0.GFL_MTX_11 * c - m0.GFL_MTX_01 * s;
	GFL_MTX_12 = m0.GFL_MTX_12 * c - m0.GFL_MTX_02 * s;
}

// -----------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------
void MatrixAos44::RotateZ( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_10;
	const f32 m01 = GFL_MTX_01, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_02, m12 = GFL_MTX_12;

	GFL_MTX_00 = m10 * s + m00 * c;
	GFL_MTX_01 = m11 * s + m01 * c;
	GFL_MTX_02 = m12 * s + m02 * c;
	GFL_MTX_10 = m10 * c - m00 * s;
	GFL_MTX_11 = m11 * c - m01 * s;
	GFL_MTX_12 = m12 * c - m02 * s;
}

// -----------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------
void MatrixAos44::RotateZ_HalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_10;
	const f32 m01 = GFL_MTX_01, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_02, m12 = GFL_MTX_12;

	GFL_MTX_00 = m10;
	GFL_MTX_01 = m11;
	GFL_MTX_02 = m12;
	GFL_MTX_10 = -m00;
	GFL_MTX_11 = -m01;
	GFL_MTX_12 = -m02;
}

// -----------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------
void MatrixAos44::RotateZ_MinusHalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_10;
	const f32 m01 = GFL_MTX_01, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_02, m12 = GFL_MTX_12;

	GFL_MTX_00 = -m10;
	GFL_MTX_01 = -m11;
	GFL_MTX_02 = -m12;
	GFL_MTX_10 = m00;
	GFL_MTX_11 = m01;
	GFL_MTX_12 = m02;
}

// -----------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------
void MatrixAos44::RotateZ_OnePi( void )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_10;
	const f32 m01 = GFL_MTX_01, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_02, m12 = GFL_MTX_12;

	GFL_MTX_00 = -m00;
	GFL_MTX_01 = -m01;
	GFL_MTX_02 = -m02;
	GFL_MTX_10 = -m10;
	GFL_MTX_11 = -m11;
	GFL_MTX_12 = -m12;
}

// -----------------------------------------------------------------
// Rotate Z Reverese
// -----------------------------------------------------------------
void MatrixAos44::RotateZ_Reverse( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m01 = GFL_MTX_01;
	const f32 m10 = GFL_MTX_10, m11 = GFL_MTX_11;
	const f32 m20 = GFL_MTX_20, m21 = GFL_MTX_21;

	GFL_MTX_00 = m00 * c - m01 * s;
	GFL_MTX_10 = m10 * c - m11 * s;
	GFL_MTX_20 = m20 * c - m21 * s;
	GFL_MTX_01 = m00 * s + m01 * c;
	GFL_MTX_11 = m10 * s + m11 * c;
	GFL_MTX_12 = m20 * s + m21 * c;
}

// -----------------------------------------------------------------
// Mul33
// -----------------------------------------------------------------
void MatrixAos44::Mul33( const MatrixAos44& m0, const MatrixAos44& m1 )
{
	const f32 l_00 = m0.GFL_MTX_00, l_01 = m0.GFL_MTX_01, l_02 = m0.GFL_MTX_02;
	const f32 l_10 = m0.GFL_MTX_10, l_11 = m0.GFL_MTX_11, l_12 = m0.GFL_MTX_12;
	const f32 l_20 = m0.GFL_MTX_20, l_21 = m0.GFL_MTX_21, l_22 = m0.GFL_MTX_22;
	const f32 r_00 = m1.GFL_MTX_00, r_01 = m1.GFL_MTX_01, r_02 = m1.GFL_MTX_02;

	GFL_MTX_00 = l_00 * r_00 + l_10 * r_01 + l_20 * r_02;
	GFL_MTX_01 = l_01 * r_00 + l_11 * r_01 + l_21 * r_02;
	GFL_MTX_02 = l_02 * r_00 + l_12 * r_01 + l_22 * r_02;

	const f32 r_10 = m1.GFL_MTX_10, r_11 = m1.GFL_MTX_11, r_12 = m1.GFL_MTX_12;

	GFL_MTX_10 = l_00 * r_10 + l_10 * r_11 + l_20 * r_12;
	GFL_MTX_11 = l_01 * r_10 + l_11 * r_11 + l_21 * r_12;
	GFL_MTX_12 = l_02 * r_10 + l_12 * r_11 + l_22 * r_12;

	const f32 r_20 = m1.GFL_MTX_20, r_21 = m1.GFL_MTX_21, r_22 = m1.GFL_MTX_22;

	GFL_MTX_20 = l_00 * r_20 + l_10 * r_21 + l_20 * r_22;
	GFL_MTX_21 = l_01 * r_20 + l_11 * r_21 + l_21 * r_22;
	GFL_MTX_22 = l_02 * r_20 + l_12 * r_21 + l_22 * r_22;
}

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void MatrixAos44::Mul( const MatrixAos44& m0, const MatrixAos44& m1 )
{
	const f32 l_00 = m0.GFL_MTX_00, l_01 = m0.GFL_MTX_01, l_02 = m0.GFL_MTX_02, l_03 = m0.GFL_MTX_03;
	const f32 l_10 = m0.GFL_MTX_10, l_11 = m0.GFL_MTX_11, l_12 = m0.GFL_MTX_12, l_13 = m0.GFL_MTX_13;
	const f32 l_20 = m0.GFL_MTX_20, l_21 = m0.GFL_MTX_21, l_22 = m0.GFL_MTX_22, l_23 = m0.GFL_MTX_23;
	const f32 l_30 = m0.GFL_MTX_30, l_31 = m0.GFL_MTX_31, l_32 = m0.GFL_MTX_32, l_33 = m0.GFL_MTX_33;
	const f32 r_00 = m1.GFL_MTX_00, r_01 = m1.GFL_MTX_01, r_02 = m1.GFL_MTX_02, r_03 = m1.GFL_MTX_03;

	GFL_MTX_00 = l_00 * r_00 + l_10 * r_01 + l_20 * r_02 + l_30 * r_03;
	GFL_MTX_01 = l_01 * r_00 + l_11 * r_01 + l_21 * r_02 + l_31 * r_03;
	GFL_MTX_02 = l_02 * r_00 + l_12 * r_01 + l_22 * r_02 + l_32 * r_03;
	GFL_MTX_03 = l_03 * r_00 + l_13 * r_01 + l_23 * r_02 + l_33 * r_03;

	const f32 r_10 = m1.GFL_MTX_10, r_11 = m1.GFL_MTX_11, r_12 = m1.GFL_MTX_12, r_13 = m1.GFL_MTX_13;

	GFL_MTX_10 = l_00 * r_10 + l_10 * r_11 + l_20 * r_12 + l_30 * r_13;
	GFL_MTX_11 = l_01 * r_10 + l_11 * r_11 + l_21 * r_12 + l_31 * r_13;
	GFL_MTX_12 = l_02 * r_10 + l_12 * r_11 + l_22 * r_12 + l_32 * r_13;
	GFL_MTX_13 = l_03 * r_10 + l_13 * r_11 + l_23 * r_12 + l_33 * r_13;

	const f32 r_20 = m1.GFL_MTX_20, r_21 = m1.GFL_MTX_21, r_22 = m1.GFL_MTX_22, r_23 = m1.GFL_MTX_23;

	GFL_MTX_20 = l_00 * r_20 + l_10 * r_21 + l_20 * r_22 + l_30 * r_23;
	GFL_MTX_21 = l_01 * r_20 + l_11 * r_21 + l_21 * r_22 + l_31 * r_23;
	GFL_MTX_22 = l_02 * r_20 + l_12 * r_21 + l_22 * r_22 + l_32 * r_23;
	GFL_MTX_23 = l_03 * r_20 + l_13 * r_21 + l_23 * r_22 + l_33 * r_23;

	const f32 r_30 = m1.GFL_MTX_30, r_31 = m1.GFL_MTX_31, r_32 = m1.GFL_MTX_32, r_33 = m1.GFL_MTX_33;

	GFL_MTX_30 = l_00 * r_30 + l_10 * r_31 + l_20 * r_32 + l_30 * r_33;
	GFL_MTX_31 = l_01 * r_30 + l_11 * r_31 + l_21 * r_32 + l_31 * r_33;
	GFL_MTX_32 = l_02 * r_30 + l_12 * r_31 + l_22 * r_32 + l_32 * r_33;
	GFL_MTX_33 = l_03 * r_30 + l_13 * r_31 + l_23 * r_32 + l_33 * r_33;
}

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void MatrixAos44::Mul( const Quaternion& q0 )
{
	MatrixAos44 mat;
	mat.QuaternionToMatrix( q0 );
	Mul33( mat );
}

// -----------------------------------------------------------------
// Determinant33
// -----------------------------------------------------------------
f32 MatrixAos44::Determinant33( void ) const
{
	return(GFL_MTX_00 * GFL_MTX_11 - GFL_MTX_10 * GFL_MTX_01 )* GFL_MTX_22
		- ( GFL_MTX_00 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_01 ) * GFL_MTX_12
		+ ( GFL_MTX_10 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_11 ) * GFL_MTX_02;
}

// -----------------------------------------------------------------
// Determinant
// -----------------------------------------------------------------
f32 MatrixAos44::Determinant( void ) const
{
	return(GFL_MTX_00 * GFL_MTX_11 - GFL_MTX_10 * GFL_MTX_01 )*( GFL_MTX_22 * GFL_MTX_33 - GFL_MTX_32 * GFL_MTX_23 )
		-( GFL_MTX_00 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_01 )*( GFL_MTX_12 * GFL_MTX_33 - GFL_MTX_32 * GFL_MTX_13 )
		+( GFL_MTX_00 * GFL_MTX_31 - GFL_MTX_30 * GFL_MTX_01 )*( GFL_MTX_12 * GFL_MTX_23 - GFL_MTX_22 * GFL_MTX_13 )
		+( GFL_MTX_10 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_11 )*( GFL_MTX_02 * GFL_MTX_33 - GFL_MTX_32 * GFL_MTX_03 )
		-( GFL_MTX_10 * GFL_MTX_31 - GFL_MTX_30 * GFL_MTX_11 )*( GFL_MTX_02 * GFL_MTX_23 - GFL_MTX_22 * GFL_MTX_03 )
		+( GFL_MTX_20 * GFL_MTX_31 - GFL_MTX_30 * GFL_MTX_21 )*( GFL_MTX_02 * GFL_MTX_13 - GFL_MTX_12 * GFL_MTX_03 );
}

// -----------------------------------------------------------------
// Inverse33
// -----------------------------------------------------------------
void MatrixAos44::Inverse33( const MatrixAos44& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set33(
		 ( m0.GFL_MTX_11 * m0.GFL_MTX_22 - m0.GFL_MTX_21 * m0.GFL_MTX_12 ) * det,
		 ( m0.GFL_MTX_21 * m0.GFL_MTX_02 - m0.GFL_MTX_01 * m0.GFL_MTX_22 ) * det,
		 ( m0.GFL_MTX_01 * m0.GFL_MTX_12 - m0.GFL_MTX_11 * m0.GFL_MTX_02 ) * det,

		 ( m0.GFL_MTX_12 * m0.GFL_MTX_20 - m0.GFL_MTX_22 * m0.GFL_MTX_10 ) * det,
		 ( m0.GFL_MTX_22 * m0.GFL_MTX_00 - m0.GFL_MTX_02 * m0.GFL_MTX_20 ) * det,
		 ( m0.GFL_MTX_02 * m0.GFL_MTX_10 - m0.GFL_MTX_12 * m0.GFL_MTX_00 ) * det,

		 ( m0.GFL_MTX_10 * m0.GFL_MTX_21 - m0.GFL_MTX_20 * m0.GFL_MTX_11 ) * det,
		 ( m0.GFL_MTX_20 * m0.GFL_MTX_01 - m0.GFL_MTX_00 * m0.GFL_MTX_21 ) * det,
		 ( m0.GFL_MTX_00 * m0.GFL_MTX_11 - m0.GFL_MTX_10 * m0.GFL_MTX_01 ) * det
		 );
}

// -----------------------------------------------------------------
// Inverse
// -----------------------------------------------------------------
void MatrixAos44::Inverse( const MatrixAos44& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set(
		 m0.GFL_MTX_11 * ( m0.GFL_MTX_22 * m0.GFL_MTX_33 - m0.GFL_MTX_32 * m0.GFL_MTX_23 ) + m0.GFL_MTX_21 * ( m0.GFL_MTX_32 * m0.GFL_MTX_13 - m0.GFL_MTX_12 * m0.GFL_MTX_33 ) + m0.GFL_MTX_31 * ( m0.GFL_MTX_12 * m0.GFL_MTX_23 - m0.GFL_MTX_22 * m0.GFL_MTX_13 ),
		 m0.GFL_MTX_21 * ( m0.GFL_MTX_02 * m0.GFL_MTX_33 - m0.GFL_MTX_32 * m0.GFL_MTX_03 ) + m0.GFL_MTX_31 * ( m0.GFL_MTX_22 * m0.GFL_MTX_03 - m0.GFL_MTX_02 * m0.GFL_MTX_23 ) + m0.GFL_MTX_01 * ( m0.GFL_MTX_32 * m0.GFL_MTX_23 - m0.GFL_MTX_22 * m0.GFL_MTX_33 ),
		 m0.GFL_MTX_31 * ( m0.GFL_MTX_02 * m0.GFL_MTX_13 - m0.GFL_MTX_12 * m0.GFL_MTX_03 ) + m0.GFL_MTX_01 * ( m0.GFL_MTX_12 * m0.GFL_MTX_33 - m0.GFL_MTX_32 * m0.GFL_MTX_13 ) + m0.GFL_MTX_11 * ( m0.GFL_MTX_32 * m0.GFL_MTX_03 - m0.GFL_MTX_02 * m0.GFL_MTX_33 ),
		 m0.GFL_MTX_01 * ( m0.GFL_MTX_22 * m0.GFL_MTX_13 - m0.GFL_MTX_12 * m0.GFL_MTX_23 ) + m0.GFL_MTX_11 * ( m0.GFL_MTX_02 * m0.GFL_MTX_23 - m0.GFL_MTX_22 * m0.GFL_MTX_03 ) + m0.GFL_MTX_21 * ( m0.GFL_MTX_12 * m0.GFL_MTX_03 - m0.GFL_MTX_02 * m0.GFL_MTX_13 ),

		 m0.GFL_MTX_12 * ( m0.GFL_MTX_20 * m0.GFL_MTX_33 - m0.GFL_MTX_30 * m0.GFL_MTX_23 ) + m0.GFL_MTX_22 * ( m0.GFL_MTX_30 * m0.GFL_MTX_13 - m0.GFL_MTX_10 * m0.GFL_MTX_33 ) + m0.GFL_MTX_32 * ( m0.GFL_MTX_10 * m0.GFL_MTX_23 - m0.GFL_MTX_20 * m0.GFL_MTX_13 ),
		 m0.GFL_MTX_22 * ( m0.GFL_MTX_00 * m0.GFL_MTX_33 - m0.GFL_MTX_30 * m0.GFL_MTX_03 ) + m0.GFL_MTX_32 * ( m0.GFL_MTX_20 * m0.GFL_MTX_03 - m0.GFL_MTX_00 * m0.GFL_MTX_23 ) + m0.GFL_MTX_02 * ( m0.GFL_MTX_30 * m0.GFL_MTX_23 - m0.GFL_MTX_20 * m0.GFL_MTX_33 ),
		 m0.GFL_MTX_32 * ( m0.GFL_MTX_00 * m0.GFL_MTX_13 - m0.GFL_MTX_10 * m0.GFL_MTX_03 ) + m0.GFL_MTX_02 * ( m0.GFL_MTX_10 * m0.GFL_MTX_33 - m0.GFL_MTX_30 * m0.GFL_MTX_13 ) + m0.GFL_MTX_12 * ( m0.GFL_MTX_30 * m0.GFL_MTX_03 - m0.GFL_MTX_00 * m0.GFL_MTX_33 ),
		 m0.GFL_MTX_02 * ( m0.GFL_MTX_20 * m0.GFL_MTX_13 - m0.GFL_MTX_10 * m0.GFL_MTX_23 ) + m0.GFL_MTX_12 * ( m0.GFL_MTX_00 * m0.GFL_MTX_23 - m0.GFL_MTX_20 * m0.GFL_MTX_03 ) + m0.GFL_MTX_22 * ( m0.GFL_MTX_10 * m0.GFL_MTX_03 - m0.GFL_MTX_00 * m0.GFL_MTX_13 ),

		 m0.GFL_MTX_13 * ( m0.GFL_MTX_20 * m0.GFL_MTX_31 - m0.GFL_MTX_30 * m0.GFL_MTX_21 ) + m0.GFL_MTX_23 * ( m0.GFL_MTX_30 * m0.GFL_MTX_11 - m0.GFL_MTX_10 * m0.GFL_MTX_31 ) + m0.GFL_MTX_33 * ( m0.GFL_MTX_10 * m0.GFL_MTX_21 - m0.GFL_MTX_20 * m0.GFL_MTX_11 ),
		 m0.GFL_MTX_23 * ( m0.GFL_MTX_00 * m0.GFL_MTX_31 - m0.GFL_MTX_30 * m0.GFL_MTX_01 ) + m0.GFL_MTX_33 * ( m0.GFL_MTX_20 * m0.GFL_MTX_01 - m0.GFL_MTX_00 * m0.GFL_MTX_21 ) + m0.GFL_MTX_03 * ( m0.GFL_MTX_30 * m0.GFL_MTX_21 - m0.GFL_MTX_20 * m0.GFL_MTX_31 ),
		 m0.GFL_MTX_33 * ( m0.GFL_MTX_00 * m0.GFL_MTX_11 - m0.GFL_MTX_10 * m0.GFL_MTX_01 ) + m0.GFL_MTX_03 * ( m0.GFL_MTX_10 * m0.GFL_MTX_31 - m0.GFL_MTX_30 * m0.GFL_MTX_11 ) + m0.GFL_MTX_13 * ( m0.GFL_MTX_30 * m0.GFL_MTX_01 - m0.GFL_MTX_00 * m0.GFL_MTX_31 ),
		 m0.GFL_MTX_03 * ( m0.GFL_MTX_20 * m0.GFL_MTX_11 - m0.GFL_MTX_10 * m0.GFL_MTX_21 ) + m0.GFL_MTX_13 * ( m0.GFL_MTX_00 * m0.GFL_MTX_21 - m0.GFL_MTX_20 * m0.GFL_MTX_01 ) + m0.GFL_MTX_23 * ( m0.GFL_MTX_10 * m0.GFL_MTX_01 - m0.GFL_MTX_00 * m0.GFL_MTX_11 ),

		 m0.GFL_MTX_10 * ( m0.GFL_MTX_31 * m0.GFL_MTX_22 - m0.GFL_MTX_21 * m0.GFL_MTX_32 ) + m0.GFL_MTX_20 * ( m0.GFL_MTX_11 * m0.GFL_MTX_32 - m0.GFL_MTX_31 * m0.GFL_MTX_12 ) + m0.GFL_MTX_30 * ( m0.GFL_MTX_21 * m0.GFL_MTX_12 - m0.GFL_MTX_11 * m0.GFL_MTX_22 ),
		 m0.GFL_MTX_20 * ( m0.GFL_MTX_31 * m0.GFL_MTX_02 - m0.GFL_MTX_01 * m0.GFL_MTX_32 ) + m0.GFL_MTX_30 * ( m0.GFL_MTX_01 * m0.GFL_MTX_22 - m0.GFL_MTX_21 * m0.GFL_MTX_02 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_21 * m0.GFL_MTX_32 - m0.GFL_MTX_31 * m0.GFL_MTX_22 ),
		 m0.GFL_MTX_30 * ( m0.GFL_MTX_11 * m0.GFL_MTX_02 - m0.GFL_MTX_01 * m0.GFL_MTX_12 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_31 * m0.GFL_MTX_12 - m0.GFL_MTX_11 * m0.GFL_MTX_32 ) + m0.GFL_MTX_10 * ( m0.GFL_MTX_01 * m0.GFL_MTX_32 - m0.GFL_MTX_31 * m0.GFL_MTX_02 ),
		 m0.GFL_MTX_00 * ( m0.GFL_MTX_11 * m0.GFL_MTX_22 - m0.GFL_MTX_21 * m0.GFL_MTX_12 ) + m0.GFL_MTX_10 * ( m0.GFL_MTX_21 * m0.GFL_MTX_02 - m0.GFL_MTX_01 * m0.GFL_MTX_22 ) + m0.GFL_MTX_20 * ( m0.GFL_MTX_01 * m0.GFL_MTX_12 - m0.GFL_MTX_11 * m0.GFL_MTX_02 )
		 );
	Scale( det );
}

// -----------------------------------------------------------------
// Matrix To Quaternion
// -----------------------------------------------------------------
void MatrixAos44::MatrixToQuaternion( QuaternionNative* q ) const
{
	MatrixAos44 mat( *this );
	mat.Normalize( );

	const f32 tr = mat.GFL_MTX_00 + mat.GFL_MTX_11 + mat.GFL_MTX_22;
	f32 ww, max;
	s32 index;

	if( tr >= 0.0f ){
		ww = Math::FSqrt( tr + 1.0f );
		q->GFL_VECTOR_W = ( ww * 0.5f );
		ww = -0.5f / ww;

		q->GFL_VECTOR_X = ( ( mat.GFL_MTX_21 - mat.GFL_MTX_12 ) * ww );
		q->GFL_VECTOR_Y = ( ( mat.GFL_MTX_02 - mat.GFL_MTX_20 ) * ww );
		q->GFL_VECTOR_Z = ( ( mat.GFL_MTX_10 - mat.GFL_MTX_01 ) * ww );
		//		q->Normalize();
		return;
	}

	index = 0;
	max = mat.GFL_MTX_00;
	if( mat.GFL_MTX_11 > max ){
		index = 1;
		max = mat.GFL_MTX_11;
	}
	if( mat.GFL_MTX_22 > max ){
		index = 2;
	}

	switch( index ){
	case 0:
		ww = Math::FSqrt( mat.GFL_MTX_00 - mat.GFL_MTX_11 - mat.GFL_MTX_22 + 1.0f );
		q->GFL_VECTOR_X = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_Y = ( ( mat.GFL_MTX_01 + mat.GFL_MTX_10 ) * ww );
		q->GFL_VECTOR_Z = ( ( mat.GFL_MTX_20 + mat.GFL_MTX_02 ) * ww );
		q->GFL_VECTOR_W = ( ( mat.GFL_MTX_12 - mat.GFL_MTX_21 ) * ww );
		break;
	case 1:
		ww = Math::FSqrt( mat.GFL_MTX_11 - mat.GFL_MTX_00 - mat.GFL_MTX_22 + 1.0f );
		q->GFL_VECTOR_Y = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_Z = ( ( mat.GFL_MTX_12 + mat.GFL_MTX_21 ) * ww );
		q->GFL_VECTOR_X = ( ( mat.GFL_MTX_01 + mat.GFL_MTX_10 ) * ww );
		q->GFL_VECTOR_W = ( ( mat.GFL_MTX_20 - mat.GFL_MTX_02 ) * ww );
		break;
	case 2:
		ww = Math::FSqrt( mat.GFL_MTX_22 - mat.GFL_MTX_00 - mat.GFL_MTX_11 + 1.0f );
		q->GFL_VECTOR_Z = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_X = ( ( mat.GFL_MTX_20 + mat.GFL_MTX_02 ) * ww );
		q->GFL_VECTOR_Y = ( ( mat.GFL_MTX_12 + mat.GFL_MTX_21 ) * ww );
		q->GFL_VECTOR_W = ( ( mat.GFL_MTX_01 - mat.GFL_MTX_10 ) * ww );
	}
	//	q->Normalize();
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
Quaternion MatrixAos44::MatrixToQuaternion( void ) const
{
	Quaternion qt;
	MatrixToQuaternion( &qt );
	return qt;
}

// -----------------------------------------------------------------
// Quaternion To Matrix
// -----------------------------------------------------------------
void MatrixAos44::QuaternionToMatrix( const Quaternion& q )
{
	Quaternion qt;
	//	qt.Normalize( q );
	const f32 scl = qt.NormalizeRet( q );

	const f32 x = qt.GFL_VECTOR_X;
	const f32 y = qt.GFL_VECTOR_Y;
	const f32 z = qt.GFL_VECTOR_Z;
	const f32 w = qt.GFL_VECTOR_W;

	const f32 xx = x * x;
	const f32 yy = y * y;
	const f32 zz = z * z;
	const f32 xy = x * y;
	const f32 xz = x * z;
	const f32 yz = y * z;
	const f32 wx = w * x;
	const f32 wy = w * y;
	const f32 wz = w * z;

	GFL_MTX_00 = 1.0f - 2.0f * ( yy + zz );
	GFL_MTX_10 = 2.0f * ( xy - wz );
	GFL_MTX_20 = 2.0f * ( xz + wy );

	GFL_MTX_01 = 2.0f * ( xy + wz );
	GFL_MTX_11 = 1.0f - 2.0f * ( xx + zz );
	GFL_MTX_21 = 2.0f * ( yz - wx );

	GFL_MTX_02 = 2.0f * ( xz - wy );
	GFL_MTX_12 = 2.0f * ( yz + wx );
	GFL_MTX_22 = 1.0f - 2.0f * ( xx + yy );

	if( !Math::IsSimilar( scl, 1.0f, 0.01f ) ){
		Scale( scl );
	}
}

// -----------------------------------------------------------------
// GetRadian
// -----------------------------------------------------------------
void MatrixAos44::GetRadian( f32* x, f32* y, f32* z ) const
{
	f32 sy = GFL_MTX_02;
	if( 1.0f - gfl::core::Math::EPSILON <= sy ){
		*x = Math::FATan2( -GFL_MTX_10, GFL_MTX_11 );
		*y = -Math::HALF_PI;
		*z = 0.0f;
	} else if( sy <= -1.0f + gfl::core::Math::EPSILON ){
		*x = Math::FATan2( GFL_MTX_10, GFL_MTX_11 );
		*y = Math::HALF_PI;
		*z = 0.0f;
#if 0
	} else if( sy == 0.0f ){
		*x = Math::FATan2( GFL_MTX_20, -GFL_MTX_10 );
		*y = 0.0f;
		*z = Math::FATan2( -GFL_MTX_10, GFL_MTX_11 );
#endif
	} else {
#if 0 // ジンバルロック対策？
		f32 ay = Math::FASin( -sy );
		static const f32 lim = Math::HALF_PI * 0.9f;

		if( lim < Math::FAbs( ay ) ){
			ay = ( 0.0f < ay ) ? lim : -lim;
		}
		f32 r_cy = Math::FRecip( Math::FCos( ay ) );

		*x = Math::FATan2( GFL_MTX_12 * r_cy, GFL_MTX_22 * r_cy );
		*y = ay;
		*z = Math::FATan2( GFL_MTX_01 * r_cy, GFL_MTX_00 * r_cy );
#else
		// 本当は GFL_MTX_12/GetScaleY(), GFL_MTX_22/GetScaleZ() だけど…
		*x = Math::FATan2( GFL_MTX_12, GFL_MTX_22 );
		*y = Math::FASin( -sy / GetScaleX( ) );
		*z = Math::FATan2( GFL_MTX_01, GFL_MTX_00 );
#endif
	}
	//	Math::OptimizeRadian( x, y, z );
}

// -----------------------------------------------------------------
// GetRadian
// -----------------------------------------------------------------
void MatrixAos44::GetRadianYXZ( f32* x, f32* y, f32* z ) const
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
	*y = Math::FATan2( GFL_MTX_20, GFL_MTX_22 );
	if( z ){
		*z = Math::FATan2( GFL_MTX_01, GFL_MTX_11 );
		//		Math::OptimizeRadian( x, y, z );
	}
#else
	f32 sx = GFL_MTX_21;
	if( 1.0f - gfl::core::Math::EPSILON <= sx ){
		if( x ){
			*x = -Math::HALF_PI;
		}
		*y = 0.0f;
		if( z ){
			*z = Math::FATan2( -GFL_MTX_02, GFL_MTX_00 );
		}
	} else if( sx <= -1.0f + gfl::core::Math::EPSILON ){
		if( x ){
			*x = Math::HALF_PI;
		}
		*y = 0.0f;
		if( z ){
			*z = Math::FATan2( GFL_MTX_02, GFL_MTX_00 );
		}
	} else {
		if( x ){
			*x = Math::FASin( -sx ); //  / GetScaleZ( )
		}
		*y = Math::FATan2( GFL_MTX_20, GFL_MTX_22 );
		if( z ){
			*z = Math::FATan2( GFL_MTX_01, GFL_MTX_11 );
		}
	}
#endif
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimZY( const VectorNative& tag, f32* zsin, f32* zcos, f32* ysin
						 , f32* ycos, const bool reverse, const bool scaled ) const
{
	Vector loc;

	( !scaled ) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GFL_VECTOR_X;
	const f32 ly = loc.GFL_VECTOR_Y;
	const f32 lz = loc.GFL_VECTOR_Z;

	f32 tmp_0 = lx * lx + ly * ly;
	if( Math::IsAlmostZero( tmp_0 ) ){
		return false;
	}
	f32 tmp_1 = tmp_0 + lz * lz;
	tmp_0 = Math::FSqrt( tmp_0 );
	f32 tmp_2 = Math::FRecip( tmp_0 );
	tmp_1 = Math::FRecip( Math::FSqrt( tmp_1 ) );

	*zsin = ly * tmp_2;
	*zcos = lx * tmp_2;
	*ysin = -lz * tmp_1;
	*ycos = tmp_0 * tmp_1;
	return true;
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimYZ( const VectorNative& tag, f32* ysin, f32* ycos, f32* zsin
, f32* zcos, const bool reverse, const bool scaled ) const
{
	Vector loc;

	( !scaled ) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );
	//	tag.PrintConsole( "tag" );
	//	loc.PrintConsole( "loc" );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GFL_VECTOR_X;
	const f32 ly = loc.GFL_VECTOR_Y;
	const f32 lz = loc.GFL_VECTOR_Z;

	f32 tmp_0 = lx * lx + lz * lz;
	if( Math::IsAlmostZero( tmp_0 ) ){
		return false;
	}
	f32 tmp_1 = tmp_0 + ly * ly;
	tmp_0 = Math::FSqrt( tmp_0 );
	f32 tmp_2 = Math::FRecip( tmp_0 );
	tmp_1 = Math::FRecip( Math::FSqrt( tmp_1 ) );

	*ysin = -lz * tmp_2;
	*ycos = lx * tmp_2;
	*zsin = ly * tmp_1;
	//	*zsin = ( ( 0.0f <= lx ) ? ly : -ly ) * tmp_1;
	*zcos = tmp_0 * tmp_1;

	//	Debug::PrintConsole( "sc %f %f %f %f\n", *ysin, *ycos, *zsin, *zcos );
	return true;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimYX( const VectorNative& tag, f32* ysin, f32* ycos, f32* xsin
, f32* xcos, const bool reverse, const bool scaled ) const
{
	Vector loc;

	( !scaled ) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GFL_VECTOR_X;
	const f32 ly = loc.GFL_VECTOR_Y;
	const f32 lz = loc.GFL_VECTOR_Z;

	f32 tmp_0 = lx * lx + lz * lz;
	if( Math::IsAlmostZero( tmp_0 ) ){
		return false;
	}
	f32 tmp_1 = tmp_0 + ly * ly;
	tmp_0 = Math::FSqrt( tmp_0 );
	f32 tmp_2 = Math::FRecip( tmp_0 );
	tmp_1 = Math::FRecip( Math::FSqrt( tmp_1 ) );

	*ysin = lx * tmp_2;
	*ycos = lz * tmp_2;
	*xsin = -ly * tmp_1;
	*xcos = tmp_0 * tmp_1;
	return true;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimXZ( const VectorNative& tag, f32* xsin, f32* xcos, f32* zsin
, f32* zcos, const bool reverse, const bool scaled ) const
{
	Vector loc;

	( !scaled ) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GFL_VECTOR_X;
	const f32 ly = loc.GFL_VECTOR_Y;
	const f32 lz = loc.GFL_VECTOR_Z;

	f32 tmp_0 = ly * ly + lz * lz;
	if( Math::IsAlmostZero( tmp_0 ) ){
		return false;
	}
	f32 tmp_1 = tmp_0 + lx * lx;
	tmp_0 = Math::FSqrt( tmp_0 );
	f32 tmp_2 = Math::FRecip( tmp_0 );
	tmp_1 = Math::FRecip( Math::FSqrt( tmp_1 ) );

	*xsin = lz * tmp_2;
	*xcos = ly * tmp_2;
	*zsin = -lx * tmp_1;
	*zcos = tmp_0 * tmp_1;
	return true;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimXY( const VectorNative& tag, f32* xsin, f32* xcos, f32* zsin
, f32* zcos, const bool reverse, const bool scaled ) const
{
	Vector loc;

	( !scaled ) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );

	if( reverse ){
		loc.Neg( );
	}

	const f32 lx = loc.GFL_VECTOR_X;
	const f32 ly = loc.GFL_VECTOR_Y;
	const f32 lz = loc.GFL_VECTOR_Z;

	f32 tmp_0 = ly * ly + lz * lz;
	if( Math::IsAlmostZero( tmp_0 ) ){
		return false;
	}
	f32 tmp_1 = tmp_0 + lx * lx;
	tmp_0 = Math::FSqrt( tmp_0 );
	f32 tmp_2 = Math::FRecip( tmp_0 );
	tmp_1 = Math::FRecip( Math::FSqrt( tmp_1 ) );

	*xsin = -ly * tmp_2;
	*xcos = lz * tmp_2;
	*zsin = lx * tmp_1;
	*zcos = tmp_0 * tmp_1;
	return true;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimZY( const VectorNative& tag, const bool reverse, const f32 rate )
{
	f32 zsin, zcos, ysin, ycos;

	if( AimZY( tag, &zsin, &zcos, &ysin, &ycos, reverse ) ){
		if( rate == 1.0f ){
			RotateZ( zsin, zcos );
			RotateY( ysin, ycos );
		} else {
			RotateZ( Math::FATan2( zsin, zcos ) * rate );
			RotateY( Math::FATan2( ysin, ycos ) * rate );
		}
		return true;
	}
	return false;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimYZ( const VectorNative& tag, const bool reverse, const f32 rate )
{
	f32 zsin, zcos, ysin, ycos;

	if( AimYZ( tag, &ysin, &ycos, &zsin, &zcos, reverse ) ){
		if( rate == 1.0f ){
			RotateY( ysin, ycos );
			RotateZ( zsin, zcos );
		} else {
			RotateY( Math::FATan2( ysin, ycos ) * rate );
			RotateZ( Math::FATan2( zsin, zcos ) * rate );
		}
		return true;
	}
	return false;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimYX( const VectorNative& tag, const bool reverse, const f32 rate )
{
	f32 xsin, xcos, ysin, ycos;

	if( AimYX( tag, &ysin, &ycos, &xsin, &xcos, reverse ) ){
		if( rate == 1.0f ){
			RotateY( ysin, ycos );
			RotateX( xsin, xcos );
		} else {
			RotateY( Math::FATan2( ysin, ycos ) * rate );
			RotateX( Math::FATan2( xsin, xcos ) * rate );
		}
		return true;
	}
	return false;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimXZ( const VectorNative& tag, const bool reverse, const f32 rate )
{
	f32 xsin, xcos, zsin, zcos;

	if( AimXZ( tag, &xsin, &xcos, &zsin, &zcos, reverse ) ){
		if( rate == 1.0f ){
			RotateX( xsin, xcos );
			RotateZ( zsin, zcos );
		} else {
			RotateX( Math::FATan2( xsin, xcos ) * rate );
			RotateZ( Math::FATan2( zsin, zcos ) * rate );
		}
		return true;
	}
	return false;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
bool MatrixAos44::AimXY( const VectorNative& tag, const bool reverse, const f32 rate )
{
	f32 xsin, xcos, zsin, zcos;

	if( AimXY( tag, &xsin, &xcos, &zsin, &zcos, reverse ) ){
		if( rate == 1.0f ){
			RotateX( xsin, xcos );
			RotateY( zsin, zcos );
		} else {
			RotateX( Math::FATan2( xsin, xcos ) * rate );
			RotateY( Math::FATan2( zsin, zcos ) * rate );
		}
		return true;
	}
	return false;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
void MatrixAos44::AimX( f32* xsin, f32* xcos )
{
	const f32 x = GFL_MTX_20;
	const f32 y = GFL_MTX_21;
	const f32 z = GFL_MTX_22;

	f32 xz2 = x * x + z*z;
	//	f32 r_1 = Math::FRecip( Math::FSqrt(xz2 + y * y) );
	//	f32 r_1 = Math::FRecip( xz2 + y * y );

	*xsin = y; //*r_1;
	*xcos = Math::FSqrt( xz2 ); // * r_1;
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
void MatrixAos44::AimX( void )
{
	f32 xsin, xcos;
	AimX( &xsin, &xcos );
	RotateX( xsin, xcos );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#if 0 //
void MatrixAos44::Clamp( const Matrix44Aos& parent, const Vector3& low, const Vector3& high )
{
	Vector3 ang;
	Matrix loc;
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
void MatrixAos44::GetPositionByLowestY( VectorNative* res, const f32 y0, const f32 r ) const
{
	// 元々の骨の向きの単位ベクトルを求める
	Vector v;
	ApplyX1( &v );

	const f32 y = GetTransY( ) - y0;
	f32 r_y2 = ( r * r - y * y );
	// 半径より高さが高い場合
	if( r_y2 < 0.0f ){
		r_y2 = 0.0f;
	}
	const f32 l = v.GFL_VECTOR_X;
	const f32 n = v.GFL_VECTOR_Z;
	const f32 l_2 = l*l;
	// ローカルX座標
	f32 xl = Math::FSqrt( ( l_2 * r_y2 ) / ( l_2 + n * n ) );
	f32 zl = ( n * xl ) / l;

	// 内積が負の場合は別解
	if( xl * l + zl * n < 0.0f ){
		xl = -xl;
		zl = -zl;
	}

	res->GFL_VECTOR_X = ( xl + GetTransX( ) );
	//	res->GFL_VECTOR_Y = ( y0 );
	res->GFL_VECTOR_Z = ( zl + GetTransZ( ) );
}

#if GFL_USE_SOA
//==============================================================================
//==============================================================================
void MatrixAos44::FromSOA( const MatrixSoaNative33& src )
{
	GFL_MTX_00 = src.m[0][0];
	GFL_MTX_10 = src.m[0][1];
	GFL_MTX_20 = src.m[0][2];
	GFL_MTX_01 = src.m[1][0];
	GFL_MTX_11 = src.m[1][1];
	GFL_MTX_21 = src.m[1][2];
	GFL_MTX_02 = src.m[2][0];
	GFL_MTX_12 = src.m[2][1];
	GFL_MTX_22 = src.m[2][2];
	GFL_MTX_03 = GFL_MTX_13 = GFL_MTX_23 = GFL_MTX_30 = GFL_MTX_31 = GFL_MTX_32 = 0.0f;
	GFL_MTX_33 = 1.0f;
}
//==============================================================================
//==============================================================================
void MatrixAos44::FromSOA( const MatrixSoaNative44& src )
{
	GFL_MTX_00 = src.m[0][0];
	GFL_MTX_10 = src.m[0][1];
	GFL_MTX_20 = src.m[0][2];
	GFL_MTX_30 = src.m[0][3];
	GFL_MTX_01 = src.m[1][0];
	GFL_MTX_11 = src.m[1][1];
	GFL_MTX_21 = src.m[1][2];
	GFL_MTX_31 = src.m[1][3];
	GFL_MTX_02 = src.m[2][0];
	GFL_MTX_12 = src.m[2][1];
	GFL_MTX_22 = src.m[2][2];
	GFL_MTX_32 = src.m[2][3];
	GFL_MTX_03 = src.m[3][0];
	GFL_MTX_13 = src.m[3][1];
	GFL_MTX_23 = src.m[3][2];
	GFL_MTX_33 = src.m[3][3];
}
//==============================================================================
//==============================================================================
void MatrixAos44::ToSOA( MatrixSoaNative33* dst ) const
{
	dst->m[0][0] = GFL_MTX_00;
	dst->m[0][1] = GFL_MTX_10;
	dst->m[0][2] = GFL_MTX_20;
	dst->m[1][0] = GFL_MTX_01;
	dst->m[1][1] = GFL_MTX_11;
	dst->m[1][2] = GFL_MTX_21;
	dst->m[2][0] = GFL_MTX_02;
	dst->m[2][1] = GFL_MTX_12;
	dst->m[2][2] = GFL_MTX_22;
}
//==============================================================================
//==============================================================================
void MatrixAos44::ToSOA( MatrixSoaNative34* dst ) const
{
	dst->m[0][0] = GFL_MTX_00;
	dst->m[0][1] = GFL_MTX_10;
	dst->m[0][2] = GFL_MTX_20;
	dst->m[0][3] = GFL_MTX_30;
	dst->m[1][0] = GFL_MTX_01;
	dst->m[1][1] = GFL_MTX_11;
	dst->m[1][2] = GFL_MTX_21;
	dst->m[1][3] = GFL_MTX_31;
	dst->m[2][0] = GFL_MTX_02;
	dst->m[2][1] = GFL_MTX_12;
	dst->m[2][2] = GFL_MTX_22;
	dst->m[2][3] = GFL_MTX_32;
}
//==============================================================================
//==============================================================================
void MatrixAos44::ToSOA( MatrixSoaNative44* dst ) const
{
	dst->m[0][0] = GFL_MTX_00;
	dst->m[0][1] = GFL_MTX_10;
	dst->m[0][2] = GFL_MTX_20;
	dst->m[0][3] = GFL_MTX_30;
	dst->m[1][0] = GFL_MTX_01;
	dst->m[1][1] = GFL_MTX_11;
	dst->m[1][2] = GFL_MTX_21;
	dst->m[1][3] = GFL_MTX_31;
	dst->m[2][0] = GFL_MTX_02;
	dst->m[2][1] = GFL_MTX_12;
	dst->m[2][2] = GFL_MTX_22;
	dst->m[2][3] = GFL_MTX_32;
	dst->m[3][0] = GFL_MTX_03;
	dst->m[3][1] = GFL_MTX_13;
	dst->m[3][2] = GFL_MTX_23;
	dst->m[3][3] = GFL_MTX_33;
}
#endif

//==============================================================================
//==============================================================================
MatrixAos44& MatrixAos44::Slerp( const MatrixAos44& m0, const MatrixAos44& m1, const f32 rate )
{
	Quaternion res;
	//	m0.MatrixToQuaternion().PrintConsole("m0");
	//	m1.MatrixToQuaternion().PrintConsole("m1");
	res.Slerp( m0.MatrixToQuaternion( ), m1.MatrixToQuaternion( ), rate );
	//	res.PrintConsole("q");
	res.QuaternionToMatrix( this );
	return *this;
}
}
}

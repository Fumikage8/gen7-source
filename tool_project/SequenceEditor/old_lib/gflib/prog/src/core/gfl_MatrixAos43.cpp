// マトリックス演算系　※汎用性重視で各ハードウェアに対して最適化はしていません

namespace gfl {
namespace core {

// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void MatrixAos43::Set( const MatrixAos43& m )
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
void MatrixAos43::Set( const MatrixAos44& m )
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
void MatrixAos43::Set( f32 m0, f32 m1, f32 m2, f32 m3,
					   f32 m4, f32 m5, f32 m6, f32 m7,
					   f32 m8, f32 m9, f32 m10, f32 m11 )
{
	GFL_MTX_00 = m0;
	GFL_MTX_01 = m1;
	GFL_MTX_02 = m2;
	GFL_MTX_10 = m3;
	GFL_MTX_11 = m4;
	GFL_MTX_12 = m5;
	GFL_MTX_20 = m6;
	GFL_MTX_21 = m7;
	GFL_MTX_22 = m8;
	GFL_MTX_30 = m9;
	GFL_MTX_31 = m10;
	GFL_MTX_32 = m11;
}


// -----------------------------------------------------------------
// SetUnit
// -----------------------------------------------------------------
void MatrixAos43::SetUnit( void )
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
	SetRow( 0, Vector3::GetX1( ) );
	SetRow( 1, Vector3::GetY1( ) );
	SetRow( 2, Vector3::GetZ1( ) );
	SetRow( 3, Vector3::GetZero( ) );
#endif
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void MatrixAos43::Transpose( const MatrixAos43& m )
{
	GFL_MTX_00 = m.GFL_MTX_00;
	GFL_MTX_01 = m.GFL_MTX_10;
	GFL_MTX_02 = m.GFL_MTX_20;
	GFL_MTX_10 = m.GFL_MTX_01;
	GFL_MTX_11 = m.GFL_MTX_11;
	GFL_MTX_12 = m.GFL_MTX_21;
	GFL_MTX_20 = m.GFL_MTX_02;
	GFL_MTX_21 = m.GFL_MTX_12;
	GFL_MTX_22 = m.GFL_MTX_22;
	GFL_MTX_30 = m.GFL_MTX_30;
	GFL_MTX_31 = m.GFL_MTX_31;
	GFL_MTX_32 = m.GFL_MTX_32;
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void MatrixAos43::Transpose( void )
{
	Math::Swap( GFL_MTX_01, GFL_MTX_10 );
	Math::Swap( GFL_MTX_02, GFL_MTX_20 );
	Math::Swap( GFL_MTX_12, GFL_MTX_21 );
}

// -----------------------------------------------------------------
// Normalize
// -----------------------------------------------------------------
void MatrixAos43::Normalize( VectorNative* scl )
{
	gfl::core::Vector3 vx, vy, vz;

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
void MatrixAos43::PrintConsole( void ) const
{
	Debug::PrintConsole( "%ff, %ff, %ff, \n%ff, %ff, %ff, \n%ff, %ff, %ff, \n%ff, %ff, %ff\n",
						 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02,
						 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12,
						 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22,
						 GFL_MTX_30, GFL_MTX_31, GFL_MTX_32 );
}

// -----------------------------------------------------------------
// Set Rotate X
// -----------------------------------------------------------------
void MatrixAos43::SetRotateX( const f32 s, const f32 c )
{
	SetRow( 0, Vector3::s_X1 );

	GFL_MTX_10 = 0.0f;
	GFL_MTX_11 = c;
	GFL_MTX_12 = s;

	GFL_MTX_20 = 0.0f;
	GFL_MTX_21 = -s;
	GFL_MTX_22 = c;
}

// -----------------------------------------------------------------
// Set Rotate Y
// -----------------------------------------------------------------
void MatrixAos43::SetRotateY( const f32 s, const f32 c )
{
	GFL_MTX_00 = c;
	GFL_MTX_01 = 0.0f;
	GFL_MTX_02 = -s;

	SetRow( 1, Vector3::s_Y1 );

	GFL_MTX_20 = s;
	GFL_MTX_21 = 0.0f;
	GFL_MTX_22 = c;
}

// -----------------------------------------------------------------
// Set Rotate Z
// -----------------------------------------------------------------
void MatrixAos43::SetRotateZ( const f32 s, const f32 c )
{
	GFL_MTX_00 = c;
	GFL_MTX_01 = s;
	GFL_MTX_02 = 0.0f;

	GFL_MTX_10 = -s;
	GFL_MTX_11 = c;
	GFL_MTX_12 = 0.0f;

	SetRow( 2, Vector3::s_Z1 );
}

// -----------------------------------------------------------------
// Set Scale
// -----------------------------------------------------------------
void MatrixAos43::SetScale( const f32 x, const f32 y, const f32 z )
{
	GFL_MTX_01 =
		GFL_MTX_02 =
		GFL_MTX_10 =
		GFL_MTX_12 =
		GFL_MTX_20 =
		GFL_MTX_21 = 0.0f;
	GFL_MTX_00 = x;
	GFL_MTX_11 = y;
	GFL_MTX_22 = z;
}

// -----------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------
void MatrixAos43::RotateX( const MatrixAos43& m0, const f32 s, const f32 c )
{
	GFL_MTX_00 = m0.GFL_MTX_00;
	GFL_MTX_01 = m0.GFL_MTX_01;
	GFL_MTX_02 = m0.GFL_MTX_02;
	GFL_MTX_30 = m0.GFL_MTX_30;
	GFL_MTX_31 = m0.GFL_MTX_31;
	GFL_MTX_32 = m0.GFL_MTX_32;
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
void MatrixAos43::RotateX( const f32 s, const f32 c )
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
void MatrixAos43::RotateX_HalfPi( void )
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
void MatrixAos43::RotateX_MinusHalfPi( void )
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
void MatrixAos43::RotateX_OnePi( void )
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
void MatrixAos43::RotateX_Reverse( const f32 s, const f32 c )
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
void MatrixAos43::RotateY( const MatrixAos43& m0, const f32 s, const f32 c )
{
	GFL_MTX_10 = m0.GFL_MTX_10;
	GFL_MTX_11 = m0.GFL_MTX_11;
	GFL_MTX_12 = m0.GFL_MTX_12;
	GFL_MTX_30 = m0.GFL_MTX_30;
	GFL_MTX_31 = m0.GFL_MTX_31;
	GFL_MTX_32 = m0.GFL_MTX_32;
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
void MatrixAos43::RotateY( const f32 s, const f32 c )
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
void MatrixAos43::RotateY_HalfPi( void )
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
void MatrixAos43::RotateY_MinusHalfPi( void )
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
void MatrixAos43::RotateY_OnePi( void )
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
void MatrixAos43::RotateY_Reverse( const MatrixAos43& m0, const f32 s, const f32 c )
{
	const f32 m00 = m0.GFL_MTX_00, m02 = m0.GFL_MTX_02;
	const f32 m10 = m0.GFL_MTX_10, m12 = m0.GFL_MTX_12;
	const f32 m20 = m0.GFL_MTX_20, m22 = m0.GFL_MTX_22;

	GFL_MTX_01 = m0.GFL_MTX_01;
	GFL_MTX_11 = m0.GFL_MTX_11;
	GFL_MTX_21 = m0.GFL_MTX_21;
	GFL_MTX_31 = m0.GFL_MTX_31;

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
void MatrixAos43::RotateY_Reverse( const f32 s, const f32 c )
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
void MatrixAos43::RotateZ( const MatrixAos43& m0, const f32 s, const f32 c )
{
	GFL_MTX_20 = m0.GFL_MTX_20;
	GFL_MTX_21 = m0.GFL_MTX_21;
	GFL_MTX_22 = m0.GFL_MTX_22;
	GFL_MTX_30 = m0.GFL_MTX_30;
	GFL_MTX_31 = m0.GFL_MTX_31;
	GFL_MTX_32 = m0.GFL_MTX_32;
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
void MatrixAos43::RotateZ( const f32 s, const f32 c )
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
void MatrixAos43::RotateZ_HalfPi( void )
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
void MatrixAos43::RotateZ_MinusHalfPi( void )
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
void MatrixAos43::RotateZ_OnePi( void )
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
void MatrixAos43::RotateZ_Reverse( const f32 s, const f32 c )
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
void MatrixAos43::Mul33( const MatrixAos43& m0, const MatrixAos43& m1 )
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
void MatrixAos43::Mul( const MatrixAos43& m0, const MatrixAos43& m1 )
{
	const f32 l_00 = m0.GFL_MTX_00, l_01 = m0.GFL_MTX_01, l_02 = m0.GFL_MTX_02;
	const f32 l_10 = m0.GFL_MTX_10, l_11 = m0.GFL_MTX_11, l_12 = m0.GFL_MTX_12;
	const f32 l_20 = m0.GFL_MTX_20, l_21 = m0.GFL_MTX_21, l_22 = m0.GFL_MTX_22;
	const f32 l_30 = m0.GFL_MTX_30, l_31 = m0.GFL_MTX_31, l_32 = m0.GFL_MTX_32;
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

	const f32 r_30 = m1.GFL_MTX_30, r_31 = m1.GFL_MTX_31, r_32 = m1.GFL_MTX_32;

	GFL_MTX_30 = l_00 * r_30 + l_10 * r_31 + l_20 * r_32 + l_30;
	GFL_MTX_31 = l_01 * r_30 + l_11 * r_31 + l_21 * r_32 + l_31;
	GFL_MTX_32 = l_02 * r_30 + l_12 * r_31 + l_22 * r_32 + l_32;
}

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void MatrixAos43::Mul( const Quaternion& q0 )
{
	MatrixAos43 mat;
	mat.QuaternionToMatrix( q0 );
	Mul33( mat );
}

// -----------------------------------------------------------------
// Determinant33
// -----------------------------------------------------------------
f32 MatrixAos43::Determinant33( void ) const
{
	return(GFL_MTX_00 * GFL_MTX_11 - GFL_MTX_10 * GFL_MTX_01 )* GFL_MTX_22
		- ( GFL_MTX_00 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_01 ) * GFL_MTX_12
		+ ( GFL_MTX_10 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_11 ) * GFL_MTX_02;
}

// -----------------------------------------------------------------
// Determinant
// -----------------------------------------------------------------
f32 MatrixAos43::Determinant( void ) const
{
	return Determinant33( );
}

// -----------------------------------------------------------------
// Inverse33
// -----------------------------------------------------------------
void MatrixAos43::Inverse33( const MatrixAos43& m0 )
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
void MatrixAos43::Inverse( const MatrixAos43& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set(
		 m0.GFL_MTX_11 * m0.GFL_MTX_22 - m0.GFL_MTX_21 * m0.GFL_MTX_12,
		 m0.GFL_MTX_21 * m0.GFL_MTX_02 - m0.GFL_MTX_01 * m0.GFL_MTX_22,
		 m0.GFL_MTX_01 * m0.GFL_MTX_12 - m0.GFL_MTX_11 * m0.GFL_MTX_02,

		 m0.GFL_MTX_12 * m0.GFL_MTX_20 - m0.GFL_MTX_22 * m0.GFL_MTX_10,
		 m0.GFL_MTX_22 * m0.GFL_MTX_00 - m0.GFL_MTX_02 * m0.GFL_MTX_20,
		 m0.GFL_MTX_02 * m0.GFL_MTX_10 - m0.GFL_MTX_12 * m0.GFL_MTX_00,

		 m0.GFL_MTX_10 * m0.GFL_MTX_21 - m0.GFL_MTX_20 * m0.GFL_MTX_11,
		 m0.GFL_MTX_20 * m0.GFL_MTX_01 - m0.GFL_MTX_00 * m0.GFL_MTX_21,
		 m0.GFL_MTX_00 * m0.GFL_MTX_11 - m0.GFL_MTX_10 * m0.GFL_MTX_01,

		 m0.GFL_MTX_10 * ( m0.GFL_MTX_31 * m0.GFL_MTX_22 - m0.GFL_MTX_21 * m0.GFL_MTX_32 ) + m0.GFL_MTX_20 * ( m0.GFL_MTX_11 * m0.GFL_MTX_32 - m0.GFL_MTX_31 * m0.GFL_MTX_12 ) + m0.GFL_MTX_30 * ( m0.GFL_MTX_21 * m0.GFL_MTX_12 - m0.GFL_MTX_11 * m0.GFL_MTX_22 ),
		 m0.GFL_MTX_20 * ( m0.GFL_MTX_31 * m0.GFL_MTX_02 - m0.GFL_MTX_01 * m0.GFL_MTX_32 ) + m0.GFL_MTX_30 * ( m0.GFL_MTX_01 * m0.GFL_MTX_22 - m0.GFL_MTX_21 * m0.GFL_MTX_02 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_21 * m0.GFL_MTX_32 - m0.GFL_MTX_31 * m0.GFL_MTX_22 ),
		 m0.GFL_MTX_30 * ( m0.GFL_MTX_11 * m0.GFL_MTX_02 - m0.GFL_MTX_01 * m0.GFL_MTX_12 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_31 * m0.GFL_MTX_12 - m0.GFL_MTX_11 * m0.GFL_MTX_32 ) + m0.GFL_MTX_10 * ( m0.GFL_MTX_01 * m0.GFL_MTX_32 - m0.GFL_MTX_31 * m0.GFL_MTX_02 )
		 );
	Scale( det );
}

// -----------------------------------------------------------------
// Matrix To Quaternion
// -----------------------------------------------------------------
void MatrixAos43::MatrixToQuaternion( QuaternionNative* q ) const
{
	const f32 tr = GFL_MTX_00 + GFL_MTX_11 + GFL_MTX_22;
	f32 ww, max;
	s32 index;

	if( tr >= 0.0f ){
		ww = Math::FSqrt( tr + 1.0f );
		q->GFL_VECTOR_W = ( ww * 0.5f );
		ww = -0.5f / ww;

		q->GFL_VECTOR_X = ( ( GFL_MTX_21 - GFL_MTX_12 ) * ww );
		q->GFL_VECTOR_Y = ( ( GFL_MTX_02 - GFL_MTX_20 ) * ww );
		q->GFL_VECTOR_Z = ( ( GFL_MTX_10 - GFL_MTX_01 ) * ww );
		//		q->Normalize();
		return;
	}

	index = 0;
	max = GFL_MTX_00;
	if( GFL_MTX_11 > max ){
		index = 1;
		max = GFL_MTX_11;
	}
	if( GFL_MTX_22 > max ){
		index = 2;
	}

	switch( index ){
	case 0:
		ww = Math::FSqrt( GFL_MTX_00 - GFL_MTX_11 - GFL_MTX_22 + 1.0f );
		q->GFL_VECTOR_X = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_Y = ( ( GFL_MTX_01 + GFL_MTX_10 ) * ww );
		q->GFL_VECTOR_Z = ( ( GFL_MTX_20 + GFL_MTX_02 ) * ww );
		q->GFL_VECTOR_W = ( ( GFL_MTX_12 - GFL_MTX_21 ) * ww );
		break;
	case 1:
		ww = Math::FSqrt( GFL_MTX_11 - GFL_MTX_00 - GFL_MTX_22 + 1.0f );
		q->GFL_VECTOR_Y = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_Z = ( ( GFL_MTX_12 + GFL_MTX_21 ) * ww );
		q->GFL_VECTOR_X = ( ( GFL_MTX_01 + GFL_MTX_10 ) * ww );
		q->GFL_VECTOR_W = ( ( GFL_MTX_20 - GFL_MTX_02 ) * ww );
		break;
	case 2:
		ww = Math::FSqrt( GFL_MTX_22 - GFL_MTX_00 - GFL_MTX_11 + 1.0f );
		q->GFL_VECTOR_Z = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_X = ( ( GFL_MTX_20 + GFL_MTX_02 ) * ww );
		q->GFL_VECTOR_Y = ( ( GFL_MTX_12 + GFL_MTX_21 ) * ww );
		q->GFL_VECTOR_W = ( ( GFL_MTX_01 - GFL_MTX_10 ) * ww );
	}
	//	q->Normalize();
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
Quaternion MatrixAos43::MatrixToQuaternion( void ) const
{
	Quaternion qt;
	MatrixToQuaternion( &qt );
	return qt;
}

// -----------------------------------------------------------------
// Quaternion To Matrix
// -----------------------------------------------------------------
void MatrixAos43::QuaternionToMatrix( const Quaternion& q )
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
void MatrixAos43::GetRadian( f32* x, f32* y, f32* z ) const
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
		*y = Math::FASin( -sy ); // / GetScaleX( ) 
		*z = Math::FATan2( GFL_MTX_01, GFL_MTX_00 );
#endif
	}
	//	Math::OptimizeRadian( x, y, z );
}

// -----------------------------------------------------------------
// GetRadian
// -----------------------------------------------------------------
void MatrixAos43::GetRadianYXZ( f32* x, f32* y, f32* z ) const
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
bool MatrixAos43::AimZY( const VectorNative& tag, f32* zsin, f32* zcos, f32* ysin
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
bool MatrixAos43::AimYZ( const VectorNative& tag, f32* ysin, f32* ycos, f32* zsin
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
bool MatrixAos43::AimYX( const VectorNative& tag, f32* ysin, f32* ycos, f32* xsin
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
bool MatrixAos43::AimXZ( const VectorNative& tag, f32* xsin, f32* xcos, f32* zsin
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
bool MatrixAos43::AimXY( const VectorNative& tag, f32* xsin, f32* xcos, f32* zsin
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
bool MatrixAos43::AimZY( const VectorNative& tag, const bool reverse, const f32 rate )
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
bool MatrixAos43::AimYZ( const VectorNative& tag, const bool reverse, const f32 rate )
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
bool MatrixAos43::AimYX( const VectorNative& tag, const bool reverse, const f32 rate )
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
bool MatrixAos43::AimXZ( const VectorNative& tag, const bool reverse, const f32 rate )
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
bool MatrixAos43::AimXY( const VectorNative& tag, const bool reverse, const f32 rate )
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
void MatrixAos43::AimX( f32* xsin, f32* xcos )
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
void MatrixAos43::AimX( void )
{
	f32 xsin, xcos;
	AimX( &xsin, &xcos );
	RotateX( xsin, xcos );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#if 0 //
void MatrixAos43::Clamp( const Matrix43AOS& parent, const Vector3& low, const Vector3& high )
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
void MatrixAos43::GetPositionByLowestY( VectorNative* res, const f32 y0, const f32 r ) const
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
void MatrixAos43::FromSOA( const MatrixSoaNative33& src )
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
}
//==============================================================================
//==============================================================================
void MatrixAos43::FromSOA( const MatrixSoaNative34& src )
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
}
//==============================================================================
//==============================================================================
void MatrixAos43::FromSOA( const MatrixSoaNative44& src )
{
	FromSOA( ( const MatrixSoaNative34& )src );
}
//==============================================================================
//==============================================================================
void MatrixAos43::ToSOA( MatrixSoaNative33* dst ) const
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
void MatrixAos43::ToSOA( MatrixSoaNative34* dst ) const
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
void MatrixAos43::ToSOA( MatrixSoaNative44* dst ) const
{
	ToSOA( ( MatrixSoaNative34* )dst );
}
#endif

//==============================================================================
//==============================================================================
MatrixAos43& MatrixAos43::Slerp( const MatrixAos43& m0, const MatrixAos43& m1, const f32 rate )
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

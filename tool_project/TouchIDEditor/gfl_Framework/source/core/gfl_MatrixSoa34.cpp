
#include "core/gfl_Debug.h"
#include "core/gfl_MathBase.h"


// マトリックス演算系　※汎用性重視で各ハードウェアに対して最適化はしていません

namespace gfl {
namespace core {
// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void MatrixSoa34::Set( f32 m0, f32 m1, f32 m2, f32 m3,
					   f32 m4, f32 m5, f32 m6, f32 m7,
					   f32 m8, f32 m9, f32 m10, f32 m11 )
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
}

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void MatrixSoa34::Mul( const MatrixSoaNative34& m0, const MatrixSoaNative34& m1 )
{
#ifdef GFL_PLATFORM_3DS
	nn::math::MTX34Mult( this, &m0, &m1 );
#else // 
	const f32 l_00 = m0.GFL_MTX_00, l_01 = m0.GFL_MTX_10, l_02 = m0.GFL_MTX_20;
	const f32 l_10 = m0.GFL_MTX_01, l_11 = m0.GFL_MTX_11, l_12 = m0.GFL_MTX_21;
	const f32 l_20 = m0.GFL_MTX_02, l_21 = m0.GFL_MTX_12, l_22 = m0.GFL_MTX_22;
	const f32 r_00 = m1.GFL_MTX_00, r_01 = m1.GFL_MTX_10, r_02 = m1.GFL_MTX_20;

	GFL_MTX_00 = l_00 * r_00 + l_10 * r_01 + l_20 * r_02;
	GFL_MTX_10 = l_01 * r_00 + l_11 * r_01 + l_21 * r_02;
	GFL_MTX_20 = l_02 * r_00 + l_12 * r_01 + l_22 * r_02;

	const f32 r_10 = m1.GFL_MTX_01, r_11 = m1.GFL_MTX_11, r_12 = m1.GFL_MTX_21;

	GFL_MTX_01 = l_00 * r_10 + l_10 * r_11 + l_20 * r_12;
	GFL_MTX_11 = l_01 * r_10 + l_11 * r_11 + l_21 * r_12;
	GFL_MTX_21 = l_02 * r_10 + l_12 * r_11 + l_22 * r_12;

	const f32 r_20 = m1.GFL_MTX_02, r_21 = m1.GFL_MTX_12, r_22 = m1.GFL_MTX_22;

	GFL_MTX_02 = l_00 * r_20 + l_10 * r_21 + l_20 * r_22;
	GFL_MTX_12 = l_01 * r_20 + l_11 * r_21 + l_21 * r_22;
	GFL_MTX_22 = l_02 * r_20 + l_12 * r_21 + l_22 * r_22;

	const f32 r_30 = m1.GFL_MTX_03, r_31 = m1.GFL_MTX_13, r_32 = m1.GFL_MTX_23;

	GFL_MTX_03 = l_00 * r_30 + l_10 * r_31 + l_20 * r_32 + m0.GFL_MTX_03;
	GFL_MTX_13 = l_01 * r_30 + l_11 * r_31 + l_21 * r_32 + m0.GFL_MTX_13;
	GFL_MTX_23 = l_02 * r_30 + l_12 * r_31 + l_22 * r_32 + m0.GFL_MTX_23;
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MatrixSoa34::Mul( const QuaternionNative& q0 )
{
	MatrixSoa34 mat;
	mat.QuaternionToMatrix( q0 );
	Mul33( mat );
}
// -----------------------------------------------------------------------------
// Mul33
// -----------------------------------------------------------------------------
void MatrixSoa34::Mul33( const MatrixSoa34& m0, const MatrixSoa34& m1 )
{
#if 0 // def GFL_PLATFORM_3DS // Library BUG !!!
	nn::math::MTX33Mult( this, &m0, &m1 );
#else // 
	const f32 l_00 = m0.GFL_MTX_00, l_01 = m0.GFL_MTX_10, l_02 = m0.GFL_MTX_20;
	const f32 l_10 = m0.GFL_MTX_01, l_11 = m0.GFL_MTX_11, l_12 = m0.GFL_MTX_21;
	const f32 l_20 = m0.GFL_MTX_02, l_21 = m0.GFL_MTX_12, l_22 = m0.GFL_MTX_22;
	const f32 r_00 = m1.GFL_MTX_00, r_01 = m1.GFL_MTX_10, r_02 = m1.GFL_MTX_20;
	const f32 r_10 = m1.GFL_MTX_01, r_11 = m1.GFL_MTX_11, r_12 = m1.GFL_MTX_21;
	const f32 r_20 = m1.GFL_MTX_02, r_21 = m1.GFL_MTX_12, r_22 = m1.GFL_MTX_22;

	GFL_MTX_00 = l_00 * r_00 + l_10 * r_01 + l_20 * r_02;
	GFL_MTX_01 = l_00 * r_10 + l_10 * r_11 + l_20 * r_12;
	GFL_MTX_02 = l_00 * r_20 + l_10 * r_21 + l_20 * r_22;
	GFL_MTX_10 = l_01 * r_00 + l_11 * r_01 + l_21 * r_02;
	GFL_MTX_11 = l_01 * r_10 + l_11 * r_11 + l_21 * r_12;
	GFL_MTX_12 = l_01 * r_20 + l_11 * r_21 + l_21 * r_22;
	GFL_MTX_20 = l_02 * r_00 + l_12 * r_01 + l_22 * r_02;
	GFL_MTX_21 = l_02 * r_10 + l_12 * r_11 + l_22 * r_12;
	GFL_MTX_22 = l_02 * r_20 + l_12 * r_21 + l_22 * r_22;
#endif
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void MatrixSoa34::Transpose( const MatrixSoaNative34& m )
{
	GFL_MTX_00 = m.GFL_MTX_00;
	GFL_MTX_01 = m.GFL_MTX_10;
	GFL_MTX_02 = m.GFL_MTX_20;
	GFL_MTX_03 = m.GFL_MTX_03;
	GFL_MTX_10 = m.GFL_MTX_01;
	GFL_MTX_11 = m.GFL_MTX_11;
	GFL_MTX_12 = m.GFL_MTX_21;
	GFL_MTX_13 = m.GFL_MTX_13;
	GFL_MTX_20 = m.GFL_MTX_02;
	GFL_MTX_21 = m.GFL_MTX_12;
	GFL_MTX_22 = m.GFL_MTX_22;
	GFL_MTX_23 = m.GFL_MTX_23;
}

// -----------------------------------------------------------------
// Determinant
// -----------------------------------------------------------------
f32 MatrixSoa34::Determinant( void ) const
{
	return Determinant33( );
}

// -----------------------------------------------------------------
// Inverse
// -----------------------------------------------------------------
void MatrixSoa34::Inverse( const MatrixSoa34& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set(
		 m0.GFL_MTX_11 * m0.GFL_MTX_22 - m0.GFL_MTX_12 * m0.GFL_MTX_21, // 00
		 m0.GFL_MTX_21 * m0.GFL_MTX_02 - m0.GFL_MTX_22 * m0.GFL_MTX_01, // 01
		 m0.GFL_MTX_01 * m0.GFL_MTX_12 - m0.GFL_MTX_02 * m0.GFL_MTX_11, // 02
		 m0.GFL_MTX_01 * ( m0.GFL_MTX_13 * m0.GFL_MTX_22 - m0.GFL_MTX_12 * m0.GFL_MTX_23 ) + m0.GFL_MTX_02 * ( m0.GFL_MTX_11 * m0.GFL_MTX_23 - m0.GFL_MTX_13 * m0.GFL_MTX_21 ) + m0.GFL_MTX_03 * ( m0.GFL_MTX_12 * m0.GFL_MTX_21 - m0.GFL_MTX_11 * m0.GFL_MTX_22 ),
		 m0.GFL_MTX_12 * m0.GFL_MTX_20 - m0.GFL_MTX_10 * m0.GFL_MTX_22, // 10
		 m0.GFL_MTX_22 * m0.GFL_MTX_00 - m0.GFL_MTX_20 * m0.GFL_MTX_02, // 11
		 m0.GFL_MTX_02 * m0.GFL_MTX_10 - m0.GFL_MTX_00 * m0.GFL_MTX_12, // 12
		 m0.GFL_MTX_02 * ( m0.GFL_MTX_13 * m0.GFL_MTX_20 - m0.GFL_MTX_10 * m0.GFL_MTX_23 ) + m0.GFL_MTX_03 * ( m0.GFL_MTX_10 * m0.GFL_MTX_22 - m0.GFL_MTX_12 * m0.GFL_MTX_20 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_12 * m0.GFL_MTX_23 - m0.GFL_MTX_13 * m0.GFL_MTX_22 ),
		 m0.GFL_MTX_10 * m0.GFL_MTX_21 - m0.GFL_MTX_11 * m0.GFL_MTX_20, // 20
		 m0.GFL_MTX_20 * m0.GFL_MTX_01 - m0.GFL_MTX_21 * m0.GFL_MTX_00, // 21
		 m0.GFL_MTX_00 * m0.GFL_MTX_11 - m0.GFL_MTX_01 * m0.GFL_MTX_10, // 22
		 m0.GFL_MTX_03 * ( m0.GFL_MTX_11 * m0.GFL_MTX_20 - m0.GFL_MTX_10 * m0.GFL_MTX_21 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_13 * m0.GFL_MTX_21 - m0.GFL_MTX_11 * m0.GFL_MTX_23 ) + m0.GFL_MTX_01 * ( m0.GFL_MTX_10 * m0.GFL_MTX_23 - m0.GFL_MTX_13 * m0.GFL_MTX_20 )
		 );
	Scale( det );
}

#if GFL_USE_AOS
//==============================================================================
//==============================================================================
void MatrixSoa34::FromAOS( const MatrixAos43& src )
{
	GFL_MTX_00 = src.GFL_MTX_00;
	GFL_MTX_01 = src.GFL_MTX_10;
	GFL_MTX_02 = src.GFL_MTX_20;
	GFL_MTX_03 = src.GFL_MTX_30;
	GFL_MTX_10 = src.GFL_MTX_01;
	GFL_MTX_11 = src.GFL_MTX_11;
	GFL_MTX_12 = src.GFL_MTX_21;
	GFL_MTX_13 = src.GFL_MTX_31;
	GFL_MTX_20 = src.GFL_MTX_02;
	GFL_MTX_21 = src.GFL_MTX_12;
	GFL_MTX_22 = src.GFL_MTX_22;
	GFL_MTX_23 = src.GFL_MTX_32;
}
//==============================================================================
//==============================================================================
void MatrixSoa34::FromAOS( const MatrixAos44& src )
{
	GFL_MTX_00 = src.GFL_MTX_00;
	GFL_MTX_01 = src.GFL_MTX_10;
	GFL_MTX_02 = src.GFL_MTX_20;
	GFL_MTX_03 = src.GFL_MTX_30;
	GFL_MTX_10 = src.GFL_MTX_01;
	GFL_MTX_11 = src.GFL_MTX_11;
	GFL_MTX_12 = src.GFL_MTX_21;
	GFL_MTX_13 = src.GFL_MTX_31;
	GFL_MTX_20 = src.GFL_MTX_02;
	GFL_MTX_21 = src.GFL_MTX_12;
	GFL_MTX_22 = src.GFL_MTX_22;
	GFL_MTX_23 = src.GFL_MTX_32;
}
//==============================================================================
//==============================================================================
void MatrixSoa34::ToAOS( MatrixAos43* dst ) const
{
	dst->GFL_MTX_00 = GFL_MTX_00;
	dst->GFL_MTX_01 = GFL_MTX_10;
	dst->GFL_MTX_02 = GFL_MTX_20;
	dst->GFL_MTX_10 = GFL_MTX_01;
	dst->GFL_MTX_11 = GFL_MTX_11;
	dst->GFL_MTX_12 = GFL_MTX_21;
	dst->GFL_MTX_20 = GFL_MTX_02;
	dst->GFL_MTX_21 = GFL_MTX_12;
	dst->GFL_MTX_22 = GFL_MTX_22;
	dst->GFL_MTX_30 = GFL_MTX_03;
	dst->GFL_MTX_31 = GFL_MTX_13;
	dst->GFL_MTX_32 = GFL_MTX_23;
}
//==============================================================================
//==============================================================================
void MatrixSoa34::ToAOS( MatrixAos44* dst ) const
{
	dst->GFL_MTX_00 = GFL_MTX_00;
	dst->GFL_MTX_01 = GFL_MTX_10;
	dst->GFL_MTX_02 = GFL_MTX_20;
	dst->GFL_MTX_03 = 0.0f;
	dst->GFL_MTX_10 = GFL_MTX_01;
	dst->GFL_MTX_11 = GFL_MTX_11;
	dst->GFL_MTX_12 = GFL_MTX_21;
	dst->GFL_MTX_13 = 0.0f;
	dst->GFL_MTX_20 = GFL_MTX_02;
	dst->GFL_MTX_21 = GFL_MTX_12;
	dst->GFL_MTX_22 = GFL_MTX_22;
	dst->GFL_MTX_23 = 0.0f;
	dst->GFL_MTX_30 = GFL_MTX_03;
	dst->GFL_MTX_31 = GFL_MTX_13;
	dst->GFL_MTX_32 = GFL_MTX_23;
	dst->GFL_MTX_33 = 1.0f;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
MatrixAos44 MatrixSoa34::ToAOS( ) const
{
	MatrixAos44 mat;
	ToAOS( &mat );
	return mat;
}
#endif
#ifdef GFL_PLATFORM_3DS
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MatrixSoa34::ToSOA( gfl::math::MTX34* tag ) const
{
	( ( MatrixSoa34* )tag )->Set( *this );
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MatrixSoa34::FromSOA( const gfl::math::MTX34& src )
{
	Set( ( const MatrixSoa34& )src );
}
#endif

//==============================================================================
// 注意！ここからしたは MatrixSoa44 と共通！
//==============================================================================
void MatrixSoa34::Set( const MatrixSoaNative44& mat )
{
	Set( reinterpret_cast < const MatrixSoaNative34& >( mat ) );
}

// -----------------------------------------------------------------------------
// Set Rotate X
// -----------------------------------------------------------------------------
void MatrixSoa34::SetRotateX( const f32 s, const f32 c )
{
	SetRow( 0, Vector3::GetX1( ) );

	GFL_MTX_10 = 0.0f;
	GFL_MTX_11 = c;
	GFL_MTX_12 = -s;

	GFL_MTX_20 = 0.0f;
	GFL_MTX_21 = s;
	GFL_MTX_22 = c;
}
// -----------------------------------------------------------------------------
// Set Rotate Y
// -----------------------------------------------------------------------------
void MatrixSoa34::SetRotateY( const f32 s, const f32 c )
{
	GFL_MTX_00 = c;
	GFL_MTX_01 = 0.0f;
	GFL_MTX_02 = s;

	SetRow( 1, Vector3::GetY1( ) );

	GFL_MTX_20 = -s;
	GFL_MTX_21 = 0.0f;
	GFL_MTX_22 = c;
}
// -----------------------------------------------------------------------------
// Set Rotate Z
// -----------------------------------------------------------------------------
void MatrixSoa34::SetRotateZ( const f32 s, const f32 c )
{
	GFL_MTX_00 = c;
	GFL_MTX_01 = -s;
	GFL_MTX_02 = 0.0f;
	GFL_MTX_10 = s;
	GFL_MTX_11 = c;
	GFL_MTX_12 = 0.0f;
	SetRow( 2, Vector3::GetZ1( ) );
}
// -----------------------------------------------------------------------------
// Set Scale
// -----------------------------------------------------------------------------
void MatrixSoa34::SetScale( const f32 x, const f32 y, const f32 z )
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
// -----------------------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateX( const MatrixSoa34& m0, const f32 s, const f32 c )
{
	GFL_MTX_00 = m0.GFL_MTX_00;
	GFL_MTX_10 = m0.GFL_MTX_10;
	GFL_MTX_20 = m0.GFL_MTX_20;
	GFL_MTX_03 = m0.GFL_MTX_03;
	GFL_MTX_13 = m0.GFL_MTX_13;
	GFL_MTX_23 = m0.GFL_MTX_23;
	GFL_MTX_01 = m0.GFL_MTX_02 * s + m0.GFL_MTX_01 * c;
	GFL_MTX_11 = m0.GFL_MTX_12 * s + m0.GFL_MTX_11 * c;
	GFL_MTX_21 = m0.GFL_MTX_22 * s + m0.GFL_MTX_21 * c;
	GFL_MTX_02 = m0.GFL_MTX_02 * c - m0.GFL_MTX_01 * s;
	GFL_MTX_12 = m0.GFL_MTX_12 * c - m0.GFL_MTX_11 * s;
	GFL_MTX_22 = m0.GFL_MTX_22 * c - m0.GFL_MTX_21 * s;
}
// -----------------------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateX( const f32 s, const f32 c )
{
	const f32 m01 = GFL_MTX_01, m02 = GFL_MTX_02;
	const f32 m11 = GFL_MTX_11, m12 = GFL_MTX_12;
	const f32 m21 = GFL_MTX_21, m22 = GFL_MTX_22;

	GFL_MTX_01 = m02 * s + m01 * c;
	GFL_MTX_11 = m12 * s + m11 * c;
	GFL_MTX_21 = m22 * s + m21 * c;
	GFL_MTX_02 = m02 * c - m01 * s;
	GFL_MTX_12 = m12 * c - m11 * s;
	GFL_MTX_22 = m22 * c - m21 * s;
}
// -----------------------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateX_HalfPi( void )
{
	const f32 m01 = GFL_MTX_01, m02 = GFL_MTX_02;
	const f32 m11 = GFL_MTX_11, m12 = GFL_MTX_12;
	const f32 m21 = GFL_MTX_21, m22 = GFL_MTX_22;

	GFL_MTX_01 = m02;
	GFL_MTX_11 = m12;
	GFL_MTX_21 = m22;
	GFL_MTX_02 = -m01;
	GFL_MTX_12 = -m11;
	GFL_MTX_22 = -m21;
}
// -----------------------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateX_MinusHalfPi( void )
{
	const f32 m01 = GFL_MTX_01, m02 = GFL_MTX_02;
	const f32 m11 = GFL_MTX_11, m12 = GFL_MTX_12;
	const f32 m21 = GFL_MTX_21, m22 = GFL_MTX_22;

	GFL_MTX_01 = -m02;
	GFL_MTX_11 = -m12;
	GFL_MTX_21 = -m22;
	GFL_MTX_02 = m01;
	GFL_MTX_12 = m11;
	GFL_MTX_22 = m21;
}
// -----------------------------------------------------------------------------
// RotateX
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateX_OnePi( void )
{
	const f32 m01 = GFL_MTX_01, m02 = GFL_MTX_02;
	const f32 m11 = GFL_MTX_11, m12 = GFL_MTX_12;
	const f32 m21 = GFL_MTX_21, m22 = GFL_MTX_22;

	GFL_MTX_01 = -m01;
	GFL_MTX_11 = -m11;
	GFL_MTX_21 = -m21;
	GFL_MTX_02 = -m02;
	GFL_MTX_12 = -m12;
	GFL_MTX_22 = -m22;
}
// -----------------------------------------------------------------------------
// RotateX Reverse
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateX_Reverse( const f32 s, const f32 c )
{
	const f32 m01 = GFL_MTX_10, m02 = GFL_MTX_20;
	const f32 m11 = GFL_MTX_11, m12 = GFL_MTX_21;
	const f32 m21 = GFL_MTX_12, m22 = GFL_MTX_22;

	GFL_MTX_10 = m01 * c - m02 * s;
	GFL_MTX_11 = m11 * c - m12 * s;
	GFL_MTX_12 = m21 * c - m22 * s;
	GFL_MTX_20 = m02 * c + m01 * s;
	GFL_MTX_21 = m12 * c + m11 * s;
	GFL_MTX_22 = m22 * c + m21 * s;
}

// -----------------------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateY( const MatrixSoa34& m0, const f32 s, const f32 c )
{
	GFL_MTX_01 = m0.GFL_MTX_01;
	GFL_MTX_11 = m0.GFL_MTX_11;
	GFL_MTX_21 = m0.GFL_MTX_21;
	GFL_MTX_03 = m0.GFL_MTX_03;
	GFL_MTX_13 = m0.GFL_MTX_13;
	GFL_MTX_23 = m0.GFL_MTX_23;
	GFL_MTX_02 = m0.GFL_MTX_00 * c + m0.GFL_MTX_02 * s;
	GFL_MTX_12 = m0.GFL_MTX_10 * c + m0.GFL_MTX_12 * s;
	GFL_MTX_22 = m0.GFL_MTX_20 * c + m0.GFL_MTX_22 * s;
	GFL_MTX_00 = m0.GFL_MTX_00 * c - m0.GFL_MTX_02 * s;
	GFL_MTX_10 = m0.GFL_MTX_10 * c - m0.GFL_MTX_12 * s;
	GFL_MTX_20 = m0.GFL_MTX_20 * c - m0.GFL_MTX_22 * s;
}
// -----------------------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateY( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_02;
	const f32 m01 = GFL_MTX_10, m21 = GFL_MTX_12;
	const f32 m02 = GFL_MTX_20, m22 = GFL_MTX_22;

	GFL_MTX_02 = m00 * s + m20 * c;
	GFL_MTX_12 = m01 * s + m21 * c;
	GFL_MTX_22 = m02 * s + m22 * c;
	GFL_MTX_00 = m00 * c - m20 * s;
	GFL_MTX_10 = m01 * c - m21 * s;
	GFL_MTX_20 = m02 * c - m22 * s;
}
// -----------------------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateY_HalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_02;
	const f32 m01 = GFL_MTX_10, m21 = GFL_MTX_12;
	const f32 m02 = GFL_MTX_20, m22 = GFL_MTX_22;

	GFL_MTX_02 = m00;
	GFL_MTX_12 = m01;
	GFL_MTX_22 = m02;
	GFL_MTX_00 = -m20;
	GFL_MTX_10 = -m21;
	GFL_MTX_20 = -m22;
}
// -----------------------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateY_MinusHalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_02;
	const f32 m01 = GFL_MTX_10, m21 = GFL_MTX_12;
	const f32 m02 = GFL_MTX_20, m22 = GFL_MTX_22;

	GFL_MTX_02 = -m00;
	GFL_MTX_12 = -m01;
	GFL_MTX_22 = -m02;
	GFL_MTX_00 = m20;
	GFL_MTX_10 = m21;
	GFL_MTX_20 = m22;
}
// -----------------------------------------------------------------------------
// Rotate Y
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateY_OnePi( void )
{
	const f32 m00 = GFL_MTX_00, m20 = GFL_MTX_02;
	const f32 m01 = GFL_MTX_10, m21 = GFL_MTX_12;
	const f32 m02 = GFL_MTX_20, m22 = GFL_MTX_22;

	GFL_MTX_02 = -m20;
	GFL_MTX_12 = -m21;
	GFL_MTX_22 = -m22;
	GFL_MTX_00 = -m00;
	GFL_MTX_10 = -m01;
	GFL_MTX_20 = -m02;
}
// -----------------------------------------------------------------------------
// Rotate Y Reverse
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateY_Reverse( const MatrixSoa34& m0, const f32 s, const f32 c )
{
	const f32 m00 = m0.GFL_MTX_00, m02 = m0.GFL_MTX_20;
	const f32 m10 = m0.GFL_MTX_01, m12 = m0.GFL_MTX_21;
	const f32 m20 = m0.GFL_MTX_02, m22 = m0.GFL_MTX_22;

	GFL_MTX_10 = m0.GFL_MTX_10;
	GFL_MTX_11 = m0.GFL_MTX_11;
	GFL_MTX_12 = m0.GFL_MTX_12;
	GFL_MTX_13 = m0.GFL_MTX_13;

	GFL_MTX_00 = m00 * c + m02 * s;
	GFL_MTX_01 = m10 * c + m12 * s;
	GFL_MTX_02 = m20 * c + m22 * s;
	GFL_MTX_20 = m02 * c - m00 * s;
	GFL_MTX_21 = m12 * c - m10 * s;
	GFL_MTX_22 = m22 * c - m20 * s;

	const f32 x = m0.GFL_MTX_03;
	const f32 z = m0.GFL_MTX_23;
	GFL_MTX_03 = Math::GetX_RotateY_SinCosXZ( s, c, x, z );
	GFL_MTX_23 = Math::GetZ_RotateY_SinCosXZ( s, c, x, z );
}
// -----------------------------------------------------------------------------
// Rotate Y Reverse
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateY_Reverse( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m02 = GFL_MTX_20;
	const f32 m10 = GFL_MTX_01, m12 = GFL_MTX_21;
	const f32 m20 = GFL_MTX_02, m22 = GFL_MTX_22;

	GFL_MTX_00 = m00 * c + m02 * s;
	GFL_MTX_01 = m10 * c + m12 * s;
	GFL_MTX_02 = m20 * c + m22 * s;
	GFL_MTX_20 = m02 * c - m00 * s;
	GFL_MTX_21 = m12 * c - m10 * s;
	GFL_MTX_22 = m22 * c - m20 * s;
}
// -----------------------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateZ( const MatrixSoa34& m0, const f32 s, const f32 c )
{
	GFL_MTX_02 = m0.GFL_MTX_02;
	GFL_MTX_12 = m0.GFL_MTX_12;
	GFL_MTX_22 = m0.GFL_MTX_22;
	GFL_MTX_03 = m0.GFL_MTX_03;
	GFL_MTX_13 = m0.GFL_MTX_13;
	GFL_MTX_23 = m0.GFL_MTX_23;
	GFL_MTX_00 = m0.GFL_MTX_01 * s + m0.GFL_MTX_00 * c;
	GFL_MTX_10 = m0.GFL_MTX_11 * s + m0.GFL_MTX_10 * c;
	GFL_MTX_20 = m0.GFL_MTX_21 * s + m0.GFL_MTX_20 * c;
	GFL_MTX_01 = m0.GFL_MTX_01 * c - m0.GFL_MTX_00 * s;
	GFL_MTX_11 = m0.GFL_MTX_11 * c - m0.GFL_MTX_10 * s;
	GFL_MTX_21 = m0.GFL_MTX_21 * c - m0.GFL_MTX_20 * s;
}
// -----------------------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateZ( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_01;
	const f32 m01 = GFL_MTX_10, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_20, m12 = GFL_MTX_21;

	GFL_MTX_00 = m10 * s + m00 * c;
	GFL_MTX_10 = m11 * s + m01 * c;
	GFL_MTX_20 = m12 * s + m02 * c;
	GFL_MTX_01 = m10 * c - m00 * s;
	GFL_MTX_11 = m11 * c - m01 * s;
	GFL_MTX_21 = m12 * c - m02 * s;
}
// -----------------------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateZ_HalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_01;
	const f32 m01 = GFL_MTX_10, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_20, m12 = GFL_MTX_21;

	GFL_MTX_00 = m10;
	GFL_MTX_10 = m11;
	GFL_MTX_20 = m12;
	GFL_MTX_01 = -m00;
	GFL_MTX_11 = -m01;
	GFL_MTX_21 = -m02;
}
// -----------------------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateZ_MinusHalfPi( void )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_01;
	const f32 m01 = GFL_MTX_10, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_20, m12 = GFL_MTX_21;

	GFL_MTX_00 = -m10;
	GFL_MTX_10 = -m11;
	GFL_MTX_20 = -m12;
	GFL_MTX_01 = m00;
	GFL_MTX_11 = m01;
	GFL_MTX_21 = m02;
}
// -----------------------------------------------------------------------------
// Rotate Z
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateZ_OnePi( void )
{
	const f32 m00 = GFL_MTX_00, m10 = GFL_MTX_01;
	const f32 m01 = GFL_MTX_10, m11 = GFL_MTX_11;
	const f32 m02 = GFL_MTX_20, m12 = GFL_MTX_21;

	GFL_MTX_00 = -m00;
	GFL_MTX_10 = -m01;
	GFL_MTX_20 = -m02;
	GFL_MTX_01 = -m10;
	GFL_MTX_11 = -m11;
	GFL_MTX_21 = -m12;
}
// -----------------------------------------------------------------------------
// Rotate Z Reverese
// -----------------------------------------------------------------------------
void MatrixSoa34::RotateZ_Reverse( const f32 s, const f32 c )
{
	const f32 m00 = GFL_MTX_00, m01 = GFL_MTX_10;
	const f32 m10 = GFL_MTX_01, m11 = GFL_MTX_11;
	const f32 m20 = GFL_MTX_02, m21 = GFL_MTX_12;

	GFL_MTX_00 = m00 * c - m01 * s;
	GFL_MTX_01 = m10 * c - m11 * s;
	GFL_MTX_02 = m20 * c - m21 * s;
	GFL_MTX_10 = m00 * s + m01 * c;
	GFL_MTX_11 = m10 * s + m11 * c;
	GFL_MTX_21 = m20 * s + m21 * c;
}
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void MatrixSoa34::Apply( VectorNative* v0, const VectorNative& v1 ) const
{
	f32 x = v1.GFL_VECTOR_X;
	f32 y = v1.GFL_VECTOR_Y;
	f32 z = v1.GFL_VECTOR_Z;

	v0->GFL_VECTOR_X = ( Get( 0, 0 ) * x + Get( 0, 1 ) * y + Get( 0, 2 ) * z );
	v0->GFL_VECTOR_Y = ( Get( 1, 0 ) * x + Get( 1, 1 ) * y + Get( 1, 2 ) * z );
	v0->GFL_VECTOR_Z = ( Get( 2, 0 ) * x + Get( 2, 1 ) * y + Get( 2, 2 ) * z );
}
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void MatrixSoa34::ApplyTranspose( VectorNative* v0, const VectorNative& v1 ) const
{
	f32 x = v1.GFL_VECTOR_X;
	f32 y = v1.GFL_VECTOR_Y;
	f32 z = v1.GFL_VECTOR_Z;

	v0->GFL_VECTOR_X = ( Get( 0, 0 ) * x + Get( 1, 0 ) * y + Get( 2, 0 ) * z );
	v0->GFL_VECTOR_Y = ( Get( 0, 1 ) * x + Get( 1, 1 ) * y + Get( 2, 1 ) * z );
	v0->GFL_VECTOR_Z = ( Get( 0, 2 ) * x + Get( 1, 2 ) * y + Get( 2, 2 ) * z );
}
// -----------------------------------------------------------------------------
// Determinant33
// -----------------------------------------------------------------------------
f32 MatrixSoa34::Determinant33( void ) const
{
	return(GFL_MTX_00 * GFL_MTX_11 - GFL_MTX_10 * GFL_MTX_01 )* GFL_MTX_22
		- ( GFL_MTX_00 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_01 ) * GFL_MTX_12
		+ ( GFL_MTX_10 * GFL_MTX_21 - GFL_MTX_20 * GFL_MTX_11 ) * GFL_MTX_02;
}
// -----------------------------------------------------------------------------
// Inverse33
// -----------------------------------------------------------------------------
void MatrixSoa34::Inverse33( const MatrixSoa34& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set33(
		 ( m0.GFL_MTX_11 * m0.GFL_MTX_22 - m0.GFL_MTX_12 * m0.GFL_MTX_21 ) * det, // 00
		 ( m0.GFL_MTX_21 * m0.GFL_MTX_02 - m0.GFL_MTX_22 * m0.GFL_MTX_01 ) * det, // 01
		 ( m0.GFL_MTX_01 * m0.GFL_MTX_12 - m0.GFL_MTX_02 * m0.GFL_MTX_11 ) * det, // 02
		 ( m0.GFL_MTX_12 * m0.GFL_MTX_20 - m0.GFL_MTX_10 * m0.GFL_MTX_22 ) * det, // 10
		 ( m0.GFL_MTX_22 * m0.GFL_MTX_00 - m0.GFL_MTX_20 * m0.GFL_MTX_02 ) * det, // 11
		 ( m0.GFL_MTX_02 * m0.GFL_MTX_10 - m0.GFL_MTX_00 * m0.GFL_MTX_12 ) * det, // 12
		 ( m0.GFL_MTX_10 * m0.GFL_MTX_21 - m0.GFL_MTX_11 * m0.GFL_MTX_20 ) * det, // 20
		 ( m0.GFL_MTX_20 * m0.GFL_MTX_01 - m0.GFL_MTX_21 * m0.GFL_MTX_00 ) * det, // 21
		 ( m0.GFL_MTX_00 * m0.GFL_MTX_11 - m0.GFL_MTX_01 * m0.GFL_MTX_10 ) * det // 22
		 );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixSoa34::GetPositionByLowestY( VectorNative* res, const f32 y0, const f32 r ) const
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

// -----------------------------------------------------------------------------
// Matrix To Quaternion
// -----------------------------------------------------------------------------
void MatrixSoa34::MatrixToQuaternion( QuaternionNative* q ) const
{
	const f32 tr = GFL_MTX_00 + GFL_MTX_11 + GFL_MTX_22;
	f32 ww, max;
	s32 index;

	if( tr >= 0.0f ){
		ww = Math::FSqrt( tr + 1.0f );
		q->GFL_VECTOR_W = ( ww * 0.5f );
		ww = -0.5f / ww;

		q->GFL_VECTOR_X = ( ( GFL_MTX_12 - GFL_MTX_21 ) * ww );
		q->GFL_VECTOR_Y = ( ( GFL_MTX_20 - GFL_MTX_02 ) * ww );
		q->GFL_VECTOR_Z = ( ( GFL_MTX_01 - GFL_MTX_10 ) * ww );
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
		q->GFL_VECTOR_Y = ( ( GFL_MTX_10 + GFL_MTX_01 ) * ww );
		q->GFL_VECTOR_Z = ( ( GFL_MTX_02 + GFL_MTX_20 ) * ww );
		q->GFL_VECTOR_W = ( ( GFL_MTX_21 - GFL_MTX_12 ) * ww );
		break;
	case 1:
		ww = Math::FSqrt( GFL_MTX_11 - GFL_MTX_00 - GFL_MTX_22 + 1.0f );
		q->GFL_VECTOR_Y = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_Z = ( ( GFL_MTX_21 + GFL_MTX_12 ) * ww );
		q->GFL_VECTOR_X = ( ( GFL_MTX_10 + GFL_MTX_01 ) * ww );
		q->GFL_VECTOR_W = ( ( GFL_MTX_02 - GFL_MTX_20 ) * ww );
		break;
	case 2:
		ww = Math::FSqrt( GFL_MTX_22 - GFL_MTX_00 - GFL_MTX_11 + 1.0f );
		q->GFL_VECTOR_Z = ( ww * 0.5f );
		ww = 0.5f / ww;
		q->GFL_VECTOR_X = ( ( GFL_MTX_02 + GFL_MTX_20 ) * ww );
		q->GFL_VECTOR_Y = ( ( GFL_MTX_21 + GFL_MTX_12 ) * ww );
		q->GFL_VECTOR_W = ( ( GFL_MTX_10 - GFL_MTX_01 ) * ww );
	}
	//	q->Normalize();
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Quaternion MatrixSoa34::MatrixToQuaternion( void ) const
{
	Quaternion qt;
	MatrixToQuaternion( &qt );
	return qt;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Quaternion MatrixSoa34::MatrixToQuaternion( VectorNative* scl ) const
{
	MatrixSoa34 mat(*this);
	mat.Normalize(scl);
	Quaternion qt;
	mat.MatrixToQuaternion( &qt );
	return qt;
}
// -----------------------------------------------------------------------------
// Quaternion To Matrix
// -----------------------------------------------------------------------------
void MatrixSoa34::QuaternionToMatrix( const QuaternionNative& q )
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
	GFL_MTX_01 = 2.0f * ( xy - wz );
	GFL_MTX_02 = 2.0f * ( xz + wy );

	GFL_MTX_10 = 2.0f * ( xy + wz );
	GFL_MTX_11 = 1.0f - 2.0f * ( xx + zz );
	GFL_MTX_12 = 2.0f * ( yz - wx );

	GFL_MTX_20 = 2.0f * ( xz - wy );
	GFL_MTX_21 = 2.0f * ( yz + wx );
	GFL_MTX_22 = 1.0f - 2.0f * ( xx + yy );

	if( !Math::IsSimilar( scl, 1.0f, 0.01f ) ){
		Scale( scl );
	}
}
// -----------------------------------------------------------------------------
// GetRadian
// -----------------------------------------------------------------------------
void MatrixSoa34::GetRadian( f32* x, f32* y, f32* z ) const
{
	f32 sy = GFL_MTX_20;
	if( 1.0f - gfl::core::Math::EPSILON <= sy ){
		*x = Math::FATan2( -GFL_MTX_01, GFL_MTX_11 );
		*y = -Math::HALF_PI;
		*z = 0.0f;
	} else if( sy <= -1.0f + gfl::core::Math::EPSILON ){
		*x = Math::FATan2( GFL_MTX_01, GFL_MTX_11 );
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
		*x = Math::FATan2( GFL_MTX_21, GFL_MTX_22 );
		*y = Math::FASin( -sy ); //  / GetScaleX( )
		*z = Math::FATan2( GFL_MTX_10, GFL_MTX_00 );
#endif
	}
	//	Math::OptimizeRadian( x, y, z );
}

// -----------------------------------------------------------------------------
// GetRadian
// -----------------------------------------------------------------------------
void MatrixSoa34::GetRadianYXZ( f32* x, f32* y, f32* z ) const
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
	f32 sx = GFL_MTX_12;
	if( 1.0f - gfl::core::Math::EPSILON <= sx ){
		if( x ){
			*x = -Math::HALF_PI;
		}
		*y = 0.0f;
		if( z ){
			*z = Math::FATan2( -GFL_MTX_20, GFL_MTX_00 );
		}
	} else if( sx <= -1.0f + gfl::core::Math::EPSILON ){
		if( x ){
			*x = Math::HALF_PI;
		}
		*y = 0.0f;
		if( z ){
			*z = Math::FATan2( GFL_MTX_20, GFL_MTX_00 );
		}
	} else {
		if( x ){
			*x = Math::FASin( -sx );
		}
		*y = Math::FATan2( GFL_MTX_02, GFL_MTX_22 );
		if( z ){
			*z = Math::FATan2( GFL_MTX_10, GFL_MTX_11 );
		}
	}
#endif
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MatrixSoa34::GlobalToLocal33( VectorNative* loc, const VectorNative& pos ) const
{
	ApplyTranspose( loc, pos );
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MatrixSoa34::GlobalToLocal33Scaled( VectorNative* loc, const VectorNative& pos ) const
{
	GetInverse33( ).Apply( loc, pos );
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimZY( const VectorNative& tag, f32* zsin, f32* zcos, f32* ysin
						 , f32* ycos, const bool reverse, const bool scaled ) const
{
	Vector loc;
	//	MatrixSoa34 mat(*this);

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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimYZ( const VectorNative& tag, f32* ysin, f32* ycos, f32* zsin
						 , f32* zcos, const bool reverse, const bool scaled ) const
{
	Vector loc;
	//	MatrixSoa34 mat(*this);

	//	mat.(!scaled) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimYX( const VectorNative& tag, f32* ysin, f32* ycos, f32* xsin
						 , f32* xcos, const bool reverse, const bool scaled ) const
{
	Vector loc;
	//	MatrixSoa34 mat( *this );

	//	mat.(!scaled) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimXZ( const VectorNative& tag, f32* xsin, f32* xcos, f32* zsin
						 , f32* zcos, const bool reverse, const bool scaled ) const
{
	Vector loc;
	//	MatrixSoa34 mat( *this );

	//	mat.(!scaled) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimXY( const VectorNative& tag, f32* xsin, f32* xcos, f32* zsin
						 , f32* zcos, const bool reverse, const bool scaled ) const
{
	Vector loc;
	//	MatrixSoa34 mat( *this );

	//	mat.(!scaled) ? GlobalToLocal( &loc, tag ) : GlobalToLocalScaled( &loc, tag );
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimZY( const VectorNative& tag, const bool reverse, const f32 rate )
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimYZ( const VectorNative& tag, const bool reverse, const f32 rate )
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimYX( const VectorNative& tag, const bool reverse, const f32 rate )
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimXZ( const VectorNative& tag, const bool reverse, const f32 rate )
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool MatrixSoa34::AimXY( const VectorNative& tag, const bool reverse, const f32 rate )
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MatrixSoa34::AimX( f32* xsin, f32* xcos )
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
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MatrixSoa34::AimX( void )
{
	f32 xsin, xcos;
	AimX( &xsin, &xcos );
	RotateX( xsin, xcos );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MatrixSoa34::AimY( const gfl::core::Vector& tag )
{
	Vector loc;
	
	GlobalToLocal( &loc, tag );

	const f32 lx = loc.GFL_VECTOR_X;
	const f32 lz = loc.GFL_VECTOR_Z;

	const f32 tmp_0 = lx * lx + lz * lz;
	if( Math::IsAlmostZero( tmp_0 ) ){
		return;
	}
	const f32 tmp_2 = Math::FRecip( Math::FSqrt( tmp_0 ) );

//	RotateY( lx * tmp_2, lz * tmp_2 ); // こっちのはずだけど…
	RotateY( -lz * tmp_2, lx * tmp_2 );
}

#if 0 // 
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixSoa34::Clamp( const BaseMatrix34& parent, const VectorNative4& low, const VectorNative4& high )
{
	Vector4 ang;
	MatrixSoa34 loc;
	loc.Mul33( parent.GetTranspose33( ), *this );
	loc.GetRadian( &ang );

	ang.Clamp( low, high );

	Set33( parent );
	RotateZYX( ang );
}
#endif
// -----------------------------------------------------------------------------
// IsSimilar
// -----------------------------------------------------------------------------
bool MatrixSoa34::IsSimilar( const MatrixSoa34& m0, const f32 rot_scale_range, const f32 trans_range ) const
{
	return GetRow( 0 ).IsSimilar( m0.GetRow( 0 ), rot_scale_range ) &&
		GetRow( 1 ).IsSimilar( m0.GetRow( 1 ), rot_scale_range ) &&
		GetRow( 2 ).IsSimilar( m0.GetRow( 2 ), rot_scale_range );
}
// -----------------------------------------------------------------------------
// Normalize
// -----------------------------------------------------------------------------
void MatrixSoa34::Normalize( VectorNative* scl )
{
	gfl::core::Vector3 vx, vy, vz;

	vx = GetColumn( 0 );
	vy = GetColumn( 1 );
	vz = GetColumn( 2 );

	if( !scl ){
		vx.Normalize( );
		vy.Normalize( );
		vz.Normalize( );
	} else {
		scl->Set( vx.NormalizeRet( ),
				 vy.NormalizeRet( ),
				 vz.NormalizeRet( ) );
	}

	SetColumn( 0, vx );
	SetColumn( 1, vy );
	SetColumn( 2, vz );
}
//==============================================================================
//==============================================================================
MatrixSoa34& MatrixSoa34::Slerp( const MatrixSoa34& m0, const MatrixSoa34& m1, const f32 rate )
{
	Quaternion res;
	//	m0.MatrixToQuaternion().PrintConsole("m0");
	//	m1.MatrixToQuaternion().PrintConsole("m1");
	res.Slerp( m0.MatrixToQuaternion( ), m1.MatrixToQuaternion( ), rate );
	//	res.PrintConsole("q");
	res.QuaternionToMatrix( this );
	return *this;
}
// -----------------------------------------------------------------
// operator ==
// -----------------------------------------------------------------
bool MatrixSoa34::operator==( const MatrixSoa34& m0 )
{
	return GetRow( 0 ) == m0.GetRow( 0 ) &&
		GetRow( 1 ) == m0.GetRow( 1 ) &&
		GetRow( 2 ) == m0.GetRow( 2 );
}

// -----------------------------------------------------------------
// operator !=
// -----------------------------------------------------------------
bool MatrixSoa34::operator!=( const MatrixSoa34& m0 )
{
	return GetRow( 0 ) != m0.GetRow( 0 ) ||
		GetRow( 1 ) != m0.GetRow( 1 ) ||
		GetRow( 2 ) != m0.GetRow( 2 );
}
// -----------------------------------------------------------------------------
// Print
// -----------------------------------------------------------------------------
void MatrixSoa34::PrintConsole( void ) const
{
	Debug::PrintConsole( "%ff, %ff, %ff, %ff, \n%ff, %ff, %ff, %ff, \n%ff, %ff, %ff, %ff, \n",
						 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, GFL_MTX_03,
						 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, GFL_MTX_13,
						 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, GFL_MTX_23 );
}
}
}

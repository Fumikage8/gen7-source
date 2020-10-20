// マトリックス演算系　※汎用性重視で各ハードウェアに対して最適化はしていません

namespace gfl {
namespace core {

// =================================================================
// Matrix44Soa
// =================================================================
// ---------------------------------------------------------------------------------
// LookAtMatrix生成
// ---------------------------------------------------------------------------------
MatrixSoa44 MatrixSoa44::GetLookAt( const Vector& eye, const Vector& aim, const Vector& up )
{
	return GetLookAtGL( eye, aim, up );
}

// ---------------------------------------------------------------------------------
// OpenGL互換 LookAtMatrix生成
// ---------------------------------------------------------------------------------
MatrixSoa44 MatrixSoa44::GetLookAtGL( const Vector& eye, const Vector& aim, const Vector& up )
{
	Vector3 d( aim - eye );
	d.Normalize( );

	Vector3 s( d );
	s.Cross( up );
	s.Normalize( );

	Vector3 r( s );
	r.Cross( d );

	MatrixSoa44 mat;

	mat.Set( s.GFL_VECTOR_X, s.GFL_VECTOR_Y, s.GFL_VECTOR_Z, -eye.Dot( s ),
			 r.GFL_VECTOR_X, r.GFL_VECTOR_Y, r.GFL_VECTOR_Z, -eye.Dot( r ),
			 -d.GFL_VECTOR_X, -d.GFL_VECTOR_Y, -d.GFL_VECTOR_Z, eye.Dot( d ),
			 0.0f, 0.0f, 0.0f, 1.0f );
	//mat.Inverse();

	return mat;
}

// ---------------------------------------------------------------------------------
// PerspectiveMatrix生成
// ---------------------------------------------------------------------------------
MatrixSoa44 MatrixSoa44::GetPerspective( float fovy, float aspect, float zNear, float zFar )
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
MatrixSoa44 MatrixSoa44::GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar )
{
	f32 f, rangeInv;

	f = tanf( ( Math::HALF_PI ) - fovy * 0.5f );
	//f = Math::FCos( fovy * 0.5f ); // これじゃ駄目な場合もあるらしい
	rangeInv = 1.0f / ( zNear - zFar );

	MatrixSoa44 mat;

	mat.Set( f / aspect, 0.0f, 0.0f, 0.0f,
			 0.0f, f, 0.0f, 0.0f,
			 0.0f, 0.0f, ( zNear + zFar ) * rangeInv, zNear * zFar * rangeInv * 2.0f,
			 0.0f, 0.0f, -1.0f, 0.0f );

	return mat;
}

// -----------------------------------------------------------------
// Set
// -----------------------------------------------------------------
void MatrixSoa44::Set( f32 m0, f32 m1, f32 m2, f32 m3,
					   f32 m4, f32 m5, f32 m6, f32 m7,
					   f32 m8, f32 m9, f32 m10, f32 m11,
					   f32 m12, f32 m13, f32 m14, f32 m15 )
{
	CAST_34_PTR( this )->Set( m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11 );
	GetRow( 3 ).Set( m12, m13, m14, m15 );
}

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void MatrixSoa44::Mul( const MatrixSoa44& m0, const MatrixSoa44& m1 )
{
#ifdef GFL_PLATFORM_3DS
	* this = m0 * m1;
#else // 
	const f32 l_00 = m0.GFL_MTX_00, l_01 = m0.GFL_MTX_10, l_02 = m0.GFL_MTX_20, l_03 = m0.GFL_MTX_30;
	const f32 l_10 = m0.GFL_MTX_01, l_11 = m0.GFL_MTX_11, l_12 = m0.GFL_MTX_21, l_13 = m0.GFL_MTX_31;
	const f32 l_20 = m0.GFL_MTX_02, l_21 = m0.GFL_MTX_12, l_22 = m0.GFL_MTX_22, l_23 = m0.GFL_MTX_32;
	const f32 l_30 = m0.GFL_MTX_03, l_31 = m0.GFL_MTX_13, l_32 = m0.GFL_MTX_23, l_33 = m0.GFL_MTX_33;
	const f32 r_00 = m1.GFL_MTX_00, r_01 = m1.GFL_MTX_10, r_02 = m1.GFL_MTX_20, r_03 = m1.GFL_MTX_30;

	GFL_MTX_00 = l_00 * r_00 + l_10 * r_01 + l_20 * r_02 + l_30 * r_03;
	GFL_MTX_10 = l_01 * r_00 + l_11 * r_01 + l_21 * r_02 + l_31 * r_03;
	GFL_MTX_20 = l_02 * r_00 + l_12 * r_01 + l_22 * r_02 + l_32 * r_03;
	GFL_MTX_30 = l_03 * r_00 + l_13 * r_01 + l_23 * r_02 + l_33 * r_03;

	const f32 r_10 = m1.GFL_MTX_01, r_11 = m1.GFL_MTX_11, r_12 = m1.GFL_MTX_21, r_13 = m1.GFL_MTX_31;

	GFL_MTX_01 = l_00 * r_10 + l_10 * r_11 + l_20 * r_12 + l_30 * r_13;
	GFL_MTX_11 = l_01 * r_10 + l_11 * r_11 + l_21 * r_12 + l_31 * r_13;
	GFL_MTX_21 = l_02 * r_10 + l_12 * r_11 + l_22 * r_12 + l_32 * r_13;
	GFL_MTX_31 = l_03 * r_10 + l_13 * r_11 + l_23 * r_12 + l_33 * r_13;

	const f32 r_20 = m1.GFL_MTX_02, r_21 = m1.GFL_MTX_12, r_22 = m1.GFL_MTX_22, r_23 = m1.GFL_MTX_32;

	GFL_MTX_02 = l_00 * r_20 + l_10 * r_21 + l_20 * r_22 + l_30 * r_23;
	GFL_MTX_12 = l_01 * r_20 + l_11 * r_21 + l_21 * r_22 + l_31 * r_23;
	GFL_MTX_22 = l_02 * r_20 + l_12 * r_21 + l_22 * r_22 + l_32 * r_23;
	GFL_MTX_32 = l_03 * r_20 + l_13 * r_21 + l_23 * r_22 + l_33 * r_23;

	const f32 r_30 = m1.GFL_MTX_03, r_31 = m1.GFL_MTX_13, r_32 = m1.GFL_MTX_23, r_33 = m1.GFL_MTX_33;

	GFL_MTX_03 = l_00 * r_30 + l_10 * r_31 + l_20 * r_32 + l_30 * r_33;
	GFL_MTX_13 = l_01 * r_30 + l_11 * r_31 + l_21 * r_32 + l_31 * r_33;
	GFL_MTX_23 = l_02 * r_30 + l_12 * r_31 + l_22 * r_32 + l_32 * r_33;
	GFL_MTX_33 = l_03 * r_30 + l_13 * r_31 + l_23 * r_32 + l_33 * r_33;
#endif
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void MatrixSoa44::Transpose( const MatrixSoa44& m0 )
{
	GFL_MTX_00 = m0.GFL_MTX_00;
	GFL_MTX_01 = m0.GFL_MTX_10;
	GFL_MTX_02 = m0.GFL_MTX_20;
	GFL_MTX_03 = m0.GFL_MTX_30;
	GFL_MTX_10 = m0.GFL_MTX_01;
	GFL_MTX_11 = m0.GFL_MTX_11;
	GFL_MTX_12 = m0.GFL_MTX_21;
	GFL_MTX_13 = m0.GFL_MTX_31;
	GFL_MTX_20 = m0.GFL_MTX_02;
	GFL_MTX_21 = m0.GFL_MTX_12;
	GFL_MTX_22 = m0.GFL_MTX_22;
	GFL_MTX_23 = m0.GFL_MTX_32;
	GFL_MTX_30 = m0.GFL_MTX_03;
	GFL_MTX_31 = m0.GFL_MTX_13;
	GFL_MTX_32 = m0.GFL_MTX_23;
	GFL_MTX_33 = m0.GFL_MTX_33;
}

// -----------------------------------------------------------------
// Transpose
// -----------------------------------------------------------------
void MatrixSoa44::Transpose( void )
{
	Math::Swap( GFL_MTX_01, GFL_MTX_10 );
	Math::Swap( GFL_MTX_02, GFL_MTX_20 );
	Math::Swap( GFL_MTX_03, GFL_MTX_30 );
	Math::Swap( GFL_MTX_12, GFL_MTX_21 );
	Math::Swap( GFL_MTX_13, GFL_MTX_31 );
	Math::Swap( GFL_MTX_23, GFL_MTX_32 );
}

// -----------------------------------------------------------------
// Inverse
// -----------------------------------------------------------------
void MatrixSoa44::Inverse( const MatrixSoa44& m0 )
{
	f32 det = m0.Determinant33( );
	if( det < Math::EPSILON ){
		*this = m0;
		return;
	}
	det = Math::FRecip( det );
	Set(
		 m0.GFL_MTX_11 * ( m0.GFL_MTX_22 * m0.GFL_MTX_33 - m0.GFL_MTX_23 * m0.GFL_MTX_32 ) + m0.GFL_MTX_12 * ( m0.GFL_MTX_23 * m0.GFL_MTX_31 - m0.GFL_MTX_21 * m0.GFL_MTX_33 ) + m0.GFL_MTX_13 * ( m0.GFL_MTX_21 * m0.GFL_MTX_32 - m0.GFL_MTX_22 * m0.GFL_MTX_31 ), //00
		 m0.GFL_MTX_21 * ( m0.GFL_MTX_02 * m0.GFL_MTX_33 - m0.GFL_MTX_03 * m0.GFL_MTX_32 ) + m0.GFL_MTX_22 * ( m0.GFL_MTX_03 * m0.GFL_MTX_31 - m0.GFL_MTX_01 * m0.GFL_MTX_33 ) + m0.GFL_MTX_23 * ( m0.GFL_MTX_01 * m0.GFL_MTX_32 - m0.GFL_MTX_02 * m0.GFL_MTX_31 ), //01
		 m0.GFL_MTX_31 * ( m0.GFL_MTX_02 * m0.GFL_MTX_13 - m0.GFL_MTX_03 * m0.GFL_MTX_12 ) + m0.GFL_MTX_32 * ( m0.GFL_MTX_03 * m0.GFL_MTX_11 - m0.GFL_MTX_01 * m0.GFL_MTX_13 ) + m0.GFL_MTX_33 * ( m0.GFL_MTX_01 * m0.GFL_MTX_12 - m0.GFL_MTX_02 * m0.GFL_MTX_11 ), //02
		 m0.GFL_MTX_01 * ( m0.GFL_MTX_13 * m0.GFL_MTX_22 - m0.GFL_MTX_12 * m0.GFL_MTX_23 ) + m0.GFL_MTX_02 * ( m0.GFL_MTX_11 * m0.GFL_MTX_23 - m0.GFL_MTX_13 * m0.GFL_MTX_21 ) + m0.GFL_MTX_03 * ( m0.GFL_MTX_12 * m0.GFL_MTX_21 - m0.GFL_MTX_11 * m0.GFL_MTX_22 ), //03
		 m0.GFL_MTX_12 * ( m0.GFL_MTX_20 * m0.GFL_MTX_33 - m0.GFL_MTX_23 * m0.GFL_MTX_30 ) + m0.GFL_MTX_13 * ( m0.GFL_MTX_22 * m0.GFL_MTX_30 - m0.GFL_MTX_20 * m0.GFL_MTX_32 ) + m0.GFL_MTX_10 * ( m0.GFL_MTX_23 * m0.GFL_MTX_32 - m0.GFL_MTX_22 * m0.GFL_MTX_33 ), //10
		 m0.GFL_MTX_22 * ( m0.GFL_MTX_00 * m0.GFL_MTX_33 - m0.GFL_MTX_03 * m0.GFL_MTX_30 ) + m0.GFL_MTX_23 * ( m0.GFL_MTX_02 * m0.GFL_MTX_30 - m0.GFL_MTX_00 * m0.GFL_MTX_32 ) + m0.GFL_MTX_20 * ( m0.GFL_MTX_03 * m0.GFL_MTX_32 - m0.GFL_MTX_02 * m0.GFL_MTX_33 ), //11
		 m0.GFL_MTX_32 * ( m0.GFL_MTX_00 * m0.GFL_MTX_13 - m0.GFL_MTX_03 * m0.GFL_MTX_10 ) + m0.GFL_MTX_33 * ( m0.GFL_MTX_02 * m0.GFL_MTX_10 - m0.GFL_MTX_00 * m0.GFL_MTX_12 ) + m0.GFL_MTX_30 * ( m0.GFL_MTX_03 * m0.GFL_MTX_12 - m0.GFL_MTX_02 * m0.GFL_MTX_13 ), //12
		 m0.GFL_MTX_02 * ( m0.GFL_MTX_13 * m0.GFL_MTX_20 - m0.GFL_MTX_10 * m0.GFL_MTX_23 ) + m0.GFL_MTX_03 * ( m0.GFL_MTX_10 * m0.GFL_MTX_22 - m0.GFL_MTX_12 * m0.GFL_MTX_20 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_12 * m0.GFL_MTX_23 - m0.GFL_MTX_13 * m0.GFL_MTX_22 ), //13
		 m0.GFL_MTX_13 * ( m0.GFL_MTX_20 * m0.GFL_MTX_31 - m0.GFL_MTX_21 * m0.GFL_MTX_30 ) + m0.GFL_MTX_10 * ( m0.GFL_MTX_21 * m0.GFL_MTX_33 - m0.GFL_MTX_23 * m0.GFL_MTX_31 ) + m0.GFL_MTX_11 * ( m0.GFL_MTX_23 * m0.GFL_MTX_30 - m0.GFL_MTX_20 * m0.GFL_MTX_33 ), //20
		 m0.GFL_MTX_23 * ( m0.GFL_MTX_00 * m0.GFL_MTX_31 - m0.GFL_MTX_01 * m0.GFL_MTX_30 ) + m0.GFL_MTX_20 * ( m0.GFL_MTX_01 * m0.GFL_MTX_33 - m0.GFL_MTX_03 * m0.GFL_MTX_31 ) + m0.GFL_MTX_21 * ( m0.GFL_MTX_03 * m0.GFL_MTX_30 - m0.GFL_MTX_00 * m0.GFL_MTX_33 ), //21
		 m0.GFL_MTX_33 * ( m0.GFL_MTX_00 * m0.GFL_MTX_11 - m0.GFL_MTX_01 * m0.GFL_MTX_10 ) + m0.GFL_MTX_30 * ( m0.GFL_MTX_01 * m0.GFL_MTX_13 - m0.GFL_MTX_03 * m0.GFL_MTX_11 ) + m0.GFL_MTX_31 * ( m0.GFL_MTX_03 * m0.GFL_MTX_10 - m0.GFL_MTX_00 * m0.GFL_MTX_13 ), //22
		 m0.GFL_MTX_03 * ( m0.GFL_MTX_11 * m0.GFL_MTX_20 - m0.GFL_MTX_10 * m0.GFL_MTX_21 ) + m0.GFL_MTX_00 * ( m0.GFL_MTX_13 * m0.GFL_MTX_21 - m0.GFL_MTX_11 * m0.GFL_MTX_23 ) + m0.GFL_MTX_01 * ( m0.GFL_MTX_10 * m0.GFL_MTX_23 - m0.GFL_MTX_13 * m0.GFL_MTX_20 ), //23
		 m0.GFL_MTX_10 * ( m0.GFL_MTX_22 * m0.GFL_MTX_31 - m0.GFL_MTX_21 * m0.GFL_MTX_32 ) + m0.GFL_MTX_11 * ( m0.GFL_MTX_20 * m0.GFL_MTX_32 - m0.GFL_MTX_22 * m0.GFL_MTX_30 ) + m0.GFL_MTX_12 * ( m0.GFL_MTX_21 * m0.GFL_MTX_30 - m0.GFL_MTX_20 * m0.GFL_MTX_31 ), //30
		 m0.GFL_MTX_20 * ( m0.GFL_MTX_02 * m0.GFL_MTX_31 - m0.GFL_MTX_01 * m0.GFL_MTX_32 ) + m0.GFL_MTX_21 * ( m0.GFL_MTX_00 * m0.GFL_MTX_32 - m0.GFL_MTX_02 * m0.GFL_MTX_30 ) + m0.GFL_MTX_22 * ( m0.GFL_MTX_01 * m0.GFL_MTX_30 - m0.GFL_MTX_00 * m0.GFL_MTX_31 ), //31
		 m0.GFL_MTX_30 * ( m0.GFL_MTX_02 * m0.GFL_MTX_11 - m0.GFL_MTX_01 * m0.GFL_MTX_12 ) + m0.GFL_MTX_31 * ( m0.GFL_MTX_00 * m0.GFL_MTX_12 - m0.GFL_MTX_02 * m0.GFL_MTX_10 ) + m0.GFL_MTX_32 * ( m0.GFL_MTX_01 * m0.GFL_MTX_10 - m0.GFL_MTX_00 * m0.GFL_MTX_11 ), //32
		 m0.GFL_MTX_00 * ( m0.GFL_MTX_11 * m0.GFL_MTX_22 - m0.GFL_MTX_12 * m0.GFL_MTX_21 ) + m0.GFL_MTX_01 * ( m0.GFL_MTX_12 * m0.GFL_MTX_20 - m0.GFL_MTX_10 * m0.GFL_MTX_22 ) + m0.GFL_MTX_02 * ( m0.GFL_MTX_10 * m0.GFL_MTX_21 - m0.GFL_MTX_11 * m0.GFL_MTX_20 ) //33
		 );
	Scale( det );
}
#if GFL_USE_AOS
//==============================================================================
//==============================================================================
void MatrixSoa44::FromAOS( const MatrixAos43& src )
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
	SetRow(3,Vector4::GetW1());
}
//==============================================================================
//==============================================================================
void MatrixSoa44::FromAOS( const MatrixAos44& src )
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
	GFL_MTX_30 = src.GFL_MTX_03;
	GFL_MTX_31 = src.GFL_MTX_13;
	GFL_MTX_32 = src.GFL_MTX_23;
	GFL_MTX_33 = src.GFL_MTX_33;
}
//==============================================================================
//==============================================================================
void MatrixSoa44::ToAOS( MatrixAos43* dst ) const
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
void MatrixSoa44::ToAOS( MatrixAos44* dst ) const
{
	dst->GFL_MTX_00 = GFL_MTX_00;
	dst->GFL_MTX_01 = GFL_MTX_10;
	dst->GFL_MTX_02 = GFL_MTX_20;
	dst->GFL_MTX_03 = GFL_MTX_30;
	dst->GFL_MTX_10 = GFL_MTX_01;
	dst->GFL_MTX_11 = GFL_MTX_11;
	dst->GFL_MTX_12 = GFL_MTX_21;
	dst->GFL_MTX_13 = GFL_MTX_31;
	dst->GFL_MTX_20 = GFL_MTX_02;
	dst->GFL_MTX_21 = GFL_MTX_12;
	dst->GFL_MTX_22 = GFL_MTX_22;
	dst->GFL_MTX_23 = GFL_MTX_32;
	dst->GFL_MTX_30 = GFL_MTX_03;
	dst->GFL_MTX_31 = GFL_MTX_13;
	dst->GFL_MTX_32 = GFL_MTX_23;
	dst->GFL_MTX_33 = GFL_MTX_33;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
MatrixAos44 MatrixSoa44::ToAOS( ) const
{
	MatrixAos44 mat;
	ToAOS( &mat );
	return mat;
}
#endif
// -----------------------------------------------------------------
// operator ==
// -----------------------------------------------------------------
bool MatrixSoa44::operator==( const MatrixSoa44& m0 )
{
	return GetRow( 0 ) == m0.GetRow( 0 ) &&
		GetRow( 1 ) == m0.GetRow( 1 ) &&
		GetRow( 2 ) == m0.GetRow( 2 ) &&
		GetRow( 3 ) == m0.GetRow( 3 );
}

// -----------------------------------------------------------------
// operator !=
// -----------------------------------------------------------------
bool MatrixSoa44::operator!=( const MatrixSoa44& m0 )
{
	return GetRow( 0 ) != m0.GetRow( 0 ) ||
		GetRow( 1 ) != m0.GetRow( 1 ) ||
		GetRow( 2 ) != m0.GetRow( 2 ) ||
		GetRow( 3 ) != m0.GetRow( 3 );
}
// -----------------------------------------------------------------
// Print
// -----------------------------------------------------------------
void MatrixSoa44::PrintConsoleW( void ) const
{
	Debug::PrintConsole( "%ff, %ff, %ff, %ff, \n",
						 GFL_MTX_30, GFL_MTX_31, GFL_MTX_32, GFL_MTX_33 );
}
}
}

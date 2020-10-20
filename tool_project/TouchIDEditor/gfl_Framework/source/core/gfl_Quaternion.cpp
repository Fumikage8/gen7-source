// =================================================================
// Quaternion
// =================================================================

namespace gfl {
namespace core {

// -----------------------------------------------------------------
// Mul
// -----------------------------------------------------------------
void Quaternion::Mul( const QuaternionNative& t, const QuaternionNative& s )
{
	Set( t.GFL_VECTOR_W * s.GFL_VECTOR_X + t.GFL_VECTOR_X * s.GFL_VECTOR_W + t.GFL_VECTOR_Y * s.GFL_VECTOR_Z - t.GFL_VECTOR_Z * s.GFL_VECTOR_Y,
		 t.GFL_VECTOR_W * s.GFL_VECTOR_Y + t.GFL_VECTOR_Y * s.GFL_VECTOR_W + t.GFL_VECTOR_Z * s.GFL_VECTOR_X - t.GFL_VECTOR_X * s.GFL_VECTOR_Z,
		 t.GFL_VECTOR_W * s.GFL_VECTOR_Z + t.GFL_VECTOR_Z * s.GFL_VECTOR_W + t.GFL_VECTOR_X * s.GFL_VECTOR_Y - t.GFL_VECTOR_Y * s.GFL_VECTOR_X,
		 t.GFL_VECTOR_W * s.GFL_VECTOR_W - t.GFL_VECTOR_X * s.GFL_VECTOR_X - t.GFL_VECTOR_Y * s.GFL_VECTOR_Y - t.GFL_VECTOR_Z * s.GFL_VECTOR_Z );
}

// -----------------------------------------------------------------
// Inverse
// -----------------------------------------------------------------
void Quaternion::Inverse( const Quaternion& t )
{
	f32 norm = t.Magnitude2Recip( );
	f32 norm_neg = -norm;
	Set( t.GFL_VECTOR_X * norm_neg,
		 t.GFL_VECTOR_Y * norm_neg,
		 t.GFL_VECTOR_Z * norm_neg,
		 t.GFL_VECTOR_W * norm );
}

// -----------------------------------------------------------------
// Slerp
// -----------------------------------------------------------------
void Quaternion::Slerp( const Quaternion& t, const Quaternion& s, const f32 rate )
{
	//	GFL_MATH_ASSERT( t.Magnitude( ) - 1.0f < 0.001f, "t is not unit quarternion.\n" );
	//	GFL_MATH_ASSERT( s.Magnitude( ) - 1.0f < 0.001f, "s is not unit quarternion.\n" );
#if 0 // スケールがかかっている場合
	Quaternion t, s;
	const f32 t_scl = t.NormalizeRet( t_org );
	const f32 s_scl = s.NormalizeRet( s_org );
#endif
#if 1 // 
	Vector4 tpl;
	f32 dot = t.Dot( s );
	if( dot < 0.0f ){
		tpl.Neg( ( const Vector4& )t );
		dot = -dot;
	} else {
		tpl.Set( ( const Vector4& )t );
	}

	if( dot < 1.0f - Math::EPSILON ){
		f32 ang = Math::FACos( dot );
		f32 sin_recip = Math::FRecip( Math::FSin( ang ) );
		tpl.Mul( Math::FSin( ( 1.0f - rate ) * ang ) * sin_recip );
		Vector4 r;
		r.Mul( ( const Vector4& )s, Math::FSin( rate * ang ) * sin_recip );
		tpl.Add( r );
		Set( ( const Quaternion& )tpl );
	} else {
		Set( t );
	}
#elif 0 // 近似 単純すぎて使えない
	( ( gfl::core::VectorNative4* )this )->Lerp( ( const gfl::core::VectorNative4& )t, ( const gfl::core::VectorNative4& )s, rate );
	Normalize( );
#else // 
	const f32 adotb = t.x * s.x + t.y * s.y + t.z * s.z + t.w * s.w;
	const f32 mRecipOnePlusAdotB = 1.0f / ( 1.0f + adotb );
	const f32 mC1 = 1.570994357f + ( .5642929859f + ( -.1783657717f + .4319949352e-1f * adotb ) * adotb ) * adotb;
	const f32 mC3 = -.6461396382f + ( .5945657936f + ( .8610323953e-1f - .3465122928e-1f * adotb ) * adotb ) * adotb;
	const f32 mC5 = .7949823521e-1f + ( -.1730436931f + ( .1079279599f - .1439397801e-1f * adotb ) * adotb ) * adotb;
	const f32 mC7 = -.4354102836e-2f + ( .1418962736e-1f + ( -.1567189691e-1f + .5848706227e-2f * adotb ) * adotb ) * adotb;
	// 以上は初期化　
	const f32 T = 1.0f - rate, t2 = rate * rate, T2 = T * T;
	const f32 alpha = ( mC1 + ( mC3 + ( mC5 + mC7 * T2 ) * T2 ) * T2 ) * T * mRecipOnePlusAdotB;
	const f32 beta = ( mC1 + ( mC3 + ( mC5 + mC7 * t2 ) * t2 ) * t2 ) * rate * mRecipOnePlusAdotB;
	Set( alpha * t.x + beta * s.x, alpha * t.y + beta * s.y, alpha * t.z + beta * s.z, alpha * t.w + beta * s.w );
#endif
#if 0 // 
	if( !Math::IsSimilar( t_scl, 1.0f, 0.01f ) || !Math::IsSimilar( s_scl, 1.0f, 0.01f ) ){
		Scale( t_scl * rate + s_scl * ( 1.0f - rate ) );
	}
#endif
}

// -----------------------------------------------------------------
// Radian Y To Quaternion
// -----------------------------------------------------------------
void Quaternion::RadianYToQuaternion( const f32 yang )
{
	f32 c, s;

	Math::FSinCos( &s, &c, yang );
	SetX( 0.0f );
	c *= 2.0f;
	SetZ( 0.0f );

	const f32 tr = 1.0f + c;
	f32 ww;

	if( tr >= 0.0f ){
		ww = Math::FSqrt( tr + 1.0f );
		SetW( ww * 0.5f );
		SetY( s / ww );
	} else {
		ww = Math::FSqrt( 2.0f - c );
		SetY( ww * 0.5f );
		SetW( s / ww );
	}
}

// -----------------------------------------------------------------
// Radian To Quaternion
// -----------------------------------------------------------------
void Quaternion::RadianToQuaternion( const f32 x, const f32 y, const f32 z )
{
	Matrix mat;
	mat.SetRotateZYX( x, y, z );
	mat.MatrixToQuaternion( this );
}

// -----------------------------------------------------------------
// Radian To Quaternion
// -----------------------------------------------------------------
void Quaternion::RadianYXZToQuaternion( const f32 x, const f32 y, const f32 z )
{
	Matrix mat;
	mat.SetRotateY( y );
	mat.RotateX( x );
	mat.RotateZ( z );
	mat.MatrixToQuaternion( this );
}

// -----------------------------------------------------------------
// Quaternion To Radian
// -----------------------------------------------------------------
void Quaternion::QuaternionToRadian( f32* x_ang, f32* y_ang, f32* z_ang ) const
{
#if 0 //
	Matrix44AOS mat;
	mat.QuaternionToMatrix( *this );
	mat.GetRadian( x, y, z );
#else
	Quaternion qt;
	const f32 scl = qt.NormalizeRet( *this );

	const f32 x = qt.GetX( );
	const f32 y = qt.GetY( );
	const f32 z = qt.GetZ( );
	const f32 w = qt.GetW( );

	const f32 xz = x * z;
	const f32 wy = w * y;

	const f32 xx = x * x;
	const f32 xy = x * y;
	const f32 zz = z * z;
	const f32 wz = w * z;

	const f32 sy = 2.0f * ( xz - wy );
	if( 1.0f - gfl::core::Math::EPSILON <= sy ){
		*x_ang = Math::FATan2( -2.0f * ( xy - wz ), 1.0f - 2.0f * ( xx + zz ) );
		*y_ang = -Math::HALF_PI;
		*z_ang = 0.0f;
	} else if( sy <= -1.0f+ gfl::core::Math::EPSILON ){
		*x_ang = Math::FATan2( 2.0f * ( xy - wz ), 1.0f - 2.0f * ( xx + zz ) );
		*y_ang = Math::HALF_PI;
		*z_ang = 0.0f;
	} else {
		const f32 yy = y * y;
		*x_ang = Math::FATan2( 2.0f * ( y * z + w * x ), 1.0f - 2.0f * ( xx + yy ) );
		*y_ang = Math::FASin( -sy / scl );
		*z_ang = Math::FATan2( 2.0f * ( xy + wz ), 1.0f - 2.0f * ( yy + zz ) );
	}
#endif
}

// -----------------------------------------------------------------
// Quaternion To Radian
// -----------------------------------------------------------------
void Quaternion::QuaternionToRadianYXZ( f32* x, f32* y, f32* z ) const
{
	Matrix mat;
	mat.QuaternionToMatrix( *this );
	mat.GetRadianYXZ( x, y, z );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
f32 Quaternion::QuaternionToRadianY( void ) const
{
	const f32 gx = GetX( );
	const f32 gy = GetY( );
	const f32 gz = GetZ( );
	const f32 gw = GetW( );

	return Math::FATan2( 2.0f * ( gx * gz + gw * gy ), 1.0f - 2.0f * ( gx * gx + gy * gy ) );
}
void Quaternion::SetUnit( void )
{
	Set( reinterpret_cast < const Quaternion& >( Vector4::GetW1( ) ) );
}
}
}

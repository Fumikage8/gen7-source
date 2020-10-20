// =============================================================================
// Math
// =============================================================================
// -----------------------------------------------------------------------------
// Static valuable
// -----------------------------------------------------------------------------
const f32 gfl::core::Math::F32_ZERO = 0.0f;
const f32 gfl::core::Math::F32_MIN = -F32_MAX;
const f32 gfl::core::Math::F32_MAX = 1.0E+38f;
const f32 gfl::core::Math::EPSILON = 1.0e-6f;
const f32 gfl::core::Math::ONE_PI = 3.1415926535f;
const f32 gfl::core::Math::QUARTER_PI = ONE_PI * 0.25f;
const f32 gfl::core::Math::HALF_PI = ONE_PI * 0.5f;
const f32 gfl::core::Math::ONE_HALF_PI = ONE_PI * 1.5f;
const f32 gfl::core::Math::TWO_PI = ONE_PI * 2.0f;
const f32 gfl::core::Math::ROOT_2 = 1.41421356f;
const f32 gfl::core::Math::ROOT_3 = 1.7320508f;

const gfl::core::TupleStr3<f32> gfl::core::Tuple3f::sZero = { 0.0f, 0.0f, 0.0f };
const gfl::core::TupleStr4<f32> gfl::core::Tuple4f::sZero = { 0.0f, 0.0f, 0.0f, 0.0f };
const gfl::core::TupleStr4<f32> gfl::core::Tuple4f::sX1 = { 1.0f, 0.0f, 0.0f, 0.0f };
const gfl::core::TupleStr4<f32> gfl::core::Tuple4f::sY1 = { 0.0f, 1.0f, 0.0f, 0.0f };
const gfl::core::TupleStr4<f32> gfl::core::Tuple4f::sZ1 = { 0.0f, 0.0f, 1.0f, 0.0f };
const gfl::core::TupleStr4<f32> gfl::core::Tuple4f::sW1 = { 0.0f, 0.0f, 0.0f, 1.0f };

// -----------------------------------------------------------------------------
// Function
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------
// GetXZ by RotateY
// -----------------------------------------------------------------
void gfl::core::Math::GetXZRotateY( f32* xpos, f32* zpos, const f32 yang )
{
	f32 x = *xpos;
	f32 z = *zpos;

	f32 s, c;
	FSinCos( &s, &c, yang );

	*xpos = GetX_RotateY_SinCosXZ( s, c, x, z );
	*zpos = GetZ_RotateY_SinCosXZ( s, c, x, z );
}

// -----------------------------------------------------------------
// GetXZ by RotateY
// -----------------------------------------------------------------
void gfl::core::Math::GetXZRotateY( f32* xtag, f32* ztag, const f32 yang, const f32 xorg, const f32 zorg )
{
	f32 s, c;
	FSinCos( &s, &c, yang );

	*xtag = GetX_RotateY_SinCosXZ( s, c, xorg, zorg );
	*ztag = GetZ_RotateY_SinCosXZ( s, c, xorg, zorg );
}
// -----------------------------------------------------------------
// AddXZ by RotateY
// -----------------------------------------------------------------
void gfl::core::Math::AddXZRotateY( f32* xtag, f32* ztag, const f32 yang, const f32 xorg, const f32 zorg )
{
	f32 x, z;
	GetXZRotateY( &x, &z, yang, xorg, zorg );
	*xtag += x;
	*ztag += z;
}

// -----------------------------------------------------------------
// GetXZ by RotateY
// -----------------------------------------------------------------
void gfl::core::Math::GetXZRotateY_X( f32* xtag, f32* ztag, const f32 yang, const f32 xorg )
{
	f32 s, c;
	FSinCos( &s, &c, yang );

	*xtag = c * xorg;
	*ztag = -s * xorg;
}
// -----------------------------------------------------------------
// AddXZ by RotateY
// -----------------------------------------------------------------
void gfl::core::Math::AddXZRotateY_X( f32* xtag, f32* ztag, const f32 yang, const f32 xorg )
{
	f32 x, z;
	GetXZRotateY_X( &x, &z, yang, xorg );
	*xtag += x;
	*ztag += z;
}


// -----------------------------------------------------------------
// GetXZ by RotateY
// -----------------------------------------------------------------
void gfl::core::Math::GetXZRotateY_Z( f32* xtag, f32* ztag, const f32 yang, const f32 zorg )
{
	f32 s, c;
	FSinCos( &s, &c, yang );

	*xtag = s * zorg;
	*ztag = c * zorg;
}

// -----------------------------------------------------------------
// AddXZ by RotateY
// -----------------------------------------------------------------
void gfl::core::Math::AddXZRotateY_Z( f32* xtag, f32* ztag, const f32 yang, const f32 zorg )
{
	f32 x, z;
	GetXZRotateY_Z( &x, &z, yang, zorg );
	*xtag += x;
	*ztag += z;
}

// -----------------------------------------------------------------
// OptimizeRad
// -----------------------------------------------------------------
f32 gfl::core::Math::OptimizeRadian( f32 rad )
{
	rad = fmod( rad, TWO_PI ); // -TWO_PI < rad < TWO_PI
	if( ONE_PI < rad ){ // -PI < rad < PI
		rad -= TWO_PI;
	} else if( rad < -ONE_PI ){
		rad += TWO_PI;
	}
	return AlmostZeroToZero( rad );
}

// -----------------------------------------------------------------
// OptimizeRad
// -----------------------------------------------------------------
void gfl::core::Math::OptimizeRadian( f32* vxp, f32* vyp, f32* vzp )
{
	f32 vx = *vxp;
	f32 vy = *vyp;
	f32 vz = *vzp;

	// 絶対値が小さいほうの角度に設定
	f32 tx = ( vx >= 0.0f ) ? ( vx - ONE_PI ) : ( vx + ONE_PI );
	f32 ty = ( vy >= 0.0f ) ? ( ONE_PI - vy ) : ( -ONE_PI - vy );
	f32 tz = ( vz >= 0.0f ) ? ( vz - ONE_PI ) : ( vz + ONE_PI );

	if( ( gfl::core::Math::FAbs( vx ) + gfl::core::Math::FAbs( vy ) + gfl::core::Math::FAbs( vz ) )
		> ( gfl::core::Math::FAbs( tx ) + gfl::core::Math::FAbs( ty ) + gfl::core::Math::FAbs( tz ) ) ){
		*vxp = tx, *vyp = ty, *vzp = tz;
	}
}

// -----------------------------------------------------------------
// SmoothRadian
// -----------------------------------------------------------------
void gfl::core::Math::SmoothRadian( f32* vxp, f32* vyp, f32* vzp, f32 bx, f32 by, f32 bz )
{
#define ROT_SMOOTH(X,X_1) ((X) + ( -gfl::core::Math::TWO_PI*(f32)(((s32)ceil(((X)-(X_1))/ONE_PI)) >> 1)))
	f32 vx = *vxp;
	f32 vy = *vyp;
	f32 vz = *vzp;

	// 前のデータはOptimizeされていない状態に変更されているかもしれないので
	f32 obx = gfl::core::Math::OptimizeRadian( bx );
	f32 oby = gfl::core::Math::OptimizeRadian( by );
	f32 obz = gfl::core::Math::OptimizeRadian( bz );

	f32 tx = ( vx >= 0.0f ) ? ( vx - ONE_PI ) : ( vx + ONE_PI );
	f32 ty = ( vy >= 0.0f ) ? ( ONE_PI - vy ) : ( -ONE_PI - vy );
	f32 tz = ( vz >= 0.0f ) ? ( vz - ONE_PI ) : ( vz + ONE_PI );

	if( ( gfl::core::Math::FAbs( vx - obx ) + gfl::core::Math::FAbs( vy - oby ) + gfl::core::Math::FAbs( vz - obz ) )
		> ( gfl::core::Math::FAbs( tx - obx ) + gfl::core::Math::FAbs( ty - oby ) + gfl::core::Math::FAbs( tz - obz ) ) ){
		vx = tx, vy = ty, vz = tz;
	}

	*vxp = ROT_SMOOTH( vx, bx );
	*vyp = ROT_SMOOTH( vy, by );
	*vzp = ROT_SMOOTH( vz, bz );
}

// -----------------------------------------------------------------
// Add Radian
// -----------------------------------------------------------------
f32 gfl::core::Math::AddRadian( const f32 rad, const f32 add )
{
	register f32 ans = rad + add;
	if( ONE_PI < ans ){
		return ans - TWO_PI;
	} else if( ans < -ONE_PI ){
		return ans + TWO_PI;
	} else {
		return ans;
	}
}

// -----------------------------------------------------------------
// Round Radian Step
// -----------------------------------------------------------------
u32 gfl::core::Math::RoundRadianStep( f32 val_f, const u32 step )
{
	const f32 seg = ONE_PI / step;
	val_f = ( 0.0f <= val_f ) ? ( val_f + seg ) : ( val_f - seg );
	return static_cast < u32 >( val_f / ( seg * 2.0f ) ) & ( step - 1 );
}

// -----------------------------------------------------------------
// Round Float
// -----------------------------------------------------------------
f32 gfl::core::Math::RoundFloat( f32 val, const f32 step )
{
	s32 mul = static_cast < s32 >( val / step );
	f32 edge = static_cast < f32 >( mul ) * step;
	if( 0.0f <= val ){
		return(val - edge < 0.5f * step ) ? edge : ( edge + step );
	} else {
		return(edge - val < 0.5f * step ) ? edge : ( edge - step );
	}
}

// -----------------------------------------------------------------
// Round Off
// -----------------------------------------------------------------
f32 gfl::core::Math::RoundOff( f32 val, const f32 range )
{
	f32 int_val, float_val;
	float_val = modff( val, &int_val );
	if( float_val < range ){
		val = static_cast < f32 >( int_val );
	} else if( 1.0f - range <= float_val ){
		val = static_cast < f32 >( int_val + 1 );
	}
	return val;
}


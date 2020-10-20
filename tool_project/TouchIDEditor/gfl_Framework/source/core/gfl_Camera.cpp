// -----------------------------------------------------------------------------
// コンストラクタ
// -----------------------------------------------------------------------------
gfl::core::Camera::Camera( void ) : _NearClip( GFL_DEFAULT_NEAR_CLIP ), _FarClip( GFL_DEFAULT_FAR_CLIP ), _Perspective( GFL_DEFAULT_PERSPECTIVE ), _Mode( 0 )
{
	_ControlRotationX = gfl::core::Math::DegreeToRadian( -10.0f );
	_ControlRotationY = gfl::core::Math::DegreeToRadian( 30.0f );
	_ControlDistance = METER( 10.0f );
	_Trans.SetW(1.0f);

	SetInterest( gfl::core::Vector::GetZero( ) );
}

// ---------------------------------------------------------------------------
// Set trans by interest
// ---------------------------------------------------------------------------
void gfl::core::Camera::SetTransByInterest( void )
{
	gfl::core::Matrix mat;
	mat.SetUnit( );
	mat.SetTrans( _Interest );
	mat.RotateY( _ControlRotationY );
	mat.RotateX( _ControlRotationX );
	mat.TransZ( &_Trans, _ControlDistance );
}

// ---------------------------------------------------------------------------
// set rotation by interest
// ---------------------------------------------------------------------------
void gfl::core::Camera::SetRotationByInterest( void )
{
	gfl::core::Vector v;

	v.Sub( _Trans, _Interest );
	_Rotation.SetY( gfl::core::Math::FATan2( v.GetX( ), v.GetZ( ) ) );
	_Rotation.SetX( gfl::core::Math::FATan2( -v.GetY( ), v.MagnitudeXZ( ) ) );
	_Rotation.SetZ( 0.0f );
}

// ---------------------------------------------------------------------------
// set matrix by position
// ---------------------------------------------------------------------------
void gfl::core::Camera::SetMatrixByPosition( f32 scale /* = 1.0f */ )
{
	_Matrix.SetUnit( );
    if ( scale != 1.0f )
    {
      _Matrix.Scale( scale ); // 11.11.22 funakoshi スケールを乗じることでInterest採用、かつ平行投影時に望むような画が得られる
    }
	_Matrix.RotateX( -_Rotation.GetX( ) );
	_Matrix.RotateY( -_Rotation.GetY( ) );
	_Matrix.RotateZ( -_Rotation.GetZ( ) );
	_Matrix.Trans( -_Trans.GetX( ), -_Trans.GetY( ), -_Trans.GetZ( ) );
}

// ---------------------------------------------------------------------------
// Set gfl::core::Camera
// ---------------------------------------------------------------------------
void gfl::core::Camera::UpdateMatrixByInterest( f32 scale /* = 1.0f */ )
{
	SetTransByInterest( );
	SetRotationByInterest( );
	SetMatrixByPosition( scale );
}

// ---------------------------------------------------------------------------
// Set gfl::core::Camera
// ---------------------------------------------------------------------------
void gfl::core::Camera::SetInterest( const gfl::core::Vector& pos )
{
	_Interest.Set( pos );
	UpdateMatrixByInterest( );
}

// ---------------------------------------------------------------------------
// add gfl::core::Camera distance
// ---------------------------------------------------------------------------
void gfl::core::Camera::AddDistance( const f32 dist )
{
	f32 x = _ControlDistance + dist;
	f32 cLimit = _FarClip;
	if( cLimit < x ){
		x = cLimit;
	} else {
		cLimit = _NearClip;
		if( x < cLimit ){
			x = cLimit;
		}
	}
	_ControlDistance = x;
}

// ---------------------------------------------------------------------------
// add gfl::core::Camera rotation
// ---------------------------------------------------------------------------
void gfl::core::Camera::AddRotationX( const f32 ang )
{
	f32 x = gfl::core::Math::AddRadian( _ControlRotationX, ang );
	f32 cLimit = gfl::core::Math::HALF_PI - gfl::core::Math::DegreeToRadian( 5 );
	if( cLimit < x ){
		x = cLimit;
	} else {
		cLimit = -cLimit;
		if( x < cLimit ){
			x = cLimit;
		}
	}
	_ControlRotationX = x;
}

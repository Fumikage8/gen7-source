#ifndef GFL_PLATFORM_3DS

//==============================================================================
/**
 * @file	gfl_MatrixSoaNative34.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/11/01, 10:16
 */
// =============================================================================

namespace gfl {
namespace core {
MatrixSoaNative33::MatrixSoaNative33( const MatrixSoaNative34& mat )
{ // 関数が一個しかないので間借り
	GetRow( 0 ) = mat.GetRow( 0 );
	GetRow( 1 ) = mat.GetRow( 1 );
	GetRow( 2 ) = mat.GetRow( 2 );
}
MatrixSoaNative34 & MatrixSoaNative34::operator+=( const MatrixSoaNative34& t )
{
	GetRow( 0 ).Add( t.GetRow( 0 ) );
	GetRow( 1 ).Add( t.GetRow( 1 ) );
	GetRow( 2 ).Add( t.GetRow( 2 ) );
	return *this;
}
MatrixSoaNative34 & MatrixSoaNative34::operator-=( const MatrixSoaNative34& t )
{
	GetRow( 0 ).Sub( t.GetRow( 0 ) );
	GetRow( 1 ).Sub( t.GetRow( 1 ) );
	GetRow( 2 ).Sub( t.GetRow( 2 ) );
	return *this;
}
MatrixSoaNative34 & MatrixSoaNative34::operator*=( const f32 t )
{
	GetRow( 0 ).Mul( t );
	GetRow( 1 ).Mul( t );
	GetRow( 2 ).Mul( t );
	return *this;
}
MatrixSoaNative34 & MatrixSoaNative34::operator*=( const MatrixSoaNative34& t )
{
	GetRow( 0 ).Mul( t.GetRow( 0 ) );
	GetRow( 1 ).Mul( t.GetRow( 1 ) );
	GetRow( 2 ).Mul( t.GetRow( 2 ) );
	return *this;
}
MatrixSoaNative34 & MatrixSoaNative34::operator/=( const f32 t )
{
	GetRow( 0 ).Div( t );
	GetRow( 1 ).Div( t );
	GetRow( 2 ).Div( t );
	return *this;
}
MatrixSoaNative34& MatrixSoaNative34::SetupRotateXyz( const VectorNative3& rot )
{
	MatrixSoa34* mat = reinterpret_cast < MatrixSoa34* >( this );
	mat->SetTransUnit( );
	mat->SetRotateZYX( rot );
	return *this;
}
MatrixSoaNative34& MatrixSoaNative34::SetupRotate( const VectorNative3& rot, f32 ang )
{
	MatrixSoa34* mat = reinterpret_cast < MatrixSoa34* >( this );
	mat->SetTransUnit( );
	if( rot.GFL_VECTOR_X == 1.0f ){
		mat->SetRotateX( ang );
	} else if( rot.GFL_VECTOR_Y == 1.0f ){
		mat->SetRotateY( ang );
	} else if( rot.GFL_VECTOR_Z == 1.0f ){
		mat->SetRotateZ( ang );
	} else {
		mat->SetUnit33( );
	}
	return *this;
}
MatrixSoaNative34& MatrixSoaNative34::SetupRotate( const QuaternionNative& qua )
{
	MatrixSoa34* mat = reinterpret_cast < MatrixSoa34* >( this );
	mat->QuaternionToMatrix( qua );
	return *this;
}
MatrixSoaNative34& MatrixSoaNative34::SetupLookAt(const VectorNative3& cam, const VectorNative3& up, const VectorNative3& tag)
{
	Vector3 d, s, r;

	d.Sub( tag, cam );
	d.Normalize( );
	s.Cross( d, up );
	s.Normalize( );
	r.Cross( s, d );

	GetRow(0).Set(s.GFL_VECTOR_X, s.GFL_VECTOR_Z, s.GFL_VECTOR_Z, -cam.Dot( s ));
	GetRow(1).Set(r.GFL_VECTOR_X, r.GFL_VECTOR_Z, r.GFL_VECTOR_Z, -cam.Dot( r ));
	GetRow(2).Set(-d.GFL_VECTOR_X, -d.GFL_VECTOR_Z, -d.GFL_VECTOR_Z, cam.Dot( d ));
	return *this;
}
MatrixSoaNative34& MatrixSoaNative34::SetupCameraRotate( const VectorNative3& pos, const VectorNative3& deg )
{
	MatrixSoa34* mat = reinterpret_cast < MatrixSoa34* >( this );
	mat->SetTrans( -pos );
	mat->SetRotateZ( gfl::core::Math::DegreeToRadian( -deg.GFL_VECTOR_Z ) );
	mat->RotateY( gfl::core::Math::DegreeToRadian( -deg.GFL_VECTOR_Y ) );
	mat->RotateX( gfl::core::Math::DegreeToRadian( -deg.GFL_VECTOR_X ) );
	return *this;
}

}
}
#endif

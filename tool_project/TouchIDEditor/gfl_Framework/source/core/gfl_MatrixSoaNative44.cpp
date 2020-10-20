#ifndef GFL_PLATFORM_3DS
//==============================================================================
/**
 * @file	gfl_MatrixSoaNative44.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/11/01, 10:17
 */
// =============================================================================

namespace gfl {
namespace core {

/**
 *   角度成分の転置行列を設定
 * @param  無し
 * @return 無し
 */
MatrixSoaNative44& MatrixSoaNative44::Transpose( void )
{
	MatrixSoa44* mat = reinterpret_cast < MatrixSoa44* >( this );
	mat->Transpose33( );
	Vector4 col = GetColumn( 3 );
	SetColumn( 3, GetRow( 3 ) );
	GetRow( 3 ) = col;
	return *this;
}
MatrixSoaNative44 & MatrixSoaNative44::operator+=(const MatrixSoaNative44& t) {
	GetRow(0).Add( t.GetRow(0) );
	GetRow(1).Add( t.GetRow(1) );
	GetRow(2).Add( t.GetRow(2) );
	GetRow(3).Add( t.GetRow(3) );
	return *this;
}

MatrixSoaNative44 & MatrixSoaNative44::operator-=(const MatrixSoaNative44& t) {
	GetRow(0).Sub( t.GetRow(0) );
	GetRow(1).Sub( t.GetRow(1) );
	GetRow(2).Sub( t.GetRow(2) );
	GetRow(3).Sub( t.GetRow(3) );
	return *this;
}

MatrixSoaNative44 & MatrixSoaNative44::operator*=(const f32 t) {
	GetRow(0).Mul( t );
	GetRow(1).Mul( t );
	GetRow(2).Mul( t );
	GetRow(3).Mul( t );
	return *this;
}

MatrixSoaNative44 & MatrixSoaNative44::operator*=(const MatrixSoaNative44& t) {
	GetRow(0).Mul( t.GetRow(0) );
	GetRow(1).Mul( t.GetRow(1) );
	GetRow(2).Mul( t.GetRow(2) );
	GetRow(3).Mul( t.GetRow(3) );
	return *this;
}

MatrixSoaNative44 & MatrixSoaNative44::operator/=(const f32 t) {
	GetRow(0).Div( t );
	GetRow(1).Div( t );
	GetRow(2).Div( t );
	GetRow(3).Div( t );
	return *this;
}

/**
 * 回転行列を設定
 * @return 
 */

MatrixSoaNative44& MatrixSoaNative44::SetupRotateXyz( const VectorNative3& rot )
{
	MatrixSoa44* mat = reinterpret_cast < MatrixSoa44* >( this );
	mat->SetTransUnit( );
	mat->SetRotateZYX( rot );
	return *this;
}
/**
 * 回転行列を設定
 * @return 
 */

MatrixSoaNative44& MatrixSoaNative44::SetupRotate( const VectorNative3& rot, f32 ang )
{
	MatrixSoa44* mat = reinterpret_cast < MatrixSoa44* >( this );
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
/**
 * 回転行列を設定
 * @return 
 */

MatrixSoaNative44& MatrixSoaNative44::SetupRotate( const QuaternionNative& qua )
{
	MatrixSoa44* mat = reinterpret_cast < MatrixSoa44* >( this );
	mat->QuaternionToMatrix( qua );
	return *this;
}
}
}
#endif // GFL_PLATFORM_3DS


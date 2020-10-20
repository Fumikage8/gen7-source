#include <clr/include/math/gfl2_Matrix_CLR.h>

namespace gfl2 { namespace clr { namespace math {

Matrix::Matrix()
{
	m_pNativeMat = new gfl2::math::Matrix44();
}

Matrix::Matrix( Matrix^  rhs )
{
	m_pNativeMat = new gfl2::math::Matrix44( *(rhs->GetNative()) );
}

Matrix::Matrix( const gfl2::math::Matrix44& rhs )
{
	m_pNativeMat = new gfl2::math::Matrix44( rhs );
}

Matrix::Matrix( Vector^ col0_, Vector^ col1_, Vector^ col2_, Vector^ col3_ )
{
	m_pNativeMat = new gfl2::math::Matrix44( *(col0_->GetNative()), *(col1_->GetNative()), *(col2_->GetNative()), *(col3_->GetNative()) );
}

Matrix::~Matrix()
{
	this->!Matrix();
}

Matrix::!Matrix()
{
	if ( m_pNativeMat ){
		delete m_pNativeMat;
	}
}

Matrix^ Matrix::operator =( Matrix^ rhs )
{
	*this->GetNative() = *rhs->GetNative();
	return this;
}

//! 要素の設定
Matrix^ Matrix::SetElem( int col, int row, float val )
{
	(*m_pNativeMat)[col][row] = val;
	return this;
}

//! 要素の取得
float Matrix::GetElem( int col, int row )
{
	return (*m_pNativeMat)[col][row];
}

//! 加算
Matrix^ Matrix::operator +( Matrix^ me, Matrix^ rhs )
{
	return gcnew Matrix( *me->GetNative() + *rhs->GetNative() );
}

//! 減算
Matrix^ Matrix::operator -( Matrix^ me, Matrix^ rhs )
{
	return gcnew Matrix( *me->GetNative() - *rhs->GetNative() );
}

//! negate
Matrix^ Matrix::operator -( Matrix^ me )
{
    return gcnew Matrix( -(*me->GetNative()) );
}

//! スカラ乗算
Matrix^ Matrix::operator *( Matrix^ me, float rhs )
{
	return  gcnew Matrix ( *me->GetNative() * rhs );
}

//! スカラ乗算代入
Matrix^ Matrix::operator *=( Matrix^ me, float rhs )
{
	*me->GetNative() *= rhs;
    return (me);
}

//! ベクトルとの乗算
Vector^ Matrix::operator *( Matrix^ me, Vector^ rhs )
{
	return gcnew Vector( *me->GetNative() * *rhs->GetNative() );
}

//! マトリックスとの乗算
Matrix^ Matrix::operator *( Matrix^ me, Matrix^ rhs )
{
	return gcnew Matrix( *me->GetNative() * *rhs->GetNative() );
}

//! 加算代入
Matrix^ Matrix::operator +=( Matrix^ me, Matrix^  rhs )
{
	*me->GetNative() += *rhs->GetNative();
	return me;
}

//! 減算代入
Matrix^ Matrix::operator -=( Matrix^ me, Matrix^  rhs )
{
	*me->GetNative() -= *rhs->GetNative();
	return me;
}

//! 乗算代入
Matrix^ Matrix::operator *=( Matrix^ me, Matrix^  rhs )
{
	*me->GetNative() *= *rhs->GetNative();
	return me;
}

//! マトリクスの転置
Matrix^ Matrix::Transpose( void )
{
	return gcnew Matrix( this->GetNative()->Transpose() );
}

//! 逆マトリクス
Matrix^ Matrix::Inverse( void )
{
	return gcnew Matrix( this->GetNative()->Inverse() );
}

//! 逆マトリクス( アフィン変換マトリクス用  高速版 )
Matrix^ Matrix::AffineInverse( void )
{
	return gcnew Matrix( this->GetNative()->AffineInverse() );
}

//=============================================================================
//! 単位マトリクス
Matrix^ Matrix::GetIdentity( )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetIdentity() );
}

//! ゼロマトリクス
Matrix^ Matrix::GetZero()
{
	return gcnew Matrix( gfl2::math::Matrix44::GetZero() );
}

//! X軸回転マトリクス
Matrix^ Matrix::GetRotationX( float radians )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetRotationX( radians ) );
}

//! Y軸回転マトリクス
Matrix^ Matrix::GetRotationY( float radians )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetRotationY(radians) );
}

//! Z軸回転マトリクス
Matrix^ Matrix::GetRotationZ( float radians )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetRotationZ(radians) );
}

//! Z * Y * X 回転マトリクス
Matrix^ Matrix::GetRotationZYX( float radiansZ, float radiansY, float radiansX )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetRotationZYX(radiansZ, radiansY, radiansX) );
}

//! スケーリングマトリクス
Matrix^ Matrix::GetScaling( Vector^ scale )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetScaling(scale->GetNativeVector3()) );
}

//! 平行移動マトリクス
Matrix^ Matrix::GetTranslation( Vector^ translation )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetTranslation(translation->GetNativeVector3()) );
}
//! OpenGL互換 LookAt
Matrix^ Matrix::GetLookAt( Vector^ eye, Vector^ aim, Vector^ up )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetLookAt( eye->GetNativeVector3(), aim->GetNativeVector3(), up->GetNativeVector3() ) );
}

//! Perspective
Matrix^ Matrix::GetPerspective( float fovy, float aspect, float zNear, float zFar )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetPerspective( fovy, aspect, zNear, zFar ) );
}

//! Frustum
Matrix^ Matrix::GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetFrustum( left, right, bottom, top, zNear, zFar ) );
}

//! ortho
Matrix^ Matrix::GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar )
{
	return gcnew Matrix( gfl2::math::Matrix44::GetOrthographic( left, right, bottom, top, zNear, zFar ) );
}

}}}
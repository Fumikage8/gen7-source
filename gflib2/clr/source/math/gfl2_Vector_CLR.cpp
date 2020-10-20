#include <clr/include/math/gfl2_Vector_CLR.h>

namespace gfl2 { namespace clr { namespace math {

//! @brief デフォルトコンストラクタ
Vector::Vector()
{
	m_pNativeVec = new gfl2::math::Vector4();
}

Vector::Vector( Vector^ other )
{
	m_pNativeVec = new gfl2::math::Vector4( *(other->m_pNativeVec) );
}

Vector::Vector( const gfl2::math::Vector4& other )
{
	m_pNativeVec = new gfl2::math::Vector4( other );
}

Vector::Vector( const gfl2::math::Vector& other )
{
	m_pNativeVec = new gfl2::math::Vector4( other.x, other.y, other.z, other.w );
}

Vector::Vector( float x, float y, float z )
{
	m_pNativeVec = new gfl2::math::Vector4( x, y, z, 1.0f );
}

//! @brief float×４からベクトルを作成
Vector::Vector( float x, float y, float z, float w )
{
	m_pNativeVec = new gfl2::math::Vector4( x, y, z, w );
}

Vector::~Vector()
{
	this->!Vector();
}

Vector::!Vector()
{
	if ( m_pNativeVec ){
		delete m_pNativeVec;
	}
}

float Vector::x::get( void )
{
	return m_pNativeVec->x;
}

void Vector::x::set( float value )
{
	m_pNativeVec->x = value;
}

float Vector::y::get( void )
{
	return m_pNativeVec->y;
}

void Vector::y::set( float value )
{
	m_pNativeVec->y = value;
}

float Vector::z::get( void )
{
	return m_pNativeVec->z;
}

void Vector::z::set( float value )
{
	m_pNativeVec->z = value;
}

float Vector::w::get( void )
{
	return m_pNativeVec->w;
}

void Vector::w::set( float value )
{
	m_pNativeVec->w = value;
}

float Vector::default::get( int index )
{
	return (*m_pNativeVec)[index];
}

void Vector::default::set( int index, float value )
{
	(*m_pNativeVec)[index] = value;
}

Vector^ Vector::Set( float x, float y, float z, float w )
{
	this->GetNative()->Set( x, y, z, w );
	return this;
}

Vector^ Vector::operator =( Vector^ rhs )
{
	return this->Set( rhs->x, rhs->y, rhs->z, rhs->w );
}

//! スカラ乗算
Vector^ Vector::operator * ( Vector^ vec, float t )
{
	return gcnew Vector( *vec->GetNative() * t );
}

//! スカラ除算
Vector^ Vector::operator /( Vector^ vec, float rhs )
{
	return gcnew Vector( *vec->GetNative() / rhs );
}

//! スカラ乗算代入
Vector^ Vector::operator *= ( Vector^ vec, float rhs )
{
	*vec->GetNative() *= rhs;
	return vec;
}
//! スカラ除算代入
Vector^ Vector::operator /= ( Vector^ vec, float rhs )
{
	*vec->GetNative() /= rhs;
	return vec;
}

//! 加算
Vector^ Vector::operator +( Vector^ vec, Vector^ rhs )
{
	return gcnew Vector( *vec->GetNative() + *rhs->GetNative() );
}

//! 減算
Vector^ Vector::operator -( Vector^ vec, Vector^ rhs )
{
	return gcnew Vector( *vec->GetNative() - *rhs->GetNative() );
}

//! 加算代入
Vector^  Vector::operator +=( Vector^ vec, Vector^ rhs )
{
	*vec->GetNative() += *rhs->GetNative();
	return vec;
}

//! 減算代入
Vector^  Vector::operator -=( Vector^ vec, Vector^ rhs )
{
	*vec->GetNative() -= *rhs->GetNative();
	return vec;
}

//! negate
Vector^ Vector::operator -( Vector^ vec )
{
	return gcnew Vector( -(*vec->GetNative()) );
}

//!  ベクトル内積
float Vector::Dot( Vector^ rhs )
{
	return this->GetNative()->Dot( *rhs->GetNative() );
}

float Vector::Dot3( Vector^ rhs )
{
	return this->GetNativeVector3().Dot( rhs->GetNativeVector3() );
}

//!  ベクトル外積
Vector^ Vector::Cross( Vector^ rhs )
{
	return gcnew Vector( this->GetNativeVector3().Cross( rhs->GetNativeVector3() ) );
}

//!  ベクトルの長さ
float Vector::Length()
{
	return this->GetNative()->Length();
}

float Vector::Length3()
{
	return this->GetNativeVector3().Length();
}

//! @brief  ４次元ベクトルの正規化( 通常版 )
//! @attention  ゼロベクトルを渡すとNaNが返ります。
Vector^ Vector::Normalize()
{
	return gcnew Vector( this->GetNative()->Normalize() );
}

Vector^ Vector::Normalize3()
{
	return gcnew Vector( this->GetNativeVector3().Normalize() );
}

//! X軸単位ベクトル(1,0,0,0)の取得
Vector^ Vector::GetXUnit( )
{
	return gcnew Vector( gfl2::math::Vector4::GetXUnit() );
}

//! Y軸単位ベクトル(0,1,0,0)の取得
Vector^ Vector::GetYUnit( )
{
	return gcnew Vector( gfl2::math::Vector4::GetYUnit() );
}

//! Z軸単位ベクトル(0,0,1,0)の取得
Vector^ Vector::GetZUnit( )
{
	return gcnew Vector( gfl2::math::Vector4::GetZUnit() );
}
//! W軸単位ベクトル(0,0,0,1)の取得
Vector^ Vector::GetWUnit( )
{
	return gcnew Vector( gfl2::math::Vector4::GetWUnit() );
}

//! ゼロベクトル(0,0,0,0)を取得
Vector^ Vector::GetZero( )
{
	return gcnew Vector( gfl2::math::Vector4::GetZero() );
}


}}}
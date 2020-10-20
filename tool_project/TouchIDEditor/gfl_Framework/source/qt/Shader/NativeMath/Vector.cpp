#include <math.h>
#include "qt/Shader/NativeMath/Vector.h"

// @todo 完成したらcppは削除してヘッダーだけの単なるインライン関数にすべて変更する予定。ので将来的にはNativeMathはライブラリではなくなる。

namespace gfl { namespace maya { namespace Math {

Vector::Vector() :
	x( 0.0f ),
	y( 0.0f ),
	z( 0.0f ),
	w( 1.0f )
{

}

Vector::Vector( const Vector& other )
{
	*this = other;
}

Vector::Vector( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector::~Vector()
{

}

Vector& Vector::Set( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	return *this;
}

Vector & Vector::operator =( const Vector& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = rhs.w;
	return *this;
}

float& Vector::operator []( int index )
{
	return  this->f[index];
}

const float& Vector::operator []( int index ) const
{
	return  this->f[index];
}

//! スカラ乗算
Vector Vector::operator *( float rhs ) const
{
	return Vector( this->x * rhs, this->y * rhs, this->z * rhs, this->w * rhs );
}

//! スカラ除算
Vector Vector::operator /( float rhs ) const
{
	return Vector( this->x / rhs, this->y / rhs, this->z / rhs, this->w / rhs );
}

//! スカラ乗算代入
Vector&  Vector::operator *=( float rhs )
{
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

//! スカラ除算代入
Vector&  Vector::operator /=( float rhs )
{
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

//! 加算
Vector Vector::operator +( const Vector& rhs ) const
{
	return Vector( this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w );
}

//! 減算
Vector Vector::operator -( const Vector& rhs ) const
{
	return Vector( this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w );
}

//! 加算代入
Vector&  Vector::operator +=( const Vector& rhs )
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

//! 加算代入
Vector&  Vector::operator +=( const float& rhs )
{
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

//! 減算代入
Vector&  Vector::operator -=( const Vector& rhs )
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

//! 減算代入
Vector&  Vector::operator -=( const float& rhs )
{
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

//! negate
Vector Vector::operator -( ) const
{
	return Vector( -this->f[0],	-this->f[1],-this->f[2], -this->f[3] );
}

//! 比較
bool  Vector::operator ==( const Vector& rhs ) const
{
	if ( this->x != rhs.x )		return false;
	if ( this->y != rhs.y )		return false;
	if ( this->z != rhs.z )		return false;
	if ( this->w != rhs.w )		return false;

	return true;
}

//! 比較
bool  Vector::operator !=( const Vector& rhs ) const
{
	if ( this->x != rhs.x )		return true;
	if ( this->y != rhs.y )		return true;
	if ( this->z != rhs.z )		return true;
	if ( this->w != rhs.w )		return true;

	return false;
}

//!  ベクトル内積
float Vector::Dot( const Vector& rhs ) const
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y ) + ( this->z * rhs.z ) + ( this->w * rhs.w );
}

float Vector::Dot3( const Vector& rhs ) const
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y ) + ( this->z * rhs.z );
}

//!  ベクトル外積
// x = v0.y * v1.z - v1.y * v0.z
// y = v0.z * v1.x - v1.z * v0.x
// z = v0.x * v1.y - v1.x * v0.y
// w = v0.w
Vector Vector::Cross( const Vector& rhs ) const
{
	return Vector(
				this->f[1] * rhs.f[2] - rhs.f[1] * this->f[2],
				this->f[2] * rhs.f[0] - rhs.f[2] * this->f[0],
				this->f[0] * rhs.f[1] - rhs.f[0] * this->f[1],
				this->f[3] );
}

//!  ベクトルの長さ
float Vector::Length( ) const
{
	return ::sqrtf( this->Dot( *this ) );
}

float Vector::Length3( ) const
{
	return ::sqrtf( this->Dot3( *this ) );
}

//! @brief  ４次元ベクトルの正規化( 通常版 )
//! @attention  ゼロベクトルを渡すとNaNが返ります。
Vector Vector::Normalize( ) const
{
	float rlength = 1.0f/Length();
	return *this * rlength;
}

Vector Vector::Normalize3( ) const
{
	float length = 1.0f / Length3();
	return Vector(
			this->f[0]*length,
			this->f[1]*length,
			this->f[2]*length,
			this->f[3]
		);
}

//! X軸単位ベクトル(1,0,0,0)の取得
Vector Vector::GetXUnit( )
{
	return Vector( 1.0f, 0.0f, 0.0f, 0.0f );
}

//! Y軸単位ベクトル(0,1,0,0)の取得
Vector Vector::GetYUnit( )
{
	return Vector( 0.0f, 1.0f, 0.0f, 0.0f );
}

//! Z軸単位ベクトル(0,0,1,0)の取得
Vector Vector::GetZUnit( )
{
	return Vector( 0.0f, 0.0f, 1.0f, 0.0f );
}

//! W軸単位ベクトル(0,0,0,1)の取得
Vector Vector::GetWUnit( )
{
	return Vector( 0.0f, 0.0f, 0.0f, 1.0f );
}
//! ゼロベクトル(0,0,0,0)を取得
Vector Vector::GetZero( )
{
	return Vector( 0.0f, 0.0f, 0.0f, 0.0f );
}

}}}
﻿
inline Vector3::Vector3() :
	x( 0.0f ),
	y( 0.0f ),
	z( 0.0f )
{

}

inline Vector3::Vector3( const Vector3& other )
{
	*this = other;
}

inline Vector3::Vector3( const Vector2& other )
{
  this->x = other.x;
  this->y = other.y;
  this->z = 0.0f;
}

inline Vector3::Vector3( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

inline Vector3& Vector3::Set( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
	return *this;
}

// Set, Get an element
inline Vector3 & Vector3::SetX( f32 x ){    this->x = x;   return *this; }
inline Vector3 & Vector3::SetY( f32 y ){    this->y = y;   return *this; }
inline Vector3 & Vector3::SetZ( f32 z ){    this->z = z;   return *this; }
inline f32 Vector3::GetX( ) const { return this->x; }
inline f32 Vector3::GetY( ) const { return this->y; }
inline f32 Vector3::GetZ( ) const { return this->z; }


inline Vector3 & Vector3::operator =( const Vector3& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	return *this;
}
inline Vector3 & Vector3::operator =( const Vector2& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = 0.0f;
	return *this;
}

inline float& Vector3::operator []( int index )
{
	return  (&this->x)[index];
}

inline const float& Vector3::operator []( int index ) const
{
	return  (&this->x)[index];
}

//! スカラ乗算
inline Vector3 Vector3::operator *( float rhs ) const
{
	return Vector3( this->x * rhs, this->y * rhs, this->z * rhs );
}

//! スカラ除算
inline Vector3 Vector3::operator /( float rhs ) const
{
	return Vector3( this->x / rhs, this->y / rhs, this->z / rhs );
}

//! スカラ乗算代入
inline Vector3&  Vector3::operator *=( float rhs )
{
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

//! スカラ除算代入
inline Vector3&  Vector3::operator /=( float rhs )
{
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

//! 加算
inline Vector3 Vector3::operator +( const Vector3& rhs ) const
{
	return Vector3( this->x + rhs.x, this->y + rhs.y, this->z + rhs.z );
}

//! 減算
inline Vector3 Vector3::operator -( const Vector3& rhs ) const
{
	return Vector3( this->x - rhs.x, this->y - rhs.y, this->z - rhs.z );
}

//! 加算代入
inline Vector3&  Vector3::operator +=( const Vector3& rhs )
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

//! 加算代入
inline Vector3&  Vector3::operator +=( const float& rhs )
{
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

//! 減算代入
inline Vector3&  Vector3::operator -=( const Vector3& rhs )
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

//! 減算代入
Vector3&  Vector3::operator -=( const float& rhs )
{
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

//! negate
inline Vector3 Vector3::operator -( ) const
{
	return Vector3( -this->x,	-this->y,-this->z );
}

//! 比較
inline bool  Vector3::operator ==( const Vector3& rhs ) const
{
	if ( this->x != rhs.x )		return false;
	if ( this->y != rhs.y )		return false;
	if ( this->z != rhs.z )		return false;

	return true;
}

//! 比較
inline bool  Vector3::operator !=( const Vector3& rhs ) const
{
	if ( this->x != rhs.x )		return true;
	if ( this->y != rhs.y )		return true;
	if ( this->z != rhs.z )		return true;

	return false;
}

// Maximum and Minimum per element
inline Vector3 Vector3::MaxPerElem( const Vector3 &rhs ) const
{
	return Vector3( 
			max( this->x, rhs.x ),
			max( this->y, rhs.y ),
			max( this->z, rhs.z )
		);
}
inline Vector3 Vector3::MinPerElem( const Vector3 &rhs ) const
{
	return Vector3( 
			min( this->x, rhs.x ),
			min( this->y, rhs.y ),
			min( this->z, rhs.z )
		);
}

// the Maximum or Minimum element
inline f32 Vector3::MaxElem( void ) const
{
	return max( max( this->x, this->y ), this->z );
}
inline f32 Vector3::MinElem( void ) const
{
	return min( min( this->x, this->y ), this->z );
}

//!  ベクトル内積
inline float Vector3::Dot( const Vector3& rhs ) const
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y ) + ( this->z * rhs.z );
}

//!  ベクトル外積
// x = v0.y * v1.z - v1.y * v0.z
// y = v0.z * v1.x - v1.z * v0.x
// z = v0.x * v1.y - v1.x * v0.y
inline Vector3 Vector3::Cross( const Vector3& rhs ) const
{
	return Vector3(
				this->y * rhs.z - rhs.y * this->z,
				this->z * rhs.x - rhs.z * this->x,
				this->x * rhs.y - rhs.x * this->y );
}

//!  ベクトルの長さ
inline float Vector3::Length( ) const
{
	return ::sqrtf( this->Dot( *this ) );
}

//!  ベクトルの長さの2乗を返す
inline float Vector3::LengthSq( ) const
{
	return this->Dot( *this );
}

//! @brief  ３次元ベクトルの正規化( 通常版 )
//! @attention  ゼロベクトルを渡すとNaNが返ります。
inline Vector3 Vector3::Normalize( ) const
{
	float rlength = 1.0f/Length();
	return *this * rlength;
}

//! ベクトルの線形補完
inline void Vector3::Lerp(const Vector3& v, f32 t)
{
  this->x += (v.x - this->x) * t;
  this->y += (v.y - this->y) * t;
  this->z += (v.z - this->z) * t;
}

//! X軸単位ベクトル(1,0,0)の取得
inline Vector3 Vector3::GetXUnit( )
{
	return Vector3( 1.0f, 0.0f, 0.0f );
}

//! Y軸単位ベクトル(0,1,0)の取得
inline Vector3 Vector3::GetYUnit( )
{
	return Vector3( 0.0f, 1.0f, 0.0f );
}

//! Z軸単位ベクトル(0,0,1)の取得
inline Vector3 Vector3::GetZUnit( )
{
	return Vector3( 0.0f, 0.0f, 1.0f );
}

//! ゼロベクトル(0,0,0)を取得
inline Vector3 Vector3::GetZero( )
{
	return Vector3( 0.0f, 0.0f, 0.0f );
}

// ベクトル要素の符号をビットマスクで取得(sign(z)<<2 | sign(y)<<1 | sign(x))
inline u32 Vector3::GetSignMask() const
{
	u32 id = 0;
	if ( this->x < 0.0f ){
		id |= 1<<0;
	}
	if ( this->y < 0.0f ){
		id |= 1<<1;
	}
	if ( this->z < 0.0f ){
		id |= 1<<2;
	}
	return id;
}


inline Vector2::Vector2() :
	x( 0.0f ),
	y( 0.0f )
{

}

inline Vector2::Vector2( const Vector2& other )
{
	*this = other;
}

inline Vector2::Vector2( float x, float y )
{
	this->x = x;
	this->y = y;
}

inline Vector2& Vector2::Set( float x, float y )
{
	this->x = x;
	this->y = y;
	return *this;
}

// Set, Get an element
inline Vector2 & Vector2::SetX( f32 x ){    this->x = x;   return *this; }
inline Vector2 & Vector2::SetY( f32 y ){    this->y = y;   return *this; }
inline f32 Vector2::GetX( ) const { return this->x; }
inline f32 Vector2::GetY( ) const { return this->y; }


inline Vector2 & Vector2::operator =( const Vector2& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
	return *this;
}

inline float& Vector2::operator []( int index )
{
	return  (&this->x)[index];
}

inline const float& Vector2::operator []( int index ) const
{
	return  (&this->x)[index];
}

//! スカラ乗算
inline Vector2 Vector2::operator *( float rhs ) const
{
	return Vector2( this->x * rhs, this->y * rhs );
}

//! スカラ除算
inline Vector2 Vector2::operator /( float rhs ) const
{
	return Vector2( this->x / rhs, this->y / rhs );
}

//! スカラ乗算代入
inline Vector2&  Vector2::operator *=( float rhs )
{
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}

//! スカラ除算代入
inline Vector2&  Vector2::operator /=( float rhs )
{
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

//! 加算
inline Vector2 Vector2::operator +( const Vector2& rhs ) const
{
	return Vector2( this->x + rhs.x, this->y + rhs.y );
}

//! 減算
inline Vector2 Vector2::operator -( const Vector2& rhs ) const
{
	return Vector2( this->x - rhs.x, this->y - rhs.y );
}

//! 加算代入
inline Vector2&  Vector2::operator +=( const Vector2& rhs )
{
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

//! 加算代入
inline Vector2&  Vector2::operator +=( const float& rhs )
{
	this->x += rhs;
	this->y += rhs;
	return *this;
}

//! 減算代入
inline Vector2&  Vector2::operator -=( const Vector2& rhs )
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

//! 減算代入
Vector2&  Vector2::operator -=( const float& rhs )
{
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}

//! negate
inline Vector2 Vector2::operator -( ) const
{
	return Vector2( -this->x,	-this->y );
}

//! 比較
inline bool  Vector2::operator ==( const Vector2& rhs ) const
{
	if ( this->x != rhs.x )		return false;
	if ( this->y != rhs.y )		return false;

	return true;
}

//! 比較
inline bool  Vector2::operator !=( const Vector2& rhs ) const
{
	if ( this->x != rhs.x )		return true;
	if ( this->y != rhs.y )		return true;

	return false;
}

// Maximum and Minimum per element
inline Vector2 Vector2::MaxPerElem( const Vector2 &rhs ) const
{
	return Vector2( 
			max( this->x, rhs.x ),
			max( this->y, rhs.y )
		);
}
inline Vector2 Vector2::MinPerElem( const Vector2 &rhs ) const
{
	return Vector2( 
			min( this->x, rhs.x ),
			min( this->y, rhs.y )
		);
}

// the Maximum or Minimum element
inline f32 Vector2::MaxElem( void ) const
{
	return max( this->x, this->y );
}
inline f32 Vector2::MinElem( void ) const
{
	return min( this->x, this->y );
}

//!  ベクトル内積
inline float Vector2::Dot( const Vector2& rhs ) const
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y );
}

//!  ベクトル外積
// z = v0.x * v1.y - v1.x * v0.y
inline f32 Vector2::Cross( const Vector2& rhs ) const
{
	return this->x * rhs.y - rhs.x * this->y;
}

//!  ベクトルの長さ
inline float Vector2::Length( ) const
{
	return ::sqrtf( this->Dot( *this ) );
}

//! @brief  ３次元ベクトルの正規化( 通常版 )
//! @attention  ゼロベクトルを渡すとNaNが返ります。
inline Vector2 Vector2::Normalize( ) const
{
	float rlength = 1.0f/Length();
	return *this * rlength;
}

//! ベクトルの線形補完
inline void Vector2::Lerp(const Vector2& v, f32 t)
{
  this->x += (v.x - this->x) * t;
  this->y += (v.y - this->y) * t;
}

//! X軸単位ベクトル(1,0)の取得
inline Vector2 Vector2::GetXUnit( )
{
	return Vector2( 1.0f, 0.0f );
}

//! Y軸単位ベクトル(0,1)の取得
inline Vector2 Vector2::GetYUnit( )
{
	return Vector2( 0.0f, 1.0f );
}

//! ゼロベクトル(0,0)を取得
inline Vector2 Vector2::GetZero( )
{
	return Vector2( 0.0f, 0.0f );
}

// ベクトル要素の符号をビットマスクで取得(sign(y)<<1 | sign(x))
inline u32 Vector2::GetSignMask() const
{
	u32 id = 0;
	if ( this->x < 0.0f ){
		id |= 1<<0;
	}
	if ( this->y < 0.0f ){
		id |= 1<<1;
	}
	return id;
}

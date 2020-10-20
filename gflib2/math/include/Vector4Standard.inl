
inline Vector4::Vector4() :
	x( 0.0f ),
	y( 0.0f ),
	z( 0.0f ),
	w( 1.0f )
{

}

inline Vector4::Vector4( const Vector4& other )
{
	*this = other;
}
inline Vector4::Vector4( const Vector3& other )
{
  this->x = other.x;
  this->y = other.y;
  this->z = other.z;
  this->w = 1.0f;
}

#if defined( GFL2_MAYA )
inline Vector4::Vector4( const MVector& other )
{
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->w =1.0f;
}
#endif

inline Vector4::Vector4( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline Vector4& Vector4::Set( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	return *this;
}

// Set, Get an element
inline Vector4 & Vector4::SetX( f32 x ){    this->x = x;   return *this; }
inline Vector4 & Vector4::SetY( f32 y ){    this->y = y;   return *this; }
inline Vector4 & Vector4::SetZ( f32 z ){    this->z = z;   return *this; }
inline Vector4 & Vector4::SetW( f32 w ){    this->w = w;   return *this; }
inline f32 Vector4::GetX( ) const { return this->x; }
inline f32 Vector4::GetY( ) const { return this->y; }
inline f32 Vector4::GetZ( ) const { return this->z; }
inline f32 Vector4::GetW( ) const { return this->w; }


inline Vector4 & Vector4::operator =( const Vector4& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = rhs.w;
	return *this;
}
inline Vector4 & Vector4::operator =( const Vector3& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = 1.0f;
	return *this;
}

#if defined( GFL2_MAYA )
inline Vector4 & Vector4::operator =( const MVector& rhs )
{
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = 1.0f;
	return *this;
}

inline MVector Vector4::GetMVector() const
{
	MVector		mvec;
	mvec.x = this->x;
	mvec.y = this->y;
	mvec.z = this->z;
	return mvec;
}

inline MFloatPoint Vector4::GetMFloatPoint() const
{
  MFloatPoint		mpoint;
	mpoint.x = this->x;
	mpoint.y = this->y;
	mpoint.z = this->z;
	return mpoint;
}
#endif

__forceinline float& Vector4::operator []( int index )
{
	return  (&this->x)[index];
}

__forceinline const float& Vector4::operator []( int index ) const
{
	return  (&this->x)[index];
}

//! スカラ乗算
inline Vector4 Vector4::operator *( float rhs ) const
{
	return Vector4( this->x * rhs, this->y * rhs, this->z * rhs, this->w * rhs );
}

//! スカラ除算
inline Vector4 Vector4::operator /( float rhs ) const
{
	return Vector4( this->x / rhs, this->y / rhs, this->z / rhs, this->w / rhs );
}

//! スカラ乗算代入
inline Vector4&  Vector4::operator *=( float rhs )
{
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	this->w *= rhs;
	return *this;
}

//! スカラ除算代入
inline Vector4&  Vector4::operator /=( float rhs )
{
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	this->w /= rhs;
	return *this;
}

//! 加算
inline Vector4 Vector4::operator +( const Vector4& rhs ) const
{
	return Vector4( this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w );
}

//! 減算
inline Vector4 Vector4::operator -( const Vector4& rhs ) const
{
	return Vector4( this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w );
}

//! 加算代入
inline Vector4&  Vector4::operator +=( const Vector4& rhs )
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

//! 加算代入
inline Vector4&  Vector4::operator +=( const float& rhs )
{
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	this->w += rhs;
	return *this;
}

//! 減算代入
inline Vector4&  Vector4::operator -=( const Vector4& rhs )
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;
	return *this;
}

//! 減算代入
Vector4&  Vector4::operator -=( const float& rhs )
{
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	this->w -= rhs;
	return *this;
}

//! negate
inline Vector4 Vector4::operator -( ) const
{
	return Vector4( -this->x,	-this->y,-this->z, -this->w );
}

//! 比較
inline bool  Vector4::operator ==( const Vector4& rhs ) const
{
	if ( this->x != rhs.x )		return false;
	if ( this->y != rhs.y )		return false;
	if ( this->z != rhs.z )		return false;
	if ( this->w != rhs.w )		return false;

	return true;
}

//! 比較
inline bool  Vector4::operator !=( const Vector4& rhs ) const
{
	if ( this->x != rhs.x )		return true;
	if ( this->y != rhs.y )		return true;
	if ( this->z != rhs.z )		return true;
	if ( this->w != rhs.w )		return true;

	return false;
}

// Maximum and Minimum per element
inline Vector4 Vector4::MaxPerElem( const Vector4 &rhs ) const
{
	return Vector4( 
			max( this->x, rhs.x ),
			max( this->y, rhs.y ),
			max( this->z, rhs.z ),
			max( this->w, rhs.w )
		);
}
inline Vector4 Vector4::MinPerElem( const Vector4 &rhs ) const
{
	return Vector4( 
			min( this->x, rhs.x ),
			min( this->y, rhs.y ),
			min( this->z, rhs.z ),
			min( this->w, rhs.w )
		);
}

// the Maximum or Minimum element
inline f32 Vector4::MaxElem( void ) const
{
	return max( max( this->x, this->y ), max( this->z, this->w ) );
}
inline f32 Vector4::MinElem( void ) const
{
	return min( min( this->x, this->y ), min( this->z, this->w ) );
}

// the Maximum or Minimum element
inline f32 Vector4::MaxElem3( void ) const
{
	return max( max( this->x, this->y ), this->z );
}
inline f32 Vector4::MinElem3( void ) const
{
	return min( min( this->x, this->y ), this->z );
}

//!  ベクトル内積
inline float Vector4::Dot( const Vector4& rhs ) const
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y ) + ( this->z * rhs.z ) + ( this->w * rhs.w );
}

inline float Vector4::Dot3( const Vector4& rhs ) const
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y ) + ( this->z * rhs.z );
}

//!  ベクトル外積
// x = v0.y * v1.z - v1.y * v0.z
// y = v0.z * v1.x - v1.z * v0.x
// z = v0.x * v1.y - v1.x * v0.y
// w = v0.w
inline Vector4 Vector4::Cross( const Vector4& rhs ) const
{
	return Vector4(
				this->y * rhs.z - rhs.y * this->z,
				this->z * rhs.x - rhs.z * this->x,
				this->x * rhs.y - rhs.x * this->y,
				this->w );
}

//!  ベクトルの長さ
inline float Vector4::Length( ) const
{
	return ::sqrtf( this->Dot( *this ) );
}

inline float Vector4::Length3( ) const
{
	return ::sqrtf( this->Dot3( *this ) );
}

//! @brief  ４次元ベクトルの正規化( 通常版 )
//! @attention  ゼロベクトルを渡すとNaNが返ります。
inline Vector4 Vector4::Normalize( ) const
{
	float rlength = 1.0f/Length();
	return *this * rlength;
}

inline Vector4 Vector4::Normalize3( ) const
{
	float length = 1.0f / Length3();
	return Vector4(
			this->x*length,
			this->y*length,
			this->z*length,
			this->w
		);
}

//! ベクトルの線形補完
inline void Vector4::Lerp(const Vector4& v, f32 t)
{
  this->x += (v.x - this->x) * t;
  this->y += (v.y - this->y) * t;
  this->z += (v.z - this->z) * t;
  this->w += (v.w - this->w) * t;
}

//! X軸単位ベクトル(1,0,0,0)の取得
inline Vector4 Vector4::GetXUnit( )
{
	return Vector4( 1.0f, 0.0f, 0.0f, 0.0f );
}

//! Y軸単位ベクトル(0,1,0,0)の取得
inline Vector4 Vector4::GetYUnit( )
{
	return Vector4( 0.0f, 1.0f, 0.0f, 0.0f );
}

//! Z軸単位ベクトル(0,0,1,0)の取得
inline Vector4 Vector4::GetZUnit( )
{
	return Vector4( 0.0f, 0.0f, 1.0f, 0.0f );
}

//! W軸単位ベクトル(0,0,0,1)の取得
inline Vector4 Vector4::GetWUnit( )
{
	return Vector4( 0.0f, 0.0f, 0.0f, 1.0f );
}
//! ゼロベクトル(0,0,0,0)を取得
inline Vector4 Vector4::GetZero( )
{
	return Vector4( 0.0f, 0.0f, 0.0f, 0.0f );
}

// ベクトル要素の符号をビットマスクで取得(sign(w)<<3 | sign(z)<<2 | sign(y)<<1 | sign(x))
inline u32 Vector4::GetSignMask() const
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
	if ( this->w < 0.0f ){
		id |= 1<<3;
	}
	return id;
}

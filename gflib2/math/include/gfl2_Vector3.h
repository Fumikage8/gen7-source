#ifndef GFLIB2_MATH_VECTOR3_H_INCLUDED
#define GFLIB2_MATH_VECTOR3_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_Vector2.h>

namespace gfl2 { namespace math {

class Vector3
{
public:

	static const u32			ElementNum = 3;

	//! @brief デフォルトコンストラクタ
	inline Vector3( );
	//! @brief コピーコンストラクタ
	inline Vector3( const Vector3& other );
  inline Vector3( const Vector2& other );

	//! @brief f32×４からベクトルを作成
	inline Vector3( f32 x, f32 y, f32 z );

	//! 要素の設定
	inline Vector3&  Set( f32 x, f32 y, f32 z );

	inline Vector3&  Set( const Vector3 &v ){ *this = v; return *this; }

	inline Vector3& SetX( float x );                //!< X要素の設定
	inline float GetX( ) const;                    //!< X要素の取得
	inline Vector3& SetY( float y );                //!< Y要素の設定
	inline float GetY( ) const;                    //!< Y要素の取得
	inline Vector3& SetZ( float z );                //!< Z要素の設定
	inline float GetZ( ) const;                    //!< Z要素の取得

	inline Vector3&  operator =( const Vector3& rhs );
	inline Vector3&  operator =( const Vector2& rhs );

	inline f32& operator []( int index );
	inline const f32& operator []( int index ) const;
	
	//! スカラ乗算
	inline Vector3 operator *( f32 rhs ) const;
	//! スカラ除算
	inline Vector3 operator /( f32 rhs ) const;
	
	//! スカラ乗算代入
	inline Vector3&  operator *=( f32 rhs );
	//! スカラ除算代入
	inline Vector3&  operator /=( f32 rhs );

	//! 加算
	inline Vector3 operator +( const Vector3& rhs ) const;
	//! 減算
	inline Vector3 operator -( const Vector3& rhs ) const;
	//! 加算代入
	inline Vector3&  operator +=( const Vector3& rhs );
	inline Vector3&  operator +=( const f32& rhs );
	//! 減算代入
	inline Vector3&  operator -=( const Vector3& rhs );
	inline Vector3&  operator -=( const f32& rhs );

	//! negate
	inline Vector3 operator -( ) const;

	//! 比較
	inline bool  operator ==( const Vector3& rhs ) const;
	inline bool  operator !=( const Vector3& rhs ) const;

	//!  ベクトル要素同士の最大値
	inline Vector3 MaxPerElem( const Vector3& rhs ) const;
	//!  ベクトル要素同士の最小値
	inline Vector3 MinPerElem( const Vector3& rhs ) const;
		
	//!  ベクトルの各要素内での最大値
	inline f32 MaxElem( ) const;
	//!  ベクトルの各要素内での最小値
	inline f32 MinElem( ) const;

	//!  ベクトル内積
	inline f32 Dot( const Vector3& rhs ) const;

	//!  ベクトル外積
	inline Vector3 Cross( const Vector3& rhs ) const;
	//!  ベクトルの長さ
	inline f32 Length( ) const;
  //!  ベクトルの長さの2乗を返す
  inline f32 LengthSq( ) const;
	//! @brief  ３次元ベクトルの正規化( 通常版 )
	//! @attention  ゼロベクトルを渡すとNaNが返ります。
	inline Vector3 Normalize( ) const;

  /**
   * @brief 線形補完
   * @param v 補完するベクトル
   * @param t 補完係数 0.0 ~ 1.0
   */
  inline void Lerp(const Vector3& v, f32 t);

	/**
	 *   正規化
	 * @param  無し
	 * @return 元の大きさ
	 */

	inline f32 NormalizeRet( void ) {
		f32 ret = Length( );
		*this *= ( math::FRecip( ret ) );
		return ret;
	}

	//! X軸単位ベクトル(1,0,0)の取得
	static inline Vector3 GetXUnit( );
	//! Y軸単位ベクトル(0,1,0)の取得
	static inline Vector3 GetYUnit( );
	//! Z軸単位ベクトル(0,0,1)の取得
	static inline Vector3 GetZUnit( );
	//! ゼロベクトル(0,0,0)を取得
	static inline Vector3 GetZero( );

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline f32& GetRefX( void ) {
		return GFL_VECTOR_X;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline f32& GetRefY( void ) {
		return GFL_VECTOR_Y;
	}

	/**
	 *   Z成分を取得
	 * @param  無し
	 * @return Z成分
	 */

	inline f32& GetRefZ( void ) {
		return GFL_VECTOR_Z;
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  無し
	 * @return 無し
	 */

	inline void Neg( void ) {
		*this = - * this;
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */

	inline void Neg( const Vector3& t ) {
		*this = -t;
	}

	// 値がすべてマイナスか判定
	inline bool IsAllMinus() const
	{
		return x < 0.0f && y < 0.0f && z < 0.0f;
	}

	// 値がどれかマイナスか判定
	inline bool IsAnyMinus() const
	{
		return x < 0.0f || y < 0.0f || z < 0.0f;
	}

	// X要素をブロードキャストしたベクトル(x,x,x)を取得
	inline Vector3 GetBroadcastX() const
	{
		float value = x;
		return Vector3( value, value, value );
	}

	// Y要素をブロードキャストしたベクトル(y,y,y)を取得
	inline Vector3 GetBroadcastY() const
	{
		float value = y;
		return Vector3( value, value, value );
	}

	// Z要素をブロードキャストしたベクトル(z,z,z)を取得
	inline Vector3 GetBroadcastZ() const
	{
		float value = z;
		return Vector3( value, value, value );
	}

	// Multiply element by element
	inline Vector3 Vector3::MulPerElem( const Vector3 &rhs ) const
	{
		return Vector3(
				x * rhs.x,
				y * rhs.y,
				z * rhs.z );
	}

	// divide element by element
	inline Vector3 Vector3::DivPerElem( const Vector3 &rhs ) const
	{
		return Vector3(
				x / rhs.x,
				y / rhs.y,
				z / rhs.z );
	}

	// Absolute values per element
	inline Vector3 Vector3::AbsPerElem( void ) const
	{
		return Vector3(
				fabsf( x ),
				fabsf( y ),
				fabsf( z ) );
	}
	
	inline u32 GetSignMask() const;

  union
  {
    struct
    {
      f32		x;
      f32		y;
      f32		z;
    };
    struct
    {
      f32   f[3];
    };
  };
};

#include "Vector3Standard.inl"

}}

#endif

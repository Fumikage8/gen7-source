#ifndef GFLIB2_MATH_VECTOR2_H_INCLUDED
#define GFLIB2_MATH_VECTOR2_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>

namespace gfl2 { namespace math {

class Vector2
{
public:

	static const u32			ElementNum = 2;

	//! @brief デフォルトコンストラクタ
	inline Vector2( );
	//! @brief コピーコンストラクタ
	inline Vector2( const Vector2& other );

	//! @brief f32×４からベクトルを作成
	inline Vector2( f32 x, f32 y );

	//! 要素の設定
	inline Vector2&  Set( f32 x, f32 y );

	inline Vector2&  Set( const Vector2 &v ){ *this = v; return *this; }

	inline Vector2& SetX( float x );                //!< X要素の設定
	inline float GetX( ) const;                    //!< X要素の取得
	inline Vector2& SetY( float y );                //!< Y要素の設定
	inline float GetY( ) const;                    //!< Y要素の取得

	inline Vector2&  operator =( const Vector2& rhs );

	inline f32& operator []( int index );
	inline const f32& operator []( int index ) const;
	
	//! スカラ乗算
	inline Vector2 operator *( f32 rhs ) const;
	//! スカラ除算
	inline Vector2 operator /( f32 rhs ) const;
	
	//! スカラ乗算代入
	inline Vector2&  operator *=( f32 rhs );
	//! スカラ除算代入
	inline Vector2&  operator /=( f32 rhs );

	//! 加算
	inline Vector2 operator +( const Vector2& rhs ) const;
	//! 減算
	inline Vector2 operator -( const Vector2& rhs ) const;
	//! 加算代入
	inline Vector2&  operator +=( const Vector2& rhs );
	inline Vector2&  operator +=( const f32& rhs );
	//! 減算代入
	inline Vector2&  operator -=( const Vector2& rhs );
	inline Vector2&  operator -=( const f32& rhs );

	//! negate
	inline Vector2 operator -( ) const;

	//! 比較
	inline bool  operator ==( const Vector2& rhs ) const;
	inline bool  operator !=( const Vector2& rhs ) const;

	//!  ベクトル要素同士の最大値
	inline Vector2 MaxPerElem( const Vector2& rhs ) const;
	//!  ベクトル要素同士の最小値
	inline Vector2 MinPerElem( const Vector2& rhs ) const;
		
	//!  ベクトルの各要素内での最大値
	inline f32 MaxElem( ) const;
	//!  ベクトルの各要素内での最小値
	inline f32 MinElem( ) const;

	//!  ベクトル内積
	inline f32 Dot( const Vector2& rhs ) const;

	//!  ベクトル外積
	inline f32 Cross( const Vector2& rhs ) const;
	//!  ベクトルの長さ
	inline f32 Length( ) const;
	//! @brief  ２次元ベクトルの正規化( 通常版 )
	//! @attention  ゼロベクトルを渡すとNaNが返ります。
	inline Vector2 Normalize( ) const;

  /**
   * @brief 線形補完
   * @param v 補完するベクトル
   * @param t 補完係数 0.0 ~ 1.0
   */
  inline void Lerp(const Vector2& v, f32 t);

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

	//! X軸単位ベクトル(1,0)の取得
	static inline Vector2 GetXUnit( );
	//! Y軸単位ベクトル(0,1)の取得
	static inline Vector2 GetYUnit( );
	//! ゼロベクトル(0,0)を取得
	static inline Vector2 GetZero( );

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

	inline void Neg( const Vector2& t ) {
		*this = -t;
	}

	// 値がすべてマイナスか判定
	inline bool IsAllMinus() const
	{
		return x < 0.0f && y < 0.0f;
	}

	// 値がどれかマイナスか判定
	inline bool IsAnyMinus() const
	{
		return x < 0.0f || y < 0.0f;
	}

	// X要素をブロードキャストしたベクトル(x,x)を取得
	inline Vector2 GetBroadcastX() const
	{
		float value = x;
		return Vector2( value, value );
	}

	// Y要素をブロードキャストしたベクトル(y,y)を取得
	inline Vector2 GetBroadcastY() const
	{
		float value = y;
		return Vector2( value, value  );
	}

	// Multiply element by element
	inline Vector2 Vector2::MulPerElem( const Vector2 &rhs ) const
	{
		return Vector2(
				x * rhs.x,
				y * rhs.y );
	}

	// divide element by element
	inline Vector2 Vector2::DivPerElem( const Vector2 &rhs ) const
	{
		return Vector2(
				x / rhs.x,
				y / rhs.y );
	}

	// Absolute values per element
	inline Vector2 Vector2::AbsPerElem( void ) const
	{
		return Vector2(
				fabsf( x ),
				fabsf( y ) );
	}
	
	inline u32 GetSignMask() const;

  union
  {
    struct
    {
      f32		x;
      f32		y;
    };
    struct
    {
      f32   f[2];
    };
  };
};

#include "Vector2Standard.inl"

}}

#endif

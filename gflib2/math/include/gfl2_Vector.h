#ifndef GFLIB2_MATH_VECTOR_H_INCLUDED
#define GFLIB2_MATH_VECTOR_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>

#if defined( GFL2_MAYA )
#include <maya/MVector.h>
#include <maya/Mf32Point.h>
#endif

namespace gfl2 { namespace math {

class Vector
{
public:

	static const u32			ElementNum = 4;

	//! @brief デフォルトコンストラクタ
	inline Vector( );
	//! @brief コピーコンストラクタ
	inline Vector( const Vector& other );
#if defined( GFL2_MAYA )
	inline Vector( const MVector& other );
#endif

	//! @brief f32×４からベクトルを作成
	inline Vector( f32 x, f32 y, f32 z, f32 w = ((f32) 1.0) );

	//! 要素の設定
	inline Vector&  Set( f32 x, f32 y, f32 z, f32 w = ((f32) 1.0) );

	inline Vector&  Set( const Vector &v ){ *this = v; return *this; }

	inline Vector& SetX( float x );                //!< X要素の設定
	inline float GetX( ) const;                    //!< X要素の取得
	inline Vector& SetY( float y );                //!< Y要素の設定
	inline float GetY( ) const;                    //!< Y要素の取得
	inline Vector& SetZ( float z );                //!< Z要素の設定
	inline float GetZ( ) const;                    //!< Z要素の取得
	inline Vector& SetW( float w );                //!< W要素の設定
	inline Vector& SetWZero();                     //!< W要素を0.0に設定
	inline Vector& SetWOne();                      //!< W要素を1.0に設定
	inline float GetW( ) const;                    //!< W要素の取得

	inline Vector&  operator =( const Vector& rhs );
#if defined( GFL2_MAYA )
	inline Vector&  operator =( const MVector& rhs );
#endif

	inline f32& operator []( int index );
	inline const f32& operator []( int index ) const;
	
	//! スカラ乗算
	inline Vector operator *( f32 rhs ) const;
	//! スカラ除算
	inline Vector operator /( f32 rhs ) const;
	
	//! スカラ乗算代入
	inline Vector&  operator *=( f32 rhs );
	//! スカラ除算代入
	inline Vector&  operator /=( f32 rhs );

	//! 加算
	inline Vector operator +( const Vector& rhs ) const;
	//! 減算
	inline Vector operator -( const Vector& rhs ) const;
	//! 加算代入
	inline Vector&  operator +=( const Vector& rhs );
	inline Vector&  operator +=( const f32& rhs );
	//! 減算代入
	inline Vector&  operator -=( const Vector& rhs );
	inline Vector&  operator -=( const f32& rhs );

	//! negate
	inline Vector operator -( ) const;

	//! 比較
	inline bool  operator ==( const Vector& rhs ) const;
	inline bool  operator !=( const Vector& rhs ) const;

	//!  ベクトル要素同士の最大値
	inline Vector MaxPerElem( const Vector& rhs ) const;
	//!  ベクトル要素同士の最小値
	inline Vector MinPerElem( const Vector& rhs ) const;
		
	//!  ベクトルの各要素内での最大値
	inline f32 MaxElem( ) const;
	//!  ベクトルの各要素内での最大値
	inline f32 MaxElem3( ) const;
	//!  ベクトルの各要素内での最小値
	inline f32 MinElem( ) const;
	//!  ベクトルの各要素内での最小値
	inline f32 MinElem3( ) const;

	//!  ベクトル内積
	inline f32 Dot( const Vector& rhs ) const;
	inline f32 Dot3( const Vector& rhs ) const;

	//!  ベクトル外積
	inline Vector Cross( const Vector& rhs ) const;
	//!  ベクトルの長さ
	inline f32 Length( ) const;
	inline f32 Length3( ) const;
	//! @brief  ４次元ベクトルの正規化( 通常版 )
	//! @attention  ゼロベクトルを渡すとNaNが返ります。
	inline Vector Normalize( ) const;
	inline Vector Normalize3( ) const;

  /**
   * @brief 線形補完
   * @param v 補完するベクトル
   * @param t 補完係数 0.0 ~ 1.0
   */
  inline void Lerp(const Vector& v, f32 t);

#if defined( GFL2_MAYA )
	inline MVector GetMVector() const;
  inline Mf32Point GetMf32Point() const;
#endif

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

	//! X軸単位ベクトル(1,0,0,0)の取得
	static inline Vector GetXUnit( );
	//! Y軸単位ベクトル(0,1,0,0)の取得
	static inline Vector GetYUnit( );
	//! Z軸単位ベクトル(0,0,1,0)の取得
	static inline Vector GetZUnit( );
	//! W軸単位ベクトル(0,0,0,1)の取得
	static inline Vector GetWUnit( );
	//! ゼロベクトル(0,0,0,0)を取得
	static inline Vector GetZero( );

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

	inline void Neg( const Vector& t ) {
		*this = -t;
	}

	// 値がすべてマイナスか判定
	inline bool IsAllMinus() const
	{
		return x < 0.0f && y < 0.0f && z < 0.0f && w < 0.0f;
	}

	// XYZの値がすべてマイナスか判定
	inline bool IsAllMinus3() const
	{
		return x < 0.0f && y < 0.0f && z < 0.0f;
	}

	// 値がどれかマイナスか判定
	inline bool IsAnyMinus() const
	{
		return x < 0.0f || y < 0.0f || z < 0.0f || w < 0.0f;
	}

	// XYZの値がどれかマイナスか判定
	inline bool IsAnyMinus3() const
	{
		return x < 0.0f || y < 0.0f || z < 0.0f;
	}

	// X要素をブロードキャストしたベクトル(x,x,x,x)を取得
	inline Vector GetBroadcastX() const
	{
		float value = x;
		return Vector( value, value, value, value );
	}

	// Y要素をブロードキャストしたベクトル(y,y,y,y)を取得
	inline Vector GetBroadcastY() const
	{
		float value = y;
		return Vector( value, value, value, value );
	}

	// Z要素をブロードキャストしたベクトル(z,z,z,z)を取得
	inline Vector GetBroadcastZ() const
	{
		float value = z;
		return Vector( value, value, value, value );
	}

	// W要素をブロードキャストしたベクトル(w,w,w,w)を取得
	inline Vector GetBroadcastW() const
	{
		float value = w;
		return Vector( value, value, value, value );
	}

	// Multiply element by element
	inline Vector Vector::MulPerElem( const Vector &rhs ) const
	{
		return Vector(
				x * rhs.x,
				y * rhs.y,
				z * rhs.z,
				w * rhs.w );
	}

	// divide element by element
	inline Vector Vector::DivPerElem( const Vector &rhs ) const
	{
		return Vector(
				x / rhs.x,
				y / rhs.y,
				z / rhs.z,
				w / rhs.w );
	}

	// Absolute values per element
	inline Vector Vector::AbsPerElem( void ) const
	{
		return Vector(
				fabsf( x ),
				fabsf( y ),
				fabsf( z ),
				fabsf( w ));
	}
	
	inline u32 GetSignMask() const;

  union
  {
    struct
    {
      f32		x;
      f32		y;
      f32		z;
      f32		w;
    };
    struct
    {
      f32   f[4];
    };
  };
};

#include "VectorStandard.inl"

}}

#endif

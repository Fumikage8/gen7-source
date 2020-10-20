#ifndef GFLIB2_MATH_VECTOR4_H_INCLUDED
#define GFLIB2_MATH_VECTOR4_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_Vector3.h>

#if defined( GFL2_MAYA )
#include <maya/MVector.h>
#include <maya/Mf32Point.h>
#endif

namespace gfl2 { namespace math {

class Vector4
{
public:

	static const u32			ElementNum = 4;

	//! @brief デフォルトコンストラクタ
	inline Vector4( );
	//! @brief コピーコンストラクタ
	inline Vector4( const Vector4& other );
  inline Vector4( const Vector3& other );
#if defined( GFL2_MAYA )
	inline Vector4( const MVector& other );
#endif

	//! @brief f32×４からベクトルを作成
	inline Vector4( f32 x, f32 y, f32 z, f32 w = ((f32) 1.0) );

	//! 要素の設定
	inline Vector4&  Set( f32 x, f32 y, f32 z, f32 w = ((f32) 1.0) );

	inline Vector4&  Set( const Vector4 &v ){ *this = v; return *this; }

	inline Vector4& SetX( float x );                //!< X要素の設定
	inline float GetX( ) const;                    //!< X要素の取得
	inline Vector4& SetY( float y );                //!< Y要素の設定
	inline float GetY( ) const;                    //!< Y要素の取得
	inline Vector4& SetZ( float z );                //!< Z要素の設定
	inline float GetZ( ) const;                    //!< Z要素の取得
	inline Vector4& SetW( float w );                //!< W要素の設定
	inline Vector4& SetWZero();                     //!< W要素を0.0に設定
	inline Vector4& SetWOne();                      //!< W要素を1.0に設定
	inline float GetW( ) const;                    //!< W要素の取得

	inline Vector4&  operator =( const Vector4& rhs );
	inline Vector4&  operator =( const Vector3& rhs );
#if defined( GFL2_MAYA )
	inline Vector4&  operator =( const MVector& rhs );
#endif

	inline f32& operator []( int index );
	inline const f32& operator []( int index ) const;
	
	//! スカラ乗算
	inline Vector4 operator *( f32 rhs ) const;
	//! スカラ除算
	inline Vector4 operator /( f32 rhs ) const;
	
	//! スカラ乗算代入
	inline Vector4&  operator *=( f32 rhs );
	//! スカラ除算代入
	inline Vector4&  operator /=( f32 rhs );

	//! 加算
	inline Vector4 operator +( const Vector4& rhs ) const;
	//! 減算
	inline Vector4 operator -( const Vector4& rhs ) const;
	//! 加算代入
	inline Vector4&  operator +=( const Vector4& rhs );
	inline Vector4&  operator +=( const f32& rhs );
	//! 減算代入
	inline Vector4&  operator -=( const Vector4& rhs );
	inline Vector4&  operator -=( const f32& rhs );

	//! negate
	inline Vector4 operator -( ) const;

	//! 比較
	inline bool  operator ==( const Vector4& rhs ) const;
	inline bool  operator !=( const Vector4& rhs ) const;

	//!  ベクトル要素同士の最大値
	inline Vector4 MaxPerElem( const Vector4& rhs ) const;
	//!  ベクトル要素同士の最小値
	inline Vector4 MinPerElem( const Vector4& rhs ) const;
		
	//!  ベクトルの各要素内での最大値
	inline f32 MaxElem( ) const;
	//!  ベクトルの各要素内での最小値
	inline f32 MinElem( ) const;

	//!  ベクトルの各要素内での最大値
	inline f32 MaxElem3( ) const;
	//!  ベクトルの各要素内での最小値
	inline f32 MinElem3( ) const;

	//!  ベクトル内積
	inline f32 Dot( const Vector4& rhs ) const;
	inline f32 Dot3( const Vector4& rhs ) const;

	//!  ベクトル外積
	inline Vector4 Cross( const Vector4& rhs ) const;

	//!  ベクトルの長さ
	inline f32 Length( ) const;
	inline f32 Length3( ) const;

	//! @brief  ４次元ベクトルの正規化( 通常版 )
	//! @attention  ゼロベクトルを渡すとNaNが返ります。
	inline Vector4 Normalize( ) const;
	inline Vector4 Normalize3( ) const;

  /**
   * @brief 線形補完
   * @param v 補完するベクトル
   * @param t 補完係数 0.0 ~ 1.0
   */
  inline void Lerp(const Vector4& v, f32 t);

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
	static inline Vector4 GetXUnit( );
	//! Y軸単位ベクトル(0,1,0,0)の取得
	static inline Vector4 GetYUnit( );
	//! Z軸単位ベクトル(0,0,1,0)の取得
	static inline Vector4 GetZUnit( );
	//! W軸単位ベクトル(0,0,0,1)の取得
	static inline Vector4 GetWUnit( );
	//! ゼロベクトル(0,0,0,0)を取得
	static inline Vector4 GetZero( );

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

	inline void Neg( const Vector4& t ) {
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
	inline Vector4 GetBroadcastX() const
	{
		float value = x;
		return Vector4( value, value, value, value );
	}

	// Y要素をブロードキャストしたベクトル(y,y,y,y)を取得
	inline Vector4 GetBroadcastY() const
	{
		float value = y;
		return Vector4( value, value, value, value );
	}

	// Z要素をブロードキャストしたベクトル(z,z,z,z)を取得
	inline Vector4 GetBroadcastZ() const
	{
		float value = z;
		return Vector4( value, value, value, value );
	}

	// W要素をブロードキャストしたベクトル(w,w,w,w)を取得
	inline Vector4 GetBroadcastW() const
	{
		float value = w;
		return Vector4( value, value, value, value );
	}

	// Multiply element by element
	inline Vector4 Vector4::MulPerElem( const Vector4 &rhs ) const
	{
		return Vector4(
				x * rhs.x,
				y * rhs.y,
				z * rhs.z,
				w * rhs.w );
	}

	// divide element by element
	inline Vector4 Vector4::DivPerElem( const Vector4 &rhs ) const
	{
		return Vector4(
				x / rhs.x,
				y / rhs.y,
				z / rhs.z,
				w / rhs.w );
	}

	// Absolute values per element
	inline Vector4 Vector4::AbsPerElem( void ) const
	{
		return Vector4(
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

#include "Vector4Standard.inl"

}}

#endif

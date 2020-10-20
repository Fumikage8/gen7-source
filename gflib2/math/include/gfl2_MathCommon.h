#ifndef GFLIB2_MATH_COMMON_H_INCLUDED
#define GFLIB2_MATH_COMMON_H_INCLUDED

#include <math.h>
#if defined(GF_PLATFORM_CTR)
#include <nn/math.h>
#endif

namespace gfl2 { namespace math {

//----------------------------------------------------------------------------
//値定義

//! 円周率 π
#define PI  (3.14159265358979323846f)
#define PI2 (PI * 2.0f)

#define	EPSILON 1.0e-6f

#define GFL_VECTOR_X x
#define GFL_VECTOR_Y y
#define GFL_VECTOR_Z z
#define GFL_VECTOR_W w

#define GFL_MTX_ARRAY1D a
#define GFL_MTX_ARRAY2D m
#define GFL_MTX_VECTOR  v
#define GFL_MTX_00  _00
#define GFL_MTX_01  _01
#define GFL_MTX_02  _02
#define GFL_MTX_03  _03
#define GFL_MTX_10  _10
#define GFL_MTX_11  _11
#define GFL_MTX_12  _12
#define GFL_MTX_13  _13
#define GFL_MTX_20  _20
#define GFL_MTX_21  _21
#define GFL_MTX_22  _22
#define GFL_MTX_23  _23
#define GFL_MTX_30  _30
#define GFL_MTX_31  _31
#define GFL_MTX_32  _32
#define GFL_MTX_33  _33

//角度変換

//! degree角度→ラジアン変換
static inline f32 ConvDegToRad( f32 deg ) { return deg * PI / ((f32) 180.0); }
//! ラジアン→degree角度変換
static inline f32 ConvRadToDeg( f32 rad ) { return rad * ((f32) 180.0) / PI; }

/// ラジアン角度の正規化(-PI〜PIの値に変換する)
static inline f32 NormalizeRadianAngle( f32 radian ){
	const f32 div  = PI;
	const f32 div2 = PI * ((f32) 2.0);
	return radian - (s32)(((radian>=((f32)0.0))?(radian+div):(radian-div))/div2)*div2;
}

/// degree角度の正規化(-180〜180の値に変換する)
static inline f32 NormalizeDegreeAngle( f32 degree ){
	const f32 div  = ((f32) 180.0);
	const f32 div2 = ((f32) 360.0);
	return degree - (s32)(((degree>=((f32)0.0))?(degree+div):(degree-div))/div2)*div2;
}

static inline f32 FRecip( const f32 val ) {
		return 1.0f / val;
}

/**
	 *   浮動小数点絶対値
	 * @param  val 小数値
	 * @return 絶対値
	 */

	static inline f32 FAbs( const f32 val ) {
		return fabsf( val );
	}

/**
	 *   ほとんど０か
	 * @param  t1 浮動小数１
	 * @param  t2 浮動小数２
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	static inline bool IsAlmostZero( const f32 t1, const f32 epsilon = EPSILON ) {
		return FAbs( t1 ) <= epsilon;
	}

	/**
	 *   浮動小数点平方根
	 * @param  val 小数値
	 * @return 平方根
	 */

	static inline f32 FSqrt( const f32 val ) {
		return sqrtf( val );
	}

	/**
	 *   アークコサイン
	 * @param  val コサイン値
	 * @return アークコサイン値
	 */

	static inline f32 FACos( const f32 val ) {
		return acosf( val );
	}

	/**
	 *   サイン
	 * @param  val ラジアン
	 * @return サイン値
	 */

	static inline f32 FSin( const f32 val ) {
		return sinf( val );
	}

	/**
	 *   コサイン
	 * @param  val ラジアン
	 * @return コサイン値
	 */

	static inline f32 FCos( const f32 val ) {
		return cosf( val );
	}

	/**
	 *   成分が似ているか？
	 * @param  t1 浮動小数１
	 * @param  t2 浮動小数２
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	static inline bool IsSimilar( const f32 t1, const f32 t2, const f32 epsilon = EPSILON ) {
		return FAbs( t1 - t2 ) <= epsilon;
	}

	/**
	 * 切り上げ
	 * @param val　入力
	 * @return 切り上げられた値
	 */

	static inline f32 FCeil( const f32 val ) {
#if defined(GF_PLATFORM_CTR)
		return nn::math::FCeil( val );
#elif defined(GF_PLATFORM_WIN32)
		return ceilf( val );
#endif
	}

	/**
	 * 線形補間
	 * @param value0 t=0の時の値
   * @param value1 t=1の時の値
   * @param t 補間係数 0.0〜1.0
	 * @return 補間された値
	 */
  static inline f32 Lerp( f32 value0, f32 value1, f32 t )
  {
    return value0 + (value1 - value0) * t;
  }

//汎用演算
#if defined( max )
#error  "max がどこかでマクロ定義されている"
#endif
#if defined( min )
#error  "min がどこかでマクロ定義されている"
#endif
template < typename Type_ >
static inline const Type_& max( const Type_& left__, const Type_& right__ )   //!<@brief  ２値の大きい方を求める( template版 )
{
	return (left__ > right__)? left__: right__;
}
template < typename Type_ >
static inline const Type_& min( const Type_& left__, const Type_& right__ )   //!<@brief  ２値の小さい方を求める( template版 )
{
	return (left__ < right__)? left__: right__;
}
template < typename Type_ >
static inline const Type_& clamp( const Type_& value__, const Type_& min__, const Type_& max__ )   //!<@brief  値を範囲内にクランプ( template版 )
{
	return max( min( value__, max__ ), min__ );
}

}}


#endif

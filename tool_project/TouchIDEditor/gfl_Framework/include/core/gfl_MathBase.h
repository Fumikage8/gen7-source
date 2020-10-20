/*
 * File:   gfl_BaseMath.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 11:29
 */

#ifndef GFL_BASEMATH_H
#define	GFL_BASEMATH_H

#ifdef GFL_PLATFORM_3DS
#include <nn/math.h>
#else
#include <math.h>
#endif
// 算術アサートは、なし
#define GFL_MATH_ASSERT(...)

/**
 * 本当はプラットフォーム別だけど
 */
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

namespace gfl {
namespace core {

class Vector2;
class Vector3;
class Vector4;

#ifdef GFL_PLATFORM_3DS
typedef nn::math::VEC2_ VectorStruct2;
typedef nn::math::VEC3_ VectorStruct3;
typedef nn::math::VEC4_ VectorStruct4;
typedef nn::math::VEC2 VectorNative2;
typedef nn::math::VEC3 VectorNative3;
typedef nn::math::VEC4 VectorNative4;
#else
struct VectorStruct2;
struct VectorStruct3;
struct VectorStruct4;
class VectorNative2;
class VectorNative3;
class VectorNative4;
#endif

#if defined( GFL_PLATFORM_SONY )
typedef Vector4 Vector; /**< デフォルトベクトル型 */
typedef VectorNative4 VectorNative; /**< デフォルトネイティブベクトル型 */
#else
typedef Vector3 Vector; /**< デフォルトベクトル型 */
typedef VectorNative3 VectorNative; /**< デフォルトネイティブベクトル型 */
#endif

/**
 *  Tuple 配列の添え字
 * @remark Math の外に出しているのは文字列を短くするため
 */

enum TupleArray {
	TX, /**< Ｘの添え字 */
	TY, /**< Ｙの添え字 */
	TZ, /**< Ｚの添え字 */
	TXY = TZ, /**< ＸＹの要素数 */
	TW, /**< Ｗの添え字 */
	TXYZ = TW, /**< ＸＹＺの要素数 */
	TXYZW /**< ＸＹＺＷの要素数 */
};

/**
 *  座標、角度、スケール
 */

enum TypeArray {
	TRANS, /**< 座標の添え字 */
	ROT, /**< 角度の添え字 */
	TRANS_ROT,
	SCALE = TRANS_ROT, /**< スケールの添え字 */
	TRANS_ROT_SCALE /**< ３つの要素数 */
};

/**
 *  Rect 配列の添え字
 */

enum RectArray {
	RX, /**< Ｘの添え字 */
	RY, /**< Ｙの添え字 */
	RW, /**< Ｗの添え字 */
	RH, /**< Ｈの添え字 */
	RXYWH /**< ＸＹＷＨの要素数 */
};

/**
 *  算術クラス
 */

class Math {
public:
	// 整数
	static const u8 U8_MAX = 0xff; /**< @brief u8  で最大の値 */
	static const s8 S8_MAX = 0x7f; /**< @brief s8  で最大の値 */
	static const s8 S8_MIN = -S8_MAX - 1; /**< @brief s8  で最小の値 */
	static const u16 U16_MAX = 0xffff; /**< @brief u16 で最大の値 */
	static const s16 S16_MAX = 0x7fff; /**< @brief s16 で最大の値 */
	static const s16 S16_MIN = -S16_MAX - 1; /**< @brief s16 で最小の値 */
	static const u32 U32_MAX = 0xffffffff; /**< @brief u32 で最大の値 */
	static const s32 S32_MAX = 0x7fffffff; /**< @brief s32 で最大の値 */
	static const u64 U64_MAX = 0xffffffffffffffff; /**< @brief u64 で最大の値 */
	static const s64 S64_MAX = 0x7fffffffffffffff; /**< @brief s64 で最大の値 */
	static const s32 S32_MIN = -S32_MAX - 1; /**< @brief s32 で最小の値 */

#undef F32_MIN
#undef F32_MAX

	// 浮動小数
	static const f32 F32_ZERO; /**< @brief f32 0.0f */
	static const f32 F32_MIN; /**< @brief f32 で最小の値 */
	static const f32 F32_MAX; /**< @brief f32 で最大の値 */
	static const f32 EPSILON; /**< @brief ほとんどゼロに近い値 */

	// ラジアン
	static const f32 ONE_PI; /**< @brief 浮動小数点のパイ */
	static const f32 QUARTER_PI; /**< @brief 浮動小数点のパイ/4 */
	static const f32 HALF_PI; /**< @brief 浮動小数点のパイ/2 */
	static const f32 ONE_HALF_PI; /**< @brief 浮動小数点のパイ*1.5f */
	static const f32 TWO_PI; /**< @brief 浮動小数点の２パイ */

	// ルート
	static const f32 ROOT_2; /**< @brief ルート２ */
	static const f32 ROOT_3; /**< @brief ルート３ */

	/**
	 *   値にリミットをかける
	 * @param  v リミットをかけたい値
	 * @param  v_min 下限
	 * @param  v_max 上限
	 * @return 無し
	 */

	template<typename T, typename S>
	static inline void Clamp( T* v, const S v_min, const S v_max ) {
		if( v_max < *v ){
			*v = v_max;
		} else if( *v < v_min ){
			*v = v_min;
		}
	}

	/**
	 *   ±v_abs で値にリミットをかける
	 * @param  v リミットをかけたい値
	 * @param  v_abs リミットの絶対値
	 * @return 無し
	 * @remark Clamp(v,-v_abs,v_abs)
	 */

	template<typename T, typename S>
	static inline void ClampAbs( T* v, const S v_abs ) {
		Clamp( v, -v_abs, v_abs );
	}

	/**
	 *   ランダムの種を設定
	 * @param  seed 種値
	 * @return 無し
	 */

	static inline void RandomSeed( const s32 seed ) {
#ifdef GFL_PLATFORM_3DS
		GFL_AVOID_WARNING( seed );
#else
		srand( seed );
#endif
	}

	/**
	 *   ランダムの値を返す（浮動小数）
	 * @param  max 最大値
	 * @return ０ から max まで（max は含まない）のランダム浮動小数値
	 */

	static inline f32 Random( const f32 max ) {
#ifdef GFL_PLATFORM_3DS
		return max;
#else
		return (static_cast<f32> (rand( )) * max) * (1.0f / (static_cast<f32> (RAND_MAX) + 1.0f));
#endif
	}

	/**
	 *   ランダムの値を返す（浮動小数）
	 * @param  min 最小値
	 * @param  max 最大値
	 * @return min から max まで（max は含まない）のランダム浮動小数値
	 */

	static inline f32 Random( const f32 min, const f32 max ) {
		f32 df = max - min;
		if( 0.0f < df ){
			df *= Random( 1.0f );
		} else {
			df = 0.0f;
		}
		return min + df;
	}

	/**
	 *   ランダムの値を返す（整数）
	 * @param  max 最大値
	 * @return ０ から max まで（max は含まない）のランダム整数値
	 */

	template<typename T>
	static inline T Random( T max ) {
		return static_cast<T> (Random( static_cast<f32> (max) ));
	}

	/**
	 *   ランダムの値を返す（整数）
	 * @param  無し
	 * @return ０ から RAND_MAX までのランダム値
	 */

	static inline int Random( void ) {
#ifdef GFL_PLATFORM_3DS
		return 0;
#else
		return rand( );
#endif
	}

	/**
	 *   浮動小数点絶対値
	 * @param  val 小数値
	 * @return 絶対値
	 */

	static inline f32 FAbs( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FAbs( val );
#else
		return fabsf( val );
#endif
	}

	/**
	 *   整数絶対値
	 * @param  val 整数値
	 * @return 絶対値
	 */

	static inline s32 Abs( const s32 val ) {
		return ( 0 <= val) ? val : -val;
	}

	/**
	 *   浮動小数点平方根
	 * @param  val 小数値
	 * @return 平方根
	 */

	static inline f32 FSqrt( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FSqrt( val );
#else
		return sqrtf( val );
#endif
	}

	/**
	 *   浮動小数点逆数
	 * @param  val 小数値
	 * @return 逆数
	 */

	static inline f32 FRecip( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FInv( val );
#else
		return 1.0f / val;
#endif
	}

	/**
	 * 浮動小数の余りを取得する
	 * @param x　分子
	 * @param y　分母
	 * @return 余り
	 */
	static inline f32 FMod( const f32 x, const f32 y ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FMod( x, y );
#else
		return fmodf( x, y );
#endif
	}

	/**
	 * 浮動少数の整数部と小数部を分離
	 * @param x　入力
	 * @param y　整数部のポインタ
	 * @return 小数部
	 */
	static inline f32 FModf( const f32 x, f32* y ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FModf( x, y );
#else
		return modff( x, y );
#endif
	}

	/**
	 * 切り上げ
	 * @param val　入力
	 * @return 切り上げられた値
	 */

	static inline f32 FCeil( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FCeil( val );
#else
		return ceilf( val );
#endif
	}

	/**
	 * 切り捨て
	 * @param val　入力
	 * @return　切り捨てられた値
	 */
	static inline f32 FFloor( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FFloor( val );
#else
		return floorf( val );
#endif
	}

	/**
	 *   サイン
	 * @param  val ラジアン
	 * @return サイン値
	 */

	static inline f32 FSin( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::SinRad( val );
#else
		return sinf( val );
#endif
	}

	/**
	 *   コサイン
	 * @param  val ラジアン
	 * @return コサイン値
	 */

	static inline f32 FCos( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::CosRad( val );
#else
		return cosf( val );
#endif
	}

	/**
	 *   タンジェント
	 * @param  val ラジアン
	 * @return タンジェント値
	 */

	static inline f32 FTan( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::TanRad( val );
#else
		return tanf( val );
#endif
	}

	/**
	 *   サインコサイン
	 * @param  sin_val サイン値
	 * @param  cos_val コサイン値
	 * @param  val ラジアン
	 * @return 無し
	 */

	static inline void FSinCos( f32* sin_val, f32* cos_val, const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		nn::math::SinCosRad( sin_val, cos_val, val );
#else
		*sin_val = FSin( val );
		*cos_val = FCos( val );
#endif
	}

	/**
	 *   アークサイン
	 * @param  val サイン値
	 * @return アークサイン値
	 */

	static inline f32 FASin( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::AsinRad( val );
#else
		return asinf( val );
#endif
	}

	/**
	 *   アークコサイン
	 * @param  val コサイン値
	 * @return アークコサイン値
	 */

	static inline f32 FACos( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::AcosRad( val );
#else
		return acosf( val );
#endif
	}

	/**
	 *   アークタンジェント
	 * @param  val タンジェント値
	 * @return アークタンジェント値
	 */

	static inline f32 FATan( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::AtanRad( val );
#else
		return atanf( val );
#endif
	}

	/**
	 *   アークタンジェント２
	 * @param  x X
	 * @param  y Y
	 * @return アークタンジェント値
	 */

	static inline f32 FATan2( const f32 x, const f32 y ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::Atan2Rad( x, y );
#else
		return atan2f( x, y );
#endif
	}

	/**
	 * サインからコサインを求める
	 * @param sc　サイン
	 * @return コサイン
	 */
	static inline f32 SinToCos( const f32 sc ) {
		return FSqrt( 1.0f - sc * sc );
	}

	/**
	 * コサインからサインを求める
	 * @param sc　コサイン
	 * @return サイン
	 */
	static inline f32 CosToSin( const f32 sc ) {
		return SinToCos( sc );
	}

	/**
	 *   サインコサインを与えてＸＺ座標を回転されたＸ座標を得る
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @param  x Ｘ座標
	 * @param  z Ｚ座標
	 * @return 回転したＸ座標
	 */

	static inline f32 GetX_RotateY_SinCosXZ( const f32 s, const f32 c, const f32 x, const f32 z ) {
		return c * x + s * z;
	}

	static inline f32 GetX_RotateY_SinCosX( const f32, const f32 c, const f32 x ) {
		return c * x;
	}

	static inline f32 GetX_RotateY_SinCosZ( const f32 s, const f32, const f32 z ) {
		return s * z;
	}

	/**
	 *   サインコサインを与えてＸＺ座標を回転されたＺ座標を得る
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @param  x Ｘ座標
	 * @param  z Ｚ座標
	 * @return 回転したＺ座標
	 */

	static inline f32 GetZ_RotateY_SinCosXZ( const f32 s, const f32 c, const f32 x, const f32 z ) {
		return c * z - s * x;
	}

	static inline f32 GetZ_RotateY_SinCosX( const f32 s, const f32, const f32 x ) {
		return -s * x;
	}

	static inline f32 GetZ_RotateY_SinCosZ( const f32, const f32 c, const f32 z ) {
		return c * z;
	}

	/**
	 *   座標がＹ回転された座標を得る
	 * @param  xpos Ｘ座標
	 * @param  zpos Ｚ座標
	 * @param  yang Ｙ回転値（ラジアン）
	 * @return 無し
	 */
	static void GetXZRotateY( f32* xpos, f32* zpos, const f32 yang );

	/**
	 *   座標がＹ回転された座標を得る
	 * @param  xtag 出力Ｘ座標
	 * @param  ztag 出力Ｚ座標
	 * @param  xorg 入力Ｘ座標
	 * @param  zorg 入力Ｚ座標
	 * @param  yang Ｙ回転値（ラジアン）
	 * @return 無し
	 */
	static void GetXZRotateY( f32* xtag, f32* ztag, const f32 yang, const f32 xorg, const f32 zorg );
	static void AddXZRotateY( f32* xtag, f32* ztag, const f32 yang, const f32 xorg, const f32 zorg );
	static void AddXZRotateY( Vector* tag, const f32 yang, Vector& org );

	/**
	 *   座標がＹ回転された座標を得る
	 * @param  xtag 出力Ｘ座標
	 * @param  ztag 出力Ｚ座標
	 * @param  xorg 入力Ｘ座標
	 * @param  yang Ｙ回転値（ラジアン）
	 * @return 無し
	 */
	static void GetXZRotateY_X( f32* xtag, f32* ztag, const f32 yang, const f32 xorg );
	static void AddXZRotateY_X( f32* xtag, f32* ztag, const f32 yang, const f32 xorg );
	static void AddXZRotateY_X( Vector* tag, const f32 yang, const f32 xorg );

	/**
	 *   座標がＹ回転された座標を得る
	 * @param  xtag 出力Ｘ座標
	 * @param  ztag 出力Ｚ座標
	 * @param  zorg 入力Ｚ座標
	 * @param  yang Ｙ回転値（ラジアン）
	 * @return 無し
	 */
	static void GetXZRotateY_Z( f32* xtag, f32* ztag, const f32 yang, const f32 zorg );
	static void AddXZRotateY_Z( f32* xtag, f32* ztag, const f32 yang, const f32 zorg );
	static void AddXZRotateY_Z( Vector* tag, const f32 yang, const f32 zorg );

	static void GetXZRotateY_Z1( f32* xtag, f32* ztag, const f32 yang ) {
		FSinCos( xtag, ztag, yang );
	}

	/**
	 * 前方９０度以内にｘｚ座標が収まっているか
	 * @param  x X座標
	 * @param  z Z座標
	 * @return 真　前　偽　後左右
	 */

	static bool IsFrontDegree90( const f32 x, const f32 z ) {
		if( 0.0f < z ){
			if( 0.0f < x ){
				return ( x < z);
			} else {
				return ( -x < z);
			}
		}
		return false;
	}

	/**
	 * 後方９０度以内にｘｚ座標が収まっているか
	 * @param  x X座標
	 * @param  z Z座標
	 * @return 真　後　偽　前左右
	 */

	static bool IsBackDegree90( const f32 x, const f32 z ) {
		if( z < 0.0f ){
			if( x < 0.0f ){
				return ( z < x);
			} else {
				return ( z < -x);
			}
		}
		return false;
	}

	/**
	 * 左９０度以内にｘｚ座標が収まっているか
	 * @param  x X座標
	 * @param  z Z座標
	 * @return 真　左　偽　前後右
	 */
	static bool IsLeftDegree90( const f32 x, const f32 z ) {
		if( x < 0.0f ){
			if( 0.0f < z ){
				return ( z < x);
			} else {
				return ( -z < x);
			}
		}
		return false;
	}

	/**
	 * 右９０度以内にｘｚ座標が収まっているか
	 * @param  x X座標
	 * @param  z Z座標
	 * @return 真　右　偽　前後左
	 */
	static bool IsRightDegree90( const f32 x, const f32 z ) {
		if( 0.0f < x ){
			if( 0.0f < z ){
				return ( z < x);
			} else {
				return ( -z < x);
			}
		}
		return false;
	}

	/**
	 *   前後どちらか？
	 * @param  org 基準点
	 * @param  org_yang 基準点のY角度
	 * @param  tag 目標点
	 * @return 真　前　偽　後
	 */

	static bool IsPositionFront( const Vector& org, const f32 org_yang, const Vector& tag );
	/**
	 *   前後左右の前か？
	 * @param  org 基準点
	 * @param  org_yang 基準点のY角度
	 * @param  tag 目標点
	 * @return 真　前　偽　前じゃない
	 */

	static bool IsPositionFrontDegree90( const Vector& org, const f32 org_yang, const Vector& tag );

	/**
	 *   ほとんど０の値を０にする
	 * @param  val 値
	 * @return ０に近いときは０を、そうでないときはそのままの値
	 */

	static inline f32 AlmostZeroToZero( f32 val ) {
		if( Math::FAbs( val ) <= EPSILON ){
			val = 0.0f;
		}
		return val;
	}

	/**
	 *   角度を度数からラジアンに変換
	 * @param  x 度数
	 * @return ラジアン
	 */

	static inline f32 DegreeToRadian( const f32 x ) {
		return x * (ONE_PI / 180.0f);
	}

	/**
	 *   角度をラジアンから度数に変換
	 * @param  x ラジアン
	 * @return 度数
	 */

	static inline f32 RadianToDegree( const f32 x ) {
		return x * (180.0f / ONE_PI);
	}

	/**
	 *   ラジアンをーＰＩ〜ＰＩの範囲に収める
	 * @param  rad ラジアン
	 * @return ラジアン
	 */
	static f32 OptimizeRadian( f32 rad );

	/**
	 *   ラジアンを絶対値の小さいほうにする
	 * @param  vxp ラジアン
	 * @param  vyp ラジアン
	 * @param  vzp ラジアン
	 * @return ラジアン
	 */
	static void OptimizeRadian( f32* vxp, f32* vyp, f32* vzp );

	/**
	 *   ラジアンを目標値に徐々に近づける
	 * @param  cur 現在値
	 * @param  tag 目標値
	 * @param  rate 割合
	 * @return 補正値
	 */

	static f32 AdjustRadianGradually( const f32 cur, const f32 tag, const f32 rate ) {
		return gfl::core::Math::AddRadian( cur, gfl::core::Math::AddRadian( tag, -cur ) * rate );
	}

	/**
	 *   ラジアンを目標値に徐々に近づける
	 * @param  cur 現在値
	 * @param  tag 目標値
	 * @param  rate 割合
	 * @param  lim 差分の上限値
	 * @return 補正値
	 */

	static f32 AdjustRadianGraduallyWithLimit( const f32 cur, const f32 tag, const f32 rate, const f32 lim ) {
		f32 diff = gfl::core::Math::AddRadian( tag, -cur );
		diff *= rate;
		ClampAbs( &diff, lim );
		return gfl::core::Math::AddRadian( cur, diff );
	}

	/**
	 *   ラジアンを絶対値の小さいほうにする
	 * @param  vxp ラジアン
	 * @param  vyp ラジアン
	 * @param  vzp ラジアン
	 * @param  bx ラジアン
	 * @param  by ラジアン
	 * @param  bz ラジアン
	 * @return ラジアン
	 */
	static void SmoothRadian( f32* vxp, f32* vyp, f32* vzp, f32 bx, f32 by, f32 bz );

	/**
	 *   ラジアンの加算
	 * @param  rad ラジアン
	 * @param  add ラジアン
	 * @return ラジアン
	 * @remark 結果をーＰＩ〜ＰＩの範囲に収める。入力が範囲内でないと保障できない。
	 */
	static f32 AddRadian( f32 rad, f32 add );

	/**
	 *   ラジアンを step 段階に変換
	 * @param  val_f ラジアン
	 * @param  step 段階
	 * @return 段階
	 */
	static u32 RoundRadianStep( f32 val_f, const u32 step );

	/**
	 *   浮動小数点値 を step 段階の区切りがいい値にする
	 * @param  val  浮動小数点値
	 * @param  step 浮動小数点段階
	 * @return 浮動小数点値
	 */
	static f32 RoundFloat( f32 val, const f32 step );

	/**
	 *   浮動小数点値 を step 段階の区切りがいい値にする
	 * @param  val  浮動小数点値
	 * @return 浮動小数点値
	 */

	static f32 RoundFloat( f32 val ) {
		return FFloor( val + 0.5f );
	}

	/**
	 *   浮動小数点値 を 四捨五入する
	 * @param  val  浮動小数点値
	 * @param  range しきい値　デフォルトは 0.5f
	 * @return 浮動小数点値
	 * @remark range に 0.01f とかを入れると 1.0001f -> 1.0f, 1.1f -> 1.1f のように
	 * 浮動小数計算誤差を隠したい場合に使うと便利
	 */
	static f32 RoundOff( f32 val, const f32 range = 0.5f );

	/**
	 *   成分が似ているか？
	 * @param  t1 浮動小数１
	 * @param  t2 浮動小数２
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	static inline bool IsSimilar( const f32 t1, const f32 t2, const f32 epsilon = EPSILON ) {
		return Math::FAbs( t1 - t2 ) <= epsilon;
	}

	/**
	 *   ほとんど０か
	 * @param  t1 浮動小数１
	 * @param  t2 浮動小数２
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	static inline bool IsAlmostZero( const f32 t1, const f32 epsilon = EPSILON ) {
		return Math::FAbs( t1 ) <= epsilon;
	}

	/**
	 * 二乗の和を返す
	 * @param x　成分
	 * @param y　成分
	 * @param z　成分
	 * @return 二乗の和
	 */
	static inline f32 Magnitude2( const f32 x, const f32 y, const f32 z ) {
		return ( x * x + y * y + z * z);
	}

	/**
	 * 大きさを返す
	 * @param x　成分
	 * @param y　成分
	 * @param z　成分
	 * @return 　大きさ
	 */

	static inline f32 Magnitude( const f32 x, const f32 y, const f32 z ) {
		return Math::FSqrt( Magnitude2( x, y, z ) );
	}

	/**
	 *   スワップ関数
	 * @param  a スワップする変数またはクラス
	 * @param  b スワップする変数またはクラス
	 * @return 無し
	 */

	template <class T>
	static void Swap( T& a, T& b ) {
		T tmp = a;
		a = b;
		b = tmp;
	}

	/**
	 *   スワップ関数
	 * @param  a スワップする変数またはクラス 符号反転させて b に代入
	 * @param  b スワップする変数またはクラス b はそのまま a に代入
	 * @return 無し
	 */

	template <class T>
	static void SwapNeg( T& a, T& b ) {
		T tmp = -a;
		a = b;
		b = tmp;
	}

	/**
	 *   スワップ関数
	 * @param  a スワップする変数またはクラス 符号反転させて b に代入
	 * @param  b スワップする変数またはクラス 符号反転させて a に代入
	 * @return 無し
	 */

	template <class T>
	static void SwapNegNeg( T& a, T& b ) {
		T tmp = -a;
		a = -b;
		b = tmp;
	}

	template<typename T>
	static T Greater( const T l, const T r ) {
		return (l < r) ? r : l;
	}

	template<typename T>
	static T Lesser( const T l, const T r ) {
		return (l > r) ? r : l;
	}
};

}
}

#endif	/* GFL_BASEMATH_H */

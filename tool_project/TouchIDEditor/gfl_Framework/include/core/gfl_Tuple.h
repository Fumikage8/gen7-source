/* 
 * File:   gfl_Tuple.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_TUPLE_H
#define	GFL_TUPLE_H
#pragma once

//#if defined(GFL_PLATFORM_3DS) // 3DS にはない…
//#include <arm_neon.h>
//#endif

namespace gfl {
namespace core {

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================
template<typename T> struct TupleStr2;
template<typename T> class Tuple2;
class Tuple2f;

template<typename T> struct TupleStr3;
template<typename T> class Tuple3;
class Tuple3f;

template<typename T> struct TupleStr4;
template<typename T> class Tuple4;
class Tuple4f;

#ifdef GFL_PLATFORM_WII
typedef Tuple3f Tuple;
#else
/**
 *  デフォルトのタプル型
 */
typedef Tuple4f Tuple;
#endif

/**
 *  ２つの要素を持つタプル構造体
 */

template<typename T>
struct TupleStr2 {

	union {
		T _Array[TXY];

		struct {
			T _x;
			T _y;
		};
	};
};

/**
 *  ２つの要素を持つタプルテンプレートクラス
 */

template<typename T>
class Tuple2 : public TupleStr2<T> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple2( void ) {
	}

	/**
	 *   Ｘ，Ｙ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @return 無し
	 */

	Tuple2( const T x, const T y ) {
		Set( x, y );
	}

	/**
	 *   Tuple2 を設定するコンスタラクター
	 * @param  t Tuple2クラス
	 * @return 無し
	 */

	template<typename S>
	Tuple2( const S& t ) {
		Set( t );
	}

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T GetX( void ) const {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TY(2)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   変数のポインターを取得
	 * @param  無し
	 * @return 変数のポインター
	 */

	inline T* Get( void ) {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	inline const T* Get( void ) const {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TY(2)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T& GetRef( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を設定
	 * @param  x 設定値
	 * @return 無し
	 */

	inline void SetX( const T x ) {
		GFL_TEMPLATE_PARAMETER _x = x;
	}

	/**
	 *   Ｙ成分を設定
	 * @param  y 設定値
	 * @return 無し
	 */

	inline void SetY( const T y ) {
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   指定した成分を設定
	 * @param  i 添え字 TX(0) 〜 TY(2)
	 * @param  val 設定値
	 * @return 無し
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		GFL_TEMPLATE_PARAMETER _Array[i] = val;
	}

	/**
	 *   Ｘ，Ｙ成分を設定
	 * @param  x 設定値
	 * @param  y 設定値
	 * @return 無し
	 */

	inline void Set( const T x, const T y ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   Tuple2 を設定
	 * @param  t 設定したい Tuple の参照
	 * @return 無し
	 */

	template<typename S>
	inline void Set( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y = static_cast<T> (0);
	}

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	template<typename S>
	inline void Add( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	template<typename S, typename R>
	inline void Add( const S& t, const R& s ) {
		SetX( t._x + s._x );
		SetY( t._y + s._y );
	}

	/**
	 *   加算
	 * @param  x 加算したいＸ値
	 * @param  y 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const T x, const T y ) {
		GFL_TEMPLATE_PARAMETER _x += x;
		GFL_TEMPLATE_PARAMETER _y += y;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  x 加算したいＸ値
	 * @param  y 加算したいＹ値
	 * @return 無し
	 */

	template<typename S>
	inline void Add( const S& t, const T x, const T y ) {
		SetX( t._x + x );
		SetY( t._y + y );
	}

	// -------------------------------------------------------------------------
	// Sub
	// -------------------------------------------------------------------------

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	template<typename S>
	inline void Sub( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x -= t._x;
		GFL_TEMPLATE_PARAMETER _y -= t._y;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	template<typename S, typename R>
	inline void Sub( const S& t, const R& s ) {
		SetX( t._x - s._x );
		SetY( t._y - s._y );
	}

	/**
	 *   減算
	 * @param  x 減算したいＸ値
	 * @param  y 減算したいＹ値
	 * @return 無し
	 */

	inline void Sub( const T x, const T y ) {
		GFL_TEMPLATE_PARAMETER _x -= x;
		GFL_TEMPLATE_PARAMETER _y -= y;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  x 減算したいＸ値
	 * @param  y 減算したいＹ値
	 * @return 無し
	 */

	template<typename S>
	inline void Sub( const S& t, const T x, const T y ) {
		SetX( t._x - x );
		SetY( t._y - y );
	}

	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と各成分を乗算する
	 */

	inline void Mul( const T f ) {
		GFL_TEMPLATE_PARAMETER _x *= f;
		GFL_TEMPLATE_PARAMETER _y *= f;
	}

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	template<typename S>
	inline void Mul( const T f, const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	template<typename S>
	inline void Mul( const S& t, const T f ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	template<typename S>
	inline void Mul( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x *= t._x;
		GFL_TEMPLATE_PARAMETER _y *= t._y;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	template<typename S, typename R>
	inline void Mul( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x * s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y * s._y;
	}

	// -------------------------------------------------------------------------
	// Magnitude
	// -------------------------------------------------------------------------

	/**
	 *   大きさの二乗
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline f32 Magnitude2( void ) const {
		return (f32) (GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y);
	}

	/**
	 *   大きさ
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 Magnitude( void ) const {
		return Math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   大きさの逆数
	 * @param  無し
	 * @return 大きさの逆数
	 */

	inline f32 MagnitudeRecip( void ) const {
		return Math::FRecip( Magnitude( ) );
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */
	inline f32 Distance2( const Tuple2<T>& t ) const {
		Tuple2<T> r;
		r.Sub( *this, t );
		return r.Magnitude2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	template<typename S>
	inline f32 Distance( const S& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  無し
	 * @return 無し
	 */

	inline void Neg( void ) {
		GFL_TEMPLATE_PARAMETER _x = -GFL_TEMPLATE_PARAMETER _x;
		GFL_TEMPLATE_PARAMETER _y = -GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Neg( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = -t._x;
		GFL_TEMPLATE_PARAMETER _y = -t._y;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	template<typename S>
	inline T Dot( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y;
	}

	// -------------------------------------------------------------------------
	// Cross 二次元に外積はありません
	// -------------------------------------------------------------------------

	// -------------------------------------------------------------------------
	// Lerp
	// -------------------------------------------------------------------------

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  s 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	template<typename S, typename R>
	inline void Lerp( const S& t, const R& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_TEMPLATE_PARAMETER _x = t._x * rate_1 + s._x * rate;
		GFL_TEMPLATE_PARAMETER _y = t._y * rate_1 + s._y * rate;
	}

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	template<typename S>
	inline void Lerp( const S& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   オペレーター []
	 * @param  i 添え字 TX(0) 〜 TZ(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T operator [](s32 i) const {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [] must be TX or more than TX or TY or less than TY.\n" );
		return Get( i );
	}

	/**
	 *   オペレーター =
	 * @param  t 代入したい Tuple2<T>
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple2<T> & operator=(const S& t) {
		Set( t );
		return *this;
	}

	/**
	 *   オペレーター +=
	 * @param  t 加算したいクラス
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple2<T> & operator+=(const S& t) {
		Add( t );
		return *this;
	}

	/**
	 *   オペレーター -=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple2<T> & operator-=(const S& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   オペレーター *=
	 * @param  f 乗算したい値
	 * @return this の参照
	 */

	inline Tuple2<T> & operator*=(const T f) {
		Mul( f );
		return *this;
	}

	/**
	 *   オペレーター *=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple2<T> & operator*=(const S& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	template<typename S>
	inline bool operator==(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x == t._x && GFL_TEMPLATE_PARAMETER _y == t._y;
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	template<typename S>
	inline bool operator!=(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x != t._x || GFL_TEMPLATE_PARAMETER _y != t._y;
	}

	/**
	 *   オペレーター +
	 * @param  t 加算したいクラス
	 * @return 加算されたクラス Tuple2<T>
	 */

	template<typename S>
	inline Tuple2<T> operator+(const S& t) const {
		Tuple2<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス Tuple2<T>
	 */

	template<typename S>
	inline Tuple2<T> operator-(const S& t) const {
		Tuple2<T> r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   オペレーター *
	 * @param  f 積算したい値
	 * @return 積算されたクラス Tuple2<T>
	 */

	inline Tuple2<T> operator*(const T f) const {
		Tuple2<T> r;
		r.Mul( *this, f );
		return r;
	}

	/**
	 *   オペレーター *
	 * @param  t 積算したいクラス
	 * @return 積算されたクラス Tuple2<T>
	 */

	template<typename S>
	inline Tuple2<T> operator*(const S& t) const {
		Tuple2<T> r;
		r.Mul( *this, t );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s %d %d\n", str, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %d %d\n", str, i, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y );
	}
};

/**
 *  ２つの要素を持つタプルクラス
 */

class Tuple2f : public Tuple2<f32> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple2f( void ) {
	}

	/**
	 *   Ｘ，Ｙ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @return 無し
	 */

	Tuple2f( const f32 x, const f32 y ) : Tuple2<f32>::Tuple2( x, y ) {
	}

	/**
	 *   Tuple2f を設定するコンスタラクター
	 * @param  t Tuple2fクラス
	 * @return 無し
	 */

	template<typename S>
	Tuple2f( const S& t ) : Tuple2<f32>::Tuple2( t ) {
	}

	// -------------------------------------------------------------------------
	// Recip
	// -------------------------------------------------------------------------

	/**
	 *   逆数　Ｘ，Ｙ成分の逆数を設定する
	 * @param  無し
	 * @return 無し
	 */

	inline void Recip( void ) {
		SetX( Math::FRecip( _x ) );
		SetY( Math::FRecip( _y ) );
	}

	/**
	 *   逆数　ＴのＸ，Ｙ成分の逆数を設定する
	 * @param  t 逆数にしたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Recip( const S& t ) {
		SetX( Math::FRecip( t._x ) );
		SetY( Math::FRecip( t._y ) );
	}

	// -------------------------------------------------------------------------
	// Div
	// -------------------------------------------------------------------------

	/**
	 *   除算
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark 各成分を f で除算する
	 */

	inline void Div( const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		_x *= f_1;
		_y *= f_1;
	}

	/**
	 *   除算
	 * @param  t 除算したいクラス
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark t の各成分を f で除算する 内部的には逆数の掛け算
	 */

	template<typename S>
	inline void Div( const S& t, const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		SetX( f_1 * t._x );
		SetY( f_1 * t._y );
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   正規化
	 * @param  無し
	 * @return 無し
	 */

	inline void Normalize( void ) {
		Mul( MagnitudeRecip( ) );
	}

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Normalize( const S& t ) {
		Mul( t, MagnitudeRecip( ) );
	}

	/**
	 *   正規化
	 * @param  無し
	 * @return 元の大きさ
	 */

	inline f32 NormalizeRet( void ) {
		f32 ret = Magnitude( );
		Mul( Math::FRecip( ret ) );
		return ret;
	}

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 元の大きさ
	 */

	template<typename S>
	inline f32 NormalizeRet( const S& t ) {
		f32 ret = Magnitude( );
		Mul( t, Math::FRecip( ret ) );
		return ret;
	}

	// -------------------------------------------------------------------------
	// IsSimilar
	// -------------------------------------------------------------------------

	/**
	 *   各成分が似ているか？
	 * @param  t 対象クラス
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 各成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	template<typename S>
	inline bool IsSimilar( const S& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::FAbs( t._x - _x ) <= epsilon && Math::FAbs( t._y - _y ) <= epsilon;
	}

	// -------------------------------------------------------------------------
	// Absolute
	// -------------------------------------------------------------------------

	/**
	 *   各成分の絶対値を設定する
	 * @param  無し
	 * @return 無し
	 */

	inline void Abs( void ) {
		SetX( Math::FAbs( _x ) );
		SetY( Math::FAbs( _y ) );
	}

	/**
	 *   t の各成分の絶対値を設定する
	 * @param  t 入力クラス
	 * @return 無し
	 */

	template<typename S>
	inline void Abs( const S& t ) {
		SetX( Math::FAbs( t._x ) );
		SetY( Math::FAbs( t._y ) );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------
	using Tuple2<f32>::operator=;
	using Tuple2<f32>::operator+;
	using Tuple2<f32>::operator+=;
	using Tuple2<f32>::operator-;
	using Tuple2<f32>::operator-=;
	using Tuple2<f32>::operator*;
	using Tuple2<f32>::operator*=;

	/**
	 *   オペレーター /=
	 * @param  f 除算したい値
	 * @return this の参照
	 */

	inline Tuple2f & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   オペレーター /
	 * @param  f 除算したい値
	 * @return 除算されたクラス Tuple2f
	 */

	inline Tuple2f operator/(const f32 f) const {
		Tuple2f r;
		r.Div( *this, f );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s %f %f\n", str, _x, _y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %f %f\n", str, i, _x, _y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %f %f\n", str, i, j, _x, _y );
	}
};

/**
 *  ３つの要素を持つタプル構造体
 */

template<typename T>
struct TupleStr3 {
	// -------------------------------------------------------------------------
	// Element
	// -------------------------------------------------------------------------

	union {
		T _Array[TXYZ];

		struct {
			T mX;
			T mY;
			T mZ;
		};

		struct {
			T _x;
			T _y;
			T _z;
		};
	};
};

/**
 *  ３つの要素を持つタプルテンプレートクラス
 */

template<typename T>
class Tuple3 : public TupleStr3<T> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple3( void ) {
	}

	/**
	 *   Ｘ，Ｙ，Ｚ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @return 無し
	 */

	Tuple3( const T x, const T y, const T z ) {
		Set( x, y, z );
	}

	/**
	 *   Tuple3 を設定するコンスタラクター
	 * @param  t Tuple3クラス
	 * @return 無し
	 */

	Tuple3( const T* t ) {
		Set( t );
	}

	/**
	 *   Tuple3 を設定するコンスタラクター
	 * @param  t Tuple3クラス
	 * @return 無し
	 */

	Tuple3( T* t ) {
		Set( t );
	}

	/**
	 *   Tuple3 を設定するコンスタラクター
	 * @param  t Tuple3クラス
	 * @return 無し
	 */

	template<typename S>
	Tuple3( const S& t ) {
		Set( t );
	}

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T GetX( void ) const {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   Ｚ成分を取得
	 * @param  無し
	 * @return Ｚ成分
	 */

	inline T GetZ( void ) const {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TZ(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   Z成分を取得
	 * @param  無し
	 * @return Z成分
	 */

	inline T& GetRefZ( void ) {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TZ(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T& GetRef( const s32 i ) {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   変数のポインターを取得
	 * @param  無し
	 * @return 変数のポインター
	 */

	inline T* Get( void ) {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	inline const T* Get( void ) const {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を設定
	 * @param  x 設定値
	 * @return 無し
	 */

	inline void SetX( const T x ) {
		GFL_TEMPLATE_PARAMETER _x = x;
	}

	/**
	 *   Ｙ成分を設定
	 * @param  y 設定値
	 * @return 無し
	 */

	inline void SetY( const T y ) {
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   Ｚ成分を設定
	 * @param  z 設定値
	 * @return 無し
	 */

	inline void SetZ( const T z ) {
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   Ｗ成分を設定するふりをするダミー関数
	 * @param  T 設定値
	 * @return 無し
	 */

	inline void SetW( const T ) {
	}

	/**
	 *   指定した成分を設定
	 * @param  i 添え字 TX(0) 〜 TZ(3)
	 * @param  val 設定値
	 * @return 無し
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		GFL_TEMPLATE_PARAMETER _Array[i] = val;
	}

	/**
	 *   Ｘ，Ｙ，Ｚ成分を設定
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @return 無し
	 */

	inline void Set( const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   Ｘ，Ｙ，Ｚ成分を設定　Ｗはダミー
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @param  w ダミー値
	 * @return 無し
	 */

	inline void Set( const T x, const T y, const T z, const T w ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
		GFL_TEMPLATE_PARAMETER _z = z;
		GFL_AVOID_WARNING( w );
	}

	/**
	 *   Tuple2 を設定
	 * @param  t 設定したい Tuple2 の参照
	 * @param  z Ｚ成分の値 指定しない場合は 0 が入る
	 * @return 無し
	 */

	inline void Set( const Tuple2<T>& t, const T z = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   Tuple3 を設定
	 * @param  t 設定したい Tuple3 の参照
	 * @return 無し
	 */

	inline void Set( const f32* val ) {
		GFL_TEMPLATE_PARAMETER _x = val[TX];
		GFL_TEMPLATE_PARAMETER _y = val[TY];
		GFL_TEMPLATE_PARAMETER _z = val[TZ];
	}

	/**
	 *   Tuple3 を設定
	 * @param  t 設定したい Tuple3 の参照
	 * @return 無し
	 */

	inline void Set( f32* val ) {
		GFL_TEMPLATE_PARAMETER _x = val[TX];
		GFL_TEMPLATE_PARAMETER _y = val[TY];
		GFL_TEMPLATE_PARAMETER _z = val[TZ];
	}

	/**
	 *   Tuple3 を設定
	 * @param  t 設定したい Tuple3 の参照
	 * @return 無し
	 */

	template<typename S>
	inline void Set( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = t._z;
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y = GFL_TEMPLATE_PARAMETER _z = static_cast<T> (0);
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 * @remark Tuple3 の場合は SetZero と同じ
	 */

	inline void SetUnit( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y = GFL_TEMPLATE_PARAMETER _z = static_cast<T> (0);
	}

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	template<typename S>
	inline void Add( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
		GFL_TEMPLATE_PARAMETER _z += t._z;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	template<typename S, typename R>
	inline void Add( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y + s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z + s._z;
	}

	/**
	 *   加算
	 * @param  x 加算したいＸ値
	 * @param  y 加算したいＹ値
	 * @param  z 加算したいＺ値
	 * @return 無し
	 */

	inline void Add( const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x += x;
		GFL_TEMPLATE_PARAMETER _y += y;
		GFL_TEMPLATE_PARAMETER _z += z;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  x 加算したいＸ値
	 * @param  y 加算したいＹ値
	 * @param  z 加算したいＺ値
	 * @return 無し
	 */

	template<typename S>
	inline void Add( const S& t, const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + x;
		GFL_TEMPLATE_PARAMETER _y = t._y + y;
		GFL_TEMPLATE_PARAMETER _z = t._z + z;
	}

	// -------------------------------------------------------------------------
	// Sub
	// -------------------------------------------------------------------------

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	template<typename S>
	inline void Sub( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x -= t._x;
		GFL_TEMPLATE_PARAMETER _y -= t._y;
		GFL_TEMPLATE_PARAMETER _z -= t._z;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	template<typename S, typename R>
	inline void Sub( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y - s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z - s._z;
	}

	/**
	 *   減算
	 * @param  x 減算したいＸ値
	 * @param  y 減算したいＹ値
	 * @param  z 減算したいＺ値
	 * @return 無し
	 */

	inline void Sub( const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x -= x;
		GFL_TEMPLATE_PARAMETER _y -= y;
		GFL_TEMPLATE_PARAMETER _z -= z;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  x 減算したいＸ値
	 * @param  y 減算したいＹ値
	 * @param  z 減算したいＺ値
	 * @return 無し
	 */

	template<typename S>
	inline void Sub( const S& t, const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - x;
		GFL_TEMPLATE_PARAMETER _y = t._y - y;
		GFL_TEMPLATE_PARAMETER _z = t._z - z;
	}

	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と各成分を乗算する
	 */

	inline void Mul( const T f ) {
		GFL_TEMPLATE_PARAMETER _x *= f;
		GFL_TEMPLATE_PARAMETER _y *= f;
		GFL_TEMPLATE_PARAMETER _z *= f;
	}

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	template<typename S>
	inline void Mul( const T f, const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	template<typename S>
	inline void Mul( const S& t, const T f ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	template<typename S>
	inline void Mul( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x *= t._x;
		GFL_TEMPLATE_PARAMETER _y *= t._y;
		GFL_TEMPLATE_PARAMETER _z *= t._z;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	template<typename S, typename R>
	inline void Mul( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x * s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y * s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z * s._z;
	}

	// -------------------------------------------------------------------------
	// Magnitude
	// -------------------------------------------------------------------------

	/**
	 *   大きさの二乗 ＸＹＺのみでＷは計算しない
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline T MagnitudeXYZ2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y
			+ GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   大きさ ＸＹＺのみでＷは計算しない
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 MagnitudeXYZ( void ) const {
		return Math::FSqrt( MagnitudeXYZ2( ) );
	}

	/**
	 *   大きさの逆数 ＸＹＺのみでＷは計算しない
	 * @param  無し
	 * @return 大きさの逆数
	 */

	inline f32 MagnitudeXYZRecip( void ) const {
		return Math::FRecip( MagnitudeXYZ( ) );
	}

	/**
	 *   大きさの二乗
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline T Magnitude2( void ) const {
		return MagnitudeXYZ2( );
	}

	/**
	 *   大きさの二乗
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline T MagnitudeXZ2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z;
	}

	inline T MagnitudeXY2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   大きさ
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 Magnitude( void ) const {
		return Math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   大きさの逆数
	 * @param  無し
	 * @return 大きさの逆数
	 */

	inline f32 MagnitudeRecip( void ) const {
		const f32 len = Magnitude( );
		return (len != 0.0f) ? Math::FRecip( len ) : 0.0f;
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	template<typename S>
	inline T Distance2( const S& t ) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r.Magnitude2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	template<typename S>
	inline f32 Distance( const S& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	template<typename S>
	inline T DistanceXZ2( const S& t ) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r.MagnitudeXZ2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	template<typename S>
	inline f32 DistanceXZ( const S& t ) const {
		return Math::FSqrt( DistanceXZ2( t ) );
	}

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	template<typename S>
	inline T DistanceXY2( const S& t ) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r.MagnitudeXY2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	template<typename S>
	inline f32 DistanceXY( const S& t ) const {
		return Math::FSqrt( DistanceXY2( t ) );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  無し
	 * @return 無し
	 */

	inline void Neg( void ) {
		GFL_TEMPLATE_PARAMETER _x = -GFL_TEMPLATE_PARAMETER _x;
		GFL_TEMPLATE_PARAMETER _y = -GFL_TEMPLATE_PARAMETER _y;
		GFL_TEMPLATE_PARAMETER _z = -GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Neg( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = -t._x;
		GFL_TEMPLATE_PARAMETER _y = -t._y;
		GFL_TEMPLATE_PARAMETER _z = -t._z;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	template<typename S>
	inline T Dot( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	/**
	 *   内積XZ
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	template<typename S>
	inline T DotXZ( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	// -------------------------------------------------------------------------
	// Cross
	// -------------------------------------------------------------------------

	/**
	 *   外積
	 * @param  s 外積を求めたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Cross( const S& s ) {
		T x = GFL_TEMPLATE_PARAMETER _x;
		T y = GFL_TEMPLATE_PARAMETER _y;
		T z = GFL_TEMPLATE_PARAMETER _z;

		GFL_TEMPLATE_PARAMETER _x = y * s._z - z * s._y;
		GFL_TEMPLATE_PARAMETER _y = z * s._x - x * s._z;
		GFL_TEMPLATE_PARAMETER _z = x * s._y - y * s._x;
	}

	/**
	 *   外積
	 * @param  t 外積を求めたいクラス
	 * @param  s 外積を求めたいクラス
	 * @return 無し
	//				 * @remark t, s と this と違うクラスでないとアサートします
	 */

	template<typename S, typename R>
	inline void Cross( const S& t, const R& s ) {
		GFL_MATH_ASSERT( this != &t && this != &s, "Error !! Cross class Should be different.\n" );
		GFL_TEMPLATE_PARAMETER _x = t._y * s._z - t._z * s._y;
		GFL_TEMPLATE_PARAMETER _y = t._z * s._x - t._x * s._z;
		GFL_TEMPLATE_PARAMETER _z = t._x * s._y - t._y * s._x;
	}

	// -------------------------------------------------------------------------
	// Lerp
	// -------------------------------------------------------------------------

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  s 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	template<typename S, typename R>
	inline void Lerp( const S& t, const R& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_TEMPLATE_PARAMETER _x = t._x * rate_1 + s._x * rate;
		GFL_TEMPLATE_PARAMETER _y = t._y * rate_1 + s._y * rate;
		GFL_TEMPLATE_PARAMETER _z = t._z * rate_1 + s._z * rate;
	}

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	template<typename S>
	inline void Lerp( const S& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   オペレーター []
	 * @param  i 添え字 TX(0) 〜 TZ(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T operator [](s32 i) const {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [] must be TX or more than TX or TZ or less than TZ.\n" );
		return Get( i );
	}

	/**
	 *   オペレーター =
	 * @param  t 代入したい Tuple
	 * @return this の参照 Ｚ成分は 0 が入る
	 */

	inline Tuple3<T> & operator=(const Tuple2<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple3<T> & operator=(const Tuple3<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple3<T> & operator=(const Tuple4<T>& t) {
		Set( t );
		return *this;
	}

	/**
	 *   オペレーター +=
	 * @param  t 加算したいクラス
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple3<T> & operator+=(const S& t) {
		Add( t );
		return *this;
	}

	/**
	 *   オペレーター -=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple3<T> & operator-=(const S& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   オペレーター *=
	 * @param  f 乗算したい値
	 * @return this の参照
	 */

	inline Tuple3<T> & operator*=(const T f) {
		Mul( f );
		return *this;
	}

	/**
	 *   オペレーター *=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple3<T> & operator*=(const S& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   オペレーター ^=
	 * @param  t 外積を求めたいクラス
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple3<T> & operator^=(const S& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	template<typename S>
	inline bool operator==(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x == t._x && GFL_TEMPLATE_PARAMETER _y == t._y && GFL_TEMPLATE_PARAMETER _z == t._z;
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	template<typename S>
	inline bool operator!=(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x != t._x || GFL_TEMPLATE_PARAMETER _y != t._y || GFL_TEMPLATE_PARAMETER _z != t._z;
	}

	/**
	 *   オペレーター +
	 * @param  t 加算したいクラス
	 * @return 加算されたクラス Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator+(const S& t) const {
		Tuple3<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator-(const S& t) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   オペレーター *
	 * @param  f 積算したい値
	 * @return 積算されたクラス Tuple3<T>
	 */

	inline Tuple3<T> operator*(const T f) const {
		Tuple3<T> r;
		r.Mul( *this, f );
		return r;
	}

	/**
	 *   オペレーター *
	 * @param  t 積算したいクラス
	 * @return 積算されたクラス Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator*(const S& t) const {
		Tuple3<T> r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   オペレーター ^
	 * @param  t 外積を求めたいクラス
	 * @return 外積が設定されたクラス Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator^(const S& t) const {
		Tuple3<T> r;
		r.Cross( *this, t );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s %d %d %d\n", str, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %d %d %d\n", str, i, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %d %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z );
	}
};

/**
 *  ３つの要素を持つタプルクラス
 */

class Tuple3f : public Tuple3<f32> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple3f( void ) {
	}

	/**
	 *   Ｘ，Ｙ，Ｚ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @return 無し
	 */

	Tuple3f( const f32 x, const f32 y, const f32 z ) : Tuple3<f32>::Tuple3( x, y, z ) {
	}

	/**
	 *   Tuple3f を設定するコンスタラクター
	 * @param  t Tuple3fクラス
	 * @return 無し
	 */

	template<typename S>
	Tuple3f( const S& t ) : Tuple3<f32>::Tuple3( t ) {
	}

	// -------------------------------------------------------------------------
	// Recip
	// -------------------------------------------------------------------------

	/**
	 *   逆数　Ｘ，Ｙ，Ｚ成分の逆数を設定する
	 * @param  無し
	 * @return 無し
	 */

	inline void Recip( void ) {
		_x = Math::FRecip( _x );
		_y = Math::FRecip( _y );
		_z = Math::FRecip( _z );
	}

	/**
	 *   逆数　ＴのＸ，Ｙ，Ｚ成分の逆数を設定する
	 * @param  t 逆数にしたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Recip( const S& t ) {
		_x = Math::FRecip( t._x );
		_y = Math::FRecip( t._y );
		_z = Math::FRecip( t._z );
	}

	// -------------------------------------------------------------------------
	// Div
	// -------------------------------------------------------------------------

	/**
	 *   除算
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark 各成分を f で除算する
	 */

	inline void Div( const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x *= f_1;
		_y *= f_1;
		_z *= f_1;
	}

	/**
	 *   除算
	 * @param  t 除算したいクラス
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark t の各成分を f で除算する
	 */

	template<typename S>
	inline void Div( const S& t, const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x = f_1 * t._x;
		_y = f_1 * t._y;
		_z = f_1 * t._z;
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   正規化
	 * @param  無し
	 * @return 無し
	 */

	inline void Normalize( void ) {
		Mul( MagnitudeRecip( ) );
	}

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Normalize( const S& t ) {
		Mul( t, t.MagnitudeRecip( ) );
	}

	/**
	 *   正規化
	 * @param  無し
	 * @return 元の大きさ
	 */

	inline f32 NormalizeRet( void ) {
		const f32 ret = Magnitude( );
		Mul( Math::FRecip( ret ) );
		return ret;
	}

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 元の大きさ
	 */

	template<typename S>
	inline f32 NormalizeRet( const S& t ) {
		const f32 ret = t.Magnitude( );
		Mul( t, Math::FRecip( ret ) );
		return ret;
	}

	void RotateY( const f32 s, const f32 c ) {
		const f32 x = _x;
		const f32 z = _z;
		_x = gfl::core::Math::GetX_RotateY_SinCosXZ( s, c, x, z );
		_z = gfl::core::Math::GetZ_RotateY_SinCosXZ( s, c, x, z );
	}

	void RotateY( const f32 ang ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, ang );
		RotateY( s, c );
	}

	// -------------------------------------------------------------------------
	// IsSimilar
	// -------------------------------------------------------------------------

	/**
	 *   各成分が似ているか？
	 * @param  t 対象クラス
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 各成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	template<typename T>
	inline bool IsSimilar( const T& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( t._x, _x, epsilon ) &&
			Math::IsSimilar( t._y, _y, epsilon ) &&
			Math::IsSimilar( t._z, _z, epsilon );
	}

	inline bool IsSimilar( const f32 x, const f32 y, const f32 z, const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( x, _x, epsilon ) &&
			Math::IsSimilar( y, _y, epsilon ) &&
			Math::IsSimilar( z, _z, epsilon );
	}

	inline bool IsAlmostZero( const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( 0.0f, _x, epsilon ) &&
			Math::IsSimilar( 0.0f, _y, epsilon ) &&
			Math::IsSimilar( 0.0f, _z, epsilon );
	}

	inline bool IsNotAlmostZero( const f32 epsilon = Math::EPSILON ) const {
		return !Math::IsSimilar( 0.0f, _x, epsilon ) ||
			!Math::IsSimilar( 0.0f, _y, epsilon ) ||
			!Math::IsSimilar( 0.0f, _z, epsilon );
	}

	inline bool IsFrontDegree90( void ) {
		return gfl::core::Math::IsFrontDegree90( _x, _z );
	}

	// -------------------------------------------------------------------------
	// Absolute
	// -------------------------------------------------------------------------

	/**
	 *   各成分の絶対値を設定する
	 * @param  無し
	 * @return 無し
	 */

	inline void Abs( void ) {
		_x = Math::FAbs( _x );
		_y = Math::FAbs( _y );
		_z = Math::FAbs( _z );
	}

	/**
	 *   t の各成分の絶対値を設定する
	 * @param  t 絶対値をかけたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Abs( const S& t ) {
		_x = Math::FAbs( t._x );
		_y = Math::FAbs( t._y );
		_z = Math::FAbs( t._z );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------
	using Tuple3<f32>::operator=;
	using Tuple3<f32>::operator+;
	using Tuple3<f32>::operator+=;
	using Tuple3<f32>::operator-;
	using Tuple3<f32>::operator-=;
	using Tuple3<f32>::operator*;
	using Tuple3<f32>::operator*=;

	/**
	 *   オペレーター /=
	 * @param  f 除算したい値
	 * @return this の参照
	 */

	inline Tuple3f & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   オペレーター /
	 * @param  f 除算したい値
	 * @return 除算されたクラス Tuple3f
	 */

	inline Tuple3f operator/(const f32 f) {
		Tuple3f r;
		r.Div( *this, f );
		return r;
	}

#ifdef GFL_PLATFORM_3DS

	void FromVEC3( const gfl::math::VEC3& src ) {
		Set( src.x, src.y, src.z );
	}

	void FromVEC4( const gfl::math::VEC4& src ) {
		Set( src.x, src.y, src.z );
	}

	void ToVEC3( gfl::math::VEC3* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
	}

	void ToVEC4( gfl::math::VEC4* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
	}
#endif
	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   コンソールに各成分を出力
	 * @param  無し
	 * @return 無し
	 */

	inline void PrintConsole( void ) const {
		Debug::PrintConsole( " %ff, %ff, %ff\n", _x, _y, _z );
	}

	inline void PrintConsoleDegree( void ) const {
		Debug::PrintConsole( " %ff, %ff, %ff\n", gfl::core::Math::RadianToDegree( _x ), gfl::core::Math::RadianToDegree( _y ),
							 gfl::core::Math::RadianToDegree( _z ) );
	}

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s", str );
		PrintConsole( );
	}

	inline void PrintConsoleDegree( const c8 * const str ) const {
		Debug::PrintConsole( "%s", str );
		PrintConsoleDegree( );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d]", str, i );
		PrintConsole( );
	}

	inline void PrintConsoleDegree( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d]", str, i );
		PrintConsoleDegree( );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d]", str, i, j );
		PrintConsole( );
	}

	inline void PrintConsoleDegree( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d]", str, i, j );
		PrintConsoleDegree( );
	}

	static const TupleStr3<f32> sZero;

	static const TupleStr3<f32>& GetZero( void ) {
		return sZero;
	}

};

/**
 *  ４つの要素を持つタプル構造体　配列に代入して初期化したい時など
 */
#if defined(GFL_PLATFORM_WINDOWS)
#include <nmmintrin.h>   // MMX-SSE4.2命令セットを使用する場合インクルード //
#include <smmintrin.h>   // MMX-SSE4.1命令セットを使用する場合インクルード //
#include <intrin.h>      // MMX-SSE3命令セットを使用する場合インクルード   //
#include <emmintrin.h>   // MMX-SSE2命令セットを使用する場合インクルード   //
#include <xmmintrin.h>   // MMX-SSE命令セットを使用する場合インクルード    //
#include <mmintrin.h>    // MMX命令セットを使用する場合インクルード        //
#endif

template<typename T>
struct TupleStr4 {
	// -------------------------------------------------------------------------
	// Element
	// -------------------------------------------------------------------------

	union {
		//				f32 GFL_ATTR_ALIGN(_Array[TXYZW],16); /**< ベクターの各成分 */
		T _Array[TXYZW]; /**< ベクターの各成分 */

			struct {
			T mX;
			T mY;
			T mZ;
			T mW;
		};

		struct {
			T _x;
			T _y;
			T _z;
			T _w;
		};
#if defined(GFL_PLATFORM_3DS)
		//		float32x4_t mTuple4;
#elif defined(GFL_PLATFORM_WII)

		struct {
			u64 mXY;
			u64 mZW;
		};
#elif defined(GFL_PLATFORM_XENON)
		__vector4 mTuple4;
#elif defined(GFL_PLATFORM_PS3)
		vec_float4 mTuple4;
#elif defined(GFL_PLATFORM_WINDOWS)
		__m128 mTuple4;
#endif
	};
};

/**
 *  ４つの要素を持つタプルテンプレートクラス
 */

template<typename T>
class Tuple4 : public TupleStr4<T> {
public:

	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple4( void ) {
	}

	/**
	 *   Ｘ，Ｙ，Ｚ，Ｗ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @param  w 設定値 指定しない場合は 0 が入る
	 * @return 無し
	 */

	Tuple4( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		Set( x, y, z, w );
	}

	/**
	 *   Tuple4 を設定するコンスタラクター
	 * @param  t Tuple4クラス
	 * @return 無し
	 */

	template<typename S>
	Tuple4( const S& t ) {
		Set( t );
	}

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T GetX( void ) const {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   Ｚ成分を取得
	 * @param  無し
	 * @return Ｚ成分
	 */

	inline T GetZ( void ) const {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   Ｗ成分を取得
	 * @param  無し
	 * @return Ｗ成分
	 */

	inline T GetW( void ) const {
		return GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TW(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   Z成分を取得
	 * @param  無し
	 * @return Z成分
	 */

	inline T& GetRefZ( void ) {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   W成分を取得
	 * @param  無し
	 * @return W成分
	 */

	inline T& GetRefW( void ) {
		return GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TZ(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T& GetRef( const s32 i ) {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   変数のポインターを取得
	 * @param  無し
	 * @return 変数のポインター
	 */

	inline T* Get( void ) {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	inline const T* Get( void ) const {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を設定
	 * @param  x 設定値
	 * @return 無し
	 */

	inline void SetX( const T x ) {
		GFL_TEMPLATE_PARAMETER _x = x;
	}

	/**
	 *   Ｙ成分を設定
	 * @param  y 設定値
	 * @return 無し
	 */

	inline void SetY( const T y ) {
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   Ｚ成分を設定
	 * @param  z 設定値
	 * @return 無し
	 */

	inline void SetZ( const T z ) {
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   Ｗ成分を設定
	 * @param  w 設定値
	 * @return 無し
	 */

	inline void SetW( const T w ) {
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   指定した成分を設定
	 * @param  i 添え字 TX(0) 〜 TW(3)
	 * @param  val 設定値
	 * @return 無し
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		GFL_TEMPLATE_PARAMETER _Array[i] = val;
	}

	/**
	 *   Ｘ，Ｙ，Ｚ，Ｗ成分を設定
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @param  w 設定値 指定しない場合は 0.0f が入る
	 * @return 無し
	 */

	inline void Set( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
		GFL_TEMPLATE_PARAMETER _z = z;
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   ｆ３２配列を設定
	 * @param  a 配列の参照
	 * @return 無し
	 */

	inline void Set( const T& a ) {
		GFL_TEMPLATE_PARAMETER _x = (&a)[TX];
		GFL_TEMPLATE_PARAMETER _y = (&a)[TY];
		GFL_TEMPLATE_PARAMETER _z = (&a)[TZ];
		GFL_TEMPLATE_PARAMETER _w = (&a)[TW];
	}

	/**
	 *   Tuple2 を設定
	 * @param  t 設定したい Tuple2 の参照
	 * @param  z Ｚ成分の値 指定しない場合は 0 が入る
	 * @param  w Ｗ成分の値 指定しない場合は 0 が入る
	 * @return 無し
	 */

	inline void Set( const Tuple2<T>& t, const T z = static_cast<T> (0), const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = z;
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   Tuple3 を設定
	 * @param  t 設定したい Tuple3 の参照
	 * @param  w Ｗ成分の値 指定しない場合は 0 が入る
	 * @return 無し
	 */

	inline void Set( const Tuple3<T>& t, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = t._z;
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   Tuple4 を設定
	 * @param  t 設定したい Tuple4 の参照
	 * @return 無し
	 */

	template<typename S>
	inline void Set( const S& t ) {
#if 0 // def GFL_PLATFORM_WINDOWS
		mTuple4 = _mm_load_ps( (T*) & t );
#else
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = t._z;
		GFL_TEMPLATE_PARAMETER _w = t._w;
#endif
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y =
			GFL_TEMPLATE_PARAMETER _z = GFL_TEMPLATE_PARAMETER _w = static_cast<T> (0);
	}

	/**
	 *   Ｘ，Ｙ，Ｚ成分に０を設定しＷ成分に 1 を設定する
	 * @param  無し
	 * @return 無し
	 */

	inline void SetUnit( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y =
			GFL_TEMPLATE_PARAMETER _z = static_cast<T> (0);
		GFL_TEMPLATE_PARAMETER _w = static_cast<T> (1);
	}

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const Tuple4<T>& t ) {
#if 0 // def GFL_PLATFORM_3DS
		mTuple4 = vaddq_f32( mTuple4, t.mTuple4 );
#else
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
		GFL_TEMPLATE_PARAMETER _z += t._z;
		GFL_TEMPLATE_PARAMETER _w += t._w;
#endif
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const Tuple3<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
		GFL_TEMPLATE_PARAMETER _z += t._z;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const Tuple4<T>& t, const Tuple4<T>& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y + s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z + s._z;
		GFL_TEMPLATE_PARAMETER _w = t._w + s._w;
	}

	/**
	 *   加算
	 * @param  x 加算したいＸ値
	 * @param  y 加算したいＹ値
	 * @param  z 加算したいＺ値
	 * @param  w 加算したいＷ値 指定しない場合は 0.0f が入る
	 * @return 無し
	 */

	inline void Add( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x += x;
		GFL_TEMPLATE_PARAMETER _y += y;
		GFL_TEMPLATE_PARAMETER _z += z;
		GFL_TEMPLATE_PARAMETER _w += w;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  x 加算したいＸ値
	 * @param  y 加算したいＹ値
	 * @param  z 加算したいＺ値
	 * @param  w 加算したいＷ値 指定しない場合は 0.0f が入る
	 * @return 無し
	 */

	inline void Add( const Tuple4<T>& t, const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + x;
		GFL_TEMPLATE_PARAMETER _y = t._y + y;
		GFL_TEMPLATE_PARAMETER _z = t._z + z;
		GFL_TEMPLATE_PARAMETER _w = t._w + w;
	}

	// -------------------------------------------------------------------------
	// Sub
	// -------------------------------------------------------------------------

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const Tuple4<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x -= t._x;
		GFL_TEMPLATE_PARAMETER _y -= t._y;
		GFL_TEMPLATE_PARAMETER _z -= t._z;
		GFL_TEMPLATE_PARAMETER _w -= t._w;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const Tuple4<T>& t, const Tuple4<T>& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y - s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z - s._z;
		GFL_TEMPLATE_PARAMETER _w = t._w - s._w;
	}

	/**
	 *   減算
	 * @param  x 減算したいＸ値
	 * @param  y 減算したいＹ値
	 * @param  z 減算したいＺ値
	 * @param  w 減算したいＷ値 指定しない場合は 0.0f が入る
	 * @return 無し
	 */

	inline void Sub( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x -= x;
		GFL_TEMPLATE_PARAMETER _y -= y;
		GFL_TEMPLATE_PARAMETER _z -= z;
		GFL_TEMPLATE_PARAMETER _w -= w;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  x 減算したいＸ値
	 * @param  y 減算したいＹ値
	 * @param  z 減算したいＺ値
	 * @param  w 減算したいＷ値 指定しない場合は 0.0f が入る
	 * @return 無し
	 */

	inline void Sub( const Tuple4<T>& t, const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - x;
		GFL_TEMPLATE_PARAMETER _y = t._y - y;
		GFL_TEMPLATE_PARAMETER _z = t._z - z;
		GFL_TEMPLATE_PARAMETER _w = t._w - w;
	}

	// -------------------------------------------------------------------------
	// Scale
	// -------------------------------------------------------------------------

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と各成分を乗算する。Ｍｕｌにしていないのは継承で隠蔽されるのを避けるため。
	 */

	inline void Scale( const T f ) {
		GFL_TEMPLATE_PARAMETER _x *= f;
		GFL_TEMPLATE_PARAMETER _y *= f;
		GFL_TEMPLATE_PARAMETER _z *= f;
		GFL_TEMPLATE_PARAMETER _w *= f;
	}

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Scale( const T f, const Tuple4<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
		GFL_TEMPLATE_PARAMETER _w = f * t._w;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Scale( const Tuple4<T>& t, const T f ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
		GFL_TEMPLATE_PARAMETER _w = f * t._w;
	}

	// -------------------------------------------------------------------------
	// Magnitude
	// -------------------------------------------------------------------------

	/**
	 *   大きさの二乗 ＸＹＺのみでＷは計算しない
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline T MagnitudeXYZ2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y
			+ GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   大きさ ＸＹＺのみでＷは計算しない
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 MagnitudeXYZ( void ) const {
		return Math::FSqrt( MagnitudeXYZ2( ) );
	}

	/**
	 *   大きさの二乗の逆数 ＸＹＺのみでＷは計算しない
	 * @param  無し
	 * @return 大きさの二乗の逆数
	 */

	inline f32 MagnitudeXYZ2Recip( void ) const {
		return Math::FRecip( MagnitudeXYZ2( ) );
	}

	/**
	 *   大きさの逆数 ＸＹＺのみでＷは計算しない
	 * @param  無し
	 * @return 大きさの逆数
	 */

	inline f32 MagnitudeXYZRecip( void ) const {
		return Math::FRecip( MagnitudeXYZ( ) );
	}

	/**
	 *   大きさの二乗
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline T Magnitude2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y
			+ GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z + GFL_TEMPLATE_PARAMETER _w * GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   大きさ
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 Magnitude( void ) const {
		return Math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   大きさの二乗の逆数
	 * @param  無し
	 * @return 大きさの二乗の逆数
	 */

	inline f32 Magnitude2Recip( void ) const {
		return Math::FRecip( Magnitude2( ) );
	}

	/**
	 *   大きさの逆数
	 * @param  無し
	 * @return 大きさの逆数
	 */

	inline f32 MagnitudeRecip( void ) const {
		return Math::FRecip( Magnitude( ) );
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	template <typename S>
	inline T Distance2( const S& t ) const {
		Tuple4<T> r;
		r.Sub( *this, t );
		return r.MagnitudeXYZ2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 Distance( const Tuple4f& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  無し
	 * @return 無し
	 */

	inline void Neg( void ) {
		GFL_TEMPLATE_PARAMETER _x = -GFL_TEMPLATE_PARAMETER _x;
		GFL_TEMPLATE_PARAMETER _y = -GFL_TEMPLATE_PARAMETER _y;
		GFL_TEMPLATE_PARAMETER _z = -GFL_TEMPLATE_PARAMETER _z;
		GFL_TEMPLATE_PARAMETER _w = -GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */

	inline void Neg( const Tuple4<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x = -t._x;
		GFL_TEMPLATE_PARAMETER _y = -t._y;
		GFL_TEMPLATE_PARAMETER _z = -t._z;
		GFL_TEMPLATE_PARAMETER _w = -t._w;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   ＸＹＺ成分の内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	inline T DotXYZ( const Tuple4<T>& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	/**
	 *   ＸＺ成分の内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	template<typename S>
	inline T DotXZ( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	/**
	 *   内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	inline T Dot( const Tuple4<T>& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y + GFL_TEMPLATE_PARAMETER _z * t._z + GFL_TEMPLATE_PARAMETER _w * t._w;
	}

	// -------------------------------------------------------------------------
	// Cross
	// -------------------------------------------------------------------------

	/**
	 *   外積
	 * @param  t 外積を求めたいクラス
	 * @param  s 外積を求めたいクラス
	 * @return 無し
	 */

	template<typename S, typename R>
	inline void Cross( const S& t, const R& s ) {
		Set(
			 t._y * s._z - t._z * s._y,
			 t._z * s._x - t._x * s._z,
			 t._x * s._y - t._y * s._x );
	}

	/**
	 *   外積
	 * @param  s 外積を求めたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void Cross( const S& s ) {
		Cross( *this, s );
	}

	// -------------------------------------------------------------------------
	// Lerp
	// -------------------------------------------------------------------------

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  s 補完したいクラス
	 * @param  rate 内分値
	 * @return 無し
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	inline void Lerp( const Tuple4<T>& t, const Tuple4<T>& s, const f32 rate ) {
		f32 rate_1 = 1.0f - rate;
		Set( t._x * rate_1 + s._x * rate,
			 t._y * rate_1 + s._y * rate,
			 t._z * rate_1 + s._z * rate,
			 t._w * rate_1 + s._w * rate );
	}

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	inline void Lerp( const Tuple4<T>& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   オペレーター []
	 * @param  i 添え字 TX(0) 〜 TW(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T operator [](s32 i) const {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [] must be TX or more than TX or TW or less than TW.\n" );
		return Get( i );
	}
#if 0 // これじゃすまなかった…

	/**
	 *   オペレーター =
	 * @param  t 代入したいTuple
	 * @return this の参照
	 */

	template<typename S>
		inline Tuple4<T> & operator=(const S& t) {
		Set( t );
		return *this;
	}
#else

	/**
	 *   オペレーター =
	 * @param  t 代入したいTuple
	 * @return this の参照
	 */

	inline Tuple4<T> & operator=(const Tuple2<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple4<T> & operator=(const Tuple3<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple4<T> & operator=(const Tuple4<T>& t) {
		Set( t );
		return *this;
	}
#endif

	/**
	 *   オペレーター +
	 * @param  t 加算したいクラス
	 * @return 加算されたクラス Tuple4<T>
	 */

	inline Tuple4<T> operator+(const Tuple4<T>& t) const {
		Tuple4<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   オペレーター +=
	 * @param  t 加算したいクラス
	 * @return this の参照
	 */

	inline Tuple4<T> & operator+=(const Tuple4<T>& t) {
		Add( t );
		return *this;
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス Tuple4<T>
	 */

	inline Tuple4<T> operator-(const Tuple4<T>& t) const {
		Tuple4<T> r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   オペレーター -=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	inline Tuple4<T> & operator-=(const Tuple4<T>& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   オペレーター *
	 * @param  f 積算したい値
	 * @return 積算されたクラス Vector4
	 */

	inline Tuple4<T> operator*(const T f) const {
		Tuple4<T> r;
		r.Scale( *this, f );
		return r;
	}

	/**
	 *   オペレーター *=
	 * @param  f 乗算したい値
	 * @return this の参照
	 */

	inline Tuple4<T> & operator*=(const T f) {
		Scale( f );
		return *this;
	}

	/**
	 *   オペレーター ^=
	 * @param  t 外積を求めたいクラス
	 * @return this の参照
	 */

	inline Tuple4<T> & operator^=(const Tuple4<T>& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	inline bool operator==(const Tuple4<T>& t) const {
		return GFL_TEMPLATE_PARAMETER _x == t._x && GFL_TEMPLATE_PARAMETER _y == t._y && GFL_TEMPLATE_PARAMETER _z == t._z && GFL_TEMPLATE_PARAMETER _w == t._w;
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	inline bool operator!=(const Tuple4<T>& t) const {
		return GFL_TEMPLATE_PARAMETER _x != t._x || GFL_TEMPLATE_PARAMETER _y != t._y || GFL_TEMPLATE_PARAMETER _z != t._z || GFL_TEMPLATE_PARAMETER _w != t._w;
	}

	/**
	 *   オペレーター ^
	 * @param  t 外積を求めたいクラス
	 * @return 外積が設定されたクラス Tuple4<T>
	 */

	inline Tuple4<T> operator^(const Tuple4<T>& t) const {
		Tuple4<T> r;
		r.Cross( *this, t );
		return r;
	}

#ifdef GFL_PLATFORM_3DS

	void FromVEC3( const gfl::math::VEC3& src ) {
		Set( src.x, src.y, src.z );
	}

	void FromVEC4( const gfl::math::VEC4& src ) {
		Set( src.x, src.y, src.z, src.w );
	}

	void ToVEC3( gfl::math::VEC3* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
	}

	void ToVEC4( gfl::math::VEC4* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
		dst->w = GetW( );
	}
#endif
	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s = %d %d %d %d\n", str, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z, GFL_TEMPLATE_PARAMETER _w );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] = %d %d %d %d\n", str, i, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z, GFL_TEMPLATE_PARAMETER _w );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] = %d %d %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z, GFL_TEMPLATE_PARAMETER _w );
	}
};

/**
 *  ４つの要素を持つタプルクラス　Ｖｅｃ型と Ｑｕａ型の共通インターフェイスクラス
 */

class Tuple4f : public Tuple4<f32> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple4f( void ) {
	}

	/**
	 *   Ｘ，Ｙ，Ｚ，Ｗ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @param  w 設定値 指定しない場合は 0.0f が入る
	 * @return 無し
	 */

	Tuple4f( const f32 x, const f32 y, const f32 z, const f32 w = 0.0f ) : Tuple4<f32>::Tuple4( x, y, z, w ) {
	}

	/**
	 *   Tuple4 を設定するコンスタラクター
	 * @param  t Tuple4クラス
	 * @return 無し
	 */
	//		Tuple4f(const Tuple4<T>& t) : Tuple4<f32>::Tuple4(t) {}

	template<typename S>
	Tuple4f( const S& t ) : Tuple4<f32>::Tuple4( t ) {
	}

	static const TupleStr4<f32>& GetZero( void ) {
		return sZero;
	}

	static const TupleStr4<f32>& GetX1( void ) {
		return sX1;
	}

	static const TupleStr4<f32>& GetY1( void ) {
		return sY1;
	}

	static const TupleStr4<f32>& GetZ1( void ) {
		return sZ1;
	}

	static const TupleStr4<f32>& GetW1( void ) {
		return sW1;
	}

	// -------------------------------------------------------------------------
	// Recip
	// -------------------------------------------------------------------------

	/**
	 *   逆数　Ｘ，Ｙ，Ｚ成分の逆数を設定する
	 * @param  無し
	 * @return 無し
	 */

	inline void RecipXYZ( void ) {
		_x = Math::FRecip( _x );
		_y = Math::FRecip( _y );
		_z = Math::FRecip( _z );
	}

	/**
	 *   逆数　ＴのＸ，Ｙ，Ｚ成分の逆数を設定する
	 * @param  t 逆数にしたいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void RecipXYZ( const S& t ) {
		_x = Math::FRecip( t._x );
		_y = Math::FRecip( t._y );
		_z = Math::FRecip( t._z );
	}

	// -------------------------------------------------------------------------
	// Div
	// -------------------------------------------------------------------------

	/**
	 *   除算
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark 各成分を f で除算する
	 */

	inline void Div( const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x *= f_1;
		_y *= f_1;
		_z *= f_1;
		_w *= f_1;
	}

	/**
	 *   除算
	 * @param  t 除算したいクラス
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark t の各成分を f で除算する
	 */

	inline void Div( const Tuple4f& t, const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x = f_1 * t._x;
		_y = f_1 * t._y;
		_z = f_1 * t._z;
		_w = f_1 * t._w;
	}

	/**
	 *   Ｗ成分でＸ，Ｙ，Ｚ成分を除算
	 * @param  無し
	 * @return 無し
	 */

	inline void DivByW( void ) {
		const f32 f_1 = Math::FRecip( _w );
		_x *= f_1;
		_y *= f_1;
		_z *= f_1;
		_w = 1.0f;
	}

	/**
	 *   Ｗ成分でＸ，Ｙ，Ｚ成分を除算
	 * @param  t 除算したいクラス
	 * @return 無し
	 */

	template<typename S>
	inline void DivByW( const S& t ) {
		const f32 f_1 = Math::FRecip( _w );
		_x = f_1 * t._x;
		_y = f_1 * t._y;
		_z = f_1 * t._z;
		_w = 1.0f;
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   正規化
	 * @param  無し
	 * @return 無し
	 */

	inline void Normalize( void ) {
		Scale( MagnitudeRecip( ) );
	}

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 無し
	 */

	inline void Normalize( const Tuple4f& t ) {
		Scale( t, MagnitudeRecip( ) );
	}

	/**
	 *   正規化
	 * @param  無し
	 * @return 元の大きさ
	 */

	inline f32 NormalizeRet( void ) {
		const f32 ret = Magnitude( );
		Scale( Math::FRecip( ret ) );
		return ret;
	}

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 元の大きさ
	 */

	inline f32 NormalizeRet( const Tuple4f& t ) {
		const f32 ret = t.Magnitude( );
		Scale( t, Math::FRecip( ret ) );
		return ret;
	}

	// -------------------------------------------------------------------------
	// IsSimilar
	// -------------------------------------------------------------------------

	/**
	 *   誤差範囲を与えて比較
	 * @param  t 対象クラス
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 各成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	inline bool IsSimilar( const Tuple4f& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( t._x, _x, epsilon ) &&
			Math::IsSimilar( t._y, _y, epsilon ) &&
			Math::IsSimilar( t._z, _z, epsilon ) &&
			Math::IsSimilar( t._w, _w, epsilon );
	}

	// -------------------------------------------------------------------------
	// Abs
	// -------------------------------------------------------------------------

	/**
	 *   t の各成分の絶対値を設定する
	 * @param  t 絶対値をかけたいクラス
	 * @return 無し
	 */

	inline void Abs( const Tuple4f& t ) {
		Set( Math::FAbs( t._x ),
			 Math::FAbs( t._y ),
			 Math::FAbs( t._z ),
			 Math::FAbs( t._w ) );
	}

	/**
	 *   各成分の絶対値を設定する
	 * @param  無し
	 * @return 無し
	 */

	inline void Abs( void ) {
		Abs( *this );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------
	using Tuple4<f32>::operator=;
	using Tuple4<f32>::operator+;
	using Tuple4<f32>::operator+=;
	using Tuple4<f32>::operator-;
	using Tuple4<f32>::operator-=;
	using Tuple4<f32>::operator*;
	using Tuple4<f32>::operator*=;

	/**
	 *   オペレーター /=
	 * @param  f 除算したい値
	 * @return this の参照
	 */

	inline Tuple4f & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   オペレーター /
	 * @param  f 除算したい値
	 * @return 除算されたクラス Tuple4f
	 */

	inline Tuple4f operator/(const f32 f) {
		Tuple4f r;
		r.Div( *this, f );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s = %ff, %ff, %ff, %ff\n", str, _x, _y, _z, _w );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] = %ff, %ff, %ff, %ff\n", str, i, _x, _y, _z, _w );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] = %ff, %ff, %ff, %ff\n", str, i, j, _x, _y, _z, _w );
	}

	// -------------------------------------------------------------------------
	// static const parameters
	// -------------------------------------------------------------------------
	static const TupleStr4<f32> sZero;
	static const TupleStr4<f32> sX1;
	static const TupleStr4<f32> sY1;
	static const TupleStr4<f32> sZ1;
	static const TupleStr4<f32> sW1;

};

}
}

#endif	/* GFL_TUPLE_H */


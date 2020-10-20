﻿//==============================================================================
/**
 * @file	gfl_QuaternionNative.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/22, 15:36
 */
// =============================================================================

#if !defined( __GFL2_QUATERNIONNATIVE_H__ )
#define	__GFL2_QUATERNIONNATIVE_H__
#pragma once

#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_MathCommon.h>

namespace gfl2 { namespace math {

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================
struct QuaternionStruct;
class QuaternionNative;

/**
 *  ２つの要素を持つタプル構造体
 */

struct QuaternionStruct {
	f32 GFL_VECTOR_X;
	f32 GFL_VECTOR_Y;
	f32 GFL_VECTOR_Z;
	f32 GFL_VECTOR_W;
};

/**
 * ネイティブ命令クラス
 */

class QuaternionNative : public QuaternionStruct {
public:
    typedef QuaternionNative self_type; /**< 自分の型です */
    typedef f32  value_type; /**< 要素の型です */
	static const s32 DIMENSION = 4;

	/**
	 * デフォルトコンストラクタ　値は不定
	 */
	QuaternionNative( ) {
	}

	/**
	 * コンストラクタ
	 * @param vn　設定したいQuaternionNativeクラス
	 */
	QuaternionNative( const QuaternionNative& vn ) {
		Set(vn);
	}

	/**
	 * コンストラクタ
	 * @param vn　設定したいQuaternionNativeクラス
	 */
	QuaternionNative( const f32* vn ) {
		Set(*reinterpret_cast<const self_type*> (vn) );
	}

	/**
	 * コンストラクタ
	 * @param fx　設定したいX座標
	 * @param fy　設定したいY座標
	 * @param fz　設定したいZ座標
	 * @param fw　設定したいW座標
	 */
	QuaternionNative( f32 fx, f32 fy, f32 fz, f32 fw ) {
		GFL_VECTOR_X = fx;
		GFL_VECTOR_Y = fy;
		GFL_VECTOR_Z = fz;
		GFL_VECTOR_W = fw;
	}

	/**
	 * デストラクタ　何もしない
	 */
	~QuaternionNative( ) {
	}

	/**
	 * 浮動小数点のポインタを取得する
	 * @return 浮動小数点のポインタ
	 */
	inline operator f32*() {
		return &GFL_VECTOR_X;
	}

	/**
	 * 浮動小数点のポインタを取得する
	 * @return 浮動小数点のポインタ
	 */
	inline operator const f32*() const {
		return &GFL_VECTOR_X;
	}

	/**
	 *   オペレーター +=
	 * @param  t 加算したいクラス
	 * @return this の参照
	 */

	inline self_type & operator+=(const self_type& t) {
		Add( t );
		return *this;
	}

	/**
	 *   オペレーター -=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	inline self_type & operator-=(const self_type& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   オペレーター *=
	 * @param  f 乗算したい値
	 * @return this の参照
	 */

	inline self_type & operator*=(const f32 f) {
		Mul( f );
		return *this;
	}

	/**
	 *   オペレーター *=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	inline self_type & operator*=(const self_type& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   オペレーター /=
	 * @param  f 除算したい値
	 * @return this の参照
	 */

	inline self_type & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   オペレーター +
	 * @return 自分自身を返す。なんだこれ？
	 */

	inline self_type operator+() const {
		return *this;
	}

	/**
	 *   オペレーター +
	 * @param  t 加算したいクラス
	 * @return 加算されたクラス self_type
	 */

	inline self_type operator+(const self_type& t) const {
		self_type r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス self_type
	 * @remark W は反転しないかとおもったけど…仕方なくへんな変なライブラリに合わせる
	 */

	inline self_type operator-() const {
		return self_type( -GFL_VECTOR_X, -GFL_VECTOR_Y, -GFL_VECTOR_Z, -GFL_VECTOR_W );
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス self_type
	 */

	inline self_type operator-(const self_type& t) const {
		self_type r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   オペレーター *
	 * @param  f 積算したい値
	 * @return 積算されたクラス self_type
	 */

	inline self_type operator*(const f32 f) const {
		self_type r;
		r.Mul( *this, f );
		return r;
	}

	/**
	 *   オペレーター /
	 * @param  f 除算したい値
	 * @return 除算されたクラス Quaternion2f
	 */

	inline self_type operator/(const f32 f) const {
		self_type r;
		r.Div( *this, f );
		return r;
	}
	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	inline bool operator==(const self_type& t) const {
		return GFL_VECTOR_X == t.GFL_VECTOR_X && GFL_VECTOR_Y == t.GFL_VECTOR_Y
			&& GFL_VECTOR_Z == t.GFL_VECTOR_Z && GFL_VECTOR_W == t.GFL_VECTOR_W;
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	inline bool operator!=(const self_type& t) const {
		return GFL_VECTOR_X != t.GFL_VECTOR_X || GFL_VECTOR_Y != t.GFL_VECTOR_Y
			|| GFL_VECTOR_Z != t.GFL_VECTOR_Z || GFL_VECTOR_W != t.GFL_VECTOR_W;
	}

	/**
	 * 状態を出力
	 * @param newline 改行する
	 * @param str 表示文字列
	 */
	void Report( bool newline = true, const c8* str = 0 ) {
    (void)str;

		//Debug::PrintConsole( "%s { %ff, %ff, %ff, %ff },", str, GFL_VECTOR_X, GFL_VECTOR_Y, GFL_VECTOR_Z, GFL_VECTOR_W );
		if(newline){
			//Debug::PrintConsole( "\n" );
		}
	}

	// =========================================================================
	// ここから下は本来ない命令
	// =========================================================================
	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const self_type& t ) {
		GFL_VECTOR_X += t.GFL_VECTOR_X;
		GFL_VECTOR_Y += t.GFL_VECTOR_Y;
		GFL_VECTOR_Z += t.GFL_VECTOR_Z;
		GFL_VECTOR_W += t.GFL_VECTOR_W;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X + s.GFL_VECTOR_X, t.GFL_VECTOR_Y + s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z + s.GFL_VECTOR_Z, t.GFL_VECTOR_W + s.GFL_VECTOR_W );
	}

	/**
	 *   加算
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @param  tz 加算したいＺ値
	 * @return 無し
	 */

	inline void Add( const f32 tx, const f32 ty, const f32 tz, const f32 tw = 0.0f ) {
		GFL_VECTOR_X += tx;
		GFL_VECTOR_Y += ty;
		GFL_VECTOR_Z += tz;
		GFL_VECTOR_W += tw;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @param  tz 加算したいＺ値
	 * @return 無し
	 */

	inline void Add( const self_type& t, const f32 tx, const f32 ty, const f32 tz, const f32 tw ) {
		Set( t.GFL_VECTOR_X + tx, t.GFL_VECTOR_Y + ty, t.GFL_VECTOR_Z + tz, t.GFL_VECTOR_W + tw );
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

	inline void Sub( const self_type& t ) {
		GFL_VECTOR_X -= t.GFL_VECTOR_X;
		GFL_VECTOR_Y -= t.GFL_VECTOR_Y;
		GFL_VECTOR_Z -= t.GFL_VECTOR_Z;
		GFL_VECTOR_W -= t.GFL_VECTOR_W;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X - s.GFL_VECTOR_X, t.GFL_VECTOR_Y - s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z - s.GFL_VECTOR_Z, t.GFL_VECTOR_W - s.GFL_VECTOR_W );
	}

	/**
	 *   減算
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @param  tz 減算したいＺ値
	 * @return 無し
	 */

	inline void Sub( const f32 tx, const f32 ty, const f32 tz, const f32 tw = 0.0f ) {
		GFL_VECTOR_X -= tx;
		GFL_VECTOR_Y -= ty;
		GFL_VECTOR_Z -= tz;
		GFL_VECTOR_W -= tw;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @param  tz 減算したいＺ値
	 * @return 無し
	 */


	inline void Sub( const self_type& t, const f32 tx, const f32 ty, const f32 tz, const f32 tw = 0.0f ) {
		Set( t.GFL_VECTOR_X - tx, t.GFL_VECTOR_Y - ty, t.GFL_VECTOR_Z - tz, t.GFL_VECTOR_W - tw );
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

	inline void Mul( const f32 f ) {
		GFL_VECTOR_X *= f;
		GFL_VECTOR_Y *= f;
		GFL_VECTOR_Z *= f;
		GFL_VECTOR_W *= f;
	}

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Mul( const f32 f, const self_type& t ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y, f * t.GFL_VECTOR_Z, f * t.GFL_VECTOR_W );
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Mul( const self_type& t, const f32 f ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y, f * t.GFL_VECTOR_Z, f * t.GFL_VECTOR_W );
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const self_type& t ) {
		GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
		GFL_VECTOR_W *= t.GFL_VECTOR_W;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X, t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z, t.GFL_VECTOR_W * s.GFL_VECTOR_W );
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
		f32 f_1 = math::FRecip( f );
		GFL_VECTOR_X *= f_1;
		GFL_VECTOR_Y *= f_1;
		GFL_VECTOR_Z *= f_1;
		GFL_VECTOR_W *= f_1;
	}

	/**
	 *   除算
	 * @param  t 除算したいクラス
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark t の各成分を f で除算する 内部的には逆数の掛け算
	 */

	inline void Div( const self_type& t, const f32 f ) {
		f32 f_1 = math::FRecip( f );
		Set( f_1 * t.GFL_VECTOR_X, f_1 * t.GFL_VECTOR_Y, f_1 * t.GFL_VECTOR_Z, f_1 * t.GFL_VECTOR_W );
	}
	
private:
	void Set( f32 vx, f32 vy, f32 vz, f32 vw ) // 本当はない命令なので
	{
		GFL_VECTOR_X = vx;
		GFL_VECTOR_Y = vy;
		GFL_VECTOR_Z = vz;
		GFL_VECTOR_W = vw;
	}
	void Set( const self_type& vn ) // 本当はない命令なので
	{
		GFL_VECTOR_X = vn.GFL_VECTOR_X;
		GFL_VECTOR_Y = vn.GFL_VECTOR_Y;
		GFL_VECTOR_Z = vn.GFL_VECTOR_Z;
		GFL_VECTOR_W = vn.GFL_VECTOR_W;
	}
};

}}

#endif	/* __GFL_QUATERNIONNATIVE_H__ */


/* 
 * File:   gfl_Quaternion.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:51
 */

#ifndef GFL_QUATERNION_H
#define	GFL_QUATERNION_H

#pragma once
/**
 *  クォータニオンクラス
 */

namespace gfl {
namespace core {
//class Quaternion : public VectorTemplate<QuaternionNative> {

class Quaternion : public QuaternionNative {
public:
	typedef QuaternionNative parent_type;
	typedef Quaternion self_type;

#include "gfl_Tuple4Common.h"

	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	/**
	 *   デフォルトコンスタラクター
	 * @param  無し
	 * @return 無し
	 * @remark 何もしない
	 */

	Quaternion( void ) {
	}

	/**
	 *   Ｘ，Ｙ，Ｚ，Ｗ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  z 設定値
	 * @param  w 設定値 指定しない場合は 0.0f が入る
	 * @return 無し
	 */

	Quaternion( const f32 x, const f32 y, const f32 z, const f32 w ) {
		Set( x, y, z, w );
	}

	/**
	 *   Quaternion を設定するコンスタラクター
	 * @param  v Quaternionクラス
	 * @return 無し
	 */

	Quaternion( const Quaternion& v ) {
		Set( v );
	}

	Quaternion( const QuaternionNative& v ) {
		Set( v );
	}

	// -------------------------------------------------------------------------
	// Destruct !! not virtual for alignment
	// -------------------------------------------------------------------------

	~Quaternion( void ) {
	}

	//	using VectorTemplate<QuaternionNative>::Set;
	// -------------------------------------------------------------------------
	// Conjugate
	// -------------------------------------------------------------------------

	/**
	 *   共役
	 * @param  t 共役にしたいクラス
	 * @return 無し
	 */

	inline void Conj( const Quaternion& q ) {
		GFL_VECTOR_X = -q.GFL_VECTOR_X;
		GFL_VECTOR_Y = -q.GFL_VECTOR_Y;
		GFL_VECTOR_Z = -q.GFL_VECTOR_Z;
		GFL_VECTOR_W = q.GFL_VECTOR_W;
	}

	/**
	 *   共役
	 * @param  無し
	 * @return 無し
	 */

	inline void Conj( void ) {
		GFL_VECTOR_X = -GFL_VECTOR_X;
		GFL_VECTOR_Y = -GFL_VECTOR_Y;
		GFL_VECTOR_Z = -GFL_VECTOR_Z;
	}

	/**
	 *   共役
	 * @param  無し
	 * @return 無し
	 */

	inline Quaternion GetConj( void ) const {
		Quaternion ret;
		ret.Conj( *this );
		return ret;
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
		Conj( *this );
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */

	inline void Neg( const QuaternionNative& t ) {
		Conj( t );
	}

	inline Quaternion GetNeg( void ) {
		return GetConj( );
	}
	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------
	//	using VectorTemplate<QuaternionNative>::Mul;
#ifndef GFL_PLATFORM_3DS
	using QuaternionNative::Mul;
#endif
	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */
	void Mul( const QuaternionNative& t, const QuaternionNative& s );

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 */

	inline void Mul( const QuaternionNative& t ) {
		Mul( *this, t );
	}

	/**
	 *   内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	inline f32 Dot( const QuaternionNative& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Y * t.GFL_VECTOR_Y
			+ GFL_VECTOR_Z * t.GFL_VECTOR_Z + GFL_VECTOR_W * t.GFL_VECTOR_W;
	}

	// -------------------------------------------------------------------------
	// Inverse
	// -------------------------------------------------------------------------
	/**
	 *   Inverse
	 * @param  t Inverse したいクラス
	 * @return 無し
	 */
	void Inverse( const Quaternion& t );

	/**
	 *   Inverse
	 * @param  無し
	 * @return 無し
	 */

	inline void Inverse( void ) {
		Inverse( *this );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス self_type
	 * @remark W は反転しない…仕方なくへんな変なライブラリをオーバーロード
	 */

	inline self_type operator-() const {
		return self_type( -GFL_VECTOR_X, -GFL_VECTOR_Y, -GFL_VECTOR_Z, GFL_VECTOR_W );
	}

	// -------------------------------------------------------------------------
	// Slerp
	// -------------------------------------------------------------------------
	/**
	 *   補完
	 * @param  t 補完したいクラス
	 * @param  s 補完したいクラス
	 * @param  rate 内分値
	 * @return 無し
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	void Slerp( const Quaternion& t, const Quaternion& s, const f32 rate );

	/**
	 *   補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	inline void Slerp( const Quaternion& t, const f32 rate ) {
		Slerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// 変換
	// -------------------------------------------------------------------------
	/**
	 *   ラジアンからクォータニオンへ変換
	 * @param  x Ｘ軸角度
	 * @param  y Ｙ軸角度
	 * @param  z Ｚ軸角度
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い
	 */
	void RadianToQuaternion( const f32 x, const f32 y, const f32 z );
	void RadianYXZToQuaternion( const f32 x, const f32 y, const f32 z );

	/**
	 *   ラジアンからクォータニオンへ変換
	 * @param  v 入力ベクトル
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い
	 */

	inline void RadianToQuaternion( const Vector4& v ) {
		RadianToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	// -------------------------------------------------------------------------

	/**
	 *   ラジアンからクォータニオンへ変換
	 * @param  v 入力ベクトル
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い
	 */

	inline void RadianToQuaternion( const Vector3& v ) {
		RadianToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	inline void RadianYXZToQuaternion( const Vector3& v ) {
		RadianYXZToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	/**
	 *   角度Ｙからクォータニオンへ変換
	 * @param  yang Ｙ軸角度
	 * @return 無し
	 */
	void RadianYToQuaternion( const f32 yang );

	/**
	 *   クォータニオンからラジアンへ変換
	 * @param  x ラジアンＸ値の参照
	 * @param  y ラジアンＹ値の参照
	 * @param  z ラジアンＺ値の参照
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い。内部で正規化している。
	 */
	void QuaternionToRadian( f32* x, f32* y, f32* z ) const ;
	void QuaternionToRadianYXZ( f32* x, f32* y, f32* z ) const ;

	template <typename T>
	void QuaternionToRadianYXZ( T* v ) {
		QuaternionToRadianYXZ( &v->GetRefX( ), &v->GetRefY( ), &v->GetRefZ( ) );
	}
	f32 QuaternionToRadianY( void ) const;

	/**
	 *   クォータニオンからラジアンへ変換
	 * @param  v 入力クウォータニオン
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い
	 */

	template<typename T>
	inline void QuaternionToRadian( T* v ) const {
		QuaternionToRadian( &v->GetRefX( ), &v->GetRefY( ), &v->GetRefZ( ) );
	}

	/**
	 *   クォータニオンからラジアンへ変換
	 * @param  無し
	 * @return ラジアン値
	 * @remark 一度行列に変換しているので無駄が多い
	 */

	inline Vector QuaternionToRadian( void ) const {
		Vector ret;
		QuaternionToRadian( &ret.GetRefX( ), &ret.GetRefY( ), &ret.GetRefZ( ) );
		return ret;
	}

	/**
	 *   行列からクォータニオンへ変換
	 * @param  m 入力行列
	 * @return 無し
	 */

	void MatrixToQuaternion( const Matrix& m ) {
		Set( m.MatrixToQuaternion( ) );
	}

	/**
	 *   クォータニオンから行列へ変換
	 * @param  m 出力行列
	 * @return 無し
	 */

	template<typename T>
	void QuaternionToMatrix( T* m ) const {
		m->QuaternionToMatrix( *this );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	using QuaternionNative::operator*=;
	using QuaternionNative::operator*;

	/**
	 *   オペレーター *=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	inline Quaternion & operator*=(const Quaternion& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   オペレーター *
	 * @param  t 積算したいクラス
	 * @return 積算されたクラス Quaternion
	 */

	inline Quaternion operator*(const Quaternion& t) const {
		Quaternion r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const Quaternion& t ) {
		Quaternion r;
		r.Conj( t );
		Mul( r, *this );
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 相対Ｑｕａｔｅｒｎｉｏｎを設定
	 */

	inline void Sub( const Quaternion& t, const Quaternion& s ) {
		Conj( s );
		Mul( t );
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
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Y * GFL_VECTOR_Y +
			GFL_VECTOR_Z * GFL_VECTOR_Z + GFL_VECTOR_W*GFL_VECTOR_W;
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
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   正規化
	 * @param  無し
	 * @return 無し
	 */

	inline void Normalize( void ) {
		Mul( Math::FRecip( Magnitude( ) ) );
	}

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 無し
	 */

	inline void Normalize( const QuaternionNative& t ) {
		Set( t );
		Normalize( );
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

	inline f32 NormalizeRet( const QuaternionNative& t ) {
		Set( t );
		return NormalizeRet( );
	}

	/**
	 *   オペレーター -=
	 * @param  t 減算したいクラス
	 * @return this の参照
	 */

	inline Quaternion & operator-=(const Quaternion& t) {
		Quaternion r;
		r.Conj( t );
		Mul( r, *this );
		return *this;
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス Quaternion
	 */

	inline Quaternion operator-(const Quaternion& t) const {
		Quaternion r;
		r.Sub( *this, t );
		return r;
	}
};
}
}

#endif	/* GFL_QUATERNION_H */


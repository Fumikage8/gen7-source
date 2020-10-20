/* 
 * File:   gfl2_Quaternion.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:51
 */

#ifndef GFL2_QUATERNION_H
#define	GFL2_QUATERNION_H

#pragma once

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Matrix34.h>

/**
 *  クォータニオンクラス
 */

namespace gfl2 { namespace math {
//class Quaternion : public VectorTemplate<QuaternionNative> {

class Quaternion : public QuaternionNative {
public:
	typedef QuaternionNative parent_type;
	typedef Quaternion self_type;

#include "gfl2_Tuple4Common.h"

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
	 *   任意軸回転のコンストラクター
	 * @param  axis 回転軸
	 * @param  angleRad 回転量（ラジアン）
	 * @return 無し
	 */

	Quaternion( const Vector3& axis, f32 angleRad ) {
    Vector3 n = axis.Normalize();
    f32 halfAngle = angleRad * 0.5f;
    f32 cosValue = cos(halfAngle);
    f32 sinValue = sin(halfAngle);

    Set( n.x * sinValue, n.y * sinValue, n.z * sinValue, cosValue );
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
	void Mul( const QuaternionNative& t, const QuaternionNative& s )
	{
		Set( t.GFL_VECTOR_W * s.GFL_VECTOR_X + t.GFL_VECTOR_X * s.GFL_VECTOR_W + t.GFL_VECTOR_Y * s.GFL_VECTOR_Z - t.GFL_VECTOR_Z * s.GFL_VECTOR_Y,
		 t.GFL_VECTOR_W * s.GFL_VECTOR_Y + t.GFL_VECTOR_Y * s.GFL_VECTOR_W + t.GFL_VECTOR_Z * s.GFL_VECTOR_X - t.GFL_VECTOR_X * s.GFL_VECTOR_Z,
		 t.GFL_VECTOR_W * s.GFL_VECTOR_Z + t.GFL_VECTOR_Z * s.GFL_VECTOR_W + t.GFL_VECTOR_X * s.GFL_VECTOR_Y - t.GFL_VECTOR_Y * s.GFL_VECTOR_X,
		 t.GFL_VECTOR_W * s.GFL_VECTOR_W - t.GFL_VECTOR_X * s.GFL_VECTOR_X - t.GFL_VECTOR_Y * s.GFL_VECTOR_Y - t.GFL_VECTOR_Z * s.GFL_VECTOR_Z );
	}

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
	void Inverse( const Quaternion& t )
	{
		f32 norm = t.Magnitude2Recip( );
		f32 norm_neg = -norm;
		Set( t.GFL_VECTOR_X * norm_neg,
			 t.GFL_VECTOR_Y * norm_neg,
			 t.GFL_VECTOR_Z * norm_neg,
			 t.GFL_VECTOR_W * norm );
	}

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

	inline void Slerp( const Quaternion& t, const Quaternion& s, const f32 rate );

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
	void RadianToQuaternion( const f32 x, const f32 y, const f32 z )
	{
		Matrix34 mat = math::Matrix34::GetRotationZYX(x, y, z);
		this->MatrixToQuaternion(mat);
	}
	void RadianYXZToQuaternion( const f32 x, const f32 y, const f32 z )
	{
		Matrix34 matY = Matrix34::GetRotationY(y);
		Matrix34 matX = Matrix34::GetRotationX(x);
		Matrix34 matZ = Matrix34::GetRotationZ(z);
		Matrix34 rotMatrix = matY * matX * matZ;
		this->MatrixToQuaternion(rotMatrix);
	}

	/**
	 *   ラジアンからクォータニオンへ変換
	 * @param  v 入力ベクトル
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い
	 */

	inline void RadianToQuaternion( const Vector& v ) {
		RadianToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	// -------------------------------------------------------------------------

	/**
	 *   ラジアンからクォータニオンへ変換
	 * @param  v 入力ベクトル
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い
	 */

	inline void RadianYXZToQuaternion( const Vector& v ) {
		RadianYXZToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	/**
	 *   角度Ｙからクォータニオンへ変換
	 * @param  yang Ｙ軸角度
	 * @return 無し
	 */
	void RadianYToQuaternion( const f32 yang )
	{
		f32 c, s;

		s = math::FSin(yang);
		c = math::FCos(yang);
		SetX( 0.0f );
		c *= 2.0f;
		SetZ( 0.0f );

		const f32 tr = 1.0f + c;
		f32 ww;

		if( tr >= 0.0f ){
			ww = math::FSqrt( tr + 1.0f );
			SetW( ww * 0.5f );
			SetY( s / ww );
		} else {
			ww = math::FSqrt( 2.0f - c );
			SetY( ww * 0.5f );
			SetW( s / ww );
		}
	}

	/**
	 *   クォータニオンからラジアンへ変換
	 * @param  x ラジアンＸ値の参照
	 * @param  y ラジアンＹ値の参照
	 * @param  z ラジアンＺ値の参照
	 * @return 無し
	 * @remark 一度行列に変換しているので無駄が多い。内部で正規化している。
	 */
	void QuaternionToRadian( f32* x_ang, f32* y_ang, f32* z_ang ) const 
	{
#if 0 //
		Matrix44AOS mat;
		mat.QuaternionToMatrix( *this );
		mat.GetRadian( x, y, z );
#else
		Quaternion qt;
		const f32 scl = qt.NormalizeRet( *this );

		const f32 x = qt.GetX( );
		const f32 y = qt.GetY( );
		const f32 z = qt.GetZ( );
		const f32 w = qt.GetW( );

		const f32 xz = x * z;
		const f32 wy = w * y;

		const f32 xx = x * x;
		const f32 xy = x * y;
		const f32 zz = z * z;
		const f32 wz = w * z;

		const f32 sy = 2.0f * ( xz - wy );
		if( 1.0f - EPSILON <= sy ){
			*x_ang = atan2( -2.0f * ( xy - wz ), 1.0f - 2.0f * ( xx + zz ) );
			*y_ang = -PI / 2.0f;
			*z_ang = 0.0f;
		} else if( sy <= -1.0f+ EPSILON ){
			*x_ang = atan2( 2.0f * ( xy - wz ), 1.0f - 2.0f * ( xx + zz ) );
			*y_ang = PI / 2.0f;
			*z_ang = 0.0f;
		} else {
			const f32 yy = y * y;
			*x_ang = atan2( 2.0f * ( y * z + w * x ), 1.0f - 2.0f * ( xx + yy ) );
			*y_ang = asinf( -sy / scl );
			*z_ang = atan2( 2.0f * ( xy + wz ), 1.0f - 2.0f * ( yy + zz ) );
		}
#endif
	}
	void QuaternionToRadianYXZ( f32* x, f32* y, f32* z ) const 
	{
		const f32 gx = GetX( );
		const f32 gy = GetY( );
		const f32 gz = GetZ( );
		const f32 gw = GetW( );

		// roll
		*x = atan2( 2.0f * ( gx * gy + gw + gz ), gw * gw + gx * gx - gy * gy - gz * gz );
		// pitch
		*y = atan2( 2.0f * ( gy * gz + gw * gx ), gw * gw - gx * gx - gy * gy + gz * gz );
		// yaw
		*z = asin( -2.0f * (gx * gz - gw * gy) );
	}

	template <typename T>
	void QuaternionToRadianYXZ( T* v ) {
		QuaternionToRadianYXZ( &v->GetRefX( ), &v->GetRefY( ), &v->GetRefZ( ) );
	}

	f32 QuaternionToRadianY( void ) const
	{
		const f32 gx = GetX( );
		const f32 gy = GetY( );
		const f32 gz = GetZ( );
		const f32 gw = GetW( );

		return atan2( 2.0f * ( gy * gz + gw * gx ), gw * gw - gx * gx - gy * gy + gz * gz );
	}

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

	inline void MatrixToQuaternion( Matrix mat );
	inline void MatrixToQuaternion( const Matrix34& mat );

	/**
	 *   クォータニオンから行列へ変換
	 * @param  m 出力行列
	 * @return 無し
	 */

	inline void QuaternionToMatrix( Matrix &mat ) const;
	inline void QuaternionToMatrix( Matrix34 &mat ) const;

    /**
     * ベクトルの変換
     * @param rhs 入力ベクトル
     * @return 変換後のベクトル
     */
    inline Vector3 Transform( const Vector3& rhs ) const;
    inline Vector4 Transform( const Vector4& rhs ) const;

    /**
     * スケールつきベクトル変換
     * @param rhs 入力ベクトル
     * @param scale スケールベクトル
     * @return 変換後のベクトル
     */
    inline Vector3 TransformWithScale( const Vector3& rhs, const Vector3& scale ) const;

	/**
	 *   対数クォータニオンから通常のクォータニオンへ変換
	 */
  void ConvExp()
  {
    f32 len = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);

    // 単位クォータニオンの判定
    if (len == 0.0f)
    {
      this->x = 0.0f;
      this->y = 0.0f;
      this->z = 0.0f;
      this->w = 1.0f;
      return;
    }

    // それ以外
    f32 theta = sqrtf(len);
    f32 sinTheta = sinf(theta);
    f32 s = sinTheta / theta;

    this->x *= s;
    this->y *= s;
    this->z *= s;
    this->w = cosf(theta);
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
		return math::FRecip( Magnitude2( ) );
	}

	/**
	 *   大きさ
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 Magnitude( void ) const {
		return math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   大きさの逆数
	 * @param  無し
	 * @return 大きさの逆数
	 */

	inline f32 MagnitudeRecip( void ) const {
		return math::FRecip( Magnitude( ) );
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
		Mul( math::FRecip( Magnitude( ) ) );
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
		Mul( math::FRecip( ret ) );
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

// -----------------------------------------------------------------
// Slerp
// -----------------------------------------------------------------
inline void Quaternion::Slerp( const Quaternion& t, const Quaternion& s, const f32 rate )
{
  // 0.0fや1.0fの時にtやsと値が変わっていたのでif文で判定しておく
  if (rate <= 0.0f)
  {
    Set( t );
    return;
  }
  if (rate >= 1.0f)
  {
    Set( s );
    return;
  }

	Vector tpl(0, 0, 0, 0);
	f32 dot = t.Dot( s );
	if( dot < 0.0f ){
		tpl.Neg( ( const Vector& )t );
		dot = -dot;
	} else {
		tpl.Set( ( const Vector& )t );
	}

	if( dot < 1.0f - EPSILON ){
		f32 ang = math::FACos( dot );
		f32 sin_recip = math::FRecip( math::FSin( ang ) );
		tpl = tpl * ( math::FSin( ( 1.0f - rate ) * ang ) * sin_recip );
		Vector r(0, 0, 0, 0);
		r = ( const Vector& )s;
		r *= math::FSin( rate * ang ) * sin_recip;
		tpl = tpl + r;
		Set( ( const Quaternion& )tpl );
	} else {
		Set( t );
	}

}

// @attention Matrix34に移行したらで消す
inline void Quaternion::MatrixToQuaternion( Matrix mat )
{
	mat.Normalize( );

	const f32 tr = mat[0][0] + mat[1][1] + mat[2][2];
	f32 ww, max;
	s32 index;

	if( tr >= 0.0f ){
		ww = math::FSqrt( tr + 1.0f );
		this->GFL_VECTOR_W = ( ww * 0.5f );
		ww = -0.5f / ww;

		this->GFL_VECTOR_X = ( ( mat[2][1] - mat[1][2] ) * ww );
		this->GFL_VECTOR_Y = ( ( mat[0][2] - mat[2][0] ) * ww );
		this->GFL_VECTOR_Z = ( ( mat[1][0] - mat[0][1] ) * ww );
		//		this->Normalize();
		return;
	}

	index = 0;
	max = mat[0][0];
	if( mat[1][1] > max ){
		index = 1;
		max = mat[1][1];
	}
	if( mat[2][2] > max ){
		index = 2;
	}

	switch( index ){
	case 0:
		ww = math::FSqrt( mat[0][0] - mat[1][1] - mat[2][2] + 1.0f );
		this->GFL_VECTOR_X = ( ww * 0.5f );
		ww = 0.5f / ww;
		this->GFL_VECTOR_Y = ( ( mat[0][1] + mat[1][0] ) * ww );
		this->GFL_VECTOR_Z = ( ( mat[2][0] + mat[0][2] ) * ww );
		this->GFL_VECTOR_W = ( ( mat[1][2] - mat[2][1] ) * ww );
		break;
	case 1:
		ww = math::FSqrt( mat[1][1] - mat[0][0] - mat[2][2] + 1.0f );
		this->GFL_VECTOR_Y = ( ww * 0.5f );
		ww = 0.5f / ww;
		this->GFL_VECTOR_Z = ( ( mat[1][2] + mat[2][1] ) * ww );
		this->GFL_VECTOR_X = ( ( mat[0][1] + mat[1][0] ) * ww );
		this->GFL_VECTOR_W = ( ( mat[2][0] - mat[0][2] ) * ww );
		break;
	case 2:
		ww = math::FSqrt( mat[2][2] - mat[0][0] - mat[1][1] + 1.0f );
		this->GFL_VECTOR_Z = ( ww * 0.5f );
		ww = 0.5f / ww;
		this->GFL_VECTOR_X = ( ( mat[2][0] + mat[0][2] ) * ww );
		this->GFL_VECTOR_Y = ( ( mat[1][2] + mat[2][1] ) * ww );
		this->GFL_VECTOR_W = ( ( mat[0][1] - mat[1][0] ) * ww );
	}
}

inline void Quaternion::MatrixToQuaternion( const Matrix34& mat )
{
	const f32 tr = mat[0][0] + mat[1][1] + mat[2][2];
	f32 ww, max;
	s32 index;

	if( tr >= 0.0f ){
		ww = math::FSqrt( tr + 1.0f );
		this->GFL_VECTOR_W = ( ww * 0.5f );
		ww = -0.5f / ww;

		this->GFL_VECTOR_X = ( ( mat[1][2] - mat[2][1] ) * ww );
		this->GFL_VECTOR_Y = ( ( mat[2][0] - mat[0][2] ) * ww );
		this->GFL_VECTOR_Z = ( ( mat[0][1] - mat[1][0] ) * ww );
		//		this->Normalize();
		return;
	}

	index = 0;
	max = mat[0][0];
	if( mat[1][1] > max ){
		index = 1;
		max = mat[1][1];
	}
	if( mat[2][2] > max ){
		index = 2;
	}

	switch( index ){
	case 0:
		ww = math::FSqrt( mat[0][0] - mat[1][1] - mat[2][2] + 1.0f );
		this->GFL_VECTOR_X = ( ww * 0.5f );
		ww = 0.5f / ww;
		this->GFL_VECTOR_Y = ( ( mat[1][0] + mat[0][1] ) * ww );
		this->GFL_VECTOR_Z = ( ( mat[0][2] + mat[2][0] ) * ww );
		this->GFL_VECTOR_W = ( ( mat[2][1] - mat[1][2] ) * ww );
		break;
	case 1:
		ww = math::FSqrt( mat[1][1] - mat[0][0] - mat[2][2] + 1.0f );
		this->GFL_VECTOR_Y = ( ww * 0.5f );
		ww = 0.5f / ww;
		this->GFL_VECTOR_Z = ( ( mat[2][1] + mat[1][2] ) * ww );
		this->GFL_VECTOR_X = ( ( mat[1][0] + mat[0][1] ) * ww );
		this->GFL_VECTOR_W = ( ( mat[0][2] - mat[2][0] ) * ww );
		break;
	case 2:
		ww = math::FSqrt( mat[2][2] - mat[0][0] - mat[1][1] + 1.0f );
		this->GFL_VECTOR_Z = ( ww * 0.5f );
		ww = 0.5f / ww;
		this->GFL_VECTOR_X = ( ( mat[0][2] + mat[2][0] ) * ww );
		this->GFL_VECTOR_Y = ( ( mat[2][1] + mat[1][2] ) * ww );
		this->GFL_VECTOR_W = ( ( mat[1][0] - mat[0][1] ) * ww );
	}
}

// @attention Matrix34に移行したらで消す
inline void Quaternion::QuaternionToMatrix( Matrix &mat ) const
{
	Quaternion qt;
	//	qt.Normalize( q );
	const f32 scl = qt.NormalizeRet( *this );

	const f32 x = qt.GFL_VECTOR_X;
	const f32 y = qt.GFL_VECTOR_Y;
	const f32 z = qt.GFL_VECTOR_Z;
	const f32 w = qt.GFL_VECTOR_W;

	const f32 xx = x * x;
	const f32 yy = y * y;
	const f32 zz = z * z;
	const f32 xy = x * y;
	const f32 xz = x * z;
	const f32 yz = y * z;
	const f32 wx = w * x;
	const f32 wy = w * y;
	const f32 wz = w * z;

	mat[0][0] = 1.0f - 2.0f * ( yy + zz );
	mat[1][0] = 2.0f * ( xy - wz );
	mat[2][0] = 2.0f * ( xz + wy );

	mat[0][1] = 2.0f * ( xy + wz );
	mat[1][1] = 1.0f - 2.0f * ( xx + zz );
	mat[2][1] = 2.0f * ( yz - wx );

	mat[0][2] = 2.0f * ( xz - wy );
	mat[1][2] = 2.0f * ( yz + wx );
	mat[2][2] = 1.0f - 2.0f * ( xx + yy );

	if( !math::IsSimilar( scl, 1.0f, 0.01f ) ){
		mat[0] *= scl;
		mat[1] *= scl;
		mat[2] *= scl;
	}

  mat[3].Set(0,0,0,1);
}

inline void Quaternion::QuaternionToMatrix( Matrix34 &mat ) const
{
#if defined(GF_PLATFORM_CTR)
  // SDKの関数の方が速かった
  nn::math::QUATToMTX34((nn::math::MTX34*)&mat, (nn::math::QUAT*)this);
#else
	const f32 xx = x * x;
	const f32 yy = y * y;
	const f32 zz = z * z;
	const f32 xy = x * y;
	const f32 xz = x * z;
	const f32 yz = y * z;
	const f32 wx = w * x;
	const f32 wy = w * y;
	const f32 wz = w * z;

	mat[0][0] = 1.0f - 2.0f * ( yy + zz );
	mat[0][1] = 2.0f * ( xy - wz );
	mat[0][2] = 2.0f * ( xz + wy );

	mat[1][0] = 2.0f * ( xy + wz );
	mat[1][1] = 1.0f - 2.0f * ( xx + zz );
	mat[1][2] = 2.0f * ( yz - wx );

	mat[2][0] = 2.0f * ( xz - wy );
	mat[2][1] = 2.0f * ( yz + wx );
	mat[2][2] = 1.0f - 2.0f * ( xx + yy );

  mat[0][3] = 0.0f;
  mat[1][3] = 0.0f;
  mat[2][3] = 0.0f;
#endif
}

inline Vector3 Quaternion::Transform( const Vector3& rhs ) const
{
  Quaternion result = *this * Quaternion(rhs.GetX(), rhs.GetY(), rhs.GetZ(), 0.0f) * this->GetConj();
  return Vector3(result.GetX(), result.GetY(), result.GetZ());
}

inline Vector3 Quaternion::TransformWithScale( const Vector3& rhs, const Vector3& scale ) const
{
  Quaternion result = *this * Quaternion(rhs.GetX(), rhs.GetY(), rhs.GetZ(), 0.0f) * this->GetConj();
  return Vector3(result.GetX() * scale.GetX(), result.GetY() * scale.GetY(), result.GetZ() * scale.GetZ());
}

inline Vector4 Quaternion::Transform( const Vector4& rhs ) const
{
  Quaternion result = *this * Quaternion(rhs.GetX(), rhs.GetY(), rhs.GetZ(), 0.0f) * this->GetConj();
  return Vector4(result.GetX(), result.GetY(), result.GetZ(), 1.0f);
}

}}

#endif	/* GFL_QUATERNION_H */


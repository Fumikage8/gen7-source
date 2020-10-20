/* 
 * File:   gfl_Vector4.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_VECTOR4_H
#define	GFL_VECTOR4_H

//class Vector4 : public VectorTemplate<VectorNative4> {
class Vector4 : public VectorNative4 {
public:
	typedef VectorNative4 parent_type;
	typedef Vector4 self_type;
	
#include "gfl_Tuple4Common.h"
	
	static const VectorStruct4 s_X1;
	static const VectorStruct4 s_Y1;
	static const VectorStruct4 s_Z1;
	static const VectorStruct4 s_W1;

	// ---------------------------------------------------------------------------------
	// Constructer
	// ---------------------------------------------------------------------------------
	//  ----------------------------------------------------------------
	//! @brief  デフォルトコンスタラクター
	//! @param  無し
	//! @return 無し
	//! @remark 何もしない
	//  ----------------------------------------------------------------

	Vector4( void ) {
	}

	//  ----------------------------------------------------------------
	//! @brief  Ｘ，Ｙ，Ｚ，Ｗ成分を設定するコンスタラクター
	//! @param  x 設定値
	//! @param  y 設定値
	//! @param  z 設定値
	//! @param  w 設定値 指定しない場合は 0.0f が入る
	//! @return 無し
	//  ----------------------------------------------------------------

	Vector4( const f32 vx, const f32 vy, const f32 vz, const f32 vw = 0.0f ) {
		Set( vx, vy, vz, vw );
	}

	//  ----------------------------------------------------------------
	//! @brief  Vector4 を設定するコンスタラクター
	//! @param  v Vector4クラス
	//! @return 無し
	//  ----------------------------------------------------------------
	Vector4(const VectorNative4& vt){
		Set(vt);
	}

	//  ----------------------------------------------------------------
	//! @brief  Vector4 を設定するコンスタラクター
	//! @param  v Tuple4fクラス
	//! @return 無し
	//  ----------------------------------------------------------------
	Vector4(const VectorNative3& vt){
		Set(vt);
	}

	//  ----------------------------------------------------------------
	//! @brief  Vector4 を設定するコンスタラクター
	//! @param  v Tuple4fクラス
	//! @return 無し
	//  ----------------------------------------------------------------
	Vector4(const VectorNative3& vt, const f32 w){
		Set(vt,w);
	}

	// ---------------------------------------------------------------------------------
	// Destruct !! not virtual for alignment
	// ---------------------------------------------------------------------------------

	~Vector4( ) {
	}

	/**
	 * (0,0,0,0) を取得
	 * @return (0,0,0,0)
	 */
	static const Vector4& GetZero( void ) {
		return static_cast<const Vector4&> (VectorNative4::Zero( ));
	}

	/**
	 * (1,0,0,0) を取得
	 * @return (1,0,0,0)
	 */
	static const Vector4& GetX1( void ) {
		return static_cast<const Vector4&> (s_X1);
	}

	/**
	 * (0,1,0,0) を取得
	 * @return (0,1,0,0)
	 */
	static const Vector4& GetY1( void ) {
		return static_cast<const Vector4&> (s_Y1);
	}

	/**
	 * (0,0,1,0) を取得
	 * @return (0,0,1,0)
	 */
	static const Vector4& GetZ1( void ) {
		return static_cast<const Vector4&> (s_Z1);
	}

	/**
	 * (0,0,0,1) を取得
	 * @return (0,0,0,1)
	 */
	static const Vector4& GetW1( void ) {
		return static_cast<const Vector4&> (s_W1);
	}

	/**
	 * VectorNative4 を設定
	 * @param tx X成分
	 * @param ty Y成分
	 * @param tz Z成分
	 * @remark W成分は設定しない
	 */
	inline void Set( const f32 tx, const f32 ty, const f32 tz ) {
		GFL_VECTOR_X = tx;
		GFL_VECTOR_Y = ty;
		GFL_VECTOR_Z = tz;
	}

	/**
	 * VectorNative4 を設定
	 * @param t 設定したいVectorNative3
	 * @param tw W成分
	 */
	inline void Set( const VectorNative3& t ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
	}

	/**
	 * VectorNative4 を設定
	 * @param t 設定したいVectorNative3
	 * @param tw W成分
	 */
	inline void Set( const VectorNative3& t, const f32 tw ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z, tw );
	}

	/**
	 * (1,0,0,0) を設定
	 */
	void SetX1( void ) {
		Set( static_cast<const Vector4&> (s_X1) );
	}

	/**
	 * (0,1,0,0) を設定
	 */
	void SetY1( void ) {
		Set( static_cast<const Vector4&> (s_Y1) );
	}

	/**
	 * (0,0,1,0) を設定
	 */
	void SetZ1( void ) {
		Set( static_cast<const Vector4&> (s_Z1) );
	}

	/**
	 * (0,0,0,1) を設定
	 */
	void SetW1( void ) {
		Set( static_cast<const Vector4&> (s_W1) );
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetZero( void ) {
		Set( Zero( ) );
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
		*this = - * this;
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */

	inline void Neg( const VectorNative4& t ) {
		*this = -t;
	}

	inline Vector4 GetNeg( void ) {
		return - * this;
	}

	/**
	 * X の符号を反転させる
     */
	inline void NegX(void){
		GFL_VECTOR_X = -GFL_VECTOR_X;
	}

	/**
	 * Y の符号を反転させる
     */
	inline void NegY(void){
		GFL_VECTOR_Y = -GFL_VECTOR_Y;
	}

	/**
	 * Z の符号を反転させる
     */
	inline void NegZ(void){
		GFL_VECTOR_Z = -GFL_VECTOR_Z;
	}

	/**
	 * W の符号を反転させる
     */
	inline void NegW(void){
		GFL_VECTOR_W = -GFL_VECTOR_W;
	}

	/**
	 *   外積
	 * @param  t 外積を求めたいクラス
	 * @param  s 外積を求めたいクラス
	 * @return 無し
	//				 * @remark t, s と this と違うクラスでないとアサートします
	 */

	inline void Cross( const VectorNative4& t, const VectorNative4& s ) {
		Set( t.GFL_VECTOR_Y * s.GFL_VECTOR_Z - t.GFL_VECTOR_Z * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_X - t.GFL_VECTOR_X * s.GFL_VECTOR_Z,
			 t.GFL_VECTOR_X * s.GFL_VECTOR_Y - t.GFL_VECTOR_Y * s.GFL_VECTOR_X, 0.0f );
	}

	/**
	 *   外積
	 * @param  s 外積を求めたいクラス
	 * @return 無し
	 */

	inline void Cross( const VectorNative4& s ) {
		Cross( *this, s );
	}

	/**
	 *   Ｙ方向の外積
	 * @param  t 外積を求めたいクラス
	 * @return 正なら上向き　負なら下向き
	 */

	inline f32 CrossY( const Vector3& t ) const {
		return GFL_VECTOR_Z * t.GFL_VECTOR_X - GFL_VECTOR_X * t.GFL_VECTOR_Z;
	}

	// -------------------------------------------------------------------------
	// Optimize radian
	// -------------------------------------------------------------------------

	inline void OptimizeRadian( void ) {
		gfl::core::Math::OptimizeRadian( &GFL_VECTOR_X, &GFL_VECTOR_Y, &GFL_VECTOR_Z );
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
		return LengthSquare( );
	}

	/**
	 *   大きさの二乗の逆数
	 * @param  無し
	 * @return 大きさの二乗の逆数
	 */

	inline f32 Magnitude2Recip( void ) const {
		return Math::FRecip( LengthSquare( ) );
	}

	/**
	 *   大きさ
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 Magnitude( void ) const {
		return Length( );
	}

	/**
	 *   大きさの逆数
	 * @param  無し
	 * @return 大きさの逆数
	 */

	inline f32 MagnitudeRecip( void ) const {
		return Math::FRecip( Length( ) );
	}

	/**
	 * XZ平面の距離の二乗
	 * @return XZ平面の距離の二乗
	 */
	inline f32 MagnitudeXZ2( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Z * GFL_VECTOR_Z;
	}

	/**
	 *   ＸＺ平面の大きさ
	 * @param  無し
	 * @return ＸＺ平面の大きさ
	 */

	inline f32 MagnitudeXZ( void ) const {
		return Math::FSqrt( MagnitudeXZ2( ) );
	}

	/**
	 *   ＸＺ平面の大きさの逆数
	 * @param  無し
	 * @return ＸＺ平面の大きさの逆数
	 */

	inline f32 MagnitudeXZRecip( void ) const {
		return Math::FRecip( MagnitudeXZ( ) );
	}

	/**
	 * XY平面の距離の二乗
	 * @return XY平面の距離の二乗
	 */
	inline f32 MagnitudeXY2( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Y * GFL_VECTOR_Y;
	}
	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 無し
	 */

	inline void Normalize( const VectorNative4& t ) {
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

	inline f32 NormalizeRet( const VectorNative4& t ) {
		Set( t );
		return NormalizeRet( );
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */
	inline f32 Distance2( const VectorNative4& t )	 const
	{
#ifdef GFL_PLATFORM_3DS
		return nn::math::VEC4DistSq( this, &t );
#else
		return DistanceSquare( t );
#endif
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 Distance( const VectorNative4& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	inline f32 DistanceXZ2( const VectorNative4& t ) const {
		return Vector4( *this -t ).MagnitudeXZ2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 DistanceXZ( const VectorNative4& t ) const {
		return Math::FSqrt( DistanceXZ2( t ) );
	}

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	inline f32 DistanceXY2( const VectorNative4& t ) const {
		return Vector4( *this -t ).MagnitudeXY2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 DistanceXY( const VectorNative4& t ) const {
		return Math::FSqrt( DistanceXY2( t ) );
	}

	/**
	 *   内積XZ
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	inline f32 DotXZ( const VectorNative4& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Z * t.GFL_VECTOR_Z;
	}

	/**
	 *   オペレーター =
	 * @param  t 代入したい VectorNative4
	 * @return this の参照
	 */

	inline Vector4 & operator=(const VectorNative3& t) {
		Set( t );
		return *this;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative3& t ) {
		GFL_TP GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_TP GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_TP GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
	}
	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative4& t ) {
		GFL_TP GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_TP GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_TP GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
		GFL_TP GFL_VECTOR_W *= t.GFL_VECTOR_W;
	}

	/**
	 * 乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
     */
	inline void Mul33( const VectorNative3& t, const VectorNative3& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X,
			 t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z );
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative3& t, const VectorNative3& s ) {
		Mul33(t,s);
	}
	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative3& t, const VectorNative4& s ) {
		Mul33(t,reinterpret_cast<const VectorNative3&>(s));
	}
	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative4& t, const VectorNative3& s ) {
		Mul33(reinterpret_cast<const VectorNative3&>(t),s);
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative4& t, const VectorNative4& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X,
			 t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z,
			 t.GFL_VECTOR_W * s.GFL_VECTOR_W );
	}

	/**
	 *   オペレーター *
	 * @param  t 積算したいクラス
	 * @return 積算されたクラス VectorNative4
	 */

	inline Vector4 operator*(const VectorNative4& t) const {
		Vector4 r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   オペレーター ^=
	 * @param  t 外積を求めたいクラス
	 * @return this の参照
	 */

	inline Vector4 & operator^=(const VectorNative4& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   オペレーター ^
	 * @param  t 外積を求めたいクラス
	 * @return 外積が設定されたクラス VectorNative4
	 */

	inline Vector4 operator^(const VectorNative4& t) const {
		Vector4 r;
		r.Cross( *this, t );
		return r;
	}

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */
	inline void Lerp( const VectorNative4& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	/**
	 * 前方９０度の範囲に入っているか
	 * @return 真　入っている
	 */
	inline bool IsFrontDegree90( void ) {
		return gfl::core::Math::IsFrontDegree90( GFL_VECTOR_X, GFL_VECTOR_Z );
	}

	/**
	 * 現在のベクトルを回転させる
	 * @param s　サイン値
	 * @param c　コサイン値
	 */
	void RotateY( const f32 s, const f32 c ) {
		const f32 tx = GFL_VECTOR_X;
		const f32 tz = GFL_VECTOR_Z;
		GFL_VECTOR_X = gfl::core::Math::GetX_RotateY_SinCosXZ( s, c, tx, tz );
		GFL_VECTOR_Z = gfl::core::Math::GetZ_RotateY_SinCosXZ( s, c, tx, tz );
	}

	/**
	 * 現在のベクトルを回転させる
	 * @param ang 回転値　ラジアン
	 */
	void RotateY( const f32 ang ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, ang );
		RotateY( s, c );
	}
	
	/**
	 *   オペレーター +
	 * @param  t 加算したいクラス
	 * @return 加算されたクラス VectorNative3
	 */

	inline Vector4 operator+(const VectorNative4& t) const {
		return Vector4(VectorNative4::operator+(t));
	}
	// =========================================================================
	// using
	// =========================================================================
	using VectorNative4::Lerp;
	using VectorNative4::Set;
	using VectorNative4::operator+;
	using VectorNative4::operator*;
	using VectorNative4::Normalize;
//	using VectorNative4::Cross;
#ifndef GFL_PLATFORM_3DS
	using VectorNative4::Mul;
#endif
};

#endif	/* GFL_VECTOR4_H */


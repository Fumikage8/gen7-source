/* 
 * File:   gfl_Vector3.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_VECTOR3_H
#define	GFL_VECTOR3_H

/**
 *  3つの要素を持つクラス
 */

class Vector3 : public VectorNative3 {
public:
	static const VectorStruct3 s_X1;
	static const VectorStruct3 s_Y1;
	static const VectorStruct3 s_Z1;
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Vector3( void ) {
	}

	/**
	 *   Ｘ，Ｙ成分を設定するコンスタラクター
	 * @param  tx 設定値
	 * @param  ty 設定値
	 * @return 無し
	 */

	Vector3( const f32 tx, const f32 ty, const f32 tz ) : VectorNative3( tx, ty, tz ) {
	}
	Vector3( const f32 tx, const f32 ty, const f32 tz, const f32) : VectorNative3( tx, ty, tz ) {
	}

	/**
	 *   Vector3 を設定するコンスタラクター
	 * @param  t Vector2クラス
	 * @return 無し
	 */

	explicit Vector3( const VectorNative3& t ) : VectorNative3( t ) {
	}

	Vector3( const Vector3& t ) : VectorNative3( t ) {
	}

	Vector3( const VectorNative4& t ) {
		Set( t );
	}

	/**
	 * コンストラクタ
	 * @param vp 設定したい浮動小数点ポインタ
	 */
	explicit Vector3( const f32* vp ) {
		Set( *(const VectorNative3*)vp );
	}

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline f32 GetX( void ) const {
		return GFL_VECTOR_X;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline f32 GetY( void ) const {
		return GFL_VECTOR_Y;
	}

	/**
	 *   Ｚ成分を取得
	 * @param  無し
	 * @return Ｚ成分
	 */

	inline f32 GetZ( void ) const {
		return GFL_VECTOR_Z;
	}

	/**
	 *   W成分を取得するふりをする
	 * @param  無し
	 * @return ０
	 */

	inline f32 GetW( void ) const {
		return 0.0f;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TY(2)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline f32 Get( const s32 i ) const {
		return ( &GFL_VECTOR_X)[i];
	}

	/**
	 *   変数のポインターを取得
	 * @param  無し
	 * @return 変数のポインター
	 */

	inline f32* Get( void ) {
		return ( &GFL_VECTOR_X);
	}

	inline const f32* Get( void ) const {
		return ( &GFL_VECTOR_X);
	}

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
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) 〜 TY(2)
	 * @return 指定された成分
	 */

	inline f32& GetRef( const s32 i ) {
		return ( &GFL_VECTOR_X)[i];
	}

	/**
	 * (0,0,0) を取得
     * @return (0,0,0)
     */
	static const Vector3& GetZero( void ) {
		return static_cast<const Vector3&>(Zero());
	}

	/**
	 * (1,0,0) を取得
     * @return (1,0,0)
     */
	static const Vector3& GetX1( void ) {
		return static_cast<const Vector3&>(s_X1);
	}

	/**
	 * (0,1,0) を取得
     * @return (0,1,0)
     */
	static const Vector3& GetY1( void ) {
		return static_cast<const Vector3&>(s_Y1);
	}

	/**
	 * (0,0,1) を取得
     * @return (0,0,1)
     */
	static const Vector3& GetZ1( void ) {
		return static_cast<const Vector3&>(s_Z1);
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を設定
	 * @param  tx 設定値
	 * @return 無し
	 */

	inline void SetX( const f32 tx ) {
		GFL_VECTOR_X = tx;
	}

	/**
	 *   Ｙ成分を設定
	 * @param  ty 設定値
	 * @return 無し
	 */

	inline void SetY( const f32 ty ) {
		GFL_VECTOR_Y = ty;
	}

	/**
	 *   Ｚ成分を設定
	 * @param  tz 設定値
	 * @return 無し
	 */

	inline void SetZ( const f32 tz ) {
		GFL_VECTOR_Z = tz;
	}

	/**
	 *   Ｗ成分を設定するふりをするダミー関数
	 * @param  T 設定値
	 * @return 無し
	 */

	inline void SetW( const f32 ) {
	}

	/**
	 *   指定した成分を設定
	 * @param  i 添え字 TX(0) 〜 TY(2)
	 * @param  val 設定値
	 * @return 無し
	 */

	inline void Set( const s32 i, const f32 val ) {
		(&GFL_VECTOR_X)[i] = val;
	}

	/**
	 * 設定
	 * @param tx　X成分
	 * @param ty　Y成分
	 * @param tz　Z成分
	 * @param ダミー
	 */
	inline void Set( const f32 tx, const f32 ty, const f32 tz, const f32 ) {
		Set( tx, ty, tz );
	}

	/**
	 * VectorNative4 を設定
	 * @param t 設定したいVectorNative4
	 */
	inline void Set( const VectorNative4& t ) {
		Set( reinterpret_cast<const VectorNative3&> (t) );
	}

	/**
	 * (1,0,0) を設定
     */
	void SetX1( void ) {
		Set( static_cast<const Vector3&>(s_X1) );
	}

	/**
	 * (0,1,0) を設定
     */
	void SetY1( void ) {
		Set( static_cast<const Vector3&>(s_Y1) );
	}

	/**
	 * (0,0,1) を設定
     */
	void SetZ1( void ) {
		Set( static_cast<const Vector3&>(s_Z1) );
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetZero( void ) {
		Set( Zero( ) );
	}
	
	/**
	 * ゼロを設定 SetZero と同じ
     */
	inline void SetUnit(void){
		SetZero();
	}


	/**
	 *   外積
	 * @param  s 外積を求めたいクラス
	 * @return 無し
	 */

	inline void Cross( const VectorNative3& s ) {
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
	// Calc Normal
	// -------------------------------------------------------------------------

	void CalcNormal( const Vector3& v0, const Vector3& v1, const Vector3& v2 );

	// =========================================================================
	// using
	// =========================================================================
	using VectorNative3::Set;
	using VectorNative3::Lerp;
	using VectorNative3::operator+;
	using VectorNative3::operator*;
	using VectorNative3::Normalize;
	using VectorNative3::Cross;

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------
#ifdef GFL_PLATFORM_3DS

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const VectorNative3& t ) {
		*this += t;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const VectorNative3& t, const VectorNative3& s ) {
		*this = t + s;
	}

	/**
	 *   加算
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const f32 tx, const f32 ty, const f32 tz ) {
		Add( *this, VectorNative3( tx, ty, tz ) );
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const VectorNative3& t, const f32 tx, const f32 ty, const f32 tz ) {
		Add( t, VectorNative3( tx, ty, tz ) );
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

	inline void Sub( const VectorNative3& t ) {
		*this -= t;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const VectorNative3& t, const VectorNative3& s ) {
		*this = t - s;
	}

	/**
	 *   減算
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */

	inline void Sub( const f32 tx, const f32 ty, const f32 tz ) {
		Sub( *this, VectorNative3( tx, ty, tz ) );
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */


	inline void Sub( const VectorNative3& t, const f32 tx, const f32 ty, const f32 tz ) {
		Sub( t, VectorNative3( tx, ty, tz ) );
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
		*this *= f;
	}

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Mul( const f32 f, const VectorNative3& t ) {
		*this = f * t;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Mul( const VectorNative3& t, const f32 f ) {
		*this = f * t;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative3& t ) {
		GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative3& t, const VectorNative3& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X,
			 t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z );
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
		*this /= f;
	}

	/**
	 *   除算
	 * @param  t 除算したいクラス
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark t の各成分を f で除算する 内部的には逆数の掛け算
	 */

	inline void Div( const VectorNative3& t, const f32 f ) {
		*this = t / f;
	}
#endif
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

	inline void Neg( const VectorNative3& t ) {
		*this = -t;
	}

	inline VectorNative3 GetNeg( void ) {
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
	 * W の符号を反転させるふり
     */
	inline void NegW(void){
//		GFL_VECTOR_W = -GFL_VECTOR_W;
	}

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */
	inline void Lerp( const VectorNative3& t, const f32 rate ) {
		Lerp( *this, t, rate );
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
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */
	inline f32 Distance2( const VectorNative3& t ) const {
		return DistanceSquare( t );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 Distance( const VectorNative3& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	inline f32 DistanceXZ2( const VectorNative3& t ) const {
		return Vector3( *this -t ).MagnitudeXZ2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 DistanceXZ( const VectorNative3& t ) const {
		return Math::FSqrt( DistanceXZ2( t ) );
	}

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */

	inline f32 DistanceXY2( const VectorNative3& t ) const {
		return Vector3( *this -t ).MagnitudeXY2( );
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 DistanceXY( const VectorNative3& t ) const {
		return Math::FSqrt( DistanceXY2( t ) );
	}

	/**
	 *   内積XZ
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	inline f32 DotXZ( const VectorNative3& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Z * t.GFL_VECTOR_Z;
	}

	// -------------------------------------------------------------------------
	// Cross 二次元に外積はありません
	// -------------------------------------------------------------------------

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   オペレーター []
	 * @param  i 添え字 TX(0) 〜 TZ(3)
	 * @return 指定された成分
	 */

	inline f32 operator [](s32 i) const {
		return Get( i );
	}

	inline f32& operator [](s32 i){
		return GetRef( i );
	}
	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	inline bool operator==(const self_type& t) const {
		return IsSimilar(t);
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	inline bool operator!=(const self_type& t) const {
		return !IsSimilar(t);
	}

	/**
	 *   オペレーター =
	 * @param  t 代入したい VectorNative3
	 * @return this の参照
	 */

	inline Vector3 & operator=(const VectorNative3& t) {
		Set( t );
		return *this;
	}
	
	/**
	 *   オペレーター +
	 * @param  t 加算したいクラス
	 * @return 加算されたクラス VectorNative3
	 */

	inline Vector3 operator+(const VectorNative3& t) const {
		return Vector3(VectorNative3::operator+(t));
	}
	
	/**
	 *   オペレーター *
	 * @param  t 積算したいクラス
	 * @return 積算されたクラス VectorNative3
	 */

	inline Vector3 operator*(const VectorNative3& t) const {
		Vector3 r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   オペレーター ^=
	 * @param  t 外積を求めたいクラス
	 * @return this の参照
	 */

	inline Vector3 & operator^=(const VectorNative3& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   オペレーター ^
	 * @param  t 外積を求めたいクラス
	 * @return 外積が設定されたクラス VectorNative3
	 */

	inline Vector3 operator^(const VectorNative3& t) const {
		Vector3 r;
		r.Cross( *this, t );
		return r;
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
		Set( Math::FRecip( GFL_VECTOR_X ), Math::FRecip( GFL_VECTOR_Y ), Math::FRecip( GFL_VECTOR_Z ) );
	}

	/**
	 *   逆数　ＴのＸ，Ｙ成分の逆数を設定する
	 * @param  t 逆数にしたいクラス
	 * @return 無し
	 */


	inline void Recip( const VectorNative3& t ) {
		Set( Math::FRecip( t.GFL_VECTOR_X ), Math::FRecip( t.GFL_VECTOR_Y ), Math::FRecip( t.GFL_VECTOR_Z ) );
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 無し
	 */

	inline void Normalize( const VectorNative3& t ) {
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

	inline f32 NormalizeRet( const VectorNative3& t ) {
		Set(t);
		return NormalizeRet();
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

	inline bool IsSimilar( const f32 tx, const f32 ty, const f32 tz, const f32 epsilon = Math::EPSILON ) const {
		return Math::FAbs( tx - GFL_VECTOR_X ) <= epsilon &&
			Math::FAbs( ty - GFL_VECTOR_Y ) <= epsilon &&
			Math::FAbs( tz - GFL_VECTOR_Z ) <= epsilon;
	}

	/**
	 *   各成分が似ているか？
	 * @param  t 対象クラス
	 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
	 * @return 各成分値の誤差が epsilon 内なら真、それ以外は偽
	 */

	inline bool IsSimilar( const VectorNative3& t, const f32 epsilon = Math::EPSILON ) const {
		return IsSimilar(t.GFL_VECTOR_X,t.GFL_VECTOR_Y,t.GFL_VECTOR_Z,epsilon);
	}

	/**
	 * 全成分がほとんど０なのか？
	 * @param epsilon　誤差値
	 * @return　真 ほとんど０の時
	 */
	inline bool IsAlmostZero( const f32 epsilon = Math::EPSILON ) const {
		return Math::IsAlmostZero( GFL_VECTOR_X, epsilon ) &&
			Math::IsAlmostZero( GFL_VECTOR_Y, epsilon ) &&
			Math::IsAlmostZero( GFL_VECTOR_Z, epsilon );
	}

	/**
	 * 前方９０度の範囲に入っているか
	 * @return 真　入っている
	 */
	inline bool IsFrontDegree90( void ) {
		return gfl::core::Math::IsFrontDegree90( GFL_VECTOR_X, GFL_VECTOR_Z );
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
		Set( Math::FAbs( GFL_VECTOR_X ), Math::FAbs( GFL_VECTOR_Y ), Math::FAbs( GFL_VECTOR_Z ) );
	}

	/**
	 *   t の各成分の絶対値を設定する
	 * @param  t 入力クラス
	 * @return 無し
	 */

	inline void Abs( const VectorNative3& t ) {
		Set( Math::FAbs( t.GFL_VECTOR_X ), Math::FAbs( t.GFL_VECTOR_Y ), Math::FAbs( t.GFL_VECTOR_Z ) );
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

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @return 無し
	 */

	inline void PrintConsole( void ) const {
		Debug::PrintConsole( "{ %ff, %ff, %ff },\n", GFL_VECTOR_X, GFL_VECTOR_Y, GFL_VECTOR_Z );
	}
	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s ", str );
		PrintConsole();
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] ", str, i );
		PrintConsole( );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] ", str, i, j );
		PrintConsole( );
	}

	/**
	 * コンソールに角度で出力
	 */
	inline void PrintConsoleDegree( void ) const {
		Debug::PrintConsole( "{ %ff, %ff, %ff },\n", gfl::core::Math::RadianToDegree( GFL_VECTOR_X ), gfl::core::Math::RadianToDegree( GFL_VECTOR_Y ),
							 gfl::core::Math::RadianToDegree( GFL_VECTOR_Z ) );
	}

	/**
	 * コンソールに角度で出力
	 * @param str　文字列
	 */
	inline void PrintConsoleDegree( const c8 * const str ) const {
		Debug::PrintConsole( "%s ", str );
		PrintConsoleDegree( );
	}

	/**
	 *   コンソールに角度で出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */
	inline void PrintConsoleDegree( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d]", str, i );
		PrintConsoleDegree( );
	}

	/**
	 *   コンソールに角度で出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */
	inline void PrintConsoleDegree( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d]", str, i, j );
		PrintConsoleDegree( );
	}
#ifdef GFL_PLATFORM_3DS // これらはいずれ削除します！！
	void ToVEC3( gfl::math::VEC3* tag ){
		tag->Set(*this);
	}
	void FromVEC3(const gfl::math::VEC3& src){
		Set( (const nn::math::VEC3&)src );
	}
#endif
};
#endif	/* GFL_VECTOR3_H */


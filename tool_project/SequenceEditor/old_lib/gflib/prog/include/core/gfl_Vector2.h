/* 
 * File:   gfl_Vector2.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_VECTOR2_H
#define	GFL_VECTOR2_H

/**
 *  ２つの要素を持つタプルテンプレートクラス
 */

class Vector2 : public VectorNative2 {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Vector2( void ) {
	}

	/**
	 *   Ｘ，Ｙ成分を設定するコンスタラクター
	 * @param  tx 設定値
	 * @param  ty 設定値
	 * @return 無し
	 */

	Vector2( const f32 tx, const f32 ty ) {
		Set( tx, ty );
	}

	/**
	 *   Vector2 を設定するコンスタラクター
	 * @param  t Vector2クラス
	 * @return 無し
	 */

	explicit Vector2( const VectorNative2& t ) {
		Set( t );
	}

	Vector2( const Vector2& t ) {
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
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) ～ TY(2)
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
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) ～ TY(2)
	 * @return 指定された成分
	 */

	inline f32& GetRef( const s32 i ) {
		return ( &GFL_VECTOR_X)[i];
	}

	/**
	 * (0,0) を取得
	 * @return (0,0)
	 */
	static const Vector2& GetZero( void ) {
		return static_cast<const Vector2&> (Zero( ));
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
	 *   Vector2 を設定
	 * @param  t 設定したい Vector の参照
	 * @return 無し
	 */
	using VectorNative2::Set;

	inline void Set( const VectorNative2& t ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y );
	}

	/**
	 *   指定した成分を設定
	 * @param  i 添え字 TX(0) ～ TY(2)
	 * @param  val 設定値
	 * @return 無し
	 */

	inline void SetElem( const s32 i, const f32 val ) {
		(&GFL_VECTOR_X)[i] = val;
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
	// Add
	// -------------------------------------------------------------------------
#ifdef GFL_PLATFORM_3DS

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const VectorNative2& t ) {
		*this += t;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const VectorNative2& t, const VectorNative2& s ) {
		*this = t + s;
	}

	/**
	 *   加算
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const f32 tx, const f32 ty ) {
		Add( *this, VectorNative2( tx, ty ) );
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const VectorNative2& t, const f32 tx, const f32 ty ) {
		Add( t, VectorNative2( tx, ty ) );
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

	inline void Sub( const VectorNative2& t ) {
		*this -= t;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const VectorNative2& t, const VectorNative2& s ) {
		*this = t - s;
	}

	/**
	 *   減算
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */

	inline void Sub( const f32 tx, const f32 ty ) {
		Sub( *this, VectorNative2( tx, ty ) );
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */


	inline void Sub( const VectorNative2& t, const f32 tx, const f32 ty ) {
		Sub( t, VectorNative2( tx, ty ) );
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

	inline void Mul( const f32 f, const VectorNative2& t ) {
		*this = f * t;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Mul( const VectorNative2& t, const f32 f ) {
		*this = f * t;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative2& t ) {
		GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const VectorNative2& t, const VectorNative2& s ) {
		GFL_VECTOR_X = t.GFL_VECTOR_X * s.GFL_VECTOR_X;
		GFL_VECTOR_Y = t.GFL_VECTOR_Y * s.GFL_VECTOR_Y;
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

	inline void Div( const VectorNative2& t, const f32 f ) {
		*this = t / f;
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

	inline void Neg( const VectorNative2& t ) {
		*this = -t;
	}

	inline VectorNative2 GetNeg( void ) {
		return - * this;
	}

#endif

	using VectorNative2::Lerp;

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */
	inline void Lerp( const VectorNative2& t, const f32 rate ) {
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

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 * @remark 任天堂のライブラリにコンストがついていない（付け忘れ？）のでコンストをはずす
	 */
	inline f32 Distance2( const VectorNative2& t ) const {
#ifdef GFL_PLATFORM_3DS
		return nn::math::VEC2DistSq( this, &t );
#else
		return DistanceSquare( t ); // const が付いたらこっちに
#endif
	}

	/**
	 *   距離
	 * @param  t 距離を求めたいクラス
	 * @return 距離
	 */

	inline f32 Distance( const VectorNative2& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	// -------------------------------------------------------------------------
	// Cross 二次元に外積はありません
	// -------------------------------------------------------------------------

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   オペレーター []
	 * @param  i 添え字 TX(0) ～ TZ(3)
	 * @return 指定された成分
	 */

	inline f32 operator [](s32 i) const {
		return Get( i );
	}

	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	inline bool operator==(const self_type& t) const {
		return IsSimilar( t );
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	inline bool operator!=(const self_type& t) const {
		return !IsSimilar( t );
	}

	/**
	 *   オペレーター =
	 * @param  t 代入したい VectorNative2
	 * @return this の参照
	 */

	inline VectorNative2 & operator=(const VectorNative2& t) {
		Set( t );
		return *this;
	}

	using VectorNative2::operator*;

	/**
	 *   オペレーター *
	 * @param  t 積算したいクラス
	 * @return 積算されたクラス VectorNative2
	 */

	inline VectorNative2 operator*(const VectorNative2& t) const {
		Vector2 r;
		r.Mul( *this, t );
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
		Set( Math::FRecip( GFL_VECTOR_X ), Math::FRecip( GFL_VECTOR_Y ) );
	}

	/**
	 *   逆数　ＴのＸ，Ｙ成分の逆数を設定する
	 * @param  t 逆数にしたいクラス
	 * @return 無し
	 */


	inline void Recip( const VectorNative2& t ) {
		Set( Math::FRecip( t.GFL_VECTOR_X ), Math::FRecip( t.GFL_VECTOR_Y ) );
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	using VectorNative2::Normalize;

	/**
	 *   正規化
	 * @param  t 正規化したいクラス
	 * @return 無し
	 */

	inline void Normalize( const VectorNative2& t ) {
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

	inline f32 NormalizeRet( const VectorNative2& t ) {
		Set( t );
		return NormalizeRet( );
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

	inline bool IsSimilar( const VectorNative2& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::FAbs( t.GFL_VECTOR_X - GFL_VECTOR_X ) <= epsilon && Math::FAbs( t.GFL_VECTOR_Y - GFL_VECTOR_Y ) <= epsilon;
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
		Set( Math::FAbs( GFL_VECTOR_X ), Math::FAbs( GFL_VECTOR_Y ) );
	}

	/**
	 *   t の各成分の絶対値を設定する
	 * @param  t 入力クラス
	 * @return 無し
	 */

	inline void Abs( const VectorNative2& t ) {
		Set( Math::FAbs( t.GFL_VECTOR_X ), Math::FAbs( t.GFL_VECTOR_Y ) );
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
		Debug::PrintConsole( "%s { %ff, %ff },\n", str, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] { %ff, %ff },\n", str, i, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] { %ff, %ff },\n", str, i, j, GFL_VECTOR_X, GFL_VECTOR_Y );
	}
};

#endif	/* GFL_VECTOR2_H */

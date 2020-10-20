//==============================================================================
/**
 * @file	gfl_Tuple2.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/31, 15:30
 */
// =============================================================================

#if !defined( __GFL_TUPLE2_H__ )
#define	__GFL_TUPLE2_H__

/**
 *  ２つの要素を持つタプル構造体
 */

template<typename T>
struct TupleStr2 {
	T GFL_VECTOR_X;
	T GFL_VECTOR_Y;
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
	 * @param  tx 設定値
	 * @param  ty 設定値
	 * @return 無し
	 */

	Tuple2( const T tx, const T ty ) {
		Set( tx, ty );
	}

	/**
	 *   Tuple2 を設定するコンスタラクター
	 * @param  t Tuple2クラス
	 * @return 無し
	 */

	Tuple2( const Tuple2<T>& t ) {
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
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) ? TY(2)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T Get( const s32 i ) const {
		return &(GFL_TEMPLATE_PARAMETER GFL_VECTOR_X)[i];
	}

	/**
	 *   変数のポインターを取得
	 * @param  無し
	 * @return 変数のポインター
	 */

	inline T* Get( void ) {
		return &GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	inline const T* Get( void ) const {
		return &GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 TX(0) ? TY(2)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T& GetRef( const s32 i ) const {
		return &(GFL_TEMPLATE_PARAMETER GFL_VECTOR_X)[i];
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を設定
	 * @param  tx 設定値
	 * @return 無し
	 */

	inline void SetX( const T tx ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = tx;
	}

	/**
	 *   Ｙ成分を設定
	 * @param  ty 設定値
	 * @return 無し
	 */

	inline void SetY( const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = ty;
	}

	/**
	 *   指定した成分を設定
	 * @param  i 添え字 TX(0) ? TY(2)
	 * @param  val 設定値
	 * @return 無し
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline void SetElem( const s32 i, const T val ) {
		&(GFL_TEMPLATE_PARAMETER GFL_VECTOR_X)[i] = val;
	}

	/**
	 *   Ｘ，Ｙ成分を設定
	 * @param  tx 設定値
	 * @param  ty 設定値
	 * @return 無し
	 */

	inline void Set( const T tx, const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = tx;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = ty;
	}

	/**
	 *   Tuple2 を設定
	 * @param  t 設定したい Tuple の参照
	 * @return 無し
	 */


	inline void Set( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = t.GFL_VECTOR_Y;
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = static_cast<T> (0);
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


	inline void Add( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X += t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y += t.GFL_VECTOR_Y;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const Tuple2<T>& t, const Tuple2<T>& s ) {
		SetX( t.GFL_VECTOR_X + s.GFL_VECTOR_X );
		SetY( t.GFL_VECTOR_Y + s.GFL_VECTOR_Y );
	}

	/**
	 *   加算
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const T tx, const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X += tx;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y += ty;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */


	inline void Add( const Tuple2<T>& t, const T tx, const T ty ) {
		SetX( t.GFL_VECTOR_X + tx );
		SetY( t.GFL_VECTOR_Y + ty );
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


	inline void Sub( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X -= t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y -= t.GFL_VECTOR_Y;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const Tuple2<T>& t, const Tuple2<T>& s ) {
		SetX( t.GFL_VECTOR_X - s.GFL_VECTOR_X );
		SetY( t.GFL_VECTOR_Y - s.GFL_VECTOR_Y );
	}

	/**
	 *   減算
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */

	inline void Sub( const T tx, const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X -= tx;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y -= ty;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */


	inline void Sub( const Tuple2<T>& t, const T tx, const T ty ) {
		SetX( t.GFL_VECTOR_X - tx );
		SetY( t.GFL_VECTOR_Y - ty );
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
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X *= f;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y *= f;
	}

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */


	inline void Mul( const T f, const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = f * t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = f * t.GFL_VECTOR_Y;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */


	inline void Mul( const Tuple2<T>& t, const T f ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = f * t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = f * t.GFL_VECTOR_Y;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */


	inline void Mul( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const Tuple2<T>& t, const Tuple2<T>& s ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = t.GFL_VECTOR_X * s.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = t.GFL_VECTOR_Y * s.GFL_VECTOR_Y;
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
		return (f32) (GFL_TEMPLATE_PARAMETER GFL_VECTOR_X * GFL_TEMPLATE_PARAMETER GFL_VECTOR_X +
			GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y * GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y);
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

	inline f32 Distance( const Tuple2<T>& t ) const {
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
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = -GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = -GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y;
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */


	inline void Neg( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = -t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = -t.GFL_VECTOR_Y;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */


	inline T Dot( const Tuple2<T>& t ) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y * t.GFL_VECTOR_Y;
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

	inline void Lerp( const Tuple2<T>& t, const Tuple2<T>& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = t.GFL_VECTOR_X * rate_1 + s.GFL_VECTOR_X * rate;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = t.GFL_VECTOR_Y * rate_1 + s.GFL_VECTOR_Y * rate;
	}

	/**
	 *   直線補完
	 * @param  t 補完したいクラス
	 * @param  rate 内分値 (0.0f-1.0f)の範囲にないとアサートします。
	 * @return 無し
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	inline void Lerp( const Tuple2<T>& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   オペレーター []
	 * @param  i 添え字 TX(0) ? TZ(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T operator [](s32 i) const {
		return Get( i );
	}

	/**
	 *   オペレーター =
	 * @param  t 代入したい Tuple2<T>
	 * @return this の参照
	 */

	inline Tuple2<T> & operator=(const Tuple2<T>& t) {
		Set( t );
		return *this;
	}

	/**
	 *   オペレーター +=
	 * @param  t 加算したいクラス
	 * @return this の参照
	 */

	inline Tuple2<T> & operator+=(const Tuple2<T>& t) {
		Add( t );
		return *this;
	}

	/**
	 *   オペレーター -=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	inline Tuple2<T> & operator-=(const Tuple2<T>& t) {
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

	inline Tuple2<T> & operator*=(const Tuple2<T>& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	inline bool operator==(const Tuple2<T>& t) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X == t.GFL_VECTOR_X && GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y == t.GFL_VECTOR_Y;
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	inline bool operator!=(const Tuple2<T>& t) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X != t.GFL_VECTOR_X || GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y != t.GFL_VECTOR_Y;
	}

	/**
	 *   オペレーター +
	 * @param  t 加算したいクラス
	 * @return 加算されたクラス Tuple2<T>
	 */

	inline Tuple2<T> operator+(const Tuple2<T>& t) const {
		Tuple2<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス Tuple2<T>
	 */

	inline Tuple2<T> operator-(const Tuple2<T>& t) const {
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

	inline Tuple2<T> operator*(const Tuple2<T>& t) const {
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
		Debug::PrintConsole( "%s %d %d\n", str, GFL_TEMPLATE_PARAMETER GFL_VECTOR_X, GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %d %d\n", str, i, GFL_TEMPLATE_PARAMETER GFL_VECTOR_X, GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER GFL_VECTOR_X, GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y );
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
	 * @param  tx 設定値
	 * @param  ty 設定値
	 * @return 無し
	 */

	Tuple2f( const f32 tx, const f32 ty ) : Tuple2<f32>::Tuple2( tx, ty ) {
	}

	/**
	 *   Tuple2f を設定するコンスタラクター
	 * @param  t Tuple2fクラス
	 * @return 無し
	 */


	Tuple2f( const Tuple2f& t ) : Tuple2<f32>::Tuple2( t ) {
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
		SetX( Math::FRecip( GFL_VECTOR_X ) );
		SetY( Math::FRecip( GFL_VECTOR_Y ) );
	}

	/**
	 *   逆数　ＴのＸ，Ｙ成分の逆数を設定する
	 * @param  t 逆数にしたいクラス
	 * @return 無し
	 */


	inline void Recip( const Tuple2f& t ) {
		SetX( Math::FRecip( t.GFL_VECTOR_X ) );
		SetY( Math::FRecip( t.GFL_VECTOR_Y ) );
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
		GFL_VECTOR_X *= f_1;
		GFL_VECTOR_Y *= f_1;
	}

	/**
	 *   除算
	 * @param  t 除算したいクラス
	 * @param  f 除算したい値
	 * @return 無し
	 * @remark t の各成分を f で除算する 内部的には逆数の掛け算
	 */

	inline void Div( const Tuple2f& t, const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		SetX( f_1 * t.GFL_VECTOR_X );
		SetY( f_1 * t.GFL_VECTOR_Y );
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

	inline void Normalize( const Tuple2f& t ) {
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

	inline f32 NormalizeRet( const Tuple2f& t ) {
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

	inline bool IsSimilar( const Tuple2f& t, const f32 epsilon = Math::EPSILON ) const {
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
		SetX( Math::FAbs( GFL_VECTOR_X ) );
		SetY( Math::FAbs( GFL_VECTOR_Y ) );
	}

	/**
	 *   t の各成分の絶対値を設定する
	 * @param  t 入力クラス
	 * @return 無し
	 */

	inline void Abs( const Tuple2f& t ) {
		SetX( Math::FAbs( t.GFL_VECTOR_X ) );
		SetY( Math::FAbs( t.GFL_VECTOR_Y ) );
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
		Debug::PrintConsole( "%s %f %f\n", str, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %f %f\n", str, i, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   コンソールに各成分を出力
	 * @param  str 文字列
	 * @param  i 値　添え字などを表示したいとき
	 * @param  j 値　添え字などを表示したいとき
	 * @return 無し
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %f %f\n", str, i, j, GFL_VECTOR_X, GFL_VECTOR_Y );
	}
};

#endif	/* __GFL_TUPLE2_H__ */


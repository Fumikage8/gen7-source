//==============================================================================
/**
 * @file	gfl_VectorNative2.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/18, 16:26
 */
// =============================================================================

#if !defined( __GFL_VECTORNATIVE2_H__ )
#define	__GFL_VECTORNATIVE2_H__

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================
/**
 *  ２つの要素を持つタプル構造体
 */
struct VectorStruct2 {
	f32 GFL_VECTOR_X;
	f32 GFL_VECTOR_Y;
};

/**
 * ネイティブ命令クラス
 */

class VectorNative2 : public VectorStruct2 {
public:
    typedef VectorNative2 self_type; /**< 自分の型です */
    typedef f32  value_type; /**< 要素の型です */
	static const s32 DIMENSION = 2;

	/**
	 * デフォルトコンストラクタ　値は不定
	 */
	VectorNative2( ) {
	}

	/**
	 * コンストラクタ
	 * @param vp 設定したい浮動小数点ポインタ
	 */
	explicit VectorNative2( const f32* vp ) {
		Set( *vp, *(vp + 1) );
	}

	/**
	 * コンストラクタ
	 * @param vn　設定したいVectorNativeクラス
	 */
	VectorNative2( const VectorNative2& vn ) {
		Set( vn.GFL_VECTOR_X, vn.GFL_VECTOR_Y );
	}

	/**
	 * コンストラクタ
	 * @param fx　設定したいX座標
	 * @param fy　設定したいY座標
	 */
	VectorNative2( f32 fx, f32 fy ) {
		Set( fx, fy );
	}

	/**
	 * デストラクタ　何もしない
	 */
	~VectorNative2( ) {
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
	 */

	inline self_type operator-() const {
		return GetNeg( );
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
	 * @return 除算されたクラス Vector2f
	 */

	inline self_type operator/(const f32 f) const {
		self_type r;
		r.Div( *this, f );
		return r;
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

	inline void Lerp( const self_type& t, const self_type& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_VECTOR_X = t.GFL_VECTOR_X * rate_1 + s.GFL_VECTOR_X * rate;
		GFL_VECTOR_Y = t.GFL_VECTOR_Y * rate_1 + s.GFL_VECTOR_Y * rate;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   内積
	 * @param  t 内積を求めたいクラス
	 * @return 内積値
	 */

	inline f32 Dot( const self_type& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Y * t.GFL_VECTOR_Y;
	}

	/**
	 *   大きさの二乗 LenSqと同じ？
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline f32 LengthSquare( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Y * GFL_VECTOR_Y;
	}

	/**
	 *   大きさ
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 Length( void ) const {
		return Math::FSqrt( LenSq( ) );
	}

	/**
	 *   大きさの二乗
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline f32 LenSq( void ) const {
		return LengthSquare( );
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
		Mul( Math::FRecip( Length( ) ) );
	}

	/**
	 *   ベクトルを正規化します。 正規化に失敗した場合は指定されたベクトルを設定します。
	 * @param  
	 * @return 無し
	 */

	inline void SafeNormalize( const self_type& def ) {
		const f32 div = Length( );
		if( div != 0.0f ){
			Mul( Math::FRecip( div ) );
		} else {
			Set( def.GFL_VECTOR_X, def.GFL_VECTOR_Y );
		}
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   距離の二乗
	 * @param  t 距離を求めたいクラス
	 * @return 距離の二乗
	 */
	inline f32 DistanceSquare( const self_type& t ) const {
		self_type r;
		r.Sub( *this, t );
		return r.LengthSquare( );
	}

	/**
	 * 2つのベクトルのそれぞれの成分の最大値から構成されるベクトルを作成し設定します。
	 * @param l 入力ベクトル
	 * @param r 入力ベクトル
	 * @return this 参照
	 */
	self_type& Maximize( const self_type& l, const self_type& r ) {
		Set( Math::Greater( l.GFL_VECTOR_X, r.GFL_VECTOR_X ), Math::Greater( l.GFL_VECTOR_Y, r.GFL_VECTOR_Y ) );
		return *this;
	}

	/**
	 * 2つのベクトルのそれぞれの成分の最小値から構成されるベクトルを作成し設定します。
	 * @param l 入力ベクトル
	 * @param r 入力ベクトル
	 * @return this 参照
	 */
	self_type& Minimize( const self_type& l, const self_type& r ) {
		Set( Math::Lesser( l.GFL_VECTOR_X, r.GFL_VECTOR_X ), Math::Lesser( l.GFL_VECTOR_Y, r.GFL_VECTOR_Y ) );
		return *this;
	}

	/**
	 *   Ｘ，Ｙ成分を設定
	 * @param  tx 設定値
	 * @param  ty 設定値
	 * @return 無し
	 */

	inline void Set( const f32 tx, const f32 ty ) {
		GFL_VECTOR_X = tx;
		GFL_VECTOR_Y = ty;
	}

	/**
	 * コピー
	 * @param t　参照元
	 */
	inline void Set( const self_type& t ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y );
	}

	/**
	 *   オペレーター ==
	 * @param  t 比較したいクラス
	 * @return 各成分が一致したとき真、しないときに偽
	 */

	inline bool operator==(const self_type& t) const {
		return GFL_VECTOR_X == t.GFL_VECTOR_X && GFL_VECTOR_Y == t.GFL_VECTOR_Y;
	}

	/**
	 *   オペレーター !=
	 * @param  t 比較したいクラス
	 * @return 各成分の全てが同じときに偽、それ以外は偽
	 */

	inline bool operator!=(const self_type& t) const {
		return GFL_VECTOR_X != t.GFL_VECTOR_X || GFL_VECTOR_Y != t.GFL_VECTOR_Y;
	}

	/**
	 * ゼロベクトルか？
	 * @return 真　０ベクトルの時　疑　０ベクトルじゃない時
	 */
	inline bool IsZero( ) {
		return *this == Zero( );
	}

	/**
	 * すべての要素が 0 のベクトルを取得
	 * @return すべての要素が 1 のベクトル
	 */
	static const self_type& Zero( ) {
		return static_cast<const self_type&> (s_Zero);
	}

	/**
	 * すべての要素が 1 のベクトルを取得
	 * @return すべての要素が 1 のベクトル
	 */
	static const self_type& One( ) {
		return static_cast<const self_type&> (s_One);
	}

	/**
	 * 状態を出力
	 * @param newline 改行する
	 * @param str 表示文字列
	 */
	void Report( bool newline = true, const c8* str = 0 ) {
		Debug::PrintConsole( "%s { %ff, %ff },", str, GFL_VECTOR_X, GFL_VECTOR_Y );
		if( newline ){
			Debug::PrintConsoleDirect("\n");
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
	}

	/**
	 *   加算
	 * @param  t 加算したいクラスの参照
	 * @param  s 加算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を加算する
	 */

	inline void Add( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X + s.GFL_VECTOR_X, t.GFL_VECTOR_Y + s.GFL_VECTOR_Y );
	}

	/**
	 *   加算
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const f32 tx, const f32 ty ) {
		GFL_VECTOR_X += tx;
		GFL_VECTOR_Y += ty;
	}

	/**
	 *   加算
	 * @param  t 加算したいクラス
	 * @param  tx 加算したいＸ値
	 * @param  ty 加算したいＹ値
	 * @return 無し
	 */

	inline void Add( const self_type& t, const f32 tx, const f32 ty ) {
		Set( t.GFL_VECTOR_X + tx, t.GFL_VECTOR_Y + ty );
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
	}

	/**
	 *   減算
	 * @param  t 減算したいクラスの参照
	 * @param  s 減算したいクラスの参照
	 * @return 無し
	 * @remark 各成分同士を減算する
	 */

	inline void Sub( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X - s.GFL_VECTOR_X, t.GFL_VECTOR_Y - s.GFL_VECTOR_Y );
	}

	/**
	 *   減算
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */

	inline void Sub( const f32 tx, const f32 ty ) {
		GFL_VECTOR_X -= tx;
		GFL_VECTOR_Y -= ty;
	}

	/**
	 *   減算
	 * @param  t 減算したいクラス
	 * @param  tx 減算したいＸ値
	 * @param  ty 減算したいＹ値
	 * @return 無し
	 */


	inline void Sub( const self_type& t, const f32 tx, const f32 ty ) {
		Set( t.GFL_VECTOR_X - tx, t.GFL_VECTOR_Y - ty );
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
	}

	/**
	 *   乗算
	 * @param  f 乗算したい値
	 * @param  t 乗算したいクラス
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Mul( const f32 f, const self_type& t ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y );
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  f 乗算したい値
	 * @return 無し
	 * @remark f と t の各成分を乗算する
	 */

	inline void Mul( const self_type& t, const f32 f ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y );
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
	}

	/**
	 *   乗算
	 * @param  t 乗算したいクラス
	 * @param  s 乗算したいクラス
	 * @return 無し
	 * @remark 各成分同士を乗算する
	 */

	inline void Mul( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X, t.GFL_VECTOR_Y * s.GFL_VECTOR_Y );
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

	inline void Div( const self_type& t, const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		Set( f_1 * t.GFL_VECTOR_X, f_1 * t.GFL_VECTOR_Y );
	}

	/**
	 *   ネゲート　各成分の符号を反転させる
	 * @param  t ネゲートしたいクラス
	 * @return 無し
	 */

	inline void Neg( const self_type& t ) {
		Set( -t.GFL_VECTOR_X, -t.GFL_VECTOR_Y );
	}

	inline self_type GetNeg( void ) const {
		self_type ret;
		ret.Neg( *this );
		return ret;
	}

	static const VectorStruct2 s_Zero;
	static const VectorStruct2 s_One;
};

#endif	/* __GFL_VECTORNATIVE2_H__ */


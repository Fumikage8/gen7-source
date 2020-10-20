//==============================================================================
/**
 * @file	gfl_VectorNative4.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/18, 16:26
 */
// =============================================================================

#if !defined( __GFL_VECTORNATIVE4_H__ )
#define	__GFL_VECTORNATIVE4_H__

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================
/**
 *  ２つの要素を持つタプル構造体
 */

struct VectorStruct4 {
	f32 GFL_VECTOR_X;
	f32 GFL_VECTOR_Y;
	f32 GFL_VECTOR_Z;
	f32 GFL_VECTOR_W;
};

/**
 * ネイティブ命令クラス
 */

class VectorNative4 : public VectorStruct4 {
public:
    typedef VectorNative4 self_type; /**< 自分の型です */
    typedef f32  value_type; /**< 要素の型です */
	static const s32 DIMENSION = 4;

	/**
	 * デフォルトコンストラクタ　値は不定
	 */
	VectorNative4( ) {
	}

	/**
	 * コンストラクタ
	 * @param vn　設定したいVectorNativeクラス
	 */
	VectorNative4( const VectorNative4& vn ) {
		Set( vn.GFL_VECTOR_X, vn.GFL_VECTOR_Y, vn.GFL_VECTOR_Z, vn.GFL_VECTOR_W );
	}

	/**
	 * コンストラクタ
	 * @param vn　設定したいVectorNativeクラス
	 */
	VectorNative4( const VectorNative3& vn ) {
		Set( vn.GFL_VECTOR_X, vn.GFL_VECTOR_Y, vn.GFL_VECTOR_Z, 0.0f );
	}

	/**
	 * コンストラクタ
	 * @param fx　設定したいX座標
	 * @param fy　設定したいY座標
	 * @param fz　設定したいZ座標
	 * @param fw　設定したいW座標
	 */
	VectorNative4( f32 fx, f32 fy, f32 fz, f32 fw ) {
		Set( fx, fy, fz, fw );
	}

	/**
	 * デストラクタ　何もしない
	 */
	~VectorNative4( ) {
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
		GFL_VECTOR_Z = t.GFL_VECTOR_Z * rate_1 + s.GFL_VECTOR_Z * rate;
		GFL_VECTOR_W = t.GFL_VECTOR_W * rate_1 + s.GFL_VECTOR_W * rate;
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
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Y * t.GFL_VECTOR_Y
			+ GFL_VECTOR_Z * t.GFL_VECTOR_Z + GFL_VECTOR_W * t.GFL_VECTOR_W;
	}

	/**
	 *   大きさの二乗 LenSqと同じ？
	 * @param  無し
	 * @return 大きさの二乗
	 */

	inline f32 LengthSquare( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Y * GFL_VECTOR_Y
			+ GFL_VECTOR_Z * GFL_VECTOR_Z + GFL_VECTOR_W * GFL_VECTOR_W;
	}

	/**
	 *   大きさ
	 * @param  無し
	 * @return 大きさ
	 */

	inline f32 Length( void ) const {
		return Math::FSqrt( LengthSquare( ) );
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
			Set( def.GFL_VECTOR_X, def.GFL_VECTOR_Y, def.GFL_VECTOR_Z, def.GFL_VECTOR_W );
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
		return r.LenSq( );
	}

	/**
	 * 2つのベクトルのそれぞれの成分の最大値から構成されるベクトルを作成し設定します。
	 * @param l 入力ベクトル
	 * @param r 入力ベクトル
	 * @return this 参照
	 */
	self_type& Maximize( const self_type& l, const self_type& r ) {
		Set( Math::Greater( l.GFL_VECTOR_X, r.GFL_VECTOR_X ), Math::Greater( l.GFL_VECTOR_Y, r.GFL_VECTOR_Y ),
			 Math::Greater( l.GFL_VECTOR_Z, r.GFL_VECTOR_Z ), Math::Greater( l.GFL_VECTOR_W, r.GFL_VECTOR_W ) );
		return *this;
	}

	/**
	 * 2つのベクトルのそれぞれの成分の最大値から構成されるベクトルを作成し設定します。
	 * @param l 入力ベクトル
	 * @param r 入力ベクトル
	 * @return this 参照
	 */
	self_type& Minimize( const self_type& l, const self_type& r ) {
		Set( Math::Lesser( l.GFL_VECTOR_X, r.GFL_VECTOR_X ), Math::Lesser( l.GFL_VECTOR_Y, r.GFL_VECTOR_Y ),
			 Math::Lesser( l.GFL_VECTOR_Z, r.GFL_VECTOR_Z ), Math::Lesser( l.GFL_VECTOR_W, r.GFL_VECTOR_W ) );
		return *this;
	}

	/**
	 *   各成分を設定
	 * @param  tx 設定値
	 * @param  ty 設定値
	 * @param  tz 設定値
	 * @param  tw 設定値
	 * @return 無し
	 */

	inline void Set( const f32 tx, const f32 ty, const f32 tz, const f32 tw=0.0f ) {
		GFL_VECTOR_X = tx;
		GFL_VECTOR_Y = ty;
		GFL_VECTOR_Z = tz;
		GFL_VECTOR_W = tw;
	}

	/**
	 * コピー
	 * @param t　参照元
	 */
	inline void Set( const self_type& t ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z, t.GFL_VECTOR_W );
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
	 * ゼロベクトルか？
	 * @return 真　０ベクトルの時　疑　０ベクトルじゃない時
	 */
	inline bool IsZero( ) {
		return *this == Zero( );
	}

	/**
	 * (0,0,0,1)ベクトルか？
	 * @return 真　０ベクトルの時　疑　０ベクトルじゃない時
	 */
	bool IsZeroWOne( );

	/**
	 * すべての要素が 0 のベクトルを取得
	 * @return すべての要素が 1 のベクトル
	 */
	static const self_type& Zero( ) {
		return static_cast<const self_type&> (s_Zero);
	}

	/**
	 * すべての要素が 0 のベクトルを取得
	 * @return すべての要素が 1 のベクトル
	 */
	static const self_type& ZeroWOne( );

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
		Debug::PrintConsole( "%s { %ff, %ff, %ff, %ff },", str, GFL_VECTOR_X, GFL_VECTOR_Y, GFL_VECTOR_Z, GFL_VECTOR_W );
		if(newline){
			Debug::PrintConsole( "\n" );
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
		f32 f_1 = Math::FRecip( f );
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
		f32 f_1 = Math::FRecip( f );
		Set( f_1 * t.GFL_VECTOR_X, f_1 * t.GFL_VECTOR_Y, f_1 * t.GFL_VECTOR_Z, f_1 * t.GFL_VECTOR_W );
	}

	static const VectorStruct4 s_Zero;
	static const VectorStruct4 s_One;
};

#endif	/* __GFL_VECTORNATIVE4_H__ */


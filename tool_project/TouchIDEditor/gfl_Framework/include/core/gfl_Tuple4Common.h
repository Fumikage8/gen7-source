//==============================================================================
/**
 * @file	gfl_Tuple4.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/24, 16:30
 */
// =============================================================================

//#if !defined( __GFL_TUPLE4_H__ )
//#define	__GFL_TUPLE4_H__

/**
 * Vector4 と Quaternion の共通メソッドを定義
 * template にするとインテリセンスが効かないのでマクロ化
 */

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
 *   Ｗ成分を取得
 * @param  無し
 * @return Ｗ成分
 */

inline f32 GetW( void ) const {
	return GFL_VECTOR_W;
}

/**
 *   指定された成分を取得
 * @param  i 添え字 TX(0) ? TW(3)
 * @return 指定された成分
 */
inline f32 Get( const s32 i ) const {
	return ( &GFL_VECTOR_X)[i];
}

/**
 * f32 としてポインタを取得
 * @return f32 ポインタ
 */
inline f32* Get( void ) {
	return ( &GFL_VECTOR_X);
}

/**
 * const f32 としてポインタを取得
 * @return const f32 ポインタ
 */
inline const f32* Get( void ) const {
	return ( &GFL_VECTOR_X);
}

/**
 *   Ｘ成分参照を取得
 * @param  無し
 * @return Ｘ成分
 */
inline f32& GetRefX( void ) {
	return GFL_VECTOR_X;
}

/**
 *   Ｙ成分参照を取得
 * @param  無し
 * @return Ｙ成分
 */
inline f32& GetRefY( void ) {
	return GFL_VECTOR_Y;
}

/**
 *   Z成分参照を取得
 * @param  無し
 * @return Z成分
 */
inline f32& GetRefZ( void ) {
	return GFL_VECTOR_Z;
}

/**
 *   W成分参照を取得
 * @param  無し
 * @return W成分
 */
inline f32& GetRefW( void ) {
	return GFL_VECTOR_W;
}

/**
 *   指定された成分参照を取得
 * @param  i 添え字 TX(0) ? TZ(3)
 * @return 指定された成分
 */
inline f32& GetRef( const s32 i ) {
	return ( &GFL_VECTOR_X)[i];
}

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
 *   Ｗ成分を設定
 * @param  tw 設定値
 * @return 無し
 */
inline void SetW( const f32 tw ) {
	GFL_VECTOR_W = tw;
}

/**
 *   Ｘ，Ｙ，Ｚ，Ｗ成分を設定
 * @param  tx 設定値
 * @param  ty 設定値
 * @param  tz 設定値
 * @param  tw 設定値 指定しない場合は 0.0f が入る
 * @return 無し
 */
inline void Set( const f32 tx, const f32 ty, const f32 tz, const f32 tw ) {
	GFL_VECTOR_X = tx;
	GFL_VECTOR_Y = ty;
	GFL_VECTOR_Z = tz;
	GFL_VECTOR_W = tw;
}

/**
 *   指定した成分を設定
 * @param  i 添え字 TX(0) ? TW(3)
 * @param  val 設定値
 * @return 無し
 */
inline void Set( const s32 i, const f32 val ) {
	(&GFL_VECTOR_X)[i] = val;
}

/**
 * XYZWそれぞれの成分を設定
 * @param t　入力
 */
inline void Set( const parent_type& t ) {
	Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z, t.GFL_VECTOR_W );
}
/**
 *   Ｘ，Ｙ，Ｚ成分に０を設定しＷ成分に 1 を設定する
 * @param  無し
 * @return 無し
 */
void SetUnit( void );

/**
 *   オペレーター []
 * @param  i 添え字 TX(0) ? TW(3)
 * @return 指定された成分
 */
inline f32 operator [](s32 i) const {
	return Get( i );
}

/**
 *   オペレーター =
 * @param  t 代入したい入力クラス
 * @return this の参照
 */
inline self_type& operator=(const parent_type& t) {
	Set( t );
	return *this;
}

/**
 *   オペレーター ==
 * @param  t 比較したいクラス
 * @return 各成分が一致したとき真、しないときに偽
 * @remark フロート誤差を考慮した比較
 */
inline bool operator==(const parent_type& t) const {
	return IsSimilar( t );
}

/**
 *   オペレーター !=
 * @param  t 比較したいクラス
 * @return 各成分の全てが同じときに偽、それ以外は偽
 * @remark フロート誤差を考慮した比較
 */
inline bool operator!=(const parent_type& t) const {
	return !IsSimilar( t );
}

/**
 *   逆数　Ｘ，Ｙ，Ｚ成分の逆数を設定する
 * @param  無し
 * @return 無し
 */
inline void Recip( void ) {
	Set( Math::FRecip( GFL_VECTOR_X ),
		 Math::FRecip( GFL_VECTOR_Y ),
		 Math::FRecip( GFL_VECTOR_Z ),
		 Math::FRecip( GFL_VECTOR_W ) );
}

/**
 *   逆数　ＴのＸ，Ｙ，Ｚ成分の逆数を設定する
 * @param  t 逆数にしたいクラス
 * @return 無し
 */
inline void Recip( const parent_type& t ) {
	Set( Math::FRecip( t.GFL_VECTOR_X ), Math::FRecip( t.GFL_VECTOR_Y ), Math::FRecip( t.GFL_VECTOR_Z ), Math::FRecip( t.GFL_VECTOR_W ) );
}

/**
 *   各成分が似ているか？
 * @param  t 対象クラス
 * @param  epsilon 誤差範囲　指定しない場合は EPSILON
 * @return 各成分値の誤差が epsilon 内なら真、それ以外は偽
 */
inline bool IsSimilar( const parent_type& t, const f32 epsilon = Math::EPSILON ) const {
	return Math::FAbs( t.GFL_VECTOR_X - GFL_VECTOR_X ) <= epsilon &&
		Math::FAbs( t.GFL_VECTOR_Y - GFL_VECTOR_Y ) <= epsilon &&
		Math::FAbs( t.GFL_VECTOR_Z - GFL_VECTOR_Z ) <= epsilon &&
		Math::FAbs( t.GFL_VECTOR_W - GFL_VECTOR_W ) <= epsilon;
}

/**
 * ほとんど０のベクトルか？
 * @param epsilon 誤差
 * @return 真　ほとんど０　疑　ほとんど０ではない
 */
inline bool IsAlmostZero( const f32 epsilon = Math::EPSILON ) const {
	return Math::IsAlmostZero( GFL_VECTOR_X, epsilon ) &&
		Math::IsAlmostZero( GFL_VECTOR_Y, epsilon ) &&
		Math::IsAlmostZero( GFL_VECTOR_Z, epsilon ) &&
		Math::IsAlmostZero( GFL_VECTOR_W, epsilon );
}

/**
 *   各成分の絶対値を設定する
 * @param  無し
 * @return 無し
 */
inline void Abs( void ) {
	Set( Math::FAbs( GFL_VECTOR_X ), Math::FAbs( GFL_VECTOR_Y ), Math::FAbs( GFL_VECTOR_Z ), Math::FAbs( GFL_VECTOR_W ) );
}

/**
 *   t の各成分の絶対値を設定する
 * @param  t 入力クラス
 * @return 無し
 */
inline void Abs( const VectorNative4& t ) {
	Set( Math::FAbs( t.GFL_VECTOR_X ), Math::FAbs( t.GFL_VECTOR_Y ), Math::FAbs( t.GFL_VECTOR_Z ), Math::FAbs( t.GFL_VECTOR_W ) );
}

/**
 *   コンソールに各成分を出力
 * @return 無し
 */
inline void PrintConsole( void ) const {
	Debug::PrintConsole( "{ %ff, %ff, %ff, %ff },\n",
						 GFL_VECTOR_X, GFL_VECTOR_Y,
						 GFL_VECTOR_Z, GFL_VECTOR_W );
}

/**
 *   コンソールに各成分を出力
 * @param  str 文字列
 * @return 無し
 */
inline void PrintConsole( const c8 * const str ) const {
	Debug::PrintConsole( "%s ", str );
	PrintConsole( );
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
 *   コンソールに各成分を度数で出力
 * @return 無し
 */
inline void PrintConsoleDegree( void ) const {
	Debug::PrintConsole( "{ %ff, %ff, %ff },\n",
						 gfl::core::Math::RadianToDegree( GFL_VECTOR_X ),
						 gfl::core::Math::RadianToDegree( GFL_VECTOR_Y ),
						 gfl::core::Math::RadianToDegree( GFL_VECTOR_Z ) );
}

/**
 *   コンソールに各成分を度数で出力
 * @param  str 文字列
 * @return 無し
 */
inline void PrintConsoleDegree( const c8 * const str ) const {
	Debug::PrintConsole( "%s ", str );
	PrintConsoleDegree( );
}

/**
 *   コンソールに各成分を度数で出力
 * @param  str 文字列
 * @param  i 値　添え字などを表示したいとき
 * @return 無し
 */
inline void PrintConsoleDegree( const c8 * const str, const s32 i ) const {
	Debug::PrintConsole( "%s[%d]", str, i );
	PrintConsoleDegree( );
}

/**
 *   コンソールに各成分を度数で出力
 * @param  str 文字列
 * @param  i 値　添え字などを表示したいとき
 * @param  j 値　添え字などを表示したいとき
 * @return 無し
 */
inline void PrintConsoleDegree( const c8 * const str, const s32 i, const s32 j ) const {
	Debug::PrintConsole( "%s[%d][%d]", str, i, j );
	PrintConsoleDegree( );
}

#ifdef GFL_PLATFORM_3DS

/**
 *   加算
 * @param  t 加算したいクラスの参照
 * @return 無し
 * @remark 各成分同士を加算する
 */
inline void Add( const parent_type& t ) {
	*this += t;
}

/**
 *   加算
 * @param  t 加算したいクラスの参照
 * @param  s 加算したいクラスの参照
 * @return 無し
 * @remark 各成分同士を加算する
 */
inline void Add( const parent_type& t, const parent_type& s ) {
	*this = t + s;
}

/**
 *   減算
 * @param  t 減算したいクラスの参照
 * @return 無し
 * @remark 各成分同士を減算する
 */
inline void Sub( const parent_type& t ) {
	*this -= t;
}

/**
 *   減算
 * @param  t 減算したいクラスの参照
 * @param  s 減算したいクラスの参照
 * @return 無し
 * @remark 各成分同士を減算する
 */
inline void Sub( const parent_type& t, const parent_type& s ) {
	*this = t - s;
}

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
 * @param  t 乗算したいクラスの参照
 * @return 無し
 * @remark f とt の各成分を乗算して自身に設定する
 */
inline void Mul( const f32 f, const parent_type& t ) {
	*this = f * t;
}

/**
 *   乗算
 * @param  t 乗算したいクラスの参照
 * @param  f 乗算したい値=
 * @return 無し
 * @remark f とt の各成分を乗算して自身に設定する
 */
inline void Mul( const parent_type& t, const f32 f ) {
	*this = f * t;
}

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
inline void Div( const parent_type& t, const f32 f ) {
	*this = t / f;
}
#endif

//#endif	/* __GFL_TUPLE4_H__ */

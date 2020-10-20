//==============================================================================
/**
 * @file	gfl_MatrixCommon.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/12/20, 16:40
 */
// =============================================================================
// これは複数回インクルードされる

/**
 * f32 としてポインタを取得
 * @return f32 ポインタ
 */
inline f32* Get( void ) {
	return GFL_MTX_ARRAY1D;
}

/**
 * const f32 としてポインタを取得
 * @return const f32 ポインタ
 */
inline const f32* Get( void ) const {
	return GFL_MTX_ARRAY1D;
}

/**
 *   指定された成分を取得
 * @param  idx 添え字
 * @return 指定された成分の値
 */
inline f32 Get( const s32 idx ) const {
	return GFL_MTX_ARRAY1D[idx];
}

/**
 *   指定された成分を取得
 * @param  i 添え字
 * @param  j 添え字
 * @return 指定された成分の値
 */
inline f32 Get( const s32 i, const s32 j ) const {
	return GFL_MTX_ARRAY2D[i][j];
}

/**
 *   指定された成分を取得
 * @param  i 添え字
 * @param  j 添え字
 * @return 指定された成分の値
 */
inline f32& GetRef( const s32 i, const s32 j ) {
	return GFL_MTX_ARRAY2D[i][j];
}

/**
 *   指定された成分を取得
 * @param  i 添え字
 * @return 指定された成分の値
 */
inline f32 operator() (const s32 i) const {
	return GFL_MTX_ARRAY1D[i];
}

/**
 *   指定された成分を取得
 * @param  i 添え字
 * @param  j 添え字
 * @return 指定された成分の値
 */
inline f32 operator() (const s32 i, const s32 j) const {
	return GFL_MTX_ARRAY2D[i][j];
}

/**
 *   指定された成分の対角成分を取得
 * @param  i 添え字 0 ～ 3
 * @param  j 添え字 0 ～ 3
 * @return 指定された成分の値
 */

inline f32 GetDiagonal( const s32 i, const s32 j ) const {
	return GFL_MTX_ARRAY2D[j][i];
}

/**
 * 要素を設定
 * @param i　添字
 * @param val　値
 */
inline void Set( const s32 i, const f32 val ) {
	GFL_MTX_ARRAY1D[i] = val;
}

/**
 * 要素を設定
 * @param i　添字
 * @param j　添字
 * @param val　値
 */
inline void Set( const s32 i, const s32 j, const f32 val ) {
	GFL_MTX_ARRAY2D[i][j] = val;
}

/**
 *   Matrix ３Ｘ３成分を設定
 * @param  m0  要素 @param  m1  要素 @param  m2  要素 @param  m3  要素 @param  m4 要素
 * @param  m5  要素 @param  m6  要素 @param  m7  要素 @param  m8  要素
 * @return 無し
 */
void Set33( f32 m0, f32 m1, f32 m2,
			f32 m3, f32 m4, f32 m5,
			f32 m6, f32 m7, f32 m8 ) {
	GFL_MTX_00 = m0;
	GFL_MTX_01 = m1;
	GFL_MTX_02 = m2;
	GFL_MTX_10 = m3;
	GFL_MTX_11 = m4;
	GFL_MTX_12 = m5;
	GFL_MTX_20 = m6;
	GFL_MTX_21 = m7;
	GFL_MTX_22 = m8;
}

/**
 *   MATRIX_SOA_BASE の３Ｘ３成分を設定
 * @param  mat  行列
 * @return 無し
 */
template<typename T>
void Set33( const T& mat ) {
	SetRow( 0, reinterpret_cast<const VectorNative3&> (mat.GetRow( 0 )) );
	SetRow( 1, reinterpret_cast<const VectorNative3&> (mat.GetRow( 1 )) );
	SetRow( 2, reinterpret_cast<const VectorNative3&> (mat.GetRow( 2 )) );
}

/**
 * 行列の座標成分との加算
 * @param v0　出力ベクトルクラス
 * @param v1　加算する入力ベクトルクラス
 */
inline void Add( VectorNative* v0, const VectorNative& v1 ) const {
	v0->GFL_VECTOR_X = v1.GFL_VECTOR_X + GetTransX( );
	v0->GFL_VECTOR_Y = v1.GFL_VECTOR_Y + GetTransY( );
	v0->GFL_VECTOR_Z = v1.GFL_VECTOR_Z + GetTransZ( );
}

/**
 * 行列の座標成分との加算
 * @param v0　出力ベクトル構造体
 */
inline void Add( VectorNative* v0 ) const {
	v0->GFL_VECTOR_X += GetTransX( );
	v0->GFL_VECTOR_Y += GetTransY( );
	v0->GFL_VECTOR_Z += GetTransZ( );
	/* How should I treat W */
}

/**
 * 行列の座標成分との加算
 * @param v0　出力ベクトル構造体
 */
inline void Add( const VectorNative& v0 ) {
	GetTransRefX( ) += v0.GFL_VECTOR_X;
	GetTransRefY( ) += v0.GFL_VECTOR_Y;
	GetTransRefZ( ) += v0.GFL_VECTOR_Z;
	/* How should I treat W */
}

/**
 * ベクトルを角度成分に掛けたものを計算
 * @param v0　入出力ベクトルクラスポインタ
 */
inline void Apply( VectorNative* v0 ) const {
	Apply( v0, *v0 );
}

/**
 * 座標移動
 * @param v0　入力ベクトルクラス
 */
inline void Trans( const VectorNative& v0 ) {
	Vector v;
	Apply( &v, v0 );
	*this += v;
}

/**
 * 座標移動
 * @param x　ｘ成分
 * @param y　ｙ成分
 * @param z　ｚ成分
 */
inline void Trans( const f32 x, const f32 y, const f32 z ) {
	Vector v( x, y, z );
	Apply( &v, v );
	*this += v;
}

/**
 * 座標移動
 * @param v0　出力ベクトルクラスポインタ
 * @param v1　入力ベクトルクラス
 * @remark 行列に計算結果は反映されない
 */
inline void Trans( VectorNative* v0, const VectorNative& v1 ) const {
	Apply( v0, v1 );
	/*		v0->SetW( 0.0f ); */
	*v0 += GetTrans( );
}

/**
 * 座標移動
 * @param v0　出力ベクトルクラス
 * @param x　ｘ成分
 * @param y　ｙ成分
 * @param z　ｚ成分
 * @remark 行列に計算結果は反映されない
 */
inline void Trans( VectorNative* v0, const f32 x, const f32 y, const f32 z ) const {
	Vector v( x, y, z );
	Trans( v0, v );
}

/**
 *   行列の座標成分を設定
 * @param  mat 入力行列
 * @return 無し
 */

inline void SetTrans( const self_type& mat ) {
	SetTrans( mat.GetTrans( ) );
}

/**
 *   ３Ｘ３の決定因子
 * @param  m0 入力行列
 * @return 決定因子
 */
inline f32 Determinant33( const self_type& m0 ) const {
	return m0.Determinant33( );
}

/**
 *   決定因子
 * @param  m0 入力行列
 * @return 決定因子
 */
inline f32 Determinant( const self_type& m0 ) const {
	return m0.Determinant( );
}

/**
 *   ３Ｘ３の逆行列
 * @param  無し
 * @return 無し
 * @remark 座標成分には（０，０，０，１）が設定される。\n
 * スケールがかかってない場合は転置行列の使用をお勧めします。
 */

inline void Inverse33( void ) {
	Inverse33( *this );
}

/**
 * 逆行列を設定
 */
inline void Inverse( void ) {
	Inverse( *this );
}

// -----------------------------------------------------------------------------
// Transpose33
// -----------------------------------------------------------------------------

/**
 *   角度成分の転置行列を設定
 * @param  無し
 * @return 無し
 */
void Transpose33( void ) {
	Math::Swap( GFL_MTX_01, GFL_MTX_10 );
	Math::Swap( GFL_MTX_02, GFL_MTX_20 );
	Math::Swap( GFL_MTX_12, GFL_MTX_21 );
}

/**
 *   角度成分の転置行列を設定
 * @param  m0 入力行列
 * @return 無し
 */
void Transpose33( const self_type& m ) {
	GFL_MTX_00 = m.GFL_MTX_00;
	GFL_MTX_01 = m.GFL_MTX_10;
	GFL_MTX_02 = m.GFL_MTX_20;
	GFL_MTX_10 = m.GFL_MTX_01;
	GFL_MTX_11 = m.GFL_MTX_11;
	GFL_MTX_12 = m.GFL_MTX_21;
	GFL_MTX_20 = m.GFL_MTX_02;
	GFL_MTX_21 = m.GFL_MTX_12;
	GFL_MTX_22 = m.GFL_MTX_22;
}

/**
 *   角度成分の転置行列を求める
 * @param  無し
 * @return 転置された行列の実体
 */

self_type GetTranspose33( void ) const {
	self_type ret;
	ret.Transpose33( *this );
	return ret;
}

/**
 *   転置行列を求める
 * @param  無し
 * @return 転置された行列の実体
 */

self_type GetTranspose( void ) const {
	self_type ret;
	ret.Transpose( *this );
	return ret;
}

/**
 *   角度成分の転置行列を求める
 * @param  無し
 * @return 転置された行列の実体
 */

self_type GetInverse33( void ) const {
	self_type ret;
	ret.Inverse33( *this );
	return ret;
}

/**
 *   転置行列を求める
 * @param  無し
 * @return 転置された行列の実体
 */

self_type GetInverse( void ) const {
	self_type ret;
	ret.Inverse( *this );
	return ret;
}

/**
 * グローバル座標をローカル座標に変換
 * @param loc　ローカル座標　出力ベクトルクラスポインタ
 * @param glo　グローバル座標　入力ベクトル
 */
inline void GlobalToLocal( VectorNative* loc, const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	GlobalToLocal33( loc, pos );
}

/**
 * グローバル座標をローカル座標に変換
 * @param loc　ローカル座標　出力ベクトルクラスポインタ
 * @param glo　グローバル座標　入力ベクトル
 */
inline void GlobalToLocalScaled( VectorNative* loc, const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	GlobalToLocal33Scaled( loc, pos );
}

/**
 *   ターゲットポジションに骨を向けてＺＹＸ回転させる
 * @param  tag 目標座標
 * @return なし
 * @remark Ｘ軸を補正してＹ軸鉛直を維持する
 */


bool AimZYX( const VectorNative& tag ) {
	if( AimZY( tag ) ){
		AimX( );
		return true;
	}
	return false;
}

/**
 * ベクトルと行列の向きの前後方向が同じか
 * @param v　ベクトル
 * @return 真　同じ方向　偽　反対方向
 */
inline bool IsVectorSameDirection( const VectorNative& v ) const {
	return IsVectorSameDirection( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
}

/**
 * ベクトルと行列の向きの左右方向が同じか
 * @param v　ベクトル
 * @return 真　同じ方向　偽　反対方向
 */
inline bool IsVectorSameSide( const VectorNative& v ) const {
	return IsVectorSameSide( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
}

/**
 * ベクトルと行列の向きが同じ方向９０度以内か
 * @param v　ベクトル
 * @return 真　同じ方向９０度以内　偽　それ以外
 */
inline bool IsVectorSameDirectionDegree90( const VectorNative& v ) const {
	return IsVectorSameDirectionDegree90( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
}

/**
 * 座標が行列からみて前にあるか
 * @param pos　チェックしたい座標
 * @return 真　前　偽　後ろ
 */
inline bool IsPositionFront( const VectorNative& pos ) const {
	return IsVectorSameDirection( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * 座標が行列からみて左にあるか
 * @param pos　チェックしたい座標
 * @return 真　左　偽　右
 */
inline bool IsPositionLeft( const VectorNative& pos ) const {
	return IsVectorSameSide( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * 座標が行列からみて前９０度以内にあるか
 * @param pos　チェックしたい座標
 * @return 真　前９０度以内　偽　それ以外
 */
inline bool IsPositionFrontDegree90( const VectorNative& pos ) const {
	return IsVectorSameDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * 座標が行列からみて後ろ９０度以内にあるか
 * @param pos　チェックしたい座標
 * @return 真　後９０度以内　偽　それ以外
 */
inline bool IsPositionBackDegree90( const VectorNative& pos ) const {
	return IsVectorOppositeDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * 座標が行列からみて左９０度以内にあるか
 * @param pos　チェックしたい座標
 * @return 真　左９０度以内　偽　それ以外
 */
inline bool IsPositionLeftDegree90( const VectorNative& pos ) const {
	return IsVectorLeftDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * 座標が行列からみて右９０度以内にあるか
 * @param pos　チェックしたい座標
 * @return 真　右９０度以内　偽　それ以外
 */
inline bool IsPositionRightDegree90( const VectorNative& pos ) const {
	return IsVectorRightDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 *   行列のＸ軸で回転させた角度成分を設定
 * @param  rad 角度（ラジアン）
 * @return 無し
 * @remark 座標成分には何もしない
 */

inline void SetRotateX( const f32 rad ) {
	f32 s, c;
	Math::FSinCos( &s, &c, rad );
	SetRotateX( s, c );
}

/**
 * 行列にｘ回転を掛ける
 * @param m0　入力行列
 * @param rad　入力角度ラジアン
 * @remark m0 に rad の回転を掛けあわせたものを設定します。
 */
inline void RotateX( const self_type& m0, const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateX( m0, s, c );
}

/**
 * 行列にｘ回転を掛ける
 * @param rad　入力角度ラジアン
 * @remark 現在の行列に rad の回転を掛けあわせたものを設定します。
 */
inline void RotateX( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateX( s, c );
}

/**
 * 行列にｘ回転を掛ける
 * @param rad　入力角度ラジアン
 * @remark 現在の行列に rad の回転を逆から掛けあわせたものを設定します。
 */
inline void RotateX_Reverse( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateX_Reverse( s, c );
}

/**
 * 行列にｙ回転を設定
 * @param rad　入力角度ラジアン
 * @remark 元の角度成分はクリアされます。座標成分に対しては何もしません
 */
inline void SetRotateY( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	SetRotateY( s, c );
}

/**
 * 行列にｙ回転を掛ける
 * @param m0　入力行列
 * @param rad　入力角度ラジアン
 * @remark m0 に rad の回転を掛けあわせたものを設定します。
 */
inline void RotateY( const self_type& m0, const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateY( m0, s, c );
}

/**
 * 行列にｙ回転を掛ける
 * @param rad　入力角度ラジアン
 * @remark 現在の行列に rad の回転を掛けあわせたものを設定します。
 */
inline void RotateY( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateY( s, c );
}

/**
 * 行列にｙ回転を掛ける
 * @param rad　入力角度ラジアン
 * @remark 現在の行列に rad の回転を逆から掛けあわせたものを設定します。
 */
inline void RotateY_Reverse( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateY_Reverse( s, c );
}

/**
 * 行列にｚ回転を設定
 * @param rad　入力角度ラジアン
 * @remark 元の角度成分はクリアされます。座標成分に対しては何もしません
 */
inline void SetRotateZ( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	SetRotateZ( s, c );
}

/**
 * 行列にｚ回転を掛ける
 * @param m0　入力行列
 * @param rad　入力角度ラジアン
 * @remark m0 に rad の回転を掛けあわせたものを設定します。
 */
inline void RotateZ( const self_type& m0, const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateZ( m0, s, c );
}

/**
 * 行列にｚ回転を掛ける
 * @param rad　入力角度ラジアン
 * @remark 現在の行列に rad の回転を掛けあわせたものを設定します。
 */
inline void RotateZ( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateZ( s, c );
}

/**
 * 行列にｚ回転を掛ける
 * @param rad　入力角度ラジアン
 * @remark 現在の行列に rad の回転を逆から掛けあわせたものを設定します。
 */
inline void RotateZ_Reverse( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateZ_Reverse( s, c );
}

/**
 * 行列にZYXの順番で回転を掛けたものを設定
 * @param x　入力角度ラジアン
 * @param y　入力角度ラジアン
 * @param z　入力角度ラジアン
 */
inline void SetRotateZYX( const f32 x, const f32 y, const f32 z ) {
	SetRotateZ( z );
	RotateY( y );
	RotateX( x );
}

/**
 * 行列にZYXの順番で回転を掛けたものを設定
 * @param v　入力角度ラジアンベクトル構造体
 */
inline void SetRotateZYX( const VectorNative& v ) {
	SetRotateZYX( v.GFL_VECTOR_X, v.GFL_VECTOR_Y, v.GFL_VECTOR_Z );
}

/**
 * 行列にZYXの順番で回転を掛けたものを設定
 * @param v　x,y,z の順番で入った浮動小数点配列ポインタ
 */
inline void SetRotateZYX( const f32 v[] ) {
	SetRotateZYX( v[TX], v[TY], v[TZ] );
}

/**
 * 行列にZYXの順番で回転を掛ける
 * @param x　入力角度ラジアン
 * @param y　入力角度ラジアン
 * @param z　入力角度ラジアン
 */
inline void RotateZYX( const f32 x, const f32 y, const f32 z ) {
	RotateZ( z );
	RotateY( y );
	RotateX( x );
}

/**
 * 行列にZYXの順番で回転を掛ける
 * @param v　入力ベクトル構造体
 */
inline void RotateZYX( const VectorNative& v ) {
	RotateZYX( v.GFL_VECTOR_X, v.GFL_VECTOR_Y, v.GFL_VECTOR_Z );
}

/**
 * 行列にZYXの順番で回転を掛ける
 * @param v　x,y,z の順番で入った浮動小数点配列ポインタ
 */
inline void RotateZYX( const f32 v[] ) {
	RotateZYX( v[TX], v[TY], v[TZ] );
}

/**
 * スケールを設定する
 * @param scl　スケール値
 * @remark 元の角度成分はクリアされる。座標成分は変化なし
 */
inline void SetScale( const f32 scl ) {
	SetScale( scl, scl, scl );
}

/**
 * スケールを設定する
 * @param t　スケール値ベクトル
 * @remark 元の角度成分はクリアされる。座標成分は変化なし
 */
inline void SetScale( const VectorNative& t ) {
	SetScale( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
}

/**
 * スケール値を掛ける
 * @param scl　スケール値
 * @remark ｘｙｚ方向全てに等しいスケール値を掛ける
 */
inline void Scale( const f32 scl ) {
	ScaleX( scl );
	ScaleY( scl );
	ScaleZ( scl );
}

/**
 * スケール値を掛ける
 * @param x　ｘ方向スケール
 * @param y　ｙ方向スケール
 * @param z　ｚ方向スケール
 */
inline void Scale( const f32 x, const f32 y, const f32 z ) {
	ScaleX( x );
	ScaleY( y );
	ScaleZ( z );
}

/**
 * スケール値を掛ける
 * @param t　入力ベクトル構造体
 */
inline void Scale( const VectorNative& t ) {
	Scale( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
}

/**
 * スケール情報の取得
 * @param t スケール格納クラスポインタ
 */

inline void GetScale( VectorNative* t ) const {
	t->Set( GetScaleX( ), GetScaleY( ), GetScaleZ( ) );
}

/**
 * スケール値を逆から掛ける
 * @param scl　スケール値
 * @remark x,y,z成分全てに等しい scl を逆から掛ける
 */
inline void Scale_Reverse( const f32 scl ) {
	ScaleX_Reverse( scl );
	ScaleY_Reverse( scl );
	ScaleZ_Reverse( scl );
}

/**
 * スケール値を逆から掛ける
 * @param x　ｘスケール値
 * @param y　ｙスケール値
 * @param z　ｚスケール値
 */
inline void Scale_Reverse( const f32 x, const f32 y, const f32 z ) {
	ScaleX_Reverse( x );
	ScaleY_Reverse( y );
	ScaleZ_Reverse( z );
}

/**
 * スケール値を逆から掛ける
 * @param t　入力ベクトル構造体
 */
inline void Scale_Reverse( const VectorNative& t ) {
	Scale_Reverse( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
}

/**
 * スケール値の逆数を掛ける
 * @param scl　スケール値
 * @remark ｘｙｚ方向全てに等しいスケール値を掛ける
 */
inline void ScaleRecip( const f32 scl ) {
	const f32 r_scl = gfl::core::Math::FRecip( scl );
	ScaleX( r_scl );
	ScaleY( r_scl );
	ScaleZ( r_scl );
}

/**
 * スケール値の逆数を掛ける
 * @param t　入力ベクトル構造体
 */
inline void ScaleRecip( const VectorNative& t ) {
	Scale( gfl::core::Math::FRecip( t.GFL_VECTOR_X ), gfl::core::Math::FRecip( t.GFL_VECTOR_Y ), gfl::core::Math::FRecip( t.GFL_VECTOR_Z ) );
}

/**
 *   正規化
 * @param src 入力行列
 * @param  scl スケール値ベクトル
 * @return なし
 */
void Normalize( const self_type& src, VectorNative* scl = 0 ) {
	Set( src );
	Normalize( scl );
}

/**
 *   正規化
 * @param src 入力行列
 * @param  scl スケール値ベクトル
 * @return 正規化された行列インスタンス
 */
self_type GetNormalizedMatrix( VectorNative* scl = 0 ) {
	self_type mat( *this );
	mat.Normalize( scl );
	return mat;
}

/**
 *   座標成分を取得
 * @param  v 入力ベクトル
 * @return 座標
 */

inline void GetTrans( VectorNative* v ) const {
	v->Set( GetTrans( ) );
}

/**
 *   角度を行列から取得
 * @param  v 出力ラジアンのベクトル
 * @return 無し
 * @remark w は不定。ＺＹＸの順で回した場合の角度
 */

void GetRadian( VectorNative* v ) const {
	GetRadian( &v->GFL_VECTOR_X, &v->GFL_VECTOR_Y, &v->GFL_VECTOR_Z );
}

/**
 *   角度を行列から取得
 * @param  v 出力ラジアンのベクトル
 * @return 無し
 * @remark w は不定。ＹＺＸの順で回した場合の角度
 */
void GetRadianYXZ( VectorNative* v ) const {
	GetRadianYXZ( &v->GFL_VECTOR_X, &v->GFL_VECTOR_Y, &v->GFL_VECTOR_Z );
}

/**
 * クランプする　必要ないか…
 */
void Clamp( ) {
	Math::ClampAbs( &GFL_MTX_00, 1.0f );
	Math::ClampAbs( &GFL_MTX_01, 1.0f );
	Math::ClampAbs( &GFL_MTX_02, 1.0f );
	Math::ClampAbs( &GFL_MTX_10, 1.0f );
	Math::ClampAbs( &GFL_MTX_11, 1.0f );
	Math::ClampAbs( &GFL_MTX_12, 1.0f );
	Math::ClampAbs( &GFL_MTX_20, 1.0f );
	Math::ClampAbs( &GFL_MTX_21, 1.0f );
	Math::ClampAbs( &GFL_MTX_22, 1.0f );
}

/**
 * 座標値の加算
 * @param v　入力ベクトル
 * @remark 単純に座標を加算するので、回転成分は影響しない。\n
 * 回転を考慮した移動の場合は Trans を使用してください。
 */
inline void operator+=(const VectorStruct3& v) {
	GetTransRefX( ) += v.GFL_VECTOR_X;
	GetTransRefY( ) += v.GFL_VECTOR_Y;
	GetTransRefZ( ) += v.GFL_VECTOR_Z;
}

/**
 * 行列の差を取得
 * @param m　基準となる行列
 * @return 入力行列をどれだけ回転させたら自分の行列になるかの行列
 * @remark 単純な引き算でなく、this = m * (返り値)となる行列を計算する
 */
inline self_type operator-(const self_type& m) const {
	self_type mat( GetTranspose33( ) );
	mat.Mul33( m );
	return mat;
}

/**
 * 行列の積
 * @param m0　入力行列
 * @return 参照
 * @remark this = this * m0
 */
inline self_type operator*(const self_type& m0) const {
	self_type mat;
	mat.Mul( *this, m0 );
	return mat;
}

/**
 * 行列の積
 * @param m0　入力行列
 * @remark this = this * m0
 */
inline void Mul( const parent_type& m0 ) {
	Mul( *this, (const self_type&) m0 );
}

/**
 * ｘｙｚ軸方向にスケールをかける
 * @param sclx　ｘスケール値
 * @param scly　ｙスケール値
 * @param sclz　ｚスケール値
 */
void Mul( const f32 sclx, const f32 scly, const f32 sclz ) {
	ScaleX( sclx );
	ScaleY( scly );
	ScaleZ( sclz );
}

/**
 * ｘｙｚ軸方向にスケールをかける
 * @param scl　スケールベクトル
 */
void Mul( const VectorNative& scl ) {
	ScaleX( scl.GFL_VECTOR_X );
	ScaleY( scl.GFL_VECTOR_Y );
	ScaleZ( scl.GFL_VECTOR_Z );
}

/**
 * 角度成分のみに掛け算をする
 * @param m0　入力行列
 */
inline void Mul33( const self_type& m0 ) {
	Mul33( *this, m0 );
}

/**
 * 行列補完
 * @param m0　ターゲット行列
 * @param rate　割合　０　で自分自身　１でターゲット行列
 * @return 参照
 * @remark this = this * (1-rate) + m0 * rate
 */
inline self_type& Slerp( const self_type& m0, const f32 rate ) {
	return Slerp( *this, m0, rate );
}

/**
 *   コンソールに各成分を出力
 * @param  str 文字列
 * @return 無し
 */

inline void PrintConsole( const c8 * const str ) const {
	Debug::PrintConsole( "%s = \n", str );
	PrintConsole( );
}

/**
 *   コンソールに各成分を出力
 * @param  str 文字列
 * @param  i 値　添え字などを表示したいとき
 * @return 無し
 */

inline void PrintConsole( const c8 * const str, const s32 i ) const {
	Debug::PrintConsole( "%s[%d] = \n", str, i );
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
	Debug::PrintConsole( "%s[%d][%d] = \n", str, i, j );
	PrintConsole( );
}

/* 全共通にしてもいいけどSOA34 SOA44 でインスタンスが余計にできちゃうか */

/**
 *   コンソールに各成分を出力
 * @param  無し
 * @return 無し
 */
void PrintConsole33( void ) const {
	Debug::PrintConsole( "%ff, %ff, %ff,\n%ff, %ff, %ff,\n%ff, %ff, %ff\n",
						 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02,
						 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12,
						 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22 );
}

/**
 *   コンソールに各成分を出力
 * @param  str 文字列
 * @return 無し
 */

inline void PrintConsole33( const c8 * const str ) const {
	Debug::PrintConsole( "%s = \n", str );
	PrintConsole33( );
}

/**
 *   コンソールに各成分を出力
 * @param  str 文字列
 * @param  i 値　添え字などを表示したいとき
 * @return 無し
 */

inline void PrintConsole33( const c8 * const str, const s32 i ) const {
	Debug::PrintConsole( "%s[%d] = \n", str, i );
	PrintConsole33( );
}

/**
 *   コンソールに各成分を出力
 * @param  str 文字列
 * @param  i 値　添え字などを表示したいとき
 * @param  j 値　添え字などを表示したいとき
 * @return 無し
 */

inline void PrintConsole33( const c8 * const str, const s32 i, const s32 j ) const {
	Debug::PrintConsole( "%s[%d][%d] = \n", str, i, j );
	PrintConsole33( );
}


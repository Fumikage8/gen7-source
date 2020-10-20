//==============================================================================
/**
 * @file	gfl_MatrixAosCommon.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/12/20, 16:58
 */
// =============================================================================
#if 0 // 
//#include "gfl_MatrixCommon.h"

// -------------------------------------------------------------------------
// Apply
// -------------------------------------------------------------------------

/**
 *   角度スケール成分とベクトルの積算
 * @param  v0 出力ベクトル
 * @param  v1 入力ベクトル
 * @return 無し
 */


void Apply( VectorNative* v0, const VectorNative& v1 ) const {
	f32 x = v1.GFL_VECTOR_X;
	f32 y = v1.GFL_VECTOR_Y;
	f32 z = v1.GFL_VECTOR_Z;

	v0->Set( Get( 0, 0 ) * x + Get( 1, 0 ) * y + Get( 2, 0 ) * z,
			 Get( 0, 1 ) * x + Get( 1, 1 ) * y + Get( 2, 1 ) * z,
			 Get( 0, 2 ) * x + Get( 1, 2 ) * y + Get( 2, 2 ) * z );
}

/**
 *   角度スケール成分とＸ成分の積算
 * @param  v0 出力ベクトル
 * @param  val Ｘ成分
 * @return 無し
 */


inline void ApplyX( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 0 ) * val,
			 Get( 0, 1 ) * val,
			 Get( 0, 2 ) * val );
}

/**
 *   角度スケール成分とＸ１．０ｆ成分の積算
 * @param  v0 出力ベクトル
 * @return 無し
 */


inline void ApplyX1( VectorNative* v0 ) const {
	v0->Set( Get( 0, 0 ), Get( 0, 1 ), Get( 0, 2 ) );
}

/**
 *   角度スケール成分とＹ成分の積算
 * @param  v0 出力ベクトル
 * @param  val Ｙ成分
 * @return 無し
 */


inline void ApplyY( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 1, 0 ) * val, Get( 1, 1 ) * val, Get( 1, 2 ) * val );
}

/**
 *   角度スケール成分とＹ１．０ｆ成分の積算
 * @param  v0 出力ベクトル
 * @return 無し
 */


inline void ApplyY1( VectorNative* v0 ) const {
	v0->Set( Get( 1, 0 ), Get( 1, 1 ), Get( 1, 2 ) );
}

/**
 *   角度スケール成分とＺ成分の積算
 * @param  v0 出力ベクトル
 * @param  val Ｚ成分
 * @return 無し
 */


inline void ApplyZ( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 2, 0 ) * val, Get( 2, 1 ) * val, Get( 2, 2 ) * val );
}

/**
 *   角度スケール成分とＺ１．０ｆ成分の積算
 * @param  v0 出力ベクトル
 * @return 無し
 */


inline void ApplyZ1( VectorNative* v0 ) const {
	v0->Set( Get( 2, 0 ), Get( 2, 1 ), Get( 2, 2 ) );
}

/**
 *   角度スケール成分とベクトルの積算
 * @param  v0 出力ベクトル
 * @param  v1 入力ベクトル
 * @return 無し
 */


void ApplyTranspose( VectorNative* v0, const VectorNative& v1 ) const {
	f32 x = v1.GFL_VECTOR_X;
	f32 y = v1.GFL_VECTOR_Y;
	f32 z = v1.GFL_VECTOR_Z;

	v0->Set( Get( 0, 0 ) * x + Get( 0, 1 ) * y + Get( 0, 2 ) * z,
			 Get( 1, 0 ) * x + Get( 1, 1 ) * y + Get( 1, 2 ) * z,
			 Get( 2, 0 ) * x + Get( 2, 1 ) * y + Get( 2, 2 ) * z );
}

// -------------------------------------------------------------------------
// Trans
// -------------------------------------------------------------------------

/**
 *   Ｘ座標移動
 * @param  val Ｘ座標
 * @return 無し
 */

inline void TransX( const f32 val ) {
	GetTransRefX( ) += Get( 0, 0 ) * val;
	GetTransRefY( ) += Get( 0, 1 ) * val;
	GetTransRefZ( ) += Get( 0, 2 ) * val;
}

/**
 *   Ｙ座標移動
 * @param  val Ｙ座標
 * @return 無し
 */

inline void TransY( const f32 val ) {
	GetTransRefX( ) += Get( 1, 0 ) * val;
	GetTransRefY( ) += Get( 1, 1 ) * val;
	GetTransRefZ( ) += Get( 1, 2 ) * val;
}

/**
 *   Ｚ座標移動
 * @param  val Ｚ座標
 * @return 無し
 */

inline void TransZ( const f32 val ) {
	GetTransRefX( ) += Get( 2, 0 ) * val;
	GetTransRefY( ) += Get( 2, 1 ) * val;
	GetTransRefZ( ) += Get( 2, 2 ) * val;
}

/**
 *   Ｘ座標を移動したベクトルを取得
 * @param  v0 出力ベクトル
 * @param  val Ｘ座標
 * @return 無し
 */


inline void TransX( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 0 ) * val + GetTransX( ),
			 Get( 0, 1 ) * val + GetTransY( ),
			 Get( 0, 2 ) * val + GetTransZ( ) );
}

/**
 *   Ｙ標を移動したベクトルを取得
 * @param  v0 出力ベクトル
 * @param  val Ｙ座標
 * @return 無し
 */


inline void TransY( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 1, 0 ) * val + GetTransX( ),
			 Get( 1, 1 ) * val + GetTransY( ),
			 Get( 1, 2 ) * val + GetTransZ( ) );
}

/**
 *   Ｚ座標を移動したベクトルを取得
 * @param  v0 出力ベクトル
 * @param  val Ｚ座標
 * @return 無し
 */


inline void TransZ( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 2, 0 ) * val + GetTransX( ),
			 Get( 2, 1 ) * val + GetTransY( ),
			 Get( 2, 2 ) * val + GetTransZ( ) );
}

/**
 *   Ｘ座標成分を取得
 * @param  無し
 * @return Ｘ座標
 */

inline f32 GetTransX( void ) const {
	return Get( 3, TX );
}

/**
 *   Ｙ座標成分を取得
 * @param  無し
 * @return Ｙ座標
 */

inline f32 GetTransY( void ) const {
	return Get( 3, TY );
}

/**
 *   Ｚ座標成分を取得
 * @param  無し
 * @return Ｚ座標
 */

inline f32 GetTransZ( void ) const {
	return Get( 3, TZ );
}

/**
 *   Ｘ座標成分を取得
 * @param  無し
 * @return Ｘ座標
 */

inline f32& GetTransRefX( void ) {
	return GetRef( 3, TX );
}

/**
 *   Ｙ座標成分を取得
 * @param  無し
 * @return Ｙ座標
 */

inline f32& GetTransRefY( void ) {
	return GetRef( 3, TY );
}

/**
 *   Ｚ座標成分を取得
 * @param  無し
 * @return Ｚ座標
 */

inline f32& GetTransRefZ( void ) {
	return GetRef( 3, TZ );
}

/**
 *   座標成分を設定
 * @param  x Ｘ座標
 * @return 無し
 */

inline void SetTransX( const f32 x ) {
	Set( 3, TX, x );
}

/**
 *   座標成分を設定
 * @param  y Ｙ座標
 * @return 無し
 */

inline void SetTransY( const f32 y ) {
	Set( 3, TY, y );
}

/**
 *   座標成分を設定
 * @param  z Ｚ座標
 * @return 無し
 */

inline void SetTransZ( const f32 z ) {
	Set( 3, TZ, z );
}

/**
 *   座標成分を設定
 * @param  v 入力ベクトル
 * @return 無し
 */

inline void SetTrans( const VectorNative3& v ) {
	SetRow( 3, v );
}

/**
 *   座標成分を設定
 * @param  v 入力ベクトル
 * @return 無し
 */

inline void SetTrans( const VectorNative4& v ) {
	SetRow( 3, v );
}

/**
 *   グローバル座標をこの行列からみたローカル座標に変換
 * @param  loc ローカル座標
 * @param  pos グローバル座標
 * @return 無し
 */


void GlobalToLocal33( VectorNative* loc, const VectorNative& pos ) const {
	const f32 x = pos.GFL_VECTOR_X;
	const f32 y = pos.GFL_VECTOR_Y;
	const f32 z = pos.GFL_VECTOR_Z;
	loc->Set( Get( 0, 0 ) * x + Get( 0, 1 ) * y + Get( 0, 2 ) * z,
			 Get( 1, 0 ) * x + Get( 1, 1 ) * y + Get( 1, 2 ) * z,
			 Get( 2, 0 ) * x + Get( 2, 1 ) * y + Get( 2, 2 ) * z );
}

/**
 *   グローバル座標をこの行列からみたローカル座標に変換
 * @param  loc ローカル座標
 * @param  pos グローバル座標
 * @return 無し
 */
void GlobalToLocal33Scaled( VectorNative* loc, const VectorNative& pos ) const {
	self_type mat( GetInverse33( ) );
	const f32 x = pos.GFL_VECTOR_X;
	const f32 y = pos.GFL_VECTOR_Y;
	const f32 z = pos.GFL_VECTOR_Z;
	loc->Set( mat.Get( 0, 0 ) * x + Get( 1, 0 ) * y + Get( 2, 0 ) * z,
			 mat.Get( 0, 1 ) * x + Get( 1, 1 ) * y + Get( 2, 1 ) * z,
			 mat.Get( 0, 2 ) * x + Get( 1, 2 ) * y + Get( 2, 2 ) * z );
}

/**
 *   グローバル座標をこの行列からみたローカルＸ座標を求める
 * @param  glo グローバル座標
 * @return ローカルＸ座標
 */


f32 GlobalToLocalX( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 0, 0 ) * pos.GetX( ) + Get( 0, 1 ) * pos.GetY( ) + Get( 0, 2 ) * pos.GetZ( );
}

/**
 *   グローバル座標をこの行列からみたローカルＹ座標を求める
 * @param  glo グローバル座標
 * @return ローカルＹ座標
 */


f32 GlobalToLocalY( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 1, 0 ) * pos.GetX( ) + Get( 1, 1 ) * pos.GetY( ) + Get( 1, 2 ) * pos.GetZ( );
}

/**
 *   グローバル座標をこの行列からみたローカルＺ座標を求める
 * @param  glo グローバル座標
 * @return ローカルＺ座標
 */


f32 GlobalToLocalZ( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 2, 0 ) * pos.GetX( ) + Get( 2, 1 ) * pos.GetY( ) + Get( 2, 2 ) * pos.GetZ( );
}

// -------------------------------------------------------------------------
// マトリックスの向きとベクトルが同じ方向を向いているか？
// -------------------------------------------------------------------------

bool IsVectorSameDirection( const f32 x, const f32 z ) const {
	return ( 0.0f <= Get( 2, 0 ) * x + Get( 2, 2 ) * z);
}

bool IsVectorSameSide( const f32 x, const f32 z ) const {
	return ( 0.0f <= Get( 0, 0 ) * x + Get( 0, 2 ) * z);
}

bool IsVectorSameDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return Math::IsFrontDegree90( x, z );
}

bool IsVectorOppositeDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return Math::IsBackDegree90( x, z );
}

// -----------------------------------------------------------------------------
// operator
// -----------------------------------------------------------------------------

/**
 *   代入
 * @param  m 入力行列
 * @return 無し
 */

inline self_type& operator=(const self_type& m) {
	Set( m );
	return *this;
}

/**
 *   比較
 * @param  m0 比較対象行列
 * @return 各成分が全て完全に一致した場合は真、他は偽
 */
bool operator==(const self_type& m0) {
	return GetRow( 0 ) == m0.GetRow( 0 ) &&
		GetRow( 1 ) == m0.GetRow( 1 ) &&
		GetRow( 2 ) == m0.GetRow( 2 ) &&
		GetRow( 3 ) == m0.GetRow( 3 );
}

/**
 *   比較
 * @param  m0 比較対象行列
 * @return 各成分が全て完全に一致した場合は偽、他は真
 */
bool operator!=(const self_type& m0) {
	return GetRow( 0 ) != m0.GetRow( 0 ) ||
		GetRow( 1 ) != m0.GetRow( 1 ) ||
		GetRow( 2 ) != m0.GetRow( 2 ) ||
		GetRow( 3 ) != m0.GetRow( 3 );
}

/**
 *   誤差範囲を与えて比較
 * @param  m0 比較対象行列
 * @param  rot_scale_range ３Ｘ３成分の誤差範囲
 * @param  trans_range 座標成分の誤差範囲
 * @return 各成分の差が誤差範囲内の場合は真、他は偽
 */
bool IsSimilar( const self_type& m0, const f32 rot_scale_range, const f32 trans_range ) const {
	return GetRow( 0 ).IsSimilar( m0.GetRow( 0 ), rot_scale_range ) &&
		GetRow( 1 ).IsSimilar( m0.GetRow( 1 ), rot_scale_range ) &&
		GetRow( 2 ).IsSimilar( m0.GetRow( 2 ), rot_scale_range ) &&
		GetRow( 3 ).IsSimilar( m0.GetRow( 3 ), trans_range );
}

#if GFL_USE_SOA

/**
 *   代入
 * @param  m 入力行列
 * @return 無し
 */
inline self_type& operator=(const MatrixSoaNative34& m) {
	FromSOA( m );
	return *this;
}

/**
 *   代入
 * @param  m 入力行列
 * @return 無し
 */
inline self_type& operator=(const MatrixSoaNative44& m) {
	FromSOA( m );
	return *this;
}
#endif

/**
 * ｘｙｚ軸方向にスケールをかける
 * @param scl　スケール値
 */
void Mul( const f32 scl ) {
	Scale(scl);
}

// -----------------------------------------------------------------------------
// Scale
// -----------------------------------------------------------------------------

/**
 *   Ｘにスケールをかける
 * @param  x Ｘ軸にかけるスケール
 * @return 無し
 */

inline void ScaleX( const f32 x ) {
	GetRef( 0, 0 ) *= x;
	GetRef( 0, 1 ) *= x;
	GetRef( 0, 2 ) *= x;
}

/**
 * 行列のXスケールを取得
 * @return スケール値
 */
inline f32 GetScaleX( void ) const {
	return GetRow( 0 ).Magnitude( );
}

/**
 *   Ｙにスケールをかける
 * @param  y Ｙ軸にかけるスケール
 * @return 無し
 */

inline void ScaleY( const f32 y ) {
	GetRef( 1, 0 ) *= y;
	GetRef( 1, 1 ) *= y;
	GetRef( 1, 2 ) *= y;
}

/**
 * 行列のYスケールを取得
 * @return スケール値
 */
inline f32 GetScaleY( void ) const {
	return GetRow( 1 ).Magnitude( );
}

/**
 *   Ｚにスケールをかける
 * @param  z Ｚ軸にかけるスケール
 * @return 無し
 */

inline void ScaleZ( const f32 z ) {
	GetRef( 2, 0 ) *= z;
	GetRef( 2, 1 ) *= z;
	GetRef( 2, 2 ) *= z;
}

/**
 * 行列のZスケールを取得
 * @return スケール値
 */
inline f32 GetScaleZ( void ) const {
	return GetRow( 2 ).Magnitude( );
}

/**
 * 正規化
 */
inline void Normalize( VectorNative* scl = 0 );

/**
 *   Ｘにスケールを逆からかける
 * @param  x Ｘ軸にかけるスケール
 * @return 無し
 */

inline void ScaleX_Reverse( const f32 x ) {
	GetRef( 0, 0 ) *= x;
	GetRef( 1, 0 ) *= x;
	GetRef( 2, 0 ) *= x;
}

/**
 *   Ｙにスケールを逆からかける
 * @param  y Ｙ軸にかけるスケール
 * @return 無し
 */

inline void ScaleY_Reverse( const f32 y ) {
	GetRef( 0, 1 ) *= y;
	GetRef( 1, 1 ) *= y;
	GetRef( 2, 1 ) *= y;
}

/**
 *   Ｚにスケールを逆からかける
 * @param  z Ｚ軸にかけるスケール
 * @return 無し
 */

inline void ScaleZ_Reverse( const f32 z ) {
	GetRef( 0, 2 ) *= z;
	GetRef( 1, 2 ) *= z;
	GetRef( 2, 2 ) *= z;
}

/**
 * ベクトルが行列の左方向９０度以内か
 * @param vx　ベクトルｘ成分
 * @param vz　ベクトルｚ成分
 * @return 真　左舷９０度以内　偽　それ以外
 */
inline bool IsVectorLeftDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsLeftDegree90( x, z );
}

/**
 * ベクトルが行列の右方向９０度以内か
 * @param vx　ベクトルｘ成分
 * @param vz　ベクトルｚ成分
 * @return 真　右舷９０度以内　偽　それ以外
 */
inline bool IsVectorRightDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsRightDegree90( x, z );
}

#endif

/* 
 * File:   gfl_MatrixSoaCommon.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 14:20
 */

//#ifndef GFL_MATRIX_SOA_BASE_H // このファイルは34,44共用なので２回インクルードしたい
//#define	GFL_MATRIX_SOA_BASE_H

#include "gfl_MatrixCommon.h"

/**
 *   指定された列を取得
 * @param  i 添え字
 * @return 指定された列
 */
inline Vector4& GetRow( const s32 i ) {
	return reinterpret_cast<Vector4&> (parent_type::GetRow( i ));
}

/**
 *   指定された列を取得
 * @param  i 添え字
 * @return 指定された列
 */
inline const Vector4& GetRow( const s32 i ) const {
	return reinterpret_cast<const Vector4&> (parent_type::GetRow( i ));
}

/**
 * 指定された行を取得
 * @param i　添字
 * @return 行を設定したVector3の実体
 */
inline Vector3 GetColumn( const s32 i ) const {
	return Vector3( parent_type::GetColumn( i ) );
}

/**
 *   Ｘ座標成分を取得
 * @param  無し
 * @return Ｘ座標
 */
inline f32 GetTransX( void ) const {
	return Get( TX, 3 );
}

/**
 *   Ｙ座標成分を取得
 * @param  無し
 * @return Ｙ座標
 */
inline f32 GetTransY( void ) const {
	return Get( TY, 3 );
}

/**
 *   Ｚ座標成分を取得
 * @param  無し
 * @return Ｚ座標
 */
inline f32 GetTransZ( void ) const {
	return Get( TZ, 3 );
}

/**
 *   Ｘ座標成分参照を取得
 * @param  無し
 * @return Ｘ座標
 */
inline f32& GetTransRefX( void ) {
	return GetRef( TX, 3 );
}

/**
 *   Ｙ座標成分参照を取得
 * @param  無し
 * @return Ｙ座標
 */
inline f32& GetTransRefY( void ) {
	return GetRef( TY, 3 );
}

/**
 *   Ｚ座標成分参照を取得
 * @param  無し
 * @return Ｚ座標
 */
inline f32& GetTransRefZ( void ) {
	return GetRef( TZ, 3 );
}

/**
 *   座標成分を取得
 * @param  無し
 * @return 座標
 */
inline Vector3 GetTrans( void ) const {
	return GetColumn( 3 );
}

/**
 *   要素を設定
 * @param  mat 入力３ｘ３行列
 * @return 無し
 */
void Set( const MatrixSoaNative33& mat ) {
	Set33( mat );
}

/**
 * 要素を設定
 * @param m0 入力３x４行列
 */
void Set( const MatrixSoaNative34& m0 ) {
	SetRow( 0, m0.GetRow( 0 ) );
	SetRow( 1, m0.GetRow( 1 ) );
	SetRow( 2, m0.GetRow( 2 ) );
}

/**
 * 列を設定
 * @param i　添字
 * @param m0　入力ベクトル構造体４要素
 */
inline void SetRow( const s32 i, const VectorStruct4& m0 ) {
	GFL_MTX_VECTOR[i] = m0;
}

/**
 * 列を設定
 * @param i　添字
 * @param m0　入力ベクトル構造体３要素
 */
inline void SetRow( const s32 i, const VectorStruct3& m0 ) {
	GFL_MTX_VECTOR[i].GFL_VECTOR_X = m0.GFL_VECTOR_X;
	GFL_MTX_VECTOR[i].GFL_VECTOR_Y = m0.GFL_VECTOR_Y;
	GFL_MTX_VECTOR[i].GFL_VECTOR_Z = m0.GFL_VECTOR_Z;
}

/**
 * 列を設定
 * @param i　添字
 * @param m0　入力ベクトル構造体３要素
 * @param w 入力　座標成分
 */
inline void SetRow( const s32 i, const VectorStruct3& m0, const f32 w ) {
	GFL_MTX_VECTOR[i].GFL_VECTOR_X = m0.GFL_VECTOR_X;
	GFL_MTX_VECTOR[i].GFL_VECTOR_Y = m0.GFL_VECTOR_Y;
	GFL_MTX_VECTOR[i].GFL_VECTOR_Z = m0.GFL_VECTOR_Z;
	GFL_MTX_VECTOR[i].GFL_VECTOR_W = w;
}

/**
 * 行を設定
 * @param i　添字
 * @param m0　入力ベクトル構造体３要素
 */
inline void SetColumn( const s32 i, const VectorStruct3& m0 ) {
	GFL_MTX_ARRAY2D[0][i] = m0.GFL_VECTOR_X;
	GFL_MTX_ARRAY2D[1][i] = m0.GFL_VECTOR_Y;
	GFL_MTX_ARRAY2D[2][i] = m0.GFL_VECTOR_Z;
}

/**
 * ｘ座標を設定
 * @param x　ｘ座標
 */
inline void SetTransX( const f32 x ) {
	GFL_MTX_ARRAY2D[TX][3] = x;
}

/**
 * ｙ座標を設定
 * @param y　ｙ座標
 */
inline void SetTransY( const f32 y ) {
	GFL_MTX_ARRAY2D[TY][3] = y;
}

/**
 * ｚ座標を設定
 * @param z　ｚ座標
 */
inline void SetTransZ( const f32 z ) {
	GFL_MTX_ARRAY2D[TZ][3] = z;
}

/**
 * 座標を設定
 * @param v　ベクトル構造体３要素
 */
inline void SetTrans( const VectorNative3& v ) {
	SetColumn( 3, v );
}

/**
 * 座標を設定
 * @param v　ベクトル構造体４要素
 */
inline void SetTrans( const VectorNative4& v ) {
	SetColumn( 3, v );
}

/**
 * ｘｙｚ軸方向にスケールをかける
 * @param scl　スケール値
 */
void Mul( const f32 scl ) {
	*this *= scl;
}

/**
 * スケール値を逆から掛ける
 * @param x　ｘスケール値
 */
inline void ScaleX_Reverse( const f32 x ) {
	GetRef( 0, 0 ) *= x;
	GetRef( 0, 1 ) *= x;
	GetRef( 0, 2 ) *= x;
}

/**
 * スケール値を逆から掛ける
 * @param y　ｙスケール値
 */
inline void ScaleY_Reverse( const f32 y ) {
	GetRef( 1, 0 ) *= y;
	GetRef( 1, 1 ) *= y;
	GetRef( 1, 2 ) *= y;
}

/**
 * スケール値を逆から掛ける
 * @param z　ｚスケール値
 */
inline void ScaleZ_Reverse( const f32 z ) {
	GetRef( 2, 0 ) *= z;
	GetRef( 2, 1 ) *= z;
	GetRef( 2, 2 ) *= z;
}

/**
 * ｘ方向ベクトルを角度成分に掛けたものを計算
 * @param v0　出力ベクトルクラス
 * @param val　ｘ値
 */
inline void ApplyX( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 0 ) * val, Get( 1, 0 ) * val, Get( 2, 0 ) * val );
}

/**
 * ｘ方向ベクトルを角度成分に長さ１を掛けたものを計算
 * @param v0　出力ベクトルクラス
 */
inline void ApplyX1( VectorNative* v0 ) const {
	v0->Set( Get( 0, 0 ), Get( 1, 0 ), Get( 2, 0 ) );
}

/**
 * ｙ方向ベクトルを角度成分に掛けたものを計算
 * @param v0　出力ベクトルクラス
 * @param val　ｙ値
 */
inline void ApplyY( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 1 ) * val, Get( 1, 1 ) * val, Get( 2, 1 ) * val );
}

/**
 * ｚ方向ベクトルを角度成分に長さ１を掛けたものを計算
 * @param v0　出力ベクトルクラス
 */
inline void ApplyY1( VectorNative* v0 ) const {
	v0->Set( Get( 0, 1 ), Get( 1, 1 ), Get( 2, 1 ) );
}

/**
 * ｚ方向ベクトルを角度成分に掛けたものを計算
 * @param v0　出力ベクトルクラス
 * @param val　ｚ値
 */
inline void ApplyZ( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 2 ) * val, Get( 1, 2 ) * val, Get( 2, 2 ) * val );
}

/**
 * ｚ方向ベクトルを角度成分に長さ１を掛けたものを計算
 * @param v0　出力ベクトルクラス
 */
inline void ApplyZ1( VectorNative* v0 ) const {
	v0->Set( Get( 0, 2 ), Get( 1, 2 ), Get( 2, 2 ) );
}

/**
 * 座標移動
 * @param val　ｘ成分
 */
inline void TransX( const f32 val ) {
	GetTransRefX( ) += Get( 0, 0 ) * val;
	GetTransRefY( ) += Get( 1, 0 ) * val;
	GetTransRefZ( ) += Get( 2, 0 ) * val;
}

/**
 * 座標移動
 * @param val　ｙ成分
 */
inline void TransY( const f32 val ) {
	GetTransRefX( ) += Get( 0, 1 ) * val;
	GetTransRefY( ) += Get( 1, 1 ) * val;
	GetTransRefZ( ) += Get( 2, 1 ) * val;
}

/**
 * 座標移動
 * @param val　ｚ成分
 */
inline void TransZ( const f32 val ) {
	GetTransRefX( ) += Get( 0, 2 ) * val;
	GetTransRefY( ) += Get( 1, 2 ) * val;
	GetTransRefZ( ) += Get( 2, 2 ) * val;
}

/**
 * 座標移動
 * @param v0　出力ベクトルクラスポインタ
 * @param val　ｘ成分
 * @remark 行列に計算結果は反映されない
 */
inline void TransX( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 0 ) * val + GetTransX( ),
			 Get( 1, 0 ) * val + GetTransY( ),
			 Get( 2, 0 ) * val + GetTransZ( ) );
}

/**
 * 座標移動
 * @param v0　出力ベクトルクラスポインタ
 * @param val　ｙ成分
 * @remark 行列に計算結果は反映されない
 */
inline void TransY( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 1 ) * val + GetTransX( ),
			 Get( 1, 1 ) * val + GetTransY( ),
			 Get( 2, 1 ) * val + GetTransZ( ) );
}

/**
 * 座標移動
 * @param v0　出力ベクトルクラスポインタ
 * @param val　ｚ成分
 * @remark 行列に計算結果は反映されない
 */
inline void TransZ( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 2 ) * val + GetTransX( ),
			 Get( 1, 2 ) * val + GetTransY( ),
			 Get( 2, 2 ) * val + GetTransZ( ) );
}

/**
 * グローバル座標をローカル座標に変換したｘ座標を取得
 * @param glo　グローバル座標　入力ベクトル
 * @return ローカルｘ座標
 */
inline f32 GlobalToLocalX( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 0, 0 ) * pos.GetX( ) + Get( 1, 0 ) * pos.GetY( ) + Get( 2, 0 ) * pos.GetZ( );
}

/**
 * グローバル座標をローカル座標に変換したｙ座標を取得
 * @param glo　グローバル座標　入力ベクトル
 * @return ローカルｙ座標
 */
inline f32 GlobalToLocalY( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 0, 1 ) * pos.GetX( ) + Get( 1, 1 ) * pos.GetY( ) + Get( 2, 1 ) * pos.GetZ( );
}

/**
 * グローバル座標をローカル座標に変換したｚ座標を取得
 * @param glo　グローバル座標　入力ベクトル
 * @return ローカルｚ座標
 */
inline f32 GlobalToLocalZ( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 0, 2 ) * pos.GetX( ) + Get( 1, 2 ) * pos.GetY( ) + Get( 2, 2 ) * pos.GetZ( );
}

/**
 * ベクトルと行列の向きの前後方向が同じか
 * @param x　ベクトルｘ成分
 * @param z　ベクトルｚ成分
 * @return 真　同じ方向　偽　反対方向
 */
inline bool IsVectorSameDirection( const f32 x, const f32 z ) const {
	return ( 0.0f <= Get( 0, 2 ) * x + Get( 2, 2 ) * z);
}

/**
 * ベクトルと行列の向きの左右方向が同じか
 * @param x　ベクトルｘ成分
 * @param z　ベクトルｚ成分
 * @return 真　同じ方向　偽　反対方向
 */
inline bool IsVectorSameSide( const f32 x, const f32 z ) const {
	return ( 0.0f <= Get( 0, 0 ) * x + Get( 2, 0 ) * z);
}

/**
 * ベクトルが行列と同じ方向９０度以内か
 * @param vx　ベクトルｘ成分
 * @param vz　ベクトルｚ成分
 * @return 真　同じ方向９０度以内　偽　それ以外
 */
inline bool IsVectorSameDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
	const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsFrontDegree90( x, z );
}

/**
 * ベクトルが行列と反対方向の９０度以内か
 * @param vx　ベクトルｘ成分
 * @param vz　ベクトルｚ成分
 * @return 真　反対方向９０度以内　偽　それ以外
 */
inline bool IsVectorOppositeDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
	const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsBackDegree90( x, z );
}

/**
 * ベクトルが行列の左方向９０度以内か
 * @param vx　ベクトルｘ成分
 * @param vz　ベクトルｚ成分
 * @return 真　左舷９０度以内　偽　それ以外
 */
inline bool IsVectorLeftDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
	const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsLeftDegree90( x, z );
}

/**
 * ベクトルが行列の右方向９０度以内か
 * @param vx　ベクトルｘ成分
 * @param vz　ベクトルｚ成分
 * @return 真　右舷９０度以内　偽　それ以外
 */
inline bool IsVectorRightDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
	const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsRightDegree90( x, z );
}

/**
 * 行列を代入
 * @param m　入力行列
 * @return 参照
 */
inline self_type& operator=(const MatrixSoaNative34& m) {
	Set( m );
	return *this;
}

/**
 * 行列を代入
 * @param m　入力行列
 * @return 参照
 */
inline self_type& operator=(const MatrixSoaNative44& m) {
	Set( m );
	return *this;
}

#if GFL_USE_AOS

/**
 *   代入
 * @param  m 入力行列
 * @return 無し
 */
inline self_type& operator=(const MatrixAos43& m) {
	FromAOS( m );
	return *this;
}

/**
 *   代入
 * @param  m 入力行列
 * @return 無し
 */
inline self_type& operator=(const MatrixAos44& m) {
	FromAOS( m );
	return *this;
}
#endif

using parent_type::operator+=;

/**
 * 単位行列を設定
 */
void SetUnit( void ) {
	parent_type::SetupIdentity( );
}

/**
 * ｘ軸方向にスケールをかける
 * @param scl　スケール値
 */
void ScaleX( const f32 scl ) {
	GFL_MTX_00 *= scl;
	GFL_MTX_10 *= scl;
	GFL_MTX_20 *= scl;
}

/**
 * ｙ軸方向にスケールをかける
 * @param scl　スケール値
 */
void ScaleY( const f32 scl ) {
	GFL_MTX_01 *= scl;
	GFL_MTX_11 *= scl;
	GFL_MTX_21 *= scl;
}

/**
 * ｚ軸方向にスケールをかける
 * @param scl　スケール値
 */
void ScaleZ( const f32 scl ) {
	GFL_MTX_02 *= scl;
	GFL_MTX_12 *= scl;
	GFL_MTX_22 *= scl;
}

/**
 * 行列からスケール値を取得
 * @return ｘスケール値
 */
inline f32 GetScaleX( void ) const {
	return GetColumn( 0 ).Magnitude( );
}

/**
 * 行列からスケール値を取得
 * @return ｙスケール値
 */
inline f32 GetScaleY( void ) const {
	return GetColumn( 1 ).Magnitude( );
}

/**
 * 行列からスケール値を取得
 * @return ｚスケール値
 */
inline f32 GetScaleZ( void ) const {
	return GetColumn( 2 ).Magnitude( );
}

/**
 * 行列からスケール値を取得
 * @param スケールベクトルポインタ
 */
inline void GetScale( VectorNative* v ) {
	v->Set( GetScaleX( ), GetScaleY( ), GetScaleZ( ) );
}

/**
 * 行列からスケール値を取得
 * @param スケールベクトルポインタ
 */
inline Vector GetScale( void ) {
	Vector v;
	v.Set( GetScaleX( ), GetScaleY( ), GetScaleZ( ) );
	return v;
}

/**
 * 角度成分３ｘ３に単位行列を設定
 */
inline void SetUnit33( void ) {
	SetRow( 0, Vector3::GetX1( ) );
	SetRow( 1, Vector3::GetY1( ) );
	SetRow( 2, Vector3::GetZ1( ) );
}

//#endif

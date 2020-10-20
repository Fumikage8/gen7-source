/*
 * File:   gfl_MatrixSOA34.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 14:20
 */

#ifndef GFL_MATRIX34SOA_H
#define	GFL_MATRIX34SOA_H

#pragma once

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================
class MatrixSoa34;

// =============================================================================
// このファイルには含まれていないが宣言が必要なクラス群
// =============================================================================
class Quaternion;
class MatrixAos43;
class MatrixAos44;
class MatrixSoa44;

/**
 *  ４つの要素を持つベクトルクラス SOA
 */

class MatrixSoa34 : public MatrixSoaNative34 {
public:
	typedef MatrixSoaNative34 parent_type; /**< 親の型です */
	typedef MatrixSoa34 self_type; /**< 自分の型です */
#if 0 // この中を変更する場合は１にしてテストすべし
#include "gfl_MatrixSoaCommon.h"
#else
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
//	self_type GetNormalizedMatrix( VectorNative* scl = 0 ) { // これだとインテリセンスが…
	MatrixSoa34 GetNormalizedMatrix( VectorNative* scl = 0 ) {
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
#endif
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	/**
	 *   デフォルトコンスタラクター
	 * @param  無し
	 * @return 無し
	 * @remark 何もしない
	 */

	MatrixSoa34( void ) {
	}

	/**
	 *   MatrixSoa34 を設定するコンスタラクター
	 * @param  m0 MatrixSoa34クラス
	 * @return 無し
	 */

	MatrixSoa34( const MatrixSoaNative33& m0 ) {
		Set33( m0 );
		SetTransUnit( );
	}

	/**
	 *   MatrixSoa34 を設定するコンスタラクター
	 * @param  m0 MatrixSoa34クラス
	 * @return 無し
	 */

	MatrixSoa34( const MatrixSoaNative34& m0 ) {
		Set( m0 );
	}

	/**
	 *   MatrixSoa34 を設定するコンスタラクター
	 * @param  m0 MatrixSoa34クラス
	 * @return 無し
	 */

	MatrixSoa34( const MatrixSoaNative44& m0 ) {
		Set( m0 );
	}

	// -------------------------------------------------------------------------
	// Destruct !! not virtual for alignment
	// -------------------------------------------------------------------------

	~MatrixSoa34( ) {
	}

	/**
	 *   Matrix44Soa を設定
	 * @param  m 設定したい Matrix44Soa の参照
	 * @return 無し
	 */
	void Set( const MatrixSoaNative44& mat );

	/**
	 *   BaseMatrix34 を設定
	 * @param  m0  要素 @param  m1  要素 @param  m2  要素 @param  m3  要素 @param  m4 要素
	 * @param  m5  要素 @param  m6  要素 @param  m7  要素 @param  m8  要素 @param  m9 要素
	 * @param  m10 要素 @param  m11
	 * @return 無し
	 */
	void Set( f32 m0, f32 m1, f32 m2, f32 m3,
			 f32 m4, f32 m5, f32 m6, f32 m7,
			 f32 m8, f32 m9, f32 m10, f32 m11 );

	/**
	 *   要素を設定
	 * @param  i 要素
	 * @param  m0 値
	 * @return 無し
	 */
	inline void SetColumn( const s32 i, const VectorStruct4& m0 ) {
		GFL_MTX_ARRAY2D[0][i] = m0.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][i] = m0.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][i] = m0.GFL_VECTOR_Z;
	}

	/**
	 *   座標成分を設定
	 * @param  x Ｘ座標
	 * @param  y Ｙ座標
	 * @param  z Ｚ座標
	 * @return 無し
	 */

	inline void SetTrans( const f32 x, const f32 y, const f32 z ) {
		SetTransX( x );
		SetTransY( y );
		SetTransZ( z );
	}

	/**
	 * 座標成分をクリア
	 */
	inline void SetTransUnit( void ) {
		SetColumn( 3, Vector3::GetZero( ) );
	}

	/**
	 * クォータニオンを掛ける
	 * @param q0　クォータニオン
	 * @return なし
	 */
	void Mul( const QuaternionNative& q0 );

	// -------------------------------------------------------------------------
	// Inverse
	// -------------------------------------------------------------------------
	/**
	 *   逆行列
	 * @param  m0 入力行列
	 * @return 無し
	 * @remark 重い
	 */
	void Inverse( const MatrixSoa34& m0 );

	/**
	 *   乗算
	 * @param  m0 入力行列
	 * @param  m1 入力行列
	 * @return 無し
	 */
	void Mul( const MatrixSoaNative34& m0, const MatrixSoaNative34& m1 );

	/**
	 *   転置行列を設定
	 * @param  m0 入力行列の参照
	 * @return 無し
	 */
	void Transpose( const MatrixSoaNative34& m0 );

	// -------------------------------------------------------------------------
	// Scale
	// -------------------------------------------------------------------------
	void SetScale( const f32 x, const f32 y, const f32 z );
	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------
	/**
	 *   ３Ｘ３成分の乗算
	 * @param  m0 入力行列
	 * @param  m1 入力行列
	 * @return 無し
	 */
	void Mul33( const self_type& m0, const self_type& m1 );

	/**
	 *   決定因子
	 * @param  無し
	 * @return 決定因子
	 */
	f32 Determinant( void ) const;

	inline void operator+=(const VectorStruct4& v) {
		GetTransRefX( ) += v.GFL_VECTOR_X;
		GetTransRefY( ) += v.GFL_VECTOR_Y;
		GetTransRefZ( ) += v.GFL_VECTOR_Z;
	}
	bool operator==(const self_type& m0);
	bool operator!=(const self_type& m0);
	bool IsSimilar( const self_type& m0, const f32 rot_scale_range = Math::EPSILON, const f32 trans_range = Math::EPSILON ) const;

#if GFL_USE_AOS
	void FromAOS( const gfl::core::MatrixAos43& src );
	void FromAOS( const gfl::core::MatrixAos44& src );
	void ToAOS( gfl::core::MatrixAos43* mat ) const;
	void ToAOS( gfl::core::MatrixAos44* mat ) const;
	MatrixAos44 ToAOS( void ) const;
#endif
#ifdef GFL_PLATFORM_3DS
	void ToSOA( gfl::math::MTX34* tag ) const;
	void FromSOA( const gfl::math::MTX34& src );
#endif
	// =========================================================================
	// ここから下は MatrixSoa44と共通
	// =========================================================================

	// -------------------------------------------------------------------------
	// Determinant
	// -------------------------------------------------------------------------
	/**
	 *   角度スケール成分とベクトルの積算
	 * @param  v0 出力ベクトル
	 * @param  v1 入力ベクトル
	 * @return 無し
	 */

	void Apply( VectorNative* v0, const VectorNative& v1 ) const;

	/**
	 *   角度スケール成分とベクトルの積算
	 * @param  v0 出力ベクトル
	 * @param  v1 入力ベクトル
	 * @return 無し
	 */

	void ApplyTranspose( VectorNative* v0, const VectorNative& v1 ) const;

	// -------------------------------------------------------------------------
	// Rotate X
	// -------------------------------------------------------------------------
	/**
	 *   行列のＸ軸で回転させた角度成分を設定
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 * @remark 座標成分には何もしない
	 */
	void SetRotateX( const f32 s, const f32 c );

	/**
	 *   行列 m0 にＸ軸で回転させた角度成分をかける
	 * @param  m0 入力行列
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateX( const self_type& m0, const f32 s, const f32 c );

	/**
	 *   自分にＸ軸で回転させた角度成分をかける
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateX( const f32 s, const f32 c );

	/**
	 *   自分にＸ軸で 1/2PI 回転させた角度成分をかける
	 * @return 無し
	 * @remark SinCos と積を省略しているので高速
	 */
	void RotateX_HalfPi( void );

	/**
	 *   自分にＸ軸で -1/2PI 回転させた角度成分をかける
	 * @return 無し
	 */
	void RotateX_MinusHalfPi( void );

	/**
	 *   自分にＸ軸で PI 回転させた角度成分をかける
	 * @return 無し
	 */
	void RotateX_OnePi( void );

	/**
	 *   自分にＸ軸で回転させた角度成分を逆向きにかける
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateX_Reverse( const f32 s, const f32 c );

	// -------------------------------------------------------------------------
	// Rotate Y
	// -------------------------------------------------------------------------
	/**
	 *   行列のＹ軸で回転させた角度成分を設定
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 * @remark 座標成分には何もしない
	 */
	void SetRotateY( const f32 s, const f32 c );

	/**
	 *   行列 m0 にＹ軸で回転させた角度成分をかける
	 * @param  m0 入力行列
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateY( const self_type& m0, const f32 s, const f32 c );

	/**
	 *   自分にＹ軸で回転させた角度成分をかける
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateY( const f32 s, const f32 c );

	/**
	 *   自分にｙ軸で 1/2PI 回転させた角度成分をかける
	 * @return 無し
	 * @remark SinCos と積を省略しているので高速
	 */
	void RotateY_HalfPi( void );

	/**
	 *   自分にｙ軸で -1/2PI 回転させた角度成分をかける
	 * @return 無し
	 */
	void RotateY_MinusHalfPi( void );

	/**
	 *   自分にｙ軸で PI 回転させた角度成分をかける
	 * @return 無し
	 */
	void RotateY_OnePi( void );

	/**
	 *   自分にＹ軸で回転させた角度成分を逆向きにかける
	 * @param  m0 入力行列.
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateY_Reverse( const self_type& m0, const f32 s, const f32 c );

	/**
	 *   自分にＹ軸で回転させた角度成分を逆向きにかける
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateY_Reverse( const f32 s, const f32 c );

	// -------------------------------------------------------------------------
	// Rotate Z
	// -------------------------------------------------------------------------
	/**
	 *   行列のＺ軸で回転させた角度成分を設定
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 * @remark 座標成分には何もしない
	 */
	void SetRotateZ( const f32 s, const f32 c );

	/**
	 *   行列 m0 にＺ軸で回転させた角度成分をかける
	 * @param  m0 入力行列
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateZ( const self_type& m0, const f32 s, const f32 c );

	/**
	 *   自分にＺ軸で回転させた角度成分をかける
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateZ( const f32 s, const f32 c );

	/**
	 *   自分にｚ軸で 1/2PI 回転させた角度成分をかける
	 * @return 無し
	 * @remark SinCos と積を省略しているので高速
	 */
	void RotateZ_HalfPi( void );

	/**
	 *   自分にｚ軸で -1/2PI 回転させた角度成分をかける
	 * @return 無し
	 */
	void RotateZ_MinusHalfPi( void );

	/**
	 *   自分にｚ軸で PI 回転させた角度成分をかける
	 * @return 無し
	 */
	void RotateZ_OnePi( void );

	/**
	 *   自分にＺ軸で回転させた角度成分を逆向きにかける
	 * @param  s サイン値
	 * @param  c コサイン値
	 * @return 無し
	 */
	void RotateZ_Reverse( const f32 s, const f32 c );

	// -------------------------------------------------------------------------
	// Determinant
	// -------------------------------------------------------------------------

	/**
	 *   ３Ｘ３の決定因子
	 * @return 決定因子
	 */
	f32 Determinant33( void ) const;
	/**
	 *   ３Ｘ３の逆行列
	 * @param  m0 入力行列
	 * @return 無し
	 * @remark 座標成分には（０，０，０，１）が設定される。\n
	 * スケールがかかってない場合は転置行列の使用をお勧めします。
	 */
	void Inverse33( const self_type& m0 );
	/**
	 *   Y座標以下にならないようなベクトルを求める
	 * @param  res 計算結果ベクトル
	 * @param  y0 最低Y座標
	 * @param  r 骨の長さ
	 * @return なし
	 */
	void GetPositionByLowestY( VectorNative* res, const f32 y0, const f32 r ) const;
	/**
	 *   クォータニオンを取得
	 * @param  q 出力クォータニオン
	 * @param scl スケール値
	 * @return 無し
	 */
	void MatrixToQuaternion( QuaternionNative* q ) const;
	/**
	 *   クォータニオンを取得
	 * @param  q 出力クォータニオン
	 * @param scl スケール値
	 * @return 無し
	 */
	void MatrixToQuaternion( QuaternionNative* q, VectorNative* scl ) const;
	/**
	 *   クォータニオンを取得
	 * @param  無し
	 * @return q 出力クォータニオン
	 */
	Quaternion MatrixToQuaternion( void ) const;
	/**
	 *   クォータニオンを取得
	 * @param  無し
	 * @return q 出力クォータニオン
	 */
	Quaternion MatrixToQuaternion( VectorNative* scl ) const;
	/**
	 *   クォータニオンから行列を設定
	 * @param  q 入力クォータニオン
	 * @return 無し
	 */
	void QuaternionToMatrix( const QuaternionNative& q );
	/**
	 *   角度を行列から取得
	 * @param  x Ｘ軸角度
	 * @param  y Ｙ軸角度
	 * @param  z Ｚ軸角度
	 * @return 無し
	 * @remark ＺＹＸの順で回した場合の角度
	 */
	void GetRadian( f32* x, f32* y, f32* z ) const;
	/**
	 *   角度を行列から取得
	 * @param  x Ｘ軸角度
	 * @param  y Ｙ軸角度
	 * @param  z Ｚ軸角度
	 * @return 無し
	 * @remark ＹＸＺの順で回した場合の角度
	 */
	void GetRadianYXZ( f32* x, f32* y, f32* z ) const;
	/**
	 * グローバル座標をローカル座標へ変換する
	 * @param loc　出力　ローカル座標
	 * @param pos　入力　グローバル座標
	 */
	void GlobalToLocal33( VectorNative* loc, const VectorNative& pos ) const;
	/**
	 * グローバル座標をローカル座標へ変換する
	 * @param loc　出力　ローカル座標
	 * @param pos　入力　グローバル座標
	 */
	void GlobalToLocal33Scaled( VectorNative* loc, const VectorNative& pos ) const;
	/**
	 *   ターゲットポジションに骨を向けるサインコサインを求める
	 * @param  tag 目標座標
	 * @param  zang_sin Ｚ回転サイン
	 * @param  zang_cos Ｚ回転コサイン
	 * @param  yang_sin Ｙ回転サイン
	 * @param  yang_cos Ｙ回転コサイン
	 * @return なし
	 */
	bool AimZY( const VectorNative& tag, f32* zang_sin, f32* zang_cos, f32* yang_sin
			 , f32* yang_cos, const bool reverse = false, const bool scaled = false ) const;
	/**
	 *   ターゲットポジションに骨を向けるサインコサインを求める
	 * @param  tag 目標座標
	 * @param  yang_sin Ｙ回転サイン
	 * @param  yang_cos Ｙ回転コサイン
	 * @param  zang_sin Ｚ回転サイン
	 * @param  zang_cos Ｚ回転コサイン
	 * @param  reverse 目標座標の符号を反転させるか
	 * @return なし
	 * @remark ＹＺの順番に軸を回す
	 */
	bool AimYZ( const VectorNative& tag, f32* yang_sin, f32* yang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const;
	/**
	 * Z軸に伸びている骨とYXの順番で回転させるサインコサインを求める
	 * @param tag　目標座標
	 * @param yang_sin　Y回転サイン
	 * @param yang_cos　Y回転コサイン
	 * @param xang_sin　X回転サイン
	 * @param xang_cos　X回転コサイン
	 * @param reverse 目標座標の符号を反転させるか
	 */
	bool AimYX( const VectorNative& tag, f32* yang_sin, f32* yang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const;
	/**
	 * Y軸に伸びている骨とXZの順番で回転させるサインコサインを求める
	 * @param tag　目標座標
	 * @param xang_sin　X回転サイン
	 * @param xang_cos　X回転コサイン
	 * @param zang_sin　Z回転サイン
	 * @param zang_cos　Z回転コサイン
	 * @param reverse 目標座標の符号を反転させるか
	 */
	bool AimXZ( const VectorNative& tag, f32* xang_sin, f32* xang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const;
	/**
	 * Z軸に伸びている骨とXYの順番で回転させるサインコサインを求める
	 * @param tag　目標座標
	 * @param xang_sin　X回転サイン
	 * @param xang_cos　X回転コサイン
	 * @param zang_sin　Z回転サイン
	 * @param zang_cos　Z回転コサイン
	 * @param reverse 目標座標の符号を反転させるか
	 */
	bool AimXY( const VectorNative& tag, f32* xang_sin, f32* xang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const;
	/**
	 *   ターゲットポジションに骨を向けてＺＹ回転させる
	 * @param  tag 目標座標
	 * @return 真　成功　偽　失敗
	 */
	bool AimZY( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );
	/**
	 *   ターゲットポジションに骨を向けてYZ回転させる
	 * @param  tag 目標座標
	 * @param  reverse 目標座標の符号を反転させるか
	 * @param  rate 向ける割合　０　向けない　１　完全に向ける
	 * @return 真　成功　偽　失敗
	 */
	bool AimYZ( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );
	/**
	 *   ターゲットポジションに骨を向けてYX回転させる
	 * @param  tag 目標座標
	 * @param  reverse 目標座標の符号を反転させるか
	 * @param  rate 向ける割合　０　向けない　１　完全に向ける
	 * @return 真　成功　偽　失敗
	 */
	bool AimYX( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );
	/**
	 *   ターゲットポジションに骨を向けてXZ回転させる
	 * @param  tag 目標座標
	 * @param  reverse 目標座標の符号を反転させるか
	 * @param  rate 向ける割合　０　向けない　１　完全に向ける
	 * @return 真　成功　偽　失敗
	 */
	bool AimXZ( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );
	/**
	 *   ターゲットポジションに骨を向けてXY回転させる
	 * @param  tag 目標座標
	 * @param  reverse 目標座標の符号を反転させるか
	 * @param  rate 向ける割合　０　向けない　１　完全に向ける
	 * @return 真　成功　偽　失敗
	 */
	bool AimXY( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );
	void AimX( f32* xsin, f32* xcos );
	void AimX( void );
	void AimY( const gfl::core::Vector& tag );
	/**
	 *   正規化
	 * @param  scl スケール値ベクトル
	 * @return なし
	 */
	void Normalize( VectorNative* scl = 0 );

	/**
	 *   補完
	 * @param  m0 補完したいクラス
	 * @param  m1 補完したいクラス
	 * @param  rate 内分値
	 * @return 無し
	 * @remark this = VectorNative*rate + VectorNative*(1.0f-rate)
	 */
	self_type& Slerp( const self_type& m0, const self_type& m1, const f32 rate );
	/**
	 *   コンソールに各成分を出力
	 * @param  無し
	 * @return 無し
	 */
	void PrintConsole( void ) const;

	// =========================================================================
	// using
	// =========================================================================
	using MatrixSoaNative34::SetColumn;
	using MatrixSoaNative34::Transpose;
};

#endif	/* GFL_MATRIX34SOA_H */

/* 
 * File:   gfl_Matrix44AOS.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 14:20
 */

#ifndef GFL_MATRIX44AOS_H
#define	GFL_MATRIX44AOS_H

#pragma once

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================
class MatrixAos44;

// =============================================================================
// このファイルには含まれていないが宣言が必要なクラス群
// =============================================================================
class Quaternion;
class MatrixAos43;

/**
 *  ４つの要素を持つベクトルクラス AOS
 */

class MatrixAos44 : public MatrixStruct44Base {
public:
	typedef MatrixAos44 self_type; /**< 自分の型です */
	typedef MatrixStruct44Base parent_type; /**< 親の型です */
	typedef f32 value_type; /**< 要素の型です */
	static const s32 ROW_COUNT = 4;
	static const s32 COLUMN_COUNT = 4;

	// -------------------------------------------------------------------------
	// LookAtMatrix生成
	// -------------------------------------------------------------------------
	static MatrixAos44 GetLookAt( const Vector& eye, const Vector& aim, const Vector& up );
	static MatrixAos44 GetLookAtGL( const Vector& eye, const Vector& aim, const Vector& up );

	// -------------------------------------------------------------------------
	// Perspective生成
	// -------------------------------------------------------------------------
	static MatrixAos44 GetPerspective( float fovy, float aspect, float zNear, float zFar );
	static MatrixAos44 GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar );

	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	/**
	 *   デフォルトコンスタラクター
	 * @param  無し
	 * @return 無し
	 * @remark 何もしない
	 */

	MatrixAos44( void ) {
	}

	/**
	 *   Matrix44AOS を設定するコンスタラクター
	 * @param  m Matrix44AOSクラス
	 * @return 無し
	 */

	MatrixAos44( const MatrixAos44& m ) {
		Set( m );
	}

	/**
	 *   Matrix44AOS を設定するコンスタラクター
	 * @param  m Matrix44AOSクラス
	 * @return 無し
	 */

	MatrixAos44( const MatrixAos43& m ) {
		Set( m );
	}

	// -------------------------------------------------------------------------
	// Destruct !! not virtual for alignment
	// -------------------------------------------------------------------------

	~MatrixAos44( ) {
	}
#if 0
#include "gfl_MatrixAosCommon.h"
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
		Scale( scl );
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

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   指定された列を取得
	 * @param  i 添え字 0 ～ 3
	 * @return 指定された列
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline Vector4& GetRow( const s32 i ) const {
		return const_cast<Vector4&> (reinterpret_cast<const Vector4&> (GFL_MTX_ARRAY2D[i]));
	}

	/**
	 *   座標成分を取得
	 * @param  無し
	 * @return 座標
	 */

	inline Vector& GetTrans( void ) const {
		return (Vector&) GetRow( 3 );
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------
	/**
	 *   BaseMatrix44 を設定
	 * @param  m 設定したい BaseMatrix44 の参照
	 * @return 無し
	 */
	void Set( const MatrixAos44& m );

	/**
	 *   BaseMatrix43 を設定
	 * @param  m 設定したい BaseMatrix43 の参照
	 * @return 無し
	 */
	void Set( const MatrixAos43& m );
#if 0 // 

	/**
	 *   BaseMatrix44 を設定
	 * @param  m 設定したい BaseMatrix44 の参照
	 * @return 無し
	 */

	inline void Set( const MatrixAos44* m ) {
		Set( *m );
	}

	/**
	 *   BaseMatrix44 を設定
	 * @param  m 設定したい BaseMatrix43 の参照
	 * @return 無し
	 */

	inline void Set( const MatrixAos43Base* m ) {
		Set( *m );
	}
#endif
	/**
	 *   BaseMatrix44 を設定
	 * @param  m0  要素 @param  m1  要素 @param  m2  要素 @param  m3  要素 @param  m4 要素
	 * @param  m5  要素 @param  m6  要素 @param  m7  要素 @param  m8  要素 @param  m9 要素
	 * @param  m10 要素 @param  m11 要素 @param  m12 要素 @param  m13 要素 @param  m14 要素
	 * @param  m15 要素
	 * @return 無し
	 */
	void Set( f32 m0, f32 m1, f32 m2, f32 m3,
			 f32 m4, f32 m5, f32 m6, f32 m7,
			 f32 m8, f32 m9, f32 m10, f32 m11,
			 f32 m12, f32 m13, f32 m14, f32 m15 );

	/**
	 *   要素を設定
	 * @param  i 要素
	 * @param  m 値
	 * @return 無し
	 */

	inline void SetRow( const u32 i, const VectorStruct4& m0 ) {
		GFL_MTX_VECTOR[i] = m0; // うまくいかない
	}

	/**
	 *   要素を設定
	 * @param  i 要素
	 * @param  m 値
	 * @return 無し
	 */

	inline void SetRow( const u32 i, const VectorStruct3& m0 ) {
		GFL_MTX_VECTOR[i].GFL_VECTOR_X = m0.GFL_VECTOR_X;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Y = m0.GFL_VECTOR_Y;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Z = m0.GFL_VECTOR_Z;
	}

	/**
	 *   要素を設定
	 * @param  i 要素
	 * @param  m 値
	 * @return 無し
	 */

	inline void SetRow( const u32 i, const VectorStruct3& m0, const f32 w ) {
		GFL_MTX_VECTOR[i].GFL_VECTOR_X = m0.GFL_VECTOR_X;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Y = m0.GFL_VECTOR_Y;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Z = m0.GFL_VECTOR_Z;
		GFL_MTX_VECTOR[i].GFL_VECTOR_W = w;
	}

	/**
	 *   単位行列を設定
	 * @param  無し
	 * @return 無し
	 */
	void SetUnit( void );

	/**
	 *   単位行列を角度成分のみに設定
	 * @param  無し
	 * @return 無し
	 */
	inline void SetUnit33( void ) {
		SetRow( 0, Vector4::GetX1( ) );
		SetRow( 1, Vector4::GetY1( ) );
		SetRow( 2, Vector4::GetZ1( ) );
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
		SetTransW( 1.0f );
	}

	/**
	 *   Ｗ成分を設定
	 * @param  i 要素番号 AOS の場合 GFL_MTX_ARRAY2D[i][3]
	 * @param  val セットする値　デフォルトは０
	 * @return 無し
	 */

	inline void SetW( const s32 i, const f32 val = 0.0f ) {
		GFL_MTX_ARRAY2D[i][3] = val;
	}

	/**
	 *   座標成分を設定
	 * @param  w パッド成分
	 * @return 無し
	 */

	inline void SetTransW( const f32 w ) {
		Set( 3, TW, w );
	}

	/**
	 *   座標成分を（０，０，０，１）に設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetTransUnit( void ) {
		SetRow( 3, Vector4::GetW1( ) );
	}

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
	void RotateX( const MatrixAos44& m0, const f32 s, const f32 c );

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
	void RotateY( const MatrixAos44& m0, const f32 s, const f32 c );

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
	void RotateY_Reverse( const MatrixAos44& m0, const f32 s, const f32 c );

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
	void RotateZ( const MatrixAos44& m0, const f32 s, const f32 c );

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
	void Mul33( const MatrixAos44& m0, const MatrixAos44& m1 );

	/**
	 *   乗算
	 * @param  m0 入力行列
	 * @param  m1 入力行列
	 * @return 無し
	 */
	void Mul( const MatrixAos44& m0, const MatrixAos44& m1 );

	/**
	 *   クォータニオンとの乗算
	 * @param  q0 入力クォータニオン
	 * @return 無し
	 * @remark 行列に変換してから Mul33 している
	 */
	inline void Mul( const Quaternion& q0 );

	// -------------------------------------------------------------------------
	// Determinant
	// -------------------------------------------------------------------------
	/**
	 *   ３Ｘ３の決定因子
	 * @param  無し
	 * @return 決定因子
	 */
	f32 Determinant33( void ) const;

	/**
	 *   決定因子
	 * @param  無し
	 * @return 決定因子
	 */
	f32 Determinant( void ) const;

	// -------------------------------------------------------------------------
	// Inverse
	// -------------------------------------------------------------------------
	/**
	 *   ３Ｘ３の逆行列
	 * @param  m0 入力行列
	 * @return 無し
	 * @remark 座標成分には（０，０，０，１）が設定される。\n
	 * スケールがかかってない場合は転置行列の使用をお勧めします。
	 */
	void Inverse33( const MatrixAos44& m0 );

	/**
	 *   逆行列
	 * @param  m0 入力行列
	 * @return 無し
	 * @remark 重い
	 */
	void Inverse( const MatrixAos44& m0 );

	/**
	 *   Y座標以下にならないようなベクトルを求める
	 * @param  res 計算結果ベクトル
	 * @param  y0 最低Y座標
	 * @param  r 骨の長さ
	 * @return なし
	 */

	void GetPositionByLowestY( VectorNative* res, const f32 y0, const f32 r ) const;

	// -------------------------------------------------------------------------
	// 変換
	// -------------------------------------------------------------------------
	/**
	 *   クォータニオンを取得
	 * @param  q 出力クォータニオン
	 * @return 無し
	 */
	void MatrixToQuaternion( QuaternionNative* q ) const;

	/**
	 *   クォータニオンを取得
	 * @param  無し
	 * @return q 出力クォータニオン
	 */

	Quaternion MatrixToQuaternion( void ) const;

	/**
	 *   クォータニオンから行列を設定
	 * @param  q 入力クォータニオン
	 * @return 無し
	 */
	void QuaternionToMatrix( const Quaternion& q );

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

	bool AimXY( const VectorNative& tag, f32* xang_sin, f32* xang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const;
	/**
	 *   ターゲットポジションに骨を向けてＺＹ回転させる
	 * @param  tag 目標座標
	 * @return なし
	 */

	bool AimZY( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );

	/**
	 *   ターゲットポジションに骨を向けてＹZ回転させる
	 * @param  tag 目標座標
	 * @param  reverse 目標座標の符号を反転させるか
	 * @param  rate 向ける割合　０　向けない　１　完全に向ける
	 * @return なし
	 */

	bool AimYZ( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );

	bool AimYX( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );

	bool AimXZ( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );

	bool AimXY( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f );

	void AimX( f32* xsin, f32* xcos );
	void AimX( void );

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------


	/**
	 *   補完
	 * @param  m0 補完したいクラス
	 * @param  m1 補完したいクラス
	 * @param  rate 内分値
	 * @return 無し
	 * @remark this = Vector*rate + Vector*(1.0f-rate)
	 */
	MatrixAos44& Slerp( const MatrixAos44& m0, const MatrixAos44& m1, const f32 rate );

#if GFL_USE_SOA
	void FromSOA( const MatrixSoaNative33& src );
	void FromSOA( const MatrixSoaNative34& src );
	void FromSOA( const MatrixSoaNative44& src );
	void ToSOA( MatrixSoaNative33* mat ) const;
	void ToSOA( MatrixSoaNative34* mat ) const;
	void ToSOA( MatrixSoaNative44* mat ) const;
#endif
	// -------------------------------------------------------------------------
	// Transpose
	// -------------------------------------------------------------------------
	/**
	 *   転置行列を設定
	 * @param  m0 入力行列の参照
	 * @return 無し
	 */
	void Transpose( const MatrixAos44& m0 );

	/**
	 *   転置行列を設定
	 * @param  無し
	 * @return 無し
	 */
	void Transpose( void );

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------
	/**
	 *   コンソールに各成分を出力
	 * @param  無し
	 * @return 無し
	 */
	void PrintConsole( void ) const;
};

#endif	/* GFL_MATRIX44AOS_H */


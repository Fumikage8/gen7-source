//==============================================================================
/**
 * @file	gfl_MatrixSoaNative44.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/23, 18:11
 */
// =============================================================================

#if !defined( __GFL_MATRIXSOANATIVE44_H__ )
#define	__GFL_MATRIXSOANATIVE44_H__
#pragma once

#ifndef GFL_PLATFORM_3DS

enum PivotDirection {
	PIVOT_NONE, /**< 回転処理をおこないません。*/
	PIVOT_UPSIDE_TO_TOP, /**< カメラ上方向が画面上方向を向くように回転します。*/
	PIVOT_UPSIDE_TO_RIGHT, /**< カメラ上方向が画面右方向を向くように回転します。*/
	PIVOT_UPSIDE_TO_BOTTOM, /**< カメラ上方向が画面下方向を向くように回転します。*/
	PIVOT_UPSIDE_TO_LEFT, /**< カメラ上方向が画面左方向を向くように回転します。*/
	PIVOT_NUM
};

/**
 * SOA 44 型のネイティブクラス　PC用
 */
class MatrixSoaNative44 : public MatrixStruct44Base {
public:
	typedef MatrixSoaNative44 self_type; /**< 自分の型です */
	typedef f32 value_type; /**< 要素の型です */
	static const s32 ROW_COUNT = 4;
	static const s32 COLUMN_COUNT = 4;

	MatrixSoaNative44( void ) {
	}

	explicit MatrixSoaNative44( const f32* p ) {
		const Vector4* vp = reinterpret_cast<const Vector4*> (p);
		GetRow( 0 ) = vp[0];
		GetRow( 1 ) = vp[1];
		GetRow( 2 ) = vp[2];
		GetRow( 3 ) = vp[3];
	}

	explicit MatrixSoaNative44( const MatrixSoaNative34& mat ) {
		GetRow( 0 ) = mat.GetRow( 0 );
		GetRow( 1 ) = mat.GetRow( 1 );
		GetRow( 2 ) = mat.GetRow( 2 );
	}

	MatrixSoaNative44( const MatrixSoaNative44& mat ) {
		GetRow( 0 ) = mat.GetRow( 0 );
		GetRow( 1 ) = mat.GetRow( 1 );
		GetRow( 2 ) = mat.GetRow( 2 );
		GetRow( 3 ) = mat.GetRow( 3 );
	}

	explicit MatrixSoaNative44( f32 val0, f32 val1, f32 val2, f32 val3,
							 f32 val4, f32 val5, f32 val6, f32 val7, f32 val8,
							 f32 val9, f32 val10, f32 val11, f32 val12, f32 val13,
							 f32 val14, f32 val15 ) {
		GFL_MTX_00 = val0;
		GFL_MTX_01 = val1;
		GFL_MTX_02 = val2;
		GFL_MTX_03 = val3;
		GFL_MTX_10 = val4;
		GFL_MTX_11 = val5;
		GFL_MTX_12 = val6;
		GFL_MTX_13 = val7;
		GFL_MTX_20 = val8;
		GFL_MTX_21 = val9;
		GFL_MTX_22 = val10;
		GFL_MTX_23 = val11;
		GFL_MTX_30 = val12;
		GFL_MTX_31 = val13;
		GFL_MTX_32 = val14;
		GFL_MTX_33 = val15;
	}

	~MatrixSoaNative44( void ) {
	}

	/**
	 * 浮動小数点のポインタを取得する
	 * @return 浮動小数点のポインタ
	 */
	inline operator f32*() {
		return &GFL_MTX_00;
	}

	/**
	 * 浮動小数点のポインタを取得する
	 * @return 浮動小数点のポインタ
	 */
	inline operator const f32*() const {
		return &GFL_MTX_00;
	}

	/**
	 *   指定された列を取得
	 * @param  i 添え字 0 〜 2
	 * @return 指定された列
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline VectorNative4& GetRow( const s32 i ) {
		return reinterpret_cast<VectorNative4&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   指定された列を取得
	 * @param  i 添え字 0 〜 2
	 * @return 指定された列
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline const VectorNative4& GetRow( const s32 i ) const {
		return reinterpret_cast<const VectorNative4&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   指定された行を取得
	 * @param  i 添え字 0 〜 3
	 * @return 指定された列
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline VectorNative4 GetColumn( const s32 i ) const {
		return VectorNative4( GFL_MTX_ARRAY2D[0][i], GFL_MTX_ARRAY2D[1][i], GFL_MTX_ARRAY2D[2][i], GFL_MTX_ARRAY2D[3][i] );
	}

	/**
	 *   要素を設定
	 * @param  i 要素
	 * @param  m 値
	 * @return 無し
	 */

	inline void SetColumn( const s32 i, const VectorNative4& val ) {
		GFL_MTX_ARRAY2D[0][i] = val.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][i] = val.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][i] = val.GFL_VECTOR_Z;
		GFL_MTX_ARRAY2D[3][i] = val.GFL_VECTOR_W;
	}

	/**
	 *   オペレーター +=
	 * @param  t 加算したいクラス
	 * @return this の参照
	 */

	self_type & operator+=(const self_type& t);

	/**
	 *   オペレーター -=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	self_type & operator-=(const self_type& t);

	/**
	 *   オペレーター *=
	 * @param  f 乗算したい値
	 * @return this の参照
	 */

	self_type & operator*=(const f32 f);

	/**
	 *   オペレーター *=
	 * @param  t 乗算したいクラス
	 * @return this の参照
	 */

	self_type & operator*=(const self_type& t);

	/**
	 *   オペレーター /=
	 * @param  f 除算したい値
	 * @return this の参照
	 */

	self_type & operator/=(const f32 f);

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
		self_type r(*this);
		r += t;
		return r;
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス self_type
	 */

	inline self_type operator-() const {
		return self_type( -GFL_MTX_00, -GFL_MTX_01, -GFL_MTX_02, -GFL_MTX_03,
						 -GFL_MTX_10, -GFL_MTX_11, -GFL_MTX_12, -GFL_MTX_13,
						 -GFL_MTX_20, -GFL_MTX_21, -GFL_MTX_22, -GFL_MTX_23,
						 -GFL_MTX_30, -GFL_MTX_31, -GFL_MTX_32, -GFL_MTX_33 );
	}

	/**
	 *   オペレーター -
	 * @param  t 減算したいクラス
	 * @return 減算されたクラス self_type
	 */

	inline self_type operator-(const self_type& t) const {
		self_type r(*this);
		r -= t;
		return r;
	}

	inline self_type operator*(const f32 f) const {
		self_type r(*this);
		r *= f;
		return r;
	}

	/**
	 *   オペレーター /
	 * @param  f 除算したい値
	 * @return 除算されたクラス Vector2f
	 */

	inline self_type operator/(const f32 f) const {
		self_type r(*this);
		r /= f;
		return r;
	}

	/**
	 *   角度成分の転置行列を設定
	 * @param  無し
	 * @return 無し
	 */
	self_type& Transpose( void );

	/**
	 * 単位行列を設定
	 * @return 
	 */

	inline self_type& SetupIdentity( void ) {
		GetRow( 0 ) = Vector4::GetX1( );
		GetRow( 1 ) = Vector4::GetY1( );
		GetRow( 2 ) = Vector4::GetZ1( );
		GetRow( 3 ) = Vector4::GetW1( );
		return *this;
	}

	/**
	 * スケール行列を設定
	 * @return 
	 */

	inline self_type& SetupScale( const VectorNative3& scl ) {
		SetupIdentity( );
		GFL_MTX_00 = scl.GFL_VECTOR_X;
		GFL_MTX_11 = scl.GFL_VECTOR_Y;
		GFL_MTX_22 = scl.GFL_VECTOR_Z;
		return *this;
	}

	/**
	 * 移動行列を設定
	 * @return 
	 */

	inline self_type& SetupTranslate( const VectorNative3& tra ) {
		SetupIdentity( );
		GFL_MTX_ARRAY2D[0][3] = tra.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][3] = tra.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][3] = tra.GFL_VECTOR_Z;
		return *this;
	}

	/**
	 * 回転行列を設定
	 * @return 
	 */

	inline self_type& SetupRotateXyz( const VectorNative3& rot );

	/**
	 * 回転行列を設定
	 * @return 
	 */

	inline self_type& SetupRotate( const VectorNative3& rot, f32 ang );

	/**
	 * 回転行列を設定
	 * @return 
	 */

	inline self_type& SetupRotate( const QuaternionNative& qua );
#if 0 // 
	self_type & SetupFrustum( f32 l, f32 r, f32 b, f32 t, f32 n, f32 f,
							 PivotDirection pivot = PIVOT_NONE ) {
		return *this;
	}

	self_type & SetupOrtho( f32 l, f32 r, f32 b, f32 t, f32 n, f32 f,
						 PivotDirection pivot = PIVOT_NONE ) {
		return *this;
	}
	self_type & SetupPerspective( f32 fovyRad, f32 aspect, f32 n, f32 f,
								  PivotDirection pivot = PIVOT_NONE ){
		return *this;
	}
#else
	self_type & SetupFrustum( f32 , f32 , f32 , f32 , f32 , f32 ,
							 PivotDirection ) {
		return *this;
	}

	self_type & SetupOrtho( f32 , f32 , f32 , f32 , f32 , f32 ,
						 PivotDirection ) {
		return *this;
	}
	self_type & SetupPerspective( f32 , f32 , f32 , f32 ,
								  PivotDirection ){
		return *this;
	}
#endif
	/**
	 *   比較
	 * @param  m0 比較対象行列
	 * @return 各成分が全て完全に一致した場合は真、他は偽
	 */
	bool operator==(const self_type& m0) const {
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
	bool operator!=(const self_type& m0) const {
		return GetRow( 0 ) != m0.GetRow( 0 ) ||
			GetRow( 1 ) != m0.GetRow( 1 ) ||
			GetRow( 2 ) != m0.GetRow( 2 ) ||
			GetRow( 3 ) != m0.GetRow( 3 );
	}

	/**
	 * 単位行列か？
	 * @return 真　単位行列のとき
	 */
	bool IsIdentity( ) const {
		return Identity( ) == *this;
	}

	/**
	 * 単位行列の取得
	 * @return 
	 */
	static const self_type& Identity( ) {
		return *reinterpret_cast<const self_type*> (&Vector4::s_X1);
	}

	/**
	 * 状態を出力
	 * @param newline 改行する
	 * @param str 表示文字列
	 */
	void Report( bool newline = true, const c8* str = 0 ) {
		const c8 c = newline ? '\n' : ' ';
		Debug::PrintConsole( "%s =%c", str, c );
		Debug::PrintConsole( "%ff, %ff, %ff, %ff,%c%ff, %ff, %ff, %ff,%c%ff, %ff, %ff, %ff%c, %ff, %ff, %ff%c",
							 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, GFL_MTX_03, c,
							 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, GFL_MTX_13, c,
							 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, GFL_MTX_23, c,
							 GFL_MTX_30, GFL_MTX_31, GFL_MTX_32, GFL_MTX_33, c );
	}
};
#endif

#endif	/* __GFL_MATRIXSOANATIVE44_H__ */


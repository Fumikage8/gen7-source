//==============================================================================
/**
 * @file	gfl_MatrixSoaNative34.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/22, 10:48
 */
// =============================================================================

#if !defined( __GFL_MATRIXSOA34NATIVE_H__ )
#define	__GFL_MATRIXSOA34NATIVE_H__
#pragma once
#ifndef GFL_PLATFORM_3DS

/**
 * 共用体
 */
struct MatrixStruct34 {

	union {

		/** パラメーターひとつずつアクセスする場合 */
		struct {
			f32 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, GFL_MTX_03,
				GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, GFL_MTX_13,
				GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, GFL_MTX_23;
		};
		// コンパイルを通すために残してる

		struct {
			f32 _m00, _m01, _m02, _m03,
				_m10, _m11, _m12, _m13,
				_m20, _m21, _m22, _m23;
		};
		/** １次元配列としてアクセスする場合 */
		f32 GFL_MTX_ARRAY1D[12];
		/** ２次元配列としてアクセスする場合 */
		f32 GFL_MTX_ARRAY2D[3][4];
		/** TupleStr3 としてアクセスする場合 */
		VectorStruct4 GFL_MTX_VECTOR[3];
	};
};

/**
 * SOA 34 型のネイティブクラス　PC用
 */
class MatrixSoaNative34 : public MatrixStruct34 {
public:
	typedef MatrixSoaNative34 self_type; /**< 自分の型です */
	typedef f32 value_type; /**< 要素の型です */
	static const s32 ROW_COUNT = 3;
	static const s32 COLUMN_COUNT = 4;

	MatrixSoaNative34( void ) {
	}

	explicit MatrixSoaNative34( const f32* p ) {
		const Vector4* vp = reinterpret_cast<const Vector4*> (p);
		GetRow( 0 ) = vp[0];
		GetRow( 1 ) = vp[1];
		GetRow( 2 ) = vp[2];
	}

	explicit MatrixSoaNative34( const MatrixSoaNative33& mat ) {
		(Vector3&) GetRow( 0 ) = mat.GetRow( 0 );
		(Vector3&) GetRow( 1 ) = mat.GetRow( 1 );
		(Vector3&) GetRow( 2 ) = mat.GetRow( 2 );
	}

	MatrixSoaNative34( const MatrixSoaNative34& mat ) {
		GetRow( 0 ) = mat.GetRow( 0 );
		GetRow( 1 ) = mat.GetRow( 1 );
		GetRow( 2 ) = mat.GetRow( 2 );
	}

	explicit MatrixSoaNative34( f32 val0, f32 val1, f32 val2, f32 val3,
							 f32 val4, f32 val5, f32 val6, f32 val7, f32 val8,
							 f32 val9, f32 val10, f32 val11 ) {
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
	}

	~MatrixSoaNative34( void ) {
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
	 * @param  i 添え字 0 ～ 2
	 * @return 指定された列
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline VectorNative4& GetRow( const s32 i ) {
		return reinterpret_cast<VectorNative4&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   指定された列を取得
	 * @param  i 添え字 0 ～ 2
	 * @return 指定された列
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline const VectorNative4& GetRow( const s32 i ) const {
		return reinterpret_cast<const VectorNative4&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   指定された行を取得
	 * @param  i 添え字 0 ～ 3
	 * @return 指定された列
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline VectorNative3 GetColumn( const s32 i ) const {
		return VectorNative3( GFL_MTX_ARRAY2D[0][i], GFL_MTX_ARRAY2D[1][i], GFL_MTX_ARRAY2D[2][i] );
	}

	/**
	 *   要素を設定
	 * @param  i 要素
	 * @param  m 値
	 * @return 無し
	 */

	inline void SetColumn( const s32 i, const VectorNative3& val ) {
		GFL_MTX_ARRAY2D[0][i] = val.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][i] = val.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][i] = val.GFL_VECTOR_Z;
	}

	/**
	 *   オペレーター +=
	 * @param  t 加算したいクラス
	 * @return this の参照
	 * @remark この命令の意味が分からん。何に使うんじゃろうか…
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
						 -GFL_MTX_20, -GFL_MTX_21, -GFL_MTX_22, -GFL_MTX_23 );
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
	self_type& Transpose( void ) {
		Math::Swap( GFL_MTX_01, GFL_MTX_10 );
		Math::Swap( GFL_MTX_02, GFL_MTX_20 );
		Math::Swap( GFL_MTX_12, GFL_MTX_21 );
		return *this;
	}

	/**
	 * 単位行列を設定
	 * @return 
	 */

	inline self_type& SetupIdentity( void ) {
		GetRow( 0 ) = Vector4::GetX1( );
		GetRow( 1 ) = Vector4::GetY1( );
		GetRow( 2 ) = Vector4::GetZ1( );
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
		SetColumn( 3, tra );
		return *this;
	}

	/**
	 * 回転行列を設定
	 * @return 
	 */

	self_type& SetupRotateXyz( const VectorNative3& rot );

	/**
	 * 回転行列を設定
	 * @return 
	 */

	self_type& SetupRotate( const VectorNative3& rot, f32 ang );

	/**
	 * 回転行列を設定
	 * @return 
	 */

	self_type& SetupRotate( const QuaternionNative& qua );

	self_type& SetupLookAt( const VectorNative3& cam, const VectorNative3& up, const VectorNative3& tag );

	self_type& SetupCameraRotate( const VectorNative3& pos, const VectorNative3& deg );

#if 0 // 
	self_type& SetupLookAt( const VectorNative3& cam, const f32 deg, const VectorNative3& tag ){
		return *this;//_SetupLookAt(cam,Math::DegreeToRadian(deg),tag);
	}
	self_type & SetupTextureProjectionFrustum( f32 l, f32 r, f32 b, f32 t,
											   f32 n, f32 scaleS, f32 scaleT,
											   f32 translateS, f32 translateT ){
		return *this;
	}

	self_type & SetupTextureProjectionFrustum( f32 fovyRad, f32 aspect,
											   f32 scaleS, f32 scaleT,
											   f32 translateS, f32 translateT ){
		return *this;
	}

	self_type & SetupTextureProjectionFrustum( f32 l, f32 r, f32 b, f32 t,
											   f32 scaleS, f32 scaleT,
											   f32 translateS, f32 translateT ){
		return *this;
	}
#else // dummy ... wait for a while
	self_type& SetupLookAt( const VectorNative3& , const f32 , const VectorNative3& ){
		return *this;//_SetupLookAt(cam,Math::DegreeToRadian(deg),tag);
	}
	self_type & SetupTextureProjectionFrustum( f32 , f32 , f32 , f32 ,
											   f32 , f32 , f32 ,
											   f32 , f32 ){
		return *this;
	}

	self_type & SetupTextureProjectionFrustum( f32 , f32 ,
											   f32 , f32 ,
											   f32 , f32 ){
		return *this;
	}

	self_type & SetupTextureProjectionFrustum( f32 , f32 , f32 , f32 ,
											   f32 , f32 ,
											   f32 , f32 ){
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
			GetRow( 2 ) == m0.GetRow( 2 );
	}

	/**
	 *   比較
	 * @param  m0 比較対象行列
	 * @return 各成分が全て完全に一致した場合は偽、他は真
	 */
	bool operator!=(const self_type& m0) const {
		return GetRow( 0 ) != m0.GetRow( 0 ) ||
			GetRow( 1 ) != m0.GetRow( 1 ) ||
			GetRow( 2 ) != m0.GetRow( 2 );
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
		return *reinterpret_cast<const self_type*> (&Vector3::s_X1);
	}

	/**
	 * 状態を出力
	 * @param newline 改行する
	 * @param str 表示文字列
	 */
	void Report( bool newline = true, const c8* str = 0 ) {
		const c8 c = newline ? '\n' : ' ';
		Debug::PrintConsole( "%s =%c", str, c );
		Debug::PrintConsole( "%ff, %ff, %ff, %ff,%c%ff, %ff, %ff, %ff,%c%ff, %ff, %ff, %ff%c",
							 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, GFL_MTX_03, c,
							 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, GFL_MTX_13, c,
							 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, GFL_MTX_23, c );
	}
};
#endif

#endif	/* __GFL_MATRIXSOA34NATIVE_H__ */

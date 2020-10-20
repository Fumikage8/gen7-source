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
 * ���p��
 */
struct MatrixStruct34 {

	union {

		/** �p�����[�^�[�ЂƂ��A�N�Z�X����ꍇ */
		struct {
			f32 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, GFL_MTX_03,
				GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, GFL_MTX_13,
				GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, GFL_MTX_23;
		};
		// �R���p�C����ʂ����߂Ɏc���Ă�

		struct {
			f32 _m00, _m01, _m02, _m03,
				_m10, _m11, _m12, _m13,
				_m20, _m21, _m22, _m23;
		};
		/** �P�����z��Ƃ��ăA�N�Z�X����ꍇ */
		f32 GFL_MTX_ARRAY1D[12];
		/** �Q�����z��Ƃ��ăA�N�Z�X����ꍇ */
		f32 GFL_MTX_ARRAY2D[3][4];
		/** TupleStr3 �Ƃ��ăA�N�Z�X����ꍇ */
		VectorStruct4 GFL_MTX_VECTOR[3];
	};
};

/**
 * SOA 34 �^�̃l�C�e�B�u�N���X�@PC�p
 */
class MatrixSoaNative34 : public MatrixStruct34 {
public:
	typedef MatrixSoaNative34 self_type; /**< �����̌^�ł� */
	typedef f32 value_type; /**< �v�f�̌^�ł� */
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
	 * ���������_�̃|�C���^���擾����
	 * @return ���������_�̃|�C���^
	 */
	inline operator f32*() {
		return &GFL_MTX_00;
	}

	/**
	 * ���������_�̃|�C���^���擾����
	 * @return ���������_�̃|�C���^
	 */
	inline operator const f32*() const {
		return &GFL_MTX_00;
	}

	/**
	 *   �w�肳�ꂽ����擾
	 * @param  i �Y���� 0 �` 2
	 * @return �w�肳�ꂽ��
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline VectorNative4& GetRow( const s32 i ) {
		return reinterpret_cast<VectorNative4&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   �w�肳�ꂽ����擾
	 * @param  i �Y���� 0 �` 2
	 * @return �w�肳�ꂽ��
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline const VectorNative4& GetRow( const s32 i ) const {
		return reinterpret_cast<const VectorNative4&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   �w�肳�ꂽ�s���擾
	 * @param  i �Y���� 0 �` 3
	 * @return �w�肳�ꂽ��
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline VectorNative3 GetColumn( const s32 i ) const {
		return VectorNative3( GFL_MTX_ARRAY2D[0][i], GFL_MTX_ARRAY2D[1][i], GFL_MTX_ARRAY2D[2][i] );
	}

	/**
	 *   �v�f��ݒ�
	 * @param  i �v�f
	 * @param  m �l
	 * @return ����
	 */

	inline void SetColumn( const s32 i, const VectorNative3& val ) {
		GFL_MTX_ARRAY2D[0][i] = val.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][i] = val.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][i] = val.GFL_VECTOR_Z;
	}

	/**
	 *   �I�y���[�^�[ +=
	 * @param  t ���Z�������N���X
	 * @return this �̎Q��
	 * @remark ���̖��߂̈Ӗ����������B���Ɏg���񂶂�낤���c
	 */

	self_type & operator+=(const self_type& t);

	/**
	 *   �I�y���[�^�[ -=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	self_type & operator-=(const self_type& t);

	/**
	 *   �I�y���[�^�[ *=
	 * @param  f ��Z�������l
	 * @return this �̎Q��
	 */

	self_type & operator*=(const f32 f);

	/**
	 *   �I�y���[�^�[ *=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	self_type & operator*=(const self_type& t);

	/**
	 *   �I�y���[�^�[ /=
	 * @param  f ���Z�������l
	 * @return this �̎Q��
	 */

	self_type & operator/=(const f32 f);

	/**
	 *   �I�y���[�^�[ +
	 * @return �������g��Ԃ��B�Ȃ񂾂���H
	 */

	inline self_type operator+() const {
		return *this;
	}

	/**
	 *   �I�y���[�^�[ +
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X self_type
	 */

	inline self_type operator+(const self_type& t) const {
		self_type r(*this);
		r += t;
		return r;
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X self_type
	 */

	inline self_type operator-() const {
		return self_type( -GFL_MTX_00, -GFL_MTX_01, -GFL_MTX_02, -GFL_MTX_03,
						 -GFL_MTX_10, -GFL_MTX_11, -GFL_MTX_12, -GFL_MTX_13,
						 -GFL_MTX_20, -GFL_MTX_21, -GFL_MTX_22, -GFL_MTX_23 );
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X self_type
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
	 *   �I�y���[�^�[ /
	 * @param  f ���Z�������l
	 * @return ���Z���ꂽ�N���X Vector2f
	 */

	inline self_type operator/(const f32 f) const {
		self_type r(*this);
		r /= f;
		return r;
	}

	/**
	 *   �p�x�����̓]�u�s���ݒ�
	 * @param  ����
	 * @return ����
	 */
	self_type& Transpose( void ) {
		Math::Swap( GFL_MTX_01, GFL_MTX_10 );
		Math::Swap( GFL_MTX_02, GFL_MTX_20 );
		Math::Swap( GFL_MTX_12, GFL_MTX_21 );
		return *this;
	}

	/**
	 * �P�ʍs���ݒ�
	 * @return 
	 */

	inline self_type& SetupIdentity( void ) {
		GetRow( 0 ) = Vector4::GetX1( );
		GetRow( 1 ) = Vector4::GetY1( );
		GetRow( 2 ) = Vector4::GetZ1( );
		return *this;
	}

	/**
	 * �X�P�[���s���ݒ�
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
	 * �ړ��s���ݒ�
	 * @return 
	 */

	inline self_type& SetupTranslate( const VectorNative3& tra ) {
		SetupIdentity( );
		SetColumn( 3, tra );
		return *this;
	}

	/**
	 * ��]�s���ݒ�
	 * @return 
	 */

	self_type& SetupRotateXyz( const VectorNative3& rot );

	/**
	 * ��]�s���ݒ�
	 * @return 
	 */

	self_type& SetupRotate( const VectorNative3& rot, f32 ang );

	/**
	 * ��]�s���ݒ�
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
	 *   ��r
	 * @param  m0 ��r�Ώۍs��
	 * @return �e�������S�Ċ��S�Ɉ�v�����ꍇ�͐^�A���͋U
	 */
	bool operator==(const self_type& m0) const {
		return GetRow( 0 ) == m0.GetRow( 0 ) &&
			GetRow( 1 ) == m0.GetRow( 1 ) &&
			GetRow( 2 ) == m0.GetRow( 2 );
	}

	/**
	 *   ��r
	 * @param  m0 ��r�Ώۍs��
	 * @return �e�������S�Ċ��S�Ɉ�v�����ꍇ�͋U�A���͐^
	 */
	bool operator!=(const self_type& m0) const {
		return GetRow( 0 ) != m0.GetRow( 0 ) ||
			GetRow( 1 ) != m0.GetRow( 1 ) ||
			GetRow( 2 ) != m0.GetRow( 2 );
	}

	/**
	 * �P�ʍs�񂩁H
	 * @return �^�@�P�ʍs��̂Ƃ�
	 */
	bool IsIdentity( ) const {
		return Identity( ) == *this;
	}

	/**
	 * �P�ʍs��̎擾
	 * @return 
	 */
	static const self_type& Identity( ) {
		return *reinterpret_cast<const self_type*> (&Vector3::s_X1);
	}

	/**
	 * ��Ԃ��o��
	 * @param newline ���s����
	 * @param str �\��������
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

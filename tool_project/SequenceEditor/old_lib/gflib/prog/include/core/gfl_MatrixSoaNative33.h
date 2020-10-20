//==============================================================================
/**
 * @file	gfl_MatrixSoaNative33.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/22, 10:59
 */
// =============================================================================

#if !defined( __GFL_MATRIXSOA33NATIVE_H__ )
#define	__GFL_MATRIXSOA33NATIVE_H__
#pragma once
#ifndef GFL_PLATFORM_3DS

class MatrixSoaNative34;

/**
 * ���p��
 */
struct MatrixStruct33 {

	union {

		/** �p�����[�^�[�ЂƂ��A�N�Z�X����ꍇ */
		struct {
			f32 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02,
				GFL_MTX_10, GFL_MTX_11, GFL_MTX_12,
				GFL_MTX_20, GFL_MTX_21, GFL_MTX_22;
		};
		// �R���p�C����ʂ����߂Ɏc���Ă�

		struct {
			f32 _m00, _m01, _m02,
				_m10, _m11, _m12,
				_m20, _m21, _m22;
		};
		/** �P�����z��Ƃ��ăA�N�Z�X����ꍇ */
		f32 GFL_MTX_ARRAY1D[9];
		/** �Q�����z��Ƃ��ăA�N�Z�X����ꍇ */
		f32 GFL_MTX_ARRAY2D[3][3];
		/** TupleStr3 �Ƃ��ăA�N�Z�X����ꍇ */
		VectorStruct3 GFL_MTX_VECTOR[3];
	};
};

/**
 * SOA 33 �^�̃l�C�e�B�u�N���X�@PC�p
 */
class MatrixSoaNative33 : public MatrixStruct33 {
public:
    typedef MatrixSoaNative33 self_type; /**< �����̌^�ł� */
    typedef f32  value_type; /**< �v�f�̌^�ł� */
	static const s32 ROW_COUNT = 3;
	static const s32 COLUMN_COUNT = 3;

	MatrixSoaNative33( void ) {
	}

	explicit MatrixSoaNative33( const f32* p ) {
		const Vector3* vp = reinterpret_cast<const Vector3*> (p);
		GetRow(0) = vp[0];
		GetRow(1) = vp[1];
		GetRow(2) = vp[2];
	}

	MatrixSoaNative33( const MatrixSoaNative34& mat );

	explicit MatrixSoaNative33( f32 val0, f32 val1, f32 val2, f32 val3,
							 f32 val4, f32 val5, f32 val6, f32 val7, f32 val8 ) {
		GFL_MTX_00 = val0;
		GFL_MTX_01 = val1;
		GFL_MTX_02 = val2;
		GFL_MTX_10 = val3;
		GFL_MTX_11 = val4;
		GFL_MTX_12 = val5;
		GFL_MTX_20 = val6;
		GFL_MTX_21 = val7;
		GFL_MTX_22 = val8;
	}

	~MatrixSoaNative33( void ) {
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

	inline Vector3& GetRow( const s32 i ) {
		return reinterpret_cast<Vector3&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   �w�肳�ꂽ����擾
	 * @param  i �Y���� 0 �` 2
	 * @return �w�肳�ꂽ��
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline const Vector3& GetRow( const s32 i ) const {
		return reinterpret_cast<const Vector3&> (GFL_MTX_VECTOR[i]);
	}

	/**
	 *   �w�肳�ꂽ�s���擾
	 * @param  i �Y���� 0 �` 3
	 * @return �w�肳�ꂽ��
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline Vector3 GetColumn( const s32 i ) const {
		return Vector3( GFL_MTX_ARRAY2D[0][i], GFL_MTX_ARRAY2D[1][i], GFL_MTX_ARRAY2D[2][i] );
	}

	/**
	 *   �v�f��ݒ�
	 * @param  i �v�f
	 * @param  m �l
	 * @return ����
	 */

	inline void SetColumn( const s32 i, const Vector3& val ) {
		GFL_MTX_ARRAY2D[0][i] = val.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][i] = val.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][i] = val.GFL_VECTOR_Z;
	}

	/**
	 * �P�ʍs���ݒ�
	 * @return 
	 */

	inline self_type& SetupIdentity( void ) {
		GetRow( 0 ) = Vector3::GetX1( );
		GetRow( 1 ) = Vector3::GetY1( );
		GetRow( 2 ) = Vector3::GetZ1( );
	}

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
		Debug::PrintConsole( "%ff, %ff, %ff,%c%ff, %ff, %ff,%c%ff, %ff, %ff%c",
							 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, c,
							 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, c,
							 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, c );
	}
};
#endif // !GFL_PLATFORM_3DS

#endif	/* __GFL_MATRIXSOA33NATIVE_H__ */


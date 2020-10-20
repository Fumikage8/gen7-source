/* 
 * File:   gfl_Matrix44SOA.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 14:20
 */

#ifndef GFL_MATRIX44SOA_H
#define	GFL_MATRIX44SOA_H

#pragma once

// =============================================================================
// ���̃t�@�C���Ɋ܂܂��N���X�Q
// =============================================================================
class MatrixSoa44;

// =============================================================================
// ���̃t�@�C���ɂ͊܂܂�Ă��Ȃ����錾���K�v�ȃN���X�Q
// =============================================================================
class Quaternion;

/**
 *  �S�̗v�f�����x�N�g���N���X SOA
 */

class MatrixSoa44 : public MatrixSoaNative44 {
public:
	typedef MatrixSoaNative44 parent_type; /**< �e�̌^�ł� */
	typedef MatrixSoa44 self_type; /**< �����̌^�ł� */
#if 0
#include "gfl_MatrixSoaCommon.h"
#else
/**
	 * f32 �Ƃ��ă|�C���^���擾
	 * @return f32 �|�C���^
	 */
	inline f32* Get( void ) {
		return GFL_MTX_ARRAY1D;
	}

	/**
	 * const f32 �Ƃ��ă|�C���^���擾
	 * @return const f32 �|�C���^
	 */
	inline const f32* Get( void ) const {
		return GFL_MTX_ARRAY1D;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  idx �Y����
	 * @return �w�肳�ꂽ�����̒l
	 */
	inline f32 Get( const s32 idx ) const {
		return GFL_MTX_ARRAY1D[idx];
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y����
	 * @param  j �Y����
	 * @return �w�肳�ꂽ�����̒l
	 */
	inline f32 Get( const s32 i, const s32 j ) const {
		return GFL_MTX_ARRAY2D[i][j];
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y����
	 * @param  j �Y����
	 * @return �w�肳�ꂽ�����̒l
	 */
	inline f32& GetRef( const s32 i, const s32 j ) {
		return GFL_MTX_ARRAY2D[i][j];
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y����
	 * @return �w�肳�ꂽ�����̒l
	 */
	inline f32 operator() (const s32 i) const {
		return GFL_MTX_ARRAY1D[i];
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y����
	 * @param  j �Y����
	 * @return �w�肳�ꂽ�����̒l
	 */
	inline f32 operator() (const s32 i, const s32 j) const {
		return GFL_MTX_ARRAY2D[i][j];
	}

	/**
	 *   �w�肳�ꂽ�����̑Ίp�������擾
	 * @param  i �Y���� 0 �` 3
	 * @param  j �Y���� 0 �` 3
	 * @return �w�肳�ꂽ�����̒l
	 */

	inline f32 GetDiagonal( const s32 i, const s32 j ) const {
		return GFL_MTX_ARRAY2D[j][i];
	}

	/**
	 * �v�f��ݒ�
	 * @param i�@�Y��
	 * @param val�@�l
	 */
	inline void Set( const s32 i, const f32 val ) {
		GFL_MTX_ARRAY1D[i] = val;
	}

	/**
	 * �v�f��ݒ�
	 * @param i�@�Y��
	 * @param j�@�Y��
	 * @param val�@�l
	 */
	inline void Set( const s32 i, const s32 j, const f32 val ) {
		GFL_MTX_ARRAY2D[i][j] = val;
	}

	/**
	 *   Matrix �R�w�R������ݒ�
	 * @param  m0  �v�f @param  m1  �v�f @param  m2  �v�f @param  m3  �v�f @param  m4 �v�f
	 * @param  m5  �v�f @param  m6  �v�f @param  m7  �v�f @param  m8  �v�f
	 * @return ����
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
	 *   MATRIX_SOA_BASE �̂R�w�R������ݒ�
	 * @param  mat  �s��
	 * @return ����
	 */
	template<typename T>
	void Set33( const T& mat ) {
		SetRow( 0, reinterpret_cast<const VectorNative3&> (mat.GetRow( 0 )) );
		SetRow( 1, reinterpret_cast<const VectorNative3&> (mat.GetRow( 1 )) );
		SetRow( 2, reinterpret_cast<const VectorNative3&> (mat.GetRow( 2 )) );
	}

	/**
	 * �s��̍��W�����Ƃ̉��Z
	 * @param v0�@�o�̓x�N�g���N���X
	 * @param v1�@���Z������̓x�N�g���N���X
	 */
	inline void Add( VectorNative* v0, const VectorNative& v1 ) const {
		v0->GFL_VECTOR_X = v1.GFL_VECTOR_X + GetTransX( );
		v0->GFL_VECTOR_Y = v1.GFL_VECTOR_Y + GetTransY( );
		v0->GFL_VECTOR_Z = v1.GFL_VECTOR_Z + GetTransZ( );
	}

	/**
	 * �s��̍��W�����Ƃ̉��Z
	 * @param v0�@�o�̓x�N�g���\����
	 */
	inline void Add( VectorNative* v0 ) const {
		v0->GFL_VECTOR_X += GetTransX( );
		v0->GFL_VECTOR_Y += GetTransY( );
		v0->GFL_VECTOR_Z += GetTransZ( );
		/* How should I treat W */
	}

	/**
	 * �s��̍��W�����Ƃ̉��Z
	 * @param v0�@�o�̓x�N�g���\����
	 */
	inline void Add( const VectorNative& v0 ) {
		GetTransRefX( ) += v0.GFL_VECTOR_X;
		GetTransRefY( ) += v0.GFL_VECTOR_Y;
		GetTransRefZ( ) += v0.GFL_VECTOR_Z;
		/* How should I treat W */
	}

	/**
	 * �x�N�g�����p�x�����Ɋ|�������̂��v�Z
	 * @param v0�@���o�̓x�N�g���N���X�|�C���^
	 */
	inline void Apply( VectorNative* v0 ) const {
		Apply( v0, *v0 );
	}

	/**
	 * ���W�ړ�
	 * @param v0�@���̓x�N�g���N���X
	 */
	inline void Trans( const VectorNative& v0 ) {
		Vector v;
		Apply( &v, v0 );
		*this += v;
	}

	/**
	 * ���W�ړ�
	 * @param x�@������
	 * @param y�@������
	 * @param z�@������
	 */
	inline void Trans( const f32 x, const f32 y, const f32 z ) {
		Vector v( x, y, z );
		Apply( &v, v );
		*this += v;
	}

	/**
	 * ���W�ړ�
	 * @param v0�@�o�̓x�N�g���N���X�|�C���^
	 * @param v1�@���̓x�N�g���N���X
	 * @remark �s��Ɍv�Z���ʂ͔��f����Ȃ�
	 */
	inline void Trans( VectorNative* v0, const VectorNative& v1 ) const {
		Apply( v0, v1 );
		/*		v0->SetW( 0.0f ); */
		*v0 += GetTrans( );
	}

	/**
	 * ���W�ړ�
	 * @param v0�@�o�̓x�N�g���N���X
	 * @param x�@������
	 * @param y�@������
	 * @param z�@������
	 * @remark �s��Ɍv�Z���ʂ͔��f����Ȃ�
	 */
	inline void Trans( VectorNative* v0, const f32 x, const f32 y, const f32 z ) const {
		Vector v( x, y, z );
		Trans( v0, v );
	}

	/**
	 *   �s��̍��W������ݒ�
	 * @param  mat ���͍s��
	 * @return ����
	 */

	inline void SetTrans( const self_type& mat ) {
		SetTrans( mat.GetTrans( ) );
	}

	/**
	 *   �R�w�R�̌�����q
	 * @param  m0 ���͍s��
	 * @return ������q
	 */
	inline f32 Determinant33( const self_type& m0 ) const {
		return m0.Determinant33( );
	}

	/**
	 *   ������q
	 * @param  m0 ���͍s��
	 * @return ������q
	 */
	inline f32 Determinant( const self_type& m0 ) const {
		return m0.Determinant( );
	}

	/**
	 *   �R�w�R�̋t�s��
	 * @param  ����
	 * @return ����
	 * @remark ���W�����ɂ́i�O�C�O�C�O�C�P�j���ݒ肳���B\n
	 * �X�P�[�����������ĂȂ��ꍇ�͓]�u�s��̎g�p�������߂��܂��B
	 */

	inline void Inverse33( void ) {
		Inverse33( *this );
	}

	/**
	 * �t�s���ݒ�
	 */
	inline void Inverse( void ) {
		Inverse( *this );
	}

	// -----------------------------------------------------------------------------
	// Transpose33
	// -----------------------------------------------------------------------------

	/**
	 *   �p�x�����̓]�u�s���ݒ�
	 * @param  ����
	 * @return ����
	 */
	void Transpose33( void ) {
		Math::Swap( GFL_MTX_01, GFL_MTX_10 );
		Math::Swap( GFL_MTX_02, GFL_MTX_20 );
		Math::Swap( GFL_MTX_12, GFL_MTX_21 );
	}

	/**
	 *   �p�x�����̓]�u�s���ݒ�
	 * @param  m0 ���͍s��
	 * @return ����
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
	 *   �p�x�����̓]�u�s������߂�
	 * @param  ����
	 * @return �]�u���ꂽ�s��̎���
	 */

	self_type GetTranspose33( void ) const {
		self_type ret;
		ret.Transpose33( *this );
		return ret;
	}

	/**
	 *   �]�u�s������߂�
	 * @param  ����
	 * @return �]�u���ꂽ�s��̎���
	 */

	self_type GetTranspose( void ) const {
		self_type ret;
		ret.Transpose( *this );
		return ret;
	}

	/**
	 *   �p�x�����̓]�u�s������߂�
	 * @param  ����
	 * @return �]�u���ꂽ�s��̎���
	 */

	self_type GetInverse33( void ) const {
		self_type ret;
		ret.Inverse33( *this );
		return ret;
	}

	/**
	 *   �]�u�s������߂�
	 * @param  ����
	 * @return �]�u���ꂽ�s��̎���
	 */

	self_type GetInverse( void ) const {
		self_type ret;
		ret.Inverse( *this );
		return ret;
	}

	/**
	 * �O���[�o�����W�����[�J�����W�ɕϊ�
	 * @param loc�@���[�J�����W�@�o�̓x�N�g���N���X�|�C���^
	 * @param glo�@�O���[�o�����W�@���̓x�N�g��
	 */
	inline void GlobalToLocal( VectorNative* loc, const VectorNative& glo ) const {
		Vector pos;
		pos.Sub( glo, GetTrans( ) );
		GlobalToLocal33( loc, pos );
	}

	/**
	 * �O���[�o�����W�����[�J�����W�ɕϊ�
	 * @param loc�@���[�J�����W�@�o�̓x�N�g���N���X�|�C���^
	 * @param glo�@�O���[�o�����W�@���̓x�N�g��
	 */
	inline void GlobalToLocalScaled( VectorNative* loc, const VectorNative& glo ) const {
		Vector pos;
		pos.Sub( glo, GetTrans( ) );
		GlobalToLocal33Scaled( loc, pos );
	}

	/**
	 *   �^�[�Q�b�g�|�W�V�����ɍ��������Ăy�x�w��]������
	 * @param  tag �ڕW���W
	 * @return �Ȃ�
	 * @remark �w����␳���Ăx���������ێ�����
	 */


	bool AimZYX( const VectorNative& tag ) {
		if( AimZY( tag ) ){
			AimX( );
			return true;
		}
		return false;
	}

	/**
	 * �x�N�g���ƍs��̌����̑O�������������
	 * @param v�@�x�N�g��
	 * @return �^�@���������@�U�@���Ε���
	 */
	inline bool IsVectorSameDirection( const VectorNative& v ) const {
		return IsVectorSameDirection( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
	}

	/**
	 * �x�N�g���ƍs��̌����̍��E������������
	 * @param v�@�x�N�g��
	 * @return �^�@���������@�U�@���Ε���
	 */
	inline bool IsVectorSameSide( const VectorNative& v ) const {
		return IsVectorSameSide( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
	}

	/**
	 * �x�N�g���ƍs��̌��������������X�O�x�ȓ���
	 * @param v�@�x�N�g��
	 * @return �^�@���������X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsVectorSameDirectionDegree90( const VectorNative& v ) const {
		return IsVectorSameDirectionDegree90( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
	}

	/**
	 * ���W���s�񂩂�݂đO�ɂ��邩
	 * @param pos�@�`�F�b�N���������W
	 * @return �^�@�O�@�U�@���
	 */
	inline bool IsPositionFront( const VectorNative& pos ) const {
		return IsVectorSameDirection( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
	}

	/**
	 * ���W���s�񂩂�݂č��ɂ��邩
	 * @param pos�@�`�F�b�N���������W
	 * @return �^�@���@�U�@�E
	 */
	inline bool IsPositionLeft( const VectorNative& pos ) const {
		return IsVectorSameSide( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
	}

	/**
	 * ���W���s�񂩂�݂đO�X�O�x�ȓ��ɂ��邩
	 * @param pos�@�`�F�b�N���������W
	 * @return �^�@�O�X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsPositionFrontDegree90( const VectorNative& pos ) const {
		return IsVectorSameDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
	}

	/**
	 * ���W���s�񂩂�݂Č��X�O�x�ȓ��ɂ��邩
	 * @param pos�@�`�F�b�N���������W
	 * @return �^�@��X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsPositionBackDegree90( const VectorNative& pos ) const {
		return IsVectorOppositeDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
	}

	/**
	 * ���W���s�񂩂�݂č��X�O�x�ȓ��ɂ��邩
	 * @param pos�@�`�F�b�N���������W
	 * @return �^�@���X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsPositionLeftDegree90( const VectorNative& pos ) const {
		return IsVectorLeftDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
	}

	/**
	 * ���W���s�񂩂�݂ĉE�X�O�x�ȓ��ɂ��邩
	 * @param pos�@�`�F�b�N���������W
	 * @return �^�@�E�X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsPositionRightDegree90( const VectorNative& pos ) const {
		return IsVectorRightDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
	}

	/**
	 *   �s��̂w���ŉ�]�������p�x������ݒ�
	 * @param  rad �p�x�i���W�A���j
	 * @return ����
	 * @remark ���W�����ɂ͉������Ȃ�
	 */

	inline void SetRotateX( const f32 rad ) {
		f32 s, c;
		Math::FSinCos( &s, &c, rad );
		SetRotateX( s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param m0�@���͍s��
	 * @param rad�@���͊p�x���W�A��
	 * @remark m0 �� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateX( const self_type& m0, const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateX( m0, s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���݂̍s��� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateX( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateX( s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���݂̍s��� rad �̉�]���t����|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateX_Reverse( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateX_Reverse( s, c );
	}

	/**
	 * �s��ɂ���]��ݒ�
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���̊p�x�����̓N���A����܂��B���W�����ɑ΂��Ă͉������܂���
	 */
	inline void SetRotateY( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		SetRotateY( s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param m0�@���͍s��
	 * @param rad�@���͊p�x���W�A��
	 * @remark m0 �� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateY( const self_type& m0, const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateY( m0, s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���݂̍s��� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateY( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateY( s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���݂̍s��� rad �̉�]���t����|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateY_Reverse( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateY_Reverse( s, c );
	}

	/**
	 * �s��ɂ���]��ݒ�
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���̊p�x�����̓N���A����܂��B���W�����ɑ΂��Ă͉������܂���
	 */
	inline void SetRotateZ( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		SetRotateZ( s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param m0�@���͍s��
	 * @param rad�@���͊p�x���W�A��
	 * @remark m0 �� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateZ( const self_type& m0, const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateZ( m0, s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���݂̍s��� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateZ( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateZ( s, c );
	}

	/**
	 * �s��ɂ���]���|����
	 * @param rad�@���͊p�x���W�A��
	 * @remark ���݂̍s��� rad �̉�]���t����|�����킹�����̂�ݒ肵�܂��B
	 */
	inline void RotateZ_Reverse( const f32 rad ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, rad );
		RotateZ_Reverse( s, c );
	}

	/**
	 * �s���ZYX�̏��Ԃŉ�]���|�������̂�ݒ�
	 * @param x�@���͊p�x���W�A��
	 * @param y�@���͊p�x���W�A��
	 * @param z�@���͊p�x���W�A��
	 */
	inline void SetRotateZYX( const f32 x, const f32 y, const f32 z ) {
		SetRotateZ( z );
		RotateY( y );
		RotateX( x );
	}

	/**
	 * �s���ZYX�̏��Ԃŉ�]���|�������̂�ݒ�
	 * @param v�@���͊p�x���W�A���x�N�g���\����
	 */
	inline void SetRotateZYX( const VectorNative& v ) {
		SetRotateZYX( v.GFL_VECTOR_X, v.GFL_VECTOR_Y, v.GFL_VECTOR_Z );
	}

	/**
	 * �s���ZYX�̏��Ԃŉ�]���|�������̂�ݒ�
	 * @param v�@x,y,z �̏��Ԃœ��������������_�z��|�C���^
	 */
	inline void SetRotateZYX( const f32 v[] ) {
		SetRotateZYX( v[TX], v[TY], v[TZ] );
	}

	/**
	 * �s���ZYX�̏��Ԃŉ�]���|����
	 * @param x�@���͊p�x���W�A��
	 * @param y�@���͊p�x���W�A��
	 * @param z�@���͊p�x���W�A��
	 */
	inline void RotateZYX( const f32 x, const f32 y, const f32 z ) {
		RotateZ( z );
		RotateY( y );
		RotateX( x );
	}

	/**
	 * �s���ZYX�̏��Ԃŉ�]���|����
	 * @param v�@���̓x�N�g���\����
	 */
	inline void RotateZYX( const VectorNative& v ) {
		RotateZYX( v.GFL_VECTOR_X, v.GFL_VECTOR_Y, v.GFL_VECTOR_Z );
	}

	/**
	 * �s���ZYX�̏��Ԃŉ�]���|����
	 * @param v�@x,y,z �̏��Ԃœ��������������_�z��|�C���^
	 */
	inline void RotateZYX( const f32 v[] ) {
		RotateZYX( v[TX], v[TY], v[TZ] );
	}

	/**
	 * �X�P�[����ݒ肷��
	 * @param scl�@�X�P�[���l
	 * @remark ���̊p�x�����̓N���A�����B���W�����͕ω��Ȃ�
	 */
	inline void SetScale( const f32 scl ) {
		SetScale( scl, scl, scl );
	}

	/**
	 * �X�P�[����ݒ肷��
	 * @param t�@�X�P�[���l�x�N�g��
	 * @remark ���̊p�x�����̓N���A�����B���W�����͕ω��Ȃ�
	 */
	inline void SetScale( const VectorNative& t ) {
		SetScale( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
	}

	/**
	 * �X�P�[���l���|����
	 * @param scl�@�X�P�[���l
	 * @remark �����������S�Ăɓ������X�P�[���l���|����
	 */
	inline void Scale( const f32 scl ) {
		ScaleX( scl );
		ScaleY( scl );
		ScaleZ( scl );
	}

	/**
	 * �X�P�[���l���|����
	 * @param x�@�������X�P�[��
	 * @param y�@�������X�P�[��
	 * @param z�@�������X�P�[��
	 */
	inline void Scale( const f32 x, const f32 y, const f32 z ) {
		ScaleX( x );
		ScaleY( y );
		ScaleZ( z );
	}

	/**
	 * �X�P�[���l���|����
	 * @param t�@���̓x�N�g���\����
	 */
	inline void Scale( const VectorNative& t ) {
		Scale( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
	}

	/**
	 * �X�P�[�����̎擾
	 * @param t �X�P�[���i�[�N���X�|�C���^
	 */

	inline void GetScale( VectorNative* t ) const {
		t->Set( GetScaleX( ), GetScaleY( ), GetScaleZ( ) );
	}

	/**
	 * �X�P�[���l���t����|����
	 * @param scl�@�X�P�[���l
	 * @remark x,y,z�����S�Ăɓ����� scl ���t����|����
	 */
	inline void Scale_Reverse( const f32 scl ) {
		ScaleX_Reverse( scl );
		ScaleY_Reverse( scl );
		ScaleZ_Reverse( scl );
	}

	/**
	 * �X�P�[���l���t����|����
	 * @param x�@���X�P�[���l
	 * @param y�@���X�P�[���l
	 * @param z�@���X�P�[���l
	 */
	inline void Scale_Reverse( const f32 x, const f32 y, const f32 z ) {
		ScaleX_Reverse( x );
		ScaleY_Reverse( y );
		ScaleZ_Reverse( z );
	}

	/**
	 * �X�P�[���l���t����|����
	 * @param t�@���̓x�N�g���\����
	 */
	inline void Scale_Reverse( const VectorNative& t ) {
		Scale_Reverse( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
	}

	/**
	 * �X�P�[���l�̋t�����|����
	 * @param scl�@�X�P�[���l
	 * @remark �����������S�Ăɓ������X�P�[���l���|����
	 */
	inline void ScaleRecip( const f32 scl ) {
		const f32 r_scl = gfl::core::Math::FRecip( scl );
		ScaleX( r_scl );
		ScaleY( r_scl );
		ScaleZ( r_scl );
	}

	/**
	 * �X�P�[���l�̋t�����|����
	 * @param t�@���̓x�N�g���\����
	 */
	inline void ScaleRecip( const VectorNative& t ) {
		Scale( gfl::core::Math::FRecip( t.GFL_VECTOR_X ), gfl::core::Math::FRecip( t.GFL_VECTOR_Y ), gfl::core::Math::FRecip( t.GFL_VECTOR_Z ) );
	}

	/**
	 *   ���K��
	 * @param src ���͍s��
	 * @param  scl �X�P�[���l�x�N�g��
	 * @return �Ȃ�
	 */
	void Normalize( const self_type& src, VectorNative* scl = 0 ) {
		Set( src );
		Normalize( scl );
	}

	/**
	 *   ���K��
	 * @param src ���͍s��
	 * @param  scl �X�P�[���l�x�N�g��
	 * @return ���K�����ꂽ�s��C���X�^���X
	 */
	//self_type GetNormalizedMatrix( VectorNative* scl = 0 ) {
	MatrixSoa44 GetNormalizedMatrix( VectorNative* scl = 0 ) {
		self_type mat( *this );
		mat.Normalize( scl );
		return mat;
	}

	/**
	 *   ���W�������擾
	 * @param  v ���̓x�N�g��
	 * @return ���W
	 */

	inline void GetTrans( VectorNative* v ) const {
		v->Set( GetTrans( ) );
	}

	/**
	 *   �p�x���s�񂩂�擾
	 * @param  v �o�̓��W�A���̃x�N�g��
	 * @return ����
	 * @remark w �͕s��B�y�x�w�̏��ŉ񂵂��ꍇ�̊p�x
	 */

	void GetRadian( VectorNative* v ) const {
		GetRadian( &v->GFL_VECTOR_X, &v->GFL_VECTOR_Y, &v->GFL_VECTOR_Z );
	}

	/**
	 *   �p�x���s�񂩂�擾
	 * @param  v �o�̓��W�A���̃x�N�g��
	 * @return ����
	 * @remark w �͕s��B�x�y�w�̏��ŉ񂵂��ꍇ�̊p�x
	 */
	void GetRadianYXZ( VectorNative* v ) const {
		GetRadianYXZ( &v->GFL_VECTOR_X, &v->GFL_VECTOR_Y, &v->GFL_VECTOR_Z );
	}

	/**
	 * �N�����v����@�K�v�Ȃ����c
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
	 * ���W�l�̉��Z
	 * @param v�@���̓x�N�g��
	 * @remark �P���ɍ��W�����Z����̂ŁA��]�����͉e�����Ȃ��B\n
	 * ��]���l�������ړ��̏ꍇ�� Trans ���g�p���Ă��������B
	 */
	inline void operator+=(const VectorStruct3& v) {
		GetTransRefX( ) += v.GFL_VECTOR_X;
		GetTransRefY( ) += v.GFL_VECTOR_Y;
		GetTransRefZ( ) += v.GFL_VECTOR_Z;
	}

	/**
	 * �s��̍����擾
	 * @param m�@��ƂȂ�s��
	 * @return ���͍s����ǂꂾ����]�������玩���̍s��ɂȂ邩�̍s��
	 * @remark �P���Ȉ����Z�łȂ��Athis = m * (�Ԃ�l)�ƂȂ�s����v�Z����
	 */
	inline self_type operator-(const self_type& m) const {
		self_type mat( GetTranspose33( ) );
		mat.Mul33( m );
		return mat;
	}

	/**
	 * �s��̐�
	 * @param m0�@���͍s��
	 * @return �Q��
	 * @remark this = this * m0
	 */
	inline self_type operator*(const self_type& m0) const {
		self_type mat;
		mat.Mul( *this, m0 );
		return mat;
	}

	/**
	 * �s��̐�
	 * @param m0�@���͍s��
	 * @remark this = this * m0
	 */
	inline void Mul( const parent_type& m0 ) {
		Mul( *this, (const self_type&) m0 );
	}

	/**
	 * �������������ɃX�P�[����������
	 * @param sclx�@���X�P�[���l
	 * @param scly�@���X�P�[���l
	 * @param sclz�@���X�P�[���l
	 */
	void Mul( const f32 sclx, const f32 scly, const f32 sclz ) {
		ScaleX( sclx );
		ScaleY( scly );
		ScaleZ( sclz );
	}

	/**
	 * �������������ɃX�P�[����������
	 * @param scl�@�X�P�[���x�N�g��
	 */
	void Mul( const VectorNative& scl ) {
		ScaleX( scl.GFL_VECTOR_X );
		ScaleY( scl.GFL_VECTOR_Y );
		ScaleZ( scl.GFL_VECTOR_Z );
	}

	/**
	 * �p�x�����݂̂Ɋ|���Z������
	 * @param m0�@���͍s��
	 */
	inline void Mul33( const self_type& m0 ) {
		Mul33( *this, m0 );
	}

	/**
	 * �s��⊮
	 * @param m0�@�^�[�Q�b�g�s��
	 * @param rate�@�����@�O�@�Ŏ������g�@�P�Ń^�[�Q�b�g�s��
	 * @return �Q��
	 * @remark this = this * (1-rate) + m0 * rate
	 */
	inline self_type& Slerp( const self_type& m0, const f32 rate ) {
		return Slerp( *this, m0, rate );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s = \n", str );
		PrintConsole( );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] = \n", str, i );
		PrintConsole( );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] = \n", str, i, j );
		PrintConsole( );
	}

	/* �S���ʂɂ��Ă���������SOA34 SOA44 �ŃC���X�^���X���]�v�ɂł����Ⴄ�� */

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  ����
	 * @return ����
	 */
	void PrintConsole33( void ) const {
		Debug::PrintConsole( "%ff, %ff, %ff,\n%ff, %ff, %ff,\n%ff, %ff, %ff\n",
							 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02,
							 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12,
							 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22 );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole33( const c8 * const str ) const {
		Debug::PrintConsole( "%s = \n", str );
		PrintConsole33( );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole33( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] = \n", str, i );
		PrintConsole33( );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole33( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] = \n", str, i, j );
		PrintConsole33( );
	}

	/**
	 *   �w�肳�ꂽ����擾
	 * @param  i �Y����
	 * @return �w�肳�ꂽ��
	 */
	inline Vector4& GetRow( const s32 i ) {
		return reinterpret_cast<Vector4&> (parent_type::GetRow( i ));
	}

	/**
	 *   �w�肳�ꂽ����擾
	 * @param  i �Y����
	 * @return �w�肳�ꂽ��
	 */
	inline const Vector4& GetRow( const s32 i ) const {
		return reinterpret_cast<const Vector4&> (parent_type::GetRow( i ));
	}

	/**
	 * �w�肳�ꂽ�s���擾
	 * @param i�@�Y��
	 * @return �s��ݒ肵��Vector3�̎���
	 */
	inline Vector3 GetColumn( const s32 i ) const {
		return Vector3( parent_type::GetColumn( i ) );
	}

	/**
	 *   �w���W�������擾
	 * @param  ����
	 * @return �w���W
	 */
	inline f32 GetTransX( void ) const {
		return Get( TX, 3 );
	}

	/**
	 *   �x���W�������擾
	 * @param  ����
	 * @return �x���W
	 */
	inline f32 GetTransY( void ) const {
		return Get( TY, 3 );
	}

	/**
	 *   �y���W�������擾
	 * @param  ����
	 * @return �y���W
	 */
	inline f32 GetTransZ( void ) const {
		return Get( TZ, 3 );
	}

	/**
	 *   �w���W�����Q�Ƃ��擾
	 * @param  ����
	 * @return �w���W
	 */
	inline f32& GetTransRefX( void ) {
		return GetRef( TX, 3 );
	}

	/**
	 *   �x���W�����Q�Ƃ��擾
	 * @param  ����
	 * @return �x���W
	 */
	inline f32& GetTransRefY( void ) {
		return GetRef( TY, 3 );
	}

	/**
	 *   �y���W�����Q�Ƃ��擾
	 * @param  ����
	 * @return �y���W
	 */
	inline f32& GetTransRefZ( void ) {
		return GetRef( TZ, 3 );
	}

	/**
	 *   ���W�������擾
	 * @param  ����
	 * @return ���W
	 */
	inline Vector3 GetTrans( void ) const {
		return GetColumn( 3 );
	}

	/**
	 *   �v�f��ݒ�
	 * @param  mat ���͂R���R�s��
	 * @return ����
	 */
	void Set( const MatrixSoaNative33& mat ) {
		Set33( mat );
	}

	/**
	 * �v�f��ݒ�
	 * @param m0 ���͂Rx�S�s��
	 */
	void Set( const MatrixSoaNative34& m0 ) {
		SetRow( 0, m0.GetRow( 0 ) );
		SetRow( 1, m0.GetRow( 1 ) );
		SetRow( 2, m0.GetRow( 2 ) );
	}

	/**
	 * ���ݒ�
	 * @param i�@�Y��
	 * @param m0�@���̓x�N�g���\���̂S�v�f
	 */
	inline void SetRow( const s32 i, const VectorStruct4& m0 ) {
		GFL_MTX_VECTOR[i] = m0;
	}

	/**
	 * ���ݒ�
	 * @param i�@�Y��
	 * @param m0�@���̓x�N�g���\���̂R�v�f
	 */
	inline void SetRow( const s32 i, const VectorStruct3& m0 ) {
		GFL_MTX_VECTOR[i].GFL_VECTOR_X = m0.GFL_VECTOR_X;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Y = m0.GFL_VECTOR_Y;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Z = m0.GFL_VECTOR_Z;
	}

	/**
	 * ���ݒ�
	 * @param i�@�Y��
	 * @param m0�@���̓x�N�g���\���̂R�v�f
	 * @param w ���́@���W����
	 */
	inline void SetRow( const s32 i, const VectorStruct3& m0, const f32 w ) {
		GFL_MTX_VECTOR[i].GFL_VECTOR_X = m0.GFL_VECTOR_X;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Y = m0.GFL_VECTOR_Y;
		GFL_MTX_VECTOR[i].GFL_VECTOR_Z = m0.GFL_VECTOR_Z;
		GFL_MTX_VECTOR[i].GFL_VECTOR_W = w;
	}

	/**
	 * �s��ݒ�
	 * @param i�@�Y��
	 * @param m0�@���̓x�N�g���\���̂R�v�f
	 */
	inline void SetColumn( const s32 i, const VectorStruct3& m0 ) {
		GFL_MTX_ARRAY2D[0][i] = m0.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][i] = m0.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][i] = m0.GFL_VECTOR_Z;
	}

	/**
	 * �����W��ݒ�
	 * @param x�@�����W
	 */
	inline void SetTransX( const f32 x ) {
		GFL_MTX_ARRAY2D[TX][3] = x;
	}

	/**
	 * �����W��ݒ�
	 * @param y�@�����W
	 */
	inline void SetTransY( const f32 y ) {
		GFL_MTX_ARRAY2D[TY][3] = y;
	}

	/**
	 * �����W��ݒ�
	 * @param z�@�����W
	 */
	inline void SetTransZ( const f32 z ) {
		GFL_MTX_ARRAY2D[TZ][3] = z;
	}

	/**
	 * ���W��ݒ�
	 * @param v�@�x�N�g���\���̂R�v�f
	 */
	inline void SetTrans( const VectorNative3& v ) {
		SetColumn( 3, v );
	}

	/**
	 * ���W��ݒ�
	 * @param v�@�x�N�g���\���̂S�v�f
	 */
	inline void SetTrans( const VectorNative4& v ) {
		SetColumn( 3, v );
	}

	/**
	 * �������������ɃX�P�[����������
	 * @param scl�@�X�P�[���l
	 */
	void Mul( const f32 scl ) {
		*this *= scl;
	}

	/**
	 * �X�P�[���l���t����|����
	 * @param x�@���X�P�[���l
	 */
	inline void ScaleX_Reverse( const f32 x ) {
		GetRef( 0, 0 ) *= x;
		GetRef( 0, 1 ) *= x;
		GetRef( 0, 2 ) *= x;
	}

	/**
	 * �X�P�[���l���t����|����
	 * @param y�@���X�P�[���l
	 */
	inline void ScaleY_Reverse( const f32 y ) {
		GetRef( 1, 0 ) *= y;
		GetRef( 1, 1 ) *= y;
		GetRef( 1, 2 ) *= y;
	}

	/**
	 * �X�P�[���l���t����|����
	 * @param z�@���X�P�[���l
	 */
	inline void ScaleZ_Reverse( const f32 z ) {
		GetRef( 2, 0 ) *= z;
		GetRef( 2, 1 ) *= z;
		GetRef( 2, 2 ) *= z;
	}

	/**
	 * �������x�N�g�����p�x�����Ɋ|�������̂��v�Z
	 * @param v0�@�o�̓x�N�g���N���X
	 * @param val�@���l
	 */
	inline void ApplyX( VectorNative* v0, const f32 val ) const {
		v0->Set( Get( 0, 0 ) * val, Get( 1, 0 ) * val, Get( 2, 0 ) * val );
	}

	/**
	 * �������x�N�g�����p�x�����ɒ����P���|�������̂��v�Z
	 * @param v0�@�o�̓x�N�g���N���X
	 */
	inline void ApplyX1( VectorNative* v0 ) const {
		v0->Set( Get( 0, 0 ), Get( 1, 0 ), Get( 2, 0 ) );
	}

	/**
	 * �������x�N�g�����p�x�����Ɋ|�������̂��v�Z
	 * @param v0�@�o�̓x�N�g���N���X
	 * @param val�@���l
	 */
	inline void ApplyY( VectorNative* v0, const f32 val ) const {
		v0->Set( Get( 0, 1 ) * val, Get( 1, 1 ) * val, Get( 2, 1 ) * val );
	}

	/**
	 * �������x�N�g�����p�x�����ɒ����P���|�������̂��v�Z
	 * @param v0�@�o�̓x�N�g���N���X
	 */
	inline void ApplyY1( VectorNative* v0 ) const {
		v0->Set( Get( 0, 1 ), Get( 1, 1 ), Get( 2, 1 ) );
	}

	/**
	 * �������x�N�g�����p�x�����Ɋ|�������̂��v�Z
	 * @param v0�@�o�̓x�N�g���N���X
	 * @param val�@���l
	 */
	inline void ApplyZ( VectorNative* v0, const f32 val ) const {
		v0->Set( Get( 0, 2 ) * val, Get( 1, 2 ) * val, Get( 2, 2 ) * val );
	}

	/**
	 * �������x�N�g�����p�x�����ɒ����P���|�������̂��v�Z
	 * @param v0�@�o�̓x�N�g���N���X
	 */
	inline void ApplyZ1( VectorNative* v0 ) const {
		v0->Set( Get( 0, 2 ), Get( 1, 2 ), Get( 2, 2 ) );
	}

	/**
	 * ���W�ړ�
	 * @param val�@������
	 */
	inline void TransX( const f32 val ) {
		GetTransRefX( ) += Get( 0, 0 ) * val;
		GetTransRefY( ) += Get( 1, 0 ) * val;
		GetTransRefZ( ) += Get( 2, 0 ) * val;
	}

	/**
	 * ���W�ړ�
	 * @param val�@������
	 */
	inline void TransY( const f32 val ) {
		GetTransRefX( ) += Get( 0, 1 ) * val;
		GetTransRefY( ) += Get( 1, 1 ) * val;
		GetTransRefZ( ) += Get( 2, 1 ) * val;
	}

	/**
	 * ���W�ړ�
	 * @param val�@������
	 */
	inline void TransZ( const f32 val ) {
		GetTransRefX( ) += Get( 0, 2 ) * val;
		GetTransRefY( ) += Get( 1, 2 ) * val;
		GetTransRefZ( ) += Get( 2, 2 ) * val;
	}

	/**
	 * ���W�ړ�
	 * @param v0�@�o�̓x�N�g���N���X�|�C���^
	 * @param val�@������
	 * @remark �s��Ɍv�Z���ʂ͔��f����Ȃ�
	 */
	inline void TransX( VectorNative* v0, const f32 val ) const {
		v0->Set( Get( 0, 0 ) * val + GetTransX( ),
				 Get( 1, 0 ) * val + GetTransY( ),
				 Get( 2, 0 ) * val + GetTransZ( ) );
	}

	/**
	 * ���W�ړ�
	 * @param v0�@�o�̓x�N�g���N���X�|�C���^
	 * @param val�@������
	 * @remark �s��Ɍv�Z���ʂ͔��f����Ȃ�
	 */
	inline void TransY( VectorNative* v0, const f32 val ) const {
		v0->Set( Get( 0, 1 ) * val + GetTransX( ),
				 Get( 1, 1 ) * val + GetTransY( ),
				 Get( 2, 1 ) * val + GetTransZ( ) );
	}

	/**
	 * ���W�ړ�
	 * @param v0�@�o�̓x�N�g���N���X�|�C���^
	 * @param val�@������
	 * @remark �s��Ɍv�Z���ʂ͔��f����Ȃ�
	 */
	inline void TransZ( VectorNative* v0, const f32 val ) const {
		v0->Set( Get( 0, 2 ) * val + GetTransX( ),
				 Get( 1, 2 ) * val + GetTransY( ),
				 Get( 2, 2 ) * val + GetTransZ( ) );
	}

	/**
	 * �O���[�o�����W�����[�J�����W�ɕϊ����������W���擾
	 * @param glo�@�O���[�o�����W�@���̓x�N�g��
	 * @return ���[�J�������W
	 */
	inline f32 GlobalToLocalX( const VectorNative& glo ) const {
		Vector pos;
		pos.Sub( glo, GetTrans( ) );
		return Get( 0, 0 ) * pos.GetX( ) + Get( 1, 0 ) * pos.GetY( ) + Get( 2, 0 ) * pos.GetZ( );
	}

	/**
	 * �O���[�o�����W�����[�J�����W�ɕϊ����������W���擾
	 * @param glo�@�O���[�o�����W�@���̓x�N�g��
	 * @return ���[�J�������W
	 */
	inline f32 GlobalToLocalY( const VectorNative& glo ) const {
		Vector pos;
		pos.Sub( glo, GetTrans( ) );
		return Get( 0, 1 ) * pos.GetX( ) + Get( 1, 1 ) * pos.GetY( ) + Get( 2, 1 ) * pos.GetZ( );
	}

	/**
	 * �O���[�o�����W�����[�J�����W�ɕϊ����������W���擾
	 * @param glo�@�O���[�o�����W�@���̓x�N�g��
	 * @return ���[�J�������W
	 */
	inline f32 GlobalToLocalZ( const VectorNative& glo ) const {
		Vector pos;
		pos.Sub( glo, GetTrans( ) );
		return Get( 0, 2 ) * pos.GetX( ) + Get( 1, 2 ) * pos.GetY( ) + Get( 2, 2 ) * pos.GetZ( );
	}

	/**
	 * �x�N�g���ƍs��̌����̑O�������������
	 * @param x�@�x�N�g��������
	 * @param z�@�x�N�g��������
	 * @return �^�@���������@�U�@���Ε���
	 */
	inline bool IsVectorSameDirection( const f32 x, const f32 z ) const {
		return ( 0.0f <= Get( 0, 2 ) * x + Get( 2, 2 ) * z);
	}

	/**
	 * �x�N�g���ƍs��̌����̍��E������������
	 * @param x�@�x�N�g��������
	 * @param z�@�x�N�g��������
	 * @return �^�@���������@�U�@���Ε���
	 */
	inline bool IsVectorSameSide( const f32 x, const f32 z ) const {
		return ( 0.0f <= Get( 0, 0 ) * x + Get( 2, 0 ) * z);
	}

	/**
	 * �x�N�g�����s��Ɠ��������X�O�x�ȓ���
	 * @param vx�@�x�N�g��������
	 * @param vz�@�x�N�g��������
	 * @return �^�@���������X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsVectorSameDirectionDegree90( const f32 vx, const f32 vz ) const {
		const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
		const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
		return gfl::core::Math::IsFrontDegree90( x, z );
	}

	/**
	 * �x�N�g�����s��Ɣ��Ε����̂X�O�x�ȓ���
	 * @param vx�@�x�N�g��������
	 * @param vz�@�x�N�g��������
	 * @return �^�@���Ε����X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsVectorOppositeDirectionDegree90( const f32 vx, const f32 vz ) const {
		const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
		const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
		return gfl::core::Math::IsBackDegree90( x, z );
	}

	/**
	 * �x�N�g�����s��̍������X�O�x�ȓ���
	 * @param vx�@�x�N�g��������
	 * @param vz�@�x�N�g��������
	 * @return �^�@�����X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsVectorLeftDirectionDegree90( const f32 vx, const f32 vz ) const {
		const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
		const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
		return gfl::core::Math::IsLeftDegree90( x, z );
	}

	/**
	 * �x�N�g�����s��̉E�����X�O�x�ȓ���
	 * @param vx�@�x�N�g��������
	 * @param vz�@�x�N�g��������
	 * @return �^�@�E���X�O�x�ȓ��@�U�@����ȊO
	 */
	inline bool IsVectorRightDirectionDegree90( const f32 vx, const f32 vz ) const {
		const f32 x = Get( 0, 0 ) * vx + Get( 2, 0 ) * vz;
		const f32 z = Get( 0, 2 ) * vx + Get( 2, 2 ) * vz;
		return gfl::core::Math::IsRightDegree90( x, z );
	}

	/**
	 * �s�����
	 * @param m�@���͍s��
	 * @return �Q��
	 */
	inline self_type& operator=(const MatrixSoaNative34& m) {
		Set( m );
		return *this;
	}

	/**
	 * �s�����
	 * @param m�@���͍s��
	 * @return �Q��
	 */
	inline self_type& operator=(const MatrixSoaNative44& m) {
		Set( m );
		return *this;
	}

#if GFL_USE_AOS

	/**
	 *   ���
	 * @param  m ���͍s��
	 * @return ����
	 */
	inline self_type& operator=(const MatrixAos43& m) {
		FromAOS( m );
		return *this;
	}

	/**
	 *   ���
	 * @param  m ���͍s��
	 * @return ����
	 */
	inline self_type& operator=(const MatrixAos44& m) {
		FromAOS( m );
		return *this;
	}
#endif

	using parent_type::operator+=;

	/**
	 * �P�ʍs���ݒ�
	 */
	void SetUnit( void ) {
		parent_type::SetupIdentity( );
	}

	/**
	 * ���������ɃX�P�[����������
	 * @param scl�@�X�P�[���l
	 */
	void ScaleX( const f32 scl ) {
		GFL_MTX_00 *= scl;
		GFL_MTX_10 *= scl;
		GFL_MTX_20 *= scl;
	}

	/**
	 * ���������ɃX�P�[����������
	 * @param scl�@�X�P�[���l
	 */
	void ScaleY( const f32 scl ) {
		GFL_MTX_01 *= scl;
		GFL_MTX_11 *= scl;
		GFL_MTX_21 *= scl;
	}

	/**
	 * ���������ɃX�P�[����������
	 * @param scl�@�X�P�[���l
	 */
	void ScaleZ( const f32 scl ) {
		GFL_MTX_02 *= scl;
		GFL_MTX_12 *= scl;
		GFL_MTX_22 *= scl;
	}

	/**
	 * �s�񂩂�X�P�[���l���擾
	 * @return ���X�P�[���l
	 */
	inline f32 GetScaleX( void ) const {
		return GetColumn( 0 ).Magnitude( );
	}

	/**
	 * �s�񂩂�X�P�[���l���擾
	 * @return ���X�P�[���l
	 */
	inline f32 GetScaleY( void ) const {
		return GetColumn( 1 ).Magnitude( );
	}

	/**
	 * �s�񂩂�X�P�[���l���擾
	 * @return ���X�P�[���l
	 */
	inline f32 GetScaleZ( void ) const {
		return GetColumn( 2 ).Magnitude( );
	}

	/**
	 * �s�񂩂�X�P�[���l���擾
	 * @param �X�P�[���x�N�g���|�C���^
	 */
	inline void GetScale( VectorNative* v ) {
		v->Set( GetScaleX( ), GetScaleY( ), GetScaleZ( ) );
	}

	/**
	 * �s�񂩂�X�P�[���l���擾
	 * @param �X�P�[���x�N�g���|�C���^
	 */
	inline Vector GetScale( void ) {
		Vector v;
		v.Set( GetScaleX( ), GetScaleY( ), GetScaleZ( ) );
		return v;
	}

	/**
	 * �p�x�����R���R�ɒP�ʍs���ݒ�
	 */
	inline void SetUnit33( void ) {
		SetRow( 0, Vector3::GetX1( ) );
		SetRow( 1, Vector3::GetY1( ) );
		SetRow( 2, Vector3::GetZ1( ) );
	}
#endif

	// -------------------------------------------------------------------------
	// LookAtMatrix����
	// -------------------------------------------------------------------------
	static MatrixSoa44 GetLookAt( const Vector& eye, const Vector& aim, const Vector& up );
	static MatrixSoa44 GetLookAtGL( const Vector& eye, const Vector& aim, const Vector& up );

	// -------------------------------------------------------------------------
	// Perspective����
	// -------------------------------------------------------------------------
	static MatrixSoa44 GetPerspective( float fovy, float aspect, float zNear, float zFar );
	static MatrixSoa44 GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar );

	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	/**
	 *   �f�t�H���g�R���X�^���N�^�[
	 * @param  ����
	 * @return ����
	 * @remark �������Ȃ�
	 */

	MatrixSoa44( void ) {
	}

	/**
	 *   Matrix44SOA ��ݒ肷��R���X�^���N�^�[
	 * @param  m Matrix44SOA�N���X
	 * @return ����
	 */

	MatrixSoa44( const MatrixSoaNative33& m0 ) {
		Set33( m0 );
		SetRow( 3, Vector4::GetW1( ) );
	}

	/**
	 *   Matrix44SOA ��ݒ肷��R���X�^���N�^�[
	 * @param  m Matrix44SOA�N���X
	 * @return ����
	 */

	MatrixSoa44( const MatrixSoaNative34& m0 ) {
		Set( m0 );
		SetRow( 3, Vector4::GetW1( ) );
	}

	/**
	 *   Matrix44SOA ��ݒ肷��R���X�^���N�^�[
	 * @param  m Matrix44SOA�N���X
	 * @return ����
	 */

	MatrixSoa44( const MatrixSoaNative44& m0 ) {
		Set( m0 );
	}

	// -------------------------------------------------------------------------
	// Destruct !! not virtual for alignment
	// -------------------------------------------------------------------------

	~MatrixSoa44( ) {
	}

	inline f32& GetTransRefW( void ) {
		return GetRef( TW, 3 );
	}

	/**
	 *   �w�肳�ꂽ�s���擾
	 * @param  i �Y���� 0 �` 3
	 * @return �w�肳�ꂽ��
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline Vector4 GetColumn4( const s32 i ) const {
		return Vector4( GFL_MTX_ARRAY2D[0][i], GFL_MTX_ARRAY2D[1][i], GFL_MTX_ARRAY2D[2][i], GFL_MTX_ARRAY2D[3][i] );
	}

	/**
	 *   ���W�������i�O�C�O�C�O�C�P�j�ɐݒ�
	 * @param  ����
	 * @return ����
	 */

	/**
	 *   ���W�������擾
	 * @param  ����
	 * @return ���W
	 */

	inline Vector4 GetTrans4( void ) const {
		return GetColumn4( 3 );
	}

	/**
	 *   ���W�������擾
	 * @param  v ���̓x�N�g��
	 * @return ���W
	 */

	inline void GetTrans4( Vector4* v ) const {
		v->Set( GetTrans4( ) );
	}

	/**
	 *   �]�u�s���ݒ�
	 * @param  m0 ���͍s��̎Q��
	 * @return ����
	 */
	void Transpose( const MatrixSoa44& m0 );

	/**
	 *   �]�u�s���ݒ�
	 * @param  ����
	 * @return ����
	 */
	void Transpose( void );

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *  �s���ݒ�
	 * @param  m0  �v�f @param  m1  �v�f @param  m2  �v�f @param  m3  �v�f @param  m4 �v�f
	 * @param  m5  �v�f @param  m6  �v�f @param  m7  �v�f @param  m8  �v�f @param  m9 �v�f
	 * @param  m10 �v�f @param  m11 
	 * @return ����
	 */
	void Set( f32 m0, f32 m1, f32 m2, f32 m3,
			 f32 m4, f32 m5, f32 m6, f32 m7,
			 f32 m8, f32 m9, f32 m10, f32 m11,
			 f32 m12, f32 m13, f32 m14, f32 m15 );

	/**
	 *   �v�f��ݒ�
	 * @param  i �v�f
	 * @param  m �l
	 * @return ����
	 */

	inline void SetColumn( const s32 i, const VectorStruct4& m0 ) {
		GFL_MTX_ARRAY2D[0][i] = m0.GFL_VECTOR_X;
		GFL_MTX_ARRAY2D[1][i] = m0.GFL_VECTOR_Y;
		GFL_MTX_ARRAY2D[2][i] = m0.GFL_VECTOR_Z;
		GFL_MTX_ARRAY2D[3][i] = m0.GFL_VECTOR_W;
	}

	/**
	 *   �v������ݒ�
	 * @param  i �v�f�ԍ� SOA �̏ꍇ GFL_MTX_ARRAY2D[i][3]
	 * @param  val �Z�b�g����l�@�f�t�H���g�͂O
	 * @return ����
	 */

	inline void SetW( const s32 i, const f32 val = 0.0f ) {
		GFL_MTX_ARRAY2D[3][i] = val;
	}

	/**
	 *   ���W������ݒ�
	 * @param  w �p�b�h����
	 * @return ����
	 */

	inline void SetTransW( const f32 w ) {
		SetW( 3, w );
	}

	/**
	 *   ���W������ݒ�
	 * @param  x �w���W
	 * @param  y �x���W
	 * @param  z �y���W
	 * @return ����
	 */

	inline void SetTrans( const f32 x, const f32 y, const f32 z ) {
		SetTransX( x );
		SetTransY( y );
		SetTransZ( z );
		SetTransW( 1.0f );
	}

	/**
	 * ���W�������N���A
	 */
	inline void SetTransUnit( void ) {
		SetColumn( 3, Vector4::GetZero( ) );
	}
#if 0 // 

	/**
	 * ���K��
	 */
	void Normalize( void ) {
		Vector3 scl;
		GetScale( &scl );
		scl.Recip( );
		Scale( scl );
	}
#endif

	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------
	/**
	 *   ��Z
	 * @param  m0 ���͍s��
	 * @param  m1 ���͍s��
	 * @return ����
	 */
	void Mul( const MatrixSoa44& m0, const MatrixSoa44& m1 );

	// -------------------------------------------------------------------------
	// Determinant
	// -------------------------------------------------------------------------

	/**
	 *   ������q
	 * @param  ����
	 * @return ������q
	 */
	f32 Determinant( void ) const;

	// -------------------------------------------------------------------------
	// Inverse
	// -------------------------------------------------------------------------
	/**
	 *   �t�s��
	 * @param  m0 ���͍s��
	 * @return ����
	 * @remark �d��
	 */
	void Inverse( const MatrixSoa44& m0 );

	inline void operator+=(const VectorStruct4& v) {
		GetTransRefX( ) += v.GFL_VECTOR_X;
		GetTransRefY( ) += v.GFL_VECTOR_Y;
		GetTransRefZ( ) += v.GFL_VECTOR_Z;
		GetTransRefW( ) += v.GFL_VECTOR_W;
	}
#if GFL_USE_AOS
	void FromAOS( const gfl::core::MatrixAos43& src );
	void FromAOS( const gfl::core::MatrixAos44& src );
	void ToAOS( gfl::core::MatrixAos43* mat ) const;
	void ToAOS( gfl::core::MatrixAos44* mat ) const;
	MatrixAos44 ToAOS( void ) const;
#endif

protected:
	void PrintConsoleW( void ) const;
	// =========================================================================
	// �������牺�� MatrixSoa34 �̖��߂��Ă�
	// =========================================================================
public:

	/**
	 *   Matrix44Soa ��ݒ�
	 * @param  m �ݒ肵���� Matrix44Soa �̎Q��
	 * @return ����
	 */
	void Set( const MatrixSoaNative44& mat ) {
		CAST_34_PTR( this )->Set( reinterpret_cast<const MatrixSoaNative34&> (mat) );
		SetRow( 3, mat.GetRow( 3 ) );
	}

	void Set( f32 m0, f32 m1, f32 m2, f32 m3,
			 f32 m4, f32 m5, f32 m6, f32 m7,
			 f32 m8, f32 m9, f32 m10, f32 m11 ) {
		CAST_34_PTR( this )->Set( m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11 );
	}

	// -------------------------------------------------------------------------
	// Rotate X
	// -------------------------------------------------------------------------

	/**
	 *   �s��̂w���ŉ�]�������p�x������ݒ�
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 * @remark ���W�����ɂ͉������Ȃ�
	 */
	void SetRotateX( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->SetRotateX( s, c );
	}

	/**
	 *   �s�� m0 �ɂw���ŉ�]�������p�x������������
	 * @param  m0 ���͍s��
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateX( const self_type& m0, const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateX( m0, s, c );
	}

	/**
	 *   �����ɂw���ŉ�]�������p�x������������
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateX( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateX( s, c );
	}

	/**
	 *   �����ɂw���� 1/2PI ��]�������p�x������������
	 * @return ����
	 * @remark SinCos �Ɛς��ȗ����Ă���̂ō���
	 */
	void RotateX_HalfPi( void ) {
		CAST_34_PTR( this )->RotateX_HalfPi( );
	}

	/**
	 *   �����ɂw���� -1/2PI ��]�������p�x������������
	 * @return ����
	 */
	void RotateX_MinusHalfPi( void ) {
		CAST_34_PTR( this )->RotateX_MinusHalfPi( );
	}

	/**
	 *   �����ɂw���� PI ��]�������p�x������������
	 * @return ����
	 */
	void RotateX_OnePi( void ) {
		CAST_34_PTR( this )->RotateX_OnePi( );
	}

	/**
	 *   �����ɂw���ŉ�]�������p�x�������t�����ɂ�����
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateX_Reverse( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateX_Reverse( s, c );
	}
	// -------------------------------------------------------------------------
	// Rotate Y
	// -------------------------------------------------------------------------

	/**
	 *   �s��̂x���ŉ�]�������p�x������ݒ�
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 * @remark ���W�����ɂ͉������Ȃ�
	 */
	void SetRotateY( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->SetRotateY( s, c );
	}

	/**
	 *   �s�� m0 �ɂx���ŉ�]�������p�x������������
	 * @param  m0 ���͍s��
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateY( const self_type& m0, const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateY( m0, s, c );
	}

	/**
	 *   �����ɂx���ŉ�]�������p�x������������
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateY( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateY( s, c );
	}

	/**
	 *   �����ɂ����� 1/2PI ��]�������p�x������������
	 * @return ����
	 * @remark SinCos �Ɛς��ȗ����Ă���̂ō���
	 */
	void RotateY_HalfPi( void ) {
		CAST_34_PTR( this )->RotateY_HalfPi( );
	}

	/**
	 *   �����ɂ����� -1/2PI ��]�������p�x������������
	 * @return ����
	 */
	void RotateY_MinusHalfPi( void ) {
		CAST_34_PTR( this )->RotateY_MinusHalfPi( );
	}

	/**
	 *   �����ɂ����� PI ��]�������p�x������������
	 * @return ����
	 */
	void RotateY_OnePi( void ) {
		CAST_34_PTR( this )->RotateY_OnePi( );
	}

	/**
	 *   �����ɂx���ŉ�]�������p�x�������t�����ɂ�����
	 * @param  m0 ���͍s��.
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateY_Reverse( const self_type& m0, const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateY_Reverse( m0, s, c );
	}

	/**
	 *   �����ɂx���ŉ�]�������p�x�������t�����ɂ�����
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateY_Reverse( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateY_Reverse( s, c );
	}
	// -------------------------------------------------------------------------
	// Rotate Z
	// -------------------------------------------------------------------------

	/**
	 *   �s��̂y���ŉ�]�������p�x������ݒ�
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 * @remark ���W�����ɂ͉������Ȃ�
	 */
	void SetRotateZ( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->SetRotateZ( s, c );
	}

	/**
	 *   �s�� m0 �ɂy���ŉ�]�������p�x������������
	 * @param  m0 ���͍s��
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateZ( const self_type& m0, const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateZ( m0, s, c );
	}

	/**
	 *   �����ɂy���ŉ�]�������p�x������������
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateZ( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateZ( s, c );
	}

	/**
	 *   �����ɂ����� 1/2PI ��]�������p�x������������
	 * @return ����
	 * @remark SinCos �Ɛς��ȗ����Ă���̂ō���
	 */
	void RotateZ_HalfPi( void ) {
		CAST_34_PTR( this )->RotateZ_HalfPi( );
	}

	/**
	 *   �����ɂ����� -1/2PI ��]�������p�x������������
	 * @return ����
	 */
	void RotateZ_MinusHalfPi( void ) {
		CAST_34_PTR( this )->RotateZ_MinusHalfPi( );
	}

	/**
	 *   �����ɂ����� PI ��]�������p�x������������
	 * @return ����
	 */
	void RotateZ_OnePi( void ) {
		CAST_34_PTR( this )->RotateZ_OnePi( );
	}

	/**
	 *   �����ɂy���ŉ�]�������p�x�������t�����ɂ�����
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @return ����
	 */
	void RotateZ_Reverse( const f32 s, const f32 c ) {
		CAST_34_PTR( this )->RotateZ_Reverse( s, c );
	}

	// -------------------------------------------------------------------------
	// Determinant
	// -------------------------------------------------------------------------

	/**
	 *   �R�w�R�̌�����q
	 * @return ������q
	 */
	f32 Determinant33( void ) const {
		return CONST_CAST_34_PTR( this )->Determinant33( );
	}

	/**
	 *   �R�w�R�̋t�s��
	 * @param  m0 ���͍s��
	 * @return ����
	 * @remark ���W�����ɂ́i�O�C�O�C�O�C�P�j���ݒ肳���B\n
	 * �X�P�[�����������ĂȂ��ꍇ�͓]�u�s��̎g�p�������߂��܂��B
	 */
	void Inverse33( const self_type& m0 ) {
		CAST_34_PTR( this )->Inverse33( m0 );
	}

	/**
	 *   Y���W�ȉ��ɂȂ�Ȃ��悤�ȃx�N�g�������߂�
	 * @param  res �v�Z���ʃx�N�g��
	 * @param  y0 �Œ�Y���W
	 * @param  r ���̒���
	 * @return �Ȃ�
	 */
	void GetPositionByLowestY( VectorNative* res, const f32 y0, const f32 r ) const {
		CONST_CAST_34_PTR( this )->GetPositionByLowestY( res, y0, r );
	}

	void MatrixToQuaternion( QuaternionNative* q ) const {
		CONST_CAST_34_PTR( this )->MatrixToQuaternion( q );
	}

	Quaternion MatrixToQuaternion( void ) const;

	void QuaternionToMatrix( const QuaternionNative& q ) {
		CAST_34_PTR( this )->QuaternionToMatrix( q );
	}

	void GetRadian( f32* x, f32* y, f32* z ) const {
		CONST_CAST_34_PTR( this )->GetRadian( x, y, z );
	}

	void GetRadianYXZ( f32* x, f32* y, f32* z ) const {
		CONST_CAST_34_PTR( this )->GetRadianYXZ( x, y, z );
	}

	void GlobalToLocal33( VectorNative* loc, const VectorNative& pos ) const {
		CONST_CAST_34_PTR( this )->GlobalToLocal33( loc, pos );
	}

	void GlobalToLocal33Scaled( VectorNative* loc, const VectorNative& pos ) const {
		CONST_CAST_34_PTR( this )->GlobalToLocal33Scaled( loc, pos );
	}

	bool AimZY( const VectorNative& tag, f32* zang_sin, f32* zang_cos, f32* yang_sin
			 , f32* yang_cos, const bool reverse = false, const bool scaled = false ) const {
		return CONST_CAST_34_PTR( this )->AimZY( tag, zang_sin, zang_cos, yang_sin, yang_cos, reverse, scaled );
	}

	bool AimYZ( const VectorNative& tag, f32* yang_sin, f32* yang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const {
		return CONST_CAST_34_PTR( this )->AimYZ( tag, yang_sin, yang_cos, zang_sin, zang_cos, reverse, scaled );
	}

	bool AimYX( const VectorNative& tag, f32* yang_sin, f32* yang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const {
		return CONST_CAST_34_PTR( this )->AimYZ( tag, yang_sin, yang_cos, zang_sin, zang_cos, reverse, scaled );
	}

	bool AimXZ( const VectorNative& tag, f32* xang_sin, f32* xang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const {
		return CONST_CAST_34_PTR( this )->AimYZ( tag, xang_sin, xang_cos, zang_sin, zang_cos, reverse, scaled );
	}

	bool AimXY( const VectorNative& tag, f32* xang_sin, f32* xang_cos, f32* zang_sin
			 , f32* zang_cos, const bool reverse = false, const bool scaled = false ) const {
		return CONST_CAST_34_PTR( this )->AimYZ( tag, xang_sin, xang_cos, zang_sin, zang_cos, reverse, scaled );
	}

	bool AimZY( const VectorNative& tag ) {
		return CAST_34_PTR( this )->AimZY( tag );
	}

	bool AimYZ( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f ) {
		return CAST_34_PTR( this )->AimYZ( tag, reverse, rate );
	}

	bool AimYX( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f ) {
		return CAST_34_PTR( this )->AimYZ( tag, reverse, rate );
	}

	bool AimXZ( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f ) {
		return CAST_34_PTR( this )->AimYZ( tag, reverse, rate );
	}

	bool AimXY( const VectorNative& tag, const bool reverse = false, const f32 rate = 1.0f ) {
		return CAST_34_PTR( this )->AimYZ( tag, reverse, rate );
	}

	void AimX( f32* xsin, f32* xcos ) {
		CAST_34_PTR( this )->AimX( xsin, xcos );
	}

	void AimX( void ) {
		CAST_34_PTR( this )->AimX( );
	}

	void Apply( VectorNative* v0, const VectorNative& v1 ) const {
		CONST_CAST_34_PTR( this )->Apply( v0, v1 );
	}

	void SetScale( const f32 x, const f32 y, const f32 z ) {
		CAST_34_PTR( this )->SetScale( x, y, z );
	}

	/**
	 *   �R�w�R�����̏�Z
	 * @param  m0 ���͍s��
	 * @param  m1 ���͍s��
	 * @return ����
	 */
	void Mul33( const self_type& m0, const self_type& m1 ) {
		CAST_34_PTR( this )->Mul33( m0, m1 );
	}

	void Normalize( VectorNative* scl = 0 ) {
		CAST_34_PTR( this )->Normalize( scl );
	}

	self_type& Slerp( const self_type& m0, const self_type& m1, const f32 rate ) {
		CAST_34_PTR( this )->Slerp( m0, m1, rate );
		return *this;
	}
	bool operator==(const self_type& m0);
	bool operator!=(const self_type& m0);

	void PrintConsole( void ) const {
		CONST_CAST_34_PTR( this )->PrintConsole( );
	}

	// =========================================================================
	// =========================================================================
	using MatrixSoaNative44::SetColumn;
};

#endif	/* GFL_MATRIX44SOA_H */


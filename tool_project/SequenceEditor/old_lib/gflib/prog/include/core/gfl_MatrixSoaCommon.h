/* 
 * File:   gfl_MatrixSoaCommon.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 14:20
 */

//#ifndef GFL_MATRIX_SOA_BASE_H // ���̃t�@�C����34,44���p�Ȃ̂łQ��C���N���[�h������
//#define	GFL_MATRIX_SOA_BASE_H

#include "gfl_MatrixCommon.h"

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

//#endif

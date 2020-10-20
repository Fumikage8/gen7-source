/* 
 * File:   gfl_Quaternion.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:51
 */

#ifndef GFL_QUATERNION_H
#define	GFL_QUATERNION_H

#pragma once
/**
 *  �N�H�[�^�j�I���N���X
 */

namespace gfl {
namespace core {
//class Quaternion : public VectorTemplate<QuaternionNative> {

class Quaternion : public QuaternionNative {
public:
	typedef QuaternionNative parent_type;
	typedef Quaternion self_type;

#include "gfl_Tuple4Common.h"

	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	/**
	 *   �f�t�H���g�R���X�^���N�^�[
	 * @param  ����
	 * @return ����
	 * @remark �������Ȃ�
	 */

	Quaternion( void ) {
	}

	/**
	 *   �w�C�x�C�y�C�v������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @param  w �ݒ�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	 * @return ����
	 */

	Quaternion( const f32 x, const f32 y, const f32 z, const f32 w ) {
		Set( x, y, z, w );
	}

	/**
	 *   Quaternion ��ݒ肷��R���X�^���N�^�[
	 * @param  v Quaternion�N���X
	 * @return ����
	 */

	Quaternion( const Quaternion& v ) {
		Set( v );
	}

	Quaternion( const QuaternionNative& v ) {
		Set( v );
	}

	// -------------------------------------------------------------------------
	// Destruct !! not virtual for alignment
	// -------------------------------------------------------------------------

	~Quaternion( void ) {
	}

	//	using VectorTemplate<QuaternionNative>::Set;
	// -------------------------------------------------------------------------
	// Conjugate
	// -------------------------------------------------------------------------

	/**
	 *   ����
	 * @param  t �����ɂ������N���X
	 * @return ����
	 */

	inline void Conj( const Quaternion& q ) {
		GFL_VECTOR_X = -q.GFL_VECTOR_X;
		GFL_VECTOR_Y = -q.GFL_VECTOR_Y;
		GFL_VECTOR_Z = -q.GFL_VECTOR_Z;
		GFL_VECTOR_W = q.GFL_VECTOR_W;
	}

	/**
	 *   ����
	 * @param  ����
	 * @return ����
	 */

	inline void Conj( void ) {
		GFL_VECTOR_X = -GFL_VECTOR_X;
		GFL_VECTOR_Y = -GFL_VECTOR_Y;
		GFL_VECTOR_Z = -GFL_VECTOR_Z;
	}

	/**
	 *   ����
	 * @param  ����
	 * @return ����
	 */

	inline Quaternion GetConj( void ) const {
		Quaternion ret;
		ret.Conj( *this );
		return ret;
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  ����
	 * @return ����
	 */

	inline void Neg( void ) {
		Conj( *this );
	}

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  t �l�Q�[�g�������N���X
	 * @return ����
	 */

	inline void Neg( const QuaternionNative& t ) {
		Conj( t );
	}

	inline Quaternion GetNeg( void ) {
		return GetConj( );
	}
	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------
	//	using VectorTemplate<QuaternionNative>::Mul;
#ifndef GFL_PLATFORM_3DS
	using QuaternionNative::Mul;
#endif
	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */
	void Mul( const QuaternionNative& t, const QuaternionNative& s );

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 */

	inline void Mul( const QuaternionNative& t ) {
		Mul( *this, t );
	}

	/**
	 *   ����
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	inline f32 Dot( const QuaternionNative& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Y * t.GFL_VECTOR_Y
			+ GFL_VECTOR_Z * t.GFL_VECTOR_Z + GFL_VECTOR_W * t.GFL_VECTOR_W;
	}

	// -------------------------------------------------------------------------
	// Inverse
	// -------------------------------------------------------------------------
	/**
	 *   Inverse
	 * @param  t Inverse �������N���X
	 * @return ����
	 */
	void Inverse( const Quaternion& t );

	/**
	 *   Inverse
	 * @param  ����
	 * @return ����
	 */

	inline void Inverse( void ) {
		Inverse( *this );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X self_type
	 * @remark W �͔��]���Ȃ��c�d���Ȃ��ւ�ȕςȃ��C�u�������I�[�o�[���[�h
	 */

	inline self_type operator-() const {
		return self_type( -GFL_VECTOR_X, -GFL_VECTOR_Y, -GFL_VECTOR_Z, GFL_VECTOR_W );
	}

	// -------------------------------------------------------------------------
	// Slerp
	// -------------------------------------------------------------------------
	/**
	 *   �⊮
	 * @param  t �⊮�������N���X
	 * @param  s �⊮�������N���X
	 * @param  rate �����l
	 * @return ����
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	void Slerp( const Quaternion& t, const Quaternion& s, const f32 rate );

	/**
	 *   �⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	inline void Slerp( const Quaternion& t, const f32 rate ) {
		Slerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// �ϊ�
	// -------------------------------------------------------------------------
	/**
	 *   ���W�A������N�H�[�^�j�I���֕ϊ�
	 * @param  x �w���p�x
	 * @param  y �x���p�x
	 * @param  z �y���p�x
	 * @return ����
	 * @remark ��x�s��ɕϊ����Ă���̂Ŗ��ʂ�����
	 */
	void RadianToQuaternion( const f32 x, const f32 y, const f32 z );
	void RadianYXZToQuaternion( const f32 x, const f32 y, const f32 z );

	/**
	 *   ���W�A������N�H�[�^�j�I���֕ϊ�
	 * @param  v ���̓x�N�g��
	 * @return ����
	 * @remark ��x�s��ɕϊ����Ă���̂Ŗ��ʂ�����
	 */

	inline void RadianToQuaternion( const Vector4& v ) {
		RadianToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	// -------------------------------------------------------------------------

	/**
	 *   ���W�A������N�H�[�^�j�I���֕ϊ�
	 * @param  v ���̓x�N�g��
	 * @return ����
	 * @remark ��x�s��ɕϊ����Ă���̂Ŗ��ʂ�����
	 */

	inline void RadianToQuaternion( const Vector3& v ) {
		RadianToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	inline void RadianYXZToQuaternion( const Vector3& v ) {
		RadianYXZToQuaternion( v.GetX( ), v.GetY( ), v.GetZ( ) );
	}

	/**
	 *   �p�x�x����N�H�[�^�j�I���֕ϊ�
	 * @param  yang �x���p�x
	 * @return ����
	 */
	void RadianYToQuaternion( const f32 yang );

	/**
	 *   �N�H�[�^�j�I�����烉�W�A���֕ϊ�
	 * @param  x ���W�A���w�l�̎Q��
	 * @param  y ���W�A���x�l�̎Q��
	 * @param  z ���W�A���y�l�̎Q��
	 * @return ����
	 * @remark ��x�s��ɕϊ����Ă���̂Ŗ��ʂ������B�����Ő��K�����Ă���B
	 */
	void QuaternionToRadian( f32* x, f32* y, f32* z ) const ;
	void QuaternionToRadianYXZ( f32* x, f32* y, f32* z ) const ;

	template <typename T>
	void QuaternionToRadianYXZ( T* v ) {
		QuaternionToRadianYXZ( &v->GetRefX( ), &v->GetRefY( ), &v->GetRefZ( ) );
	}
	f32 QuaternionToRadianY( void ) const;

	/**
	 *   �N�H�[�^�j�I�����烉�W�A���֕ϊ�
	 * @param  v ���̓N�E�H�[�^�j�I��
	 * @return ����
	 * @remark ��x�s��ɕϊ����Ă���̂Ŗ��ʂ�����
	 */

	template<typename T>
	inline void QuaternionToRadian( T* v ) const {
		QuaternionToRadian( &v->GetRefX( ), &v->GetRefY( ), &v->GetRefZ( ) );
	}

	/**
	 *   �N�H�[�^�j�I�����烉�W�A���֕ϊ�
	 * @param  ����
	 * @return ���W�A���l
	 * @remark ��x�s��ɕϊ����Ă���̂Ŗ��ʂ�����
	 */

	inline Vector QuaternionToRadian( void ) const {
		Vector ret;
		QuaternionToRadian( &ret.GetRefX( ), &ret.GetRefY( ), &ret.GetRefZ( ) );
		return ret;
	}

	/**
	 *   �s�񂩂�N�H�[�^�j�I���֕ϊ�
	 * @param  m ���͍s��
	 * @return ����
	 */

	void MatrixToQuaternion( const Matrix& m ) {
		Set( m.MatrixToQuaternion( ) );
	}

	/**
	 *   �N�H�[�^�j�I������s��֕ϊ�
	 * @param  m �o�͍s��
	 * @return ����
	 */

	template<typename T>
	void QuaternionToMatrix( T* m ) const {
		m->QuaternionToMatrix( *this );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	using QuaternionNative::operator*=;
	using QuaternionNative::operator*;

	/**
	 *   �I�y���[�^�[ *=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	inline Quaternion & operator*=(const Quaternion& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  t �ώZ�������N���X
	 * @return �ώZ���ꂽ�N���X Quaternion
	 */

	inline Quaternion operator*(const Quaternion& t) const {
		Quaternion r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const Quaternion& t ) {
		Quaternion r;
		r.Conj( t );
		Mul( r, *this );
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark ���΂p��������������������ݒ�
	 */

	inline void Sub( const Quaternion& t, const Quaternion& s ) {
		Conj( s );
		Mul( t );
	}

	// -------------------------------------------------------------------------
	// Magnitude
	// -------------------------------------------------------------------------

	/**
	 *   �傫���̓��
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline f32 Magnitude2( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Y * GFL_VECTOR_Y +
			GFL_VECTOR_Z * GFL_VECTOR_Z + GFL_VECTOR_W*GFL_VECTOR_W;
	}

	/**
	 *   �傫���̓��̋t��
	 * @param  ����
	 * @return �傫���̓��̋t��
	 */

	inline f32 Magnitude2Recip( void ) const {
		return Math::FRecip( Magnitude2( ) );
	}

	/**
	 *   �傫��
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 Magnitude( void ) const {
		return Math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   �傫���̋t��
	 * @param  ����
	 * @return �傫���̋t��
	 */

	inline f32 MagnitudeRecip( void ) const {
		return Math::FRecip( Magnitude( ) );
	}
	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   ���K��
	 * @param  ����
	 * @return ����
	 */

	inline void Normalize( void ) {
		Mul( Math::FRecip( Magnitude( ) ) );
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	inline void Normalize( const QuaternionNative& t ) {
		Set( t );
		Normalize( );
	}

	/**
	 *   ���K��
	 * @param  ����
	 * @return ���̑傫��
	 */

	inline f32 NormalizeRet( void ) {
		f32 ret = Magnitude( );
		Mul( Math::FRecip( ret ) );
		return ret;
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ���̑傫��
	 */

	inline f32 NormalizeRet( const QuaternionNative& t ) {
		Set( t );
		return NormalizeRet( );
	}

	/**
	 *   �I�y���[�^�[ -=
	 * @param  t ���Z�������N���X
	 * @return this �̎Q��
	 */

	inline Quaternion & operator-=(const Quaternion& t) {
		Quaternion r;
		r.Conj( t );
		Mul( r, *this );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Quaternion
	 */

	inline Quaternion operator-(const Quaternion& t) const {
		Quaternion r;
		r.Sub( *this, t );
		return r;
	}
};
}
}

#endif	/* GFL_QUATERNION_H */


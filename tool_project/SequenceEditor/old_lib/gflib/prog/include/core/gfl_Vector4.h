/* 
 * File:   gfl_Vector4.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_VECTOR4_H
#define	GFL_VECTOR4_H

//class Vector4 : public VectorTemplate<VectorNative4> {
class Vector4 : public VectorNative4 {
public:
	typedef VectorNative4 parent_type;
	typedef Vector4 self_type;
	
#include "gfl_Tuple4Common.h"
	
	static const VectorStruct4 s_X1;
	static const VectorStruct4 s_Y1;
	static const VectorStruct4 s_Z1;
	static const VectorStruct4 s_W1;

	// ---------------------------------------------------------------------------------
	// Constructer
	// ---------------------------------------------------------------------------------
	//  ----------------------------------------------------------------
	//! @brief  �f�t�H���g�R���X�^���N�^�[
	//! @param  ����
	//! @return ����
	//! @remark �������Ȃ�
	//  ----------------------------------------------------------------

	Vector4( void ) {
	}

	//  ----------------------------------------------------------------
	//! @brief  �w�C�x�C�y�C�v������ݒ肷��R���X�^���N�^�[
	//! @param  x �ݒ�l
	//! @param  y �ݒ�l
	//! @param  z �ݒ�l
	//! @param  w �ݒ�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	//! @return ����
	//  ----------------------------------------------------------------

	Vector4( const f32 vx, const f32 vy, const f32 vz, const f32 vw = 0.0f ) {
		Set( vx, vy, vz, vw );
	}

	//  ----------------------------------------------------------------
	//! @brief  Vector4 ��ݒ肷��R���X�^���N�^�[
	//! @param  v Vector4�N���X
	//! @return ����
	//  ----------------------------------------------------------------
	Vector4(const VectorNative4& vt){
		Set(vt);
	}

	//  ----------------------------------------------------------------
	//! @brief  Vector4 ��ݒ肷��R���X�^���N�^�[
	//! @param  v Tuple4f�N���X
	//! @return ����
	//  ----------------------------------------------------------------
	Vector4(const VectorNative3& vt){
		Set(vt);
	}

	//  ----------------------------------------------------------------
	//! @brief  Vector4 ��ݒ肷��R���X�^���N�^�[
	//! @param  v Tuple4f�N���X
	//! @return ����
	//  ----------------------------------------------------------------
	Vector4(const VectorNative3& vt, const f32 w){
		Set(vt,w);
	}

	// ---------------------------------------------------------------------------------
	// Destruct !! not virtual for alignment
	// ---------------------------------------------------------------------------------

	~Vector4( ) {
	}

	/**
	 * (0,0,0,0) ���擾
	 * @return (0,0,0,0)
	 */
	static const Vector4& GetZero( void ) {
		return static_cast<const Vector4&> (VectorNative4::Zero( ));
	}

	/**
	 * (1,0,0,0) ���擾
	 * @return (1,0,0,0)
	 */
	static const Vector4& GetX1( void ) {
		return static_cast<const Vector4&> (s_X1);
	}

	/**
	 * (0,1,0,0) ���擾
	 * @return (0,1,0,0)
	 */
	static const Vector4& GetY1( void ) {
		return static_cast<const Vector4&> (s_Y1);
	}

	/**
	 * (0,0,1,0) ���擾
	 * @return (0,0,1,0)
	 */
	static const Vector4& GetZ1( void ) {
		return static_cast<const Vector4&> (s_Z1);
	}

	/**
	 * (0,0,0,1) ���擾
	 * @return (0,0,0,1)
	 */
	static const Vector4& GetW1( void ) {
		return static_cast<const Vector4&> (s_W1);
	}

	/**
	 * VectorNative4 ��ݒ�
	 * @param tx X����
	 * @param ty Y����
	 * @param tz Z����
	 * @remark W�����͐ݒ肵�Ȃ�
	 */
	inline void Set( const f32 tx, const f32 ty, const f32 tz ) {
		GFL_VECTOR_X = tx;
		GFL_VECTOR_Y = ty;
		GFL_VECTOR_Z = tz;
	}

	/**
	 * VectorNative4 ��ݒ�
	 * @param t �ݒ肵����VectorNative3
	 * @param tw W����
	 */
	inline void Set( const VectorNative3& t ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
	}

	/**
	 * VectorNative4 ��ݒ�
	 * @param t �ݒ肵����VectorNative3
	 * @param tw W����
	 */
	inline void Set( const VectorNative3& t, const f32 tw ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z, tw );
	}

	/**
	 * (1,0,0,0) ��ݒ�
	 */
	void SetX1( void ) {
		Set( static_cast<const Vector4&> (s_X1) );
	}

	/**
	 * (0,1,0,0) ��ݒ�
	 */
	void SetY1( void ) {
		Set( static_cast<const Vector4&> (s_Y1) );
	}

	/**
	 * (0,0,1,0) ��ݒ�
	 */
	void SetZ1( void ) {
		Set( static_cast<const Vector4&> (s_Z1) );
	}

	/**
	 * (0,0,0,1) ��ݒ�
	 */
	void SetW1( void ) {
		Set( static_cast<const Vector4&> (s_W1) );
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 */

	inline void SetZero( void ) {
		Set( Zero( ) );
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
		*this = - * this;
	}

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  t �l�Q�[�g�������N���X
	 * @return ����
	 */

	inline void Neg( const VectorNative4& t ) {
		*this = -t;
	}

	inline Vector4 GetNeg( void ) {
		return - * this;
	}

	/**
	 * X �̕����𔽓]������
     */
	inline void NegX(void){
		GFL_VECTOR_X = -GFL_VECTOR_X;
	}

	/**
	 * Y �̕����𔽓]������
     */
	inline void NegY(void){
		GFL_VECTOR_Y = -GFL_VECTOR_Y;
	}

	/**
	 * Z �̕����𔽓]������
     */
	inline void NegZ(void){
		GFL_VECTOR_Z = -GFL_VECTOR_Z;
	}

	/**
	 * W �̕����𔽓]������
     */
	inline void NegW(void){
		GFL_VECTOR_W = -GFL_VECTOR_W;
	}

	/**
	 *   �O��
	 * @param  t �O�ς����߂����N���X
	 * @param  s �O�ς����߂����N���X
	 * @return ����
	//				 * @remark t, s �� this �ƈႤ�N���X�łȂ��ƃA�T�[�g���܂�
	 */

	inline void Cross( const VectorNative4& t, const VectorNative4& s ) {
		Set( t.GFL_VECTOR_Y * s.GFL_VECTOR_Z - t.GFL_VECTOR_Z * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_X - t.GFL_VECTOR_X * s.GFL_VECTOR_Z,
			 t.GFL_VECTOR_X * s.GFL_VECTOR_Y - t.GFL_VECTOR_Y * s.GFL_VECTOR_X, 0.0f );
	}

	/**
	 *   �O��
	 * @param  s �O�ς����߂����N���X
	 * @return ����
	 */

	inline void Cross( const VectorNative4& s ) {
		Cross( *this, s );
	}

	/**
	 *   �x�����̊O��
	 * @param  t �O�ς����߂����N���X
	 * @return ���Ȃ������@���Ȃ牺����
	 */

	inline f32 CrossY( const Vector3& t ) const {
		return GFL_VECTOR_Z * t.GFL_VECTOR_X - GFL_VECTOR_X * t.GFL_VECTOR_Z;
	}

	// -------------------------------------------------------------------------
	// Optimize radian
	// -------------------------------------------------------------------------

	inline void OptimizeRadian( void ) {
		gfl::core::Math::OptimizeRadian( &GFL_VECTOR_X, &GFL_VECTOR_Y, &GFL_VECTOR_Z );
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
		return LengthSquare( );
	}

	/**
	 *   �傫���̓��̋t��
	 * @param  ����
	 * @return �傫���̓��̋t��
	 */

	inline f32 Magnitude2Recip( void ) const {
		return Math::FRecip( LengthSquare( ) );
	}

	/**
	 *   �傫��
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 Magnitude( void ) const {
		return Length( );
	}

	/**
	 *   �傫���̋t��
	 * @param  ����
	 * @return �傫���̋t��
	 */

	inline f32 MagnitudeRecip( void ) const {
		return Math::FRecip( Length( ) );
	}

	/**
	 * XZ���ʂ̋����̓��
	 * @return XZ���ʂ̋����̓��
	 */
	inline f32 MagnitudeXZ2( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Z * GFL_VECTOR_Z;
	}

	/**
	 *   �w�y���ʂ̑傫��
	 * @param  ����
	 * @return �w�y���ʂ̑傫��
	 */

	inline f32 MagnitudeXZ( void ) const {
		return Math::FSqrt( MagnitudeXZ2( ) );
	}

	/**
	 *   �w�y���ʂ̑傫���̋t��
	 * @param  ����
	 * @return �w�y���ʂ̑傫���̋t��
	 */

	inline f32 MagnitudeXZRecip( void ) const {
		return Math::FRecip( MagnitudeXZ( ) );
	}

	/**
	 * XY���ʂ̋����̓��
	 * @return XY���ʂ̋����̓��
	 */
	inline f32 MagnitudeXY2( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Y * GFL_VECTOR_Y;
	}
	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	inline void Normalize( const VectorNative4& t ) {
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

	inline f32 NormalizeRet( const VectorNative4& t ) {
		Set( t );
		return NormalizeRet( );
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */
	inline f32 Distance2( const VectorNative4& t )	 const
	{
#ifdef GFL_PLATFORM_3DS
		return nn::math::VEC4DistSq( this, &t );
#else
		return DistanceSquare( t );
#endif
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 Distance( const VectorNative4& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	inline f32 DistanceXZ2( const VectorNative4& t ) const {
		return Vector4( *this -t ).MagnitudeXZ2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 DistanceXZ( const VectorNative4& t ) const {
		return Math::FSqrt( DistanceXZ2( t ) );
	}

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	inline f32 DistanceXY2( const VectorNative4& t ) const {
		return Vector4( *this -t ).MagnitudeXY2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 DistanceXY( const VectorNative4& t ) const {
		return Math::FSqrt( DistanceXY2( t ) );
	}

	/**
	 *   ����XZ
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	inline f32 DotXZ( const VectorNative4& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Z * t.GFL_VECTOR_Z;
	}

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ��������� VectorNative4
	 * @return this �̎Q��
	 */

	inline Vector4 & operator=(const VectorNative3& t) {
		Set( t );
		return *this;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative3& t ) {
		GFL_TP GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_TP GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_TP GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
	}
	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative4& t ) {
		GFL_TP GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_TP GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_TP GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
		GFL_TP GFL_VECTOR_W *= t.GFL_VECTOR_W;
	}

	/**
	 * ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
     */
	inline void Mul33( const VectorNative3& t, const VectorNative3& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X,
			 t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z );
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative3& t, const VectorNative3& s ) {
		Mul33(t,s);
	}
	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative3& t, const VectorNative4& s ) {
		Mul33(t,reinterpret_cast<const VectorNative3&>(s));
	}
	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative4& t, const VectorNative3& s ) {
		Mul33(reinterpret_cast<const VectorNative3&>(t),s);
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative4& t, const VectorNative4& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X,
			 t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z,
			 t.GFL_VECTOR_W * s.GFL_VECTOR_W );
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  t �ώZ�������N���X
	 * @return �ώZ���ꂽ�N���X VectorNative4
	 */

	inline Vector4 operator*(const VectorNative4& t) const {
		Vector4 r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ ^=
	 * @param  t �O�ς����߂����N���X
	 * @return this �̎Q��
	 */

	inline Vector4 & operator^=(const VectorNative4& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ^
	 * @param  t �O�ς����߂����N���X
	 * @return �O�ς��ݒ肳�ꂽ�N���X VectorNative4
	 */

	inline Vector4 operator^(const VectorNative4& t) const {
		Vector4 r;
		r.Cross( *this, t );
		return r;
	}

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */
	inline void Lerp( const VectorNative4& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	/**
	 * �O���X�O�x�͈̔͂ɓ����Ă��邩
	 * @return �^�@�����Ă���
	 */
	inline bool IsFrontDegree90( void ) {
		return gfl::core::Math::IsFrontDegree90( GFL_VECTOR_X, GFL_VECTOR_Z );
	}

	/**
	 * ���݂̃x�N�g������]������
	 * @param s�@�T�C���l
	 * @param c�@�R�T�C���l
	 */
	void RotateY( const f32 s, const f32 c ) {
		const f32 tx = GFL_VECTOR_X;
		const f32 tz = GFL_VECTOR_Z;
		GFL_VECTOR_X = gfl::core::Math::GetX_RotateY_SinCosXZ( s, c, tx, tz );
		GFL_VECTOR_Z = gfl::core::Math::GetZ_RotateY_SinCosXZ( s, c, tx, tz );
	}

	/**
	 * ���݂̃x�N�g������]������
	 * @param ang ��]�l�@���W�A��
	 */
	void RotateY( const f32 ang ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, ang );
		RotateY( s, c );
	}
	
	/**
	 *   �I�y���[�^�[ +
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X VectorNative3
	 */

	inline Vector4 operator+(const VectorNative4& t) const {
		return Vector4(VectorNative4::operator+(t));
	}
	// =========================================================================
	// using
	// =========================================================================
	using VectorNative4::Lerp;
	using VectorNative4::Set;
	using VectorNative4::operator+;
	using VectorNative4::operator*;
	using VectorNative4::Normalize;
//	using VectorNative4::Cross;
#ifndef GFL_PLATFORM_3DS
	using VectorNative4::Mul;
#endif
};

#endif	/* GFL_VECTOR4_H */


//==============================================================================
/**
 * @file	gfl_QuaternionNative.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/22, 15:36
 */
// =============================================================================

#if !defined( __GFL_QUATERNIONNATIVE_H__ )
#define	__GFL_QUATERNIONNATIVE_H__
#pragma once

// =============================================================================
// ���̃t�@�C���Ɋ܂܂��N���X�Q
// =============================================================================
struct QuaternionStruct;
class QuaternionNative;

/**
 *  �Q�̗v�f�����^�v���\����
 */

struct QuaternionStruct {
	f32 GFL_VECTOR_X;
	f32 GFL_VECTOR_Y;
	f32 GFL_VECTOR_Z;
	f32 GFL_VECTOR_W;
};

/**
 * �l�C�e�B�u���߃N���X
 */

class QuaternionNative : public QuaternionStruct {
public:
    typedef QuaternionNative self_type; /**< �����̌^�ł� */
    typedef f32  value_type; /**< �v�f�̌^�ł� */
	static const s32 DIMENSION = 4;

	/**
	 * �f�t�H���g�R���X�g���N�^�@�l�͕s��
	 */
	QuaternionNative( ) {
	}

	/**
	 * �R���X�g���N�^
	 * @param vn�@�ݒ肵����QuaternionNative�N���X
	 */
	QuaternionNative( const QuaternionNative& vn ) {
		Set(vn);
	}

	/**
	 * �R���X�g���N�^
	 * @param vn�@�ݒ肵����QuaternionNative�N���X
	 */
	QuaternionNative( const f32* vn ) {
		Set(*reinterpret_cast<const self_type*> (vn) );
	}

	/**
	 * �R���X�g���N�^
	 * @param fx�@�ݒ肵����X���W
	 * @param fy�@�ݒ肵����Y���W
	 * @param fz�@�ݒ肵����Z���W
	 * @param fw�@�ݒ肵����W���W
	 */
	QuaternionNative( f32 fx, f32 fy, f32 fz, f32 fw ) {
		GFL_VECTOR_X = fx;
		GFL_VECTOR_Y = fy;
		GFL_VECTOR_Z = fz;
		GFL_VECTOR_W = fw;
	}

	/**
	 * �f�X�g���N�^�@�������Ȃ�
	 */
	~QuaternionNative( ) {
	}

	/**
	 * ���������_�̃|�C���^���擾����
	 * @return ���������_�̃|�C���^
	 */
	inline operator f32*() {
		return &GFL_VECTOR_X;
	}

	/**
	 * ���������_�̃|�C���^���擾����
	 * @return ���������_�̃|�C���^
	 */
	inline operator const f32*() const {
		return &GFL_VECTOR_X;
	}

	/**
	 *   �I�y���[�^�[ +=
	 * @param  t ���Z�������N���X
	 * @return this �̎Q��
	 */

	inline self_type & operator+=(const self_type& t) {
		Add( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ -=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	inline self_type & operator-=(const self_type& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  f ��Z�������l
	 * @return this �̎Q��
	 */

	inline self_type & operator*=(const f32 f) {
		Mul( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	inline self_type & operator*=(const self_type& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ /=
	 * @param  f ���Z�������l
	 * @return this �̎Q��
	 */

	inline self_type & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

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
		self_type r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X self_type
	 * @remark W �͔��]���Ȃ����Ƃ����������ǁc�d���Ȃ��ւ�ȕςȃ��C�u�����ɍ��킹��
	 */

	inline self_type operator-() const {
		return self_type( -GFL_VECTOR_X, -GFL_VECTOR_Y, -GFL_VECTOR_Z, -GFL_VECTOR_W );
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X self_type
	 */

	inline self_type operator-(const self_type& t) const {
		self_type r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  f �ώZ�������l
	 * @return �ώZ���ꂽ�N���X self_type
	 */

	inline self_type operator*(const f32 f) const {
		self_type r;
		r.Mul( *this, f );
		return r;
	}

	/**
	 *   �I�y���[�^�[ /
	 * @param  f ���Z�������l
	 * @return ���Z���ꂽ�N���X Quaternion2f
	 */

	inline self_type operator/(const f32 f) const {
		self_type r;
		r.Div( *this, f );
		return r;
	}
	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	inline bool operator==(const self_type& t) const {
		return GFL_VECTOR_X == t.GFL_VECTOR_X && GFL_VECTOR_Y == t.GFL_VECTOR_Y
			&& GFL_VECTOR_Z == t.GFL_VECTOR_Z && GFL_VECTOR_W == t.GFL_VECTOR_W;
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	inline bool operator!=(const self_type& t) const {
		return GFL_VECTOR_X != t.GFL_VECTOR_X || GFL_VECTOR_Y != t.GFL_VECTOR_Y
			|| GFL_VECTOR_Z != t.GFL_VECTOR_Z || GFL_VECTOR_W != t.GFL_VECTOR_W;
	}

	/**
	 * ��Ԃ��o��
	 * @param newline ���s����
	 * @param str �\��������
	 */
	void Report( bool newline = true, const c8* str = 0 ) {
		Debug::PrintConsole( "%s { %ff, %ff, %ff, %ff },", str, GFL_VECTOR_X, GFL_VECTOR_Y, GFL_VECTOR_Z, GFL_VECTOR_W );
		if(newline){
			Debug::PrintConsole( "\n" );
		}
	}

	// =========================================================================
	// �������牺�͖{���Ȃ�����
	// =========================================================================
	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const self_type& t ) {
		GFL_VECTOR_X += t.GFL_VECTOR_X;
		GFL_VECTOR_Y += t.GFL_VECTOR_Y;
		GFL_VECTOR_Z += t.GFL_VECTOR_Z;
		GFL_VECTOR_W += t.GFL_VECTOR_W;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X + s.GFL_VECTOR_X, t.GFL_VECTOR_Y + s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z + s.GFL_VECTOR_Z, t.GFL_VECTOR_W + s.GFL_VECTOR_W );
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @param  tz ���Z�������y�l
	 * @return ����
	 */

	inline void Add( const f32 tx, const f32 ty, const f32 tz, const f32 tw = 0.0f ) {
		GFL_VECTOR_X += tx;
		GFL_VECTOR_Y += ty;
		GFL_VECTOR_Z += tz;
		GFL_VECTOR_W += tw;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @param  tz ���Z�������y�l
	 * @return ����
	 */

	inline void Add( const self_type& t, const f32 tx, const f32 ty, const f32 tz, const f32 tw ) {
		Set( t.GFL_VECTOR_X + tx, t.GFL_VECTOR_Y + ty, t.GFL_VECTOR_Z + tz, t.GFL_VECTOR_W + tw );
	}

	// -------------------------------------------------------------------------
	// Sub
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const self_type& t ) {
		GFL_VECTOR_X -= t.GFL_VECTOR_X;
		GFL_VECTOR_Y -= t.GFL_VECTOR_Y;
		GFL_VECTOR_Z -= t.GFL_VECTOR_Z;
		GFL_VECTOR_W -= t.GFL_VECTOR_W;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X - s.GFL_VECTOR_X, t.GFL_VECTOR_Y - s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z - s.GFL_VECTOR_Z, t.GFL_VECTOR_W - s.GFL_VECTOR_W );
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @param  tz ���Z�������y�l
	 * @return ����
	 */

	inline void Sub( const f32 tx, const f32 ty, const f32 tz, const f32 tw = 0.0f ) {
		GFL_VECTOR_X -= tx;
		GFL_VECTOR_Y -= ty;
		GFL_VECTOR_Z -= tz;
		GFL_VECTOR_W -= tw;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @param  tz ���Z�������y�l
	 * @return ����
	 */


	inline void Sub( const self_type& t, const f32 tx, const f32 ty, const f32 tz, const f32 tw = 0.0f ) {
		Set( t.GFL_VECTOR_X - tx, t.GFL_VECTOR_Y - ty, t.GFL_VECTOR_Z - tz, t.GFL_VECTOR_W - tw );
	}

	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �Ɗe��������Z����
	 */

	inline void Mul( const f32 f ) {
		GFL_VECTOR_X *= f;
		GFL_VECTOR_Y *= f;
		GFL_VECTOR_Z *= f;
		GFL_VECTOR_W *= f;
	}

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Mul( const f32 f, const self_type& t ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y, f * t.GFL_VECTOR_Z, f * t.GFL_VECTOR_W );
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Mul( const self_type& t, const f32 f ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y, f * t.GFL_VECTOR_Z, f * t.GFL_VECTOR_W );
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const self_type& t ) {
		GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
		GFL_VECTOR_W *= t.GFL_VECTOR_W;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X, t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z, t.GFL_VECTOR_W * s.GFL_VECTOR_W );
	}

	// -------------------------------------------------------------------------
	// Div
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark �e������ f �ŏ��Z����
	 */

	inline void Div( const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		GFL_VECTOR_X *= f_1;
		GFL_VECTOR_Y *= f_1;
		GFL_VECTOR_Z *= f_1;
		GFL_VECTOR_W *= f_1;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark t �̊e������ f �ŏ��Z���� �����I�ɂ͋t���̊|���Z
	 */

	inline void Div( const self_type& t, const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		Set( f_1 * t.GFL_VECTOR_X, f_1 * t.GFL_VECTOR_Y, f_1 * t.GFL_VECTOR_Z, f_1 * t.GFL_VECTOR_W );
	}
	
private:
	void Set( f32 vx, f32 vy, f32 vz, f32 vw ) // �{���͂Ȃ����߂Ȃ̂�
	{
		GFL_VECTOR_X = vx;
		GFL_VECTOR_Y = vy;
		GFL_VECTOR_Z = vz;
		GFL_VECTOR_W = vw;
	}
	void Set( const self_type& vn ) // �{���͂Ȃ����߂Ȃ̂�
	{
		GFL_VECTOR_X = vn.GFL_VECTOR_X;
		GFL_VECTOR_Y = vn.GFL_VECTOR_Y;
		GFL_VECTOR_Z = vn.GFL_VECTOR_Z;
		GFL_VECTOR_W = vn.GFL_VECTOR_W;
	}
};

#endif	/* __GFL_QUATERNIONNATIVE_H__ */


/* 
 * File:   gfl_Vector2.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_VECTOR2_H
#define	GFL_VECTOR2_H

/**
 *  �Q�̗v�f�����^�v���e���v���[�g�N���X
 */

class Vector2 : public VectorNative2 {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Vector2( void ) {
	}

	/**
	 *   �w�C�x������ݒ肷��R���X�^���N�^�[
	 * @param  tx �ݒ�l
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	Vector2( const f32 tx, const f32 ty ) {
		Set( tx, ty );
	}

	/**
	 *   Vector2 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Vector2�N���X
	 * @return ����
	 */

	explicit Vector2( const VectorNative2& t ) {
		Set( t );
	}

	Vector2( const Vector2& t ) {
		Set( t );
	}

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline f32 GetX( void ) const {
		return GFL_VECTOR_X;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline f32 GetY( void ) const {
		return GFL_VECTOR_Y;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TY(2)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline f32 Get( const s32 i ) const {
		return ( &GFL_VECTOR_X)[i];
	}

	/**
	 *   �ϐ��̃|�C���^�[���擾
	 * @param  ����
	 * @return �ϐ��̃|�C���^�[
	 */

	inline f32* Get( void ) {
		return ( &GFL_VECTOR_X);
	}

	inline const f32* Get( void ) const {
		return ( &GFL_VECTOR_X);
	}

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline f32& GetRefX( void ) {
		return GFL_VECTOR_X;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline f32& GetRefY( void ) {
		return GFL_VECTOR_Y;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TY(2)
	 * @return �w�肳�ꂽ����
	 */

	inline f32& GetRef( const s32 i ) {
		return ( &GFL_VECTOR_X)[i];
	}

	/**
	 * (0,0) ���擾
	 * @return (0,0)
	 */
	static const Vector2& GetZero( void ) {
		return static_cast<const Vector2&> (Zero( ));
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   �w������ݒ�
	 * @param  tx �ݒ�l
	 * @return ����
	 */

	inline void SetX( const f32 tx ) {
		GFL_VECTOR_X = tx;
	}

	/**
	 *   �x������ݒ�
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	inline void SetY( const f32 ty ) {
		GFL_VECTOR_Y = ty;
	}

	/**
	 *   Vector2 ��ݒ�
	 * @param  t �ݒ肵���� Vector �̎Q��
	 * @return ����
	 */
	using VectorNative2::Set;

	inline void Set( const VectorNative2& t ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y );
	}

	/**
	 *   �w�肵��������ݒ�
	 * @param  i �Y���� TX(0) �` TY(2)
	 * @param  val �ݒ�l
	 * @return ����
	 */

	inline void SetElem( const s32 i, const f32 val ) {
		(&GFL_VECTOR_X)[i] = val;
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
	// Add
	// -------------------------------------------------------------------------
#ifdef GFL_PLATFORM_3DS

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const VectorNative2& t ) {
		*this += t;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const VectorNative2& t, const VectorNative2& s ) {
		*this = t + s;
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const f32 tx, const f32 ty ) {
		Add( *this, VectorNative2( tx, ty ) );
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const VectorNative2& t, const f32 tx, const f32 ty ) {
		Add( t, VectorNative2( tx, ty ) );
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

	inline void Sub( const VectorNative2& t ) {
		*this -= t;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const VectorNative2& t, const VectorNative2& s ) {
		*this = t - s;
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Sub( const f32 tx, const f32 ty ) {
		Sub( *this, VectorNative2( tx, ty ) );
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */


	inline void Sub( const VectorNative2& t, const f32 tx, const f32 ty ) {
		Sub( t, VectorNative2( tx, ty ) );
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
		*this *= f;
	}

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Mul( const f32 f, const VectorNative2& t ) {
		*this = f * t;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Mul( const VectorNative2& t, const f32 f ) {
		*this = f * t;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative2& t ) {
		GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative2& t, const VectorNative2& s ) {
		GFL_VECTOR_X = t.GFL_VECTOR_X * s.GFL_VECTOR_X;
		GFL_VECTOR_Y = t.GFL_VECTOR_Y * s.GFL_VECTOR_Y;
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
		*this /= f;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark t �̊e������ f �ŏ��Z���� �����I�ɂ͋t���̊|���Z
	 */

	inline void Div( const VectorNative2& t, const f32 f ) {
		*this = t / f;
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

	inline void Neg( const VectorNative2& t ) {
		*this = -t;
	}

	inline VectorNative2 GetNeg( void ) {
		return - * this;
	}

#endif

	using VectorNative2::Lerp;

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */
	inline void Lerp( const VectorNative2& t, const f32 rate ) {
		Lerp( *this, t, rate );
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

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 * @remark �C�V���̃��C�u�����ɃR���X�g�����Ă��Ȃ��i�t���Y��H�j�̂ŃR���X�g���͂���
	 */
	inline f32 Distance2( const VectorNative2& t ) const {
#ifdef GFL_PLATFORM_3DS
		return nn::math::VEC2DistSq( this, &t );
#else
		return DistanceSquare( t ); // const ���t�����炱������
#endif
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 Distance( const VectorNative2& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	// -------------------------------------------------------------------------
	// Cross �񎟌��ɊO�ς͂���܂���
	// -------------------------------------------------------------------------

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   �I�y���[�^�[ []
	 * @param  i �Y���� TX(0) �` TZ(3)
	 * @return �w�肳�ꂽ����
	 */

	inline f32 operator [](s32 i) const {
		return Get( i );
	}

	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	inline bool operator==(const self_type& t) const {
		return IsSimilar( t );
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	inline bool operator!=(const self_type& t) const {
		return !IsSimilar( t );
	}

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ��������� VectorNative2
	 * @return this �̎Q��
	 */

	inline VectorNative2 & operator=(const VectorNative2& t) {
		Set( t );
		return *this;
	}

	using VectorNative2::operator*;

	/**
	 *   �I�y���[�^�[ *
	 * @param  t �ώZ�������N���X
	 * @return �ώZ���ꂽ�N���X VectorNative2
	 */

	inline VectorNative2 operator*(const VectorNative2& t) const {
		Vector2 r;
		r.Mul( *this, t );
		return r;
	}

	// -------------------------------------------------------------------------
	// Recip
	// -------------------------------------------------------------------------

	/**
	 *   �t���@�w�C�x�����̋t����ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void Recip( void ) {
		Set( Math::FRecip( GFL_VECTOR_X ), Math::FRecip( GFL_VECTOR_Y ) );
	}

	/**
	 *   �t���@�s�̂w�C�x�����̋t����ݒ肷��
	 * @param  t �t���ɂ������N���X
	 * @return ����
	 */


	inline void Recip( const VectorNative2& t ) {
		Set( Math::FRecip( t.GFL_VECTOR_X ), Math::FRecip( t.GFL_VECTOR_Y ) );
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	using VectorNative2::Normalize;

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	inline void Normalize( const VectorNative2& t ) {
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

	inline f32 NormalizeRet( const VectorNative2& t ) {
		Set( t );
		return NormalizeRet( );
	}

	// -------------------------------------------------------------------------
	// IsSimilar
	// -------------------------------------------------------------------------

	/**
	 *   �e���������Ă��邩�H
	 * @param  t �ΏۃN���X
	 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
	 * @return �e�����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
	 */

	inline bool IsSimilar( const VectorNative2& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::FAbs( t.GFL_VECTOR_X - GFL_VECTOR_X ) <= epsilon && Math::FAbs( t.GFL_VECTOR_Y - GFL_VECTOR_Y ) <= epsilon;
	}

	// -------------------------------------------------------------------------
	// Absolute
	// -------------------------------------------------------------------------

	/**
	 *   �e�����̐�Βl��ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void Abs( void ) {
		Set( Math::FAbs( GFL_VECTOR_X ), Math::FAbs( GFL_VECTOR_Y ) );
	}

	/**
	 *   t �̊e�����̐�Βl��ݒ肷��
	 * @param  t ���̓N���X
	 * @return ����
	 */

	inline void Abs( const VectorNative2& t ) {
		Set( Math::FAbs( t.GFL_VECTOR_X ), Math::FAbs( t.GFL_VECTOR_Y ) );
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s { %ff, %ff },\n", str, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] { %ff, %ff },\n", str, i, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] { %ff, %ff },\n", str, i, j, GFL_VECTOR_X, GFL_VECTOR_Y );
	}
};

#endif	/* GFL_VECTOR2_H */

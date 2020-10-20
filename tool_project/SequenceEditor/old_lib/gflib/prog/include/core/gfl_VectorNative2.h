//==============================================================================
/**
 * @file	gfl_VectorNative2.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/18, 16:26
 */
// =============================================================================

#if !defined( __GFL_VECTORNATIVE2_H__ )
#define	__GFL_VECTORNATIVE2_H__

// =============================================================================
// ���̃t�@�C���Ɋ܂܂��N���X�Q
// =============================================================================
/**
 *  �Q�̗v�f�����^�v���\����
 */
struct VectorStruct2 {
	f32 GFL_VECTOR_X;
	f32 GFL_VECTOR_Y;
};

/**
 * �l�C�e�B�u���߃N���X
 */

class VectorNative2 : public VectorStruct2 {
public:
    typedef VectorNative2 self_type; /**< �����̌^�ł� */
    typedef f32  value_type; /**< �v�f�̌^�ł� */
	static const s32 DIMENSION = 2;

	/**
	 * �f�t�H���g�R���X�g���N�^�@�l�͕s��
	 */
	VectorNative2( ) {
	}

	/**
	 * �R���X�g���N�^
	 * @param vp �ݒ肵�������������_�|�C���^
	 */
	explicit VectorNative2( const f32* vp ) {
		Set( *vp, *(vp + 1) );
	}

	/**
	 * �R���X�g���N�^
	 * @param vn�@�ݒ肵����VectorNative�N���X
	 */
	VectorNative2( const VectorNative2& vn ) {
		Set( vn.GFL_VECTOR_X, vn.GFL_VECTOR_Y );
	}

	/**
	 * �R���X�g���N�^
	 * @param fx�@�ݒ肵����X���W
	 * @param fy�@�ݒ肵����Y���W
	 */
	VectorNative2( f32 fx, f32 fy ) {
		Set( fx, fy );
	}

	/**
	 * �f�X�g���N�^�@�������Ȃ�
	 */
	~VectorNative2( ) {
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
	 */

	inline self_type operator-() const {
		return GetNeg( );
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
	 * @return ���Z���ꂽ�N���X Vector2f
	 */

	inline self_type operator/(const f32 f) const {
		self_type r;
		r.Div( *this, f );
		return r;
	}

	// -------------------------------------------------------------------------
	// Lerp
	// -------------------------------------------------------------------------

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  s �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	inline void Lerp( const self_type& t, const self_type& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_VECTOR_X = t.GFL_VECTOR_X * rate_1 + s.GFL_VECTOR_X * rate;
		GFL_VECTOR_Y = t.GFL_VECTOR_Y * rate_1 + s.GFL_VECTOR_Y * rate;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   ����
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	inline f32 Dot( const self_type& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Y * t.GFL_VECTOR_Y;
	}

	/**
	 *   �傫���̓�� LenSq�Ɠ����H
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline f32 LengthSquare( void ) const {
		return GFL_VECTOR_X * GFL_VECTOR_X + GFL_VECTOR_Y * GFL_VECTOR_Y;
	}

	/**
	 *   �傫��
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 Length( void ) const {
		return Math::FSqrt( LenSq( ) );
	}

	/**
	 *   �傫���̓��
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline f32 LenSq( void ) const {
		return LengthSquare( );
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
		Mul( Math::FRecip( Length( ) ) );
	}

	/**
	 *   �x�N�g���𐳋K�����܂��B ���K���Ɏ��s�����ꍇ�͎w�肳�ꂽ�x�N�g����ݒ肵�܂��B
	 * @param  
	 * @return ����
	 */

	inline void SafeNormalize( const self_type& def ) {
		const f32 div = Length( );
		if( div != 0.0f ){
			Mul( Math::FRecip( div ) );
		} else {
			Set( def.GFL_VECTOR_X, def.GFL_VECTOR_Y );
		}
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */
	inline f32 DistanceSquare( const self_type& t ) const {
		self_type r;
		r.Sub( *this, t );
		return r.LengthSquare( );
	}

	/**
	 * 2�̃x�N�g���̂��ꂼ��̐����̍ő�l����\�������x�N�g�����쐬���ݒ肵�܂��B
	 * @param l ���̓x�N�g��
	 * @param r ���̓x�N�g��
	 * @return this �Q��
	 */
	self_type& Maximize( const self_type& l, const self_type& r ) {
		Set( Math::Greater( l.GFL_VECTOR_X, r.GFL_VECTOR_X ), Math::Greater( l.GFL_VECTOR_Y, r.GFL_VECTOR_Y ) );
		return *this;
	}

	/**
	 * 2�̃x�N�g���̂��ꂼ��̐����̍ŏ��l����\�������x�N�g�����쐬���ݒ肵�܂��B
	 * @param l ���̓x�N�g��
	 * @param r ���̓x�N�g��
	 * @return this �Q��
	 */
	self_type& Minimize( const self_type& l, const self_type& r ) {
		Set( Math::Lesser( l.GFL_VECTOR_X, r.GFL_VECTOR_X ), Math::Lesser( l.GFL_VECTOR_Y, r.GFL_VECTOR_Y ) );
		return *this;
	}

	/**
	 *   �w�C�x������ݒ�
	 * @param  tx �ݒ�l
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	inline void Set( const f32 tx, const f32 ty ) {
		GFL_VECTOR_X = tx;
		GFL_VECTOR_Y = ty;
	}

	/**
	 * �R�s�[
	 * @param t�@�Q�ƌ�
	 */
	inline void Set( const self_type& t ) {
		Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y );
	}

	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	inline bool operator==(const self_type& t) const {
		return GFL_VECTOR_X == t.GFL_VECTOR_X && GFL_VECTOR_Y == t.GFL_VECTOR_Y;
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	inline bool operator!=(const self_type& t) const {
		return GFL_VECTOR_X != t.GFL_VECTOR_X || GFL_VECTOR_Y != t.GFL_VECTOR_Y;
	}

	/**
	 * �[���x�N�g�����H
	 * @return �^�@�O�x�N�g���̎��@�^�@�O�x�N�g������Ȃ���
	 */
	inline bool IsZero( ) {
		return *this == Zero( );
	}

	/**
	 * ���ׂĂ̗v�f�� 0 �̃x�N�g�����擾
	 * @return ���ׂĂ̗v�f�� 1 �̃x�N�g��
	 */
	static const self_type& Zero( ) {
		return static_cast<const self_type&> (s_Zero);
	}

	/**
	 * ���ׂĂ̗v�f�� 1 �̃x�N�g�����擾
	 * @return ���ׂĂ̗v�f�� 1 �̃x�N�g��
	 */
	static const self_type& One( ) {
		return static_cast<const self_type&> (s_One);
	}

	/**
	 * ��Ԃ��o��
	 * @param newline ���s����
	 * @param str �\��������
	 */
	void Report( bool newline = true, const c8* str = 0 ) {
		Debug::PrintConsole( "%s { %ff, %ff },", str, GFL_VECTOR_X, GFL_VECTOR_Y );
		if( newline ){
			Debug::PrintConsoleDirect("\n");
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
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X + s.GFL_VECTOR_X, t.GFL_VECTOR_Y + s.GFL_VECTOR_Y );
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const f32 tx, const f32 ty ) {
		GFL_VECTOR_X += tx;
		GFL_VECTOR_Y += ty;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const self_type& t, const f32 tx, const f32 ty ) {
		Set( t.GFL_VECTOR_X + tx, t.GFL_VECTOR_Y + ty );
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
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X - s.GFL_VECTOR_X, t.GFL_VECTOR_Y - s.GFL_VECTOR_Y );
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Sub( const f32 tx, const f32 ty ) {
		GFL_VECTOR_X -= tx;
		GFL_VECTOR_Y -= ty;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */


	inline void Sub( const self_type& t, const f32 tx, const f32 ty ) {
		Set( t.GFL_VECTOR_X - tx, t.GFL_VECTOR_Y - ty );
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
	}

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Mul( const f32 f, const self_type& t ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y );
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Mul( const self_type& t, const f32 f ) {
		Set( f * t.GFL_VECTOR_X, f * t.GFL_VECTOR_Y );
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
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const self_type& t, const self_type& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X, t.GFL_VECTOR_Y * s.GFL_VECTOR_Y );
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
		Set( f_1 * t.GFL_VECTOR_X, f_1 * t.GFL_VECTOR_Y );
	}

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  t �l�Q�[�g�������N���X
	 * @return ����
	 */

	inline void Neg( const self_type& t ) {
		Set( -t.GFL_VECTOR_X, -t.GFL_VECTOR_Y );
	}

	inline self_type GetNeg( void ) const {
		self_type ret;
		ret.Neg( *this );
		return ret;
	}

	static const VectorStruct2 s_Zero;
	static const VectorStruct2 s_One;
};

#endif	/* __GFL_VECTORNATIVE2_H__ */


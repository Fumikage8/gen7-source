//==============================================================================
/**
 * @file	gfl_Tuple4.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/24, 16:30
 */
// =============================================================================

//#if !defined( __GFL_TUPLE4_H__ )
//#define	__GFL_TUPLE4_H__

/**
 * Vector4 �� Quaternion �̋��ʃ��\�b�h���`
 * template �ɂ���ƃC���e���Z���X�������Ȃ��̂Ń}�N����
 */

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
 *   �y�������擾
 * @param  ����
 * @return �y����
 */
inline f32 GetZ( void ) const {
	return GFL_VECTOR_Z;
}

/**
 *   �v�������擾
 * @param  ����
 * @return �v����
 */

inline f32 GetW( void ) const {
	return GFL_VECTOR_W;
}

/**
 *   �w�肳�ꂽ�������擾
 * @param  i �Y���� TX(0) ? TW(3)
 * @return �w�肳�ꂽ����
 */
inline f32 Get( const s32 i ) const {
	return ( &GFL_VECTOR_X)[i];
}

/**
 * f32 �Ƃ��ă|�C���^���擾
 * @return f32 �|�C���^
 */
inline f32* Get( void ) {
	return ( &GFL_VECTOR_X);
}

/**
 * const f32 �Ƃ��ă|�C���^���擾
 * @return const f32 �|�C���^
 */
inline const f32* Get( void ) const {
	return ( &GFL_VECTOR_X);
}

/**
 *   �w�����Q�Ƃ��擾
 * @param  ����
 * @return �w����
 */
inline f32& GetRefX( void ) {
	return GFL_VECTOR_X;
}

/**
 *   �x�����Q�Ƃ��擾
 * @param  ����
 * @return �x����
 */
inline f32& GetRefY( void ) {
	return GFL_VECTOR_Y;
}

/**
 *   Z�����Q�Ƃ��擾
 * @param  ����
 * @return Z����
 */
inline f32& GetRefZ( void ) {
	return GFL_VECTOR_Z;
}

/**
 *   W�����Q�Ƃ��擾
 * @param  ����
 * @return W����
 */
inline f32& GetRefW( void ) {
	return GFL_VECTOR_W;
}

/**
 *   �w�肳�ꂽ�����Q�Ƃ��擾
 * @param  i �Y���� TX(0) ? TZ(3)
 * @return �w�肳�ꂽ����
 */
inline f32& GetRef( const s32 i ) {
	return ( &GFL_VECTOR_X)[i];
}

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
 *   �y������ݒ�
 * @param  tz �ݒ�l
 * @return ����
 */
inline void SetZ( const f32 tz ) {
	GFL_VECTOR_Z = tz;
}

/**
 *   �v������ݒ�
 * @param  tw �ݒ�l
 * @return ����
 */
inline void SetW( const f32 tw ) {
	GFL_VECTOR_W = tw;
}

/**
 *   �w�C�x�C�y�C�v������ݒ�
 * @param  tx �ݒ�l
 * @param  ty �ݒ�l
 * @param  tz �ݒ�l
 * @param  tw �ݒ�l �w�肵�Ȃ��ꍇ�� 0.0f ������
 * @return ����
 */
inline void Set( const f32 tx, const f32 ty, const f32 tz, const f32 tw ) {
	GFL_VECTOR_X = tx;
	GFL_VECTOR_Y = ty;
	GFL_VECTOR_Z = tz;
	GFL_VECTOR_W = tw;
}

/**
 *   �w�肵��������ݒ�
 * @param  i �Y���� TX(0) ? TW(3)
 * @param  val �ݒ�l
 * @return ����
 */
inline void Set( const s32 i, const f32 val ) {
	(&GFL_VECTOR_X)[i] = val;
}

/**
 * XYZW���ꂼ��̐�����ݒ�
 * @param t�@����
 */
inline void Set( const parent_type& t ) {
	Set( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z, t.GFL_VECTOR_W );
}
/**
 *   �w�C�x�C�y�����ɂO��ݒ肵�v������ 1 ��ݒ肷��
 * @param  ����
 * @return ����
 */
void SetUnit( void );

/**
 *   �I�y���[�^�[ []
 * @param  i �Y���� TX(0) ? TW(3)
 * @return �w�肳�ꂽ����
 */
inline f32 operator [](s32 i) const {
	return Get( i );
}

/**
 *   �I�y���[�^�[ =
 * @param  t ������������̓N���X
 * @return this �̎Q��
 */
inline self_type& operator=(const parent_type& t) {
	Set( t );
	return *this;
}

/**
 *   �I�y���[�^�[ ==
 * @param  t ��r�������N���X
 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
 * @remark �t���[�g�덷���l��������r
 */
inline bool operator==(const parent_type& t) const {
	return IsSimilar( t );
}

/**
 *   �I�y���[�^�[ !=
 * @param  t ��r�������N���X
 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
 * @remark �t���[�g�덷���l��������r
 */
inline bool operator!=(const parent_type& t) const {
	return !IsSimilar( t );
}

/**
 *   �t���@�w�C�x�C�y�����̋t����ݒ肷��
 * @param  ����
 * @return ����
 */
inline void Recip( void ) {
	Set( Math::FRecip( GFL_VECTOR_X ),
		 Math::FRecip( GFL_VECTOR_Y ),
		 Math::FRecip( GFL_VECTOR_Z ),
		 Math::FRecip( GFL_VECTOR_W ) );
}

/**
 *   �t���@�s�̂w�C�x�C�y�����̋t����ݒ肷��
 * @param  t �t���ɂ������N���X
 * @return ����
 */
inline void Recip( const parent_type& t ) {
	Set( Math::FRecip( t.GFL_VECTOR_X ), Math::FRecip( t.GFL_VECTOR_Y ), Math::FRecip( t.GFL_VECTOR_Z ), Math::FRecip( t.GFL_VECTOR_W ) );
}

/**
 *   �e���������Ă��邩�H
 * @param  t �ΏۃN���X
 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
 * @return �e�����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
 */
inline bool IsSimilar( const parent_type& t, const f32 epsilon = Math::EPSILON ) const {
	return Math::FAbs( t.GFL_VECTOR_X - GFL_VECTOR_X ) <= epsilon &&
		Math::FAbs( t.GFL_VECTOR_Y - GFL_VECTOR_Y ) <= epsilon &&
		Math::FAbs( t.GFL_VECTOR_Z - GFL_VECTOR_Z ) <= epsilon &&
		Math::FAbs( t.GFL_VECTOR_W - GFL_VECTOR_W ) <= epsilon;
}

/**
 * �قƂ�ǂO�̃x�N�g�����H
 * @param epsilon �덷
 * @return �^�@�قƂ�ǂO�@�^�@�قƂ�ǂO�ł͂Ȃ�
 */
inline bool IsAlmostZero( const f32 epsilon = Math::EPSILON ) const {
	return Math::IsAlmostZero( GFL_VECTOR_X, epsilon ) &&
		Math::IsAlmostZero( GFL_VECTOR_Y, epsilon ) &&
		Math::IsAlmostZero( GFL_VECTOR_Z, epsilon ) &&
		Math::IsAlmostZero( GFL_VECTOR_W, epsilon );
}

/**
 *   �e�����̐�Βl��ݒ肷��
 * @param  ����
 * @return ����
 */
inline void Abs( void ) {
	Set( Math::FAbs( GFL_VECTOR_X ), Math::FAbs( GFL_VECTOR_Y ), Math::FAbs( GFL_VECTOR_Z ), Math::FAbs( GFL_VECTOR_W ) );
}

/**
 *   t �̊e�����̐�Βl��ݒ肷��
 * @param  t ���̓N���X
 * @return ����
 */
inline void Abs( const VectorNative4& t ) {
	Set( Math::FAbs( t.GFL_VECTOR_X ), Math::FAbs( t.GFL_VECTOR_Y ), Math::FAbs( t.GFL_VECTOR_Z ), Math::FAbs( t.GFL_VECTOR_W ) );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @return ����
 */
inline void PrintConsole( void ) const {
	Debug::PrintConsole( "{ %ff, %ff, %ff, %ff },\n",
						 GFL_VECTOR_X, GFL_VECTOR_Y,
						 GFL_VECTOR_Z, GFL_VECTOR_W );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @param  str ������
 * @return ����
 */
inline void PrintConsole( const c8 * const str ) const {
	Debug::PrintConsole( "%s ", str );
	PrintConsole( );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @param  str ������
 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @return ����
 */
inline void PrintConsole( const c8 * const str, const s32 i ) const {
	Debug::PrintConsole( "%s[%d] ", str, i );
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
	Debug::PrintConsole( "%s[%d][%d] ", str, i, j );
	PrintConsole( );
}

/**
 *   �R���\�[���Ɋe������x���ŏo��
 * @return ����
 */
inline void PrintConsoleDegree( void ) const {
	Debug::PrintConsole( "{ %ff, %ff, %ff },\n",
						 gfl::core::Math::RadianToDegree( GFL_VECTOR_X ),
						 gfl::core::Math::RadianToDegree( GFL_VECTOR_Y ),
						 gfl::core::Math::RadianToDegree( GFL_VECTOR_Z ) );
}

/**
 *   �R���\�[���Ɋe������x���ŏo��
 * @param  str ������
 * @return ����
 */
inline void PrintConsoleDegree( const c8 * const str ) const {
	Debug::PrintConsole( "%s ", str );
	PrintConsoleDegree( );
}

/**
 *   �R���\�[���Ɋe������x���ŏo��
 * @param  str ������
 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @return ����
 */
inline void PrintConsoleDegree( const c8 * const str, const s32 i ) const {
	Debug::PrintConsole( "%s[%d]", str, i );
	PrintConsoleDegree( );
}

/**
 *   �R���\�[���Ɋe������x���ŏo��
 * @param  str ������
 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @return ����
 */
inline void PrintConsoleDegree( const c8 * const str, const s32 i, const s32 j ) const {
	Debug::PrintConsole( "%s[%d][%d]", str, i, j );
	PrintConsoleDegree( );
}

#ifdef GFL_PLATFORM_3DS

/**
 *   ���Z
 * @param  t ���Z�������N���X�̎Q��
 * @return ����
 * @remark �e�������m�����Z����
 */
inline void Add( const parent_type& t ) {
	*this += t;
}

/**
 *   ���Z
 * @param  t ���Z�������N���X�̎Q��
 * @param  s ���Z�������N���X�̎Q��
 * @return ����
 * @remark �e�������m�����Z����
 */
inline void Add( const parent_type& t, const parent_type& s ) {
	*this = t + s;
}

/**
 *   ���Z
 * @param  t ���Z�������N���X�̎Q��
 * @return ����
 * @remark �e�������m�����Z����
 */
inline void Sub( const parent_type& t ) {
	*this -= t;
}

/**
 *   ���Z
 * @param  t ���Z�������N���X�̎Q��
 * @param  s ���Z�������N���X�̎Q��
 * @return ����
 * @remark �e�������m�����Z����
 */
inline void Sub( const parent_type& t, const parent_type& s ) {
	*this = t - s;
}

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
 * @param  t ��Z�������N���X�̎Q��
 * @return ����
 * @remark f ��t �̊e��������Z���Ď��g�ɐݒ肷��
 */
inline void Mul( const f32 f, const parent_type& t ) {
	*this = f * t;
}

/**
 *   ��Z
 * @param  t ��Z�������N���X�̎Q��
 * @param  f ��Z�������l=
 * @return ����
 * @remark f ��t �̊e��������Z���Ď��g�ɐݒ肷��
 */
inline void Mul( const parent_type& t, const f32 f ) {
	*this = f * t;
}

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
inline void Div( const parent_type& t, const f32 f ) {
	*this = t / f;
}
#endif

//#endif	/* __GFL_TUPLE4_H__ */

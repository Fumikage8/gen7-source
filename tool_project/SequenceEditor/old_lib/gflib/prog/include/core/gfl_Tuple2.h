//==============================================================================
/**
 * @file	gfl_Tuple2.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/31, 15:30
 */
// =============================================================================

#if !defined( __GFL_TUPLE2_H__ )
#define	__GFL_TUPLE2_H__

/**
 *  �Q�̗v�f�����^�v���\����
 */

template<typename T>
struct TupleStr2 {
	T GFL_VECTOR_X;
	T GFL_VECTOR_Y;
};

/**
 *  �Q�̗v�f�����^�v���e���v���[�g�N���X
 */

template<typename T>
class Tuple2 : public TupleStr2<T> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple2( void ) {
	}

	/**
	 *   �w�C�x������ݒ肷��R���X�^���N�^�[
	 * @param  tx �ݒ�l
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	Tuple2( const T tx, const T ty ) {
		Set( tx, ty );
	}

	/**
	 *   Tuple2 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple2�N���X
	 * @return ����
	 */

	Tuple2( const Tuple2<T>& t ) {
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

	inline T GetX( void ) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) ? TY(2)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T Get( const s32 i ) const {
		return &(GFL_TEMPLATE_PARAMETER GFL_VECTOR_X)[i];
	}

	/**
	 *   �ϐ��̃|�C���^�[���擾
	 * @param  ����
	 * @return �ϐ��̃|�C���^�[
	 */

	inline T* Get( void ) {
		return &GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	inline const T* Get( void ) const {
		return &GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) ? TY(2)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T& GetRef( const s32 i ) const {
		return &(GFL_TEMPLATE_PARAMETER GFL_VECTOR_X)[i];
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   �w������ݒ�
	 * @param  tx �ݒ�l
	 * @return ����
	 */

	inline void SetX( const T tx ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = tx;
	}

	/**
	 *   �x������ݒ�
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	inline void SetY( const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = ty;
	}

	/**
	 *   �w�肵��������ݒ�
	 * @param  i �Y���� TX(0) ? TY(2)
	 * @param  val �ݒ�l
	 * @return ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline void SetElem( const s32 i, const T val ) {
		&(GFL_TEMPLATE_PARAMETER GFL_VECTOR_X)[i] = val;
	}

	/**
	 *   �w�C�x������ݒ�
	 * @param  tx �ݒ�l
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	inline void Set( const T tx, const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = tx;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = ty;
	}

	/**
	 *   Tuple2 ��ݒ�
	 * @param  t �ݒ肵���� Tuple �̎Q��
	 * @return ����
	 */


	inline void Set( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = t.GFL_VECTOR_Y;
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = static_cast<T> (0);
	}

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */


	inline void Add( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X += t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y += t.GFL_VECTOR_Y;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const Tuple2<T>& t, const Tuple2<T>& s ) {
		SetX( t.GFL_VECTOR_X + s.GFL_VECTOR_X );
		SetY( t.GFL_VECTOR_Y + s.GFL_VECTOR_Y );
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const T tx, const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X += tx;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y += ty;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */


	inline void Add( const Tuple2<T>& t, const T tx, const T ty ) {
		SetX( t.GFL_VECTOR_X + tx );
		SetY( t.GFL_VECTOR_Y + ty );
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


	inline void Sub( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X -= t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y -= t.GFL_VECTOR_Y;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const Tuple2<T>& t, const Tuple2<T>& s ) {
		SetX( t.GFL_VECTOR_X - s.GFL_VECTOR_X );
		SetY( t.GFL_VECTOR_Y - s.GFL_VECTOR_Y );
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Sub( const T tx, const T ty ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X -= tx;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y -= ty;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */


	inline void Sub( const Tuple2<T>& t, const T tx, const T ty ) {
		SetX( t.GFL_VECTOR_X - tx );
		SetY( t.GFL_VECTOR_Y - ty );
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

	inline void Mul( const T f ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X *= f;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y *= f;
	}

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */


	inline void Mul( const T f, const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = f * t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = f * t.GFL_VECTOR_Y;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */


	inline void Mul( const Tuple2<T>& t, const T f ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = f * t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = f * t.GFL_VECTOR_Y;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */


	inline void Mul( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const Tuple2<T>& t, const Tuple2<T>& s ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = t.GFL_VECTOR_X * s.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = t.GFL_VECTOR_Y * s.GFL_VECTOR_Y;
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
		return (f32) (GFL_TEMPLATE_PARAMETER GFL_VECTOR_X * GFL_TEMPLATE_PARAMETER GFL_VECTOR_X +
			GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y * GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y);
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
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */
	inline f32 Distance2( const Tuple2<T>& t ) const {
		Tuple2<T> r;
		r.Sub( *this, t );
		return r.Magnitude2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 Distance( const Tuple2<T>& t ) const {
		return Math::FSqrt( Distance2( t ) );
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
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = -GFL_TEMPLATE_PARAMETER GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = -GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y;
	}

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  t �l�Q�[�g�������N���X
	 * @return ����
	 */


	inline void Neg( const Tuple2<T>& t ) {
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = -t.GFL_VECTOR_X;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = -t.GFL_VECTOR_Y;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   ����
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */


	inline T Dot( const Tuple2<T>& t ) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y * t.GFL_VECTOR_Y;
	}

	// -------------------------------------------------------------------------
	// Cross �񎟌��ɊO�ς͂���܂���
	// -------------------------------------------------------------------------

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

	inline void Lerp( const Tuple2<T>& t, const Tuple2<T>& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_X = t.GFL_VECTOR_X * rate_1 + s.GFL_VECTOR_X * rate;
		GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y = t.GFL_VECTOR_Y * rate_1 + s.GFL_VECTOR_Y * rate;
	}

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	inline void Lerp( const Tuple2<T>& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   �I�y���[�^�[ []
	 * @param  i �Y���� TX(0) ? TZ(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T operator [](s32 i) const {
		return Get( i );
	}

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ��������� Tuple2<T>
	 * @return this �̎Q��
	 */

	inline Tuple2<T> & operator=(const Tuple2<T>& t) {
		Set( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ +=
	 * @param  t ���Z�������N���X
	 * @return this �̎Q��
	 */

	inline Tuple2<T> & operator+=(const Tuple2<T>& t) {
		Add( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ -=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	inline Tuple2<T> & operator-=(const Tuple2<T>& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  f ��Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple2<T> & operator*=(const T f) {
		Mul( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	inline Tuple2<T> & operator*=(const Tuple2<T>& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	inline bool operator==(const Tuple2<T>& t) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X == t.GFL_VECTOR_X && GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y == t.GFL_VECTOR_Y;
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	inline bool operator!=(const Tuple2<T>& t) const {
		return GFL_TEMPLATE_PARAMETER GFL_VECTOR_X != t.GFL_VECTOR_X || GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y != t.GFL_VECTOR_Y;
	}

	/**
	 *   �I�y���[�^�[ +
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple2<T>
	 */

	inline Tuple2<T> operator+(const Tuple2<T>& t) const {
		Tuple2<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple2<T>
	 */

	inline Tuple2<T> operator-(const Tuple2<T>& t) const {
		Tuple2<T> r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  f �ώZ�������l
	 * @return �ώZ���ꂽ�N���X Tuple2<T>
	 */

	inline Tuple2<T> operator*(const T f) const {
		Tuple2<T> r;
		r.Mul( *this, f );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  t �ώZ�������N���X
	 * @return �ώZ���ꂽ�N���X Tuple2<T>
	 */

	inline Tuple2<T> operator*(const Tuple2<T>& t) const {
		Tuple2<T> r;
		r.Mul( *this, t );
		return r;
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
		Debug::PrintConsole( "%s %d %d\n", str, GFL_TEMPLATE_PARAMETER GFL_VECTOR_X, GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %d %d\n", str, i, GFL_TEMPLATE_PARAMETER GFL_VECTOR_X, GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER GFL_VECTOR_X, GFL_TEMPLATE_PARAMETER GFL_VECTOR_Y );
	}
};

/**
 *  �Q�̗v�f�����^�v���N���X
 */

class Tuple2f : public Tuple2<f32> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple2f( void ) {
	}

	/**
	 *   �w�C�x������ݒ肷��R���X�^���N�^�[
	 * @param  tx �ݒ�l
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	Tuple2f( const f32 tx, const f32 ty ) : Tuple2<f32>::Tuple2( tx, ty ) {
	}

	/**
	 *   Tuple2f ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple2f�N���X
	 * @return ����
	 */


	Tuple2f( const Tuple2f& t ) : Tuple2<f32>::Tuple2( t ) {
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
		SetX( Math::FRecip( GFL_VECTOR_X ) );
		SetY( Math::FRecip( GFL_VECTOR_Y ) );
	}

	/**
	 *   �t���@�s�̂w�C�x�����̋t����ݒ肷��
	 * @param  t �t���ɂ������N���X
	 * @return ����
	 */


	inline void Recip( const Tuple2f& t ) {
		SetX( Math::FRecip( t.GFL_VECTOR_X ) );
		SetY( Math::FRecip( t.GFL_VECTOR_Y ) );
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

	inline void Div( const Tuple2f& t, const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		SetX( f_1 * t.GFL_VECTOR_X );
		SetY( f_1 * t.GFL_VECTOR_Y );
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
		Mul( MagnitudeRecip( ) );
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	inline void Normalize( const Tuple2f& t ) {
		Mul( t, MagnitudeRecip( ) );
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

	inline f32 NormalizeRet( const Tuple2f& t ) {
		f32 ret = Magnitude( );
		Mul( t, Math::FRecip( ret ) );
		return ret;
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

	inline bool IsSimilar( const Tuple2f& t, const f32 epsilon = Math::EPSILON ) const {
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
		SetX( Math::FAbs( GFL_VECTOR_X ) );
		SetY( Math::FAbs( GFL_VECTOR_Y ) );
	}

	/**
	 *   t �̊e�����̐�Βl��ݒ肷��
	 * @param  t ���̓N���X
	 * @return ����
	 */

	inline void Abs( const Tuple2f& t ) {
		SetX( Math::FAbs( t.GFL_VECTOR_X ) );
		SetY( Math::FAbs( t.GFL_VECTOR_Y ) );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------
	using Tuple2<f32>::operator=;
	using Tuple2<f32>::operator+;
	using Tuple2<f32>::operator+=;
	using Tuple2<f32>::operator-;
	using Tuple2<f32>::operator-=;
	using Tuple2<f32>::operator*;
	using Tuple2<f32>::operator*=;

	/**
	 *   �I�y���[�^�[ /=
	 * @param  f ���Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple2f & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ /
	 * @param  f ���Z�������l
	 * @return ���Z���ꂽ�N���X Tuple2f
	 */

	inline Tuple2f operator/(const f32 f) const {
		Tuple2f r;
		r.Div( *this, f );
		return r;
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
		Debug::PrintConsole( "%s %f %f\n", str, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %f %f\n", str, i, GFL_VECTOR_X, GFL_VECTOR_Y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %f %f\n", str, i, j, GFL_VECTOR_X, GFL_VECTOR_Y );
	}
};

#endif	/* __GFL_TUPLE2_H__ */


/* 
 * File:   gfl_Vector3.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_VECTOR3_H
#define	GFL_VECTOR3_H

/**
 *  3�̗v�f�����N���X
 */

class Vector3 : public VectorNative3 {
public:
	static const VectorStruct3 s_X1;
	static const VectorStruct3 s_Y1;
	static const VectorStruct3 s_Z1;
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Vector3( void ) {
	}

	/**
	 *   �w�C�x������ݒ肷��R���X�^���N�^�[
	 * @param  tx �ݒ�l
	 * @param  ty �ݒ�l
	 * @return ����
	 */

	Vector3( const f32 tx, const f32 ty, const f32 tz ) : VectorNative3( tx, ty, tz ) {
	}
	Vector3( const f32 tx, const f32 ty, const f32 tz, const f32) : VectorNative3( tx, ty, tz ) {
	}

	/**
	 *   Vector3 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Vector2�N���X
	 * @return ����
	 */

	explicit Vector3( const VectorNative3& t ) : VectorNative3( t ) {
	}

	Vector3( const Vector3& t ) : VectorNative3( t ) {
	}

	Vector3( const VectorNative4& t ) {
		Set( t );
	}

	/**
	 * �R���X�g���N�^
	 * @param vp �ݒ肵�������������_�|�C���^
	 */
	explicit Vector3( const f32* vp ) {
		Set( *(const VectorNative3*)vp );
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
	 *   �y�������擾
	 * @param  ����
	 * @return �y����
	 */

	inline f32 GetZ( void ) const {
		return GFL_VECTOR_Z;
	}

	/**
	 *   W�������擾����ӂ������
	 * @param  ����
	 * @return �O
	 */

	inline f32 GetW( void ) const {
		return 0.0f;
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
	 *   Z�������擾
	 * @param  ����
	 * @return Z����
	 */

	inline f32& GetRefZ( void ) {
		return GFL_VECTOR_Z;
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
	 * (0,0,0) ���擾
     * @return (0,0,0)
     */
	static const Vector3& GetZero( void ) {
		return static_cast<const Vector3&>(Zero());
	}

	/**
	 * (1,0,0) ���擾
     * @return (1,0,0)
     */
	static const Vector3& GetX1( void ) {
		return static_cast<const Vector3&>(s_X1);
	}

	/**
	 * (0,1,0) ���擾
     * @return (0,1,0)
     */
	static const Vector3& GetY1( void ) {
		return static_cast<const Vector3&>(s_Y1);
	}

	/**
	 * (0,0,1) ���擾
     * @return (0,0,1)
     */
	static const Vector3& GetZ1( void ) {
		return static_cast<const Vector3&>(s_Z1);
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
	 *   �y������ݒ�
	 * @param  tz �ݒ�l
	 * @return ����
	 */

	inline void SetZ( const f32 tz ) {
		GFL_VECTOR_Z = tz;
	}

	/**
	 *   �v������ݒ肷��ӂ������_�~�[�֐�
	 * @param  T �ݒ�l
	 * @return ����
	 */

	inline void SetW( const f32 ) {
	}

	/**
	 *   �w�肵��������ݒ�
	 * @param  i �Y���� TX(0) �` TY(2)
	 * @param  val �ݒ�l
	 * @return ����
	 */

	inline void Set( const s32 i, const f32 val ) {
		(&GFL_VECTOR_X)[i] = val;
	}

	/**
	 * �ݒ�
	 * @param tx�@X����
	 * @param ty�@Y����
	 * @param tz�@Z����
	 * @param �_�~�[
	 */
	inline void Set( const f32 tx, const f32 ty, const f32 tz, const f32 ) {
		Set( tx, ty, tz );
	}

	/**
	 * VectorNative4 ��ݒ�
	 * @param t �ݒ肵����VectorNative4
	 */
	inline void Set( const VectorNative4& t ) {
		Set( reinterpret_cast<const VectorNative3&> (t) );
	}

	/**
	 * (1,0,0) ��ݒ�
     */
	void SetX1( void ) {
		Set( static_cast<const Vector3&>(s_X1) );
	}

	/**
	 * (0,1,0) ��ݒ�
     */
	void SetY1( void ) {
		Set( static_cast<const Vector3&>(s_Y1) );
	}

	/**
	 * (0,0,1) ��ݒ�
     */
	void SetZ1( void ) {
		Set( static_cast<const Vector3&>(s_Z1) );
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 */

	inline void SetZero( void ) {
		Set( Zero( ) );
	}
	
	/**
	 * �[����ݒ� SetZero �Ɠ���
     */
	inline void SetUnit(void){
		SetZero();
	}


	/**
	 *   �O��
	 * @param  s �O�ς����߂����N���X
	 * @return ����
	 */

	inline void Cross( const VectorNative3& s ) {
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
	// Calc Normal
	// -------------------------------------------------------------------------

	void CalcNormal( const Vector3& v0, const Vector3& v1, const Vector3& v2 );

	// =========================================================================
	// using
	// =========================================================================
	using VectorNative3::Set;
	using VectorNative3::Lerp;
	using VectorNative3::operator+;
	using VectorNative3::operator*;
	using VectorNative3::Normalize;
	using VectorNative3::Cross;

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

	inline void Add( const VectorNative3& t ) {
		*this += t;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const VectorNative3& t, const VectorNative3& s ) {
		*this = t + s;
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const f32 tx, const f32 ty, const f32 tz ) {
		Add( *this, VectorNative3( tx, ty, tz ) );
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const VectorNative3& t, const f32 tx, const f32 ty, const f32 tz ) {
		Add( t, VectorNative3( tx, ty, tz ) );
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

	inline void Sub( const VectorNative3& t ) {
		*this -= t;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const VectorNative3& t, const VectorNative3& s ) {
		*this = t - s;
	}

	/**
	 *   ���Z
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */

	inline void Sub( const f32 tx, const f32 ty, const f32 tz ) {
		Sub( *this, VectorNative3( tx, ty, tz ) );
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  tx ���Z�������w�l
	 * @param  ty ���Z�������x�l
	 * @return ����
	 */


	inline void Sub( const VectorNative3& t, const f32 tx, const f32 ty, const f32 tz ) {
		Sub( t, VectorNative3( tx, ty, tz ) );
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

	inline void Mul( const f32 f, const VectorNative3& t ) {
		*this = f * t;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Mul( const VectorNative3& t, const f32 f ) {
		*this = f * t;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative3& t ) {
		GFL_VECTOR_X *= t.GFL_VECTOR_X;
		GFL_VECTOR_Y *= t.GFL_VECTOR_Y;
		GFL_VECTOR_Z *= t.GFL_VECTOR_Z;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	inline void Mul( const VectorNative3& t, const VectorNative3& s ) {
		Set( t.GFL_VECTOR_X * s.GFL_VECTOR_X,
			 t.GFL_VECTOR_Y * s.GFL_VECTOR_Y,
			 t.GFL_VECTOR_Z * s.GFL_VECTOR_Z );
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

	inline void Div( const VectorNative3& t, const f32 f ) {
		*this = t / f;
	}
#endif
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

	inline void Neg( const VectorNative3& t ) {
		*this = -t;
	}

	inline VectorNative3 GetNeg( void ) {
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
	 * W �̕����𔽓]������ӂ�
     */
	inline void NegW(void){
//		GFL_VECTOR_W = -GFL_VECTOR_W;
	}

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */
	inline void Lerp( const VectorNative3& t, const f32 rate ) {
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
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */
	inline f32 Distance2( const VectorNative3& t ) const {
		return DistanceSquare( t );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 Distance( const VectorNative3& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	inline f32 DistanceXZ2( const VectorNative3& t ) const {
		return Vector3( *this -t ).MagnitudeXZ2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 DistanceXZ( const VectorNative3& t ) const {
		return Math::FSqrt( DistanceXZ2( t ) );
	}

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	inline f32 DistanceXY2( const VectorNative3& t ) const {
		return Vector3( *this -t ).MagnitudeXY2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 DistanceXY( const VectorNative3& t ) const {
		return Math::FSqrt( DistanceXY2( t ) );
	}

	/**
	 *   ����XZ
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	inline f32 DotXZ( const VectorNative3& t ) const {
		return GFL_VECTOR_X * t.GFL_VECTOR_X + GFL_VECTOR_Z * t.GFL_VECTOR_Z;
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

	inline f32& operator [](s32 i){
		return GetRef( i );
	}
	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	inline bool operator==(const self_type& t) const {
		return IsSimilar(t);
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	inline bool operator!=(const self_type& t) const {
		return !IsSimilar(t);
	}

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ��������� VectorNative3
	 * @return this �̎Q��
	 */

	inline Vector3 & operator=(const VectorNative3& t) {
		Set( t );
		return *this;
	}
	
	/**
	 *   �I�y���[�^�[ +
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X VectorNative3
	 */

	inline Vector3 operator+(const VectorNative3& t) const {
		return Vector3(VectorNative3::operator+(t));
	}
	
	/**
	 *   �I�y���[�^�[ *
	 * @param  t �ώZ�������N���X
	 * @return �ώZ���ꂽ�N���X VectorNative3
	 */

	inline Vector3 operator*(const VectorNative3& t) const {
		Vector3 r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ ^=
	 * @param  t �O�ς����߂����N���X
	 * @return this �̎Q��
	 */

	inline Vector3 & operator^=(const VectorNative3& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ^
	 * @param  t �O�ς����߂����N���X
	 * @return �O�ς��ݒ肳�ꂽ�N���X VectorNative3
	 */

	inline Vector3 operator^(const VectorNative3& t) const {
		Vector3 r;
		r.Cross( *this, t );
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
		Set( Math::FRecip( GFL_VECTOR_X ), Math::FRecip( GFL_VECTOR_Y ), Math::FRecip( GFL_VECTOR_Z ) );
	}

	/**
	 *   �t���@�s�̂w�C�x�����̋t����ݒ肷��
	 * @param  t �t���ɂ������N���X
	 * @return ����
	 */


	inline void Recip( const VectorNative3& t ) {
		Set( Math::FRecip( t.GFL_VECTOR_X ), Math::FRecip( t.GFL_VECTOR_Y ), Math::FRecip( t.GFL_VECTOR_Z ) );
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	inline void Normalize( const VectorNative3& t ) {
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

	inline f32 NormalizeRet( const VectorNative3& t ) {
		Set(t);
		return NormalizeRet();
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

	inline bool IsSimilar( const f32 tx, const f32 ty, const f32 tz, const f32 epsilon = Math::EPSILON ) const {
		return Math::FAbs( tx - GFL_VECTOR_X ) <= epsilon &&
			Math::FAbs( ty - GFL_VECTOR_Y ) <= epsilon &&
			Math::FAbs( tz - GFL_VECTOR_Z ) <= epsilon;
	}

	/**
	 *   �e���������Ă��邩�H
	 * @param  t �ΏۃN���X
	 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
	 * @return �e�����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
	 */

	inline bool IsSimilar( const VectorNative3& t, const f32 epsilon = Math::EPSILON ) const {
		return IsSimilar(t.GFL_VECTOR_X,t.GFL_VECTOR_Y,t.GFL_VECTOR_Z,epsilon);
	}

	/**
	 * �S�������قƂ�ǂO�Ȃ̂��H
	 * @param epsilon�@�덷�l
	 * @return�@�^ �قƂ�ǂO�̎�
	 */
	inline bool IsAlmostZero( const f32 epsilon = Math::EPSILON ) const {
		return Math::IsAlmostZero( GFL_VECTOR_X, epsilon ) &&
			Math::IsAlmostZero( GFL_VECTOR_Y, epsilon ) &&
			Math::IsAlmostZero( GFL_VECTOR_Z, epsilon );
	}

	/**
	 * �O���X�O�x�͈̔͂ɓ����Ă��邩
	 * @return �^�@�����Ă���
	 */
	inline bool IsFrontDegree90( void ) {
		return gfl::core::Math::IsFrontDegree90( GFL_VECTOR_X, GFL_VECTOR_Z );
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
		Set( Math::FAbs( GFL_VECTOR_X ), Math::FAbs( GFL_VECTOR_Y ), Math::FAbs( GFL_VECTOR_Z ) );
	}

	/**
	 *   t �̊e�����̐�Βl��ݒ肷��
	 * @param  t ���̓N���X
	 * @return ����
	 */

	inline void Abs( const VectorNative3& t ) {
		Set( Math::FAbs( t.GFL_VECTOR_X ), Math::FAbs( t.GFL_VECTOR_Y ), Math::FAbs( t.GFL_VECTOR_Z ) );
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

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( void ) const {
		Debug::PrintConsole( "{ %ff, %ff, %ff },\n", GFL_VECTOR_X, GFL_VECTOR_Y, GFL_VECTOR_Z );
	}
	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s ", str );
		PrintConsole();
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
	 * �R���\�[���Ɋp�x�ŏo��
	 */
	inline void PrintConsoleDegree( void ) const {
		Debug::PrintConsole( "{ %ff, %ff, %ff },\n", gfl::core::Math::RadianToDegree( GFL_VECTOR_X ), gfl::core::Math::RadianToDegree( GFL_VECTOR_Y ),
							 gfl::core::Math::RadianToDegree( GFL_VECTOR_Z ) );
	}

	/**
	 * �R���\�[���Ɋp�x�ŏo��
	 * @param str�@������
	 */
	inline void PrintConsoleDegree( const c8 * const str ) const {
		Debug::PrintConsole( "%s ", str );
		PrintConsoleDegree( );
	}

	/**
	 *   �R���\�[���Ɋp�x�ŏo��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */
	inline void PrintConsoleDegree( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d]", str, i );
		PrintConsoleDegree( );
	}

	/**
	 *   �R���\�[���Ɋp�x�ŏo��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */
	inline void PrintConsoleDegree( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d]", str, i, j );
		PrintConsoleDegree( );
	}
#ifdef GFL_PLATFORM_3DS // �����͂�����폜���܂��I�I
	void ToVEC3( gfl::math::VEC3* tag ){
		tag->Set(*this);
	}
	void FromVEC3(const gfl::math::VEC3& src){
		Set( (const nn::math::VEC3&)src );
	}
#endif
};
#endif	/* GFL_VECTOR3_H */


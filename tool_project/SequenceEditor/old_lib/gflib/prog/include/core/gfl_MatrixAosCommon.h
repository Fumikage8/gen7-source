//==============================================================================
/**
 * @file	gfl_MatrixAosCommon.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/12/20, 16:58
 */
// =============================================================================
#if 0 // 
//#include "gfl_MatrixCommon.h"

// -------------------------------------------------------------------------
// Apply
// -------------------------------------------------------------------------

/**
 *   �p�x�X�P�[�������ƃx�N�g���̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @param  v1 ���̓x�N�g��
 * @return ����
 */


void Apply( VectorNative* v0, const VectorNative& v1 ) const {
	f32 x = v1.GFL_VECTOR_X;
	f32 y = v1.GFL_VECTOR_Y;
	f32 z = v1.GFL_VECTOR_Z;

	v0->Set( Get( 0, 0 ) * x + Get( 1, 0 ) * y + Get( 2, 0 ) * z,
			 Get( 0, 1 ) * x + Get( 1, 1 ) * y + Get( 2, 1 ) * z,
			 Get( 0, 2 ) * x + Get( 1, 2 ) * y + Get( 2, 2 ) * z );
}

/**
 *   �p�x�X�P�[�������Ƃw�����̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @param  val �w����
 * @return ����
 */


inline void ApplyX( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 0 ) * val,
			 Get( 0, 1 ) * val,
			 Get( 0, 2 ) * val );
}

/**
 *   �p�x�X�P�[�������Ƃw�P�D�O�������̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @return ����
 */


inline void ApplyX1( VectorNative* v0 ) const {
	v0->Set( Get( 0, 0 ), Get( 0, 1 ), Get( 0, 2 ) );
}

/**
 *   �p�x�X�P�[�������Ƃx�����̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @param  val �x����
 * @return ����
 */


inline void ApplyY( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 1, 0 ) * val, Get( 1, 1 ) * val, Get( 1, 2 ) * val );
}

/**
 *   �p�x�X�P�[�������Ƃx�P�D�O�������̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @return ����
 */


inline void ApplyY1( VectorNative* v0 ) const {
	v0->Set( Get( 1, 0 ), Get( 1, 1 ), Get( 1, 2 ) );
}

/**
 *   �p�x�X�P�[�������Ƃy�����̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @param  val �y����
 * @return ����
 */


inline void ApplyZ( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 2, 0 ) * val, Get( 2, 1 ) * val, Get( 2, 2 ) * val );
}

/**
 *   �p�x�X�P�[�������Ƃy�P�D�O�������̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @return ����
 */


inline void ApplyZ1( VectorNative* v0 ) const {
	v0->Set( Get( 2, 0 ), Get( 2, 1 ), Get( 2, 2 ) );
}

/**
 *   �p�x�X�P�[�������ƃx�N�g���̐ώZ
 * @param  v0 �o�̓x�N�g��
 * @param  v1 ���̓x�N�g��
 * @return ����
 */


void ApplyTranspose( VectorNative* v0, const VectorNative& v1 ) const {
	f32 x = v1.GFL_VECTOR_X;
	f32 y = v1.GFL_VECTOR_Y;
	f32 z = v1.GFL_VECTOR_Z;

	v0->Set( Get( 0, 0 ) * x + Get( 0, 1 ) * y + Get( 0, 2 ) * z,
			 Get( 1, 0 ) * x + Get( 1, 1 ) * y + Get( 1, 2 ) * z,
			 Get( 2, 0 ) * x + Get( 2, 1 ) * y + Get( 2, 2 ) * z );
}

// -------------------------------------------------------------------------
// Trans
// -------------------------------------------------------------------------

/**
 *   �w���W�ړ�
 * @param  val �w���W
 * @return ����
 */

inline void TransX( const f32 val ) {
	GetTransRefX( ) += Get( 0, 0 ) * val;
	GetTransRefY( ) += Get( 0, 1 ) * val;
	GetTransRefZ( ) += Get( 0, 2 ) * val;
}

/**
 *   �x���W�ړ�
 * @param  val �x���W
 * @return ����
 */

inline void TransY( const f32 val ) {
	GetTransRefX( ) += Get( 1, 0 ) * val;
	GetTransRefY( ) += Get( 1, 1 ) * val;
	GetTransRefZ( ) += Get( 1, 2 ) * val;
}

/**
 *   �y���W�ړ�
 * @param  val �y���W
 * @return ����
 */

inline void TransZ( const f32 val ) {
	GetTransRefX( ) += Get( 2, 0 ) * val;
	GetTransRefY( ) += Get( 2, 1 ) * val;
	GetTransRefZ( ) += Get( 2, 2 ) * val;
}

/**
 *   �w���W���ړ������x�N�g�����擾
 * @param  v0 �o�̓x�N�g��
 * @param  val �w���W
 * @return ����
 */


inline void TransX( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 0, 0 ) * val + GetTransX( ),
			 Get( 0, 1 ) * val + GetTransY( ),
			 Get( 0, 2 ) * val + GetTransZ( ) );
}

/**
 *   �x�W���ړ������x�N�g�����擾
 * @param  v0 �o�̓x�N�g��
 * @param  val �x���W
 * @return ����
 */


inline void TransY( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 1, 0 ) * val + GetTransX( ),
			 Get( 1, 1 ) * val + GetTransY( ),
			 Get( 1, 2 ) * val + GetTransZ( ) );
}

/**
 *   �y���W���ړ������x�N�g�����擾
 * @param  v0 �o�̓x�N�g��
 * @param  val �y���W
 * @return ����
 */


inline void TransZ( VectorNative* v0, const f32 val ) const {
	v0->Set( Get( 2, 0 ) * val + GetTransX( ),
			 Get( 2, 1 ) * val + GetTransY( ),
			 Get( 2, 2 ) * val + GetTransZ( ) );
}

/**
 *   �w���W�������擾
 * @param  ����
 * @return �w���W
 */

inline f32 GetTransX( void ) const {
	return Get( 3, TX );
}

/**
 *   �x���W�������擾
 * @param  ����
 * @return �x���W
 */

inline f32 GetTransY( void ) const {
	return Get( 3, TY );
}

/**
 *   �y���W�������擾
 * @param  ����
 * @return �y���W
 */

inline f32 GetTransZ( void ) const {
	return Get( 3, TZ );
}

/**
 *   �w���W�������擾
 * @param  ����
 * @return �w���W
 */

inline f32& GetTransRefX( void ) {
	return GetRef( 3, TX );
}

/**
 *   �x���W�������擾
 * @param  ����
 * @return �x���W
 */

inline f32& GetTransRefY( void ) {
	return GetRef( 3, TY );
}

/**
 *   �y���W�������擾
 * @param  ����
 * @return �y���W
 */

inline f32& GetTransRefZ( void ) {
	return GetRef( 3, TZ );
}

/**
 *   ���W������ݒ�
 * @param  x �w���W
 * @return ����
 */

inline void SetTransX( const f32 x ) {
	Set( 3, TX, x );
}

/**
 *   ���W������ݒ�
 * @param  y �x���W
 * @return ����
 */

inline void SetTransY( const f32 y ) {
	Set( 3, TY, y );
}

/**
 *   ���W������ݒ�
 * @param  z �y���W
 * @return ����
 */

inline void SetTransZ( const f32 z ) {
	Set( 3, TZ, z );
}

/**
 *   ���W������ݒ�
 * @param  v ���̓x�N�g��
 * @return ����
 */

inline void SetTrans( const VectorNative3& v ) {
	SetRow( 3, v );
}

/**
 *   ���W������ݒ�
 * @param  v ���̓x�N�g��
 * @return ����
 */

inline void SetTrans( const VectorNative4& v ) {
	SetRow( 3, v );
}

/**
 *   �O���[�o�����W�����̍s�񂩂�݂����[�J�����W�ɕϊ�
 * @param  loc ���[�J�����W
 * @param  pos �O���[�o�����W
 * @return ����
 */


void GlobalToLocal33( VectorNative* loc, const VectorNative& pos ) const {
	const f32 x = pos.GFL_VECTOR_X;
	const f32 y = pos.GFL_VECTOR_Y;
	const f32 z = pos.GFL_VECTOR_Z;
	loc->Set( Get( 0, 0 ) * x + Get( 0, 1 ) * y + Get( 0, 2 ) * z,
			 Get( 1, 0 ) * x + Get( 1, 1 ) * y + Get( 1, 2 ) * z,
			 Get( 2, 0 ) * x + Get( 2, 1 ) * y + Get( 2, 2 ) * z );
}

/**
 *   �O���[�o�����W�����̍s�񂩂�݂����[�J�����W�ɕϊ�
 * @param  loc ���[�J�����W
 * @param  pos �O���[�o�����W
 * @return ����
 */
void GlobalToLocal33Scaled( VectorNative* loc, const VectorNative& pos ) const {
	self_type mat( GetInverse33( ) );
	const f32 x = pos.GFL_VECTOR_X;
	const f32 y = pos.GFL_VECTOR_Y;
	const f32 z = pos.GFL_VECTOR_Z;
	loc->Set( mat.Get( 0, 0 ) * x + Get( 1, 0 ) * y + Get( 2, 0 ) * z,
			 mat.Get( 0, 1 ) * x + Get( 1, 1 ) * y + Get( 2, 1 ) * z,
			 mat.Get( 0, 2 ) * x + Get( 1, 2 ) * y + Get( 2, 2 ) * z );
}

/**
 *   �O���[�o�����W�����̍s�񂩂�݂����[�J���w���W�����߂�
 * @param  glo �O���[�o�����W
 * @return ���[�J���w���W
 */


f32 GlobalToLocalX( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 0, 0 ) * pos.GetX( ) + Get( 0, 1 ) * pos.GetY( ) + Get( 0, 2 ) * pos.GetZ( );
}

/**
 *   �O���[�o�����W�����̍s�񂩂�݂����[�J���x���W�����߂�
 * @param  glo �O���[�o�����W
 * @return ���[�J���x���W
 */


f32 GlobalToLocalY( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 1, 0 ) * pos.GetX( ) + Get( 1, 1 ) * pos.GetY( ) + Get( 1, 2 ) * pos.GetZ( );
}

/**
 *   �O���[�o�����W�����̍s�񂩂�݂����[�J���y���W�����߂�
 * @param  glo �O���[�o�����W
 * @return ���[�J���y���W
 */


f32 GlobalToLocalZ( const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	return Get( 2, 0 ) * pos.GetX( ) + Get( 2, 1 ) * pos.GetY( ) + Get( 2, 2 ) * pos.GetZ( );
}

// -------------------------------------------------------------------------
// �}�g���b�N�X�̌����ƃx�N�g�������������������Ă��邩�H
// -------------------------------------------------------------------------

bool IsVectorSameDirection( const f32 x, const f32 z ) const {
	return ( 0.0f <= Get( 2, 0 ) * x + Get( 2, 2 ) * z);
}

bool IsVectorSameSide( const f32 x, const f32 z ) const {
	return ( 0.0f <= Get( 0, 0 ) * x + Get( 0, 2 ) * z);
}

bool IsVectorSameDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return Math::IsFrontDegree90( x, z );
}

bool IsVectorOppositeDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return Math::IsBackDegree90( x, z );
}

// -----------------------------------------------------------------------------
// operator
// -----------------------------------------------------------------------------

/**
 *   ���
 * @param  m ���͍s��
 * @return ����
 */

inline self_type& operator=(const self_type& m) {
	Set( m );
	return *this;
}

/**
 *   ��r
 * @param  m0 ��r�Ώۍs��
 * @return �e�������S�Ċ��S�Ɉ�v�����ꍇ�͐^�A���͋U
 */
bool operator==(const self_type& m0) {
	return GetRow( 0 ) == m0.GetRow( 0 ) &&
		GetRow( 1 ) == m0.GetRow( 1 ) &&
		GetRow( 2 ) == m0.GetRow( 2 ) &&
		GetRow( 3 ) == m0.GetRow( 3 );
}

/**
 *   ��r
 * @param  m0 ��r�Ώۍs��
 * @return �e�������S�Ċ��S�Ɉ�v�����ꍇ�͋U�A���͐^
 */
bool operator!=(const self_type& m0) {
	return GetRow( 0 ) != m0.GetRow( 0 ) ||
		GetRow( 1 ) != m0.GetRow( 1 ) ||
		GetRow( 2 ) != m0.GetRow( 2 ) ||
		GetRow( 3 ) != m0.GetRow( 3 );
}

/**
 *   �덷�͈͂�^���Ĕ�r
 * @param  m0 ��r�Ώۍs��
 * @param  rot_scale_range �R�w�R�����̌덷�͈�
 * @param  trans_range ���W�����̌덷�͈�
 * @return �e�����̍����덷�͈͓��̏ꍇ�͐^�A���͋U
 */
bool IsSimilar( const self_type& m0, const f32 rot_scale_range, const f32 trans_range ) const {
	return GetRow( 0 ).IsSimilar( m0.GetRow( 0 ), rot_scale_range ) &&
		GetRow( 1 ).IsSimilar( m0.GetRow( 1 ), rot_scale_range ) &&
		GetRow( 2 ).IsSimilar( m0.GetRow( 2 ), rot_scale_range ) &&
		GetRow( 3 ).IsSimilar( m0.GetRow( 3 ), trans_range );
}

#if GFL_USE_SOA

/**
 *   ���
 * @param  m ���͍s��
 * @return ����
 */
inline self_type& operator=(const MatrixSoaNative34& m) {
	FromSOA( m );
	return *this;
}

/**
 *   ���
 * @param  m ���͍s��
 * @return ����
 */
inline self_type& operator=(const MatrixSoaNative44& m) {
	FromSOA( m );
	return *this;
}
#endif

/**
 * �������������ɃX�P�[����������
 * @param scl�@�X�P�[���l
 */
void Mul( const f32 scl ) {
	Scale(scl);
}

// -----------------------------------------------------------------------------
// Scale
// -----------------------------------------------------------------------------

/**
 *   �w�ɃX�P�[����������
 * @param  x �w���ɂ�����X�P�[��
 * @return ����
 */

inline void ScaleX( const f32 x ) {
	GetRef( 0, 0 ) *= x;
	GetRef( 0, 1 ) *= x;
	GetRef( 0, 2 ) *= x;
}

/**
 * �s���X�X�P�[�����擾
 * @return �X�P�[���l
 */
inline f32 GetScaleX( void ) const {
	return GetRow( 0 ).Magnitude( );
}

/**
 *   �x�ɃX�P�[����������
 * @param  y �x���ɂ�����X�P�[��
 * @return ����
 */

inline void ScaleY( const f32 y ) {
	GetRef( 1, 0 ) *= y;
	GetRef( 1, 1 ) *= y;
	GetRef( 1, 2 ) *= y;
}

/**
 * �s���Y�X�P�[�����擾
 * @return �X�P�[���l
 */
inline f32 GetScaleY( void ) const {
	return GetRow( 1 ).Magnitude( );
}

/**
 *   �y�ɃX�P�[����������
 * @param  z �y���ɂ�����X�P�[��
 * @return ����
 */

inline void ScaleZ( const f32 z ) {
	GetRef( 2, 0 ) *= z;
	GetRef( 2, 1 ) *= z;
	GetRef( 2, 2 ) *= z;
}

/**
 * �s���Z�X�P�[�����擾
 * @return �X�P�[���l
 */
inline f32 GetScaleZ( void ) const {
	return GetRow( 2 ).Magnitude( );
}

/**
 * ���K��
 */
inline void Normalize( VectorNative* scl = 0 );

/**
 *   �w�ɃX�P�[�����t���炩����
 * @param  x �w���ɂ�����X�P�[��
 * @return ����
 */

inline void ScaleX_Reverse( const f32 x ) {
	GetRef( 0, 0 ) *= x;
	GetRef( 1, 0 ) *= x;
	GetRef( 2, 0 ) *= x;
}

/**
 *   �x�ɃX�P�[�����t���炩����
 * @param  y �x���ɂ�����X�P�[��
 * @return ����
 */

inline void ScaleY_Reverse( const f32 y ) {
	GetRef( 0, 1 ) *= y;
	GetRef( 1, 1 ) *= y;
	GetRef( 2, 1 ) *= y;
}

/**
 *   �y�ɃX�P�[�����t���炩����
 * @param  z �y���ɂ�����X�P�[��
 * @return ����
 */

inline void ScaleZ_Reverse( const f32 z ) {
	GetRef( 0, 2 ) *= z;
	GetRef( 1, 2 ) *= z;
	GetRef( 2, 2 ) *= z;
}

/**
 * �x�N�g�����s��̍������X�O�x�ȓ���
 * @param vx�@�x�N�g��������
 * @param vz�@�x�N�g��������
 * @return �^�@�����X�O�x�ȓ��@�U�@����ȊO
 */
inline bool IsVectorLeftDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsLeftDegree90( x, z );
}

/**
 * �x�N�g�����s��̉E�����X�O�x�ȓ���
 * @param vx�@�x�N�g��������
 * @param vz�@�x�N�g��������
 * @return �^�@�E���X�O�x�ȓ��@�U�@����ȊO
 */
inline bool IsVectorRightDirectionDegree90( const f32 vx, const f32 vz ) const {
	const f32 x = Get( 0, 0 ) * vx + Get( 0, 2 ) * vz;
	const f32 z = Get( 2, 0 ) * vx + Get( 2, 2 ) * vz;
	return gfl::core::Math::IsRightDegree90( x, z );
}

#endif

//==============================================================================
/**
 * @file	gfl_MatrixCommon.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/12/20, 16:40
 */
// =============================================================================
// ����͕�����C���N���[�h�����

/**
 * f32 �Ƃ��ă|�C���^���擾
 * @return f32 �|�C���^
 */
inline f32* Get( void ) {
	return GFL_MTX_ARRAY1D;
}

/**
 * const f32 �Ƃ��ă|�C���^���擾
 * @return const f32 �|�C���^
 */
inline const f32* Get( void ) const {
	return GFL_MTX_ARRAY1D;
}

/**
 *   �w�肳�ꂽ�������擾
 * @param  idx �Y����
 * @return �w�肳�ꂽ�����̒l
 */
inline f32 Get( const s32 idx ) const {
	return GFL_MTX_ARRAY1D[idx];
}

/**
 *   �w�肳�ꂽ�������擾
 * @param  i �Y����
 * @param  j �Y����
 * @return �w�肳�ꂽ�����̒l
 */
inline f32 Get( const s32 i, const s32 j ) const {
	return GFL_MTX_ARRAY2D[i][j];
}

/**
 *   �w�肳�ꂽ�������擾
 * @param  i �Y����
 * @param  j �Y����
 * @return �w�肳�ꂽ�����̒l
 */
inline f32& GetRef( const s32 i, const s32 j ) {
	return GFL_MTX_ARRAY2D[i][j];
}

/**
 *   �w�肳�ꂽ�������擾
 * @param  i �Y����
 * @return �w�肳�ꂽ�����̒l
 */
inline f32 operator() (const s32 i) const {
	return GFL_MTX_ARRAY1D[i];
}

/**
 *   �w�肳�ꂽ�������擾
 * @param  i �Y����
 * @param  j �Y����
 * @return �w�肳�ꂽ�����̒l
 */
inline f32 operator() (const s32 i, const s32 j) const {
	return GFL_MTX_ARRAY2D[i][j];
}

/**
 *   �w�肳�ꂽ�����̑Ίp�������擾
 * @param  i �Y���� 0 �` 3
 * @param  j �Y���� 0 �` 3
 * @return �w�肳�ꂽ�����̒l
 */

inline f32 GetDiagonal( const s32 i, const s32 j ) const {
	return GFL_MTX_ARRAY2D[j][i];
}

/**
 * �v�f��ݒ�
 * @param i�@�Y��
 * @param val�@�l
 */
inline void Set( const s32 i, const f32 val ) {
	GFL_MTX_ARRAY1D[i] = val;
}

/**
 * �v�f��ݒ�
 * @param i�@�Y��
 * @param j�@�Y��
 * @param val�@�l
 */
inline void Set( const s32 i, const s32 j, const f32 val ) {
	GFL_MTX_ARRAY2D[i][j] = val;
}

/**
 *   Matrix �R�w�R������ݒ�
 * @param  m0  �v�f @param  m1  �v�f @param  m2  �v�f @param  m3  �v�f @param  m4 �v�f
 * @param  m5  �v�f @param  m6  �v�f @param  m7  �v�f @param  m8  �v�f
 * @return ����
 */
void Set33( f32 m0, f32 m1, f32 m2,
			f32 m3, f32 m4, f32 m5,
			f32 m6, f32 m7, f32 m8 ) {
	GFL_MTX_00 = m0;
	GFL_MTX_01 = m1;
	GFL_MTX_02 = m2;
	GFL_MTX_10 = m3;
	GFL_MTX_11 = m4;
	GFL_MTX_12 = m5;
	GFL_MTX_20 = m6;
	GFL_MTX_21 = m7;
	GFL_MTX_22 = m8;
}

/**
 *   MATRIX_SOA_BASE �̂R�w�R������ݒ�
 * @param  mat  �s��
 * @return ����
 */
template<typename T>
void Set33( const T& mat ) {
	SetRow( 0, reinterpret_cast<const VectorNative3&> (mat.GetRow( 0 )) );
	SetRow( 1, reinterpret_cast<const VectorNative3&> (mat.GetRow( 1 )) );
	SetRow( 2, reinterpret_cast<const VectorNative3&> (mat.GetRow( 2 )) );
}

/**
 * �s��̍��W�����Ƃ̉��Z
 * @param v0�@�o�̓x�N�g���N���X
 * @param v1�@���Z������̓x�N�g���N���X
 */
inline void Add( VectorNative* v0, const VectorNative& v1 ) const {
	v0->GFL_VECTOR_X = v1.GFL_VECTOR_X + GetTransX( );
	v0->GFL_VECTOR_Y = v1.GFL_VECTOR_Y + GetTransY( );
	v0->GFL_VECTOR_Z = v1.GFL_VECTOR_Z + GetTransZ( );
}

/**
 * �s��̍��W�����Ƃ̉��Z
 * @param v0�@�o�̓x�N�g���\����
 */
inline void Add( VectorNative* v0 ) const {
	v0->GFL_VECTOR_X += GetTransX( );
	v0->GFL_VECTOR_Y += GetTransY( );
	v0->GFL_VECTOR_Z += GetTransZ( );
	/* How should I treat W */
}

/**
 * �s��̍��W�����Ƃ̉��Z
 * @param v0�@�o�̓x�N�g���\����
 */
inline void Add( const VectorNative& v0 ) {
	GetTransRefX( ) += v0.GFL_VECTOR_X;
	GetTransRefY( ) += v0.GFL_VECTOR_Y;
	GetTransRefZ( ) += v0.GFL_VECTOR_Z;
	/* How should I treat W */
}

/**
 * �x�N�g�����p�x�����Ɋ|�������̂��v�Z
 * @param v0�@���o�̓x�N�g���N���X�|�C���^
 */
inline void Apply( VectorNative* v0 ) const {
	Apply( v0, *v0 );
}

/**
 * ���W�ړ�
 * @param v0�@���̓x�N�g���N���X
 */
inline void Trans( const VectorNative& v0 ) {
	Vector v;
	Apply( &v, v0 );
	*this += v;
}

/**
 * ���W�ړ�
 * @param x�@������
 * @param y�@������
 * @param z�@������
 */
inline void Trans( const f32 x, const f32 y, const f32 z ) {
	Vector v( x, y, z );
	Apply( &v, v );
	*this += v;
}

/**
 * ���W�ړ�
 * @param v0�@�o�̓x�N�g���N���X�|�C���^
 * @param v1�@���̓x�N�g���N���X
 * @remark �s��Ɍv�Z���ʂ͔��f����Ȃ�
 */
inline void Trans( VectorNative* v0, const VectorNative& v1 ) const {
	Apply( v0, v1 );
	/*		v0->SetW( 0.0f ); */
	*v0 += GetTrans( );
}

/**
 * ���W�ړ�
 * @param v0�@�o�̓x�N�g���N���X
 * @param x�@������
 * @param y�@������
 * @param z�@������
 * @remark �s��Ɍv�Z���ʂ͔��f����Ȃ�
 */
inline void Trans( VectorNative* v0, const f32 x, const f32 y, const f32 z ) const {
	Vector v( x, y, z );
	Trans( v0, v );
}

/**
 *   �s��̍��W������ݒ�
 * @param  mat ���͍s��
 * @return ����
 */

inline void SetTrans( const self_type& mat ) {
	SetTrans( mat.GetTrans( ) );
}

/**
 *   �R�w�R�̌�����q
 * @param  m0 ���͍s��
 * @return ������q
 */
inline f32 Determinant33( const self_type& m0 ) const {
	return m0.Determinant33( );
}

/**
 *   ������q
 * @param  m0 ���͍s��
 * @return ������q
 */
inline f32 Determinant( const self_type& m0 ) const {
	return m0.Determinant( );
}

/**
 *   �R�w�R�̋t�s��
 * @param  ����
 * @return ����
 * @remark ���W�����ɂ́i�O�C�O�C�O�C�P�j���ݒ肳���B\n
 * �X�P�[�����������ĂȂ��ꍇ�͓]�u�s��̎g�p�������߂��܂��B
 */

inline void Inverse33( void ) {
	Inverse33( *this );
}

/**
 * �t�s���ݒ�
 */
inline void Inverse( void ) {
	Inverse( *this );
}

// -----------------------------------------------------------------------------
// Transpose33
// -----------------------------------------------------------------------------

/**
 *   �p�x�����̓]�u�s���ݒ�
 * @param  ����
 * @return ����
 */
void Transpose33( void ) {
	Math::Swap( GFL_MTX_01, GFL_MTX_10 );
	Math::Swap( GFL_MTX_02, GFL_MTX_20 );
	Math::Swap( GFL_MTX_12, GFL_MTX_21 );
}

/**
 *   �p�x�����̓]�u�s���ݒ�
 * @param  m0 ���͍s��
 * @return ����
 */
void Transpose33( const self_type& m ) {
	GFL_MTX_00 = m.GFL_MTX_00;
	GFL_MTX_01 = m.GFL_MTX_10;
	GFL_MTX_02 = m.GFL_MTX_20;
	GFL_MTX_10 = m.GFL_MTX_01;
	GFL_MTX_11 = m.GFL_MTX_11;
	GFL_MTX_12 = m.GFL_MTX_21;
	GFL_MTX_20 = m.GFL_MTX_02;
	GFL_MTX_21 = m.GFL_MTX_12;
	GFL_MTX_22 = m.GFL_MTX_22;
}

/**
 *   �p�x�����̓]�u�s������߂�
 * @param  ����
 * @return �]�u���ꂽ�s��̎���
 */

self_type GetTranspose33( void ) const {
	self_type ret;
	ret.Transpose33( *this );
	return ret;
}

/**
 *   �]�u�s������߂�
 * @param  ����
 * @return �]�u���ꂽ�s��̎���
 */

self_type GetTranspose( void ) const {
	self_type ret;
	ret.Transpose( *this );
	return ret;
}

/**
 *   �p�x�����̓]�u�s������߂�
 * @param  ����
 * @return �]�u���ꂽ�s��̎���
 */

self_type GetInverse33( void ) const {
	self_type ret;
	ret.Inverse33( *this );
	return ret;
}

/**
 *   �]�u�s������߂�
 * @param  ����
 * @return �]�u���ꂽ�s��̎���
 */

self_type GetInverse( void ) const {
	self_type ret;
	ret.Inverse( *this );
	return ret;
}

/**
 * �O���[�o�����W�����[�J�����W�ɕϊ�
 * @param loc�@���[�J�����W�@�o�̓x�N�g���N���X�|�C���^
 * @param glo�@�O���[�o�����W�@���̓x�N�g��
 */
inline void GlobalToLocal( VectorNative* loc, const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	GlobalToLocal33( loc, pos );
}

/**
 * �O���[�o�����W�����[�J�����W�ɕϊ�
 * @param loc�@���[�J�����W�@�o�̓x�N�g���N���X�|�C���^
 * @param glo�@�O���[�o�����W�@���̓x�N�g��
 */
inline void GlobalToLocalScaled( VectorNative* loc, const VectorNative& glo ) const {
	Vector pos;
	pos.Sub( glo, GetTrans( ) );
	GlobalToLocal33Scaled( loc, pos );
}

/**
 *   �^�[�Q�b�g�|�W�V�����ɍ��������Ăy�x�w��]������
 * @param  tag �ڕW���W
 * @return �Ȃ�
 * @remark �w����␳���Ăx���������ێ�����
 */


bool AimZYX( const VectorNative& tag ) {
	if( AimZY( tag ) ){
		AimX( );
		return true;
	}
	return false;
}

/**
 * �x�N�g���ƍs��̌����̑O�������������
 * @param v�@�x�N�g��
 * @return �^�@���������@�U�@���Ε���
 */
inline bool IsVectorSameDirection( const VectorNative& v ) const {
	return IsVectorSameDirection( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
}

/**
 * �x�N�g���ƍs��̌����̍��E������������
 * @param v�@�x�N�g��
 * @return �^�@���������@�U�@���Ε���
 */
inline bool IsVectorSameSide( const VectorNative& v ) const {
	return IsVectorSameSide( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
}

/**
 * �x�N�g���ƍs��̌��������������X�O�x�ȓ���
 * @param v�@�x�N�g��
 * @return �^�@���������X�O�x�ȓ��@�U�@����ȊO
 */
inline bool IsVectorSameDirectionDegree90( const VectorNative& v ) const {
	return IsVectorSameDirectionDegree90( v.GFL_VECTOR_X, v.GFL_VECTOR_Z );
}

/**
 * ���W���s�񂩂�݂đO�ɂ��邩
 * @param pos�@�`�F�b�N���������W
 * @return �^�@�O�@�U�@���
 */
inline bool IsPositionFront( const VectorNative& pos ) const {
	return IsVectorSameDirection( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * ���W���s�񂩂�݂č��ɂ��邩
 * @param pos�@�`�F�b�N���������W
 * @return �^�@���@�U�@�E
 */
inline bool IsPositionLeft( const VectorNative& pos ) const {
	return IsVectorSameSide( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * ���W���s�񂩂�݂đO�X�O�x�ȓ��ɂ��邩
 * @param pos�@�`�F�b�N���������W
 * @return �^�@�O�X�O�x�ȓ��@�U�@����ȊO
 */
inline bool IsPositionFrontDegree90( const VectorNative& pos ) const {
	return IsVectorSameDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * ���W���s�񂩂�݂Č��X�O�x�ȓ��ɂ��邩
 * @param pos�@�`�F�b�N���������W
 * @return �^�@��X�O�x�ȓ��@�U�@����ȊO
 */
inline bool IsPositionBackDegree90( const VectorNative& pos ) const {
	return IsVectorOppositeDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * ���W���s�񂩂�݂č��X�O�x�ȓ��ɂ��邩
 * @param pos�@�`�F�b�N���������W
 * @return �^�@���X�O�x�ȓ��@�U�@����ȊO
 */
inline bool IsPositionLeftDegree90( const VectorNative& pos ) const {
	return IsVectorLeftDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 * ���W���s�񂩂�݂ĉE�X�O�x�ȓ��ɂ��邩
 * @param pos�@�`�F�b�N���������W
 * @return �^�@�E�X�O�x�ȓ��@�U�@����ȊO
 */
inline bool IsPositionRightDegree90( const VectorNative& pos ) const {
	return IsVectorRightDirectionDegree90( pos.GFL_VECTOR_X - GetTransX( ), pos.GFL_VECTOR_Z - GetTransZ( ) );
}

/**
 *   �s��̂w���ŉ�]�������p�x������ݒ�
 * @param  rad �p�x�i���W�A���j
 * @return ����
 * @remark ���W�����ɂ͉������Ȃ�
 */

inline void SetRotateX( const f32 rad ) {
	f32 s, c;
	Math::FSinCos( &s, &c, rad );
	SetRotateX( s, c );
}

/**
 * �s��ɂ���]���|����
 * @param m0�@���͍s��
 * @param rad�@���͊p�x���W�A��
 * @remark m0 �� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateX( const self_type& m0, const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateX( m0, s, c );
}

/**
 * �s��ɂ���]���|����
 * @param rad�@���͊p�x���W�A��
 * @remark ���݂̍s��� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateX( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateX( s, c );
}

/**
 * �s��ɂ���]���|����
 * @param rad�@���͊p�x���W�A��
 * @remark ���݂̍s��� rad �̉�]���t����|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateX_Reverse( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateX_Reverse( s, c );
}

/**
 * �s��ɂ���]��ݒ�
 * @param rad�@���͊p�x���W�A��
 * @remark ���̊p�x�����̓N���A����܂��B���W�����ɑ΂��Ă͉������܂���
 */
inline void SetRotateY( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	SetRotateY( s, c );
}

/**
 * �s��ɂ���]���|����
 * @param m0�@���͍s��
 * @param rad�@���͊p�x���W�A��
 * @remark m0 �� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateY( const self_type& m0, const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateY( m0, s, c );
}

/**
 * �s��ɂ���]���|����
 * @param rad�@���͊p�x���W�A��
 * @remark ���݂̍s��� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateY( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateY( s, c );
}

/**
 * �s��ɂ���]���|����
 * @param rad�@���͊p�x���W�A��
 * @remark ���݂̍s��� rad �̉�]���t����|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateY_Reverse( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateY_Reverse( s, c );
}

/**
 * �s��ɂ���]��ݒ�
 * @param rad�@���͊p�x���W�A��
 * @remark ���̊p�x�����̓N���A����܂��B���W�����ɑ΂��Ă͉������܂���
 */
inline void SetRotateZ( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	SetRotateZ( s, c );
}

/**
 * �s��ɂ���]���|����
 * @param m0�@���͍s��
 * @param rad�@���͊p�x���W�A��
 * @remark m0 �� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateZ( const self_type& m0, const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateZ( m0, s, c );
}

/**
 * �s��ɂ���]���|����
 * @param rad�@���͊p�x���W�A��
 * @remark ���݂̍s��� rad �̉�]���|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateZ( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateZ( s, c );
}

/**
 * �s��ɂ���]���|����
 * @param rad�@���͊p�x���W�A��
 * @remark ���݂̍s��� rad �̉�]���t����|�����킹�����̂�ݒ肵�܂��B
 */
inline void RotateZ_Reverse( const f32 rad ) {
	f32 s, c;
	gfl::core::Math::FSinCos( &s, &c, rad );
	RotateZ_Reverse( s, c );
}

/**
 * �s���ZYX�̏��Ԃŉ�]���|�������̂�ݒ�
 * @param x�@���͊p�x���W�A��
 * @param y�@���͊p�x���W�A��
 * @param z�@���͊p�x���W�A��
 */
inline void SetRotateZYX( const f32 x, const f32 y, const f32 z ) {
	SetRotateZ( z );
	RotateY( y );
	RotateX( x );
}

/**
 * �s���ZYX�̏��Ԃŉ�]���|�������̂�ݒ�
 * @param v�@���͊p�x���W�A���x�N�g���\����
 */
inline void SetRotateZYX( const VectorNative& v ) {
	SetRotateZYX( v.GFL_VECTOR_X, v.GFL_VECTOR_Y, v.GFL_VECTOR_Z );
}

/**
 * �s���ZYX�̏��Ԃŉ�]���|�������̂�ݒ�
 * @param v�@x,y,z �̏��Ԃœ��������������_�z��|�C���^
 */
inline void SetRotateZYX( const f32 v[] ) {
	SetRotateZYX( v[TX], v[TY], v[TZ] );
}

/**
 * �s���ZYX�̏��Ԃŉ�]���|����
 * @param x�@���͊p�x���W�A��
 * @param y�@���͊p�x���W�A��
 * @param z�@���͊p�x���W�A��
 */
inline void RotateZYX( const f32 x, const f32 y, const f32 z ) {
	RotateZ( z );
	RotateY( y );
	RotateX( x );
}

/**
 * �s���ZYX�̏��Ԃŉ�]���|����
 * @param v�@���̓x�N�g���\����
 */
inline void RotateZYX( const VectorNative& v ) {
	RotateZYX( v.GFL_VECTOR_X, v.GFL_VECTOR_Y, v.GFL_VECTOR_Z );
}

/**
 * �s���ZYX�̏��Ԃŉ�]���|����
 * @param v�@x,y,z �̏��Ԃœ��������������_�z��|�C���^
 */
inline void RotateZYX( const f32 v[] ) {
	RotateZYX( v[TX], v[TY], v[TZ] );
}

/**
 * �X�P�[����ݒ肷��
 * @param scl�@�X�P�[���l
 * @remark ���̊p�x�����̓N���A�����B���W�����͕ω��Ȃ�
 */
inline void SetScale( const f32 scl ) {
	SetScale( scl, scl, scl );
}

/**
 * �X�P�[����ݒ肷��
 * @param t�@�X�P�[���l�x�N�g��
 * @remark ���̊p�x�����̓N���A�����B���W�����͕ω��Ȃ�
 */
inline void SetScale( const VectorNative& t ) {
	SetScale( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
}

/**
 * �X�P�[���l���|����
 * @param scl�@�X�P�[���l
 * @remark �����������S�Ăɓ������X�P�[���l���|����
 */
inline void Scale( const f32 scl ) {
	ScaleX( scl );
	ScaleY( scl );
	ScaleZ( scl );
}

/**
 * �X�P�[���l���|����
 * @param x�@�������X�P�[��
 * @param y�@�������X�P�[��
 * @param z�@�������X�P�[��
 */
inline void Scale( const f32 x, const f32 y, const f32 z ) {
	ScaleX( x );
	ScaleY( y );
	ScaleZ( z );
}

/**
 * �X�P�[���l���|����
 * @param t�@���̓x�N�g���\����
 */
inline void Scale( const VectorNative& t ) {
	Scale( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
}

/**
 * �X�P�[�����̎擾
 * @param t �X�P�[���i�[�N���X�|�C���^
 */

inline void GetScale( VectorNative* t ) const {
	t->Set( GetScaleX( ), GetScaleY( ), GetScaleZ( ) );
}

/**
 * �X�P�[���l���t����|����
 * @param scl�@�X�P�[���l
 * @remark x,y,z�����S�Ăɓ����� scl ���t����|����
 */
inline void Scale_Reverse( const f32 scl ) {
	ScaleX_Reverse( scl );
	ScaleY_Reverse( scl );
	ScaleZ_Reverse( scl );
}

/**
 * �X�P�[���l���t����|����
 * @param x�@���X�P�[���l
 * @param y�@���X�P�[���l
 * @param z�@���X�P�[���l
 */
inline void Scale_Reverse( const f32 x, const f32 y, const f32 z ) {
	ScaleX_Reverse( x );
	ScaleY_Reverse( y );
	ScaleZ_Reverse( z );
}

/**
 * �X�P�[���l���t����|����
 * @param t�@���̓x�N�g���\����
 */
inline void Scale_Reverse( const VectorNative& t ) {
	Scale_Reverse( t.GFL_VECTOR_X, t.GFL_VECTOR_Y, t.GFL_VECTOR_Z );
}

/**
 * �X�P�[���l�̋t�����|����
 * @param scl�@�X�P�[���l
 * @remark �����������S�Ăɓ������X�P�[���l���|����
 */
inline void ScaleRecip( const f32 scl ) {
	const f32 r_scl = gfl::core::Math::FRecip( scl );
	ScaleX( r_scl );
	ScaleY( r_scl );
	ScaleZ( r_scl );
}

/**
 * �X�P�[���l�̋t�����|����
 * @param t�@���̓x�N�g���\����
 */
inline void ScaleRecip( const VectorNative& t ) {
	Scale( gfl::core::Math::FRecip( t.GFL_VECTOR_X ), gfl::core::Math::FRecip( t.GFL_VECTOR_Y ), gfl::core::Math::FRecip( t.GFL_VECTOR_Z ) );
}

/**
 *   ���K��
 * @param src ���͍s��
 * @param  scl �X�P�[���l�x�N�g��
 * @return �Ȃ�
 */
void Normalize( const self_type& src, VectorNative* scl = 0 ) {
	Set( src );
	Normalize( scl );
}

/**
 *   ���K��
 * @param src ���͍s��
 * @param  scl �X�P�[���l�x�N�g��
 * @return ���K�����ꂽ�s��C���X�^���X
 */
self_type GetNormalizedMatrix( VectorNative* scl = 0 ) {
	self_type mat( *this );
	mat.Normalize( scl );
	return mat;
}

/**
 *   ���W�������擾
 * @param  v ���̓x�N�g��
 * @return ���W
 */

inline void GetTrans( VectorNative* v ) const {
	v->Set( GetTrans( ) );
}

/**
 *   �p�x���s�񂩂�擾
 * @param  v �o�̓��W�A���̃x�N�g��
 * @return ����
 * @remark w �͕s��B�y�x�w�̏��ŉ񂵂��ꍇ�̊p�x
 */

void GetRadian( VectorNative* v ) const {
	GetRadian( &v->GFL_VECTOR_X, &v->GFL_VECTOR_Y, &v->GFL_VECTOR_Z );
}

/**
 *   �p�x���s�񂩂�擾
 * @param  v �o�̓��W�A���̃x�N�g��
 * @return ����
 * @remark w �͕s��B�x�y�w�̏��ŉ񂵂��ꍇ�̊p�x
 */
void GetRadianYXZ( VectorNative* v ) const {
	GetRadianYXZ( &v->GFL_VECTOR_X, &v->GFL_VECTOR_Y, &v->GFL_VECTOR_Z );
}

/**
 * �N�����v����@�K�v�Ȃ����c
 */
void Clamp( ) {
	Math::ClampAbs( &GFL_MTX_00, 1.0f );
	Math::ClampAbs( &GFL_MTX_01, 1.0f );
	Math::ClampAbs( &GFL_MTX_02, 1.0f );
	Math::ClampAbs( &GFL_MTX_10, 1.0f );
	Math::ClampAbs( &GFL_MTX_11, 1.0f );
	Math::ClampAbs( &GFL_MTX_12, 1.0f );
	Math::ClampAbs( &GFL_MTX_20, 1.0f );
	Math::ClampAbs( &GFL_MTX_21, 1.0f );
	Math::ClampAbs( &GFL_MTX_22, 1.0f );
}

/**
 * ���W�l�̉��Z
 * @param v�@���̓x�N�g��
 * @remark �P���ɍ��W�����Z����̂ŁA��]�����͉e�����Ȃ��B\n
 * ��]���l�������ړ��̏ꍇ�� Trans ���g�p���Ă��������B
 */
inline void operator+=(const VectorStruct3& v) {
	GetTransRefX( ) += v.GFL_VECTOR_X;
	GetTransRefY( ) += v.GFL_VECTOR_Y;
	GetTransRefZ( ) += v.GFL_VECTOR_Z;
}

/**
 * �s��̍����擾
 * @param m�@��ƂȂ�s��
 * @return ���͍s����ǂꂾ����]�������玩���̍s��ɂȂ邩�̍s��
 * @remark �P���Ȉ����Z�łȂ��Athis = m * (�Ԃ�l)�ƂȂ�s����v�Z����
 */
inline self_type operator-(const self_type& m) const {
	self_type mat( GetTranspose33( ) );
	mat.Mul33( m );
	return mat;
}

/**
 * �s��̐�
 * @param m0�@���͍s��
 * @return �Q��
 * @remark this = this * m0
 */
inline self_type operator*(const self_type& m0) const {
	self_type mat;
	mat.Mul( *this, m0 );
	return mat;
}

/**
 * �s��̐�
 * @param m0�@���͍s��
 * @remark this = this * m0
 */
inline void Mul( const parent_type& m0 ) {
	Mul( *this, (const self_type&) m0 );
}

/**
 * �������������ɃX�P�[����������
 * @param sclx�@���X�P�[���l
 * @param scly�@���X�P�[���l
 * @param sclz�@���X�P�[���l
 */
void Mul( const f32 sclx, const f32 scly, const f32 sclz ) {
	ScaleX( sclx );
	ScaleY( scly );
	ScaleZ( sclz );
}

/**
 * �������������ɃX�P�[����������
 * @param scl�@�X�P�[���x�N�g��
 */
void Mul( const VectorNative& scl ) {
	ScaleX( scl.GFL_VECTOR_X );
	ScaleY( scl.GFL_VECTOR_Y );
	ScaleZ( scl.GFL_VECTOR_Z );
}

/**
 * �p�x�����݂̂Ɋ|���Z������
 * @param m0�@���͍s��
 */
inline void Mul33( const self_type& m0 ) {
	Mul33( *this, m0 );
}

/**
 * �s��⊮
 * @param m0�@�^�[�Q�b�g�s��
 * @param rate�@�����@�O�@�Ŏ������g�@�P�Ń^�[�Q�b�g�s��
 * @return �Q��
 * @remark this = this * (1-rate) + m0 * rate
 */
inline self_type& Slerp( const self_type& m0, const f32 rate ) {
	return Slerp( *this, m0, rate );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @param  str ������
 * @return ����
 */

inline void PrintConsole( const c8 * const str ) const {
	Debug::PrintConsole( "%s = \n", str );
	PrintConsole( );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @param  str ������
 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @return ����
 */

inline void PrintConsole( const c8 * const str, const s32 i ) const {
	Debug::PrintConsole( "%s[%d] = \n", str, i );
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
	Debug::PrintConsole( "%s[%d][%d] = \n", str, i, j );
	PrintConsole( );
}

/* �S���ʂɂ��Ă���������SOA34 SOA44 �ŃC���X�^���X���]�v�ɂł����Ⴄ�� */

/**
 *   �R���\�[���Ɋe�������o��
 * @param  ����
 * @return ����
 */
void PrintConsole33( void ) const {
	Debug::PrintConsole( "%ff, %ff, %ff,\n%ff, %ff, %ff,\n%ff, %ff, %ff\n",
						 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02,
						 GFL_MTX_10, GFL_MTX_11, GFL_MTX_12,
						 GFL_MTX_20, GFL_MTX_21, GFL_MTX_22 );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @param  str ������
 * @return ����
 */

inline void PrintConsole33( const c8 * const str ) const {
	Debug::PrintConsole( "%s = \n", str );
	PrintConsole33( );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @param  str ������
 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @return ����
 */

inline void PrintConsole33( const c8 * const str, const s32 i ) const {
	Debug::PrintConsole( "%s[%d] = \n", str, i );
	PrintConsole33( );
}

/**
 *   �R���\�[���Ɋe�������o��
 * @param  str ������
 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
 * @return ����
 */

inline void PrintConsole33( const c8 * const str, const s32 i, const s32 j ) const {
	Debug::PrintConsole( "%s[%d][%d] = \n", str, i, j );
	PrintConsole33( );
}


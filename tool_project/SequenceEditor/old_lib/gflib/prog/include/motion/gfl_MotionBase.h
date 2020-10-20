/*
 * File:   GFL_MOTIONBASE.h
 * Author: s-kimura
 *
 * Created on 2010/07/06, 18:12
 */

#ifndef GFL_MOTIONBASE_H
#define	GFL_MOTIONBASE_H
#pragma once

#define IK_BEND_Z (0)
#define IK_BEND_Y (!IK_BEND_Z)
#define USE_LOCATOR_Y
#define GFL_CALC_SLOW_MOTION 1

namespace gfl {
namespace motion {

/**
 *  ���[�V�������N���X
 */

class MotionBase {
	GFL_FORBID_COPY_AND_ASSIGN( MotionBase );

public:

	enum {
		MOTION_SUM, /* ���[�V�����̑��� GetData()[Ex::MotionBase::MOTION_SUM] �Ŏ擾�\ */
		MOTION_BASEPOSE,
		MOTION_BEGIN,

		JOINT_NO_USE = -1,
#ifdef USE_LOCATOR_Y
		LOCATOR_ROT_Y = 0, /**< ���[�V�����f�[�^�̂͂��߂̓��P�[�^�[�x�p�x */
		LOCATOR_TRANS_Y, /**< ���P�[�^�[�x���W */
		LOCATOR_DATA, /**< ���P�[�^�[�f�[�^�� */

		ROOT_DATA_BEGIN = LOCATOR_DATA, /**< ���[�g�f�[�^�̎n�܂� */
#endif
	};

private:
	// FunctionCurve �̃C���X�^���X
	Fcurve _Fcurve;
	// Fcurve ���𓀂������[�V�����f�[�^�|�C���^
	f32* _pValue;
#if GFL_CALC_SLOW_MOTION
	f32* _pNextValue;
#endif
	u32* _pData;
	// ��΂h�c�̃e�[�u���|�C���^
#if MATCH_JOINT_BY_ID
	u8* _pIdTable;
#elif MATCH_JOINT_BY_HASH
	u32* _pHashTable;
#elif MATCH_JOINT_BY_NAME
	c8* _pNameTable;
	u8* m_pTextureTypeTable;
#endif
protected:
	gfl::heap::HeapBase* m_pHeapBase;
	// JointBase�̃|�C���^
	JointBase* _pJointBase;
	EffectorBase* _pEffectorBase;

private:
	// �W���C���g��
	u8 _JointCount;
	// ���[�V�����ԍ�
	// �G�t�F�N�^�[��
	u8 _EffectorCount;
	u16 _MotionNumber;

	// �W���C���g�T�C�Y
	u16 _JointSize;
	// �G�t�F�N�^�[�T�C�Y
	u16 _EffectorSize;

	u8 m_ScaleNoInherit;

public:

	MotionBase( void ) : _pValue( 0 )
#if GFL_CALC_SLOW_MOTION
	, _pNextValue( 0 )
#endif
	, _pData( 0 ), _pJointBase( 0 )	, _pEffectorBase( 0 ), _MotionNumber( 0 ), m_ScaleNoInherit( 0 ) {
	}

	virtual ~MotionBase( void );

	/**
	 * @brief Initialize
	 * @param data_void ���[�V�����f�[�^�|�C���^
	 * @param heap �q�[�v
	 * @param model�@���f���|�C���^
	 * @return �^�@����
	 */
	bool Initialize( void* data_void, gfl::heap::HeapBase* heap = 0 );

	/**
	 * @brief SetTransRot
	 * @param jnt�@�W���C���g�|�C���^
	 * @param vp�@�t�@���N�V�����J�[�u�v�Z���ʃ|�C���^
	 * @param count�@�Z�b�g���鐔
	 * @return �i�߂��t�@���N�V�����J�[�u�v�Z���ʃ|�C���^
	 */
	f32* SetTransRot( JointBase* jnt, f32* vp, const s32 count );
	/**
	 * @brief SetTransRot
	 * @param jnt �W���C���g�|�C���^
	 * @param vp �t�@���N�V�����J�[�u�v�Z���ʃ|�C���^
	 * @param count�@�Z�b�g���鐔
	 * @param jnt_begin �J�n�W���C���g�|�C���^
	 * @param jnt_end�@�I���W���C���g�|�C���^
	 * @return �i�߂��t�@���N�V�����J�[�u�v�Z���ʃ|�C���^
	 */
	f32* SetTransRot( JointBase* jnt, f32* vp, const s32 count, JointBase* jnt_begin, JointBase* jnt_end );

	/**
	 * @brief SetValue
	 * @param vp �t�@���N�V�����J�[�u�v�Z���ʃ|�C���^
	 */
	void SetValue( f32* vp ) {
		_pValue = vp;
	}

	/**
	 * @brief SetJointSize
	 * @param size �W���C���g��
	 */
	void SetJointSize( const s32 size ) {
		_JointSize = size;
	}

	/**
	 * @brief SetEffectorSize
	 * @param size �G�t�F�N�^�[��
	 */
	void SetEffectorSize( const s32 size ) {
		_EffectorSize = size;
	}

	/**
	 * @brief SetBasePoseMatrix
	 * @param jnt �W���C���g�|�C���^
	 * @param count�@�Z�b�g���鐔
	 */
	void SetBasePoseMatrix( JointBase* jnt, const s32 count );

	/**
	 * @brief SetBasePoseMatrix
	 */
	inline void SetBasePoseMatrix( void ) {
		SetBasePoseMatrix( GetJoint<JointBase*>(), GetJointCount( ) );
	}

	// =====================================================================
	// ���z�֐��Q �n�܂�
	// =====================================================================

	/**
	 *   JointBase �C���X�^���X�̍쐬
	 * @param  count �C���X�^���X�̐�
	 * @return �C���X�^���X
	 * @remark JointBase ���p�������N���X�̍쐬�ɕK�v�ł�\n
	 *         LoadBasePose() ����R�[������܂�
	 *         ���̂܂܎g���ꍇ������̂ŏ������z�֐��ɂ��Ă��܂���
	 */

	virtual JointBase* CreateJoint( const s32 count ) {
		// �������~�\�@JointBase ���p�������N���X��ݒ肵�Ă�������
		SetJointSize( sizeof (JointBase) );
		return GFL_NEW_ARRAY( m_pHeapBase ) JointBase [count];
	}

	/**
	 *   JointBase �C���X�^���X�̔j��
	 * @param  �Ȃ�
	 * @return �Ȃ�
	 * @ramark �p����̃f�X�g���N�^�ŌĂ�ł��������B
	 *         ���̂܂܎g���ꍇ������̂ŏ������z�֐��ɂ��Ă��܂���
	 */

	virtual void DestroyJoint( void ) {
		GFL_DELETE_ARRAY GetJoint<JointBase*>();
	}

	/**
	 *   EffectorBase �C���X�^���X�̍쐬
	 * @param  �Ȃ�
	 * @return �C���X�^���X
	 * @remark EffectorBase ���p�������N���X�̍쐬�ɕK�v�ł�\n
	 *         LoadBasePose() ����R�[������܂�
	 */

	virtual EffectorBase* CreateEffector( const s32 count ) {
		SetEffectorSize( sizeof (EffectorBase) );
		return GFL_NEW_ARRAY( m_pHeapBase ) EffectorBase [count];
	}

	/**
	 * @brief DestroyEffector
	 */
	virtual void DestroyEffector( void ) {
		GFL_DELETE_ARRAY GetEffector<EffectorBase*>();
	}

	// =====================================================================
	// ���z�֐��Q �I���
	// =====================================================================

	/**
	 *   �X�V
	 * @param  �Ȃ�
	 * @return �Ȃ�
	 */

	void UpdateMotionMatrix( void );

	/**
	 *   �W���C���g�̂h�c�e�[�u���|�C���^�̎擾
	 * @param  �Ȃ�
	 * @return �W���C���g�̂h�c�e�[�u���|�C���^
	 */

#if MATCH_JOINT_BY_ID

	u8* GetIdTable( void ) {
		return _pIdTable;
	}
#elif MATCH_JOINT_BY_HASH

	u32* GetHashTable( void ) {
		return _pHashTable;
	}
#elif MATCH_JOINT_BY_NAME

	c8* GetNameTable( void ) {
		return _pNameTable;
	}
#endif
#if GFL_DEBUG || defined(GFL_PLATFORM_PC)
	void SetBoneLength( void );
#endif

	/**
	 *   �W���C���g���̎擾
	 * @param  �Ȃ�
	 * @return �W���C���g��
	 */

	s32 GetJointCount( void ) const {
		return _JointCount;
	}

	/**
	 *   ���[�V�����ԍ��̎擾
	 * @param  �Ȃ�
	 * @return ���[�V�����ԍ�
	 */

	s32 GetMotionNumber( void ) const {
		return _MotionNumber;
	}

	/**
	 *   �t���[�����̎擾
	 * @param  �Ȃ�
	 * @return �t���[����
	 */

	f32 GetFrameCount( void ) {
		return GetFcurve( )->GetFrameCount( );
	}

	/**
	 *   ���݂̃t���[�����擾
	 * @param  �Ȃ�
	 * @return ���݂̃t���[��
	 */

	f32 GetFrame( void ) {
		return GetFcurve( )->GetFrame( );
	}

	/**
	 *   �t���[����ݒ�
	 * @param  frame �t���[��
	 * @return �Ȃ�
	 */
	void SetFrameAndClear( const f32 frame );

	/**
	 *   �Đ��X�s�[�h���擾
	 * @param  �Ȃ�
	 * @return �Đ��X�s�[�h
	 */

	f32 GetFrameSpeed( void ) const {
		return _Fcurve.GetFrameSpeed( );
	}

	/**
	 *   �Đ��X�s�[�h��ݒ�
	 * @param  spd �Đ��X�s�[�h
	 * @return �Ȃ�
	 */

	void SetFrameSpeed( const f32 spd ) {
		_Fcurve.SetFrameSpeed( spd );
	}

	/**
	 *   ���[�V�������I���������H
	 * @param  �Ȃ�
	 * @return �^�@�I���@�U�@���I��
	 */

	bool QueryEnd( void ) {
		return GetFcurve( )->QueryEnd( );
	}

	/**
	 *   ���[�V�������I���������H
	 * @param  �Ȃ�
	 * @return �^�@�I���@�U�@���I��
	 * @remark QueryEnd �̓t���[���X�V�̌�Ń`�F�b�N����ꍇ
	 */

	bool QueryEndBefore( void ) {
		return GetFcurve( )->QueryEndBefore( );
	}

	/**
	 *   ���[�V�����f�[�^�̃��[�h
	 * @param  data �f�[�^�|�C���^
	 * @return �^�@�����@�U�@���s
	 */
	bool Load( const void* data );

	/**
	 * ���[�V����������̂��H
	 * @param num ���[�V�����ԍ�
	 * @return �@�^�@�O�łȂ��@�U�@�O �{���̓I�t�Z�b�g�l
	 */
	size_t IsAvailable( const s32 num );
	/**
	 *   ���[�V�����ύX
	 * @param  num ���[�V�����ԍ�
	 * @return �^�@�����@�U�@���s
	 */
	virtual bool Change( const s32 num );

	/**
	 *   ���[�V�������ăX�^�[�g
	 * @param  �Ȃ�
	 * @return �Ȃ�
	 */

	inline void Repeat( void ) {
		Change( GetMotionNumber( ) );
		//SetSmoothFrameBefore(0.0f);
	}

	/**
	 *   ���[�V�������̎擾
	 * @param  �Ȃ�
	 * @return ���[�V������
	 */

	f32 GetMotionCount( void ) {
		return GetData( )[MOTION_SUM];
	}

	/**
	 *   ���Ԃ̍X�V
	 * @param  �Ȃ�
	 * @return �Ȃ�
	 */
	void UpdateFrame( void );

	/**
	 *   Fcurve ���𓀂����f�[�^�|�C���^�̎擾
	 * @param  �Ȃ�
	 * @return �f�[�^�|�C���^
	 */

	f32* GetValue( void ) {
		return _pValue;
	}

	/**
	 *   Fcurve ���𓀂����f�[�^�|�C���^�̎擾
	 * @param  �Ȃ�
	 * @return �f�[�^�|�C���^
	 */
	const f32* GetValue( void ) const {
		return _pValue;
	}

	f32 GetTextureValue( const s32 id ) {
		return _Fcurve.GetTextureValue( id );
	}

	/**
	 *   ���P�[�^�x�q�������擾
	 * @param  ����
	 * @return ���P�[�^�x�q����
	 */

	f32 GetMotionRotY( void ) const {
		return 0.0f;
		//			return GetValue( )[LOCATOR_ROT_Y];
	}

	/**
	 *   ���P�[�^�xtrans���擾
	 * @param  ����
	 * @return ���P�[�^�xtrans
	 */

	f32 GetMotionTransY( void ) const {
#if 0 //
		//return 0.0f;
		//	return GetValue( )[LOCATOR_TRANS_Y];
		if( GetValue( ) ){
			return GetMotionRootDataPointer( )[gfl::core::TY];
		}
		return 0.0f;
#else
		JointBase* jnt = _pJointBase;
		return (jnt) ? jnt->_pTrans->GetY( ) : 0.0f;
#endif
	}

	/**
	 *   ���P�[�^trans���擾
	 * @param  ����
	 * @return ���P�[�^trans
	 */

	const gfl::core::Vector* GetMotionTrans( void ) const {
		JointBase* jnt = _pJointBase;
		return (jnt) ? jnt->_pTrans : (const gfl::core::Vector*) & gfl::core::Vector::GetZero( );
	}

	/**
	 *   ���[�g�f�[�^�|�C���^���擾
	 * @param  ����
	 * @return ���[�g�f�[�^�|�C���^
	 */

	const f32* GetMotionRootDataPointer( void ) const {
#ifdef USE_LOCATOR_Y
		return &GetValue( )[ROOT_DATA_BEGIN];
#else
		return GetValue( );
#endif
	}

	/**
	 *   Fcurve�|�C���^ �̎擾
	 * @param  �Ȃ�
	 * @return Fcurve�|�C���^
	 */

	Fcurve* GetFcurve( void ) {
		return &_Fcurve;
	}

	/**
	 *   Fcurve�|�C���^ �̎擾
	 * @param  �Ȃ�
	 * @return Fcurve�|�C���^
	 */
	const Fcurve* GetFcurve( void ) const {
		return &_Fcurve;
	}

	/**
	 *   ���[�V�����A�h���X���擾
	 * @param  �Ȃ�
	 * @return ���[�V�����A�h���X
	 */

	u32* GetData( void ) {
		return _pData;
	}

	/**
	 *   ���[�V�����A�h���X���擾
	 * @param  idx �C���f�b�N�X
	 * @return ���[�V�����A�h���X
	 */

	u32* GetData( const s32 idx ) {
		u32* data = GetData( );
		if( data ){
			size_t ofs = (size_t) data[idx];
			return (ofs) ? reinterpret_cast<u32*> ((size_t) data + ofs) : 0;
		}
		return 0;
	}

	/**
	 *   �W���C���g�|�C���^�̐擪���擾
	 * @return �W���C���g�|�C���^
	 * @remark �W���C���g�|�C���^�̐擪
	 */

	template<typename T>
	T GetJoint( void ) {
		return static_cast<T> (_pJointBase);
	}
	
	/**
	 *   ID�Ԗڂ̃W���C���g�|�C���^���擾
	 * @return �W���C���g�|�C���^
	 */
	JointBase* GetJointAddress( const s32 id ){
		return gfl::core::Utility::AddAddress( _pJointBase, id * _JointSize );
	}

	/**
	 *   �W���C���g�|�C���^�̎擾
	 * @param  id �z��ԍ�
	 * @return �W���C���g�|�C���^
	 * @remark ��΂h�c�ł͂Ȃ��A�P�ɔz��ԍ��Ŏ擾
	 */

	template<typename T>
	T GetJoint( const s32 id ) {
		return static_cast<T> (
			((id < GetJointCount( ))
			? GetJointAddress(id)
			: _pJointBase));
	}

	/**
	 * @brief GetEffectorCount
	 * @return �G�t�F�N�^�[��
	 */
	s32 GetEffectorCount( void ) const {
		return _EffectorCount;
	}

	/**
	 * @brief GetEffector
	 * @return �G�t�F�N�^�[
	 */
	template<typename T>
	T GetEffector( void ) {
		return static_cast<T> (_pEffectorBase);
	}

#if MATCH_JOINT_BY_HASH

	template<typename T>
	T GetJointByHash( const u32 id ) {
		for( s32 i = 0; i < GetJointCount( ); ++i ){
			if( id == _pHashTable[i] ){
				return GetJoint<T > (i);
			}
		}
		return 0;
	}
#elif MATCH_JOINT_BY_NAME
	/**
	 * @brief GetJointByName
	 * @param name �W���C���g��������
	 * @return �W���C���g�|�C���^
	 */
	template<typename T>
	T GetJointByName( const c8 * const name ) {
		for( s32 i = 1; i < GetJointCount( ); ++i ){
			T jnt = GetJoint<T > (i);
			if( 0 == strcmp( name, jnt->_pName ) ){
				return jnt;
			}
		}
		return 0;
	}
#endif

	/**
	 * �Z�b�e�B���O�f�[�^�ݒ�
	 * @param data�@�ǂݎn�߃f�[�^�|�C���^
	 * @return �ǂݏI���f�[�^�|�C���^
	 */
	virtual u8* LoadSetting( u8* data, gfl::heap::HeapBase* heap );

	/**
	 *   ��Δԍ��ɂ�郂�[�V�����W���C���g�̎擾
	 * @param  abs_id ��Δԍ�
	 * @return ���[�V�����W���C���g�ԍ�
	 */
	s32 GetMotionJointId( const u32 abs_id );

	/**
	 * @brief NextJoint
	 * @param jnt �W���C���g�|�C���^
	 * @return ���̃W���C���g�|�C���^
	 */
	template<typename T>
	inline T NextJoint( T jnt ) {
		return gfl::core::Utility::AddAddress( jnt, _JointSize );
	}

	/**
	 * @brief PreviousJoint
	 * @param jnt �W���C���g�|�C���^
	 * @return �O�̃W���C���g�|�C���^
	 */
	template<typename T>
	inline T PreviousJoint( T jnt ) {
		return gfl::core::Utility::SubAddress( jnt, _JointSize );
	}

	/**
	 * @brief NextEffector
	 * @param eff �G�t�F�N�^�[�|�C���^
	 * @return ���̃G�t�F�N�^�[�|�C���^
	 */
	template<typename T>
	inline T NextEffector( T eff ) {
		return gfl::core::Utility::AddAddress( eff, _EffectorSize );
	}

	/**
	 * @brief PreviousEffector
	 * @param eff �G�t�F�N�^�[�|�C���^
	 * @return �O�̃G�t�F�N�^�[�|�C���^
	 */
	template<typename T>
	inline T PreviousEffector( T eff ) {
		return gfl::core::Utility::SubAddress( eff, _EffectorSize );
	}

	/**
	 * @brief CalcIK
	 * @param jnt �W���C���g�|�C���^
	 * @param count �v�Z����W���C���g��
	 * @param type�@�ǂ����IK ���[�h��
	 */
	virtual void CalcIK( JointBase* jnt, const s32 count, const size_t ofs );

	/**
	 * @brief GetHeapBase
	 * @return �q�[�v
	 */
	gfl::heap::HeapBase* GetHeapBase( ) const {
		return m_pHeapBase;
	}

	/**
	 * @brief SetGlobalRotation
	 * @param jnt �W���C���g�|�C���^
	 * @param global_mat�@�v�Z�����[�g�}�g���b�N�X
	 * @param count�@�v�Z����W���C���g��
	 */
	void SetGlobalRotation( JointBase* jnt, const gfl::core::Matrix& global_mat, const s32 count );

	enum {
		TEX_OTHER,
		TEX_EYE,
		TEX_MOUTH,
	};
#ifdef USE_TEXTURE_ANIMATION

	f32 GetCurrentTextureFrame( const s32 id ) {
		return _pValue[GetJointCount( ) * gfl::core::TXYZ * 3 + id];
	}

	u8* GetTextureTypeTable( ) const {
		return m_pTextureTypeTable;
	}

	u8 GetTextureType( const s32 id ) const {
		return m_pTextureTypeTable[id];
	}
#endif
	/**
	 * @brief CalculateMatrix
	 * @param jnt �W���C���g�|�C���^
	 * @param count�@�v�Z����W���C���g��
	 */
	void CalculateMatrix( JointBase* jnt, const s32 count );

	/**
	 * @brief SetScaleNoInherit
	 * @param ScaleNoInherit �X�P�[����e����p�����邩
	 */
	void SetScaleNoInherit( u8 ScaleNoInherit ) {
		this->m_ScaleNoInherit = ScaleNoInherit;
	}

	/**
	 * @brief GetScaleNoInherit
	 * @return �^�@�X�P�[�����p������@�U�@�X�P�[�����p�����Ȃ�
	 */
	u8 GetScaleNoInherit( ) const {
		return m_ScaleNoInherit;
	}

	/**
	 * ���[�V�����ԍ��̃t���[�������擾
	 * @param motnum ���[�V�����ԍ�
	 * @return �t���[����
	 */
	u16 GetFrameCount( const s32 motnum );

#if GFL_CALC_SLOW_MOTION
	/**
	 * @brief SetNextValue
	 * @param _pNextValue ���̃t���[���̌v�Z���ʂ̎��[�ꏊ
	 */
    void SetNextValue( f32* _pNextValue ) {
    	this->_pNextValue = _pNextValue;
    }

	/**
	 * @brief GetNextValue
	 * @return ���̃t���[���̌v�Z���ʂ̎��[�ꏊ
	 */
    f32* GetNextValue( ) const {
    	return _pNextValue;
    }
#endif
};

}
}

#endif	/* GFL_MOTIONBASE_H */


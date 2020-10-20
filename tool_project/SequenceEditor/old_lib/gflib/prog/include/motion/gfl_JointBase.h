/* 
 * File:   gfl_JointBase.h
 * Author: kimura_shin
 *
 * Created on 2010/12/02, 16:52
 */

#ifndef GFL_JOINTBASE_H
#define	GFL_JOINTBASE_H
#pragma once

namespace gfl {
namespace motion {

class EffectorBase;
// =============================================================================
// JointBase
// =============================================================================

class JointBase {
public:

	enum {
		IK_NO, // �Ȃ�ł��Ȃ�
		IK_EFFECTOR_CHILD, // �G�t�F�N�^�[�̎q��
		IK_TIP, // �G�t�F�N�^�[�̐������
		IK_CHAIN, // �`�F�C�����[�g
		IK_JOINT, // �W���C���g

		IK_EFFECTOR = 0x08,
		IK_EFFECTOR_R,
		IK_EFFECTOR_2,
		IK_EFFECTOR_2_R,

		IK_SKIP_CALC = 0x10, // ���̃I�t�Z�b�g�����Z����ƃX�L�b�v

		// �r�b�g����Ŏ�ނ𔻒f�������̂�
		IK_EFFECTOR_BIT = 0x08,
		IK_EFFECTOR_REVERSE_BIT = 0x01,
		IK_EFFECTOR_2_BIT = 0x02
	};

	JointBase( void ) :
#if MATCH_JOINT_BY_NAME
	_pName( 0 ),
#endif
	_IkType( IK_NO ) {
		_Matrix.SetUnit( );
		_StaticRot.SetZero( );
		_Scale.Set( 1.0f, 1.0f, 1.0f );
	}

	gfl::core::Matrix _MotionMatrix;
	gfl::core::Matrix _Matrix;
	// 16 �o�C�g�A���C�������g���K�v�ȃv���b�g�t�H�[��
#ifdef GFL_QUAT_NEEDS_16BYTE_ALIGN
	gfl::core::Quaternion _BasePoseQuat;
#endif
	gfl::core::Quaternion _Quat;
	gfl::core::Vector _StaticTrans;
	gfl::core::Vector _StaticRot;
	// �x�[�X�|�[�Y�Őe�W���C���g���瑊��Quaternion
	gfl::core::Vector _BasePoseLocalRot;
	// �e�W���C���g����̑��Ίp�x
	gfl::core::Vector _Rot;
	// �X�P�[��
	gfl::core::Vector _Scale;

	// �e�W���C���g����̑��΍��W�|�C���^�i���[�V�����f�[�^�����w���Ă���j
	gfl::core::Vector* _pStaticTrans;
	// ���[�V�����f�[�^�G�t�F�N�^�[���W�|�C���^�i���[�V�����f�[�^�����w���Ă���j
	gfl::core::Vector* _pTrans;

	JointBase* _pParent;
	// �G�t�F�N�^�[�N���X
	EffectorBase* _pEffectorBase;
#ifndef GFL_QUAT_NEEDS_16BYTE_ALIGN
	gfl::core::Quaternion* _pBasePoseQuat;
#endif
#if MATCH_JOINT_BY_NAME
	// ���O������
	c8* _pName;
#endif

#if GFL_DEBUG || defined(GFL_PLATFORM_PC)
	f32 _Length;
#endif
	u8 _Id;
	u8 _IkType;
	u8 _ChildCount;
	// �X�P�[���f�[�^�����݂��邩�H
	u8 _ScaleExist;

	/**
	 * @brief GetMotionMatrix
	 * @return ���[�V�����f�[�^�}�g���b�N�X
	 */
	gfl::core::Matrix* GetMotionMatrix( void ) {
		return &_MotionMatrix;
	}

	/**
	 * @brief GetMatrix
	 * @return�@�O���[�o���}�g���b�N�X
	 */
	gfl::core::Matrix* GetMatrix( void ) {
		return &_Matrix;
	}

	/**
	 * @brief GetMatrix
	 * @return �O���[�o���}�g���b�N�X
	 */
	const gfl::core::Matrix* GetMatrix( void ) const {
		return &_Matrix;
	}

	/**
	 * @brief GetTrans
	 * @return ���W�|�C���^
	 */
	gfl::core::Vector* GetTrans( void ) {
		return _pTrans;
	}

	/**
	 * @brief GetStaticTrans
	 * @return �ÓI���΍��W
	 */
	const gfl::core::Vector* GetStaticTrans( void ) const {
		return _pStaticTrans;
	}

	/**
	 * @brief GetTrans
	 * @return ���W�|�C���^
	 */
	const gfl::core::Vector* GetTrans( void ) const {
		return _pTrans;
	}

	/**
	 * @brief SetTrans
	 * @param ptr ���W�|�C���^
	 */
	void SetTrans( gfl::core::Vector* ptr ) {
		_pTrans = ptr;
	}

	/**
	 * @brief GetRot
	 * @return ���[�J���p�x
	 */
	gfl::core::Vector* GetRot( void ) {
		return &_Rot;
	}

	/**
	 * @brief GetRot
	 * @return ���[�J���p�x
	 */
	const gfl::core::Vector* GetRot( void ) const {
		return &_Rot;
	}

	/**
	 * @brief GetBasePoseQuat
	 * @return �x�[�X�|�[�Y�̃O���[�o���N�H�[�^�j�I��
	 */
	const gfl::core::Quaternion& GetBasePoseQuat( void ) const {
#ifdef GFL_QUAT_NEEDS_16BYTE_ALIGN
		return _BasePoseQuat;
#else
		return *_pBasePoseQuat;
#endif
	}

	/**
	 * @brief GetQuat
	 * @return ���[�J���N�H�[�^�j�I��
	 */
	gfl::core::Quaternion* GetQuat( void ) {
		return &_Quat;
	}

	/**
	 * @brief GetBasePoseLocalRot
	 * @return �x�[�X�|�[�Y���[�J���p�x
	 */
	const gfl::core::Vector& GetBasePoseLocalRot( void ) const {
		return _BasePoseLocalRot;
	}

	/**
	 * @brief GetIkType
	 * @return IK �^
	 */
	u8 GetIkType( void ) const {
		return _IkType;
	}

	/**
	 * @brief SetIkType
	 * @param val IK �^
	 */
	void SetIkType( const u8 val ) {
		_IkType = val;
	}

	/**
	 * @brief GetParent
	 * @return �e�W���C���g
	 */
	JointBase* GetParent( void ) {
		return _pParent;
	}

	/**
	 * @brief GetParent
	 * @return �e�W���C���g
	 */
	const JointBase* GetParent( void ) const {
		return _pParent;
	}

	/**
	 * @brief SetParent
	 * @param jnt �e�W���C���g
	 */
	void SetParent( JointBase* jnt ) {
		_pParent = jnt;
	}

	/**
	 * @brief GetScale
	 * @return �X�P�[���|�C���^
	 */
	gfl::core::Vector* GetScale( void ) {
		return &_Scale;
	}

	/**
	 * @brief GetScale
	 * @return �X�P�[���|�C���^
	 */
	const gfl::core::Vector* GetScale( void ) const {
		return &_Scale;
	}

	/**
	 * @brief GetId
	 * @return ID
	 */
	u8 GetId( ) const {
		return _Id;
	}

	/**
	 * @brief GetName
	 * @return ���O������
	 */
	c8* GetName(){
		return _pName;
	}

	/**
	 * @brief GetName
	 * @return ���O������
	 */
	const c8* GetName() const {
		return _pName;
	}

	/**
	 * @brief GetChildCount
	 * @return �q���W���C���g�̐�
	 */
	u8 GetChildCount() const {
		return _ChildCount;
	}

#if GFL_DEBUG || defined( GFL_PLATFORM_PC )
	/**
	 * @brief GetLength
	 * @return �W���C���g�̒���
	 */
	f32 GetLength() const {
		return _Length;
	}
	/**
	 * @brief SetLength
	 * @param len �W���C���g�̒���
	 */
	void SetLength(const f32 len){
		_Length = len;
	}

#endif
	friend class MotionBase;
};

}
}

#endif	/* GFL_JOINTBASE_H */


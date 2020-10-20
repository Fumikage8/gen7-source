/* 
 * File:   gfl_Effector.h
 * Author: kimura_shin
 *
 * Created on 2010/12/02, 16:54
 */

#ifndef GFL_EFFECTOR_H
#define	GFL_EFFECTOR_H
#pragma once

namespace gfl {
namespace motion {

/**
 *  �G�t�F�N�^�[���N���X
 * @remark �h�j�̌v�Z�ɕK�v
 */

class EffectorBase {
public:
	// �O���[�o�����W�i���́j
	gfl::core::Vector _Trans;
	// �O���[�o�����W�|�C���^
	gfl::core::Vector* _pTrans;

	// �`�F�C���̃W���C���g
	JointBase* _pJointBase_Chain;
	// ���̃W���C���g
	JointBase* _pJointBase_Joint;
	// �G�t�F�N�^�[�̃W���C���g
	JointBase* _pJointBase_Effector;

	EffectorBase( void ) {
	}

	~EffectorBase( void ) {
	}

	/**
	 * @brief GetTrans
	 * @return ���W�̎��̂̃|�C���^
	 */
	gfl::core::Vector* GetGlobalTrans( void ) {
		return _pTrans;
	}

	/**
	 * @brief GetJoint_Chain
	 * @return �`�F�C���̃W���C���g�|�C���^
	 */
	template<typename T>
	T GetJoint_Chain( void ) {
		return static_cast<T> (_pJointBase_Chain);
	}

	/**
	 * @brief GetJoint_Joint
	 * @return ���̃W���C���g�|�C���^
	 */
	template<typename T>
	T GetJoint_Joint( void ) {
		return static_cast<T> (_pJointBase_Joint);
	}

	/**
	 * @brief GetJoint_Effector
	 * @return �G�t�F�N�^�[�̃W���C���g�|�C���^
	 */
	template<typename T>
	T GetJoint_Effector( void ) {
		return static_cast<T> (_pJointBase_Effector);
	}

	friend class JointBase;
	friend class MotionBase;
};

}
}

#endif	/* GFL_EFFECTOR_H */


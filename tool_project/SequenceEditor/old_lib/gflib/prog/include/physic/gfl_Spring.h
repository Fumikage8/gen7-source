//==============================================================================
/**
 * @file	gfl_Spring.h
 * 	
 * @author	kimura_shin
 * @data	2011/03/14, 10:21
 */
// =============================================================================

#ifndef GFL_SPRING_H
#define	GFL_SPRING_H
#pragma once

namespace gfl {
namespace physic {

static const f32 DEFAULT_GRAVITY = (980.665f / (60.0f * 60.0f)); ///< �d�͉����x

class Spring {
private:
	gfl::core::Vector m_Position; ///< ���W
	gfl::core::Vector m_Velocity; ///< ���x
	gfl::core::Vector m_ExternalForce; ///< �O��

	f32 m_Damping; ///<�@�΂˒萔
	f32 m_Stiffness; ///<�@����
	f32 m_Mass; ///< ����
	f32 m_Gravity; ///< �d�͉����x
	f32 m_DownForce; ///< �O�͂Ƃ͕ʂ̉������̗�

public:
	static const f32 DEFAULT_DAMPING; ///<�@�΂˒萔
	static const f32 DEFAULT_STIFFNESS; ///<�@����
	static const f32 DEFAULT_MASS; ///< ����

	/**
	 * �R���X�g���N�^�@�f�t�H���g�l��ݒ�
	 */
	Spring( void ) : m_Damping( DEFAULT_DAMPING ),
	m_Stiffness( DEFAULT_STIFFNESS ), m_Mass( DEFAULT_MASS ), m_Gravity( DEFAULT_GRAVITY ) {
		Reset( gfl::core::Vector::GetZero( ) );
	}

	/**
	 * ���W�̍X�V
	 * @param tag�@�ڕW���W
	 */
	void Update( const gfl::core::Vector& tag );

	/**
	 * ���݂̍��W�̎擾
	 * @return ���݂̍��W
	 */
	const gfl::core::Vector& GetPosition( ) const {
		return m_Position;
	}

	/**
	 * ���݂̍��W�Ń��Z�b�g
	 */
	void Reset( const gfl::core::Vector& pos );

	/**
	 * �Q�b�^�[�Z�b�^�[�Q�@Netbeans�Ŏ�������
	 */

	void SetGravity( f32 Gravity ) {
		this->m_Gravity = Gravity;
	}

	f32 GetGravity( ) const {
		return m_Gravity;
	}

	void SetMass( f32 Mass ) {
		this->m_Mass = Mass;
	}

	f32 GetMass( ) const {
		return m_Mass;
	}

	void SetStiffness( f32 Stiffness ) {
		this->m_Stiffness = Stiffness;
	}

	f32 GetStiffness( ) const {
		return m_Stiffness;
	}

	void SetDamping( f32 Damping ) {
		this->m_Damping = Damping;
	}

	f32 GetDamping( ) const {
		return m_Damping;
	}

	const gfl::core::Vector* GetExternalForce( ) const {
		return &m_ExternalForce;
	}

	gfl::core::Vector* GetExternalForce( ) {
		return &m_ExternalForce;
	}

	void SetDownForce( f32 DownForce ) {
		this->m_DownForce = DownForce;
	}

	f32 GetDownForce( ) const {
		return m_DownForce;
	}
};

}
}

#endif	/* GFL_SPRING_H */

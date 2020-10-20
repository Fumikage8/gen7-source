//==============================================================================

#include "physic/gfl_Spring.h"

/**
 * @file	gfl_Spring.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/23, 16:45
 */
// =============================================================================
const f32 gfl::physic::Spring::DEFAULT_DAMPING = 0.55f; ///<�@�΂˒萔
const f32 gfl::physic::Spring::DEFAULT_STIFFNESS = 0.45f; ///<�@����
const f32 gfl::physic::Spring::DEFAULT_MASS = 1.0f; ///< ����

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::physic::Spring::Update( const gfl::core::Vector& tag )
{
	gfl::core::Vector force( tag ); // ��
	force.Add( m_ExternalForce ); // �O�͂�������
	force.GetRefY() += m_DownForce; // �ʂ̉������O�͂�������
	force.Sub( m_Position ); // ���݈ʒu����^�[�Q�b�g�ւ̃x�N�g��
	force.Mul( m_Stiffness ); // �������|����
	force.GetRefY( ) -= m_Gravity; // Y �̂ݏd�͂����Z

	gfl::core::Vector accel( force ); // �����x
	accel.Div( m_Mass ); // F = m * a -> a = F / m
	m_Velocity.Add( accel ); // v = a * t
	m_Velocity.Mul( m_Damping ); // v *= k
	m_Position.Add( m_Velocity ); // p = v * t
}

//==============================================================================
//==============================================================================
void gfl::physic::Spring::Reset( const gfl::core::Vector& pos )
{
	m_Position.Set( pos );
	m_Velocity.SetZero( );
	m_ExternalForce.SetZero( );
	m_DownForce = 0.0f;
}

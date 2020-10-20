//==============================================================================

#include "physic/gfl_Spring.h"

/**
 * @file	gfl_Spring.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/23, 16:45
 */
// =============================================================================
const f32 gfl::physic::Spring::DEFAULT_DAMPING = 0.55f; ///<　ばね定数
const f32 gfl::physic::Spring::DEFAULT_STIFFNESS = 0.45f; ///<　剛性
const f32 gfl::physic::Spring::DEFAULT_MASS = 1.0f; ///< 質量

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::physic::Spring::Update( const gfl::core::Vector& tag )
{
	gfl::core::Vector force( tag ); // 力
	force.Add( m_ExternalForce ); // 外力を加える
	force.GetRefY() += m_DownForce; // 別の下向き外力を加える
	force.Sub( m_Position ); // 現在位置からターゲットへのベクトル
	force.Mul( m_Stiffness ); // 剛性を掛ける
	force.GetRefY( ) -= m_Gravity; // Y のみ重力を加算

	gfl::core::Vector accel( force ); // 加速度
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

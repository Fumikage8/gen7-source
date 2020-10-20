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

static const f32 DEFAULT_GRAVITY = (980.665f / (60.0f * 60.0f)); ///< 重力加速度

class Spring {
private:
	gfl::core::Vector m_Position; ///< 座標
	gfl::core::Vector m_Velocity; ///< 速度
	gfl::core::Vector m_ExternalForce; ///< 外力

	f32 m_Damping; ///<　ばね定数
	f32 m_Stiffness; ///<　剛性
	f32 m_Mass; ///< 質量
	f32 m_Gravity; ///< 重力加速度
	f32 m_DownForce; ///< 外力とは別の下向きの力

public:
	static const f32 DEFAULT_DAMPING; ///<　ばね定数
	static const f32 DEFAULT_STIFFNESS; ///<　剛性
	static const f32 DEFAULT_MASS; ///< 質量

	/**
	 * コンストラクタ　デフォルト値を設定
	 */
	Spring( void ) : m_Damping( DEFAULT_DAMPING ),
	m_Stiffness( DEFAULT_STIFFNESS ), m_Mass( DEFAULT_MASS ), m_Gravity( DEFAULT_GRAVITY ) {
		Reset( gfl::core::Vector::GetZero( ) );
	}

	/**
	 * 座標の更新
	 * @param tag　目標座標
	 */
	void Update( const gfl::core::Vector& tag );

	/**
	 * 現在の座標の取得
	 * @return 現在の座標
	 */
	const gfl::core::Vector& GetPosition( ) const {
		return m_Position;
	}

	/**
	 * 現在の座標でリセット
	 */
	void Reset( const gfl::core::Vector& pos );

	/**
	 * ゲッターセッター群　Netbeansで自動生成
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

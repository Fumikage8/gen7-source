#if !defined(GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTNODE_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTNODE_H_INCLUDED
#pragma once

#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class LightNode : public NodeTrait<LightNode, CullingObjectNode>, public NodeName
{
public:

	friend class InstanceCreator;

	virtual ~LightNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::LightNode") ) );
		return hash;
	}

	//! @name 各種パラメータ設定
	//@{
		inline void SetNodeNameHash( u32 hash ){ m_NodeNameHash = hash; }
		inline void SetLightType( LightType type ) { m_LightType = type; }						//!< タイプの設定
		inline void SetColor( const math::Vector4& color) { m_Color = color; }					//!< カラーの設定
		inline void SetIntensity( f32 intensity ) { m_Intensity = intensity; }				//!< 強度の設定
		inline void SetPosition( const math::Vector3& pos ){ m_LocalSRT.translate.Set(pos.x, pos.y, pos.z); }	//!< ポジションの設定
		inline void SetDirection( const math::Vector3& dir ){ m_Direction = dir; }			//!< 方向の設定
		inline void SetDecayRate( f32 decayRate ) { m_DecayRate = decayRate; }				//!< 減衰率の設定
		inline void SetConeAngle( f32 coneAngle ) { m_ConeAngle = coneAngle; }				//!< 円錐の角度設定
		inline void SetPriority( s32 priority ) { m_Priority = priority; }						//!< プライオリティの設定
	//@}

	//! @name 各種パラメータ取得
	//@{
		inline u32 GetNodeNameHash() const { return m_NodeNameHash; }
		inline LightType GetLightType() const { return m_LightType; }								//!< タイプの取得
		inline const math::Vector4& GetColor() const { return m_Color; }							//!< カラーの取得
		inline f32 GetIntensity() const { return m_Intensity; }											//!< 強度の取得
		inline const math::Vector3& GetPosition() const { return *(math::Vector3*)&m_LocalSRT.translate; }	//!< ポジションの取得
		inline const math::Vector3& GetDirection() const { return m_Direction; }			//!< 方向の取得
		inline f32 GetDecayRate() const { return m_DecayRate; }											//!< 減衰率の取得
		inline f32 GetConeAngle() const { return m_ConeAngle; }											//!< 円錐の角度取得
		inline s32 GetPriority() const { return m_Priority; }												//!< プライオリティの取得

		inline const math::Vector3 GetWorldPosition() const { return this->m_WorldMatrix.GetElemPosition(); }	//!< ポジションの取得
		math::Vector3 GetWorldDirection() const;																									//!< 方向の取得

		math::Matrix34 GetMatrix( void ) const;																			//! Position, Direction等からマトリクスを求める
	//@}

protected:
	LightNode();

private:

	LightType					m_LightType;

	math::Vector3			m_Position;
	math::Vector3			m_Direction;
	math::Vector4			m_Color;
	f32								m_ConeAngle;
	f32								m_DecayRate;
	f32								m_Intensity;
	s32								m_Priority;
	u32								m_NodeNameHash;
	
};

}}}}

#endif


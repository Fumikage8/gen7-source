#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_LightSetRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_LightSetRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class LightSetResourceNodeFactory;

class LightSetResourceNode : public NodeTrait<LightSetResourceNode, ResourceNode>, public NodeName
{
public:
	friend class LightSetResourceNodeFactory;

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::LightSetResourceNode") ) );
		return hash;
	}

	struct LightData
	{
		u32								m_NameHash;
		c8					      m_Name[Constant::FLAT_RESOURCE_NAME_MAX];
		math::Vector			m_Position;
		math::Vector			m_Color;
		math::Vector			m_Direction;
		float							m_ConeAngle;
    LightType	        m_Type;
		float             m_Intensity;
	};

	virtual ~LightSetResourceNode();

	void SetData( const void* data, int size );
	u32 GetSetNo() const { return m_SetNo; }
	u32 GetLightNum() const { return m_LightNum; }
	const LightData* GetLightData( u32 no ) const { return &m_pLightDatas[no]; }

protected:
	LightSetResourceNode();

	u32						m_SetNo;
	u32						m_LightNum;
	LightData			*m_pLightDatas;
};

}}}}

#endif


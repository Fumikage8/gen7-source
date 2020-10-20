#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_LocatorRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_LocatorRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <math/include/gfl2_Matrix44.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class LocatorResourceNodeFactory;

class LocatorResourceNode : public NodeTrait<LocatorResourceNode, ResourceNode>, public NodeName
{
public:

	friend class LocatorResourceNodeFactory;

	virtual ~LocatorResourceNode();

	void SetData( const void* data, u32 size );

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::LocatorResourceNode") ) );
		return hash;
	}

	const math::Matrix44& GetMatrix() const { return m_LocalMatrix; }
	const c8* GetParentName() const { return m_ParentName; }

protected:
	LocatorResourceNode();

private:

	c8											m_Name[ Constant::LOCATOR_NAME_MAX ];
	c8											m_ParentName[ Constant::LOCATOR_NAME_MAX ];
	math::Matrix44						m_LocalMatrix;

};

}}}}

#endif


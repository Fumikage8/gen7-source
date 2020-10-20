#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GFMotRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GFMotRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class GfMotResourceNodeFactory;

class GfMotResourceNode : public NodeTrait<GfMotResourceNode, ResourceNode>
{
public:

	friend class GfMotResourceNodeFactory;

	virtual ~GfMotResourceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::GfMotResourceNode") ) );
		return hash;
	}

protected:
	GfMotResourceNode();

private:

};

}}}}

#endif


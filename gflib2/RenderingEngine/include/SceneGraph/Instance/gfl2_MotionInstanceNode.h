#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MOTIONINSTANCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MOTIONINSTANCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawableNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class MotionInstanceNode : public NodeTrait<MotionInstanceNode, DrawableNode>
{
public:

	friend class InstanceCreator;

	virtual ~MotionInstanceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::MotionInstanceNode") ) );
		return hash;
	}

protected:
	MotionInstanceNode();


};

}}}}

#endif


#if !defined(GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_EFFECTNODELOCATORINSTANCENODE_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_EFFECTNODELOCATORINSTANCENODE_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class EffectNodeLocatorInstanceNode : public NodeTrait<EffectNodeLocatorInstanceNode, LocatorInstanceNode>
{
public:
	friend class InstanceCreator;

	virtual ~EffectNodeLocatorInstanceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::ViewerLocatorInstanceNode") ) );
		return hash;
	}

protected:
	EffectNodeLocatorInstanceNode();
  EffectNodeLocatorInstanceNode(const c8* parentName);

private:
};

}}}}

#endif  //GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_EFFECTNODELOCATORINSTANCENODE_H_INCLUDED


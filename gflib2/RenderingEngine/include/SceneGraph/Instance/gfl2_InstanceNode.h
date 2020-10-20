#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_INSTANCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_INSTANCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { 
	
class SceneGraphManager;

namespace instance {

class InstanceCreator;

class InstanceNode : public NodeTrait<InstanceNode, DagNode>
{
public:
  static InstanceNode* Create( gfx::IGLAllocator* allocator )
  {
    return GLNew(allocator) InstanceNode();
  }

	friend class InstanceCreator;
	friend class SceneGraphManager;

	virtual ~InstanceNode();

	static util::FnvHash GetTypeHash()
	{
		return util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode") ) );
	}
protected:
	InstanceNode();
};

}}}}

#endif


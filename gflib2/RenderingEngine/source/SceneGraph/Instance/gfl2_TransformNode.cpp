#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

TransformNode::TransformNode() : NodeTrait<TransformNode, InstanceNode>()
{
  m_TransformFlags = 0;
  this->SetLocalTransformDirty(true);
  this->SetWorldTransformDirty(true);
  this->SetNonInheritanceScale(false);
  this->SetBillboardType(0); // No Billboard

	//m_LocalMatrix = math::Matrix::GetIdentity();
	m_WorldMatrix = math::Matrix34::GetIdentity();
}

TransformNode::~TransformNode()
{

}

void TransformNode::AddChild( DagNode* child)
{
	TransformNode				*pTransformNode = child->SafeCast<TransformNode>();

	GFL_ASSERT(pTransformNode);

	if ( pTransformNode )
		DagNode::AddChild( pTransformNode );
}

}}}}


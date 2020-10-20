#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

ResourceNode::ResourceNode():NodeTrait<ResourceNode, DagNode>(),
	m_pEventListener( NULL ),
	m_VersionId( 0 ),
	m_GpuReferenceCnt( 0 )
{

}

ResourceNode::~ResourceNode()
{
	GFL_ASSERT( m_GpuReferenceCnt == 0 );//まだ描画中の可能性あり

	if ( m_pEventListener )
	{
		m_pEventListener->OnDelete( this );
	}
	m_pEventListener = NULL;
}

class ResourceNodeReferenceCountVisitor
{
public:

  ResourceNodeReferenceCountVisitor() :
    m_TotalReferenceCount(0)
  {
  }

  void Visit(ResourceNode* pNode)
  {
    m_TotalReferenceCount += pNode->m_GpuReferenceCnt;
  }

  u32 GetTotalReferenceCount() const
  {
    return m_TotalReferenceCount;
  }

private:
  u32 m_TotalReferenceCount;
};

u32 ResourceNode::GetReferenceCnt() const
{
  Traverser::Visitor<ResourceNode, ResourceNodeReferenceCountVisitor> visitor;
  Traverser::Traverse<ResourceNode, ResourceNodeReferenceCountVisitor>(const_cast<ResourceNode*>(this), &visitor);

  return visitor.GetTotalReferenceCount();
}

}}}}


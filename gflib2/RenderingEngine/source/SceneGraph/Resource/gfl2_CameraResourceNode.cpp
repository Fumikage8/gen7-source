#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CameraResourceNode.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

CameraResourceNode::CameraResourceNode() : NodeTrait<CameraResourceNode, ResourceNode>(),
  m_NameHash(0)
{
	
}

CameraResourceNode::~CameraResourceNode()
{

}


void CameraResourceNode::SetData( const void* data, int size )
{
	std::memcpy( &m_CameraData, data, sizeof(m_CameraData) );
	
}


}}}}


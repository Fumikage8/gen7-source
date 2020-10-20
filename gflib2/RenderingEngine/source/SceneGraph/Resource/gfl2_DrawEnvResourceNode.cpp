#include <RenderingEngine/include/SceneGraph/Resource/gfl2_DrawEnvResourceNode.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

DrawEnvResourceNode::DrawEnvResourceNode() : NodeTrait<DrawEnvResourceNode, ResourceNode>(),
  m_NameHash(0)
{
	
}

DrawEnvResourceNode::~DrawEnvResourceNode()
{
	
}


void DrawEnvResourceNode::SetData( const void* data, int size )
{
	std::memcpy( &m_EnvData, data, sizeof(m_EnvData) );
}


}}}}


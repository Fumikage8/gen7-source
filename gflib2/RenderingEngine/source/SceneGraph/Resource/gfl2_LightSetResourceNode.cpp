#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LightSetResourceNode.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

LightSetResourceNode::LightSetResourceNode() : NodeTrait<LightSetResourceNode, ResourceNode>(),
  m_SetNo(0),
  m_LightNum(0),
	m_pLightDatas( NULL )
{
	
}

LightSetResourceNode::~LightSetResourceNode()
{
	if ( m_pLightDatas )
	{
		GetInstanceAllocator()->SystemMemoryFree( m_pLightDatas );
		m_pLightDatas = NULL;
	}
}


void LightSetResourceNode::SetData( const void* data, int size )
{
	const u32			*pData = reinterpret_cast<const u32*>( data );
	
	m_SetNo = pData[0];
	m_LightNum = pData[1];

	m_pLightDatas = reinterpret_cast<LightData*>( GetInstanceAllocator()->SystemMemoryMalloc( sizeof(LightData) * m_LightNum ) );

	std::memcpy( m_pLightDatas, &pData[2], sizeof( LightData ) * m_LightNum );
}


}}}}


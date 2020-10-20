#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LocatorResourceNode.h>
#include <math/include/gfl2_Matrix44.h>
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

LocatorResourceNode::LocatorResourceNode() : NodeTrait<LocatorResourceNode, ResourceNode>()
{
	
}

LocatorResourceNode::~LocatorResourceNode()
{

}

void LocatorResourceNode::SetData( const void* data, u32 size )
{
	GFL_ASSERT(data);
	const u8*					pSeek = reinterpret_cast<const u8*>(data);

	std::memcpy( m_Name, pSeek, sizeof(m_Name) );
	this->SetNodeName( m_Name );
	pSeek += Constant::LOCATOR_NAME_MAX;

	std::memcpy( m_ParentName, pSeek, sizeof(m_ParentName) );
	pSeek += Constant::LOCATOR_NAME_MAX;

	std::memcpy( &m_LocalMatrix, pSeek, sizeof(math::Matrix44) );
}


}}}}


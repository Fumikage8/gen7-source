#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LightSetResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LightSetResourceNode.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

using	namespace gfl2::gfx;

/*ResourceNode* LightSetResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* LightSetResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	LightSetResourceNode			*pNode = new LightSetResourceNode();

	pNode->SetNodeName( "LightSetResourceNode" );
	pNode->SetData( data, size );

	SceneGraphManager::AddChild( pNode );

  gfx::GLMemory::ClearInstanceAllocator();
	return pNode;
}

b32 LightSetResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("lightset");
	return (type == supportType);
}

}}}}



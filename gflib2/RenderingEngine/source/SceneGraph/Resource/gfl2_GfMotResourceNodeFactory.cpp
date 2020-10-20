#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* GfMotResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode(gfx::GLMemory::GetAllocator(), data, size);
}*/
ResourceNode* GfMotResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
  (void)size;

	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator );

	GfMotResourceNode				*pNode = new GfMotResourceNode();

	SceneGraphManager::AddChild( pNode );

  gfx::GLMemory::ClearInstanceAllocator();
	return pNode;
}

b32 GfMotResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("gfmot");
	return (type == supportType);
}

}}}}



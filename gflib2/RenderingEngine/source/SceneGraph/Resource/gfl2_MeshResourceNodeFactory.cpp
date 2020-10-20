#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* MeshResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* MeshResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator );
	MeshResourceNode				*pNode = new MeshResourceNode();
  gfx::GLMemory::ClearInstanceAllocator();

	pNode->SetVersionId( this->m_DataVersionId );
	pNode->SetData( data, size );
	SceneGraphManager::AddChild( pNode );

	return pNode;
}

b32 MeshResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("mesh");
	return (type == supportType);
}

}}}}



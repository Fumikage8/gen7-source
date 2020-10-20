#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* CollisionMeshResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* CollisionMeshResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	CollisionMeshResourceNode				*pNode = new CollisionMeshResourceNode();

	pNode->SetData( data, size );
	SceneGraphManager::AddChild( pNode );

  gfx::GLMemory::ClearInstanceAllocator();

	return pNode;
}

b32 CollisionMeshResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("collmesh");
	return (type == supportType);
}

}}}}



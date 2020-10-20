#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* AnimationResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* AnimationResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	AnimationResourceNode			*pNode = new AnimationResourceNode();

	pNode->SetData( data, size );
	SceneGraphManager::AddChild( pNode );

  gfx::GLMemory::ClearInstanceAllocator();

	return pNode;
}

b32 AnimationResourceNodeFactory::IsSupport( IResourceData::Type type )
{
  static const IResourceData::Type supportType("animres");
  return (type == supportType);
}

}}}}



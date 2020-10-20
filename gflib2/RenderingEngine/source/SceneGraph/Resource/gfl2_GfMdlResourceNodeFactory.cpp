#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* GfMdlResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* GfMdlResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
	GfMdlResourceNode				*pNode = new GfMdlResourceNode();

	pNode->SetVersionId( this->m_DataVersionId );
	pNode->SetData( data, size, m_CreateJointFlag );
	SceneGraphManager::AddChild( pNode );

  gfx::GLMemory::ClearInstanceAllocator();

	return pNode;
}

b32 GfMdlResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("gfmodel");
	return (type == supportType);
}

}}}}



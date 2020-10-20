#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LocatorResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LocatorResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* LocatorResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/

ResourceNode* LocatorResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int sizeParam)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator );

	const u8*									pSeek = reinterpret_cast<const u8*>(data);
	const LocatorDataHeader		*pHeader = reinterpret_cast<const LocatorDataHeader*>( pSeek );

	pSeek += sizeof(LocatorDataHeader);

	LocatorResourceNode				*pFastNode = NULL;

	for( u32 no = 0; no < static_cast<u32>(pHeader->locatorNum); ++no )
	{
		LocatorResourceNode				*pNode = new LocatorResourceNode();
		u32																size = Constant::LOCATOR_NAME_MAX//name
																						+ Constant::LOCATOR_NAME_MAX//parent name
																						+ sizeof(math::Matrix44);//matrix

		pNode->SetData( pSeek, size );

		if ( pFastNode )
		{
			pFastNode->AddChild( pNode );//まとめるだけ。親子関係解決は行わない。
		}
		else
		{
			pFastNode = pNode;
			SceneGraphManager::AddChild( pFastNode );
		}

		pSeek += size;
	}	

  gfx::GLMemory::ClearInstanceAllocator();

	return pFastNode;
}

b32 LocatorResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("locator");
	return (type == supportType);
}

}}}}



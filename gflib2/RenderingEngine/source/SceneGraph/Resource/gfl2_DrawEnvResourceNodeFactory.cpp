#include <RenderingEngine/include/SceneGraph/Resource/gfl2_DrawEnvResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_DrawEnvResourceNode.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

using	namespace gfl2::gfx;

struct DataHeader
{
	DataHeader()
	{
		nameHash = 0;
		for(int i = 0; i < 3; ++i )		dummy[i] = 0;
		std::memset( nameBuffer, 0, Constant::FLAT_RESOURCE_NAME_MAX);
	}

	u32						nameHash;
	s32						dummy[3];
	c8						nameBuffer[Constant::FLAT_RESOURCE_NAME_MAX];
};

/*ResourceNode* DrawEnvResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* DrawEnvResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator );
	const DataHeader				*pHeader = reinterpret_cast<const DataHeader*>(data);
	const u8								*pDataTop = reinterpret_cast<const u8*>(&pHeader[1]);
	DrawEnvResourceNode			*pNode = new DrawEnvResourceNode();

	pNode->SetNodeName( pHeader->nameBuffer );
	pNode->m_NameHash = pHeader->nameHash;
	pNode->SetData( pDataTop, size );

	SceneGraphManager::AddChild( pNode );

  gfx::GLMemory::ClearInstanceAllocator();
	return pNode;
}

b32 DrawEnvResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("gfenv");
	return (type == supportType);
}

}}}}



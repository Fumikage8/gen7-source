#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

using	namespace gfl2::gfx;

struct FileHeader
{//16バイトアラインサイズ
  c8								shaderName[Constant::RESOURCE_FILE_NAME_MAX];
	s32								shaderNameHash;
	s32								dumy[3];
};

/*ResourceNode* ShaderResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* ShaderResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);

  gfx::GLMemory::SetInstanceAllocator( pAllocator );
  ShaderResourceNode			*pNode = new ShaderResourceNode();
  gfx::GLMemory::ClearInstanceAllocator();

	const u8								*pData = reinterpret_cast<const u8*>(data);
	const FileHeader				*pFileHeader = reinterpret_cast<const FileHeader*>(pData);

	pNode->SetVersionId( this->m_DataVersionId );
	pNode->SetNodeName( pFileHeader->shaderName );
	pNode->m_NameHash = pFileHeader->shaderNameHash;
	pData += sizeof(FileHeader);

	pNode->SetData( pData, size );

	SceneGraphManager::AddChild( pNode );

	return pNode;
}

b32 ShaderResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("shader");
	return (type == supportType);
}

}}}}



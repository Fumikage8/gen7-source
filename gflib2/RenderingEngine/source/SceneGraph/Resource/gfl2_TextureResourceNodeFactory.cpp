#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

using	namespace gfl2::gfx;

struct ImageHeader
{
	s32						size;
	s32						dumy[3];
	c8						path[Constant::RESOURCE_FILE_NAME_MAX];
};

/*ResourceNode* TextureResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size);
}*/
ResourceNode* TextureResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);

  gfx::GLMemory::SetInstanceAllocator( pAllocator );
	TextureResourceNode			*pNode = new TextureResourceNode();
  gfx::GLMemory::ClearInstanceAllocator();

  const ImageHeader				*pHeader = reinterpret_cast<const ImageHeader*>(data);
	const u8								*pImageDataTop = reinterpret_cast<const u8*>(&pHeader[1]);

	pNode->SetNodeName( pHeader->path );
	pNode->m_NameHash = util::FnvHash( reinterpret_cast<const u8*>(pHeader->path) ).value;

#if defined(GF_PLATFORM_WIN32)
	pNode->SetImage( pImageDataTop, size, m_Pool, m_pTemporaryAllocator );
#elif defined(GF_PLATFORM_CTR)
	pNode->SetImage( pImageDataTop, size, m_Pool );
#endif

	SceneGraphManager::AddChild( pNode );

	return pNode;
}

/*ResourceNode* TextureResourceNodeFactory::CreateResourceNode(
  const c8* name,
  gfx::Texture* pTexture,
	b32 isOwner )
{
  return CreateResourceNode(gfx::GLMemory::GetAllocator(), name, pTexture, isOwner);
}*/
ResourceNode* TextureResourceNodeFactory::CreateResourceNode(
  gfx::IGLAllocator* pAllocator,
  const c8* name,
  gfx::Texture* pTexture,
	b32 isOwner )
{
  gfx::GLMemory::SetInstanceAllocator(pAllocator);
  TextureResourceNode *pNode = new TextureResourceNode(pTexture,isOwner);

  pNode->SetNodeName(name);
  pNode->m_NameHash = util::FnvHash(reinterpret_cast<const u8*>(name)).value;

  SceneGraphManager::AddChild(pNode);

  gfx::GLMemory::ClearInstanceAllocator();
  return pNode;
}

b32 TextureResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType( GetFactoryTypeName() );
	return (type == supportType);
}

const c8* TextureResourceNodeFactory::GetFactoryTypeName()
{
	return "texture";
}

}}}}



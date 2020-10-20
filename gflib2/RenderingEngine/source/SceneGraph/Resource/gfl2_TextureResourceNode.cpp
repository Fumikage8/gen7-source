#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

TextureResourceNode::TextureResourceNode() : NodeTrait<TextureResourceNode, ResourceNode>(),
	m_pTexture(NULL),
	m_isOwner(true),
  m_pVramTexture(NULL)
{
	
}

TextureResourceNode::TextureResourceNode(gfx::Texture* pTexture, b32 isOwner) : NodeTrait<TextureResourceNode, ResourceNode>(),
  m_pTexture(pTexture),
  m_NameHash(0),
	m_isOwner(isOwner),
  m_pVramTexture(NULL)
{
}

TextureResourceNode::~TextureResourceNode()
{
	if( m_pVramTexture )
    this->GetInstanceAllocator()->LocalMemoryFree(m_pVramTexture);

	if( m_pTexture && m_isOwner )
		delete m_pTexture;
}


void TextureResourceNode::SetImage( const void* data, int size, gfx::Pool pool, gfx::IGLAllocator* pAllocator )
{
	GFL_ASSERT(data);
	
	if( m_pTexture )
		delete m_pTexture;

	m_isOwner = true;

#if defined(GF_PLATFORM_CTR)

	struct FormatHeader
	{
		u16						width;
		u16						height;
		u16						format;
		u16						mipLevel;
	};


	const u8									*pSeek = reinterpret_cast<const u8*>(data);
	const FormatHeader				*pHeader = reinterpret_cast<const FormatHeader*>(data);

	pSeek += sizeof( FormatHeader );

  u32 texSizeOffset = sizeof(FormatHeader);

	{//128バイトアラインキープ
		u32				binSize = 24 + 80 + sizeof(FormatHeader);
		u32				paddingSize = 128 - ( binSize % 128 );
		

		for( s32 i = 0; i < paddingSize; ++i )
			++pSeek;


    texSizeOffset += paddingSize;
	}

	GFL_ASSERT( ((u32)pSeek % 128) == 0 );

	m_pTexture = GFGL::CreateTexture( this->GetInstanceAllocator(), pHeader->width, pHeader->height, pHeader->mipLevel, Usage::None, (Format)pHeader->format, pool );
  const void* pTexAddr = pSeek;
  if ((pool == gfx::Pool::LocalA) || (pool == gfx::Pool::LocalB))
  {
    // vramに転送
    u32 texSize = size - (reinterpret_cast<uptr>(pSeek) - reinterpret_cast<uptr>(data));
    texSize = size - texSizeOffset;
    m_pVramTexture = this->GetInstanceAllocator()->LocalMemoryMalloc(texSize, 128);

    GFGL::AddVramDmaCommand(pSeek, m_pVramTexture, texSize);

    pTexAddr = m_pVramTexture;
  }
	m_pTexture->Update( pTexAddr );
	//m_pTexture = GFGL::CreateTexture( pHeader->width, pHeader->height, 1, Usage::None, Format::A8R8G8B8, Pool::Default );
	

#elif defined(GF_PLATFORM_WIN32)
	ImageData						image(pAllocator);

	image.ReadFromMemory( reinterpret_cast<const u8*>( data ), ImageData::SwapType::OpenGL );

	m_pTexture = GFGL::CreateTextureFromImage( this->GetInstanceAllocator(), image, pool );
#endif
}


}}}}


#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRTexture.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRTexture::CTRTexture(const TextureDesc &rDesc) : Texture(rDesc)
{
  m_VirtualAddr = NULL;
  m_PhysicalAddr = NULL;
	m_Format = static_cast<PicaDataTextureFormat>(s_ctrTextureFormatTable[m_TextureDesc.format]);

  GFL_ASSERT(m_Format != 0xFF);

	if ( rDesc.usage & Usage::RenderTarget )
	{
    // レンダーターゲット用のテクスチャ
		Surface::SurfaceDesc				desc;

		desc.usage = Usage::RenderTarget | Usage::Tiled;
		desc.pool = rDesc.pool;
		desc.bufferWidth = desc.width = rDesc.width;
		desc.bufferHeight = desc.height = rDesc.height;
		desc.depth = 1;
		desc.format = rDesc.format;
		desc.multiSample = rDesc.multiSample;

    CTRSurface* pCTRSurface = new CTRSurface( desc );
    m_VirtualAddr = pCTRSurface->GetHandle().colorBuffer.virtualAddr;
    m_PhysicalAddr = nngxGetPhysicalAddr(m_VirtualAddr);
		m_pSurface[0] = pCTRSurface;
	}
  else
  {
    // 通常のテクスチャ、メモリは外部から確保されるはず
  }
}

CTRTexture::~CTRTexture()
{
  // サーフェイスの開放は~Texture()で行われる
}

//! @brief テクスチャを更新する
void CTRTexture::Update( const void* pData, u32 depth	)
{
  m_VirtualAddr = reinterpret_cast<uptr>(pData);
  m_PhysicalAddr = nngxGetPhysicalAddr(m_VirtualAddr);
}

void* CTRTexture::Lock()
{
  // デバイスメモリのアドレスしか書き換えられない
  uptr deviceMemoryAddress = nn::os::GetDeviceMemoryAddress();
  size_t deviceMemorySize = nn::os::GetDeviceMemorySize();
  GFL_ASSERT(m_VirtualAddr >= deviceMemoryAddress);
  GFL_ASSERT((m_VirtualAddr - deviceMemoryAddress) <= deviceMemorySize);

  return reinterpret_cast<void*>(m_VirtualAddr);
}

void CTRTexture::UnLock()
{
  // 何もしない
}

}}}

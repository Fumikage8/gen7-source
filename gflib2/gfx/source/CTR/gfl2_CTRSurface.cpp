#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRSurface.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace ctr {


CTRSurface::CTRSurface(const SurfaceDesc &rDesc, void* pRefAddr) : 
	Surface(rDesc),
	m_IsTexture( false )
{
	if ( m_SurfaceDesc.usage & Usage::RenderTarget )
	{
    static const u32 sc_colorSizeList[] =
    {
      4, //PICA_DATA_COLOR_RGBA8_OES = 0x0,  //PICA_DATA_COLOR_GAS_DMP   = 0x0,
      0xFF, // dummy
      2, //PICA_DATA_COLOR_RGB5_A1   = 0x2,
      2, //PICA_DATA_COLOR_RGB565    = 0x3,
      2, //PICA_DATA_COLOR_RGBA4     = 0x4
    };

    // カラーバッファ
    m_Handle.colorBuffer.format = static_cast<PicaDataColor>(s_ctrColorFormatTable[m_SurfaceDesc.format]);
    m_Handle.colorBuffer.blockSize = nn::gr::FrameBuffer::BLOCK_SIZE8; // アーリーデプステストを使うなら32
    m_Handle.colorBuffer.width = rDesc.width;
    m_Handle.colorBuffer.height = rDesc.height;

    // マルチサンプルは対応できない
    GFL_ASSERT((u32)m_Handle.colorBuffer.format < (sizeof(sc_colorSizeList) / sizeof(sc_colorSizeList[0])));
    u32 colorSize = sc_colorSizeList[m_Handle.colorBuffer.format];
    u32 bufferSize = m_Handle.colorBuffer.width * m_Handle.colorBuffer.height * colorSize;

		if ( pRefAddr )
		{
			m_Handle.colorBuffer.virtualAddr = reinterpret_cast<uptr>(pRefAddr);
		}
		else
		{
			switch( rDesc.pool ){
			case Pool::LocalA:
			case Pool::LocalB:
				m_Handle.colorBuffer.virtualAddr = reinterpret_cast<uptr>(this->GetInstanceAllocator()->LocalMemoryMalloc(rDesc.pool, bufferSize, 128));
				break;
			default:
				m_Handle.colorBuffer.virtualAddr = reinterpret_cast<uptr>(this->GetInstanceAllocator()->LocalMemoryMalloc(bufferSize, 128));
				break;
			}
		}
		
	}
	else if ( m_SurfaceDesc.usage & Usage::DepthStencil )
	{
    static const u32 sc_depthSizeList[] =
    {
      2, //PICA_DATA_DEPTH_COMPONENT16     = 0x0,
      0xFF, // dummy
      3, //PICA_DATA_DEPTH_COMPONENT24_OES = 0x2,
      4, //PICA_DATA_DEPTH24_STENCIL8_EXT  = 0x3
    };

    // デプスステンシルバッファ
    m_Handle.depthStencilBuffer.format = static_cast<PicaDataDepth>(s_ctrDepthFormatTable[m_SurfaceDesc.format]);
    m_Handle.depthStencilBuffer.width = rDesc.width;
    m_Handle.depthStencilBuffer.height = rDesc.height;

    // マルチサンプルは対応できない
    GFL_ASSERT((u32)m_Handle.depthStencilBuffer.format < (sizeof(sc_depthSizeList) / sizeof(sc_depthSizeList[0])));
    u32 depthSize = sc_depthSizeList[m_Handle.depthStencilBuffer.format];
    u32 bufferSize = m_Handle.depthStencilBuffer.width * m_Handle.depthStencilBuffer.height * depthSize;

		if ( pRefAddr )
		{
			m_Handle.depthStencilBuffer.virtualAddr = reinterpret_cast<uptr>(pRefAddr);
		}
		else
		{
			switch( rDesc.pool ){
			case Pool::LocalA:
			case Pool::LocalB:
				m_Handle.depthStencilBuffer.virtualAddr = reinterpret_cast<uptr>(this->GetInstanceAllocator()->LocalMemoryMalloc(rDesc.pool, bufferSize, 128));
				break;
			default:
				m_Handle.depthStencilBuffer.virtualAddr = reinterpret_cast<uptr>(this->GetInstanceAllocator()->LocalMemoryMalloc(bufferSize, 128));
				break;
			}
		}

	}

	m_fOwnership = true;
}

CTRSurface::~CTRSurface()
{
  // 自分でリソースを開放する必要があるか？
  if (!m_fOwnership)
  {
    return;
  }

  // カラーバッファ開放
  if (m_Handle.colorBuffer.virtualAddr != NULL)
  {
    this->GetInstanceAllocator()->LocalMemoryFree(reinterpret_cast<void*>(m_Handle.colorBuffer.virtualAddr));
    m_Handle.colorBuffer.virtualAddr = NULL;
  }

  // デプスステンシルバッファ開放
  if (m_Handle.depthStencilBuffer.virtualAddr != NULL)
  {
    this->GetInstanceAllocator()->LocalMemoryFree(reinterpret_cast<void*>(m_Handle.depthStencilBuffer.virtualAddr));
    m_Handle.depthStencilBuffer.virtualAddr = NULL;
  }
}

}}}

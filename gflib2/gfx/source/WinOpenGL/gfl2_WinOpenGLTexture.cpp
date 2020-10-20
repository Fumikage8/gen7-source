#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTexture.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLTexture::WinOpenGLTexture(const TextureDesc &rDesc) : Texture(rDesc), m_pLockedData(NULL)
{
	GLuint				format = s_openglFormatTable[ m_TextureDesc.format ];
	GLuint				size = s_openglByteTable[ m_TextureDesc.format ];

	glGenTextures( 1, &m_hTextureObject );

	if ( m_TextureDesc.format == Format::D24S8 )
	{
		glTextureImage2DEXT(m_hTextureObject, GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_TextureDesc.width, m_TextureDesc.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL );
	}
	else
	{
    u32 width = m_TextureDesc.width;
    u32 height = m_TextureDesc.height;
    for (u32 i = 0; i < m_TextureDesc.mipLevels; ++i)
    {
      glTextureImage2DEXT(m_hTextureObject, GL_TEXTURE_2D, i, format, width, height, 0, format, size, NULL );

      width /= 2;
      height /= 2;
    }
    glTextureParameteriEXT(m_hTextureObject, GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteriEXT(m_hTextureObject, GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_TextureDesc.mipLevels - 1);
	}

	if ( rDesc.usage & Usage::RenderTarget )
	{
		Surface::SurfaceDesc				desc;

		desc.usage = Usage::RenderTarget | Usage::Tiled;
		desc.pool = rDesc.pool;
		desc.width = rDesc.width;
		desc.height = rDesc.height;
		desc.depth = 1;
		desc.format = rDesc.format;
		desc.multiSample = rDesc.multiSample;

		m_pSurface[0] = new WinOpenGLSurface( desc, m_hTextureObject, true );
	}
}

WinOpenGLTexture::~WinOpenGLTexture()
{
	glDeleteTextures( 1, &m_hTextureObject );	
}

//! @brief テクスチャを更新する
void WinOpenGLTexture::Update( const void* pData, u32 depth	)
{
	GLuint				format = s_openglFormatTable[ m_TextureDesc.format ];
	GLuint				size = s_openglByteTable[ m_TextureDesc.format ];

  u32 width = m_TextureDesc.width;
  u32 height = m_TextureDesc.height;

  for (u32 i = 0; i < depth; ++i)
  {
    width /= 2;
    height /= 2;
  }

	glTextureSubImage2DEXT(m_hTextureObject, GL_TEXTURE_2D, depth, 0, 0, width, height, format, size, pData );
}

void* WinOpenGLTexture::Lock()
{
  // ロック中に呼ばれるのはおかしい
  GFL_ASSERT(m_pLockedData == NULL);

	GLuint				format = s_openglFormatTable[ m_TextureDesc.format ];
	GLuint				size = s_openglByteTable[ m_TextureDesc.format ];

  u32 width = m_TextureDesc.width;
  u32 height = m_TextureDesc.height;

  m_pLockedData = this->GetInstanceAllocator()->SystemMemoryMalloc(width * height * sizeof(u32)); // フォーマット毎のサイズ計算がめんどうなので必ず4バイトフォーマット
  glGetTextureImageEXT(m_hTextureObject, GL_TEXTURE_2D, 0, format, size, m_pLockedData);
  return m_pLockedData;
}

void WinOpenGLTexture::UnLock()
{
  // ロックされていないのに呼ばれるのはおかしい
  GFL_ASSERT(m_pLockedData != NULL);

  this->Update(m_pLockedData);
  this->GetInstanceAllocator()->SystemMemoryFree(m_pLockedData);
  m_pLockedData = NULL;
}

}}}

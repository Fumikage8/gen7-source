#include <gfx/include/gfl2_texture.h>

namespace gfl2 { namespace gfx {

Texture::Texture( const TextureDesc &rDesc ) : GLMemory()
{
	m_TextureDesc = rDesc;

	for( u32 i = 0; i < SurfaceMax; ++i )
		m_pSurface[i] = NULL;
}

Texture::~Texture()
{
	for( u32 i = 0; i < SurfaceMax; ++i )
	{
		if ( m_pSurface[i] )
		{
			delete m_pSurface[i];
			m_pSurface[i] = NULL;
		}
	}
}

}}

#include <clr/include/gfx/gfl2_Texture_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {

Texture::!Texture()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = NULL;
	}
}


}}}
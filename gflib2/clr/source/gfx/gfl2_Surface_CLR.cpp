#include <clr/include/gfx/gfl2_Surface_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {

Surface::!Surface()
{
	if ( m_pNative )
	{
		if ( m_fReference == false )
		{
			delete m_pNative;
			m_pNative = NULL;
		}
	}
}


}}}
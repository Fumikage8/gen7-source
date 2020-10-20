#include <gfx/include/gfl2_Surface.h>

namespace gfl2 { namespace gfx {

Surface::Surface( const SurfaceDesc &rDesc ) : GLMemory()
{
	m_SurfaceDesc = rDesc;
}

Surface::~Surface()
{

}

}}

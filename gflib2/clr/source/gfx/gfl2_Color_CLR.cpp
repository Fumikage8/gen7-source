#include <clr/include/gfx/gfl2_Color_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {

Color::Color()
{
	m_pNative = new gfl2::gfx::Color();
}

Color::Color( const float r, const float g, const float b, const float a )
{
	m_pNative = new gfl2::gfx::Color( r, g, b, a );
}

Color::!Color()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = 0;
	}
}


}}}
#include <clr/include/gfx/gfl2_Shader_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {

Shader::!Shader()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = NULL;
	}
}


}}}
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLSurface.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {


WinOpenGLSurface::WinOpenGLSurface(const SurfaceDesc &rDesc) : 
	Surface(rDesc),
	m_IsTexture( false ),
	m_SetTargetSlotNo( 0 )
{
	glGenRenderbuffers( 1, &m_Handle );

	if ( m_SurfaceDesc.usage & Usage::RenderTarget )
	{
		if ( m_SurfaceDesc.multiSample == MultiSample::None )
      glNamedRenderbufferStorageEXT(m_Handle, GL_RGBA, rDesc.width, rDesc.height );	
		else
      glNamedRenderbufferStorageMultisampleEXT(m_Handle, 4, GL_RGBA, rDesc.width, rDesc.height );	
	}
	else if ( m_SurfaceDesc.usage & Usage::DepthStencil )
	{
		if ( m_SurfaceDesc.multiSample == MultiSample::None )
      glNamedRenderbufferStorageEXT(m_Handle, GL_DEPTH24_STENCIL8, rDesc.width, rDesc.height );
		else
      glNamedRenderbufferStorageMultisampleEXT(m_Handle, 4, GL_DEPTH24_STENCIL8, rDesc.width, rDesc.height );
	}

	m_fOwnership = true;
}

WinOpenGLSurface::WinOpenGLSurface( const SurfaceDesc &rDesc, GLuint handle, b32 isTexture ) :
	Surface(rDesc),
	m_Handle(handle),
	m_fOwnership( false ),
	m_IsTexture( isTexture )
{

}

WinOpenGLSurface::~WinOpenGLSurface()
{
	if ( m_fOwnership )
		glDeleteRenderbuffers( 1, &m_Handle );
}

}}}

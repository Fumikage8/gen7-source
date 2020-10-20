#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLPIXELSHADER_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLPIXELSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLPixelShader : public Shader
{
public:

	WinOpenGLPixelShader( const void* code, u32 size );
	virtual ~WinOpenGLPixelShader();

	virtual void Bind() const;

	const GLuint GetHandle() const { return m_hPsProgram; }

private:

	GLuint							m_hPsProgram;

};

}}}

#endif
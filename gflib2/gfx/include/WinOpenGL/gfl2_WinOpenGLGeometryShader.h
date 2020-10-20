#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLGEOMETRYSHADER_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLGEOMETRYSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
//#include <D3Dcompiler.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLGeometryShader : public Shader
{
public:

	WinOpenGLGeometryShader( const void* code, u32 size );
	virtual ~WinOpenGLGeometryShader();

	virtual void Bind() const;

	const GLuint GetHandle() const { return m_hGsProgram; }

private:
	GLuint							m_hGsProgram;
	

};

}}}

#endif
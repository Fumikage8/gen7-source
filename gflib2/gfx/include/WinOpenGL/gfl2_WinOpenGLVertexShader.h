#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLVERTEXSHADER_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLVERTEXSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLVertexShader : public Shader
{
public:

	static const u32 s_VertexShaderInputMax = 16;

	enum AttachType{
		Geometry = 0,
		Pixel,
		Max
	};

	WinOpenGLVertexShader( const void* code, u32 size, const VertexElement* pVertexElements );
	virtual ~WinOpenGLVertexShader();

	virtual void Bind() const;
	virtual void BindVertyexAttribute( const u32 index, const u32 bufferHandle ) const;
	const GLuint GetHandle() const { return m_hVsProgram; }

private:

	//GLuint						m_hProgram;
	GLuint						m_hVsProgram;
	//GLuint						m_hAttachProgram[AttachType::Max];
  GLuint            m_hVertexArray;
	u32								m_Strides[s_VertexShaderInputMax];
  u32               m_StreamNoList[s_VertexShaderInputMax];
};

}}}

#endif
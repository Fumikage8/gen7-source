#ifndef GFLIB2_GRX_CTR_CTRGEOMETRYSHADER_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRGEOMETRYSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRGeometryShader : public Shader
{
public:

	CTRGeometryShader( const void* code, u32 size );
	virtual ~CTRGeometryShader();

	virtual void Bind() const;

	const GLuint GetHandle() const { return m_hGsProgram; }

private:
	GLuint							m_hGsProgram;
	

};

}}}

#endif

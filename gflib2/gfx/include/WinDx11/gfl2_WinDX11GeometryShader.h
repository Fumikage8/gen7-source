#ifndef GFLIB2_GRX_WINDX11_WINDX11GEOMETRYSHADER_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11GEOMETRYSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>
#include <D3Dcompiler.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11GeometryShader : public Shader
{
public:

	WinDX11GeometryShader( const void* code, u32 size );
	virtual ~WinDX11GeometryShader();

	virtual void Bind() const;

private:
	AutoComPtr<ID3D11GeometryShader>					m_pGeometryShader;
	AutoComPtr<ID3D11ShaderReflection>		m_pReflector;

};

}}}

#endif
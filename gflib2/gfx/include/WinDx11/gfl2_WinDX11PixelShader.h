#ifndef GFLIB2_GRX_WINDX11_WINDX11PIXELSHADER_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11PIXELSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>
#include <D3Dcompiler.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11PixelShader : public Shader
{
public:

	WinDX11PixelShader( const void* code, u32 size );
	virtual ~WinDX11PixelShader();

	virtual void Bind() const;

private:
	AutoComPtr<ID3D11PixelShader>					m_pPixelShader;
	AutoComPtr<ID3D11ShaderReflection>		m_pReflector;

};

}}}

#endif
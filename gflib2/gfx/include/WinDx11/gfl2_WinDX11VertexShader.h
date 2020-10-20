#ifndef GFLIB2_GRX_WINDX11_WINDX11VERTEXSHADER_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11VERTEXSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11VertexShader : public Shader
{
public:

	WinDX11VertexShader( const void* code, u32 size, const VertexElement* pVertexElements );
	virtual ~WinDX11VertexShader();

	virtual void Bind() const;

private:

	AutoComPtr<ID3D11VertexShader>			m_pVertexShader;
	AutoComPtr<ID3D11InputLayout>				m_pInputLayout;

};

}}}

#endif
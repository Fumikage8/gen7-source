#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11PixelShader.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11PixelShader::WinDX11PixelShader( const void* code, u32 size ) : Shader()
{
	HRESULT							hr;

	hr = GFGL::GetDevice()->CreatePixelShader( code, size, NULL, &m_pPixelShader);

	if (FAILED(hr))
		assert(0);

	 D3DReflect( code, size, IID_ID3D11ShaderReflection, (void**)&m_pReflector );
}

WinDX11PixelShader::~WinDX11PixelShader()
{
	m_pPixelShader.reset();
	m_pReflector.reset();
}

void WinDX11PixelShader::Bind() const
{
	GFGL::GetDeviceContext()->PSSetShader( m_pPixelShader.get(), NULL, 0);
}

}}}

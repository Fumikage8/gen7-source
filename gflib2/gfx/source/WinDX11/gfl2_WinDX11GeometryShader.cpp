#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11GeometryShader.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11GeometryShader::WinDX11GeometryShader( const void* code, u32 size ) : Shader()
{
	HRESULT							hr;

	hr = GFGL::GetDevice()->CreateGeometryShader( code, size, NULL, &m_pGeometryShader);

	if (FAILED(hr))
		assert(0);

	 D3DReflect( code, size, IID_ID3D11ShaderReflection, (void**)&m_pReflector );
}

WinDX11GeometryShader::~WinDX11GeometryShader()
{
	m_pGeometryShader.reset();
	m_pReflector.reset();
}

void WinDX11GeometryShader::Bind() const
{
	GFGL::GetDeviceContext()->GSSetShader( m_pGeometryShader.get(), NULL, 0);
}

}}}

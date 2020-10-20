#ifndef GFLIB2_GRX_WINDX11_WINDX11SURFACE_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11SURFACE_H_INCLUDED

#include <gfx/include/gfl2_Surface.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11Surface : public Surface
{
public:

	WinDX11Surface( const SurfaceDesc &rDesc );
	WinDX11Surface( const SurfaceDesc &rDesc, AutoComPtr<ID3D11Texture2D> pTexture, AutoComPtr<ID3D11RenderTargetView> pTRView, AutoComPtr<ID3D11DepthStencilView> pDepthView );
	virtual ~WinDX11Surface();

	inline AutoComPtr<ID3D11Texture2D>&				GetTargetTexture(){ return m_pTargetTexture; }
	inline AutoComPtr<ID3D11RenderTargetView>& GetRenderTargetView(){ return  m_pRenderTargetView; }
	inline AutoComPtr<ID3D11DepthStencilView>& GetDepthStencilView(){ return  m_pDepthStencilView; }

private:
	AutoComPtr<ID3D11Texture2D>					m_pTargetTexture;
	AutoComPtr<ID3D11RenderTargetView>	m_pRenderTargetView;
	AutoComPtr<ID3D11DepthStencilView>	m_pDepthStencilView;
};

}}}

#endif
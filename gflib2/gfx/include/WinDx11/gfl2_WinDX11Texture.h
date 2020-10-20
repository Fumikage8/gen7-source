#ifndef GFLIB2_GRX_WINDX11_WINDX11TEXTURE_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11TEXTURE_H_INCLUDED

#include <gfx/include/gfl2_texture.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11GL;

class WinDX11Texture : public Texture
{
public:

	friend class WinDX11GL;

	WinDX11Texture( const TextureDesc &rDesc );
	virtual ~WinDX11Texture();

	inline AutoComPtr<ID3D11Texture2D>&						GetTargetTexture(){ return m_pTargetTexture; }
	inline AutoComPtr<ID3D11ShaderResourceView>&	GetShaderResourceView(){ return  m_pShaderResourceView; }
	inline AutoComPtr<ID3D11RenderTargetView>&		GetRenderTargetView( u32 surfaceLv ){ return  m_pRenderTargetView[surfaceLv]; }

	inline const AutoComPtr<ID3D11Texture2D>&						GetTargetTexture() const { return m_pTargetTexture; }
	inline const AutoComPtr<ID3D11ShaderResourceView>&	GetShaderResourceView() const { return  m_pShaderResourceView; }
	inline const AutoComPtr<ID3D11RenderTargetView>&		GetRenderTargetView( u32 surfaceLv ) const { return  m_pRenderTargetView[surfaceLv]; }

	//! @brief テクスチャを更新する
	virtual void Update( const void* pData,	u32 depth = 0 );

private:
	AutoComPtr<ID3D11Texture2D>							m_pTargetTexture;
	AutoComPtr<ID3D11RenderTargetView>			m_pRenderTargetView[SurfaceMax];
	AutoComPtr<ID3D11ShaderResourceView>		m_pShaderResourceView;
};

}}}

#endif
#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Texture.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11Texture::WinDX11Texture(const TextureDesc &rDesc) : Texture(rDesc),
	m_pTargetTexture(NULL),
	m_pShaderResourceView(NULL)
{
	HRESULT									hr;	
	D3D11_TEXTURE2D_DESC		sTexDesc;

	ZeroMemory( &sTexDesc, sizeof(sTexDesc) );

	sTexDesc.Width							= rDesc.width;
	sTexDesc.Height							= rDesc.height;
	sTexDesc.MipLevels					= rDesc.mipLevels;
	sTexDesc.ArraySize					= 1;
	sTexDesc.Format							= s_d3d11FormatTable[rDesc.format];
	sTexDesc.SampleDesc.Count		= 1;
	sTexDesc.SampleDesc.Quality	= 0;
	sTexDesc.Usage							= D3D11_USAGE_DEFAULT;
	sTexDesc.BindFlags					= D3D11_BIND_SHADER_RESOURCE;
	sTexDesc.CPUAccessFlags			= 0;
	sTexDesc.MiscFlags					= 0;

	if ( rDesc.usage & Usage::RenderTarget )
		sTexDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	
	assert( sTexDesc.Format != DXGI_FORMAT_UNKNOWN );
	hr = GFGL::GetDevice()->CreateTexture2D( &sTexDesc, NULL, &m_pTargetTexture );

	if (FAILED(hr))
		assert( 0 );

	if ( rDesc.usage & Usage::RenderTarget )
	{
		hr = GFGL::GetDevice()->CreateRenderTargetView( m_pTargetTexture.get(),  NULL, &m_pRenderTargetView[0]);//@node とりあえず１枚のみ対応
		if (FAILED(hr))
			assert(0);

		// 描画可能テクスチャのシェーダ リソース ビューの作成
		D3D11_SHADER_RESOURCE_VIEW_DESC		srDesc;

		srDesc.Format                    = sTexDesc.Format; // テクスチャのフォーマットを指定
		srDesc.ViewDimension             = D3D_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels       = 1;

		hr = GFGL::GetDevice()->CreateShaderResourceView( m_pTargetTexture.get(), &srDesc, &m_pShaderResourceView);

		if (FAILED(hr))
			assert( 0 );

		Surface::SurfaceDesc				desc;

		desc.usage = Usage::RenderTarget | Usage::Tiled;
		desc.pool = rDesc.pool;
		desc.width = rDesc.width;
		desc.height = rDesc.height;
		desc.depth = 1;
		desc.format = rDesc.format;
		desc.multiSample = rDesc.multiSample;

		m_pSurface[0] = new WinDX11Surface( desc, AutoComPtr<ID3D11Texture2D>(NULL), m_pRenderTargetView[0], AutoComPtr<ID3D11DepthStencilView>(NULL) );
	}
	else
	{
		hr = GFGL::GetDevice()->CreateShaderResourceView( m_pTargetTexture.get(), NULL, &m_pShaderResourceView);

		if (FAILED(hr))
			assert( 0 );
	}
}

WinDX11Texture::~WinDX11Texture()
{
	m_pTargetTexture.reset();
	m_pShaderResourceView.reset();
	//m_pRenderTargetViewは規定クラスのm_pSurfaceの開放により削除される。
}

//! @brief テクスチャを更新する
void WinDX11Texture::Update( const void* pData, u32 depth	)
{
	u32	width = m_TextureDesc.width >> depth;
	u32	height = m_TextureDesc.height >> depth;

	width = math::max<u32>(width, 1);
	height = math::max<u32>(height, 1);

	u32	srcDepthPitch = GFGLUtil::GetImageSizeByFormat( width, height, m_TextureDesc.format );

	switch( m_TextureDesc.format ){
	case Format::DXT1:
	case Format::DXT3:
	case Format::DXT5:
		height /= 4;
		height = math::max<u32>(height, 1);
		break;
	}
	
	u32	srcRowPitch = srcDepthPitch / height;

	GFGL::GetDeviceContext()->UpdateSubresource( m_pTargetTexture.get(), depth, NULL, pData, srcRowPitch, srcDepthPitch );
}

}}}

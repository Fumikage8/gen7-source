#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Surface.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11Surface::WinDX11Surface( const SurfaceDesc &rDesc, AutoComPtr<ID3D11Texture2D> pTexture, AutoComPtr<ID3D11RenderTargetView> pTRView, AutoComPtr<ID3D11DepthStencilView> pDepthView ) :
	Surface(rDesc),
	m_pTargetTexture(pTexture),
	m_pRenderTargetView(pTRView),
	m_pDepthStencilView(pDepthView)
{
	int			hoge = 0;
	hoge ++;
}

WinDX11Surface::WinDX11Surface(const SurfaceDesc &rDesc) : 
	Surface(rDesc)
{
	HRESULT									hr;
	D3D11_TEXTURE2D_DESC		desc;

	desc.Width							= rDesc.width;   // 幅
	desc.Height							= rDesc.height;  // 高さ
	desc.MipLevels					= 1;       // ミップマップ・レベル数
	desc.ArraySize					= 1;       // 配列サイズ
	desc.SampleDesc.Count		= 1;  // マルチサンプリングの設定
	desc.SampleDesc.Quality	= 0;  // マルチサンプリングの品質
	desc.Usage							= D3D11_USAGE_DEFAULT;      // デフォルト使用法
	desc.CPUAccessFlags			= 0;   // CPUからはアクセスしない
	desc.MiscFlags					= 0;   // その他の設定なし
	desc.Format							= s_d3d11FormatTable[rDesc.format];  // フォーマット(深度のみ)
	desc.BindFlags					= ( rDesc.usage & Usage::DepthStencil ) ? D3D11_BIND_DEPTH_STENCIL : D3D11_BIND_RENDER_TARGET;

	hr = GFGL::GetDevice()->CreateTexture2D( &desc, NULL, &m_pTargetTexture);

	if (FAILED(hr))
		assert(0);

	if ( rDesc.usage & Usage::DepthStencil )
	{// 深度/ステンシル・ビューの作成
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

		descDSV.Format             = desc.Format;
		descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Flags              = 0;
		descDSV.Texture2D.MipSlice = 0;

		hr = GFGL::GetDevice()->CreateDepthStencilView( m_pTargetTexture.get(), &descDSV, &m_pDepthStencilView);

		if (FAILED(hr))
			assert(0);
	}
	else
	{
		hr = GFGL::GetDevice()->CreateRenderTargetView( m_pTargetTexture.get(),  NULL, &m_pRenderTargetView);

		if (FAILED(hr))
			assert(0);
	}
	
}

WinDX11Surface::~WinDX11Surface()
{
	m_pTargetTexture.reset();
	m_pDepthStencilView.reset();
	m_pRenderTargetView.reset();
}

}}}

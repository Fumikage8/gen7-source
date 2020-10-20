#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11StateObject.h>

namespace gfl2 { namespace gfx { namespace windx11 {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinDX11BlendStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinDX11BlendStateObject::WinDX11BlendStateObject() : BlendStateObject(),
	m_pBlendState(NULL)
{
}

WinDX11BlendStateObject::~WinDX11BlendStateObject()
{
	m_pBlendState.reset();
}

void WinDX11BlendStateObject::UpdateState()
{
	assert( GFGL::IsSceneDrawing() == false );

	HRESULT				hr;
	D3D11_BLEND_DESC BlendState;

	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	//BlendState.AlphaToCoverageEnable    = m_AlphaToCoverageEnable;
	BlendState.AlphaToCoverageEnable    = FALSE;
	BlendState.IndependentBlendEnable   = FALSE;

	for( u32 i = 0; i < RenderTargetMax; ++i )
	{
		BlendState.RenderTarget[i].BlendEnable           = m_StateData.m_BlendEnable;
		BlendState.RenderTarget[i].RenderTargetWriteMask = m_StateData.m_RenderTargetWriteMask[i];

		{
			BlendState.RenderTarget[i].BlendOp = s_d3d11BlendOp[ m_StateData.m_ColorBlendOp ];
			BlendState.RenderTarget[i].BlendOpAlpha = s_d3d11BlendOp[ m_StateData.m_AlphaBlendOp ];
		}

		{
			BlendState.RenderTarget[i].SrcBlend = s_d3d11BlendFunc[ m_StateData.m_SrcColorBlendFunc ];
			BlendState.RenderTarget[i].DestBlend = s_d3d11BlendFunc[ m_StateData.m_DestColorBlendFunc ];
			BlendState.RenderTarget[i].SrcBlendAlpha = s_d3d11BlendFunc[ m_StateData.m_SrcAlphaBlendFunc ];
			BlendState.RenderTarget[i].DestBlendAlpha = s_d3d11BlendFunc[ m_StateData.m_DestAlphaBlendFunc ];
		}
	}

	m_pBlendState.reset();
	hr = GFGL::GetDevice()->CreateBlendState(&BlendState, &m_pBlendState );

	if (FAILED(hr))
		assert( 0 );
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinDX11RasterizerStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinDX11RasterizerStateObject::WinDX11RasterizerStateObject() : RasterizerStateObject(),
	m_pRasterizerState(NULL)
{
}

WinDX11RasterizerStateObject::~WinDX11RasterizerStateObject()
{
	m_pRasterizerState.reset();
}

void WinDX11RasterizerStateObject::UpdateState()
{
	assert( GFGL::IsSceneDrawing() == false );

	HRESULT									hr;
	D3D11_RASTERIZER_DESC		RSDesc;

	RSDesc.FillMode							 = s_d3d11FillMode[m_StateData.m_FillMode];
	RSDesc.CullMode							 = s_d3d11CullMode[m_StateData.m_CullMode];
	RSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	RSDesc.DepthBias             = m_StateData.m_DepthBias;
	RSDesc.DepthBiasClamp        = 0;
	RSDesc.SlopeScaledDepthBias  = 0;
	RSDesc.DepthClipEnable       = TRUE;
	RSDesc.ScissorEnable         = m_StateData.m_ScissorTestEnable;
	RSDesc.MultisampleEnable     = m_StateData.m_MultisampleEnable;
	RSDesc.AntialiasedLineEnable = FALSE;

	m_pRasterizerState.reset();
	hr = GFGL::GetDevice()->CreateRasterizerState(&RSDesc, &m_pRasterizerState);

	if (FAILED(hr))
		assert( 0 );
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinDX11DepthStencilStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinDX11DepthStencilStateObject::WinDX11DepthStencilStateObject() : DepthStencilStateObject(),
	m_pDepthStencilState(NULL)
{
}

WinDX11DepthStencilStateObject::~WinDX11DepthStencilStateObject()
{
	m_pDepthStencilState.reset();
}

void WinDX11DepthStencilStateObject::UpdateState()
{
	assert( GFGL::IsSceneDrawing() == false );

	HRESULT				hr;
	D3D11_DEPTH_STENCIL_DESC DepthStencil;

	DepthStencil.DepthEnable      = m_StateData.m_DepthTestEnable;
	DepthStencil.DepthWriteMask   = m_StateData.m_DepthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencil.DepthFunc        = s_d3d11CompareFunc[m_StateData.m_DepthFunc];
	DepthStencil.StencilEnable    = m_StateData.m_StencilTestEnable;
	DepthStencil.StencilReadMask  = 0xFF;
	DepthStencil.StencilWriteMask = m_StateData.m_SetStencilWriteMask;
			
	DepthStencil.FrontFace.StencilFailOp      = s_d3d11StencilOp[ m_StateData.m_StencilFailOp[PolygonFace::CCW] ];
	DepthStencil.FrontFace.StencilDepthFailOp = s_d3d11StencilOp[ m_StateData.m_StencilDepthFailOp[PolygonFace::CCW] ];
	DepthStencil.FrontFace.StencilPassOp      = s_d3d11StencilOp[ m_StateData.m_StencilPassOp[PolygonFace::CCW] ];
	DepthStencil.FrontFace.StencilFunc        = s_d3d11CompareFunc[ m_StateData.m_StencilFunc[PolygonFace::CCW] ];
	
	if ( m_StateData.m_TwoSideStencilTestEnable )
	{
		DepthStencil.BackFace.StencilFailOp      = s_d3d11StencilOp[ m_StateData.m_StencilFailOp[PolygonFace::CW] ];
		DepthStencil.BackFace.StencilDepthFailOp = s_d3d11StencilOp[ m_StateData.m_StencilDepthFailOp[PolygonFace::CW] ];
		DepthStencil.BackFace.StencilPassOp      = s_d3d11StencilOp[ m_StateData.m_StencilPassOp[PolygonFace::CW] ];
		DepthStencil.BackFace.StencilFunc        = s_d3d11CompareFunc[ m_StateData.m_StencilFunc[PolygonFace::CW] ];
	}
	else
	{
		DepthStencil.BackFace.StencilFailOp      = DepthStencil.FrontFace.StencilFailOp;
		DepthStencil.BackFace.StencilDepthFailOp = DepthStencil.FrontFace.StencilDepthFailOp;
		DepthStencil.BackFace.StencilPassOp      = DepthStencil.FrontFace.StencilPassOp;
		DepthStencil.BackFace.StencilFunc        = DepthStencil.FrontFace.StencilFunc;
	}

	m_pDepthStencilState.reset();
	hr = GFGL::GetDevice()->CreateDepthStencilState( &DepthStencil, &m_pDepthStencilState );
	if (FAILED(hr))
		assert( 0 );

}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinDX11DepthStencilStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinDX11SamplerStateObject::WinDX11SamplerStateObject() : SamplerStateObject(),
	m_pTextureSampler( NULL )
{
	
}

WinDX11SamplerStateObject::~WinDX11SamplerStateObject()
{
	m_pTextureSampler.reset();
}

void WinDX11SamplerStateObject::UpdateState()
{
	HRESULT				hr;

	// サンプラーの作成
	D3D11_SAMPLER_DESC descSampler;

	descSampler.Filter          = s_d3d11TextureFilter[m_StateData.m_MinFilter][m_StateData.m_MagFilter][m_StateData.m_MipFilter];
	descSampler.AddressU        = s_d3d11TextureAddress[ m_StateData.m_WrapS ];
	descSampler.AddressV        = s_d3d11TextureAddress[ m_StateData.m_WrapT ];
	descSampler.AddressW        = s_d3d11TextureAddress[ m_StateData.m_WrapR ];
	descSampler.MipLODBias      = m_StateData.m_MipmapLodBias;
	descSampler.MaxAnisotropy   = m_StateData.m_MaxAniso;
	descSampler.ComparisonFunc  = s_d3d11CompareFunc[ m_StateData.m_SampleCmp ];
	descSampler.BorderColor[0]  = m_StateData.m_BorderColor.x;
	descSampler.BorderColor[1]  = m_StateData.m_BorderColor.y;
	descSampler.BorderColor[2]  = m_StateData.m_BorderColor.z;
	descSampler.BorderColor[3]  = m_StateData.m_BorderColor.w;
	descSampler.MinLOD          = m_StateData.m_MinLod;
	descSampler.MaxLOD          = m_StateData.m_MaxLod;

	m_pTextureSampler.reset();

	hr = GFGL::GetDevice()->CreateSamplerState(&descSampler, &m_pTextureSampler);
	if (FAILED(hr))
		assert( 0 );
}




}}}

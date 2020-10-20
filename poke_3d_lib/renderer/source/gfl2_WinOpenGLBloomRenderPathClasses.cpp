#if defined(GF_PLATFORM_WIN32)


//*************************************************************************************************
/**
 @file		BloomRenderPath.cpp
 @brief		ブルーム処理
 @date		2015.01.30
 @author	yuya_ikeuchi
 */
//*************************************************************************************************

#include "gfl2_WinOpenGLBloomRenderPathClasses.h"

#include <util/include/gfl2_GLHeapAllocator.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

namespace poke_3d { namespace renderer {

//----------------------------------------------------------------------------------------
//【Bloom : 1パス目】輝度抽出用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForLuminance::BloomRenderPathForLuminance(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, u32 screanWidth, u32 screanHeight) : IRenderPath()
{
	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		//m_pVertexShader			= GFGL::CreateVertexShaderFromFile( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomLuminance.gfvsh" , "VS", element, NULL, FileFormat::ShaderCode );
		//m_pPixelShader			= GFGL::CreatePixelShaderFromFile ( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomLuminance.gffsh" , "PS", NULL, FileFormat::ShaderCode );
    m_pVertexShader			= GFGL::CreateVertexShaderFromMemory( glHeapAllocator, vertexShaderCode, vertexShaderSize, element );
		m_pPixelShader			= GFGL::CreatePixelShaderFromMemory ( glHeapAllocator, pixelShaderCode , pixelShaderSize );
  }

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer( glHeapAllocator, sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject( glHeapAllocator );
	m_pBlendStateObject->SetAlphaTestEnable(true);
	m_pBlendStateObject->SetAlphaRef(0.01f);
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject( glHeapAllocator );
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject( glHeapAllocator );
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject( glHeapAllocator );
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();

	// フラグメント用定数バッファ
	{
		ConstantBufferElement element[] = 
		{
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};
		m_pConstantBufferFs = GFGL::CreateConstantBuffer( glHeapAllocator,  element, Usage::WriteOnly, Pool::Default );
	}

#if USE_SCALE_BUFFER
	// 輝度抜き出し用のレンダーバッファを作成
	m_pTextureForLuminance = GFGL::CreateTexture( glHeapAllocator, screanWidth/START_SCALE_CNT , screanHeight/START_SCALE_CNT , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#else
	int rate = (BloomRenderPathForScale::SCALE_CNT) * 2 + START_SCALE_CNT;
	// 輝度抜き出し用のレンダーバッファを作成
	m_pTextureForLuminance = GFGL::CreateTexture( glHeapAllocator, screanWidth/rate , screanHeight/rate , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#endif

	// グレア対象のテクスチャを作成
	m_pTargetTexture = GFGL::CreateTexture( glHeapAllocator, screanWidth ,screanHeight , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );	

	m_weight	= gfl2::math::Vector4(0.298912f, 0.586611f , 0.1144780f, 0.66f);
	m_mulColor	= gfl2::math::Vector4(1.0f ,1.0f ,1.0f ,0.0f);
}

void BloomRenderPathForLuminance::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	// ブロックコピー(グレア対象となる画像を取得)
	GFGL::BlitSurfaceToTexture( rDrawContext.m_pRenderTarget , m_pTargetTexture  );

	//------------------------------------------------
	// 輝度テクスチャの作成
	//------------------------------------------------

	// 輝度用サーフェスをセットして描画開始
	GFGL::SetRenderTarget( m_pTextureForLuminance->GetSurface() );
	GFGL::SetDepthStencil( NULL );
	GFGL::BeginScene();

	// サーフェスを初期化
	GFGL::ClearRenderTarget( m_pTextureForLuminance->GetSurface(),gfl2::gfx::Color::GetBlack() );

	const gfx::Surface::SurfaceDesc &rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	// フラグメント用定数バッファの更新
	{
		PixelShaderVectorConstants psVectorConstants;
		psVectorConstants.cConstantWeight   = m_weight;
		psVectorConstants.cConstantMulColor = m_mulColor;
		m_pConstantBufferFs->Update(&psVectorConstants, sizeof(psVectorConstants));
	}
	
	// シェーダと定数バッファのセット
	GFGL::SetVertexShader( m_pVertexShader );
	GFGL::SetPixelShader( m_pPixelShader );
	GFGL::SetPixelShaderConstant ( 8, m_pConstantBufferFs );

	// ポリゴンとレンダーステートのセット
	GFGL::SetIndices( m_pIndexBuffer );	
	GFGL::SetBlendState( m_pBlendStateObject );
	GFGL::SetRasterizerState( m_RasterizerStateObject );
	GFGL::SetDepthStencilState( m_DepthStencilStateObject );
	GFGL::SetTexture( 0 , m_pTargetTexture );
	GFGL::SetTexture( 1 , m_pAlphaMasTexture );
	GFGL::SetSamplerState( 0 , m_SamplerStateObject );
	GFGL::SetSamplerState( 1 , m_SamplerStateObject );

	// ポリゴン描画
	GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

	GFGL::EndScene();
}

//----------------------------------------------------------------------------------------
//【Bloom : 2パス目】縮小用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForScale::BloomRenderPathForScale(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, gfl2::gfx::Texture *m_pOriginTexture) : IRenderPath()
{
	m_pTargetTexture = m_pOriginTexture;

	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		//m_pVertexShader = GFGL::CreateVertexShaderFromFile( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomScale.gfvsh", "VS", element, NULL, FileFormat::ShaderCode );
		//m_pPixelShader  = GFGL::CreatePixelShaderFromFile ( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomScale.gffsh", "PS", NULL, FileFormat::ShaderCode );
    m_pVertexShader			= GFGL::CreateVertexShaderFromMemory( glHeapAllocator, vertexShaderCode, vertexShaderSize, element );
		m_pPixelShader			= GFGL::CreatePixelShaderFromMemory ( glHeapAllocator, pixelShaderCode , pixelShaderSize );
  }

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer( glHeapAllocator, sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject(glHeapAllocator);
	m_pBlendStateObject->SetBlendEnable(false);
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject(glHeapAllocator);
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject(glHeapAllocator);
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject(glHeapAllocator);
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();

	for( int i=0 ; i<SCALE_CNT ; i++ )
	{
		// 縮小用のレンダーバッファを作成
		m_pTextureForScale[i] = GFGL::CreateTexture(glHeapAllocator, m_pOriginTexture->GetTextureDesc().width / ((i*2)+2) , m_pOriginTexture->GetTextureDesc().height / ((i*2)+2) , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
	}
}

void BloomRenderPathForScale::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	for( int i=0; i<SCALE_CNT ; i++ )
	{
		// 輝度用サーフェスをセットして描画開始
		GFGL::SetRenderTarget( m_pTextureForScale[i]->GetSurface() );
		GFGL::SetDepthStencil( NULL );				
		GFGL::BeginScene();

		const gfx::Surface::SurfaceDesc &rDesc1 = GFGL::GetRenderTarget()->GetSurfaceDesc();
		GFGL::SetViewport(0, 0, rDesc1.width, rDesc1.height);

		// シェーダのセット
		GFGL::SetVertexShader( m_pVertexShader );
		GFGL::SetPixelShader( m_pPixelShader );

		// ポリゴンとレンダーステートのセット
		GFGL::SetIndices( m_pIndexBuffer );		
		GFGL::SetBlendState( m_pBlendStateObject );
		GFGL::SetRasterizerState( m_RasterizerStateObject );
		GFGL::SetDepthStencilState( m_DepthStencilStateObject );
		if( i== 0 ){ GFGL::SetTexture( 0 , m_pTargetTexture );        }
		else       { GFGL::SetTexture( 0 , m_pTextureForScale[i-1] ); }
		GFGL::SetSamplerState( 0 , m_SamplerStateObject );

		// ポリゴン描画
		GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

		GFGL::EndScene();
	}
}

//----------------------------------------------------------------------------------------
//【Bloom : 3パス目】ブラー用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForBlur::BloomRenderPathForBlur(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, gfl2::gfx::Texture *m_pOriginTexture) : IRenderPath()
{
	m_pTargetTexture = m_pOriginTexture;

	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		//m_pVertexShader = GFGL::CreateVertexShaderFromFile( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomBlur.gfvsh", "VS", element, NULL, FileFormat::ShaderCode );
		//m_pPixelShader  = GFGL::CreatePixelShaderFromFile ( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomBlur.gffsh", "PS", NULL, FileFormat::ShaderCode );
	  m_pVertexShader			= GFGL::CreateVertexShaderFromMemory( glHeapAllocator, vertexShaderCode, vertexShaderSize, element );
		m_pPixelShader			= GFGL::CreatePixelShaderFromMemory ( glHeapAllocator, pixelShaderCode , pixelShaderSize );
  }

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer(glHeapAllocator, sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject(glHeapAllocator);
	m_pBlendStateObject->SetBlendEnable(true);
	m_pBlendStateObject->SetBlendOp( BlendOp::Add );
	m_pBlendStateObject->SetBlendFunc( BlendFunc::SrcAlpha, BlendFunc::InverseSrcAlpha );
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject(glHeapAllocator);
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject(glHeapAllocator);
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject(glHeapAllocator);
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();

	// 頂点用定数バッファ
	{
		ConstantBufferElement element[] = 
		{
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};
		m_pConstantBufferVs = GFGL::CreateConstantBuffer(glHeapAllocator,  element, Usage::WriteOnly, Pool::Default );
	}

	// 重ね用のレンダーバッファを作成
	m_pTextureForBlur = GFGL::CreateTexture(glHeapAllocator, m_pOriginTexture->GetTextureDesc().width , m_pOriginTexture->GetTextureDesc().height , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );

	m_uvOffset[0] = gfl2::math::Vector4( 0.000f, 0.000f, 0.0f);
	m_uvOffset[1] = gfl2::math::Vector4( 0.000f,-0.004f, 0.0f);
	m_uvOffset[2] = gfl2::math::Vector4( 0.004f,-0.004f, 0.0f);
	m_uvOffset[3] = gfl2::math::Vector4( 0.004f, 0.000f, 0.0f);
	m_uvOffset[4] = gfl2::math::Vector4(-0.004f, 0.000f, 0.0f);
	m_uvOffset[5] = gfl2::math::Vector4(-0.004f,-0.004f, 0.0f);
	m_blur_cnt    = MAX_BLUR_CNT;
}

void BloomRenderPathForBlur::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	// サーフェスを初期化
	GFGL::SetRenderTarget( m_pTextureForBlur->GetSurface() );
	GFGL::SetDepthStencil( NULL );
	GFGL::BeginScene();

	GFGL::ClearRenderTarget( m_pTextureForBlur->GetSurface(),gfl2::gfx::Color::GetBlack() );

	// 輝度用サーフェスをセットして描画開始
	const gfx::Surface::SurfaceDesc &rDesc1 = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc1.width, rDesc1.height);

	for( int i=0; i<m_blur_cnt ; i+=3 )
	{
		// 頂点用定数バッファの更新
		{
			VertexShaderVectorConstants vsVectorConstants;
			vsVectorConstants.cOffset0.x = m_uvOffset[i+0].x;
			vsVectorConstants.cOffset0.y = m_uvOffset[i+0].y;
			vsVectorConstants.cOffset1.x = m_uvOffset[i+1].x;
			vsVectorConstants.cOffset1.y = m_uvOffset[i+1].y;
			vsVectorConstants.cOffset2.x = m_uvOffset[i+2].x;
			vsVectorConstants.cOffset2.y = m_uvOffset[i+2].y;
			m_pConstantBufferVs->Update(&vsVectorConstants, sizeof(vsVectorConstants));
		}

		// シェーダのセット
		GFGL::SetVertexShader( m_pVertexShader );
		GFGL::SetVertexShaderConstant( 0, m_pConstantBufferVs );
		GFGL::SetPixelShader( m_pPixelShader );

		// ポリゴンとレンダーステートのセット
		GFGL::SetIndices( m_pIndexBuffer );		
		GFGL::SetBlendState( m_pBlendStateObject );
		GFGL::SetRasterizerState( m_RasterizerStateObject );
		GFGL::SetDepthStencilState( m_DepthStencilStateObject );
		GFGL::SetTexture( 0 , m_pTargetTexture );
		GFGL::SetSamplerState( 0 , m_SamplerStateObject );

		// ポリゴン描画
		GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);
	}

	GFGL::EndScene();
}

//----------------------------------------------------------------------------------------
//【Bloom : 4パス目】加算合成用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForAdd::BloomRenderPathForAdd(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, gfl2::gfx::Texture* pTargetTexture,gfl2::gfx::Texture* pTextureForAdd) : IRenderPath()
{
	m_pTargetTexture = pTargetTexture;
	m_pTextureForAdd = pTextureForAdd;

	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		//m_pVertexShader = GFGL::CreateVertexShaderFromFile( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomAdd.gfvsh", "VS", element, NULL, FileFormat::ShaderCode );
		//m_pPixelShader  = GFGL::CreatePixelShaderFromFile ( "R:/home/momiji/exec/data_win32/data/app/test/kawada_test/not_conv/model_sample/shader/BloomAdd.gffsh", "PS", NULL, FileFormat::ShaderCode );
	  m_pVertexShader			= GFGL::CreateVertexShaderFromMemory( glHeapAllocator, vertexShaderCode, vertexShaderSize, element );
		m_pPixelShader			= GFGL::CreatePixelShaderFromMemory ( glHeapAllocator, pixelShaderCode , pixelShaderSize );
  }

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer(glHeapAllocator, sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject(glHeapAllocator);
	m_pBlendStateObject->SetBlendEnable(false);
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject(glHeapAllocator);
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject(glHeapAllocator);
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject(glHeapAllocator);
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();
}

void BloomRenderPathForAdd::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	// デフォルトのサーフェスをセットして描画開始
	GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
	GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
	GFGL::BeginScene();

	const gfx::Surface::SurfaceDesc &rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	// シェーダのセット
	GFGL::SetVertexShader( m_pVertexShader );
	GFGL::SetPixelShader( m_pPixelShader );

	// ポリゴンとレンダーステートのセット
	GFGL::SetIndices( m_pIndexBuffer );		
	GFGL::SetBlendState( m_pBlendStateObject );
	GFGL::SetRasterizerState( m_RasterizerStateObject );
	GFGL::SetDepthStencilState( m_DepthStencilStateObject );
	GFGL::SetTexture( 0 , m_pTargetTexture );
	GFGL::SetTexture( 1 , m_pTextureForAdd );
	GFGL::SetSamplerState( 0 , m_SamplerStateObject );
	GFGL::SetSamplerState( 1 , m_SamplerStateObject );

	// ポリゴン描画
	GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

	GFGL::EndScene();
}

}}


#endif  // GF_PLATFORM_WIN32

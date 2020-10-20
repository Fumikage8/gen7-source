#if defined(GF_PLATFORM_WIN32)


//*************************************************************************************************
/**
 @file		BloomRenderPath.cpp
 @brief		ブルーム処理
 @date		2015.01.30
 @author	yuya_ikeuchi
 */
//*************************************************************************************************

#ifndef BLOOM_RENDER_PATH_H_INCLUDED
#define BLOOM_RENDER_PATH_H_INCLUDED

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

namespace gfl2 { namespace util {
  class GLHeapAllocator;
} }

namespace poke_3d { namespace renderer {

// 縮小バッファを利用するかどうか
#define USE_SCALE_BUFFER 1

//----------------------------------------------------------------------------------------
//! @brief 【Bloom : 1パス目】輝度抽出用パス
//----------------------------------------------------------------------------------------
class BloomRenderPathForLuminance: public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

	// コンストラクタ
	BloomRenderPathForLuminance(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, u32 screanWidth, u32 screanHeight);
	// デストラクタ
	virtual ~BloomRenderPathForLuminance()
	{
		GFX_SAFE_DELETE( m_pBlendStateObject );
		GFX_SAFE_DELETE( m_RasterizerStateObject );
		GFX_SAFE_DELETE( m_DepthStencilStateObject );
		GFX_SAFE_DELETE( m_SamplerStateObject );
		GFX_SAFE_DELETE( m_pVertexShader );
		GFX_SAFE_DELETE( m_pPixelShader );
		GFX_SAFE_DELETE( m_pConstantBufferFs );
		GFX_SAFE_DELETE( m_pIndexBuffer );
		GFX_SAFE_DELETE( m_pTargetTexture );
		GFX_SAFE_DELETE( m_pTextureForLuminance );
	}
	// グレア対象となっているテクスチャを取得
	gfl2::gfx::Texture* GetTargetTexture(void)				{ return m_pTargetTexture; }
	// 輝度テクスチャを取得
	gfl2::gfx::Texture* GetLuminanceTexture(void)			{ return m_pTextureForLuminance; }
	// 輝度用ウェイトを取得
	gfl2::math::Vector4 getLuminanceWeight(void)				{ return m_weight; }
	// 抽出した輝度に対する係数を取得
	gfl2::math::Vector4 getLuminanceMulColor(void)			{ return gfl2::math::Vector4(m_mulColor.x,m_mulColor.y,m_mulColor.z,1.0f); }
	// 輝度の閾値を取得
	f32 getLuminanceThreshold(void)							{ return m_weight.w; }
	// 輝度用ウェイトを設定
	void setLuminanceWeight(gfl2::math::Vector4 weight)		{ m_weight.x = weight.x; m_weight.y = weight.y; m_weight.z = weight.z; }
	// 抽出した輝度に対する係数を設定
	void setLuminanceMulColor(gfl2::math::Vector4 mulColor)	{ m_mulColor.x = mulColor.x; m_mulColor.y = mulColor.y; m_mulColor.z = mulColor.z;}
	// 輝度の閾値を設定
	void setLuminanceThreshold(f32 threshold)				{ m_weight.w = threshold; }
	// 輝度テクスチャを設定
	void SetAlphaMaskTexture(const gfl2::gfx::Texture* pTexture)	{ m_pAlphaMasTexture = pTexture; }

public:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

private:

	static const int					START_SCALE_CNT = 2;			// グレア対象となるテクスチャから何分割の大きさで輝度テクスチャを作成するか

	gfl2::gfx::Shader					*m_pVertexShader;				// 頂点シェーダ
	gfl2::gfx::Shader					*m_pPixelShader;				// ピクセル・シェーダ
	gfl2::gfx::ConstantBuffer			*m_pConstantBufferFs;			// フラグメント用定数バッファ
	gfl2::gfx::IndexBuffer				*m_pIndexBuffer;				// インデックスバッファ
	gfl2::gfx::BlendStateObject			*m_pBlendStateObject;			// ブレンド・ステート・オブジェクト
	gfl2::gfx::RasterizerStateObject	*m_RasterizerStateObject;		// ラスタライザ・ステート・オブジェクト
	gfl2::gfx::DepthStencilStateObject	*m_DepthStencilStateObject;		// 深度/ステンシル・ステート・オブジェクト
	gfl2::gfx::SamplerStateObject		*m_SamplerStateObject;			// サンプラー・ステート・オブジェクト
	gfl2::gfx::Texture					*m_pTargetTexture;				// グレア対象となるテクスチャ
	gfl2::gfx::Texture					*m_pTextureForLuminance;		// 輝度テクスチャ
	gfl2::math::Vector4					 m_weight;						// xyz成分は輝度用ウェイト、w成分は輝度の閾値
	gfl2::math::Vector4					 m_mulColor;					// xyz成分は抽出した輝度に対する係数、w成分は空き
	const gfl2::gfx::Texture			*m_pAlphaMasTexture;			// ブルームの係り具合を制御するαマスク

	// フラグメント用定数バッファ
	struct PixelShaderVectorConstants
	{
		gfl2::math::Vector4 cConstantWeight;
		gfl2::math::Vector4 cConstantMulColor;
	};
};

//----------------------------------------------------------------------------------------
//! @brief 【Bloom : 2パス目】縮小用パス
//----------------------------------------------------------------------------------------
class BloomRenderPathForScale : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

	// コンストラクタ
	BloomRenderPathForScale(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, gfl2::gfx::Texture *m_pOriginTexture);
	// デストラクタ
	virtual ~BloomRenderPathForScale()
	{
		GFX_SAFE_DELETE( m_pBlendStateObject );
		GFX_SAFE_DELETE( m_RasterizerStateObject );
		GFX_SAFE_DELETE( m_DepthStencilStateObject );
		GFX_SAFE_DELETE( m_SamplerStateObject );
		GFX_SAFE_DELETE( m_pVertexShader );
		GFX_SAFE_DELETE( m_pPixelShader );
		GFX_SAFE_DELETE( m_pIndexBuffer );
		for( int i=0 ; i<SCALE_CNT ; i++ )
		{
			GFX_SAFE_DELETE( m_pTextureForScale[i] );
		}
	}
	// ターゲットを縮小したテクスチャを取得
	gfl2::gfx::Texture* GetMinScaleTexture(void){ return m_pTextureForScale[SCALE_CNT-1]; }

	static const int SCALE_CNT = 1;	// 縮小回数

public:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );	

private:

	gfl2::gfx::Shader					*m_pVertexShader;					// 頂点シェーダ
	gfl2::gfx::Shader					*m_pPixelShader;					// ピクセル・シェーダ
	gfl2::gfx::IndexBuffer				*m_pIndexBuffer;					// インデックスバッファ
	gfl2::gfx::BlendStateObject			*m_pBlendStateObject;				// ブレンド・ステート・オブジェクト
	gfl2::gfx::RasterizerStateObject	*m_RasterizerStateObject;			// ラスタライザ・ステート・オブジェクト
	gfl2::gfx::DepthStencilStateObject	*m_DepthStencilStateObject;			// 深度/ステンシル・ステート・オブジェクト
	gfl2::gfx::SamplerStateObject		*m_SamplerStateObject;				// サンプラー・ステート・オブジェクト
	gfl2::gfx::Texture					*m_pTargetTexture;					// 縮小対象となるテクスチャ
	gfl2::gfx::Texture					*m_pTextureForScale[SCALE_CNT];		// 縮小テクスチャ
};

//----------------------------------------------------------------------------------------
//! @brief 【Bloom : 3パス目】ブラー用パス
//----------------------------------------------------------------------------------------
class BloomRenderPathForBlur : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

	// コンストラクタ
	BloomRenderPathForBlur(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, gfl2::gfx::Texture *m_pOriginTexture);
	// デストラクタ
	virtual ~BloomRenderPathForBlur()
	{
		GFX_SAFE_DELETE( m_pBlendStateObject );
		GFX_SAFE_DELETE( m_RasterizerStateObject );
		GFX_SAFE_DELETE( m_DepthStencilStateObject );
		GFX_SAFE_DELETE( m_SamplerStateObject );
		GFX_SAFE_DELETE( m_pVertexShader );
		GFX_SAFE_DELETE( m_pPixelShader );
		GFX_SAFE_DELETE( m_pIndexBuffer );
		GFX_SAFE_DELETE( m_pTextureForBlur );
		GFX_SAFE_DELETE( m_pConstantBufferVs );
	}
	// ブラーテクスチャを取得
	gfl2::gfx::Texture* GetBlurTexture(void)	{ return m_pTextureForBlur; }
	// ブラー方向を取得
	gfl2::math::Vector4 getBlurOffset(u32 index)	{ return m_uvOffset[index]; }
	// ブラー回数を取得
	u32 getBlurCnt(void)						{ return m_blur_cnt; }
	// ブラー方向を設定
	void setBlurOffset(gfl2::math::Vector4& offset,u32 index){ m_uvOffset[index] = offset; }
	// ブラー回数を設定
	void setBlurCnt(u32 cnt)					{ m_blur_cnt = cnt; }

public:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

private:

	static const int MAX_BLUR_CNT = 6;

	gfl2::gfx::Shader					*m_pVertexShader;				// 頂点シェーダ
	gfl2::gfx::Shader					*m_pPixelShader;				// ピクセル・シェーダ
	gfl2::gfx::ConstantBuffer			*m_pConstantBufferVs;			// 頂点用定数バッファ
	gfl2::gfx::IndexBuffer				*m_pIndexBuffer;				// インデックスバッファ
	gfl2::gfx::BlendStateObject			*m_pBlendStateObject;			// ブレンド・ステート・オブジェクト
	gfl2::gfx::RasterizerStateObject	*m_RasterizerStateObject;		// ラスタライザ・ステート・オブジェクト
	gfl2::gfx::DepthStencilStateObject	*m_DepthStencilStateObject;		// 深度/ステンシル・ステート・オブジェクト
	gfl2::gfx::SamplerStateObject		*m_SamplerStateObject;			// サンプラー・ステート・オブジェクト
	gfl2::gfx::Texture					*m_pTargetTexture;				// ブラー対象となるテクスチャ
	gfl2::gfx::Texture					*m_pTextureForBlur;				// ブラーテクスチャ
	gfl2::math::Vector4					 m_uvOffset	[MAX_BLUR_CNT];		// テクスチャを合成する際のuvオフセット
	int									 m_blur_cnt;					// 重ねる回数

	// 頂点用定数バッファ
	struct VertexShaderVectorConstants
	{
		gfl2::math::Vector4 cOffset0;
		gfl2::math::Vector4 cOffset1;
		gfl2::math::Vector4 cOffset2;
	};
};

//----------------------------------------------------------------------------------------
//! @brief 【Bloom : 4パス目】加算合成用パス
//----------------------------------------------------------------------------------------
class BloomRenderPathForAdd : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
	
	// コンストラクタ
	BloomRenderPathForAdd(gfl2::util::GLHeapAllocator* glHeapAllocator, const void* vertexShaderCode, u32 vertexShaderSize, const void* pixelShaderCode, u32 pixelShaderSize, gfl2::gfx::Texture* pTargetTexture,gfl2::gfx::Texture* pTextureForAdd);
	// デストラクタ
	virtual ~BloomRenderPathForAdd()
	{
		GFX_SAFE_DELETE( m_pBlendStateObject );
		GFX_SAFE_DELETE( m_RasterizerStateObject );
		GFX_SAFE_DELETE( m_DepthStencilStateObject );
		GFX_SAFE_DELETE( m_SamplerStateObject );
		GFX_SAFE_DELETE( m_pVertexShader );
		GFX_SAFE_DELETE( m_pPixelShader );
		GFX_SAFE_DELETE( m_pIndexBuffer );
	}

public:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

private:

	gfl2::gfx::IndexBuffer				*m_pIndexBuffer;				// インデックスバッファ
	gfl2::gfx::Shader					*m_pVertexShader;				// 頂点シェーダ
	gfl2::gfx::Shader					*m_pPixelShader;				// ピクセル・シェーダ
	gfl2::gfx::BlendStateObject			*m_pBlendStateObject;			// ブレンド・ステート・オブジェクト
	gfl2::gfx::RasterizerStateObject	*m_RasterizerStateObject;		// ラスタライザ・ステート・オブジェクト
	gfl2::gfx::DepthStencilStateObject	*m_DepthStencilStateObject;		// 深度/ステンシル・ステート・オブジェクト
	gfl2::gfx::SamplerStateObject		*m_SamplerStateObject;			// サンプラー・ステート・オブジェクト
	gfl2::gfx::Texture					*m_pTargetTexture;				// 合成対象となるテクスチャ
	gfl2::gfx::Texture					*m_pTextureForAdd;				// 合成用テクスチャ
};

}}

#endif


#endif  // GF_PLATFORM_WIN32

#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

class EdgeMapSceneRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	class EdgeMapShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
	{
	public:
		EdgeMapShaderDriver();
		virtual ~EdgeMapShaderDriver();

		virtual b32 Apply( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag = NULL ) const;
		virtual void SetTextureOverride( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const;

		gfl2::gfx::Shader						*m_pVertexShader;
		gfl2::gfx::Shader						*m_pPixelShader;
		gfl2::gfx::ConstantBuffer		*m_pConstantBuffer;
		gfl2::gfx::BlendStateObject	*m_pBlendStateObject[3];    // ブレンド・ステート・オブジェクト

		gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
		gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode;
	};

	enum EdgeColor{
		Normal = 0,
		ID,
		Max
	};

	EdgeMapSceneRenderPath();
	virtual ~EdgeMapSceneRenderPath();

	void CreateEdgeMapTexture( f32 screanWidth, f32 screanHeight );

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	const gfl2::gfx::Texture* GetEdgeMapTexture( EdgeColor type ){ return m_pEdgeMapTexture[type]; }

private:
	virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride();
	virtual const gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
	virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
	EdgeMapShaderDriver																																							m_EdgeMapShaderDriver;
	gfl2::gfx::Texture																																							*m_pEdgeMapTexture[EdgeColor::Max];
};

class OutLinePostRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
	OutLinePostRenderPath(float screanSizeX, float screanSizeY);
	virtual ~OutLinePostRenderPath();

	void SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor type, const gfl2::gfx::Texture* pTexture);

private:
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	const gfl2::gfx::Texture						*m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Max];    // テクスチャ

	gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Max];
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Max];
};

// 縮小バッファを利用するかどうか
#define USE_SCALE_BUFFER 1

//----------------------------------------------------------------------------------------
//! @brief 【Bloom : 1パス目】輝度抽出用パス
//----------------------------------------------------------------------------------------
class BloomRenderPathForLuminance: public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

	// コンストラクタ
	BloomRenderPathForLuminance(u32 screanWidth, u32 screanHeight);
	// デストラクタ
	virtual ~BloomRenderPathForLuminance()
	{ 
		GFX_SAFE_DELETE( m_pVertexShader );
		GFX_SAFE_DELETE( m_pPixelShader );
		GFX_SAFE_DELETE( m_pAlphaMasTexture);
		GFX_SAFE_DELETE( m_pTargetTexture );
		GFX_SAFE_DELETE( m_pTextureForLuminance );
	}
	// グレア対象となっているテクスチャを取得
	gfl2::gfx::Texture* GetTargetTexture(void)				{ return m_pTargetTexture; }
	// 輝度テクスチャを取得
	gfl2::gfx::Texture* GetLuminanceTexture(void)			{ return m_pTextureForLuminance; }
	// 輝度用ウェイトを取得
	gfl2::math::Vector getLuminanceWeight(void)				{ return m_weight; }
	// 抽出した輝度に対する係数を取得
	gfl2::math::Vector getLuminanceMulColor(void)			{ return gfl2::math::Vector(m_mulColor.x,m_mulColor.y,m_mulColor.z,1.0f); }
	// 輝度の閾値を取得
	f32 getLuminanceThreshold(void)							{ return m_weight.w; }
	// 輝度用ウェイトを設定
	void setLuminanceWeight(gfl2::math::Vector& weight)		{ m_weight.x = weight.x; m_weight.y = weight.y; m_weight.z = weight.z; }
	// 抽出した輝度に対する係数を設定
	void setLuminanceMulColor(gfl2::math::Vector& mulColor)	{ m_mulColor.x = mulColor.x; m_mulColor.y = mulColor.y; m_mulColor.z = mulColor.z;}
	// 輝度の閾値を設定
	void setLuminanceThreshold(f32 threshold)				{ m_weight.w = threshold; }

private:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

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
	gfl2::gfx::Texture					*m_pAlphaMasTexture;			// ブルームの係り具合を制御するαマスク
	gfl2::math::Vector					 m_weight;						// xyz成分は輝度用ウェイト、w成分は輝度の閾値
	gfl2::math::Vector					 m_mulColor;					// xyz成分は抽出した輝度に対する係数、w成分は空き

	// フラグメント用定数バッファ
	struct PixelShaderVectorConstants
	{
		gfl2::math::Vector cConstantWeight;
		gfl2::math::Vector cConstantMulColor;
	};
};

//----------------------------------------------------------------------------------------
//! @brief 【Bloom : 2パス目】縮小用パス
//----------------------------------------------------------------------------------------
class BloomRenderPathForScale : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

	// コンストラクタ
	BloomRenderPathForScale(gfl2::gfx::Texture *m_pOriginTexture);
	// デストラクタ
	virtual ~BloomRenderPathForScale()
	{
		GFX_SAFE_DELETE( m_pVertexShader );
		GFX_SAFE_DELETE( m_pPixelShader );
		for( int i=0 ; i<SCALE_CNT ; i++ )
		{
			GFX_SAFE_DELETE( m_pTextureForScale[i] );
		}
	}
	// ターゲットを縮小したテクスチャを取得
	gfl2::gfx::Texture* GetMinScaleTexture(void){ return m_pTextureForScale[SCALE_CNT-1]; }

	static const int SCALE_CNT = 1;	// 縮小回数

private:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );	

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
	BloomRenderPathForBlur(gfl2::gfx::Texture *m_pOriginTexture);
	// デストラクタ
	virtual ~BloomRenderPathForBlur()
	{
		GFX_SAFE_DELETE( m_pVertexShader );
		GFX_SAFE_DELETE( m_pPixelShader );
		GFX_SAFE_DELETE( m_pTextureForBlur );
	}
	// ブラーテクスチャを取得
	gfl2::gfx::Texture* GetBlurTexture(void)	{ return m_pTextureForBlur; }
	// ブラー方向を取得
	gfl2::math::Vector getBlurOffset(u32 index)	{ return m_uvOffset[index]; }
	// ブラー回数を取得
	u32 getBlurCnt(void)						{ return m_blur_cnt; }
	// ブラー方向を設定
	void setBlurOffset(gfl2::math::Vector& offset,u32 index){ m_uvOffset[index] = offset; }
	// ブラー回数を設定
	void setBlurCnt(u32 cnt)					{ m_blur_cnt = cnt; }

private:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

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
	gfl2::math::Vector					 m_uvOffset	[MAX_BLUR_CNT];		// テクスチャを合成する際のuvオフセット
	int									 m_blur_cnt;					// 重ねる回数

	// 頂点用定数バッファ
	struct VertexShaderVectorConstants
	{
		gfl2::math::Vector cOffset0;
		gfl2::math::Vector cOffset1;
		gfl2::math::Vector cOffset2;
	};
};

//----------------------------------------------------------------------------------------
//! @brief 【Bloom : 4パス目】加算合成用パス
//----------------------------------------------------------------------------------------
class BloomRenderPathForAdd : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
	
	// コンストラクタ
	BloomRenderPathForAdd(gfl2::gfx::Texture* pTargetTexture,gfl2::gfx::Texture* pTextureForAdd);
	// デストラクタ
	virtual ~BloomRenderPathForAdd(){}

private:

	// 描画実行
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

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

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	MyRenderingPipeLine( f32 screanWidth, f32 screanHeight );
	virtual ~MyRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_SceneRenderPath.AddDrawEnv(pNode);
  }

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

	u32																																		m_RenderPathCnt;
	
	EdgeMapSceneRenderPath																								m_EdgeMapSceneRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
	OutLinePostRenderPath																									m_OutLinePostRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;

	BloomRenderPathForLuminance		m_BloomRenderPathForLuminace;
	BloomRenderPathForScale			m_BloomRenderPathForScale;
	BloomRenderPathForBlur			m_BloomRenderPathForBlur;
	BloomRenderPathForAdd			m_BloomRenderPathForAdd;
};

#endif

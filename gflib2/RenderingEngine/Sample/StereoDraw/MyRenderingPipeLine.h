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

	class EdgeColor{
	public:
		enum {
			Normal = 0,
			ID,
			Max
		};
	};

	EdgeMapSceneRenderPath();
	virtual ~EdgeMapSceneRenderPath();

	void CreateEdgeMapTexture( f32 screanWidth, f32 screanHeight );

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	const gfl2::gfx::Texture* GetEdgeMapTexture( u32 type ){ return m_pEdgeMapTexture[type]; }

private:
	virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride();
	virtual const gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
	virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
	EdgeMapShaderDriver																																							m_EdgeMapShaderDriver;
	gfl2::gfx::Texture																																							*m_pEdgeMapTexture[EdgeColor::Max];
};

class MainRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:

	MainRenderPath();
	virtual ~MainRenderPath();

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

private:
	virtual const gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;

};

class OutLinePostRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
	OutLinePostRenderPath(float screanSizeX, float screanSizeY);
	virtual ~OutLinePostRenderPath();

	void SetEdgeMapTexture( u32 type, const gfl2::gfx::Texture* pTexture);

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

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	MyRenderingPipeLine( f32 screanWidth, f32 screanHeight );
	virtual ~MyRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_MainRenderPath.AddDrawEnv(pNode);
  }

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void SetDepthTexture( gfl2::gfx::Texture* pTexture );

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

	u32																																		m_RenderPathCnt;
	
	EdgeMapSceneRenderPath																								m_EdgeMapSceneRenderPath;
	MainRenderPath																												m_MainRenderPath;
	//gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
	OutLinePostRenderPath																									m_OutLinePostRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;
};

#endif

#ifndef FieldTestMyRenderingPipeLine_H_INCLUDED
#define FieldTestMyRenderingPipeLine_H_INCLUDED

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include <util/include/gfl2_UtilTextRenderPath.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

// ※※※↓のマクロが0の場合はniji拡張分が実行されます。マージの際には気をつけてください yy-ikeuchi ※※※
#define USE_CUSTOM_CONTAINER 1  // カスタムコンテナを使うかどうか、使用しない場合はSceneGraphがもつAABBが利用可能

// ※※※↓のマクロが1の場合はniji拡張分が実行されます。マージの際には気をつけてください yy-ikeuchi  ※※※
#define USE_SKYBOX_BUFFER_CLEAR 1

// ※※※↓のマクロが1の場合はniji拡張分が実行されます。マージの際には気をつけてください yy-ikeuchi  ※※※
#define REMOVAL_OF_MEMORY_LEAK  1

#define SAMPLE_DIFF 1    // サンプルからの差分はこの定義の内部収める(できるかぎり) 150317 ikuta_junya

#define USE_LOCAL 1  // ローカル編集を参照

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
#if REMOVAL_OF_MEMORY_LEAK
    static const s32 BIN_DATA_CNT = 32;
    c8*  m_pBinaryData[BIN_DATA_CNT];
#endif
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
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	const gfl2::gfx::Texture* GetEdgeMapTexture( u32 type ){ return m_pEdgeMapTexture[type]; }

private:
	virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride();
	virtual const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
	virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

#if USE_CUSTOM_CONTAINER
	gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
#endif
	EdgeMapShaderDriver																																							m_EdgeMapShaderDriver;
	gfl2::gfx::Texture																																							*m_pEdgeMapTexture[EdgeColor::Max];
};

class MainRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:

	MainRenderPath();
	virtual ~MainRenderPath();

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

private:
	virtual const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
  virtual void SetDrawableNode();

#if USE_CUSTOM_CONTAINER
	gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
#endif

};

class OutLinePostRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
	OutLinePostRenderPath(float screanSizeX, float screanSizeY);
	virtual ~OutLinePostRenderPath();

	void SetEdgeMapTexture( u32 type, const gfl2::gfx::Texture* pTexture);

#if SAMPLE_DIFF
  void SetEnable( u32 edge, b32 flag ){ m_EnabledTable[ edge ] = flag; }
#endif

private:
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	const gfl2::gfx::Texture						*m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Max];    // テクスチャ

	gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Max];
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Max];

#if REMOVAL_OF_MEMORY_LEAK
  static const s32 BIN_DATA_CNT = 32;
  c8*  m_pBinaryData[BIN_DATA_CNT];
#endif

#if SAMPLE_DIFF
  b32       m_EnabledTable[EdgeMapSceneRenderPath::EdgeColor::Max];
#endif

};

#if defined(GF_PLATFORM_CTR)
	#define USE_EDGE 1
#else
	#define USE_EDGE 1
#endif

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:

#if SAMPLE_DIFF

  enum EdgeMode
  {
    EDGE_MODE_OFF = 0,    // エッジ無し
    EDGE_MODE_ID,         // IDのみ
    EDGE_MODE_NORMAL,     // 法線のみ
    EDGE_MODE_FULL,       // 法線+ID
  };

  void SetEdgeMode( EdgeMode edgeMode );
  EdgeMode GetEdgeMode( void ) const { return m_EdgeMode; }

#endif // #if SAMPLE_DIFF

	//----------------------------------------------------------------------------
	/**
	*  @brief  コンストラクタ
	*/
	//-----------------------------------------------------------------------------
	MyRenderingPipeLine( f32 screanWidth, f32 screanHeight );
	
	//----------------------------------------------------------------------------
	/**
	*  @brief  デストラクタ
	*/
	//-----------------------------------------------------------------------------
	virtual ~MyRenderingPipeLine();

	//----------------------------------------------------------------------------
	/**
	*  @brief  最初のパスから描画できる状態にする
	*/
	//-----------------------------------------------------------------------------
	virtual b32 StartRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  現状のパスにあったシーンを返す
	*  @return シーン
	*/
	//-----------------------------------------------------------------------------
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  次のパスへ遷移
	*  @return trueなら次のパス、falseなら描画終了
	*/
	//-----------------------------------------------------------------------------
	virtual b32 NextRenderPath(void);

public:

	//----------------------------------------------------------------------------
	/**
	*  @brief  レイアウトのレンダーパスを取得する
	*/
	//-----------------------------------------------------------------------------
	gfl2::lyt::LayoutRenderPath* GetLayoutRenderPath( void ) { return &m_LayoutRenderPath; }

	//----------------------------------------------------------------------------
	/**
	*  @brief  エフェクトのレンダーパスを取得する
	*/
	//-----------------------------------------------------------------------------
	gfl2::Effect::EffectRenderPath* GetEffectRenderPath( void ) { return &m_EffectRenderPath; }

	//----------------------------------------------------------------------------
	/**
	*  @brief  シーンパスにライト等の環境ノードを追加
	*  @param  pNode 環境ノード
	*/
	//-----------------------------------------------------------------------------
	void MyRenderingPipeLine::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode ){ m_MainRenderPath.AddDrawEnv( pNode ); }

	//----------------------------------------------------------------------------
	/**
	*  @brief  指定モデルインスタンスにエッジを描画する
	*  @param  pNode モデルインスタンス
	*/
	//-----------------------------------------------------------------------------
	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
	{
#if USE_EDGE
		m_EdgeMapSceneRenderPath.AddEdgeRenderingTarget( pNode );
#endif
		m_MainRenderPath.AddEdgeRenderingTarget( pNode );
	}

	//----------------------------------------------------------------------------
	/**
	*  @brief  指定モデルインスタンスにエッジを描画する
	*  @param  pNode モデルインスタンス
	*/
	//-----------------------------------------------------------------------------
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
	{
#if USE_EDGE
		m_EdgeMapSceneRenderPath.RemoveEdgeRenderingTarget( pNode );
#endif
		m_MainRenderPath.RemoveEdgeRenderingTarget( pNode );
	}

	//----------------------------------------------------------------------------
	/**
	*  @brief  アウトライン用デプステクスチャの設定
	*  @param  pTexture 書き込みに利用するテクスチャ
	*/
	//-----------------------------------------------------------------------------
	void SetDepthTexture( gfl2::gfx::Texture* pTexture )
	{
#if defined(GF_PLATFORM_CTR)
		m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	
#endif
	}

    void ExecuteTest( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
    void UpdateTest( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  //----------------------------------------------------------------------------
  /**
  *  @brief  DrawUtilTextのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  gfl2::util::UtilTextRenderPath* GetUtilTextRenderPath( void ) { return &m_UtilTextRenderPath; }

private:
  u32                                    m_RenderPathCnt;
  EdgeMapSceneRenderPath                 m_EdgeMapSceneRenderPath;
  MainRenderPath                         m_MainRenderPath;
  OutLinePostRenderPath                  m_OutLinePostRenderPath;
  gfl2::lyt::LayoutRenderPath            m_LayoutRenderPath;        // Layoutも描画できるように  @todo kawada_koji_20150209
  gfl2::Effect::EffectRenderPath         m_EffectRenderPath;        // エフェクト
  gfl2::util::UtilTextRenderPath         m_UtilTextRenderPath;

#if SAMPLE_DIFF
  EdgeMode m_EdgeMode;
#endif

};

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#endif // #ifndef FieldTestMyRenderingPipeLine_H_INCLUDED

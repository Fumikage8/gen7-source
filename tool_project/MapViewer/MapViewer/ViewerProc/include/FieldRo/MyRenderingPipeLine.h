#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <macro/include/gfl2_Macros.h>

#define FIELD_CUSTOM 1

#if FIELD_CUSTOM
#include <util/include/gfl2_UtilTextRenderPath.h>
#include <renderer/include/gfl2_BloomRenderPath.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#endif


// 前方宣言
GFL_NAMESPACE_BEGIN( System )
class NijiOutLinePostSceneRenderPath;
class NijiEdgeMapSceneRenderPath;
class nijiAllocator;
GFL_NAMESPACE_END( System )

GFL_NAMESPACE_BEGIN( Field );
class FieldNode;


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// ↓のマクロが1の場合はniji拡張分が実行されます。マージの際には気をつけてください yy-ikeuchi  ※※※
#define REMOVAL_OF_MEMORY_LEAK 1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

#if FIELD_CUSTOM
class SkyBoxRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	SkyBoxRenderPath();
	virtual ~SkyBoxRenderPath();
	void AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride(void);
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* GetCameraParamAddress(void){ return &m_camera; }
  void CreateNode( gfl2::gfx::IGLAllocator *pAllocator );
  void DeleteNode();
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void Update ( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>	m_DrawableNodeContainer;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera m_camera;
  gfl2::renderingengine::scenegraph::instance::TransformNode                  *m_node;
};

class OrthoRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	OrthoRenderPath();
	virtual ~OrthoRenderPath();
	void AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride(void);
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* GetCameraParamAddress(void){ return &m_camera; }
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void Update ( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>	m_DrawableNodeContainer;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera m_camera;
};
#endif

class MainRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:

	MainRenderPath();
	virtual ~MainRenderPath();

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

#if FIELD_CUSTOM
	void AddDrawEnvForFog(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode){ m_DrawEnvNodeContainerForFog.push_back(pDrawEnvNode); }
	void ApplyFogParam(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode);
#endif

private:
#if FIELD_CUSTOM
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
#endif

  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;

#if FIELD_CUSTOM
  gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawEnvNode*>	m_DrawEnvNodeContainerForFog;
#endif

};

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
  MyRenderingPipeLine( 
    gfl2::heap::HeapBase* pHeap, 
    System::nijiAllocator*  pNijiAllocatorForEdgeMapTexture, 
    void*                   pEdgeMapResBuf,
    void*                   pOutLineResBuf,
    f32 screanWidth, 
    f32 screanHeight 
    );

	virtual ~MyRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_MainRenderPath.AddDrawEnv(pNode);
    m_SkyBoxRenderPath.AddDrawEnv(pNode);
    m_OrthoRenderPath.AddDrawEnv(pNode);
    m_MainRenderPath.AddDrawEnvForFog(pNode);
  }

#if FIELD_CUSTOM
	void AddSkyBoxRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveSkyBoxRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
#endif
	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void SetDepthTexture( gfl2::gfx::Texture* pTexture );

#if FIELD_CUSTOM

  //----------------------------------------------------------------------------
  /**
  *  @brief  OrthoRenderPathのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  OrthoRenderPath* GetOrthoRenderPath( void ){ return &m_OrthoRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  SkyBoxRenderPathのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  SkyBoxRenderPath* GetSkyBoxRenderPath( void ){ return &m_SkyBoxRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  DrawUtilTextのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  gfl2::util::UtilTextRenderPath* GetUtilTextRenderPath( void ) { return &m_UtilTextRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  Bloomのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  poke_3d::renderer::BloomRenderPath* GetBloomRenderPath( void ){ return &m_BloomRenderPath; }

#endif

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();


	u32																																		m_RenderPathCnt;
	
  System::NijiEdgeMapSceneRenderPath*																		m_pEdgeMapSceneRenderPath;
  System::NijiOutLinePostSceneRenderPath*															  m_pOutLinePostSceneRenderPath;
	MainRenderPath																												m_MainRenderPath;
	//gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;

#if FIELD_CUSTOM
  SkyBoxRenderPath                                                      m_SkyBoxRenderPath;
  OrthoRenderPath                                                       m_OrthoRenderPath;
  poke_3d::renderer::BloomRenderPath                                    m_BloomRenderPath;
  gfl2::util::UtilTextRenderPath                                        m_UtilTextRenderPath;
#endif

};

GFL_NAMESPACE_END( Field );

#endif

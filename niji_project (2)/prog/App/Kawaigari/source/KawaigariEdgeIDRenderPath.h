//======================================================================
/**
 * @file	KawaigariEdgeIDRenderPath.h
 * @brief	Kawaigari Edge ID Offscreen Buffer Render Code
 * @author	PETE
 * @data	2015.11.10
 */
//======================================================================
#if !defined( __KW_EDGEID_RENDERPATH_H__ )
#define __KW_EDGEID_RENDERPATH_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)
  
class KawaigariEdgeIDRenderPath: public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	KawaigariEdgeIDRenderPath(gfl2::gfx::IGLAllocator *allocator,int modelNum);
	virtual ~KawaigariEdgeIDRenderPath();

  void Dump(void);

  void SetupRenderTargetTexture(u32 screenWidth,u32 screenHeight);

	void AddNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  virtual const Camera* CameraOverride(){ return mpCamera; }

  //カメラをセットします
  void AddSceneCamera(gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCamera);
  void RemoveSceneCamera(void);

  void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* envNode){m_pDrawManager->AddDrawEnv(envNode);}
  void RemoveDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* envNode){m_pDrawManager->RemoveDrawEnv(envNode);}

  // --------------------------------------------------------------------------
  /**
    * @brief  ＩＤモデル描画オフスクリーンバッファコピー先設定
    */
  // --------------------------------------------------------------------------
  void SetCopyBuffer( u32 *pDest ){ mOffscreenBuf = pDest; }
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
  virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*> mNodeContainer;

  gfl2::gfx::IGLAllocator *mGlAllocator;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* mpCamera;
  
  gfl2::gfx::Texture *mRenderTexture; // Render Buffer

  u32 *mOffscreenBuf;

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif //__KW_EDGEID_RENDERPATH_H__
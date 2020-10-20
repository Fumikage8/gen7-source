//======================================================================
/**
 * @file	ModelRenderPath.h
 * @brief	モデル用描画パス
 * @author	ariizumi
 * @data	15/045/23
 */
//======================================================================
#if !defined( __MODEL_RENDERPATH_H__ )
#define __MODEL_RENDERPATH_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>


GFL_NAMESPACE_BEGIN(System)
  
class ModelRenderPath: public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	ModelRenderPath(gfl2::gfx::IGLAllocator *allocator,int modelNum);
	virtual ~ModelRenderPath();

	void AddNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  virtual const Camera* CameraOverride(){ return mpCamera; }

  //カメラをセットします
  void AddSheneCamera(gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCamera);
  void RemoveSheneCamera(void);

  void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* envNode){m_pDrawManager->AddDrawEnv(envNode);}
  void RemoveDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* envNode){m_pDrawManager->RemoveDrawEnv(envNode);}

  int GetNodeNum(void){return mNodeContainer.size();}
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*> mNodeContainer;

  gfl2::gfx::IGLAllocator *mGlAllocator;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* mpCamera;
  
};

#if 0
//実験中(使えません
class FillRenderPath: public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	FillRenderPath();
	virtual ~FillRenderPath();

private:

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
};
#endif
GFL_NAMESPACE_END(System)



#endif //__MODEL_RENDERPATH_H__
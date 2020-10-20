//======================================================================
/**
 * @file MantainSurfRenderingPipeline.h
 * @date 2016/12/06 17:30:03
 * @author fang_yicheng
 * @brief レンダリングパイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MANTAIN_SURF_RENDERING_PIPELINE__
#define __MANTAIN_SURF_RENDERING_PIPELINE__
#pragma once

#include <AppLib/include/Util/AppRenderingManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <System/include/nijiAllocator.h>
#include <System/include/RenderPath/ModelRenderPath.h>
#include <System/include/RenderPath/EdgeMapRenderPath.h>
#include <System/include/RenderPath/OutLinePostRenderPath.h>
#include <System/include/Skybox/Skybox.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

#include <GameSys/include/GameProcManager.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

class MantainSurfAppManager;

/**
 * @class CharaMantainSurfRenderingPipeline
 * @brief レンダリングパイプライン
 */
class MantainSurfRenderingPipeline : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(MantainSurfRenderingPipeline);

public:

  enum ViewerPath
  {
    VIEWER_PATH_SKY,
    VIEWER_PATH_BG,
    VIEWER_PATH_EDGE,
    VIEWER_PATH_NAMI,
    VIEWER_PATH_MAIN,
    VIEWER_PATH_OUTLINE,
    VIEWER_PATH_EFFECT,
    VIEWER_PATH_MODEL_EFFECT,
    VIEWER_PATH_COUNT
  };

public:

  MantainSurfRenderingPipeline();
  virtual ~MantainSurfRenderingPipeline();

  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pNijiAllocator);
  bool IsInitializeFinished();
  void Finalize();

  void SetAppManager(MantainSurfAppManager* pAppManager);

	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

  void SetupEffectRenderPathConfig(const gfl2::Effect::EffectRenderPath::Config& config);

  void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);

  void AddSkyModel(poke_3d::model::BaseModel* pModel);
  void AddBgModel(poke_3d::model::BaseModel* pModel);
  void AddNamiModel(poke_3d::model::BaseModel* pModel);
  void AddModelEffect(poke_3d::model::BaseModel* pModel);
  void AddBaseModel(poke_3d::model::BaseModel* pModel, bool edgeFlag = true);
  void AddDressUpModel(poke_3d::model::DressUpModel* pModel, bool edgeFlag = true);

  void RemoveObject(poke_3d::model::BaseModel* pModel);
  void RemoveObject(poke_3d::model::DressUpModel* pModel);

private:

  void AddObject(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, bool edgeFlag);
  void RemoveObject(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode);

private:

  s32                         m_seq;
  gfl2::fs::AsyncFileManager* m_pFileManager;
  gfl2::heap::HeapBase*       m_pHeap;
  System::nijiAllocator*      m_pNijiAllocator;
  MantainSurfAppManager*      m_pAppManager;

  System::ModelRenderPath*                m_pSkyRenderPath;
  System::ModelRenderPath*                m_pBgRenderPath;
  System::ModelRenderPath*                m_pMainModelRenderPath;
  System::ModelRenderPath*                m_pNamiModelRenderPath;
  System::ModelRenderPath*                m_pModelEffectRenderPath;
  System::NijiEdgeMapSceneRenderPath*     m_pEdgeMapPath;
  System::NijiOutLinePostSceneRenderPath* m_pOutLinePath;
  gfl2::Effect::EffectRenderPath*         m_pEffectPath;

  void* m_pEdgeMapData;
  void* m_pOutLineData;

  gfl2::gfx::Texture* m_pDepthStencilTexture;

  u32 m_pathCount;
};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MANTAIN_SURF_RENDERING_PIPELINE__


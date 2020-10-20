//======================================================================
/**
 * @file OteireTestRenderingPipeLine.h
 * @date 2015/10/30 17:06:17
 * @author araki_syo
 * @brief お手入れテスト用レンダリングパイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined OTEIRE_TEST_RENDERING_PIPELINE
#define OTEIRE_TEST_RENDERING_PIPELINE
#pragma once

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"
#include <system/include/nijiAllocator.h>

#include <GameSys/include/GameProcManager.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(OteireTest)

class GridModel;
class FactoryParam;

/**
 * @class CharaViewerRenderingPipeline
 * @brief レンダリングパイプライン
 */
class OteireTestRenderingPipeline : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(OteireTestRenderingPipeline);

public:

  OteireTestRenderingPipeline();
  virtual ~OteireTestRenderingPipeline();

  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pNijiAllocator);
  void Finalize();

  virtual b32 StartRenderPath();
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
  virtual b32 NextRenderPath();

  void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);
  void RemoveDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);

  void AddNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, bool edgeFlag = true);
  void RemoveNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode);

private:
  System::ModelRenderPath* m_pModelRenderPath;

  System::NijiEdgeMapSceneRenderPath* m_pEdgeMapPath;
  System::NijiOutLinePostSceneRenderPath* m_pOutLinePath;

  void* m_pEdgeMapData;
  void* m_pOutLineData;

  gfl2::gfx::Texture* m_pDepthStencilTexture;

  u32 m_PathCount;
};

GFL_NAMESPACE_END(OteireTest)
GFL_NAMESPACE_END(Test)

#endif // OTEIRE_TEST_RENDERING_PIPELINE

#endif // PM_DEBUG

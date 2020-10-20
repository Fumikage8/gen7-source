//======================================================================
/**
 * @file RenderingPipeline.h
 * @date 2016/09/07 11:36:13
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RENDERING_PIPELINE_H_INCLUDED__
#define __LUNA_SOL_DIVE__RENDERING_PIPELINE_H_INCLUDED__
#pragma once

#include <Effect/include/gfl2_EffectRenderPath.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <system/include/nijiAllocator.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"

namespace LunaSolDive{
class RenderingPipeline : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderingPipeline);
public:
  // constructor
  RenderingPipeline();
  // destructor
  virtual ~RenderingPipeline();
  // Initialize
  void Initialize(
              gfl2::fs::AsyncFileManager* pFileManager,
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pNijiAllocator);
  // Finalize
  void Finalize();
  // Set Up Effect Path
  void SetupEffectRenderPathConfig(
              const gfl2::Effect::EffectRenderPath::Config& config);
  // Reset Path Count
  virtual b32 StartRenderPath();
  // Get Present Render Path
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
  // Increment Path Count
  virtual b32 NextRenderPath();
  // Add DrawEnv to SkyBox & Model Path
  void AddDrawEnv(
              gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);
  // Register to SkyBox Path
  void AddSkyBox(
              gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode);
  // Register to Model Path & Edge Path(if edgeFlag is true)
  void AddObject(
              gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode,
              bool edgeFlag);
  // Unregister from Path
  void RemoveObject(
              gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode);
private:
  // SkyBox Path
  System::ModelRenderPath* m_pSkyBoxRenderPath;
  // Model Path
  System::ModelRenderPath* m_pModelRenderPath;
  // Edge Map Path
  System::NijiEdgeMapSceneRenderPath* m_pEdgeMapPath;
  // Outline Path
  System::NijiOutLinePostSceneRenderPath* m_pOutLinePath;
  // Effect Path
  gfl2::Effect::EffectRenderPath* m_pEffectPath;
  // Edge Map Data
  void* m_pEdgeMapData;
  // Outline Data
  void* m_pOutLineData;
  // Depth Stencil Texture
  gfl2::gfx::Texture* m_pDepthStencilTexture;
  // Path Count
  u32 m_PathCount;
};
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RENDERING_PIPELINE_H_INCLUDED__

//======================================================================
/**
 * @file PokeViewerRenderingPipeline.h
 * @date 2015/12/03 11:20:58
 * @author araki_syo
 * @brief ポケビューアのレンダリングパイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined POKEVIEWER_RENDERINGPIPELINE_H_INCLUDED
#define POKEVIEWER_RENDERINGPIPELINE_H_INCLUDED
#pragma once

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ClearRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>
#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"
#include "System/include/nijiAllocator.h"

#include <GameSys/include/GameProcManager.h>

#include "Poketool/include/PokeModelSystem.h"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

/**
 * @class PokeViewerRenderingPipeline
 * @brief レンダリングパイプライン
 */
class PokeViewerRenderingPipeline : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeViewerRenderingPipeline);

public:

  PokeViewerRenderingPipeline();
  virtual ~PokeViewerRenderingPipeline();

  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pNijiAllocator);
  void Finalize();

  virtual b32 StartRenderPath();
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
  virtual b32 NextRenderPath();

  void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);
  void RemoveDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);

  void AddNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, bool edgeFlag = true);
  void RemoveNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode);

  void AddDistortionNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);
  void RemoveDistortionNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);

  void SetClearColor(const gfl2::gfx::Color& color);
  void SetEdgeColor(const gfl2::gfx::Color& color);
  /**
   * @fn SetPokeModelSys
   * @brief カラーシェーダ用ポケモンモデルシステムのセットアップ NULLを設定するとカラーシェーダが無効になる
   */
  void SetPokeModelSys(PokeTool::PokeModelSystem *pPokeModelSys);

private:
  gfl2::renderingengine::renderer::util::ClearRenderPath* m_pClearRenderPath;

  System::ModelRenderPath* m_pModelRenderPath;
  System::NijiEdgeMapSceneRenderPath* m_pEdgeMapPath;
  System::NijiOutLinePostSceneRenderPath* m_pOutLinePath;
  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath* m_pStretchBltFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath* m_pDistortionPostRenderPath;

  void* m_pEdgeMapData;
  void* m_pOutLineData;

  gfl2::gfx::Texture* m_pDepthStencilTexture;

  u32 m_PathCount;

  PokeTool::PokeModelSystem* m_pPokeModelSys;
};

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // POKEVIEWER_RENDERINGPIPELINE_H_INCLUDED

#endif // PM_DEBUG
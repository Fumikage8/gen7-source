//======================================================================
/**
 * @file PokeViewerRenderingPipeline.cpp
 * @date 2015/12/03 11:25:16
 * @author araki_syo
 * @brief ポケビューアのレンダリングパイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/PokeViewerRenderingPipeline.h"
#include "System/include/GflUse.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

PokeViewerRenderingPipeline::PokeViewerRenderingPipeline() :
  m_pClearRenderPath(NULL),
  m_pModelRenderPath(NULL),
  m_pEdgeMapPath(NULL),
  m_pOutLinePath(NULL),
  m_pStretchBltFrameBufferPath(NULL),
  m_pDistortionPostRenderPath(NULL),
  m_pEdgeMapData(NULL),
  m_pOutLineData(NULL),
  m_pDepthStencilTexture(NULL),
  m_PathCount(0),
  m_pPokeModelSys(NULL)
{
}

PokeViewerRenderingPipeline::~PokeViewerRenderingPipeline()
{
}

void PokeViewerRenderingPipeline::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pNijiAllocator)
{
  u32 arcId = ARCID_RENDERPATH;
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId = arcId;
    req.heapForFile = pHeap;
    req.heapForReq  = pHeap;

    pFileManager->SyncArcFileOpen(req);
  }

  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = arcId;
    req.datId      = GARC_NijiRenderPath_EdgeMapResource_BIN;
    req.ppBuf      = &m_pEdgeMapData;
    req.heapForBuf = pHeap;
    req.align      = 128; // テクスチャは128バイトアラインメント
    req.heapForReq = pHeap;
    req.heapForCompressed = NULL;
    pFileManager->SyncArcFileLoadData(req);

    req.datId     = GARC_NijiRenderPath_OutLineResource_BIN;
    req.ppBuf     = &m_pOutLineData;
    pFileManager->SyncArcFileLoadData(req);
  }

  {
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize	  = 256;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

    m_pClearRenderPath = GFL_NEW(pHeap) gfl2::renderingengine::renderer::util::ClearRenderPath();

    m_pModelRenderPath = GFL_NEW(pHeap) System::ModelRenderPath(pNijiAllocator, 128);
    m_pModelRenderPath->Initialize(pNijiAllocator, desc);

    m_pEdgeMapPath = GFL_NEW(pHeap) System::NijiEdgeMapSceneRenderPath(pNijiAllocator, 128);
    m_pEdgeMapPath->SetResource(m_pEdgeMapData);
    m_pEdgeMapPath->CreateEdgeMapTexture(pNijiAllocator, 512, 256);
    m_pEdgeMapPath->Initialize(pNijiAllocator, desc);

    m_pOutLinePath = GFL_NEW(pHeap) System::NijiOutLinePostSceneRenderPath(pNijiAllocator);
    m_pOutLinePath->SetResource(m_pOutLineData);
  }

  {
    m_pStretchBltFrameBufferPath = GFL_NEW(pHeap) gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath();
    m_pDistortionPostRenderPath = GFL_NEW(pHeap) poke_3d::renderer::DistortionPostRenderPath();

    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    m_pStretchBltFrameBufferPath->CreateFrameBufferTexture(pNijiAllocator,pBackBuffer);

    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize = 4;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;
    m_pDistortionPostRenderPath->Initialize(pNijiAllocator, desc);
    m_pDistortionPostRenderPath->CreateBuffer(pNijiAllocator, 4);
    m_pDistortionPostRenderPath->SetOverrideTexture( m_pStretchBltFrameBufferPath->GetFrameBufferTexture() );
  }

  for (u32 i = 0; i < poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max; ++i)
  {
    const gfl2::gfx::Texture* pTex = m_pEdgeMapPath->GetEdgeMapTexture(i);
    m_pOutLinePath->SetEdgeMapTexture(i, pTex);
  }

#if defined(GF_PLATFORM_CTR)
  m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(pNijiAllocator, gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
  m_pOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, m_pDepthStencilTexture);
#endif

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId = arcId;
    req.heapForReq = pHeap;

    pFileManager->SyncArcFileClose(req);
  }
}

void PokeViewerRenderingPipeline::Finalize()
{
  GFL_SAFE_DELETE(m_pDepthStencilTexture);
  GFL_SAFE_DELETE(m_pClearRenderPath);
  GFL_SAFE_DELETE(m_pModelRenderPath);
  GFL_SAFE_DELETE(m_pStretchBltFrameBufferPath);
  GFL_SAFE_DELETE(m_pDistortionPostRenderPath);
  GFL_SAFE_DELETE(m_pEdgeMapPath);
  GFL_SAFE_DELETE(m_pOutLinePath);
  GflHeapSafeFreeMemory(m_pEdgeMapData);
  GflHeapSafeFreeMemory(m_pOutLineData);
}

enum PokeViewerPath
{
  POKEVIEWER_PATH_CLEAR,
  POKEVIEWER_PATH_EDGE,
  POKEVIEWER_PATH_MAIN,
  POKEVIEWER_PATH_OUTLINE,
  POKEVIEWER_PATH_STRETCH,
  POKEVIEWER_PATH_DISTORTION,

  POKEVIEWER_PATH_COUNT
};

b32 PokeViewerRenderingPipeline::StartRenderPath(void)
{
  m_PathCount = 0;
  return true;
}

gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* PokeViewerRenderingPipeline::GetRenderPath(void)
{
  //ポケモン用カラーシェーダー描画
  const int pokePathNum = m_pPokeModelSys->GetPokeShaderPathNum();
  if( m_PathCount < pokePathNum )
  {
    return m_pPokeModelSys->GetPokeShaderPath(m_PathCount);
  }

  switch (m_PathCount - pokePathNum)
  {
  case POKEVIEWER_PATH_CLEAR:
    return m_pClearRenderPath;
  case POKEVIEWER_PATH_EDGE:
    return m_pEdgeMapPath;
  case POKEVIEWER_PATH_MAIN:
    return m_pModelRenderPath;
  case POKEVIEWER_PATH_OUTLINE:
    return m_pOutLinePath;
  case POKEVIEWER_PATH_STRETCH:
    return m_pStretchBltFrameBufferPath;
  case POKEVIEWER_PATH_DISTORTION:
    return m_pDistortionPostRenderPath;
  }
  return NULL;
}

b32 PokeViewerRenderingPipeline::NextRenderPath(void)
{
  ++m_PathCount;
  return (m_PathCount < POKEVIEWER_PATH_COUNT + m_pPokeModelSys->GetPokeShaderPathNum());
}

void PokeViewerRenderingPipeline::AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pModelRenderPath->AddDrawEnv(pDrawEnvNode);
}

void PokeViewerRenderingPipeline::RemoveDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pModelRenderPath->RemoveDrawEnv(pDrawEnvNode);
}

void PokeViewerRenderingPipeline::AddNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, bool edgeFlag)
{
  if (pDrawableNode == NULL)
  {
    return;
  }

  m_pModelRenderPath->AddNode(pDrawableNode);
  if (edgeFlag)
  {
    m_pEdgeMapPath->AddEdgeRenderingTarget(pDrawableNode);
  }
}

void PokeViewerRenderingPipeline::RemoveNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode)
{
  if (pDrawableNode == NULL)
  {
    return;
  }

  m_pModelRenderPath->RemoveNode(pDrawableNode);
  m_pEdgeMapPath->RemoveEdgeRenderingTarget(pDrawableNode);
}

void PokeViewerRenderingPipeline::AddDistortionNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  if(pDrawableNode == NULL || pDrawEnvNode == NULL)
  {
    return;
  }

  m_pDistortionPostRenderPath->AddDistortionRenderingTarget(pDrawableNode);
  m_pDistortionPostRenderPath->AddDrawEnv(pDrawEnvNode);
}

void PokeViewerRenderingPipeline::RemoveDistortionNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  if(pDrawableNode == NULL || pDrawEnvNode == NULL)
  {
    return;
  }

  m_pDistortionPostRenderPath->RemoveDistortionRenderingTarget(pDrawableNode);
  m_pDistortionPostRenderPath->RemoveDrawEnv(pDrawEnvNode);
}

void PokeViewerRenderingPipeline::SetClearColor(const gfl2::gfx::Color& color)
{
  m_pClearRenderPath->SetClearColor(color);
}

void PokeViewerRenderingPipeline::SetEdgeColor(const gfl2::gfx::Color& color)
{
  m_pOutLinePath->SetEdgeColor( 255 * color.GetR(), 255 * color.GetG(), 255 * color.GetB() );
}

void PokeViewerRenderingPipeline::SetPokeModelSys(PokeTool::PokeModelSystem* pPokeModelSys)
{
  m_pPokeModelSys = pPokeModelSys;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
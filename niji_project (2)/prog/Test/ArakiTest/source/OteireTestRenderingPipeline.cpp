//======================================================================
/**
 * @file OteireTestRenderingPipeLine.cpp
 * @date 2015/10/30 17:09:25
 * @author araki_syo
 * @brief お手入れテスト用レンダリングパイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/OteireTestRenderingPipeline.h"
#include "System/include/GflUse.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(OteireTest)

OteireTestRenderingPipeline::OteireTestRenderingPipeline() :
  m_pModelRenderPath(NULL),
  m_pEdgeMapPath(NULL),
  m_pOutLinePath(NULL),
  m_pEdgeMapData(NULL),
  m_pOutLineData(NULL),
  m_pDepthStencilTexture(NULL),
  m_PathCount(0)
{
}

OteireTestRenderingPipeline::~OteireTestRenderingPipeline()
{
}

void OteireTestRenderingPipeline::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pNijiAllocator)
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

    m_pModelRenderPath = GFL_NEW(pHeap) System::ModelRenderPath(pNijiAllocator, 128);
    m_pModelRenderPath->Initialize(pNijiAllocator, desc);

    m_pEdgeMapPath = GFL_NEW(pHeap) System::NijiEdgeMapSceneRenderPath(pNijiAllocator, 128);
    m_pEdgeMapPath->SetResource(m_pEdgeMapData);
    m_pEdgeMapPath->CreateEdgeMapTexture(pNijiAllocator, 512, 256);
    m_pEdgeMapPath->Initialize(pNijiAllocator, desc);

    m_pOutLinePath = GFL_NEW(pHeap) System::NijiOutLinePostSceneRenderPath(pNijiAllocator);
    m_pOutLinePath->SetResource(m_pOutLineData);
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

void OteireTestRenderingPipeline::Finalize()
{
  GFL_SAFE_DELETE(m_pDepthStencilTexture);
  GFL_SAFE_DELETE(m_pModelRenderPath);
  GFL_SAFE_DELETE(m_pEdgeMapPath);
  GFL_SAFE_DELETE(m_pOutLinePath);
  GflHeapSafeFreeMemory(m_pEdgeMapData);
  GflHeapSafeFreeMemory(m_pOutLineData);
}

enum OteireTestPath
{
  OTEIRE_TEST_PATH_EDGE,
  OTEIRE_TEST_PATH_MAIN,
  OTEIRE_TEST_PATH_OUTLINE,

  OTEIRE_TEST_PATH_COUNT
};

b32 OteireTestRenderingPipeline::StartRenderPath(void)
{
  m_PathCount = 0;
  return true;
}

gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* OteireTestRenderingPipeline::GetRenderPath(void)
{
  switch (m_PathCount)
  {
  case OTEIRE_TEST_PATH_EDGE:
    return m_pEdgeMapPath;
  case OTEIRE_TEST_PATH_MAIN:
    return m_pModelRenderPath;
  case OTEIRE_TEST_PATH_OUTLINE:
    return m_pOutLinePath;
  }
  return NULL;
}

b32 OteireTestRenderingPipeline::NextRenderPath(void)
{
  ++m_PathCount;
  return (m_PathCount < OTEIRE_TEST_PATH_COUNT);
}

void OteireTestRenderingPipeline::AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pModelRenderPath->AddDrawEnv(pDrawEnvNode);
}

void OteireTestRenderingPipeline::RemoveDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pModelRenderPath->RemoveDrawEnv(pDrawEnvNode);
}

void OteireTestRenderingPipeline::AddNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, bool edgeFlag)
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

void OteireTestRenderingPipeline::RemoveNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode)
{
  if (pDrawableNode == NULL)
  {
    return;
  }

  m_pModelRenderPath->RemoveNode(pDrawableNode);
  m_pEdgeMapPath->RemoveEdgeRenderingTarget(pDrawableNode);
}

GFL_NAMESPACE_END(OteireTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG

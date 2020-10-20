//======================================================================
/**
 * @file RenderingPipeline.cpp
 * @date 2016/09/07 11:54:03
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "System/include/GflUse.h"
#include "./Assert.h"
#include "./RenderingPipeline.h"
#include "arc_def_index/arc_def.h"
#include "arc_index/NijiRenderPath.gaix"

namespace LunaSolDive{
// ViewerPath
enum
{
  VIEWER_PATH__SKYBOX,
  VIEWER_PATH__EDGE,
  VIEWER_PATH__MAIN,
  VIEWER_PATH__OUTLINE,
  VIEWER_PATH__EFFECT,
  VIEWER_PATH__COUNT
};
// constructor
RenderingPipeline::RenderingPipeline()
    : m_pSkyBoxRenderPath(NULL),
      m_pModelRenderPath(NULL),
      m_pEdgeMapPath(NULL),
      m_pOutLinePath(NULL),
      m_pEffectPath(NULL),
      m_pEdgeMapData(NULL),
      m_pOutLineData(NULL),
      m_pDepthStencilTexture(NULL),
      m_PathCount(0){}
// destructor
RenderingPipeline::~RenderingPipeline(){}
// Initialize
void RenderingPipeline::Initialize(
        gfl2::fs::AsyncFileManager* pFileManager,
        gfl2::heap::HeapBase* pHeap,
        System::nijiAllocator* pNijiAllocator)
{
  // open Archive
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openRequest;
  openRequest.arcId = ARCID_RENDERPATH;
  openRequest.heapForFile = pHeap;
  openRequest.heapForReq = pHeap->GetLowerHandle();
  pFileManager->SyncArcFileOpen(openRequest);
  // load Date
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadRequest;
  loadRequest.arcId = ARCID_RENDERPATH;
  loadRequest.heapForBuf = pHeap;
  loadRequest.align = 128;
  loadRequest.heapForReq = pHeap->GetLowerHandle();
  loadRequest.heapForCompressed = NULL;
  // load Edge Map Resource
  loadRequest.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
  loadRequest.ppBuf = &m_pEdgeMapData;
  pFileManager->SyncArcFileLoadData(loadRequest);
  // load Outline Resource
  loadRequest.datId = GARC_NijiRenderPath_OutLineResource_BIN;
  loadRequest.ppBuf = &m_pOutLineData;
  pFileManager->SyncArcFileLoadData(loadRequest);
  // create Path
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription description;
  description.m_DrawManagerDesc.m_NodeStackSize = 256;
  description.m_DrawManagerDesc.m_DrawTagStackSize = description.m_DrawManagerDesc.m_NodeStackSize * 16;
  // SkyBox Path
  m_pSkyBoxRenderPath = GFL_NEW(pHeap) System::ModelRenderPath(pNijiAllocator, 1);
  m_pSkyBoxRenderPath->Initialize(pNijiAllocator, description);
  // Model Path
  m_pModelRenderPath = GFL_NEW(pHeap) System::ModelRenderPath(pNijiAllocator, 128);
  m_pModelRenderPath->Initialize(pNijiAllocator, description);
  // Edge Map Path
  m_pEdgeMapPath = GFL_NEW(pHeap) System::NijiEdgeMapSceneRenderPath(pNijiAllocator, 128);
  m_pEdgeMapPath->SetResource(m_pEdgeMapData);
  m_pEdgeMapPath->CreateEdgeMapTexture(pNijiAllocator, 512, 256);
  m_pEdgeMapPath->Initialize(pNijiAllocator, description);
  // Out Line Path
  m_pOutLinePath = GFL_NEW(pHeap) System::NijiOutLinePostSceneRenderPath(pNijiAllocator);
  m_pOutLinePath->SetResource(m_pOutLineData);
  // Effect Path
  m_pEffectPath = GFL_NEW(pHeap) gfl2::Effect::EffectRenderPath();
  // set Edge Map Texture
  for(u32 i = 0; i < poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max; ++i)
  {
    const gfl2::gfx::Texture* pTex = m_pEdgeMapPath->GetEdgeMapTexture(i);
    m_pOutLinePath->SetEdgeMapTexture(i, pTex);
  }
#if defined(GF_PLATFORM_CTR)
  m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(
              pNijiAllocator,
              gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
  m_pOutLinePath->SetEdgeMapTexture(
              poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID,
              m_pDepthStencilTexture);
#endif
  // close Archive
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeRequest;
  closeRequest.arcId = ARCID_RENDERPATH;
  closeRequest.heapForReq = pHeap->GetLowerHandle();
  pFileManager->SyncArcFileClose(closeRequest);
}
// Finalize
void RenderingPipeline::Finalize()
{
  LUNA_SOL_DIVE_ASSERT(m_pSkyBoxRenderPath->GetNodeNum() == 0);
  LUNA_SOL_DIVE_ASSERT(m_pModelRenderPath->GetNodeNum() == 0);
  GFL_SAFE_DELETE(m_pDepthStencilTexture);
  GFL_SAFE_DELETE(m_pSkyBoxRenderPath);
  GFL_SAFE_DELETE(m_pModelRenderPath);
  GFL_SAFE_DELETE(m_pEdgeMapPath);
  GFL_SAFE_DELETE(m_pOutLinePath);
  GFL_SAFE_DELETE(m_pEffectPath);
  GflHeapSafeFreeMemory(m_pEdgeMapData);
  GflHeapSafeFreeMemory(m_pOutLineData);
}
// Set Up Effect Path
void RenderingPipeline::SetupEffectRenderPathConfig(
            const gfl2::Effect::EffectRenderPath::Config& config)
{
  m_pEffectPath->SetConfig(config);
}
// Reset Path Count
b32 RenderingPipeline::StartRenderPath()
{
  m_PathCount = 0;
  return true;
}
// Get Present Render Path
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* RenderingPipeline::GetRenderPath()
{
  switch(m_PathCount)
  {
  case VIEWER_PATH__SKYBOX:
    return m_pSkyBoxRenderPath;
  case VIEWER_PATH__EDGE:
    return m_pEdgeMapPath;
  case VIEWER_PATH__MAIN:
    return m_pModelRenderPath;
  case VIEWER_PATH__OUTLINE:
    return m_pOutLinePath;
  case VIEWER_PATH__EFFECT:
    return m_pEffectPath;
  }
  return NULL;
}
// Increment Path Count
b32 RenderingPipeline::NextRenderPath()
{
  ++m_PathCount;
  return (m_PathCount < VIEWER_PATH__COUNT);
}
// Add DrawEnv to SkyBox & Model Path
void RenderingPipeline::AddDrawEnv(
            gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pSkyBoxRenderPath->AddDrawEnv(pDrawEnvNode);
  m_pModelRenderPath->AddDrawEnv(pDrawEnvNode);
}
// Register to SkyBox Path
void RenderingPipeline::AddSkyBox(
            gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode)
{
  LUNA_SOL_DIVE_ASSERT(m_pSkyBoxRenderPath != NULL);
  if (pDrawableNode == NULL) return;
  m_pSkyBoxRenderPath->AddNode(pDrawableNode);
}
// Register to Model Path & Edge Path(if edgeFlag is true)
void RenderingPipeline::AddObject(
            gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode,
            bool edgeFlag)
{
  if(pDrawableNode == NULL) return;
  m_pModelRenderPath->AddNode(pDrawableNode);
  if(edgeFlag)
  {
    m_pEdgeMapPath->AddEdgeRenderingTarget(pDrawableNode);
  }
}
// Unregister from Path
void RenderingPipeline::RemoveObject(
            gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode)
{
  if(pDrawableNode == NULL) return;
  m_pSkyBoxRenderPath->RemoveNode(pDrawableNode);
  m_pModelRenderPath->RemoveNode(pDrawableNode);
  m_pEdgeMapPath->RemoveEdgeRenderingTarget(pDrawableNode);
}
}// namespace LunaSolDive

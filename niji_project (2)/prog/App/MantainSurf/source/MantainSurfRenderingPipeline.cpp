//======================================================================
/**
 * @file MantainSurfRenderingPipeline.cpp
 * @date 2016/12/06 17:30:03
 * @author fang_yicheng
 * @brief レンダリングパイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "MantainSurfRenderingPipeline.h"
#include "MantainSurfAppManager.h"
#include "System/include/GflUse.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>
#include <System/include/Skybox/Skybox.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

MantainSurfRenderingPipeline::MantainSurfRenderingPipeline() :
  m_seq(0),
  m_pFileManager(NULL),
  m_pHeap(NULL),
  m_pNijiAllocator(NULL),
  m_pAppManager(NULL),
  m_pSkyRenderPath(NULL),
  m_pBgRenderPath(NULL),
  m_pMainModelRenderPath(NULL),
  m_pNamiModelRenderPath(NULL),
  m_pModelEffectRenderPath(NULL),
  m_pEdgeMapPath(NULL),
  m_pOutLinePath(NULL),
  m_pEffectPath(NULL),
  m_pEdgeMapData(NULL),
  m_pOutLineData(NULL),
  m_pDepthStencilTexture(NULL),
  m_pathCount(0)
{
}

MantainSurfRenderingPipeline::~MantainSurfRenderingPipeline()
{
}

void MantainSurfRenderingPipeline::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pNijiAllocator)
{
  m_seq = 0;
  m_pFileManager = pFileManager;
  m_pHeap = pHeap;
  m_pNijiAllocator = pNijiAllocator;
}

void MantainSurfRenderingPipeline::SetAppManager(MantainSurfAppManager* pAppManager)
{
  m_pAppManager = pAppManager;
}

bool MantainSurfRenderingPipeline::IsInitializeFinished()
{
  static const u32 ARC_ID = ARCID_RENDERPATH;
  switch(m_seq)
  {
  case 0:
    {
      // ARCファイルオープン
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId = ARC_ID;
      req.heapForFile = m_pHeap->GetLowerHandle();
      req.heapForReq = m_pHeap->GetLowerHandle();
      m_pFileManager->AddArcFileOpenReq(req);
      m_seq++;
    }
    /* Fall Through */
  case 1:
    {
      if(!m_pFileManager->IsArcFileOpenFinished(ARC_ID))
      {
        return false;
      }
      // データをロード
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId = ARC_ID;
      req.heapForBuf = m_pHeap;
      req.align = 128;
      req.heapForReq = m_pHeap->GetLowerHandle();
      req.heapForCompressed = NULL;
      // エッジマップ
      req.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
      req.ppBuf = &m_pEdgeMapData;
      m_pFileManager->AddArcFileLoadDataReq(req);
      // アウトライン
      req.datId = GARC_NijiRenderPath_OutLineResource_BIN;
      req.ppBuf = &m_pOutLineData;
      m_pFileManager->AddArcFileLoadDataReq(req);
      m_seq++;
    }
    /* Fall Through */
  case 2:
    {
      if(!m_pFileManager->IsArcFileLoadDataFinished(&m_pEdgeMapData))
      {
        return false;
      }
      if(!m_pFileManager->IsArcFileLoadDataFinished(&m_pOutLineData))
      {
        return false;
      }

      // データ初期化
      
      //desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

      // 天球用レンダリングパス
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
        desc.m_DrawManagerDesc.m_NodeStackSize = System::Skybox::Skybox::PARTS_TYPE_MAX;
        desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;

        m_pSkyRenderPath = GFL_NEW(m_pHeap) System::ModelRenderPath(m_pNijiAllocator, desc.m_DrawManagerDesc.m_NodeStackSize);
        m_pSkyRenderPath->Initialize(m_pNijiAllocator, desc);
      }

      // 背景用レンダリングパス
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
        desc.m_DrawManagerDesc.m_NodeStackSize = 4;
        desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;

        m_pBgRenderPath = GFL_NEW(m_pHeap) System::ModelRenderPath(m_pNijiAllocator, desc.m_DrawManagerDesc.m_NodeStackSize);
        m_pBgRenderPath->Initialize(m_pNijiAllocator, desc);
      }

      // モデルとエッジマップ用レンダリングパス
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
        desc.m_DrawManagerDesc.m_NodeStackSize = 256;
        desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;

        m_pMainModelRenderPath = GFL_NEW(m_pHeap) System::ModelRenderPath(m_pNijiAllocator, desc.m_DrawManagerDesc.m_NodeStackSize);
        m_pMainModelRenderPath->Initialize(m_pNijiAllocator, desc);
        //m_pMainModelRenderPath->GetDrawManager()->ViewSpaceRenderEnable(true);

        desc.m_DrawManagerDesc.m_NodeStackSize = 16;
        m_pNamiModelRenderPath = GFL_NEW(m_pHeap) System::ModelRenderPath(m_pNijiAllocator, desc.m_DrawManagerDesc.m_NodeStackSize);
        m_pNamiModelRenderPath->Initialize(m_pNijiAllocator, desc);

        desc.m_DrawManagerDesc.m_NodeStackSize = 256 + 16;
        m_pEdgeMapPath = GFL_NEW(m_pHeap) System::NijiEdgeMapSceneRenderPath(m_pNijiAllocator, desc.m_DrawManagerDesc.m_NodeStackSize);
        m_pEdgeMapPath->SetResource(m_pEdgeMapData);
        m_pEdgeMapPath->CreateEdgeMapTexture(m_pNijiAllocator, 512, 256);
        m_pEdgeMapPath->Initialize(m_pNijiAllocator, desc);
        //m_pEdgeMapPath->GetDrawManager()->ViewSpaceRenderEnable(true);
      }

      // アウトレット用レンダリングパス
      {
        m_pOutLinePath = GFL_NEW(m_pHeap) System::NijiOutLinePostSceneRenderPath(m_pNijiAllocator);
        m_pOutLinePath->SetResource(m_pOutLineData);
      }

      // エフェクト用レンダリングパス
      {
        m_pEffectPath = GFL_NEW(m_pHeap) gfl2::Effect::EffectRenderPath();

        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
        desc.m_DrawManagerDesc.m_NodeStackSize = 16;
        desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;

        m_pModelEffectRenderPath = GFL_NEW(m_pHeap) System::ModelRenderPath(m_pNijiAllocator, desc.m_DrawManagerDesc.m_NodeStackSize);
        m_pModelEffectRenderPath->Initialize(m_pNijiAllocator, desc);
      }

      for(u32 i = 0; i < poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max; ++i)
      {
        const gfl2::gfx::Texture* pTex = m_pEdgeMapPath->GetEdgeMapTexture(i);
        m_pOutLinePath->SetEdgeMapTexture(i, pTex);
      }

#if defined(GF_PLATFORM_CTR)
      m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(m_pNijiAllocator, gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
      m_pOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, m_pDepthStencilTexture);
#endif

      // ARCファイルをクローズ
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId = ARC_ID;
      req.heapForReq = m_pHeap->GetLowerHandle();
      m_pFileManager->AddArcFileCloseReq(req);
      m_seq++;
    }
    /* Fall Through */
  case 3:
    {
      if(!m_pFileManager->IsArcFileCloseFinished(ARC_ID))
      {
        return false;
      }
      m_seq++;
    }
    /* Fall Through */
  case 4:
    return true;
  default:
    return false;
  }
}

void MantainSurfRenderingPipeline::Finalize()
{
  GFL_SAFE_DELETE(m_pDepthStencilTexture);
  GFL_SAFE_DELETE(m_pModelEffectRenderPath);
  GFL_SAFE_DELETE(m_pNamiModelRenderPath);
  GFL_SAFE_DELETE(m_pMainModelRenderPath);
  GFL_SAFE_DELETE(m_pBgRenderPath);
  GFL_SAFE_DELETE(m_pSkyRenderPath);
  GFL_SAFE_DELETE(m_pEdgeMapPath);
  GFL_SAFE_DELETE(m_pOutLinePath);
  GFL_SAFE_DELETE(m_pEffectPath);
  GflHeapSafeFreeMemory(m_pEdgeMapData);
  GflHeapSafeFreeMemory(m_pOutLineData);
}

b32 MantainSurfRenderingPipeline::StartRenderPath(void)
{
  m_pathCount = 0;
  m_pEdgeMapPath->SetEnable(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, 1);
  m_pOutLinePath->SetEnable(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, 1);
  return true;
}

gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* MantainSurfRenderingPipeline::GetRenderPath(void)
{
#if PM_DEBUG
  if(m_pAppManager && !m_pAppManager->GetDebugMenu()->m_dbvDrawEnable[m_pathCount])
  {
    // 描画OFF
    return NULL;
  }
#endif

  switch (m_pathCount)
  {
  case VIEWER_PATH_SKY:
    return m_pSkyRenderPath;
  case VIEWER_PATH_BG:
    return m_pBgRenderPath;
  case VIEWER_PATH_EDGE:
    return m_pEdgeMapPath;
  case VIEWER_PATH_MAIN:
    return m_pMainModelRenderPath;
  case VIEWER_PATH_NAMI:
    return m_pNamiModelRenderPath;
  case VIEWER_PATH_OUTLINE:
    return m_pOutLinePath;
  case VIEWER_PATH_EFFECT:
    return m_pEffectPath;
  case VIEWER_PATH_MODEL_EFFECT:
    return m_pModelEffectRenderPath;
  }
  return NULL;
}

b32 MantainSurfRenderingPipeline::NextRenderPath(void)
{
  ++m_pathCount;
  return (m_pathCount < VIEWER_PATH_COUNT);
}

void MantainSurfRenderingPipeline::SetupEffectRenderPathConfig(const gfl2::Effect::EffectRenderPath::Config& config)
{
  m_pEffectPath->SetConfig(config);
}

void MantainSurfRenderingPipeline::AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pSkyRenderPath->AddDrawEnv(pDrawEnvNode);
  m_pBgRenderPath->AddDrawEnv(pDrawEnvNode);
  m_pMainModelRenderPath->AddDrawEnv(pDrawEnvNode);
  m_pNamiModelRenderPath->AddDrawEnv(pDrawEnvNode);
  m_pModelEffectRenderPath->AddDrawEnv(pDrawEnvNode);
}

void MantainSurfRenderingPipeline::AddSkyModel(poke_3d::model::BaseModel* pModel)
{
  m_pSkyRenderPath->AddNode(pModel->GetModelInstanceNode());
}

void MantainSurfRenderingPipeline::AddBgModel(poke_3d::model::BaseModel* pModel)
{
  m_pBgRenderPath->AddNode(pModel->GetModelInstanceNode());
}

void MantainSurfRenderingPipeline::AddNamiModel(poke_3d::model::BaseModel* pModel)
{
  m_pNamiModelRenderPath->AddNode(pModel->GetModelInstanceNode());
  m_pEdgeMapPath->AddEdgeRenderingTarget(pModel->GetModelInstanceNode());
}

void MantainSurfRenderingPipeline::AddModelEffect(poke_3d::model::BaseModel* pModel)
{
  m_pModelEffectRenderPath->AddNode(pModel->GetModelInstanceNode());
}

void MantainSurfRenderingPipeline::AddBaseModel(poke_3d::model::BaseModel* pModel, bool edgeFlag)
{
  AddObject(pModel->GetModelInstanceNode(), edgeFlag);
}

void MantainSurfRenderingPipeline::AddDressUpModel(poke_3d::model::DressUpModel* pModel, bool edgeFlag)
{
  for(u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; i++)
  {
    AddObject(pModel->GetPartsModelInstanceNode(static_cast<poke_3d::model::DressUpModel::PartsModel>(i)), edgeFlag);
  }
}

void MantainSurfRenderingPipeline::AddObject(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode, bool edgeFlag)
{
  if(pDrawableNode == NULL)
  {
    return;
  }

  m_pMainModelRenderPath->AddNode(pDrawableNode);
  if(edgeFlag)
  {
    m_pEdgeMapPath->AddEdgeRenderingTarget(pDrawableNode);
  }
}

void MantainSurfRenderingPipeline::RemoveObject(poke_3d::model::BaseModel* pModel)
{
  RemoveObject(pModel->GetModelInstanceNode());
}

void MantainSurfRenderingPipeline::RemoveObject(poke_3d::model::DressUpModel* pModel)
{
  for(u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    RemoveObject(pModel->GetPartsModelInstanceNode(static_cast<poke_3d::model::DressUpModel::PartsModel>(i)));
  }
}

void MantainSurfRenderingPipeline::RemoveObject(gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode)
{
  if (pDrawableNode == NULL)
  {
    return;
  }

  m_pSkyRenderPath->RemoveNode(pDrawableNode);
  m_pBgRenderPath->RemoveNode(pDrawableNode);
  m_pMainModelRenderPath->RemoveNode(pDrawableNode);
  m_pNamiModelRenderPath->RemoveNode(pDrawableNode);
  m_pModelEffectRenderPath->RemoveNode(pDrawableNode);
  m_pEdgeMapPath->RemoveEdgeRenderingTarget(pDrawableNode);
}

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

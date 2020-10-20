//======================================================================
/**
 * @file FinderStudioRenderPipeLine.cpp
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオ描画パイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioRenderPipeLine.h"

// System
#include "System/include/nijiAllocator.h"

// RenderPath
#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"
#include "Renderer/include/gfl2_FrameBufferClearAlphaPath.h"

// btl
#include "Battle/BackGround/include/BgSystem.h"

// PokeTool
#include "PokeTool/include/PokeModel.h"

// FinderStudio
#include "FinderStudioPokemon.h"

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>
#if defined(GF_PLATFORM_WIN32)
#include "niji_conv_header/system/renderpath/wingl/AlphaFillModel.h"
#elif defined(GF_PLATFORM_CTR)
#include "niji_conv_header/system/renderpath/ctr/AlphaFillModel.h"
#endif

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

const u32 FinderStudioRenderPipeLine::NUM_MAX_MODEL = 
  FinderStudioPokemon::GetNumPokeModel() + 
  poke_3d::model::DressUpModel::PARTS_MODEL_COUNT + 
  1; // トレーナー

FinderStudioRenderPipeLine::FinderStudioRenderPipeLine(gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pWorkHeap,gfl2::gfx::IGLAllocator* pAllocator):
  m_RenderPathCnt(0)
  ,m_pEdgeMapRenderPathRes(NULL)
  ,m_pOutLineRenderPathRes(NULL)
  ,m_pEdgeMapPath(NULL)
  ,m_pOutLinePath(NULL)
  ,m_pFloorMaskPath(NULL)
  ,m_pBgRenderPath(NULL)
  ,m_pFarRenderPath(NULL)
  ,m_pSkyBoxRenderPath(NULL)
  ,m_pDepthStencilTexture(NULL)
  ,m_pBufferClearAlphaPath(NULL)
  ,m_pFrameBufferClearAlphaBuffer(NULL)
  ,m_pDrawEnvBase(NULL)
  {
  m_pHeap = pHeap;
  m_pWorkHeap = pWorkHeap;
  m_pAllocator = pAllocator;

  // 通常モデル描画パス作成
  m_pModelRenderPath = GFL_NEW(m_pHeap) System::ModelRenderPath(m_pAllocator,NUM_MAX_MODEL);

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
  desc.m_DrawManagerDesc.m_NodeStackSize = 256;
  desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
  m_pModelRenderPath->Initialize(m_pAllocator,desc);

  // フロアマスクパス初期化
  m_pFloorMaskPath = GFL_NEW(m_pHeap) System::ModelRenderPath(m_pAllocator,1);

  desc.m_DrawManagerDesc.m_NodeStackSize = 1;
  desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;
  m_pFloorMaskPath->Initialize(m_pAllocator,desc);
}
FinderStudioRenderPipeLine::~FinderStudioRenderPipeLine()
{
  GflHeapSafeFreeMemory(m_pEdgeMapRenderPathRes);
  GflHeapSafeFreeMemory(m_pOutLineRenderPathRes);
  if(m_pFrameBufferClearAlphaBuffer)
  {
    GflHeapSafeFreeMemory(m_pFrameBufferClearAlphaBuffer);
  }

  GFL_SAFE_DELETE(m_pModelRenderPath);
  GFL_SAFE_DELETE(m_pEdgeMapPath);
  GFL_SAFE_DELETE(m_pOutLinePath);
  GFL_SAFE_DELETE(m_pFloorMaskPath);
  GFL_SAFE_DELETE(m_pDepthStencilTexture);
  if(m_pBufferClearAlphaPath)
  {
    GFL_SAFE_DELETE(m_pBufferClearAlphaPath);
  }
}

// エッジマップ・アウトラインパスを読み込み
void FinderStudioRenderPipeLine::LoadEdgeMapOutLineAsync()
{
  // 使うときになって初めて確保する
  m_pEdgeMapPath = GFL_NEW(m_pHeap) System::NijiEdgeMapSceneRenderPath(m_pAllocator,NUM_MAX_MODEL);
  m_pOutLinePath = GFL_NEW(m_pHeap) System::NijiOutLinePostSceneRenderPath(m_pAllocator);

  // エッジ・アウトラインリソース読み込み
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId	= ARCID_RENDERPATH;
    openReq.heapForFile = m_pWorkHeap->GetLowerHandle();
    openReq.heapForReq = m_pWorkHeap->GetLowerHandle();
    fileManager->SyncArcFileOpen(openReq);
  }
	{
		gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
		loadReq.arcId = ARCID_RENDERPATH;
		loadReq.align = 128;
		loadReq.heapForBuf = m_pHeap;
    loadReq.heapForReq = m_pWorkHeap->GetLowerHandle();

		loadReq.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
		loadReq.ppBuf = &m_pEdgeMapRenderPathRes;
		fileManager->AddArcFileLoadDataReq(loadReq);

		loadReq.datId = GARC_NijiRenderPath_OutLineResource_BIN;
		loadReq.ppBuf = &m_pOutLineRenderPathRes;
		fileManager->AddArcFileLoadDataReq(loadReq);
	}
}
bool FinderStudioRenderPipeLine::IsLoadEdgeMapOutLine()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  if(fileManager->IsArcFileLoadDataFinished(&m_pEdgeMapRenderPathRes) &&
		fileManager->IsArcFileLoadDataFinished(&m_pOutLineRenderPathRes))
	{
		return true;
	}

  return false;
}
//! @brief エッジマップ・アウトラインパスをセットアップ
//! @return セットアップが成功したらtrue
bool FinderStudioRenderPipeLine::SetupEdgeMapOutLine()
{
  if(!IsLoadEdgeMapOutLine()) return false;

  // エッジマップセットアップ
  m_pEdgeMapPath->SetResource(m_pEdgeMapRenderPathRes);
  m_pEdgeMapPath->CreateEdgeMapTexture(m_pAllocator,512,256);

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
  desc.m_DrawManagerDesc.m_NodeStackSize	  = 256;
  desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

  m_pEdgeMapPath->Initialize(m_pAllocator,desc);

  // アウトラインセットアップ
  m_pOutLinePath->SetResource(m_pOutLineRenderPathRes);

  // アウトラインにエッジマップセット
  for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
	{
  	const gfl2::gfx::Texture *tex = m_pEdgeMapPath->GetEdgeMapTexture( i );
		m_pOutLinePath->SetEdgeMapTexture(i,tex);
	}

#if defined(GF_PLATFORM_CTR)
  // CTRのIDエッジ表示用
  m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(m_pAllocator,gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
  m_pOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID,m_pDepthStencilTexture);
  // Win32版はm_pDepthStencilTextureはNULLのままでいい。
#endif

  // ファイルクローズ
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
	closeReq.arcId = ARCID_RENDERPATH;
	fileManager->SyncArcFileClose(closeReq);

  return true;
}

/// @brief クリアバッファ用パスの初期化
void FinderStudioRenderPipeLine::InitializeBufferClearAlphaPath()
{
  // アルファクリア用パス
  m_pBufferClearAlphaPath = GFL_NEW(m_pHeap) poke_3d::renderer::FrameBufferClearAlphaPath();

  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId = ARCID_RENDERPATH;
  openReq.heapForFile = m_pWorkHeap->GetLowerHandle();
  openReq.heapForReq = m_pWorkHeap->GetLowerHandle();
  fileManager->SyncArcFileOpen(openReq);

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = ARCID_RENDERPATH;
  loadReq.align = 128;
  loadReq.heapForBuf = m_pHeap;
  loadReq.heapForReq = m_pWorkHeap->GetLowerHandle();
  loadReq.datId = GARC_NijiRenderPath_AlphaFillModel_BIN;
  loadReq.ppBuf = &m_pFrameBufferClearAlphaBuffer;
  fileManager->SyncArcFileLoadData(loadReq);

  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = ARCID_RENDERPATH;
  closeReq.heapForReq = m_pWorkHeap->GetLowerHandle();
  fileManager->SyncArcFileClose(closeReq);

  gfl2::fs::BinLinkerAccessor accessor;
  accessor.Initialize(m_pFrameBufferClearAlphaBuffer);
#if defined(GF_PLATFORM_WIN32)
  void *gfbmdlBinary = accessor.GetData(BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_WINGL_GFBMDL);
  void *vertexShaderBinary = accessor.GetData(BL_IDX_ALPHAFILLMODEL_FILLSCREAN_WINGL_GFBVSH);
  void *fragmentShaderBinary = accessor.GetData(BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_MANUAL_WINGL_GFBFSH);
#elif defined(GF_PLATFORM_CTR)
  void *gfbmdlBinary = accessor.GetData(BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_CTR_GFBMDL);
  void *vertexShaderBinary = accessor.GetData(BL_IDX_ALPHAFILLMODEL_FILLSCREAN_CTR_GFBVSH);
  void *fragmentShaderBinary = accessor.GetData(BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_MANUAL_CTR_GFBFSH);
#endif

  m_pBufferClearAlphaPath->Initialize(m_pAllocator,
    reinterpret_cast<c8*>(gfbmdlBinary),
    reinterpret_cast<c8*>(vertexShaderBinary),
    reinterpret_cast<c8*>(fragmentShaderBinary));
}

// 地形描画パスをセット
void FinderStudioRenderPipeLine::SetBgRenderPath(btl::BgSystem* pBgSystem)
{
  m_pBgRenderPath = pBgSystem->GetRenderPath();
  m_pFarRenderPath = pBgSystem->GetFarRenderPath();
  m_pSkyBoxRenderPath = pBgSystem->GetSkyRenderPath();
}

// 背景描画パスをリセット
void FinderStudioRenderPipeLine::ResetBgRenderpath()
{
  m_pBgRenderPath = NULL;
  m_pFarRenderPath = NULL;
  m_pSkyBoxRenderPath = NULL;
}

/// @brief エッジマスクを追加
void FinderStudioRenderPipeLine::AddEdgeMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode)
{
  if(m_pEdgeMapPath) m_pEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
}
void FinderStudioRenderPipeLine::RemoveEdgeMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode)
{
  if(m_pEdgeMapPath) m_pEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
}

/// @brief フロアマスク用モデルをセット
void FinderStudioRenderPipeLine::AddFloorMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode)
{
  m_pFloorMaskPath->AddNode(mdlNode);
}

void FinderStudioRenderPipeLine::RemoveFloorMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode)
{
  m_pFloorMaskPath->RemoveNode(mdlNode);
}

// @brief 描画スタート時処理
b32 FinderStudioRenderPipeLine::StartRenderPath()
{
  m_RenderPathCnt = 0;
  return true;
}

enum{
  PATH_SKY,
  PATH_BG_FAR,
  PATH_BG,
  PATH_FLOOR_MASK,
  PATH_EDGE,
  PATH_MODEL,
  PATH_OUTLINE,
  PATH_CLEAR_BUFFER,
  PATH_MAX
};

// @brief 描画直前 描画パス取得
// @return 使用する描画パス
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* FinderStudioRenderPipeLine::GetRenderPath()
{
  gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* retRenderPath = NULL;

  switch(m_RenderPathCnt)
  {
  case PATH_SKY:
    retRenderPath = m_pSkyBoxRenderPath;
    break;
  case PATH_BG_FAR:
    retRenderPath = m_pFarRenderPath;
    break;
  case PATH_BG:
    retRenderPath = m_pBgRenderPath;
    break;
  case PATH_FLOOR_MASK:
    retRenderPath = m_pFloorMaskPath;
    break;
  case PATH_EDGE:
    retRenderPath = m_pEdgeMapPath;
    break;
  case PATH_MODEL:
    retRenderPath = m_pModelRenderPath;
    break;
  case PATH_OUTLINE:
    retRenderPath = m_pOutLinePath;
    break;
  case PATH_CLEAR_BUFFER:
    retRenderPath = m_pBufferClearAlphaPath;
    break;
  default:
    retRenderPath = NULL;
  }

  return retRenderPath;
}

// @brief 描画直後
// @retval true:次の描画パスへ false:描画終了
b32 FinderStudioRenderPipeLine::NextRenderPath()
{
  m_RenderPathCnt++;
  if(m_RenderPathCnt >= PATH_MAX)
  {
    return false;
  }
  return true;
}

// 環境ノードを登録
void FinderStudioRenderPipeLine::SetDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{ 
  if(m_pModelRenderPath) m_pModelRenderPath->AddDrawEnv( pNode );

  m_pDrawEnvBase = pNode;
}

// ポケモンモデルを追加
void FinderStudioRenderPipeLine::AddObject_Poke( PokeTool::PokeModel* pokeModel,int opt)
{
  m_pModelRenderPath->AddNode(pokeModel->GetModelInstanceNode());

  if(m_pEdgeMapPath) m_pEdgeMapPath->AddEdgeRenderingTarget(pokeModel->GetModelInstanceNode());

  //ライトも追加
  for( int i=0;i<gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSetNode = pokeModel->GetDrawEnvNode()->GetLightSet(i);
    if( lightSetNode )
    {
      m_pDrawEnvBase->AddLightSet(lightSetNode);
    }
  }
}

// ポケモンモデルを除去
void FinderStudioRenderPipeLine::RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt )
{
  m_pModelRenderPath->RemoveNode(pokeModel->GetModelInstanceNode());

  if(m_pEdgeMapPath) m_pEdgeMapPath->RemoveEdgeRenderingTarget(pokeModel->GetModelInstanceNode());

  //ライトも削除
  for( int i=0;i<gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSetNode = pokeModel->GetDrawEnvNode()->GetLightSet(i);
    if( lightSetNode )
    {
      m_pDrawEnvBase->RemoveLightSet(lightSetNode);
      // これがないとメモリリークになると思われ
      pokeModel->GetDrawEnvNode()->AddLightSet(lightSetNode); //親が切れてるから・・・
    }
  }
}

// キャラモデルを追加
void FinderStudioRenderPipeLine::AddObject_Chara( poke_3d::model::CharaModel* charaModel,int opt)
{
  m_pModelRenderPath->AddNode(charaModel->GetModelInstanceNode());
  if(m_pEdgeMapPath) m_pEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());
}

// キャラモデルを除去
void FinderStudioRenderPipeLine::RemoveObject_Chara( poke_3d::model::CharaModel* charaModel,int opt)
{
  m_pModelRenderPath->RemoveNode(charaModel->GetModelInstanceNode());
  if(m_pEdgeMapPath) m_pEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
}

//着せ替えキャラモデルを追加
void FinderStudioRenderPipeLine::AddObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      // NULLチェックしてからADD
      // 生成されていないパーツはNULL
      m_pModelRenderPath->AddNode(pParts);
      if(m_pEdgeMapPath) m_pEdgeMapPath->AddEdgeRenderingTarget(pParts);
    }
  }
}

// 着せ替えキャラモデルを除去
void FinderStudioRenderPipeLine::RemoveObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      // NULLチェックしてからRemove
      // 生成されていないパーツはNULL
      m_pModelRenderPath->RemoveNode(pParts);
      if(m_pEdgeMapPath) m_pEdgeMapPath->RemoveEdgeRenderingTarget(pParts);
    }
  }
}

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

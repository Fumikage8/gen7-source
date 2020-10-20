//==============================================================================
/**
 @file    AppRenderingManagerPokeModelRenderer.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.02
 @brief   PokeModelRenderer
 */
//==============================================================================
#include "AppRenderingManagerPokeModelRenderer.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>

#include <model/include/gfl2_DressUpModel.h>

#include "AppOutLinePostRenderPath.h"

#include <System/include/GflUse.h>

#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(app_rendering_manager_internal) 


////////////////////////////////////////////////////////////////////////////////
//
// AppRenderingManagerで使うポケモンモデルレンダリングパイプライン
//
////////////////////////////////////////////////////////////////////////////////
PokeModelRenderingPipeLine::PokeModelRenderingPipeLine(
    PokeModelRendererType::Mode::Tag  mode,
    app::util::Heap*                  pHeap,                           
    System::nijiAllocator*            pNijiAllocatorForEdgeMapTexture, 
    void*                             pEdgeMapResBuf,
    void*                             pOutLineResBuf,
    u32                               pokeModelNumMax,                    
    u32                               modelNumMax,
    u32                               charaModelNumMax, 
    u32                               dressUpModelNumMax,
    bool                              useDistortionPath
)
  : gfl2::renderingengine::renderer::RenderingPipeLine(),
    m_mode(mode),
    m_RenderPathCnt(0),
    m_pEdgeMapPath(NULL),
    m_pOutLinePath(NULL),
    m_pModelRenderPath(NULL),
    m_pStretchBltFrameBufferPath(NULL),
    m_pDistortionPostRenderPath(NULL),
    m_pDepthStencilTexture(NULL),
    m_pOuterDepthStencilTexture(NULL),
    m_pNijiAllocatorForEdgeMapTexture(pNijiAllocatorForEdgeMapTexture),
    m_pInFrameCamera(NULL),
    m_pAppCamera(NULL),
    m_pCamera(NULL),
    m_pokeModelSys(NULL)
{
  u32 l_modelNumMax = pokeModelNumMax * 2 + modelNumMax + charaModelNumMax + dressUpModelNumMax * poke_3d::model::DressUpModel::PARTS_MODEL_COUNT;  // pokeModelNumMaxは２倍しておく（本体と影）

  // EdgeMapPath
  m_pEdgeMapPath = GFL_NEW(pHeap->GetDeviceHeap()) System::NijiEdgeMapSceneRenderPath(pHeap->GetDevGLHeapAllocator(),l_modelNumMax);
  m_pEdgeMapPath->SetResource(pEdgeMapResBuf);
  m_pEdgeMapPath->CreateEdgeMapTexture(pNijiAllocatorForEdgeMapTexture, 512, 256);

  // OutLinePath
  if(m_mode == PokeModelRendererType::Mode::APP_ORIGINAL)
  {
    m_pOutLinePath = GFL_NEW(pHeap->GetDeviceHeap()) app::util::AppOutLinePostRenderPath(pHeap->GetDevGLHeapAllocator());
  }
  else
  {
    m_pOutLinePath = GFL_NEW(pHeap->GetDeviceHeap()) System::NijiOutLinePostSceneRenderPath(pHeap->GetDevGLHeapAllocator());
  }
  m_pOutLinePath->SetResource(pOutLineResBuf);

  for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
  {
    const gfl2::gfx::Texture* tex = m_pEdgeMapPath->GetEdgeMapTexture( i );
    m_pOutLinePath->SetEdgeMapTexture(i, tex);
  }

#if defined(GF_PLATFORM_CTR)
  // @fix GFNMCat[3636] アプリに出るポケモンモデル：CTR版でＩＤエッジが表示されていない
  if(m_mode == PokeModelRendererType::Mode::COMMON)  // オフスクリーンのときは内部でテクスチャ生成は行わず、後で外部から受け取ってSetだけする。
  {
    m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(pHeap->GetDevGLHeapAllocator(), gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
    m_pOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, m_pDepthStencilTexture);
  }
  // Win32版はm_pDepthStencilTextureはNULLのままでいい。

  // 石黒さん曰く
  // ＣＴＲは、ステンシルバッファに書き込まれた値によりＩＤエッジを生成しています。
  // (windowsは深度ステンシルバッファをテクスチャとして使えないので、オフスクリーンで持っています。)
  // とのこと。
#endif

  // ModelRenderPath
  m_pModelRenderPath = GFL_NEW(pHeap->GetDeviceHeap()) System::ModelRenderPath(pHeap->GetDevGLHeapAllocator(),l_modelNumMax);
 
  // ModelRenderPath, EdgeMapPath
  {
    // メインとなるシーン描画パスを初期化
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize    = l_modelNumMax;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 160;

    /*
    ポケモン数がXのとき・・・

    ポケモン１体辺り
    マテリアルは最大で１６個です。
    その中のメッシュ(DrawTag)はジョイント最大数２０で分割されています。
    現状、ジョイントの最大は１３０としていますので、
    最悪７～１０分割されるとして、

    m_NodeStackSize = X
    m_DrawTagStackSize = X x１６０

    あれば（ほぼ）良いということになります。
    */

    m_pModelRenderPath->Initialize( pHeap->GetDevGLHeapAllocator(), desc );
    m_pEdgeMapPath->Initialize( pHeap->GetDevGLHeapAllocator(), desc );
  }

  if(useDistortionPath)
  {
    // StretchBltFrameBufferPath
    m_pStretchBltFrameBufferPath = GFL_NEW(pHeap->GetDeviceHeap()) gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath();
    gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    m_pStretchBltFrameBufferPath->CreateFrameBufferTexture(pNijiAllocatorForEdgeMapTexture, pBackBuffer);

    // DistortionPostRenderPath
    m_pDistortionPostRenderPath  = GFL_NEW(pHeap->GetDeviceHeap()) poke_3d::renderer::DistortionPostRenderPath();
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize  = 4;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;
    m_pDistortionPostRenderPath->Initialize(pHeap->GetDevGLHeapAllocator(), desc);
    m_pDistortionPostRenderPath->CreateBuffer(pHeap->GetDevGLHeapAllocator(), 4);
    m_pDistortionPostRenderPath->SetOverrideTexture( m_pStretchBltFrameBufferPath->GetFrameBufferTexture() );
  }
}
PokeModelRenderingPipeLine::~PokeModelRenderingPipeLine()
{
  // DistortionPostRenderPath
  GFL_SAFE_DELETE(m_pDistortionPostRenderPath);
  // StretchBltFrameBufferPath
  GFL_SAFE_DELETE(m_pStretchBltFrameBufferPath);

  // ModelRenderPath
  GFL_SAFE_DELETE(m_pModelRenderPath);

  // m_pDepthStencilTexture
  GFL_SAFE_DELETE(m_pDepthStencilTexture);

  // OutLinePath
  GFL_SAFE_DELETE(m_pOutLinePath);

  // EdgeMapPath
  GFL_SAFE_DELETE(m_pEdgeMapPath);
}

b32 PokeModelRenderingPipeLine::StartRenderPath()
{
  m_RenderPathCnt = 0;
  return true;
}
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* PokeModelRenderingPipeLine::GetRenderPath()
{
  u32 pokeShaderPathNum = 0;
  if( m_pokeModelSys )
  {
    pokeShaderPathNum = m_pokeModelSys->GetPokeShaderPathNum();
    if( m_RenderPathCnt < pokeShaderPathNum )
    {
      return m_pokeModelSys->GetPokeShaderPath(m_RenderPathCnt);
    }
  }

  switch( m_RenderPathCnt - pokeShaderPathNum ){
  case 0:
    return m_pEdgeMapPath;
    //returnするからbreak要らない
  case 1:
    return m_pModelRenderPath;
    //returnするからbreak要らない
  case 2:
    return m_pOutLinePath;
    //returnするからbreak要らない
  case 3:
    return m_pStretchBltFrameBufferPath;  // ゆがみシェーダーを使用しないときはNULL
    //returnするからbreak要らない
  case 4:
    return m_pDistortionPostRenderPath;  // ゆがみシェーダーを使用しないときはNULL
    //returnするからbreak要らない
  }
  return NULL;
}
b32 PokeModelRenderingPipeLine::NextRenderPath()
{
  m_RenderPathCnt++;

  u32 pokeShaderPathNum = 0;
  if( m_pokeModelSys )
  {
    pokeShaderPathNum = m_pokeModelSys->GetPokeShaderPathNum();
  }

  if ( m_RenderPathCnt == 5 + pokeShaderPathNum )
    return false;
  return true;
}

System::NijiEdgeMapSceneRenderPath* PokeModelRenderingPipeLine::GetEdgeMapPath(void) const
{
  return m_pEdgeMapPath;
}
System::ModelRenderPath*            PokeModelRenderingPipeLine::GetModelRenderPath(void) const
{
  return m_pModelRenderPath;
}
poke_3d::renderer::DistortionPostRenderPath* PokeModelRenderingPipeLine::GetDistortionPath(void) const  // ゆがみシェーダーを使用しないときはNULL
{
  return m_pDistortionPostRenderPath;
}

void PokeModelRenderingPipeLine::SetOutLinePathOuterEdgeMapTexture(gfl2::gfx::Texture* pOuterDepthStencilTexture)
{
  // オフスクリーンのときは外部からデプスステンシルテクスチャをセット/アンセットする。
#if defined(GF_PLATFORM_CTR)
  if(pOuterDepthStencilTexture)
  {
    // @fix GFNMCat[3636] アプリに出るポケモンモデル：CTR版でＩＤエッジが表示されていない
    m_pOuterDepthStencilTexture = pOuterDepthStencilTexture;
    m_pOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, pOuterDepthStencilTexture);
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
#endif
}
void PokeModelRenderingPipeLine::UnsetOutLinePathOuterEdgeMapTexture(void)
{
  // オフスクリーンのときは外部からデプスステンシルテクスチャをセット/アンセットする。
#if defined(GF_PLATFORM_CTR)
  m_pOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, NULL);
  m_pOuterDepthStencilTexture = NULL;
#endif
}
gfl2::gfx::Texture* PokeModelRenderingPipeLine::GetOutLinePathOuterEdgeMapTexture(void) const
{
  gfl2::gfx::Texture* ret = NULL; 
#if defined(GF_PLATFORM_CTR)
  ret = m_pOuterDepthStencilTexture;
#endif
  return ret;
}

const System::Camera::InFrameCamera* PokeModelRenderingPipeLine::GetInFrameCamera(void) const
{
  return m_pInFrameCamera;
}
void PokeModelRenderingPipeLine::SetInFrameCamera(const System::Camera::InFrameCamera* pCamera)
{
  if(pCamera)
  {
    m_pInFrameCamera = pCamera;
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
}
void PokeModelRenderingPipeLine::UnsetInFrameCamera(void)
{
  m_pInFrameCamera = NULL;
}

const app::util::AppCamera* PokeModelRenderingPipeLine::GetAppCamera(void) const
{
  return m_pAppCamera;
}
void PokeModelRenderingPipeLine::SetAppCamera(const app::util::AppCamera* pCamera)
{
  if(pCamera)
  {
    m_pAppCamera = pCamera;
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
}
void PokeModelRenderingPipeLine::UnsetAppCamera(void)
{
  m_pAppCamera = NULL;
}

const poke_3d::model::BaseCamera* PokeModelRenderingPipeLine::GetCamera(void) const
{
  return m_pCamera;
}
void PokeModelRenderingPipeLine::SetCamera(const poke_3d::model::BaseCamera* pCamera)
{
  if(pCamera)
  {
    m_pCamera = pCamera;
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
}
void PokeModelRenderingPipeLine::UnsetCamera(void)
{
  m_pCamera = NULL;
}




////////////////////////////////////////////////////////////////////////////////
//
// AppRenderingManagerで使うポケモンモデルレンダラー生成/破棄クラス
//
////////////////////////////////////////////////////////////////////////////////
PokeModelRendererBuilder::PokeModelRendererBuilder(void)
  : m_pEdgeMapResBuf(NULL),
    m_pOutLineResBuf(NULL),
    m_createState(CREATE_STATE_NONE),
    m_pAsyncFileManager(NULL)
{}
PokeModelRendererBuilder::~PokeModelRendererBuilder()
{
  if( !this->CanDestroy() )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。これを抜けたら消すしかない。
  }

  GflHeapSafeFreeMemory(m_pOutLineResBuf);
  GflHeapSafeFreeMemory(m_pEdgeMapResBuf);
}

void PokeModelRendererBuilder::StartAsyncCreate(
    app::util::Heap*            pHeap,
    gfl2::fs::AsyncFileManager* pAsyncFileManager
)
{
  if(m_createState == CREATE_STATE_NONE)
  {
    m_createState = CREATE_STATE_CREATING;

    // リソースの読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_RENDERPATH;
      openReq.heapForFile = pHeap->GetDeviceHeap()->GetLowerHandle();  // すぐに閉じるので後方確保
      openReq.heapForReq = pHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileManager->AddArcFileOpenReq(openReq);
 
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
        loadReq.arcId = ARCID_RENDERPATH;
        loadReq.align = 128;
        loadReq.heapForBuf = pHeap->GetDeviceHeap();
        loadReq.heapForReq = pHeap->GetDeviceHeap()->GetLowerHandle();
       
        loadReq.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
        loadReq.ppBuf = &(m_pEdgeMapResBuf);
        pAsyncFileManager->AddArcFileLoadDataReq(loadReq);
       
        loadReq.datId = GARC_NijiRenderPath_OutLineResource_BIN;
        loadReq.ppBuf = &(m_pOutLineResBuf);
        pAsyncFileManager->AddArcFileLoadDataReq(loadReq);
      }
 
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_RENDERPATH;
      closeReq.heapForReq = pHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileManager->AddArcFileCloseReq(closeReq);
    }

    m_pAsyncFileManager = pAsyncFileManager;
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}

void PokeModelRendererBuilder::SyncCreate(
    app::util::Heap*            pHeap,
    gfl2::fs::AsyncFileManager* pAsyncFileManager 
)
{
  if(m_createState == CREATE_STATE_NONE)
  {
    m_createState = CREATE_STATE_CREATING;

    // リソースの読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_RENDERPATH;
      openReq.heapForFile = pHeap->GetDeviceHeap()->GetLowerHandle();  // すぐに閉じるので後方確保
      openReq.heapForReq = pHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileManager->SyncArcFileOpen(openReq);
 
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
        loadReq.arcId = ARCID_RENDERPATH;
        loadReq.align = 128;
        loadReq.heapForBuf = pHeap->GetDeviceHeap();
        loadReq.heapForReq = pHeap->GetDeviceHeap()->GetLowerHandle();
       
        loadReq.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
        loadReq.ppBuf = &(m_pEdgeMapResBuf);
        pAsyncFileManager->SyncArcFileLoadData(loadReq);
       
        loadReq.datId = GARC_NijiRenderPath_OutLineResource_BIN;
        loadReq.ppBuf = &(m_pOutLineResBuf);
        pAsyncFileManager->SyncArcFileLoadData(loadReq);
      }
 
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_RENDERPATH;
      closeReq.heapForReq = pHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileManager->SyncArcFileClose(closeReq);
    }
    
    m_createState = CREATE_STATE_CREATED;
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}

bool PokeModelRendererBuilder::IsCreated(void) const
{
  return (m_createState == CREATE_STATE_CREATED);
}

bool PokeModelRendererBuilder::CanDestroy(void) const
{
  return (m_createState == CREATE_STATE_NONE || m_createState == CREATE_STATE_CREATED);
}

PokeModelRenderingPipeLine* PokeModelRendererBuilder::CreateRenderingPipeLine(
    PokeModelRendererType::Mode::Tag  mode,
    app::util::Heap*                  pHeap,                          
    System::nijiAllocator*            pNijiAllocatorForEdgeMapTexture,
    u32                               pokeModelNumMax,                   
    u32                               modelNumMax,
    u32                               charaModelNumMax,
    u32                               dressUpModelNumMax,
    bool                              useDistortionPath
)
{
  PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine = NULL;
  if(m_createState == CREATE_STATE_CREATED && m_pEdgeMapResBuf && m_pOutLineResBuf)
  {
    pPokeModelRenderingPipeLine = GFL_NEW(pHeap->GetDeviceHeap()) PokeModelRenderingPipeLine(
        mode,
        pHeap,
        pNijiAllocatorForEdgeMapTexture,
        m_pEdgeMapResBuf,
        m_pOutLineResBuf,
        pokeModelNumMax,
        modelNumMax,
        charaModelNumMax,
        dressUpModelNumMax,
        useDistortionPath
    );
  }
  else
  {
    GFL_ASSERT(0);  // 生成に必要なデータがない。  // 開発中に気付かせるためのASSERT
  }
  return pPokeModelRenderingPipeLine;
}
void PokeModelRendererBuilder::DestroyRenderingPipeLine(PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine)
{
  GFL_SAFE_DELETE(pPokeModelRenderingPipeLine);
}

void PokeModelRendererBuilder::Update(void)
{
  if(m_createState == CREATE_STATE_CREATING)
  {
    bool isCreated = true;

    // リソース読み込み待ち
    {
      if(
             m_pAsyncFileManager->IsArcFileOpenFinished(ARCID_RENDERPATH)
          && m_pAsyncFileManager->IsArcFileLoadDataFinished(&(m_pEdgeMapResBuf))
          && m_pAsyncFileManager->IsArcFileLoadDataFinished(&(m_pOutLineResBuf))
          && m_pAsyncFileManager->IsArcFileCloseFinished(ARCID_RENDERPATH)
      )
      {
        // 読み込みが完了したのでisCreatedは変更しない
      }
      else
      {
        isCreated = false;
      }
    }

    if(isCreated)
    {
      m_pAsyncFileManager = NULL;  // もう使わないのでNULLにしておく
      m_createState = CREATE_STATE_CREATED;
    }
  }
}


GFL_NAMESPACE_END(app_rendering_manager_internal) 
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

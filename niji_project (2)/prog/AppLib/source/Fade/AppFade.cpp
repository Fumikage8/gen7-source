//=============================================================================
/**
 *
 *	@file		AppFade.cpp
 *	@brief  フェードの最初の設定を纏めたクラス
 *          基本的には gfl2::Fade::FadeManagerをみんなで呼べば事足りるはず。
 *	@author k.ohno
 *
 */
//=============================================================================

#include "System/include/HeapDefine.h"

#include "AppLib/include/Fade/AppFade.h"
#include "Fade/include/gfl2_FadeManager.h"
#include "GameSys/include/GameManager.h"
// AppLib
#include "AppLib/include/Fade/LayoutFade.h"
#include "AppLib/include/Util/AppRenderingManager.h"
// System
#include "System/include/GflUse.h"
#include "System/include/nijiAllocator.h"
// arc
#include <arc_def_index/arc_def.h>
#include "arc_index/NijiRenderPath.gaix"
#if defined(GF_PLATFORM_WIN32)
#include "niji_conv_header/system/renderpath/wingl/AlphaFillModel.h"
#elif defined(GF_PLATFORM_CTR)
#include "niji_conv_header/system/renderpath/ctr/AlphaFillModel.h"
#endif

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)

static const s32 FADE_SYSTEM_HEAP_SIZE = 0x35000;
static const s32 FADE_DEVICE_HEAP_SIZE = 0x35000;

    /**
     *	@brief  コンストラクタ
     *	@param  heap_allocator  ヒープアロケータ
     *	@param  device_allocator  デバイスヒープアロケータ
     *	@param  grpsys          グラフィックシステム
     *  @param  lyt_res         レイアウトリソース(デフォルト引数:NULL)
     */
AppFade::AppFade(void)
{
  m_pFrameBufferClearAlphaBuffer = NULL;
}

    /**
     *	@brief  デストラクタ
     */
AppFade::~AppFade(void)
{
  End();
}

/**
 *	@brief  初期化処理
 */

void AppFade::Init(gfl2::heap::HeapBase* pSys,gfl2::heap::HeapBase* pDev)
{

  // このアプリ全体に必要なメモリを作成
  // ローカルヒープを作る
  m_pAppHeap = GFL_NEW( pDev ) app::util::Heap();
	m_pAppHeap->LocalHeapCreate( pSys, pDev, FADE_SYSTEM_HEAP_SIZE, FADE_DEVICE_HEAP_SIZE );

  m_pFadeManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::Fade::FadeManager();

#if OPEN_FADE_MACRO
  // クロスフェード用
  m_pFadeManager->CreateColorBuffer( System::GflUse::GetResidentAllocator(), gfl2::gfx::Pool::LocalA, gfl2::gfx::Pool::LocalB );

  // パイプライン生成
  app::util::AppRenderingManager::Description appRenderingMgrDesc;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  appRenderingMgrDesc.isUpperDisplayStereo = true;
  m_pAppRenderingMgr = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::util::AppRenderingManager();
  m_pAppRenderingMgr->SyncCreate( appRenderingMgrDesc, m_pAppHeap, NULL, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
  GFL_ASSERT( m_pAppRenderingMgr->IsCreated() );  // 同期生成なので m_pAppRenderingMgr->IsCreated() では待たない。

  gfl2::fs::AsyncFileManager *pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
  // 上画面用
  m_pUpperFade = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) AppLib::Fade::LayoutFade();
  m_pFadeManager->AddFade( gfl2::Fade::DISP_UPPER, m_pUpperFade );
  m_pUpperFade->Initialize( m_pAppHeap, m_pAppRenderingMgr, pAsyncFileManager );
  // 下画面用
  m_pLowerFade = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) AppLib::Fade::LayoutFade();
  m_pFadeManager->AddFade( gfl2::Fade::DISP_LOWER, m_pLowerFade );
  m_pLowerFade->CloneLayout( m_pUpperFade );

  // クロスフェード用
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_RENDERPATH;
    openReq.heapForFile = pDev->GetLowerHandle();
    openReq.heapForReq = pDev->GetLowerHandle();
    pAsyncFileManager->SyncArcFileOpen( openReq );

    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_RENDERPATH;
    loadReq.align = 128;
    loadReq.heapForBuf = pDev;
    loadReq.heapForReq = pDev->GetLowerHandle();
    loadReq.datId = GARC_NijiRenderPath_AlphaFillModel_BIN;
    loadReq.ppBuf = &m_pFrameBufferClearAlphaBuffer;
    pAsyncFileManager->SyncArcFileLoadData(loadReq);

    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_RENDERPATH;
    closeReq.heapForReq = pDev->GetLowerHandle();
    pAsyncFileManager->SyncArcFileClose(closeReq);

    gfl2::fs::BinLinkerAccessor accessor;
    accessor.Initialize( m_pFrameBufferClearAlphaBuffer );
#if defined(GF_PLATFORM_WIN32)
    void *gfbmdlBinary         = accessor.GetData( BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_WINGL_GFBMDL );
    void *vertexShaderBinary   = accessor.GetData( BL_IDX_ALPHAFILLMODEL_FILLSCREAN_WINGL_GFBVSH );
    void *fragmentShaderBinary = accessor.GetData( BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_MANUAL_WINGL_GFBFSH );
#elif defined(GF_PLATFORM_CTR)
    void *gfbmdlBinary         = accessor.GetData( BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_CTR_GFBMDL );
    void *vertexShaderBinary   = accessor.GetData( BL_IDX_ALPHAFILLMODEL_FILLSCREAN_CTR_GFBVSH );
    void *fragmentShaderBinary = accessor.GetData( BL_IDX_ALPHAFILLMODEL_ALPHAFILLMODEL_MANUAL_CTR_GFBFSH );
#endif
    m_pFadeManager->createPipeLine( pDev, System::GflUse::GetResidentAllocator(), gfbmdlBinary, vertexShaderBinary, fragmentShaderBinary );
  }
#endif
}

void AppFade::End(void)
{
#if OPEN_FADE_MACRO
  GFL_SAFE_DELETE( m_pUpperFade );
  GFL_SAFE_DELETE( m_pLowerFade );

  GFL_ASSERT( m_pAppRenderingMgr->CanDestroy() );  // 同期生成なので m_pAppRenderingMgr->CanDestroy() では待たない。
  GFL_SAFE_DELETE(m_pAppRenderingMgr);

#endif

  GFL_SAFE_DELETE(m_pFadeManager);
  if( m_pFrameBufferClearAlphaBuffer )
  {
    GflHeapSafeFreeMemory( m_pFrameBufferClearAlphaBuffer );
  }
  GFL_SAFE_DELETE(m_pAppHeap);
}


void AppFade::Update(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->Update();//fade
  m_pAppRenderingMgr->Update();
}

void AppFade::PreDraw(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->PreDraw();
}

void AppFade::Draw(gfl2::gfx::CtrDisplayNo no)
{
  if(!  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEndStatus() ){

    gfl2::renderingengine::renderer::RenderingPipeLine *pPipeLine = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->GetCrossFadePipeLine();
    if( pPipeLine )
    {
      gfl2::gfx::Surface *pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer(    System::GflUse::m_HandleList[ no ] );
      u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth(  System::GflUse::m_HandleList[ no ] );
      u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ no ] );
      pPipeLine->SetRenderTarget    ( pBackBuffer  );
      pPipeLine->SetDisplaySize     ( displayWidth, displayHeight );
    }
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->Draw(no);
    m_pAppRenderingMgr->Draw(no);
  }
}



GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)






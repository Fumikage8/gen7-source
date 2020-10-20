//=============================================================================
/**
 * @file    WazaOmoidasiProc.cpp
 * @brief   技思い出し画面Proc処理
 * @author  Hiroyuki Nakamura
 * @date    2015.12.08
 */
//=============================================================================

// module
#include "App/WazaOmoidasi/include/WazaOmoidasiProc.h"
#include "App/WazaOmoidasi/include/WazaOmoidasiAppParam.h"
#include "WazaOmoidasiFrame.h"
// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/DllProc.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(WazaOmoidasi)

static const u32 MANAGER_HEAP_SIZE = 0x1000;          //!< マネージャヒープサイズ
static const u32 LOCAL_HEAP_SIZE        = 0x080000;   //!< ローカルヒープサイズ
static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x500000;   //!< ローカルデバイスヒープサイズ

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
WazaOmoidasiProc::WazaOmoidasiProc(void)
  : m_param( NULL )
  , m_pManagerHeap( NULL )
  , m_pRenderingManager( NULL )
  , m_pFrameManager( NULL )
  , m_pFrame( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
WazaOmoidasiProc::~WazaOmoidasiProc()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   セットアップ
 * @date    2015.12.10
 *
 * @param   param   外部設定パラメータ
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiProc::Setup( APP_PARAM * param )
{
  m_param = param;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   Proc初期化処理
 * @date    2015.12.10
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result WazaOmoidasiProc::InitFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    CreateHeap();
    CreateRenderManager();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    m_pRenderingManager->Update();
    if( m_pRenderingManager->IsCreated() == false )
    {
      break;
    }
    CreateFrame();
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   Proc終了処理
 * @date    2015.12.10
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result WazaOmoidasiProc::EndFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    if( DeleteFrame() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( DeleteRenderManager() == false )
    {
      break;
    }
    DeleteHeap();
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   Procアップデート処理
 * @date    2015.12.10
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result WazaOmoidasiProc::UpdateFunc( gfl2::proc::Manager * pManager )
{
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }
  m_pRenderingManager->Update();
	return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    PreDrawFunc
 * @brief   Proc描画下準備
 * @date    2015.12.10
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiProc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  m_pRenderingManager->PreDraw();
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   Proc描画処理
 * @date    2015.12.10
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pFrameManager->Draw( displayNo );
  m_pRenderingManager->Draw( displayNo );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateHeap
 * @brief   ヒープ生成
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiProc::CreateHeap(void)
{
  gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate( dev_heap, dev_heap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteHeap
 * @brief   ヒープ削除
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiProc::DeleteHeap(void)
{
  // ローカルヒープ削除
  m_heap.Delete();
  // マネージャヒープ削除
  GFL_DELETE_HEAP( m_pManagerHeap );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateRenderManager
 * @brief   描画マネージャ生成
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiProc::CreateRenderManager(void)
{
  m_pRenderingManager = GFL_NEW(m_pManagerHeap) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  for( int i=0; i<app::util::AppRenderingManager::RenderPlaceType::NUM; i++ )
  {
    desc.renderDesc[i].renderOrder     = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[i].pokeModelNumMax = 0;
    desc.renderDesc[i].modelNumMax     = 0;
  }
  desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART;

  m_pRenderingManager->StartAsyncCreate(
    desc,
    &m_heap,
    NULL,
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRenderManager
 * @brief   描画マネージャ削除
 * @date    2015.12.10
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiProc::DeleteRenderManager(void)
{
  if( m_pRenderingManager->CanDestroy() == false )
  {
    return false;
  }
  GFL_DELETE m_pRenderingManager;
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateFrame
 * @brief   フレーム生成
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiProc::CreateFrame(void)
{
  m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
  m_pFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, WazaOmoidasiFrame >( m_pFrameManager );
  m_pFrame->Setup( m_param, &m_heap, m_pRenderingManager );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFrame
 * @brief   フレーム削除
 * @date    2015.12.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiProc::DeleteFrame(void)
{
  if( m_pFrameManager == NULL )
  {
    return true;
  }
  if( m_pFrameManager->End() != false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pFrameManager );
  return true;
}


GFL_NAMESPACE_END(WazaOmoidasi)
GFL_NAMESPACE_END(App)

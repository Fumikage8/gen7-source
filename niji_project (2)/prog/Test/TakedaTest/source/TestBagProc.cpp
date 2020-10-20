//=============================================================================
/**
 * @file    BagProc.cpp
 * @brief   バッグ画面Proc関連
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================
#if PM_DEBUG

// module
#include "Test/TakedaTest/include/TestBagProc.h"
#include "Test/TakedaTest/include/TestBagAppParam.h"
#include "Test/TakedaTest/include/TestBagFrame.h"

#include "Field/FieldRo/include/Window/FieldWindowManager.h"
// niji
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"
#include "Applib/include/Frame/AppFrameManager.h"
#include "Applib/include/Util/AppRenderingManager.h"


GFL_NAMESPACE_BEGIN(takedaTest_app)

static const u32 MANAGER_HEAP_SIZE = 0x1000;
static const u32 LOCAL_HEAP_SIZE        = 0x100000;
static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x300000;

  Field::FieldWindow::FieldWindowManager    *m_pFieldWindowManager;

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
BagProc::BagProc(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
BagProc::~BagProc()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   外部設定パラメータをセット
 * @date    2015.03.04
 *
 * @param   param   外部設定パラメータ
 */
//-----------------------------------------------------------------------------
void BagProc::Setup( s32 idx )
{
  m_iIdx = idx;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   Proc初期化処理
 * @date    2015.03.04
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result BagProc::InitFunc( gfl2::proc::Manager * pManager )
{
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate( heap, dev_heap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );

  // パイプライン生成
  app::util::AppRenderingManager::Description renderingManagerDesc;
  renderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  renderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  m_pRenderingManager = GFL_NEW(m_pManagerHeap) app::util::AppRenderingManager();
  m_pRenderingManager->SyncCreate( renderingManagerDesc, &m_heap, NULL, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
  GFL_ASSERT( m_pRenderingManager->IsCreated() );  // 同期生成なので m_pRenderingManager->IsCreated() では待たない。

  // フレーム生成
  m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
  m_pBagFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BagFrame >( m_pFrameManager );
  m_pFieldWindowManager = GFL_NEW(m_pManagerHeap) Field::FieldWindow::FieldWindowManager( dev_heap, &m_heap, m_pRenderingManager, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
  
#ifdef USE_GAMESYS_FIELDWINDOW
  GameSys::GameManager::GetInstance()->SetFieldWindowManager( m_pFieldWindowManager );
#endif
  m_pBagFrame->Setup( m_pFieldWindowManager->GetAppHeap(), m_iIdx );


  return gfl2::proc::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   Proc終了処理
 * @date    2015.03.04
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result BagProc::EndFunc( gfl2::proc::Manager * pManager )
{
  // 終了処理が完了するまで待つ
  if( m_pFrameManager->End() == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  GFL_DELETE m_pFrameManager;

  GFL_ASSERT( m_pRenderingManager->CanDestroy() );  // 同期生成なので m_pRenderingManager->CanDestroy() では待たない。
  GFL_DELETE m_pRenderingManager;
  GFL_SAFE_DELETE( m_pFieldWindowManager );

  m_heap.Delete();

  GFL_DELETE_HEAP( m_pManagerHeap );

  return gfl2::proc::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   Procアップデート処理
 * @date    2015.03.04
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result BagProc::UpdateFunc( gfl2::proc::Manager * pManager )
{
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }
  m_pFieldWindowManager->Update();

  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   Proc描画処理
 * @date    2015.03.04
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void BagProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pRenderingManager->Update();  // @todo UpdateFuncで呼んで下さい。PreDrawも呼んで下さい。
  m_pFieldWindowManager->Draw( displayNo );
  m_pFrameManager->Draw( displayNo );
  m_pRenderingManager->Draw( displayNo );
#if 0
  Field::FieldWindow::FieldWindowManager      *pFieldWin = GameSys::GameManager::GetInstance()->GetFieldWindowManager();

  m_pRenderingManager->Update();  // @todo

  pFieldWin->Draw();  ///< @todo 場所が適切かどうかは置いておく.
  m_pFrameManager->Draw( displayNo );

  m_pRenderingManager->Draw( displayNo );
#endif
}


GFL_NAMESPACE_END(takedaTest_app)

#endif // PM_DEBUG

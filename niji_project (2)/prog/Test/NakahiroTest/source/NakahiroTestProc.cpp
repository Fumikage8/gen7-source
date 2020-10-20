#if PM_DEBUG
//=============================================================================
/**
 * @file    TestProc.cpp
 * @brief   中村用テスト処理
 * @author  Hiroyuki Nakamura
 * @date    2015.07.21
 */
//=============================================================================

// module
#include "Test/NakahiroTest/include/NakahiroTestProc.h"
#include "HScroll/HScrollFrame.h"
#include "BoxChatLow/BoxChatLowFrame.h"
// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/DllProc.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

static const u32 MANAGER_HEAP_SIZE = 0x1000;          //!< マネージャヒープサイズ
static const u32 LOCAL_HEAP_SIZE        = 0x200000;   //!< ローカルヒープサイズ
static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x500000;   //!< ローカルデバイスヒープサイズ


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
TestProc::TestProc(void)
  : m_mode( MODE_H_SCROLL )
  , m_pHScrollFrame( NULL )
  , m_pBoxChatLowFrame( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
TestProc::~TestProc()
{
}

void TestProc::SetupMode( Mode mode )
{
  m_mode = mode;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   Proc初期化処理
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result TestProc::InitFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    CreateHeap();
    CreateRenderManager();
    AddSubSeq();
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
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result TestProc::EndFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    if( DeleteFrame() == false )
    {
      break;
    }
    AddSubSeq();
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
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result TestProc::UpdateFunc( gfl2::proc::Manager * pManager )
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
 * @date    2015.07.09
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void TestProc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  m_pRenderingManager->PreDraw();
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   Proc描画処理
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void TestProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pFrameManager->Draw( displayNo );
  m_pRenderingManager->Draw( displayNo );
}


void TestProc::CreateHeap(void)
{
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
	gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate( heap, dev_heap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
}

void TestProc::DeleteHeap(void)
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
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void TestProc::CreateRenderManager(void)
{
  m_pRenderingManager = GFL_NEW(m_pManagerHeap) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  for( int i=0; i<app::util::AppRenderingManager::RenderPlaceType::NUM; i++ )
  {
    desc.renderDesc[i].renderOrder     = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[i].pokeModelNumMax = 0;
    desc.renderDesc[i].modelNumMax     = 0;
  }

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
 * @date    2015.07.09
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TestProc::DeleteRenderManager(void)
{
  if( m_pRenderingManager->CanDestroy() == false )
  {
    return false;
  }
  GFL_DELETE m_pRenderingManager;
  return true;
}

void TestProc::CreateFrame(void)
{
  // フレーム生成
  m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );

  switch( m_mode )
  {
  case MODE_H_SCROLL:
    m_pHScrollFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, HScrollFrame >( m_pFrameManager );
    m_pHScrollFrame->Setup( &m_heap, m_pRenderingManager );
    break;

  case MODE_BOX_CHAT_LOW:
    m_pBoxChatLowFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxChatLowFrame >( m_pFrameManager );
    m_pBoxChatLowFrame->Setup( &m_heap, m_pRenderingManager );
    break;
  }
}

bool TestProc::DeleteFrame(void)
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


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)

#endif  // PM_DEBUG

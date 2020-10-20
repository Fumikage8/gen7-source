#if PM_DEBUG
//=============================================================================
/**
 * @file    StaffListProc.cpp
 * @brief   momiji追加分のスタッフロールProc処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.02
 */
//=============================================================================

// module
#include "App/StaffList/include/StaffListProc.h"
#include "App/StaffList/include/FieldStaffList.h"
// momiji
#include "System/include/HeapDefine.h"
#include "AppLib/include/Util/AppRenderingManager.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

static const u32 MANAGER_HEAP_SIZE = 0x2000;          //!< マネージャヒープサイズ
static const u32 LOCAL_HEAP_SIZE        = 0x0004000;  //!< ローカルヒープサイズ
static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x0004000;  //!< ローカルデバイスヒープサイズ

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
StaffListProc::StaffListProc(void)
  : m_pView( NULL )
  , m_pManagerHeap( NULL )
  , m_pRenderingManager( NULL )
  , m_pFieldStaffList( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
StaffListProc::~StaffListProc()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   Proc初期化処理
 * @date    2017.03.02
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result StaffListProc::InitFunc( gfl2::proc::Manager * pManager )
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
    CreateView();
    CreateFieldStaffList();
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   Proc終了処理
 * @date    2017.03.02
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result StaffListProc::EndFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    if( DeleteFieldStaffList() == false )
    {
      break;
    }
    DeleteView();
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
 * @date    2017.03.02
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result StaffListProc::UpdateFunc( gfl2::proc::Manager * pManager )
{
  m_pFieldStaffList->DebugSetup();
  m_pView->UpdateTree();
  if( m_pFieldStaffList->IsModuleFree() != false )
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
 * @date    2017.03.02
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void StaffListProc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  m_pRenderingManager->PreDraw();
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   Proc描画処理
 * @date    2017.03.02
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void StaffListProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pView->DrawTree( displayNo );
  m_pRenderingManager->Draw( displayNo );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateHeap
 * @brief   ヒープ生成
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListProc::CreateHeap(void)
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
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListProc::DeleteHeap(void)
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
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListProc::CreateRenderManager(void)
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
 * @date    2017.03.02
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListProc::DeleteRenderManager(void)
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
 * @func    CreateView
 * @brief   親ビュー生成
 * @date    2017.03.07
 */
//-----------------------------------------------------------------------------
void StaffListProc::CreateView(void)
{
  m_pView = GFL_NEW(m_pManagerHeap) DummyView( &m_heap, m_heap.GetSystemAllocator(), m_heap.GetDeviceAllocator() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteView
 * @brief   親ビュー削除
 * @date    2017.03.07
 */
//-----------------------------------------------------------------------------
void StaffListProc::DeleteView(void)
{
  GFL_SAFE_DELETE( m_pView );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateFieldStaffList
 * @brief   フィールドスタッフリスト生成
 * @date    2017.03.07
 */
//-----------------------------------------------------------------------------
void StaffListProc::CreateFieldStaffList(void)
{
  m_pFieldStaffList = GFL_NEW(m_pManagerHeap) FieldStaffList( m_pView, m_pRenderingManager );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFieldStaffList
 * @brief   フィールドスタッフリスト削除
 * @date    2017.03.07
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListProc::DeleteFieldStaffList(void)
{
  if( m_pFieldStaffList != NULL )
  {
    if( m_pFieldStaffList->IsModuleFree() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pFieldStaffList );
  }
  return true;
}


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

#endif  // PM_DEBUG

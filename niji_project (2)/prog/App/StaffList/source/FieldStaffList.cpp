//=============================================================================
/**
 * @file    FieldStaffList.cpp
 * @brief   momiji追加分のスタッフロール: フィールド呼び出し処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.06
 */
//=============================================================================

// module
#include "App/StaffList/include/FieldStaffList.h"
#include "FieldStaffListView.h"
// momiji
#include "System/include/HeapDefine.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
FieldStaffList::FieldStaffList( Field::Fieldmap * fieldmap )
  : m_pManagerHeap( NULL )
  , m_pView( NULL )
{
  CreateHeap();
  CreateView( fieldmap );
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
FieldStaffList::~FieldStaffList()
{
  DeleteView();
  DeleteHeap();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateHeap
 * @brief   ヒープ生成
 * @date    2017.03.06
 */
//-----------------------------------------------------------------------------
void FieldStaffList::CreateHeap(void)
{
//  gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD );
  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate( dev_heap, dev_heap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteHeap
 * @brief   ヒープ削除
 * @date    2017.03.06
 */
//-----------------------------------------------------------------------------
void FieldStaffList::DeleteHeap(void)
{
  // ローカルヒープ削除
  m_heap.Delete();
  // マネージャヒープ削除
  GFL_DELETE_HEAP( m_pManagerHeap );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateView
 * @brief   ビュー生成
 * @date    2017.03.06
 *
 * @param   fieldmap  フィールドマップ
 */
//-----------------------------------------------------------------------------
void FieldStaffList::CreateView( Field::Fieldmap * fieldmap )
{
  Field::FieldWindow::FieldWindowManager * man = fieldmap->GetFieldWindowManager();
  m_pView = GFL_NEW(m_pManagerHeap) FieldStaffListView( &m_heap, m_pManagerHeap, man->GetRenderingManager() );
  man->GetUIView()->AddSubView( m_pView );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteView
 * @brief   ビュー削除
 * @date    2017.03.06
 */
//-----------------------------------------------------------------------------
void FieldStaffList::DeleteView(void)
{
  if( IsModuleFree() == false )
  {
    GFL_ASSERT_STOP( 0 );
  }
  m_pView->RemoveFromSuperView();
  GFL_SAFE_DELETE( m_pView );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsModuleFree
 * @brief   クラスを破棄できるか
 * @date    2017.03.06
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool FieldStaffList::IsModuleFree(void)
{
  if( m_pView == NULL )
  {
    GFL_ASSERT( 0 );
    return true;
  }
  return m_pView->IsModuleFree();
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCreated
 * @brief   生成が完了したか
 * @date    2017.03.06
 *
 * @retval  true  = 完了した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldStaffList::IsCreated(void)
{
  if( m_pView == NULL )
  {
    GFL_ASSERT( 0 );
    return true;
  }
  return m_pView->IsCreated();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequest
 * @brief   動作リスクエスト
 * @date    2017.03.06
 *
 * @param   no    データ番号
 * @param   mode  背景配置モード
 */
//-----------------------------------------------------------------------------
void FieldStaffList::SetRequest( u32 no, BgPutMode mode )
{
  if( m_pView == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_pView->SetRequest( no, mode );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsAllRequestEnd
 * @brief   全てのリクエストが終了したか
 * @date    2017.03.17
 *
 * @retval  true  = 終了した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldStaffList::IsAllRequestEnd(void)
{
  if( m_pView == NULL )
  {
    GFL_ASSERT( 0 );
    return true;
  }
  return m_pView->IsAllRequestEnd();
}


#if PM_DEBUG
FieldStaffList::FieldStaffList( app::ui::UIView * view, app::util::AppRenderingManager * man )
  : m_pManagerHeap( NULL )
  , m_pView( NULL )
{
  DebugCreateHeap();
  DebugCreateView( view, man );
}

void FieldStaffList::DebugCreateHeap(void)
{
  gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate( dev_heap, dev_heap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
}

void FieldStaffList::DebugCreateView( app::ui::UIView * view, app::util::AppRenderingManager * man )
{
  m_pView = GFL_NEW(m_pManagerHeap) FieldStaffListView( &m_heap, m_pManagerHeap, man );
  view->AddSubView( m_pView );
}

void FieldStaffList::DebugSetup(void)
{
  if( m_pView != NULL )
  {
    m_pView->DebugSetup();
  }
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

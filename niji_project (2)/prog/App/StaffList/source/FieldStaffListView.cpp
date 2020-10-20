//=============================================================================
/**
 * @file    FieldStaffListView.cpp
 * @brief   momiji追加分のスタッフロール: フィールド用ビュー
 * @author  Hiroyuki Nakamura
 * @date    2017.03.06
 */
//=============================================================================

// module
#include "FieldStaffListView.h"
#include "StaffListFrame.h"
// momiji
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param heap                ヒープ
 * @param manager_heap        マネージャ用ヒープ
 * @param rendering_manager   描画マネージャ
 */
//-----------------------------------------------------------------------------
FieldStaffListView::FieldStaffListView( app::util::Heap * heap, gfl2::heap::HeapBase * manager_heap, app::util::AppRenderingManager * rendering_manager )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_managerHeap( manager_heap )
  , m_heap( heap )
  , m_renderingManager( rendering_manager )
  , m_pFrameManager( NULL )
  , m_pFrame( NULL )
  , m_mainSeq( MAINSEQ_INIT )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
FieldStaffListView::~FieldStaffListView()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2017.03.06
 */
//-----------------------------------------------------------------------------
void FieldStaffListView::Update(void)
{
  app::ui::UIView::Update();
  UpdateView();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2017.03.06
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void FieldStaffListView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_pFrameManager != NULL )
  {
    m_pFrameManager->Draw( displayNo );
  }
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
bool FieldStaffListView::IsModuleFree(void)
{
  if( m_mainSeq == MAINSEQ_FINISH )
  {
    return true;
  }
  return false;
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
bool FieldStaffListView::IsCreated(void)
{
  if( m_mainSeq == MAINSEQ_MAIN &&
      ( m_pFrame != NULL && m_pFrame->IsCreated() != false ) )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateView
 * @brief   ビュー更新
 * @date    2017.03.06
 */
//-----------------------------------------------------------------------------
void FieldStaffListView::UpdateView(void)
{
  switch( m_mainSeq )
  {
  case MAINSEQ_INIT:
    if( InitFunc() == false )
    {
      m_mainSeq = MAINSEQ_MAIN;
    }
    break;

  case MAINSEQ_MAIN:
    if( UpdateFunc() == false )
    {
      m_mainSeq = MAINSEQ_END;
    }
    break;

  case MAINSEQ_END:
    if( EndFunc() == false )
    {
      m_mainSeq = MAINSEQ_FINISH;
    }
    break;

  case MAINSEQ_FINISH:
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   初期化処理
 * @date    2017.03.06
 *
 * @retval  true  = 処理中
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool FieldStaffListView::InitFunc(void)
{
  CreateFrame();
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   終了処理
 * @date    2017.03.06
 *
 * @retval  true  = 処理中
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool FieldStaffListView::EndFunc(void)
{
  if( DeleteFrame() == false )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   アップデート処理
 * @date    2017.03.06
 *
 * @retval  true  = 処理中
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool FieldStaffListView::UpdateFunc(void)
{
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return false;
  }
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateFrame
 * @brief   フレーム生成
 * @date    2017.03.06
 */
//-----------------------------------------------------------------------------
void FieldStaffListView::CreateFrame(void)
{
  m_pFrameManager = GFL_NEW(m_managerHeap) applib::frame::Manager( m_managerHeap );
  m_pFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, StaffListFrame >( m_pFrameManager );
  m_pFrame->Setup( m_heap, m_renderingManager );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFrame
 * @brief   フレーム削除
 * @date    2017.03.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldStaffListView::DeleteFrame(void)
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
void FieldStaffListView::SetRequest( u32 no, BgPutMode mode )
{
  if( m_pFrame == NULL || IsCreated() == false )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_pFrame->SetRequest( no, mode );
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
bool FieldStaffListView::IsAllRequestEnd(void)
{
  if( m_pFrame == NULL || IsCreated() == false )
  {
    GFL_ASSERT( 0 );
    return true;
  }
  return m_pFrame->IsAllRequestEnd();
}



#if PM_DEBUG
void FieldStaffListView::DebugSetup(void)
{
  if( m_pFrame != NULL )
  {
    m_pFrame->DebugSetup();
  }
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

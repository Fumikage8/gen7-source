#if PM_DEBUG

// module
#include "HScrollFrame.h"
#include "HScrollDraw.h"
// niji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
// resource
#include "arc_def_index/arc_def.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

//! メインシーケンス
enum MainSeq
{
  SEQ_MAIN = 0,       //!< メイン
  SEQ_END
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
HScrollFrame::HScrollFrame(void)
  : m_heap( NULL )
  , m_renderingManager( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
HScrollFrame::~HScrollFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.05.26
 *
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void HScrollFrame::Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_heap             = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result HScrollFrame::InitFunc(void)
{
  LoadLayoutResource();
  CreateMenuCursor();
  CreateItemIcon();
  CreateDraw();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result HScrollFrame::EndFunc(void)
{
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteItemIcon() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  DeleteMenuCursor();
  UnloadLayoutResource();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result HScrollFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_MAIN:
    break;
  case SEQ_END:
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.05.26
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void HScrollFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void HScrollFrame::CreateDraw(void)
{
  m_pDraw = GFL_NEW(m_heap->GetDeviceHeap()) HScrollDraw( m_heap, m_renderingManager );
  m_pDraw->SetDrawListener( this );
  m_pDraw->Init( m_pLytResBuff );
  m_pDraw->InitSmallList( m_pItemIcon, m_pMenuCursor[0] );
  m_pDraw->InitMiddleList( m_pItemIcon, m_pMenuCursor[1] );
  m_pDraw->InitLargeList( m_pItemIcon, m_pMenuCursor[2] );
  m_pDraw->ChangeInput();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.05.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool HScrollFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->ExitLargeList() == false )
    {
      return false;
    }
    if( m_pDraw->ExitMiddleList() == false )
    {
      return false;
    }
    if( m_pDraw->ExitSmallList() == false )
    {
      return false;
    }
    if( m_pDraw->End() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void HScrollFrame::LoadLayoutResource(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_TEST_NAKAHIRO, m_heap->GetDeviceHeap() );
  app::util::FileAccessor::FileLoadSync(
    ARCID_TEST_NAKAHIRO, 0, &m_pLytResBuff, m_heap->GetDeviceHeap(), true, 128 );
  app::util::FileAccessor::FileCloseSync( ARCID_TEST_NAKAHIRO );
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void HScrollFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

void HScrollFrame::CreateItemIcon(void)
{
  m_pItemIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::ItemIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), ITEM_ICON_MAX );
  for( u32 i=0; i<ITEM_ICON_MAX; i++ )
  {
    m_pItemIcon->AllocTextureBuffer( i );
  }
  m_pItemIcon->FileOpenSync( m_heap->GetDeviceHeap() );
}

bool HScrollFrame::DeleteItemIcon(void)
{
  if( m_pItemIcon == NULL )
  {
    return true;
  }
  if( m_pItemIcon->IsModuleFree() == false )
  {
    return false;
  }
  m_pItemIcon->FileCloseSync();
  GFL_SAFE_DELETE( m_pItemIcon );
  return true;
}

void HScrollFrame::CreateMenuCursor(void)
{
  m_pMenuCursor[0] = GFL_NEW(m_heap->GetSystemHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorResBuff );
  m_pMenuCursor[1] = GFL_NEW(m_heap->GetSystemHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorResBuff );
  m_pMenuCursor[2] = GFL_NEW(m_heap->GetSystemHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorResBuff );
  m_pMenuCursor[0]->LoadResourceSync();
}

void HScrollFrame::DeleteMenuCursor(void)
{
  m_pMenuCursor[0]->DeleteResource();
  GFL_SAFE_DELETE( m_pMenuCursor[2] );
  GFL_SAFE_DELETE( m_pMenuCursor[1] );
  GFL_SAFE_DELETE( m_pMenuCursor[0] );
}



void HScrollFrame::OnAction( HScrollDraw::HScrollListener::ActionID id )
{
  if( id == HScrollDraw::HScrollListener::ACTION_ID_END )
  {
    SetSubSeq( SEQ_END );
  }
}


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

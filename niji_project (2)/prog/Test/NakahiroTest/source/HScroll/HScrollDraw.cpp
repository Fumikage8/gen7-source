#if PM_DEBUG

// module
//#include "ItemMakeListener.h"
#include "HScrollDraw.h"
#include "ListSmall.h"
#include "ListMiddle.h"
#include "ListLarge.h"
// niji
#include "Sound/include/Sound.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
// resource
#include "niji_conv_header/app/test/nakahiro_test/h_scroll/HScroll.h"
#include "niji_conv_header/app/test/nakahiro_test/h_scroll/HScroll_pane.h"
#include "niji_conv_header/app/test/nakahiro_test/h_scroll/HScroll_anim_list.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 */
//-----------------------------------------------------------------------------
HScrollDraw::HScrollDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_listener( NULL )
  , m_inputMode( 2 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
HScrollDraw::~HScrollDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void HScrollDraw::Update(void)
{
  app::ui::UIView::Update();

  m_pListSmall->UpdateItemIcon();
  m_pListSmall->GetMenuCursor()->Update();

  m_pListMiddle->UpdateItemIcon();
  m_pListMiddle->GetMenuCursor()->Update();

  m_pListLarge->UpdateItemIcon();
  m_pListLarge->GetMenuCursor()->Update();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void HScrollDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0 );

  m_pListSmall->GetMenuCursor()->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
  m_pListMiddle->GetMenuCursor()->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
  m_pListLarge->GetMenuCursor()->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetItemMakeListener
 * @brief   リスナーをセット
 * @date    2015.05.26
 *
 * @param   listener リスナー
 */
//-----------------------------------------------------------------------------
void HScrollDraw::SetDrawListener( HScrollListener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.05.26
 *
 * @param   res_buf レイアウトリソースバッファ
 */
//-----------------------------------------------------------------------------
void HScrollDraw::Init( void * res_buf )
{
  Initialize2D( res_buf );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.05.26
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool HScrollDraw::End(void)
{
  Terminate2D();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2015.05.26
 *
 * @param   res_buf   レイアウトリソースバッファ
 */
//-----------------------------------------------------------------------------
void HScrollDraw::Initialize2D( void * res_buf )
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { res_buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_HSCROLL_LOWER_BFLYT,
      LA_HSCROLL_LOWER___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  Create2D(
    m_heap,
    NULL,
    10,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    app::ui::UIView::UNUSE_MESSAGE_DATA );
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void HScrollDraw::Terminate2D(void)
{
  Delete2D();
}

void HScrollDraw::InitSmallList( app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor )
{
  cursor->CreateLayoutWork(
    GetG2DUtil()->GetLayoutSystem(),
    GetG2DUtil()->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    GetG2DUtil()->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  cursor->SetVisible( false );

  m_pListSmall = GFL_NEW(m_heap->GetSystemHeap()) ListSmall( m_heap, GetG2DUtil(), icon, cursor );

  AddSubView( m_pListSmall->GetNewPaneList() );
}

bool HScrollDraw::ExitSmallList(void)
{
  if( m_pListSmall != NULL )
  {
    app::tool::MenuCursor * cursor = m_pListSmall->GetMenuCursor();
    if( cursor->GetLayoutWork()->IsDrawing() != false )
    {
      return false;
    }

    m_pListSmall->GetNewPaneList()->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pListSmall );

    cursor->DeleteLayoutWork();
  }
  return true;
}

void HScrollDraw::InitMiddleList( app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor )
{
  cursor->CreateLayoutWork(
    GetG2DUtil()->GetLayoutSystem(),
    GetG2DUtil()->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    GetG2DUtil()->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  cursor->SetVisible( false );

  m_pListMiddle = GFL_NEW(m_heap->GetSystemHeap()) ListMiddle( m_heap, GetG2DUtil(), icon, cursor );

  AddSubView( m_pListMiddle->GetNewPaneList() );
}

bool HScrollDraw::ExitMiddleList(void)
{
  if( m_pListMiddle != NULL )
  {
    app::tool::MenuCursor * cursor = m_pListMiddle->GetMenuCursor();
    if( cursor->GetLayoutWork()->IsDrawing() != false )
    {
      return false;
    }

    m_pListMiddle->GetNewPaneList()->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pListMiddle );

    cursor->DeleteLayoutWork();
  }
  return true;
}

void HScrollDraw::InitLargeList( app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor )
{
  cursor->CreateLayoutWork(
    GetG2DUtil()->GetLayoutSystem(),
    GetG2DUtil()->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    GetG2DUtil()->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  cursor->SetVisible( false );

  m_pListLarge = GFL_NEW(m_heap->GetSystemHeap()) ListLarge( m_heap, GetG2DUtil(), icon, cursor );

  AddSubView( m_pListLarge->GetNewPaneList() );
}

bool HScrollDraw::ExitLargeList(void)
{
  if( m_pListLarge != NULL )
  {
    app::tool::MenuCursor * cursor = m_pListLarge->GetMenuCursor();
    if( cursor->GetLayoutWork()->IsDrawing() != false )
    {
      return false;
    }

    m_pListLarge->GetNewPaneList()->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pListLarge );

    cursor->DeleteLayoutWork();
  }
  return true;
}


void HScrollDraw::ChangeInput(void)
{
  m_inputMode++;
  if( m_inputMode >= 3 )
  {
    m_inputMode = 0;
  }
  switch( m_inputMode )
  {
  case 0:
    m_pListSmall->GetNewPaneList()->SetInputEnabled( true );

    m_pListMiddle->GetNewPaneList()->SetInputEnabled( false );
    m_pListLarge->GetNewPaneList()->SetInputEnabled( false );
    GFL_PRINT( "★Input Small\n" );
    break;

  case 1:
    m_pListMiddle->GetNewPaneList()->SetInputEnabled( true );

    m_pListSmall->GetNewPaneList()->SetInputEnabled( false );
    m_pListLarge->GetNewPaneList()->SetInputEnabled( false );
    GFL_PRINT( "★Input Middle\n" );
    break;

  case 2:
    m_pListLarge->GetNewPaneList()->SetInputEnabled( true );

    m_pListSmall->GetNewPaneList()->SetInputEnabled( false );
    m_pListMiddle->GetNewPaneList()->SetInputEnabled( false );
    GFL_PRINT( "★Input Large\n" );
    break;
  }
}

App::Tool::NewPaneList * HScrollDraw::GetActiveList(void)
{
  switch( m_inputMode )
  {
  case 0:
    return m_pListSmall->GetNewPaneList();
  case 1:
    return m_pListMiddle->GetNewPaneList();
  case 2:
    return m_pListLarge->GetNewPaneList();
  }
  return NULL;
}




//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.03.05
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult HScrollDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_pListSmall->GetNewPaneList()->IsModuleAction() != false ||
      m_pListMiddle->GetNewPaneList()->IsModuleAction() != false ||
      m_pListLarge->GetNewPaneList()->IsModuleAction() != false )
  {
    return ENABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_X) )
  {
    ChangeInput();
    return DISABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    App::Tool::NewPaneList * list = GetActiveList();
    if( list->IsItemChange() != false )
    {
      list->SetItemChange( App::Tool::NewPaneList::ITEM_CHANGE_DECIDE );
    }
    return DISABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    App::Tool::NewPaneList * list = GetActiveList();
    if( list->IsItemChange() == false )
    {
      m_listener->OnAction( HScrollListener::ACTION_ID_END );
    }
    else
    {
      list->SetItemChange( App::Tool::NewPaneList::ITEM_CHANGE_CANCEL );
    }
    return DISABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_START) )
  {
    App::Tool::NewPaneList * list = GetActiveList();
    if( list->IsItemChange() == false )
    {
      list->SetItemChange( App::Tool::NewPaneList::ITEM_CHANGE_START );
    }
    else
    {
      list->SetItemChange( App::Tool::NewPaneList::ITEM_CHANGE_DECIDE );
    }
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}



GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)

#endif  // PM_DEBUG

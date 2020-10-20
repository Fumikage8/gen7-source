#if PM_DEBUG
//=============================================================================
/**
 * @file    ItemMakeDraw.cpp
 * @brief   デバッグ用アイテム作成描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.05.26
 */
//=============================================================================

// module
#include "ItemMakeListener.h"
#include "ItemMakeDraw.h"
// niji
#include "Savedata/include/MyItemSaveDef.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_index/debug_message.gaix"
#include "niji_conv_header/message/debug/msg_debug_item_make.h"
#include "niji_conv_header/app/debug/item_make/DebugItemMake.h"
#include "niji_conv_header/app/debug/item_make/DebugItemMake_pane.h"
#include "niji_conv_header/app/debug/item_make/DebugItemMake_anim_list.h"


GFL_NAMESPACE_BEGIN(Debug)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 */
//-----------------------------------------------------------------------------
ItemMakeDraw::ItemMakeDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_itemMakeListener( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ItemMakeDraw::~ItemMakeDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::Update(void)
{
  app::ui::UIView::Update();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0 );
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
void ItemMakeDraw::SetItemMakeListener( ItemMakeListener * listener )
{
  m_itemMakeListener = listener;
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
void ItemMakeDraw::Init( void * res_buf )
{
  Initialize2D( res_buf );
  CreateButton();
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
bool ItemMakeDraw::End(void)
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
void ItemMakeDraw::Initialize2D( void * res_buf )
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
      LYTRES_DEBUGITEMMAKE_ITEM_MAKE_LOWER_BFLYT,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_debug_item_make_DAT, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    10,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::Terminate2D(void)
{
  Delete2D();

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン作成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::CreateButton(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetDeviceHeap()) app::ui::ButtonInfoEx[BUTTON_ID_MAX];

  CreateNormalButton( &info[BUTTON_ID_NAME] );
  CreateExitButton( &info[BUTTON_ID_EXIT] );

  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, BUTTON_ID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonBindKey( BUTTON_ID_EXIT, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonSelectSE( BUTTON_ID_NAME, SEQ_SE_DECIDE1 );
  man->SetButtonSelectSE( BUTTON_ID_NUM, SEQ_SE_DECIDE1 );
  man->SetButtonSelectSE( BUTTON_ID_ADD_ONE, SEQ_SE_COMPLETE1 );
  man->SetButtonSelectSE( BUTTON_ID_ADD_MAX, SEQ_SE_COMPLETE1 );
  man->SetButtonSelectSE( BUTTON_ID_ENTER, SEQ_SE_COMPLETE1 );
  man->SetButtonSelectSE( BUTTON_ID_EXIT, SEQ_SE_CANCEL1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetButtonInfoEx
 * @brief   ボタン情報セット
 * @date    2015.05.26
 *
 * @param   info      情報格納場所
 * @param   parts_tbl 部品ペインテーブル
 * @param   info_tbl  デフォルト情報テーブル
 * @parma   bound     境界ペインテーブル
 * @parma   max       セット数
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, const gfl2::lyt::LytPaneIndex * bound, u32 max )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<max; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound[i], &res_id );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateNormalButton
 * @brief   通常ボタンの情報をセット
 * @date    2015.05.26
 *
 * @param   info      情報格納場所
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::CreateNormalButton( app::ui::ButtonInfoEx * info )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_00,
    PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_01,
    PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_03,
    PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_00,
    PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_01,
    PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_02,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_ITEMMAKE_BTN_PANE_BOUND_00,
    PANENAME_ITEMMAKE_BTN_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
  };

  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_ADD_ONE, NULL, NULL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_00_TOUCH,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_00_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_00_CANSEL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_00_TOUCH_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_00_ACTIVE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_00_PASSIVE,
    },
    {
      BUTTON_ID_ADD_MAX, NULL, NULL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_01_TOUCH,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_01_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_01_CANSEL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_01_TOUCH_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_01_ACTIVE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_01_PASSIVE,
    },
    {
      BUTTON_ID_ITEM_ID, NULL, NULL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_03_TOUCH,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_03_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_03_CANSEL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_03_TOUCH_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_03_ACTIVE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_03_PASSIVE,
    },
    {
      BUTTON_ID_NAME, NULL, NULL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_00_TOUCH,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_00_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_00_CANSEL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_00_TOUCH_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_00_ACTIVE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_00_PASSIVE,
    },
    {
      BUTTON_ID_NUM, NULL, NULL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_01_TOUCH,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_01_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_01_CANSEL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_01_TOUCH_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_01_ACTIVE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_01_PASSIVE,
    },
    {
      BUTTON_ID_ENTER, NULL, NULL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_02_TOUCH,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_02_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_02_CANSEL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_02_TOUCH_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_02_ACTIVE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_02_02_PASSIVE,
    },
  };

  SetButtonInfoEx( info, parts_tbl, info_tbl, bound_tbl, GFL_NELEMS(parts_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateExitButton
 * @brief   戻るボタンの情報をセット
 * @date    2015.05.26
 *
 * @param   info      情報格納場所
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::CreateExitButton( app::ui::ButtonInfoEx * info )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_EXIT,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };

  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_EXIT, NULL, NULL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_EXIT_TOUCH,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_EXIT_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_EXIT_CANSEL,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_EXIT_TOUCH_RELEASE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_EXIT_ACTIVE,
      LA_DEBUGITEMMAKE_ITEM_MAKE_LOWER__PARTS_EXIT_PASSIVE,
    },
  };

  SetButtonInfoEx( info, parts_tbl, info_tbl, bound_tbl, GFL_NELEMS(parts_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutString
 * @brief   デフォルト文字列書き込み
 * @date    2015.05.26
 *
 * @param   item  道具ID
 * @param   num   個数
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::PutString( u32 item, u32 num )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  { // ボタン１（１個）
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_00 );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &res_id );
    g2d->SetRegisterNumber( 0, 1, 3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
    g2d->SetTextBoxPaneStringExpand( text, msg_d_itemmake_button_00 );
  }
  { // ボタン２（９９９個）
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_01 );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &res_id );
    g2d->SetRegisterNumber( 0, ITEM_HAVE_MAX, 3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
    g2d->SetTextBoxPaneStringExpand( text, msg_d_itemmake_button_00 );
  }
  { //「実行」
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_02 );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &res_id );
    g2d->SetTextBoxPaneString( text, msg_d_itemmake_button_03 );
  }
  // 道具
  PutItem( item );
  // ボタン４（個数）
  PutNumber( num );
  //「名前と個数を指定」
  g2d->SetTextBoxPaneString( 0, PANENAME_ITEM_MAKE_LOWER_PANE_TEXT_02, msg_d_itemmake_item_00 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutItem
 * @brief   道具書き込み
 * @date    2016.04.11
 *
 * @param   id  道具ID
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::PutItem( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  { // 道具ID
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_03 );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_ITEMMAKE_BTN_PANE_TEXTBOX_00, &res_id );
    g2d->SetRegisterNumber( 0, id, 4, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
    g2d->SetTextBoxPaneStringExpand( text, msg_d_itemmake_item_01 );
  }
  { // 道具名
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_00 );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_ITEMMAKE_BTN_PANE_TEXTBOX_00, &res_id );
    g2d->SetRegisterItemName( 0, id );
    g2d->SetTextBoxPaneStringExpand( text, msg_d_itemmake_button_01 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutNumber
 * @brief   個数書き込み
 * @date    2015.05.27
 *
 * @param   num   個数
 */
//-----------------------------------------------------------------------------
void ItemMakeDraw::PutNumber( u32 num )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_ITEM_MAKE_LOWER_PANE_PARTS_02_01 );
  gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &res_id );
  g2d->SetRegisterNumber( 0, num, 3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  g2d->SetTextBoxPaneStringExpand( text, msg_d_itemmake_button_02 );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetNumInputTitle
 * @brief   数値入力のタイトル取得
 * @date    2015.05.27
 *
 * @return  タイトル文字列
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ItemMakeDraw::GetNumInputTitle(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * buf = g2d->GetTempStrBuf( 0 );
  g2d->GetString( buf, msg_d_itemmake_title_00 );
  return buf;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemIDInputTitle
 * @brief   道具ID入力のタイトル取得
 * @date    2016.04.11
 *
 * @return  タイトル文字列
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ItemMakeDraw::GetItemIDInputTitle(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * buf = g2d->GetTempStrBuf( 0 );
  g2d->GetString( buf, msg_d_itemmake_title_01 );
  return buf;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetTempStrBuf
 * @brief   汎用文字列バッファ取得
 * @date    2015.05.27
 *
 * @return  汎用文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ItemMakeDraw::GetTempStrBuf(void)
{
  return GetG2DUtil()->GetTempStrBuf( 0 );
}


//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.05.26
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult ItemMakeDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( button_id == BUTTON_ID_NAME )
  {
    m_itemMakeListener->OnAction( ItemMakeListener::ACTION_INPUT_NAME );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_ITEM_ID )
  {
    m_itemMakeListener->OnAction( ItemMakeListener::ACTION_INPUT_ITEM_ID );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_NUM )
  {
    m_itemMakeListener->OnAction( ItemMakeListener::ACTION_INPUT_NUM );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_ENTER )
  {
    m_itemMakeListener->OnAction( ItemMakeListener::ACTION_INPUT_ENTER );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_ADD_ONE )
  {
    m_itemMakeListener->OnAction( ItemMakeListener::ACTION_ADD_ITEM_ONE );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_ADD_MAX )
  {
    m_itemMakeListener->OnAction( ItemMakeListener::ACTION_ADD_ITEM_MAX );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_EXIT )
  {
    m_itemMakeListener->OnAction( ItemMakeListener::ACTION_EXIT );
    return DISABLE_ACCESS;
  }

  return DISABLE_ACCESS;
}


GFL_NAMESPACE_END(Debug)

#endif  // PM_DEBUG

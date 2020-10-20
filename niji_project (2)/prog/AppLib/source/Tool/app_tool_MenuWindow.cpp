//=============================================================================
/**
 * @file    app_tool_MenuWindow.cpp
 * @brief   汎用メニューウィンドウ
 * @author  Hiroyuki Nakamura
 * @date    2015.03.10
 */
//=============================================================================

// module
#include "AppLib/include/Tool/app_tool_MenuWindow.h"
#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"
// niji
#include "Print/include/PrintSystem.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
// arc
#include "arc_def_index/arc_def.h"
#include "arc_index/MenuWindow.gaix"
// resource
#include "niji_conv_header/app/common_menu/MenuWindow.h"
#include "niji_conv_header/app/common_menu/MenuWindow_pane.h"
#include "niji_conv_header/app/common_menu/MenuWindow_anim_list.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

const f32 MenuWindow::DEFAULT_PUT_FX = 160.0f;  //!< デフォルト表示X座標
const f32 MenuWindow::DEFAULT_PUT_FY = -120.0f; //!< デフォルト表示Y座標
const s32 MenuWindow::DEFAULT_PUT_X  = MenuWindow::DEFAULT_PUT_FX + DISPLAY_LOWER_WIDTH/2;  //!< デフォルト表示X座標（左上(0,0)の画面座標）
const s32 MenuWindow::DEFAULT_PUT_Y  = DISPLAY_LOWER_HEIGHT/2 - MenuWindow::DEFAULT_PUT_FY; //!< デフォルト表示Y座標（左上(0,0)の画面座標）

const f32 MenuWindow::YESNO_DEFAULT_PUT_FX = 156.0f;  //!< "はい/いいえ"用デフォルト表示X座標
const f32 MenuWindow::YESNO_DEFAULT_PUT_FY = -32;     //!< "はい/いいえ"用デフォルト表示Y座標

const f32 MenuWindow::DEFAULT_PUT_FX_UPPER = 202.0f;  //!< 上画面用デフォルト表示X座標
const f32 MenuWindow::DEFAULT_PUT_FY_UPPER = -60.0f;  //!< 上画面用デフォルト表示Y座標
const s32 MenuWindow::DEFAULT_PUT_X_UPPER = MenuWindow::DEFAULT_PUT_FX_UPPER + DISPLAY_UPPER_WIDTH/2;   //!< 上画面用デフォルト表示X座標（左上(0,0)の画面座標）
const s32 MenuWindow::DEFAULT_PUT_Y_UPPER = DISPLAY_UPPER_HEIGHT/2 - MenuWindow::DEFAULT_PUT_FY_UPPER;  //!< 上画面用デフォルト表示Y座標（左上(0,0)の画面座標）


//! 可変サイズペインテーブル
const gfl2::lyt::LytPaneIndex MenuWindow::SizeCalcPane[] =
{
  PANENAME_COMMON_BTN_LOW_008_PANE_BG_THICK,
  PANENAME_COMMON_BTN_LOW_008_PANE_BG_MAIN,
  PANENAME_COMMON_BTN_LOW_008_PANE_BOUND_00,
  PANENAME_COMMON_BTN_LOW_008_PANE_NULL_CUR,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap              ヒープ
 * @param   rendering_manager 描画マネージャ
 * @param   disp              表示画面
 * @param   cur_buff          カーソルのリソースバッファ
 */
//-----------------------------------------------------------------------------
MenuWindow::MenuWindow( app::util::Heap * heap, app::util::AppRenderingManager * rendering_manager, void * cur_buff, app::util::G2DUtil::SETUP_DISPLAY_TYPE disp )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_disp( disp )
  , m_heap( heap )
  , m_renderingManager( rendering_manager )
  , m_pLytResBuff( NULL )
  , m_width( 0.0f )
  , m_defTextWidth( 0.0f )
  , m_markWidth( 0.0f )
  , m_isCreate( false )
  , m_isCreateSelf( true )
  , m_isEnable( false )
  , m_lytwk( NULL )
  , m_pCursorResBuff( cur_buff )
  , m_listener( NULL )
  , m_max( 0 )
  , m_cursorPos( 0 )
  , m_mainSeq( SEQ_START )
  , m_subSeq( 0 )
#if PM_DEBUG
  , m_debugYesNoInvalid( false )
#endif  // PM_DEBUG
{
  CreateItemString();

  m_pMenuCursor = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuCursor( m_heap, &m_pCursorResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap              ヒープ
 * @param   rendering_manager 描画マネージャ
 * @param   cur_buff          カーソルのリソースバッファ
 * @param   disp              表示画面
 * @param   menu_window_buff  メニューウィンドウのレイアウトリソースバッファ
 */
//-----------------------------------------------------------------------------
MenuWindow::MenuWindow( app::util::Heap * heap, app::util::AppRenderingManager * rendering_manager, void * cur_buff, app::util::G2DUtil::SETUP_DISPLAY_TYPE disp, void * menu_window_buff )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_disp( disp )
  , m_heap( heap )
  , m_renderingManager( rendering_manager )
  , m_pLytResBuff( menu_window_buff )
  , m_width( 0.0f )
  , m_defTextWidth( 0.0f )
  , m_markWidth( 0.0f )
  , m_isCreate( true )
  , m_isCreateSelf( false )
  , m_isEnable( false )
  , m_lytwk( NULL )
  , m_pCursorResBuff( cur_buff )
  , m_listener( NULL )
  , m_max( 0 )
  , m_cursorPos( 0 )
  , m_mainSeq( SEQ_MAIN )
  , m_subSeq( 0 )
{
  GFL_ASSERT_STOP_MSG( menu_window_buff, "Error !! : Menu window resource buffer NULL.\n" );
  GFL_ASSERT_STOP_MSG( m_pCursorResBuff, "Error !! : Menu cursor resource buffer NULL.\n" );
  CreateItemString();

  m_pMenuCursor = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuCursor( m_heap, &m_pCursorResBuff );

  CreateLaytout();
  CreateMenuCursor();
  CreateButtonManager();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MenuWindow::~MenuWindow()
{
  Delete();
  DeleteItemString();

  GFL_DELETE m_pMenuCursor;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::Update(void)
{
  app::ui::UIView::Update();

  switch( m_mainSeq )
  {
  case SEQ_START:
    if( IsCreate() != false )
    {
      m_mainSeq = SEQ_MAIN;
      break;
    }
    m_mainSeq = SEQ_CREATE;
    /* FALL THROUGH */
  case SEQ_CREATE:
    if( Create() != false )
    {
      m_mainSeq = SEQ_MAIN;
    }
    break;
  case SEQ_MAIN:
    m_pMenuCursor->Update();
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画処理
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isEnable == false )
  {
    return;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  if( g2d != NULL )
  {
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0 );
  }
  m_pMenuCursor->Draw( m_renderingManager, displayNo, GetDispType() );
}


//-----------------------------------------------------------------------------
/**
 * @func    Create
 * @brief   レイアウトとボタン生成
 * @date    2015.03.12
 *
 * @retval  false = 生成中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuWindow::Create(void)
{
  enum
  {
    LOCAL_FILE_OPEN = 0,  // ファイルオープン
    LOCAL_FILE_LOAD,      // ファイルロード
    LOCAL_FILE_CLOSE,     // ファイルクローズ
    LOCAL_FILE_FINISH,    // ファイル操作終了

    LOCAL_LOAD_CURSOR,    // カーソル読み込み
  };

  if( IsCreate() != false )
  {
    return true;
  }

  switch( m_subSeq )
  {
  case LOCAL_FILE_OPEN:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_MENU_WINDOW, m_heap->GetSystemHeap() );
    m_subSeq = LOCAL_FILE_LOAD;
    /* FALL THROUGH */
  case LOCAL_FILE_LOAD:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_MENU_WINDOW ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_MENU_WINDOW,
      GARC_MenuWindow_MenuWindow_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq = LOCAL_FILE_CLOSE;
    /* FALL THROUGH */
  case LOCAL_FILE_CLOSE:  // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_MENU_WINDOW, m_heap->GetSystemHeap() );
    m_subSeq = LOCAL_FILE_FINISH;
    /* FALL THROUGH */
  case LOCAL_FILE_FINISH: // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_MENU_WINDOW ) == false )
    {
      break;
    }
    CreateLaytout();

    if( m_pCursorResBuff != NULL )
    {
      CreateMenuCursor();
      CreateButtonManager();
      m_isCreate = true;
      m_subSeq = 0;
      return true;
    }
    m_subSeq = LOCAL_LOAD_CURSOR;
    /* FALL THROUGH */
  case LOCAL_LOAD_CURSOR: // カーソル読み込み
    if( m_pMenuCursor->LoadResource() == false )
    {
      break;
    }
    CreateMenuCursor();
    CreateButtonManager();
    m_isCreate = true;
    m_subSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCreate
 * @brief   レイアウトとボタンの生成が完了したか
 * @date    2015.03.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuWindow::IsCreate(void)
{
  return m_isCreate;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsModuleFree
 * @brief   メニューウィンドウを破棄できるか
 * @date    2015.03.12
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool MenuWindow::IsModuleFree(void)
{
  if( ( m_mainSeq == SEQ_CREATE && IsCreate() == false ) || m_pMenuCursor->IsModuleFree() == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Delete
 * @brief   レイアウトとボタンの削除
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::Delete(void)
{
  DeleteMenuCursor();

  app::ui::UIResponder::ReleaseButtonManager();
  Delete2D();
  if( m_isCreateSelf )
  {
    GflHeapFreeMemory( m_pLytResBuff );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemString
 * @brief   項目文字列生成
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::CreateItemString(void)
{
  for( u32 i=0; i<ITEM_MAX; i++ )
  {
    m_item[i].str = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::StrBuf( ITEM_STR_LENGTH, m_heap->GetDeviceHeap() );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteItemString
 * @brief   項目文字列削除
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::DeleteItemString(void)
{
  for( u32 i=0; i<ITEM_MAX; i++ )
  {
    GFL_SAFE_DELETE( m_item[i].str );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLayout
 * @brief   レイアウト生成
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::CreateLaytout(void)
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { m_pLytResBuff, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_MENUWINDOW_MENU_PLT_LOW_000_BFLYT,
      LA_MENUWINDOW_MENU_PLT_LOW_000___NUM,
      m_pLytResBuff,
      false,
      m_disp,
      true
    },
  };

  Create2D(
    m_heap,
    NULL,
    GFL_NELEMS(data_tbl),
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    UNUSE_MESSAGE_DATA );

  InitData();

  // ペインを非表示
  for( u32 i=0; i<ITEM_MAX; i++ )
  {
    m_item[i].parts->SetVisible( false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButtonManager
 * @brief   ボタンマネージャ生成
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::CreateButtonManager(void)
{
  static const app::ui::ButtonInfoEx def_info[] =
  {
    {
      0, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_00_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_00_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_00_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_00_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_00_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_00_PASSIVE,
    },
    {
      1, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_01_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_01_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_01_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_01_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_01_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_01_PASSIVE,
    },
    {
      2, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_02_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_02_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_02_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_02_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_02_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_02_PASSIVE,
    },
    {
      3, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_03_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_03_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_03_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_03_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_03_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_03_PASSIVE,
    },
    {
      4, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_04_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_04_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_04_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_04_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_04_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_04_PASSIVE,
    },
    {
      5, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_05_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_05_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_05_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_05_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_05_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_05_PASSIVE,
    },
    {
      6, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_06_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_06_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_06_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_06_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_06_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_06_PASSIVE,
    },
    {
      7, NULL, NULL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_07_TOUCH,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_07_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_07_CANSEL,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_07_TOUCH_RELEASE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_07_ACTIVE,
      LA_MENUWINDOW_MENU_PLT_LOW_000__COMMAND_07_PASSIVE,
    },
  };

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetDeviceHeap()) app::ui::ButtonInfoEx[ITEM_MAX];

  for( u32 i=0; i<ITEM_MAX; i++ )
  {
    info[i] = def_info[i];
    info[i].picture_pane = m_item[i].parts;
    info[i].bound_pane   = GetPane( i, PANENAME_COMMON_BTN_LOW_008_PANE_BOUND_00 );
  }
  app::ui::UIResponder::CreateButtonManager( m_heap, m_lytwk, info, ITEM_MAX );

  GFL_DELETE_ARRAY info;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListener
 * @brief   リスナーをセット
 * @date    2015.03.12
 *
 * @param   セットするリスナー
 */
//-----------------------------------------------------------------------------
void MenuWindow::SetListener( MenuWindowListener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    AddItem
 * @brief   項目追加 ( MsgData版 )
 * @date    2015.03.12
 *
 * @param   msg           メッセージデータ
 * @param   id            文字列ID
 * @param   is_cancel     true = Bボタンを割り当てる
 * @param   is_mark       true = 戻るマークを表示する
 * @param   select_se_id  選択時のSEID
 */
//-----------------------------------------------------------------------------
void MenuWindow::AddItem( gfl2::str::MsgData * msg, u32 id, bool is_cancel, bool is_mark, u32 select_se_id )
{
  if( m_max == ITEM_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }
  msg->GetString( id, *m_item[m_max].str );
  m_item[m_max].is_b_button   = is_cancel;
  m_item[m_max].is_mark       = is_mark;
  m_item[m_max].select_se_id  = select_se_id;
#if PM_DEBUG
  // テキストボックスペインの文字列長が30に設定されているのでチェック 2016/03/16 by nakahiro
  GFL_ASSERT( m_item[m_max].str->GetLength() <= 30 );
#endif  // PM_DEBUG
  m_max++;
}

//-----------------------------------------------------------------------------
/**
 * @func    AddItem
 * @brief   項目追加 ( StrBuf版 )
 * @date    2015.03.12
 *
 * @param   str           文字列
 * @param   is_cancel     true = Bボタンを割り当てる
 * @param   is_mark       true = 戻るマークを表示する
 * @param   select_se_id  選択時のSEID
 */
//-----------------------------------------------------------------------------
void MenuWindow::AddItem( const gfl2::str::StrBuf & str, bool is_cancel, bool is_mark, u32 select_se_id )
{
  if( m_max == ITEM_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_item[m_max].str->SetStr( str );
  m_item[m_max].is_b_button   = is_cancel;
  m_item[m_max].is_mark       = is_mark;
  m_item[m_max].select_se_id  = select_se_id;
#if PM_DEBUG
  // テキストボックスペインの文字列長が30に設定されているのでチェック 2016/03/16 by nakahiro
  GFL_ASSERT( m_item[m_max].str->GetLength() <= 30 );
#endif  // PM_DEBUG
  m_max++;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartMenuDisplay
 * @brief   メニュー処理開始 ( 画面座標(左上0,0)指定版 )
 * @date    2015.03.12
 *
 * @param   cursor_pos  初期カーソル位置
 * @param   type        配置タイプ
 * @param   x           配置X座標
 * @param   y           配置Y座標
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool MenuWindow::StartMenuDisplay( u32 cursor_pos, PutType type, s32 x, s32 y )
{
  f32 px, py;

  if( m_disp == app::util::G2DUtil::SETUP_UPPER )
  {
    px = static_cast<f32>( x - DISPLAY_UPPER_WIDTH/2 );
    py = static_cast<f32>( DISPLAY_UPPER_HEIGHT/2 - y );
  }
  else
  {
    px = static_cast<f32>( x - DISPLAY_LOWER_WIDTH/2 );
    py = static_cast<f32>( DISPLAY_LOWER_HEIGHT/2 - y );
  }
  return StartMenu( cursor_pos, type, px, py );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartMenu
 * @brief   メニュー処理開始 ( 3D座標(中心0,0)指定版 )
 * @date    2015.03.12
 *
 * @param   cursor_pos  初期カーソル位置
 * @param   type        配置タイプ
 * @param   x           配置X座標
 * @param   y           配置Y座標
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool MenuWindow::StartMenu( u32 cursor_pos, PutType type, f32 x, f32 y )
{
  if( m_max == 0 )
  {
    GFL_ASSERT( 0 );
    return false;
  }

#if PM_DEBUG
  if( m_debugYesNoInvalid == false )
  { // "はい/いいえ"用座標チェック
    static const gfl2::str::STRCODE * str_yes = L"はい";
    static const gfl2::str::STRCODE * str_no  = L"いいえ";
    if( m_disp == app::util::G2DUtil::SETUP_LOWER &&
        m_max == 2 &&
        gfl2::str::StrComp( m_item[0].str->GetPtr(), str_yes ) != false &&
        gfl2::str::StrComp( m_item[1].str->GetPtr(), str_no ) != false )
    {
      if( type != PUT_RD || x != YESNO_DEFAULT_PUT_FX || y != YESNO_DEFAULT_PUT_FY )
      {
        GFL_ASSERT( 0 );
      }
    }
  }
#endif

  if( IsCreate() == false )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( m_isEnable != false )
  {
    return true;
  }

  CalcItemWidth();
  SetItemParam();

  nw::lyt::Size put_size = GetPane(0,PANENAME_COMMON_BTN_LOW_008_PANE_BG_MAIN)->GetSize();
  gfl2::math::VEC3 pos;
  GetPutPos( type, x, y, put_size.height, &pos );

  for( u32 i=0; i<m_max; i++ )
  {
    m_item[i].parts->SetTranslate( pos );
    m_item[i].parts->SetVisible( true );
    pos.y -= put_size.height;
  }

  m_isEnable = true;

  SetInputListener( this );

  PutCursor( cursor_pos );

  m_pMenuCursor->SetVisible( true );
  m_pMenuCursor->StartAnime();

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartYesNoMenu
 * @brief   "はい/いいえ"用メニュー処理開始 ( 3D座標(中心0,0)指定版 )
 * @date    2015.04.23
 *
 * @param   cursor_pos  初期カーソル位置
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool MenuWindow::StartYesNoMenu( u32 cursor_pos )
{
  return StartMenu( cursor_pos, PUT_RD, YESNO_DEFAULT_PUT_FX, YESNO_DEFAULT_PUT_FY );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndMenu
 * @brief   メニュー処理終了
 * @date    2015.03.12
 *
 * @param   clear   再度開始する場合にはfalseを指定しておく
 */
//-----------------------------------------------------------------------------
void MenuWindow::EndMenu( bool clear )
{
  if( m_isEnable == false )
  {
    return;
  }

  m_pMenuCursor->SetVisible( false );

  SetInputListener( NULL );

  m_isEnable = false;

  for( u32 i=0; i<m_max; i++ )
  {
    m_item[i].parts->SetVisible( false );
  }

  // 項目数の初期化
  if( clear )
  {
    m_max = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    RemoveSubView
 * @brief   親への登録を解除
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::RemoveSubView(void)
{
  RemoveFromSuperView();
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCursorPos
 * @brief   カーソルの位置を取得
 * @date    2015.04.07
 *
 * @return  カーソル位置
 */
//-----------------------------------------------------------------------------
u32 MenuWindow::GetCursorPos(void)
{
  return m_cursorPos;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPane
 * @brief   部品からペインを取得
 * @date    2015.03.12
 *
 * @param   id    項目ID
 * @param   pane  取得するペイン
 *
 * @return  指定されたペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPane * MenuWindow::GetPane( u32 id, gfl2::lyt::LytPaneIndex pane )
{
  return m_lytwk->GetPane( m_item[id].parts, pane, &m_resID);
}

//-----------------------------------------------------------------------------
/**
 * @func    GetTextBoxPane
 * @brief   部品から項目を書き込むテキストボックスペインを取得
 * @date    2015.03.12
 *
 * @param   id    項目ID
 *
 * @return  テクストボックスペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytTextBox * MenuWindow::GetTextBoxPane( u32 id  )
{
  return m_lytwk->GetTextBoxPane( m_item[id].parts, PANENAME_COMMON_BTN_LOW_008_PANE_TEXTBOX_00, &m_resID );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitData
 * @brief   データ初期化
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::InitData(void)
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_00,
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_01,
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_02,
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_03,
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_04,
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_05,
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_06,
    PANENAME_MENU_PLT_LOW_000_PANE_COMMAND_07,
  };

  // レイアウトデータ
  m_lytwk = GetLayoutWork( 0 );
  m_resID = UIView::GetG2DUtil()->GetLayoutResourceID( 0 );

  // 部品ペイン
  for( u32 i=0; i<ITEM_MAX; i++ )
  {
    m_item[i].parts = m_lytwk->GetPartsPane( tbl[i] );
  }

  { // 初期テキストボックスペインの幅
    nw::lyt::Size size = GetTextBoxPane(0)->GetSize();
    m_defTextWidth = size.width;
  }

  // 各ペインとテキストボックスペインの幅の差分
  for( u32 i=0; i<SIZE_CALC_PANE_NUM; i++ )
  {
    gfl2::lyt::LytPane * pane = GetPane( i, SizeCalcPane[i] );
    m_paneSize[i] = pane->GetSize();
  }

  { // 戻るマークの幅
    nw::lyt::Size size = GetPane( 0, PANENAME_COMMON_BTN_LOW_008_PANE_ICON_BACK )->GetSize();
    m_markWidth = size.width;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcItemWidth
 * @brief   項目の幅を計算
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::CalcItemWidth(void)
{
  m_width = m_defTextWidth;

  for( u32 i=0; i<m_max; i++ )
  {
    f32 str_width = static_cast<f32>( print::GetStringWidth( m_item[i].str, GetTextBoxPane(i) ) );
    if( m_item[i].is_mark != false )
    {
      str_width += m_markWidth;
    }
    if( str_width > m_width )
    {
      m_width = str_width;
    }
  }
  // 奇数だと配置座標によっては表示がぼやけるため対処
  if( (static_cast<s32>(m_width)%2) != 0 )
  {
    m_width += 1.0f;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetItemParam
 * @brief   項目のパラメータを設定
 * @date    2015.03.12
 */
//-----------------------------------------------------------------------------
void MenuWindow::SetItemParam(void)
{
  f32 calc_width = m_width - m_defTextWidth;

  for( u32 i=0; i<m_max; i++ )
  {
    { // テキスト
      gfl2::lyt::LytTextBox * pane = GetTextBoxPane( i );
      nw::lyt::Size size = pane->GetSize();
      size.width = m_width;
      pane->SetSize( size );
      // 書き込み
      SetTextboxPaneMessage( pane, m_item[i].str );
    }
    { // それ以外のペインのサイズ変更
      for( u32 j=0; j<SIZE_CALC_PANE_NUM; j++ )
      {
        gfl2::lyt::LytPane * pane = GetPane( i, SizeCalcPane[j] );
        nw::lyt::Size size = m_paneSize[j];
        size.width += calc_width;
        pane->SetSize( size );
      }
    }
    { // 戻るマーク
      gfl2::lyt::LytPane * pane = GetPane( i, PANENAME_COMMON_BTN_LOW_008_PANE_ICON_BACK );
      pane->SetVisible( m_item[i].is_mark );
      // Bボタン設定
      app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
      if( m_item[i].is_b_button != false )
      {
        man->SetButtonBindKey( i, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
        if( m_item[ i ].select_se_id == SMID_NULL )
        {
          man->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
        }
      }
      else
      {
        man->SetButtonBindKey( i, gfl2::ui::BUTTON_INVALID, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
        if( m_item[ i ].select_se_id == SMID_NULL )
        {
          man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
        }
      }

      if( m_item[ i ].select_se_id != SMID_NULL )
      {
        man->SetButtonSelectSE( i,m_item[ i ].select_se_id );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPutPos
 * @brief   メニューの配置座標を取得
 * @date    2015.03.12
 *
 * @param   type    配置タイプ
 * @param   px      配置基準X座標
 * @param   py      配置基準Y座標
 * @param   height  １項目の高さ
 * @param   pos     座標格納場所
 */
//-----------------------------------------------------------------------------
void MenuWindow::GetPutPos( PutType type, f32 px, f32 py, f32 height, gfl2::math::VEC3 * pos )
{
  nw::lyt::Size base_size = GetPane(0,PANENAME_COMMON_BTN_LOW_008_PANE_BOUND_00)->GetSize();
  nw::lyt::Size parts_size = m_item[0].parts->GetSize();

  switch( type )
  {
  case PUT_LT:
    pos->x = px + ( parts_size.width / 2.0f ) + ( m_width - m_defTextWidth );
    pos->y = py - ( base_size.height / 2.0f );
    break;
  case PUT_RT:
    pos->x = px - ( parts_size.width / 2.0f );
    pos->y = py - ( base_size.height / 2.0f );
    break;
  case PUT_LD:
    pos->x = px + ( parts_size.width / 2.0f ) + ( m_width - m_defTextWidth );
    pos->y = py + ( base_size.height / 2.0f ) + height * (m_max-1);
    break;
  case PUT_RD:
    pos->x = px - ( parts_size.width / 2.0f );
    pos->y = py + ( base_size.height / 2.0f ) + height * (m_max-1);
    break;
  }
  pos->z = 0.0f;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutCursor
 * @brief   カーソル配置
 * @date    2015.03.12
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void MenuWindow::PutCursor( u32 pos )
{
  m_cursorPos = pos;
  gfl2::lyt::LytPane * pane = GetPane( m_cursorPos, PANENAME_COMMON_BTN_LOW_008_PANE_NULL_CUR );
  gfl2::math::VEC3 trans = m_item[m_cursorPos].parts->GetTranslate() + pane->GetTranslate();
  nw::lyt::Size	size = pane->GetSize();
  trans.x -= ( size.width / 2.0f );
  m_pMenuCursor->SetPos( &trans );
  m_pMenuCursor->SetSize( pane );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   カーソルレイアウト作成
 * @date    2015.03.25
 */
//-----------------------------------------------------------------------------
void MenuWindow::CreateMenuCursor(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  m_pMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( GetDispType() ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursor
 * @brief   カーソルレイアウト削除
 * @date    2015.03.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuWindow::DeleteMenuCursor(void)
{
  if( m_pMenuCursor->IsModuleFree() == false )
  {
    return false;
  }
  if( m_pMenuCursor->DeleteLayoutWork() == false )
  {
    return false;
  }
  m_pMenuCursor->DeleteResource();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetDispType
 * @brief   gflのディスプレイタイプを取得
 * @date    2015.03.25
 *
 * @return  ディスプレイタイプ
 */
//-----------------------------------------------------------------------------
gfl2::lyt::DisplayType MenuWindow::GetDispType(void)
{
  if( m_disp == app::util::G2DUtil::SETUP_UPPER )
  {
    return gfl2::lyt::DISPLAY_UPPER;
  }
  else
  {
    return gfl2::lyt::DISPLAY_LOWER;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveCursor
 * @brief   カーソル移動
 * @date    2015.10.10
 *
 * @param   mv      移動値
 * @param   is_trg  true = トリガー入力
 */
//-----------------------------------------------------------------------------
void MenuWindow::MoveCursor( s32 mv, bool is_trg )
{
  s32 pos = m_cursorPos + mv;

  if( pos < 0 )
  {
    if( is_trg == false )
    {
      pos = 0;
    }
    else
    {
      pos = m_max - 1;
    }
  }
  else if( pos >= m_max )
  {
    if( is_trg == false )
    {
      pos = m_max - 1;
    }
    else
    {
      pos = 0;
    }
  }

  if( pos != m_cursorPos )
  {
    PutCursor( pos );
    Sound::PlaySE( SEQ_SE_SELECT1 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputMode
 * @brief   入力モード設定
 * @date    2016.01.26
 *
 * @param   mode  入力モード
 */
//-----------------------------------------------------------------------------
void MenuWindow::SetInputMode( app::tool::ButtonManager::InputMode mode )
{
  app::ui::UIResponder::GetButtonManager()->SetInputMode( mode );
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ペインタッチ時を検知
 * @date    2015.03.10
 *
 * @param   button_id   選択されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void MenuWindow::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
  PutCursor( button_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.03.10
 *
 * @param   button_id   選択されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult MenuWindow::OnLayoutPaneEvent( const u32 button_id )
{
  m_listener->SelectMenu( button_id );
  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.03.10
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
app::ui::UIInputListener::ListenerResult MenuWindow::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();

  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    man->StartSelectedAct( m_cursorPos );
//    Sound::PlaySE( SEQ_SE_DECIDE1 );
    return DISABLE_ACCESS;
  }

  // ボタンマネージャが動作中かどうか
  if( man->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE )
  {
    return DISABLE_ACCESS;
  }

  if( key->IsTrigger(gfl2::ui::DIR_UP) )
  {
    MoveCursor( -1, true );
    return DISABLE_ACCESS;
  }
  if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
  {
    MoveCursor( 1, true );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_UP) )
  {
    MoveCursor( -1, false );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
  {
    MoveCursor( 1, false );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}




#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
 * @brief "はい/いいえ"の座標チェックのアサートを無効化する
 *
 * @note  無断使用禁止。使用する場合は中村まで。
 */
//-----------------------------------------------------------------------------
void MenuWindow::Debug_YesNoPosAssertInvalid(void)
{
  m_debugYesNoInvalid = true;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

//=============================================================================
/**
 * @file    MenuDraw.cpp
 * @brief   フィールド下画面メニュー表示
 * @author  Hiroyuki Nakamura
 * @date    2015.05.20
 */
//=============================================================================

// module
#include "App/FieldMenu/include/FieldMenuListener.h"
#include "MenuDraw.h"
#include "MenuDefine.h"
// niji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/AppToolSlideScroll.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "Sound/include/Sound.h"
#include "App/AppEvent/include/PokeParure/PokeParureEvent.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_field_menu.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "arc_index/FieldMenu.gaix"
#include "niji_conv_header/app/field_menu/MainLower.h"
#include "niji_conv_header/app/field_menu/MainLower_pane.h"
#include "niji_conv_header/app/field_menu/MainLower_anim_list.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//! メニューアイコンの部品ペインテーブル
const gfl2::lyt::LytPaneIndex MenuDraw::IconPartsPaneTable[Savedata::FieldMenu::MENU_ICON_MAX] =
{
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_00,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_01,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_02,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_03,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_04,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_05,

  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_06,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_07,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_08,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_09,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_10,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CUSTOMBUTTON_11,
};

//! メニューアイコンの親ペインテーブル
const gfl2::lyt::LytPaneIndex MenuDraw::MenuIconParentTable[Savedata::FieldMenu::PAGE_MAX][Savedata::FieldMenu::PAGE_ICON_MAX] =
{
  {
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE00_ICON00,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE00_ICON01,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE00_ICON02,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE00_ICON03,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE00_ICON04,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE00_ICON05,
  },
  {
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE01_ICON00,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE01_ICON01,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE01_ICON02,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE01_ICON03,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE01_ICON04,
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE01_ICON05,
  },
};

//! ページマークの部品ペインテーブル
const gfl2::lyt::LytPaneIndex MenuDraw::PagePartsPaneTable[Savedata::FieldMenu::PAGE_MAX] =
{
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGEBUTTON_00,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGEBUTTON_01,
};

//! ページNewマークアニメ
const u32 MenuDraw::PageNewAnimeTable[Savedata::FieldMenu::PAGE_MAX] =
{
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_00_NEWANIME,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_01_NEWANIME,
};

//! 各ページの親ペインテーブル
const gfl2::lyt::LytPaneIndex MenuDraw::PageParentPaneTable[Savedata::FieldMenu::PAGE_MAX] =
{
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE00,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_PAGE01,
};

//! ページ切り替え矢印の部品ペインテーブル
const gfl2::lyt::LytPaneIndex MenuDraw::ArrowPartsPaneTable[2] =
{
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CURSOR_R,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_CURSOR_L,
};

//! メニューアイコンのアニメテーブル
const u32 MenuDraw::MenuIconAnimeTable[Savedata::FieldMenu::MENU_ICON_MAX] =
{
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_ICON_00,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_ICON_01,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_ICON_02,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_ICON_04,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_ICON_08,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_ICON_09,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_ICON_03,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_ICON_06,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_ICON_07,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_ICON_11,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_ICON_10,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_ICON_05,
};

//! ページドラッグ時のリリーススクロール速度
const f32 MenuDraw::PAGE_RELEASE_SCROLL_SPEED = static_cast<f32>( DISPLAY_LOWER_WIDTH / 8 );

const f32 MenuDraw::SRIDE_TOUCH_AREA_UY = 80.0f;    //!< スライド操作のタッチ範囲座標・上
const f32 MenuDraw::SRIDE_TOUCH_AREA_DY = -80.0f;   //!< スライド操作のタッチ範囲座標・下
const f32 MenuDraw::SRIDE_TOUCH_AREA_LX = -160.0f;  //!< スライド操作のタッチ範囲座標・左
const f32 MenuDraw::SRIDE_TOUCH_AREA_RX = 160.0f;   //!< スライド操作のタッチ範囲座標・右

//! メニューアイコンの場所ペイン（キャッチ時などに表示される影っぽいペイン）
const gfl2::lyt::LytPaneIndex MenuDraw::IconCathcBgPaneTable[Savedata::FieldMenu::PAGE_ICON_MAX] =
{
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_BUTTONAREA_00,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_BUTTONAREA_01,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_BUTTONAREA_02,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_BUTTONAREA_03,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_BUTTONAREA_04,
  PANENAME_FIELDMENU_MIN_LOW_000_PANE_BUTTONAREA_05,
};

//! メニューアイコンのお知らせ動作アニメ
const u32 MenuDraw::IconInfoAnime[Savedata::FieldMenu::MENU_ICON_MAX] =
{
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_MOVE,
  LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_MOVE,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   is_open     メニューを開いているか
 * @param   disp_mode   表示モード
 */
//-----------------------------------------------------------------------------
MenuDraw::MenuDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const bool * is_open, const s32 * disp_mode )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_isMenuOpen( is_open )
  , m_dispMode( disp_mode )
  , m_frameListener( NULL )
  , m_saveFieldMenu( NULL )
  , m_eventWork( NULL )
  , m_saveResort( NULL )
  , m_saveBoxPokemon( NULL )
  , m_party( NULL )
  , m_cursorPosIconID( Savedata::FieldMenu::ICON_ID_POKE_LIST )
  , m_catchIconID( Savedata::FieldMenu::ICON_ID_POKE_LIST )
  , m_catchIconPage( 0 )
  , m_releaseIconMoveCount( 0 )
  , m_lastTouchX( 0 )
  , m_lastTouchY( 0 )
  , m_iconPutPosID( 0 )
  , m_iconPutIndex( 0 )
  , m_getIconCalcX( 0.0f )
  , m_getIconCalcY( 0.0f )
  , m_putIconCalcX( 0.0f )
  , m_putIconCalcY( 0.0f )
  , m_isIconCatch( false )
  , m_pSlideScroll( NULL )
  , m_pageReleaseScrollSpeed( 0.0f )
  , m_pageReleaseScrollEnd( 0.0f )
  , m_isInputTrigger( false )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_pRotomMsgData( NULL )
  , m_pRotomMsgBuff( NULL )
  , m_pCommonIcon( NULL )
  , m_pTmpCoreParam( NULL )
  , m_isInputLock( false )
  , m_isDrawEnable( true )
  , m_isMenuInfoAnimeForceStop( false )
  , m_isInitEnd( false )
  , m_festaInfoValue( 0 )
  , m_subSeq( 0 )
{
  SetInputEnabled( false );   // デフォルトがONなのでOFFにしておく
  GetGameData();
  m_pSlideScroll = GFL_NEW(m_heap->GetSystemHeap()) App::Tool::SlideScroll( App::Tool::SlideScroll::MODE_H );
  m_pSlideScroll->SetTouchArea( SRIDE_TOUCH_AREA_UY, SRIDE_TOUCH_AREA_DY, SRIDE_TOUCH_AREA_LX, SRIDE_TOUCH_AREA_RX );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MenuDraw::~MenuDraw()
{
  GFL_DELETE m_pSlideScroll;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ関連取得
 * @date    2015.09.09
 */
//-----------------------------------------------------------------------------
void MenuDraw::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_saveFieldMenu  = gd->GetFieldMenu();
  m_eventWork      = gd->GetEventWork();
  m_saveResort     = gd->GetResortSave();
  m_saveBoxPokemon = gd->GetBoxPokemon();
  m_party          = gd->GetPlayerParty();
}

//-----------------------------------------------------------------------------S
/**
 * @func    Update
 * @brief   更新
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void MenuDraw::Update(void)
{
  app::ui::UIView::Update();
  if( m_pMenuCursor != NULL )
  {
    m_pMenuCursor->Update();
  }

  UpdateMenuInfo();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.05.22
 *
 * @param   displayNo   描画ディスプレイ番号
 */
//-----------------------------------------------------------------------------
void MenuDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_MENU, DRAW_PRI_LOWER_MENU_ICON );
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_MENU_UPPER, DRAW_PRI_LOWER_MENU_ICON );
    m_pMenuCursor->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER, DRAW_PRI_LOWER_CURSOR );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFrameListener
 * @brief   リスナーをセット
 * @date    2015.05.22
 *
 * @param   listener  リスナー
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetFrameListener( FrameListener * listener )
{
  m_frameListener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileLoad(
      ARCID_FIELD_MENU,
      GARC_FieldMenu_MainLower_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
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
 * @func    UnloadLayoutResource
 * @brief   レイアウトリソース削除
 * @date    2016.01.08
 */
//-----------------------------------------------------------------------------
void MenuDraw::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージリソース読み込み
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId();

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( arc_id ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_field_menu_DAT,
      &m_pMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_rotom_message_DAT,
      &m_pRotomMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileLoad( &m_pRotomMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:
    if( app::util::FileAccessor::IsFileClose( arc_id ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadMessage
 * @brief   メッセージリソース削除
 * @date    2016.01.08
 */
//-----------------------------------------------------------------------------
void MenuDraw::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pRotomMsgBuff );
  GflHeapFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   メニューカーソル生成
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::CreateMenuCursor(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pMenuCursor = GFL_NEW(m_heap->GetSystemHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorBuffer );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMenuCursor->LoadResource() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursor
 * @brief   メニューカーソル削除
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::DeleteMenuCursor(void)
{
  if( m_pMenuCursor != NULL )
  {
    if( m_pMenuCursor->IsModuleFree() == false )
    {
      return false;
    }
    m_pMenuCursor->DeleteResource();
    GFL_SAFE_DELETE( m_pMenuCursor );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMenuCursor
 * @brief   メニューカーソル取得
 * @date    2016.01.08
 *
 * @return  メニューカーソル
 */
//-----------------------------------------------------------------------------
app::tool::MenuCursor * MenuDraw::GetMenuCursor(void)
{
  return m_pMenuCursor;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateCommonIcon
 * @brief   Newアイコン生成
 * @date    2016.01.16
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::CreateCommonIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pCommonIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::AppCommonGrpIconData();
    m_pCommonIcon->Initialize( m_heap );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pCommonIcon->IsReady() == false )
    {
      break;
    }
    ChangeNewIconTexture();
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteCommonIcon
 * @brief   Newアイコン削除
 * @date    2016.01.16
 */
//-----------------------------------------------------------------------------
void MenuDraw::DeleteCommonIcon(void)
{
  if( m_pCommonIcon != NULL )
  {
    m_pCommonIcon->Terminate();
    GFL_SAFE_DELETE( m_pCommonIcon );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeNewIconTexture
 * @brief   Newアイコンのテクスチャ差し替え
 * @date    2016.01.16
 */
//-----------------------------------------------------------------------------
void MenuDraw::ChangeNewIconTexture(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<Savedata::FieldMenu::MENU_ICON_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( lytwk->GetPartsPane(IconPartsPaneTable[i]), PANENAME_FIELDMENU_BTN_LOW_001_PANE_NEW_ICON, &res_id );
    m_pCommonIcon->ReplacePaneTextureByNewIcon( lytwk->GetPicturePane(parts,PANENAME_COMMON_ICN_013_PANE_NEW_ICON_01,&res_id) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void MenuDraw::Init(void)
{
  GetG2DUtil()->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_ALL, false );

  CreateTempCoreParam();

  CreateButton();
  InitMenuIcon();
  InitMenuIconPos();
  EndPageChangeAnime( m_saveFieldMenu->GetMenuPage() );

  m_isInitEnd = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.05.22
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::End(void)
{
  DeleteTempCoreParam();
  return Terminate2D();
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::Initialize2D(void)
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { m_pLytResBuff, 0, app::util::G2DUtil::ATTACH_ROOT },
  };

  m_pMsgData = GFL_NEW(m_heap->GetSystemHeap()) gfl2::str::MsgData( m_pMsgBuff, m_heap->GetSystemHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetSystemHeap()) print::WordSet( m_heap->GetSystemHeap() );

  m_pRotomMsgData = GFL_NEW(m_heap->GetSystemHeap()) gfl2::str::MsgData( m_pRotomMsgBuff, m_heap->GetSystemHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    NULL,
    0,
    m_pMsgData,
    m_pWordSet );

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::Terminate2D(void)
{
  m_isDrawEnable = false;

  if( m_pMsgData == NULL )
  {
    return true;
  }

  if( IsDrawing() != false )
  {
    return false;
  }

  if( DeleteMenuCursorLayout() == false )
  {
    return false;
  }

  Delete2D();

  GFL_SAFE_DELETE( m_pRotomMsgData );

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMsgData
 * @brief   ロトムメッセージ取得
 * @date    2016.01.08
 *
 * @param   ロトムメッセージ
 */
//-----------------------------------------------------------------------------
gfl2::str::MsgData * MenuDraw::GetRotomMsgData(void)
{
  return m_pRotomMsgData;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLayout
 * @brief   レイアウト生成
 * @date    2016.01.08
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreateLayout(void)
{
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_MAINLOWER_FIELDMENU_MIN_LOW_000_BFLYT,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
    {
      0,
      LYTRES_MAINLOWER_FIELDMENU_MIN_UPP_001_BFLYT,
      LA_MAINLOWER_FIELDMENU_MIN_UPP_001___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      false
    },
  };

  GetG2DUtil()->SetLayoutWork(
    LYTID_MENU,
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_heap->GetDeviceAllocator(),
    m_heap->GetDeviceAllocator() );

  StartPagePointerAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursorLayout
 * @brief   メニューカーソル用レイアウト作成
 * @date    2016.01.08
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreateMenuCursorLayout(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  m_pMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
//  m_pMenuCursor->SetVisible( false );
  m_pMenuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursorLayout
 * @brief   メニューカーソル用レイアウト削除
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::DeleteMenuCursorLayout(void)
{
  return m_pMenuCursor->DeleteLayoutWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void MenuDraw::Start(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetLayoutDrawEnable( LYTID_MENU_UPPER, true );
  g2d->SetLayoutDrawEnable( LYTID_MENU, true );
  g2d->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_ALL, true );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    Stop
 * @brief   停止
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void MenuDraw::Stop(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetLayoutDrawEnable( LYTID_MENU_UPPER, false );
  g2d->SetLayoutDrawEnable( LYTID_MENU, false );
  g2d->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_ALL, false );
  SetInputListener( NULL );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreateButton(void)
{
  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[BUTTON_ID_MAX];

  CreatePageArrowButton( &info[BUTTON_ID_PAGE_R] );
  CreateIconButton( &info[BUTTON_ID_ICON_00] );
  CreatePageButton( &info[BUTTON_ID_PAGE_00] );
  CreateCloseButton( &info[BUTTON_ID_CLOSE] );
  CreateReportButton( &info[BUTTON_ID_REPORT] );

  app::ui::UIResponder::CreateButtonManager( m_heap, GetG2DUtil()->GetLayoutWork(LYTID_MENU), info, BUTTON_ID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonBindKey( BUTTON_ID_PAGE_L, gfl2::ui::BUTTON_L, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BUTTON_ID_PAGE_R, gfl2::ui::BUTTON_R, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BUTTON_ID_CLOSE, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  for( u32 i=0; i<BUTTON_ID_MAX; i++ )
  {
    if( i == BUTTON_ID_PAGE_R ||
        i == BUTTON_ID_PAGE_L ||
        i == BUTTON_ID_PAGE_00 ||
        i == BUTTON_ID_PAGE_01 )
    {
      man->SetButtonSelectSE( i, SEQ_SE_PAGE );
    }
    else if( i== BUTTON_ID_CLOSE )
    {
      man->SetButtonSelectSE( i, SEQ_SE_CLOSE1 );
    }
    else
    {
      man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }

  // メニューボタンの受付タイミングをアニメーション開始時に受け付けるように設定
  for( u32 i=BUTTON_ID_ICON_00; i<=BUTTON_ID_ICON_11; i++ )
  {
    man->SetNotifyTimming( i, app::tool::ButtonManager::NOTIFY_START_ANIME );
  }
  man->SetNotifyTimming( BUTTON_ID_REPORT, app::tool::ButtonManager::NOTIFY_START_ANIME );

  ChangeButtonEnable( IsMenuOpen() );
  ChangePageButton();
//  PutPageMark();

  ChangeIconButtonEnable( IsMenuOpen() );
  SetVisibleIconArea( IsMenuOpen() );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetButtonInfoEx
 * @brief   ボタン情報生成
 * @date    2015.05.26
 *
 * @param   info        情報格納場所
 * @param   parts_tbl   部品テーブル
 * @param   info_tbl    デフォルト情報
 * @param   bound       バウンディングペイン
 * @param   max         生成数
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, gfl2::lyt::LytPaneIndex bound, u32 max )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<max; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound, &res_id );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePageArrowButton
 * @brief   ページ切り替え矢印ボタン情報生成
 * @date    2015.05.26
 *
 * @param   info  格納場所
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreatePageArrowButton( app::ui::ButtonInfoEx * info )
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_PAGE_R, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_R_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_R_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_R_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_R_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_R_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_R_PASSIVE,
    },
    {
      BUTTON_ID_PAGE_L, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_L_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_L_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_L_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_L_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_L_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_L_PASSIVE,
    },
  };

  SetButtonInfoEx( info, ArrowPartsPaneTable, info_tbl, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, GFL_NELEMS(ArrowPartsPaneTable) );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateIconButton
 * @brief   メニュー項目ボタン情報生成
 * @date    2015.05.26
 *
 * @param   info  格納場所
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreateIconButton( app::ui::ButtonInfoEx * info )
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_ICON_00, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00_PASSIVE,
    },
    {
      BUTTON_ID_ICON_01, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01_PASSIVE,
    },
    {
      BUTTON_ID_ICON_02, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02_PASSIVE,
    },
    {
      BUTTON_ID_ICON_03, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03_PASSIVE,
    },
    {
      BUTTON_ID_ICON_04, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04_PASSIVE,
    },
    {
      BUTTON_ID_ICON_05, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05_PASSIVE,
    },
    {
      BUTTON_ID_ICON_06, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06_PASSIVE,
    },
    {
      BUTTON_ID_ICON_07, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07_PASSIVE,
    },
    {
      BUTTON_ID_ICON_08, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08_PASSIVE,
    },
    {
      BUTTON_ID_ICON_09, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09_PASSIVE,
    },
    {
      BUTTON_ID_ICON_10, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10_PASSIVE,
    },
    {
      BUTTON_ID_ICON_11, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11_PASSIVE,
    },
  };

  SetButtonInfoEx( info, IconPartsPaneTable, info_tbl, PANENAME_FIELDMENU_BTN_LOW_001_PANE_BOUND_00, Savedata::FieldMenu::MENU_ICON_MAX );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePageButton
 * @brief   ページボタン情報生成
 * @date    2015.05.26
 *
 * @param   info  格納場所
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreatePageButton( app::ui::ButtonInfoEx * info )
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_PAGE_00, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_00_PAGE_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_00_PAGE_PASSIVE,
    },
    {
      BUTTON_ID_PAGE_01, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_01_PAGE_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_01_PAGE_PASSIVE,
    },
  };

  SetButtonInfoEx( info, PagePartsPaneTable, info_tbl, PANENAME_FIELDMENU_BTN_LOW_005_PANE_BOUND_00, Savedata::FieldMenu::PAGE_MAX );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateCloseButton
 * @brief   戻るボタン情報生成
 * @date    2016.01.06
 *
 * @param   info  格納場所
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreateCloseButton( app::ui::ButtonInfoEx * info )
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_CLOSE, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BACKBUTTON_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BACKBUTTON_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BACKBUTTON_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_BACKBUTTON,
  };

  SetButtonInfoEx( info, parts_tbl, info_tbl, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, GFL_NELEMS(info_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateReportButton
 * @brief   レポートボタン情報生成
 * @date    2017.01.06
 *
 * @param   info  格納場所
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreateReportButton( app::ui::ButtonInfoEx * info )
{
  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_REPORT, NULL, NULL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__REPORTBUTTON_TOUCH,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__REPORTBUTTON_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__REPORTBUTTON_CANSEL,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__REPORTBUTTON_TOUCH_RELEASE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__REPORTBUTTON_ACTIVE,
      LA_MAINLOWER_FIELDMENU_MIN_LOW_000__REPORTBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_FIELDMENU_MIN_LOW_000_PANE_REPORTBUTTON,
  };

  SetButtonInfoEx( info, parts_tbl, info_tbl, PANENAME_FIELDMENU_BTN_LOW_001A_PANE_BOUND_00, GFL_NELEMS(info_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeIconButtonEnable
 * @brief   メニュー項目ボタン入力切り替え
 * @date    2015.08.21
 *
 * @param   is_open   true = メニューが開いている
 */
//-----------------------------------------------------------------------------
void MenuDraw::ChangeIconButtonEnable( bool is_open )
{
  // メニューアイコン
  for( int i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
  {
    for( int j=0; j<Savedata::FieldMenu::PAGE_ICON_MAX; j++ )
    {
      Savedata::FieldMenu::IconID id = m_saveFieldMenu->GetMenuIconID( i, j );
      if( m_saveFieldMenu->IsMenuIconEnable(id) != false )
      {
        SetVisibleMenuIcon( id, is_open );
      }
    }
  }

  { // ページボタン
    u32 cnt = 0;
    for( u32 i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
    {
      if( m_saveFieldMenu->IsMenuPageEnable(i) != false )
      {
        cnt++;
      }
    }
    if( cnt > 1 )
    {
      app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
      for( u32 i=0; i<cnt; i++ )
      {
        if( is_open == false )
        {
          man->SetButtonPassive( BUTTON_ID_PAGE_00+i, false );
          man->SetButtonInvisible( BUTTON_ID_PAGE_00+i );
        }
        else
        {
          if( i != m_saveFieldMenu->GetMenuPage() )
          {
            man->SetButtonActive( BUTTON_ID_PAGE_00+i, false );
          }
          man->SetButtonVisible( BUTTON_ID_PAGE_00+i, false );
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeButtonEnable
 * @brief   ボタン入力制御切り替え
 * @date    2015.06.01
 *
 * @param   is_open   true = メニューが開いている
 */
//-----------------------------------------------------------------------------
void MenuDraw::ChangeButtonEnable( bool is_open )
{
  static const ButtonID open[] =
  {
    BUTTON_ID_PAGE_R,
    BUTTON_ID_PAGE_L,
    BUTTON_ID_CLOSE,
  };

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( is_open )
  {
    for( u32 i=0; i<GFL_NELEMS(open); i++ )
    {
      man->SetButtonActive( open[i], false );
      man->SetButtonVisible( open[i], false );
    }
    // １ページ以下の場合は矢印を消す
    if( IsPageChange() == false )
    {
      man->SetButtonPassive( BUTTON_ID_PAGE_R, false );
      man->SetButtonPassive( BUTTON_ID_PAGE_L, false );
      man->SetButtonInvisible( BUTTON_ID_PAGE_R );
      man->SetButtonInvisible( BUTTON_ID_PAGE_L );
    }
    else if( m_saveFieldMenu->GetMenuPage() == 0 )
    {
      man->SetButtonPassive( BUTTON_ID_PAGE_L, false );
      man->SetButtonInvisible( BUTTON_ID_PAGE_L );
    }
    else
    {
      man->SetButtonPassive( BUTTON_ID_PAGE_R, false );
      man->SetButtonInvisible( BUTTON_ID_PAGE_R );
    }
  }
  else
  {
    for( u32 i=0; i<GFL_NELEMS(open); i++ )
    {
      man->SetButtonPassive( open[i], false );
      man->SetButtonInvisible( open[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleCloseButton
 * @brief   閉じるボタン表示切り替え
 * @date    2016.01.07
 *
 * @param   flg   true = 表示, false = 非表示
 *
 * @fix MMCat[61]: キャッチ用の関数なので、レポートの表示をここで切り替える
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetVisibleCloseButton( bool flg )
{
  static const ButtonID tbl[] =
  {
    BUTTON_ID_CLOSE,    // 閉じる
    BUTTON_ID_REPORT,   // レポートボタン ( momiji追加 )
  };

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( flg != false )
  {
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      man->SetButtonActive( tbl[i], false );
      man->SetButtonVisible( tbl[i], false );
    }
  }
  else
  {
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      man->SetButtonPassive( tbl[i], false );
      man->SetButtonInvisible( tbl[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePageArrowVisible
 * @brief   ページ切り替え矢印表示切り替え
 * @date    2015.06.30
 *
 * @param   page  現在のページ
 */
//-----------------------------------------------------------------------------
void MenuDraw::ChangePageArrowVisible( u32 page )
{
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( page == 0 )
  {
    man->SetButtonActive( BUTTON_ID_PAGE_R, false );
    man->SetButtonVisible( BUTTON_ID_PAGE_R, false );

    man->SetButtonPassive( BUTTON_ID_PAGE_L, false );
    man->SetButtonInvisible( BUTTON_ID_PAGE_L );
  }
  else
  {
    man->SetButtonActive( BUTTON_ID_PAGE_L, false );
    man->SetButtonVisible( BUTTON_ID_PAGE_L, false );

    man->SetButtonPassive( BUTTON_ID_PAGE_R, false );
    man->SetButtonInvisible( BUTTON_ID_PAGE_R );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePageButton
 * @brief   ページボタン表示切り替え
 * @date    2015.06.01
 */
//-----------------------------------------------------------------------------
void MenuDraw::ChangePageButton(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
  u32 cnt = 0;

  for( u32 i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
  {
    if( m_saveFieldMenu->IsMenuPageEnable(i) != false )
    {
      man->SetButtonActive( BUTTON_ID_PAGE_00+i, false );
      man->SetButtonVisible( BUTTON_ID_PAGE_00+i, false );
      cnt++;
    }
    else
    {
      man->SetButtonPassive( BUTTON_ID_PAGE_00+i, false );
      man->SetButtonInvisible( BUTTON_ID_PAGE_00+i );
    }
    if( i == m_saveFieldMenu->GetMenuPage() )
    {
      static const u32 anm[] =
      {
        LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_00_PAGE_ACTIVE,
        LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_01_PAGE_ACTIVE,
      };
      g2d->StartAnime( LYTID_MENU, anm[i] );
      man->SetButtonPassive( BUTTON_ID_PAGE_00+i, false );
    }
    else
    {
      static const u32 anm[] =
      {
        LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_00_PAGE_PASSIVE,
        LA_MAINLOWER_FIELDMENU_MIN_LOW_000__PAGEBUTTON_01_PAGE_PASSIVE,
      };
      g2d->StartAnime( LYTID_MENU, anm[i] );
    }
  }

  // １ページ以下の場合は全部消す
  if( cnt <= 1 )
  {
    for( u32 i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
    {
      man->SetButtonPassive( BUTTON_ID_PAGE_00+i, false );
      man->SetButtonInvisible( BUTTON_ID_PAGE_00+i );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPageChange
 * @brief   ページを切り替えられるか
 * @date    2015.06.09
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsPageChange(void)
{
  u32 cnt = 0;
  for( u32 i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
  {
    if( m_saveFieldMenu->IsMenuPageEnable(i) != false )
    {
      cnt++;
    }
  }
  if( cnt <= 1 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMenuIcon
 * @brief   メニュー項目表示初期化
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void MenuDraw::InitMenuIcon(void)
{
  static const u32 str_item_tbl[] =
  {
    field_menu_01_01,
    field_menu_01_02,
    field_menu_01_03,
    field_menu_20_01,
    field_menu_02_01,
    field_menu_02_02,

    field_menu_01_04,
    field_menu_03_01,
    field_menu_03_02,
    field_menu_02_04,
    field_menu_02_03,
    field_menu_01_06,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<Savedata::FieldMenu::MENU_ICON_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts  = lytwk->GetPartsPane( IconPartsPaneTable[i] );
    gfl2::lyt::LytTextBox * pane = lytwk->GetTextBoxPane( parts, PANENAME_FIELDMENU_BTN_LOW_001_PANE_TEXTBOX_00, &res_id );

    g2d->SetTextBoxPaneString( pane, str_item_tbl[i] );
    g2d->StartAnime( LYTID_MENU, MenuIconAnimeTable[i] );
  }

  { // momiji用レポート
    gfl2::lyt::LytParts * parts  = lytwk->GetPartsPane( PANENAME_FIELDMENU_MIN_LOW_000_PANE_REPORTBUTTON );
    gfl2::lyt::LytTextBox * pane = lytwk->GetTextBoxPane( parts, PANENAME_FIELDMENU_BTN_LOW_001A_PANE_TEXTBOX_00, &res_id );
    g2d->SetTextBoxPaneString( pane, field_menu_01_05 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMenuIconAnimeEnd
 * @brief   メニュー項目アニメ終了チェック
 * @date    2015.08.24
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsMenuIconAnimeEnd(void)
{
  for( u32 i=0; i<Savedata::FieldMenu::MENU_ICON_MAX; i++ )
  {
    if( GetG2DUtil()->IsAnimeEnd( LYTID_MENU, MenuIconAnimeTable[i] ) == false )
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleIconName
 * @brief   メニュー項目名表示切り替え
 * @date    2015.05.29
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetVisibleIconName( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<Savedata::FieldMenu::MENU_ICON_MAX; i++ )
  {
    g2d->SetPaneVisible(
      lytwk->GetPane( lytwk->GetPartsPane(IconPartsPaneTable[i]), PANENAME_FIELDMENU_BTN_LOW_001_PANE_FUNCITONNAME, &res_id ),
      flg );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsInfoIcon
 * @brief   メニュー項目情報表示取得
 * @date    2015.12.03
 *
 * @param   id      項目ID
 * @param   is_new  Newアイコン表示格納場所
 * @param   is_info 情報アイコン表示格納場所
 * @param   num     数値格納場所
 */
//-----------------------------------------------------------------------------
void MenuDraw::IsInfoIcon( Savedata::FieldMenu::IconID id, bool * is_new, bool * is_info, u32 * num )
{
  // 初期化
  *is_new  = false;
  *num     = 0;
  if( is_info != NULL )
  {
    *is_info = false;
  }

  if( m_saveFieldMenu->IsMenuIconEnable(id) != false )
  {
    // Newマーク
    if( m_saveFieldMenu->IsIconNew(id) != false )
    {
      *is_new = true;
    }
    // ジョインフェスタ用
    else if( id == Savedata::FieldMenu::ICON_ID_JOIN_FESTA )
    {
      // 「！」マーク
      if( is_info != NULL )
      {
        if( NetAppLib::JoinFesta::JoinFestaPersonalEvent::IsEventExist() )
        {
          *is_info = true;
        }
      }
      // 数値
      *num = NetLib::NijiNetworkSystem::GetOnlinePlayerNum();
    }
    // リゾート用
    else if( id == Savedata::FieldMenu::ICON_ID_POKE_PLANET )
    {
      // 「！」マーク
      if( is_info != NULL )
      {
        if( m_saveResort->GetResortProgress(Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING) == false )
        {
          if( m_saveBoxPokemon->GetPokemonCountAll( m_pTmpCoreParam, false ) >= RESORT_INFO_ICON_POKE_COUNT )
          {
            *is_info = true;
          }
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsIconInfoMove
 * @brief   メニュー項目情報を動作させるか
 * @date    2016.04.02
 *
 * @param   id  項目ID
 *
 * @retval  true  = 動作
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsIconInfoMove( Savedata::FieldMenu::IconID id )
{
  bool result = false;

  if( m_saveFieldMenu->IsIconNew(id) == false )
  {
    // ジョインフェスタ
    if( id == Savedata::FieldMenu::ICON_ID_JOIN_FESTA )
    {
      if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->IsExistOpenAttractionUser() )
      {
        result = true;
      }
    }
    // かわいがり
    else if( id == Savedata::FieldMenu::ICON_ID_POKE_PARURE )
    {
      pml::pokepara::PokemonParam * pp = m_party->GetMemberPointer( m_party->GetMarkingIndex() );
      bool mode = pp->StartFastMode();
      if( pp->GetHp() != 0 && pp->GetFeed() == 0 && App::Event::PokeParureEvent::IsNoHungryPokemon(pp->GetMonsNo()) == false )
      {
        result = true;
      }
      pp->EndFastMode( mode );
    }
  }

  return result;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMenuIconPos
 * @brief   メニュー項目表示位置初期化
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void MenuDraw::InitMenuIconPos(void)
{
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  for( int i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
  {
    for( int j=0; j<Savedata::FieldMenu::PAGE_ICON_MAX; j++ )
    {
      Savedata::FieldMenu::IconID id = m_saveFieldMenu->GetMenuIconID( i, j );
      SetMenuIconPos( id, i, j );

      if( m_saveFieldMenu->IsMenuIconEnable(id) == false )
      {
        SetVisibleMenuIcon( id, false );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuIconPos
 * @brief   メニュー項目表示位置セット
 * @date    2015.05.29
 *
 * @param   id    項目ID
 * @param   page  ページ
 * @param   pos   位置
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetMenuIconPos( Savedata::FieldMenu::IconID id, u32 page, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  g2d->AppendChild( LYTID_MENU, MenuIconParentTable[page][pos], IconPartsPaneTable[id] );
  {
    gfl2::math::VEC3 trans( 0.0f, 0.0f, 0.0f );
    g2d->SetPanePos( LYTID_MENU, IconPartsPaneTable[id], &trans );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleMenuIcon
 * @brief   メニュー項目表示切り替え
 * @date    2015.08.21
 *
 * @param   id    項目ID
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetVisibleMenuIcon( Savedata::FieldMenu::IconID id, bool flg )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_FIELDMENU_BTN_LOW_001_PANE_SHADOW,
    PANENAME_FIELDMENU_BTN_LOW_001_PANE_MAIN,
    PANENAME_FIELDMENU_BTN_LOW_001_PANE_NULL_VISIBLE,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( IconPartsPaneTable[id] );

  for( int i=0; i<GFL_NELEMS(tbl); i++ )
  {
    gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, tbl[i], &res_id );
    pane->SetVisible( flg );
  }

  // @note ボタンの表示を切り替えるとキャッチ移動の判定ができなくなってしまう
  if( flg == false )
  {
    app::ui::UIResponder::GetButtonManager()->SetButtonPassive( BUTTON_ID_ICON_00+id, false );
//    app::ui::UIResponder::GetButtonManager()->SetButtonInvisible( BUTTON_ID_ICON_00+id );
  }
  else
  {
    app::ui::UIResponder::GetButtonManager()->SetButtonActive( BUTTON_ID_ICON_00+id, false );
//    app::ui::UIResponder::GetButtonManager()->SetButtonVisible( BUTTON_ID_ICON_00+id, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetIconNewAnime
 * @brief   Newアイコンアニメ切り替え（メニュー項目用）
 * @date    2015.12.03
 *
 * @param   id    項目ID
 * @param   flg   true = 再生, false = 停止
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetIconNewAnime( u32 id, bool flg )
{
  static const u32 new_anm[] =
  {
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_00__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_01__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_02__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_03__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_04__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_05__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_06__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_07__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_08__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_09__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_10__NEW_ICON_LOOP,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CUSTOMBUTTON_11__NEW_ICON_LOOP,
  };
  if( flg == false )
  {
    GetG2DUtil()->StopAnime( LYTID_MENU, new_anm[id] );
  }
  else
  {
    GetG2DUtil()->StartAnime( LYTID_MENU, new_anm[id] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartIconNewAnime
 * @brief   Newアイコンアニメ開始（メニュー項目用）
 * @date    2015.12.03
 */
//-----------------------------------------------------------------------------
void MenuDraw::StartIconNewAnime(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  bool is_info, is_new;
  u32 num;

  for( u32 i=0; i<Savedata::FieldMenu::MENU_ICON_MAX; i++ )
  {
    IsInfoIcon( static_cast<Savedata::FieldMenu::IconID>(i), &is_new, &is_info, &num );

    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( IconPartsPaneTable[i] );

    g2d->SetPaneVisible( lytwk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_001_PANE_NEW_ICON, &res_id ), is_new );
    SetIconNewAnime( i, is_new );

    g2d->SetPaneVisible( lytwk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_001_PANE_INFOMARK, &res_id ), is_info );

    PutMenuInfoValue( lytwk, &res_id, parts, num );

    if( static_cast<Savedata::FieldMenu::IconID>(i) == Savedata::FieldMenu::ICON_ID_JOIN_FESTA )
    {
      m_festaInfoValue = num;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StopIconNewAnime
 * @brief   Newアイコンアニメ停止（メニュー項目用）
 * @date    2015.12.03
 */
//-----------------------------------------------------------------------------
void MenuDraw::StopIconNewAnime(void)
{
  for( u32 i=0; i<Savedata::FieldMenu::MENU_ICON_MAX; i++ )
  {
    SetIconNewAnime( i, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetIconInfoMoveAnime
 * @brief   お知らせアイコンアニメ操作
 * @date    2016.04.02
 *
 * @param   flg   true = 再生, false = 停止
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetIconInfoMoveAnime( bool flg )
{
  // 再生禁止の場合は強制的に停止
  if( m_isMenuInfoAnimeForceStop != false )
  {
    flg = false;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  for( u32 i=0; i<Savedata::FieldMenu::MENU_ICON_MAX; i++ )
  {
    if( flg != false && IsIconInfoMove( static_cast<Savedata::FieldMenu::IconID>(i) ) != false )
    {
      if( g2d->IsAnimeEnd( LYTID_MENU, IconInfoAnime[i] ) != false )
      {
        g2d->StartAnime( LYTID_MENU, IconInfoAnime[i] );
      }
    }
    else
    {
      if( g2d->IsAnimeEnd( LYTID_MENU, IconInfoAnime[i] ) == false )
      {
        g2d->StopAnime( LYTID_MENU, IconInfoAnime[i] );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuInfoAnimeEnable
 * @brief   お知らせアイコンアニメ操作制御設定
 * @date    2016.05.12
 *
 * @param   flg   true = 操作可, false = 操作不可
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetMenuInfoAnimeEnable( bool flg )
{
  m_isMenuInfoAnimeForceStop = !flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPageMark
 * @brief   ページマーク表示（全体）
 * @date    2015.06.01
 */
//-----------------------------------------------------------------------------
void MenuDraw::PutPageMark(void)
{
  for( u32 i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
  {
    PutPageMark( i );
  }
 }

//-----------------------------------------------------------------------------
/**
 * @func    PutPageMark
 * @brief   ページマーク表示（個別）
 * @date    2015.06.01
 *
 * @param   page  ページ
 */
//-----------------------------------------------------------------------------
void MenuDraw::PutPageMark( u32 page )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PagePartsPaneTable[page] );

  bool is_info = false;
  bool is_new  = false;

  if( IsPageInfo(page) != false )
  {
    is_info = true;
  }
  else if( IsPageNew( page ) != false )
  {
    is_new = true;
  }

  g2d->SetPaneVisible( lytwk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_005_PANE_INFOMARK, &res_id ), is_info );
  g2d->SetPaneVisible( lytwk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_005_PANE_NEW_ICON, &res_id ), is_new );

  if( is_new == false )
  {
    g2d->StopAnime( LYTID_MENU, PageNewAnimeTable[page] );
  }
  else
  {
    g2d->StartAnime( LYTID_MENU, PageNewAnimeTable[page] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPageInfo
 * @brief   ページ内にお知らせを表示している項目があるか
 * @date    2015.11.14
 *
 * @param   page  ページ
 *
 * @retval  true  = ある
 * @retval  false = ない
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsPageInfo( u32 page )
{
  for( u32 i=0; i<Savedata::FieldMenu::PAGE_ICON_MAX; i++ )
  {
    Savedata::FieldMenu::IconID id = m_saveFieldMenu->GetMenuIconID( page, i );
    if( id == Savedata::FieldMenu::ICON_ID_JOIN_FESTA && m_saveFieldMenu->IsMenuIconEnable(id) != false )
    {
      if( m_saveFieldMenu->IsIconNew(id) == false )
      {
        if( NetAppLib::JoinFesta::JoinFestaPersonalEvent::IsEventExist() )
        {
          return true;
        }
      }
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPageNew
 * @brief   ページ内にNewを表示している項目があるか
 * @date    2015.11.14
 *
 * @param   page  ページ
 *
 * @retval  true  = ある
 * @retval  false = ない
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsPageNew( u32 page )
{
  for( u32 i=0; i<Savedata::FieldMenu::PAGE_ICON_MAX; i++ )
  {
    Savedata::FieldMenu::IconID id = m_saveFieldMenu->GetMenuIconID( page, i );
    if( m_saveFieldMenu->IsMenuIconEnable( id ) != false && m_saveFieldMenu->IsIconNew( id ) != false )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    OpenMenu
 * @brief   メニューを開いたときの処理
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void MenuDraw::OpenMenu(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  SetVisibleIconName( true );
  ChangeIconButtonEnable( true );
  SetVisibleIconArea( true );
  StartIconNewAnime();
  SetIconInfoMoveAnime( true );
  PutPageMark();

  g2d->SetLayoutDrawEnable( LYTID_MENU_UPPER, true );
  g2d->SetLayoutDrawEnable( LYTID_MENU, true );
  g2d->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_ALL, true );
  // momijiで共通背景なくなったのでコメントアウト
  //g2d->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BG_00_BG_LOOP_00 );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseMenu
 * @brief   メニューを閉じたときの処理
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void MenuDraw::CloseMenu(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  SetVisibleIconName( false );
  ChangeIconButtonEnable( false );
  SetVisibleIconArea( false );
  StopIconNewAnime();
  SetIconInfoMoveAnime( false );
//  PutPageMark();

  g2d->SetLayoutDrawEnable( LYTID_MENU_UPPER, false );
  g2d->SetLayoutDrawEnable( LYTID_MENU, false );
  g2d->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_ALL, false );
  // momijiで共通背景なくなったのでコメントアウト
  //g2d->StopAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__BG_00_BG_LOOP_00 );
  SetInputListener( NULL );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartPageChangeArrowAnime
 * @brief   ページ切り替え矢印アニメ開始
 * @date    2015.06.11
 *
 * @param   mv  切り替え方向
 */
//-----------------------------------------------------------------------------
void MenuDraw::StartPageChangeArrowAnime( int mv )
{
  if( mv == 1 )
  {
    GetG2DUtil()->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_R_TOUCH_RELEASE );
  }
  else if( mv == -1 )
  {
    GetG2DUtil()->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__CURSOR_L_TOUCH_RELEASE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartPageChangeAnime
 * @brief   ページ切り替えアニメ開始
 * @date    2015.06.01
 *
 * @param   next_page   次のページ
 * @param   mv          切り替え方向
 */
//-----------------------------------------------------------------------------
void MenuDraw::StartPageChangeAnime( int next_page, int mv )
{
  int page = m_saveFieldMenu->GetMenuPage();

  if( next_page == page )
  {
    GFL_ASSERT( 0 );
    return;
  }

  PutPagePane( page, 0 );
  PutPagePane( next_page, mv );

  if( mv == 1 )
  {
    GetG2DUtil()->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_00 );
  }
  else if( mv == -1 )
  {
    GetG2DUtil()->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_01 );
  }
  else
  {
    GFL_ASSERT( 0 );
    return;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartPagePointerAnime
 * @brief   表示ページ位置アニメ開始
 * @date    2016.04.02
 */
//-----------------------------------------------------------------------------
void MenuDraw::StartPagePointerAnime(void)
{
  if( m_saveFieldMenu->GetMenuPage() != 0 )
  {
    GetG2DUtil()->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_02 );
  }
  else
  {
    GetG2DUtil()->StopAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_02 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPageChangeAnimeEnd
 * @brief   ページ切り替えアニメ終了チェック
 * @date    2015.06.01
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsPageChangeAnimeEnd(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( g2d->IsAnimeEnd( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_00 ) == false ||
      g2d->IsAnimeEnd( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_01 ) == false )
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndPageChangeAnime
 * @brief   ページ切り替えアニメ終了
 * @date    2015.06.01
 *
 * @param   page  ページ番号
 */
//-----------------------------------------------------------------------------
void MenuDraw::EndPageChangeAnime( int page )
{
  gfl2::math::VEC3 trans( 0.0f, 0.0f, 0.0f );
  GetG2DUtil()->SetPanePos( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_MENUICON, &trans );

  if( page == 0 )
  {
    PutPagePane( 0, 0 );
    PutPagePane( 1, 1 );
  }
  else
  {
    PutPagePane( 0, -1 );
    PutPagePane( 1, 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetForcePagePointerAnime
 * @brief   ページ位置アニメをセット
 * @date    2016.04.28
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetForcePagePointerAnime(void)
{
  static const u32 tbl[] =
  {
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_03,
    LA_MAINLOWER_FIELDMENU_MIN_LOW_000_WIPE_02,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  u32 page = m_saveFieldMenu->GetMenuPage() & 1;  // @note 2ページ以上を禁止
  g2d->StopAnime( LYTID_MENU, tbl[page^1] );
  g2d->StartAnime( LYTID_MENU, tbl[page] );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPagePane
 * @brief   ページペインを配置
 * @date    2015.06.02
 *
 * @param   page  ページ番号
 * @param   pos   配置位置
 */
//-----------------------------------------------------------------------------
void MenuDraw::PutPagePane( u32 page, int pos )
{
  gfl2::math::VEC3 trans( 0.0f, 0.0f, 0.0f );

  if( pos == 1 )
  {
    trans.x = 320.0f;
  }
  else if( pos == -1 )
  {
    trans.x = -320.0f;
  }
  else if( pos != 0 )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2DUtil()->SetPanePos( LYTID_MENU, PageParentPaneTable[page], &trans );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutCursor
 * @brief   カーソル配置
 * @date    2015.06.02
 *
 * @param   id  メニュー項目ID
 */
//-----------------------------------------------------------------------------
void MenuDraw::PutCursor( Savedata::FieldMenu::IconID id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytPane * pane = lytwk->GetPane(
                                lytwk->GetPartsPane(IconPartsPaneTable[id]),
                                PANENAME_FIELDMENU_BTN_LOW_001_PANE_NULL_CUR,
                                &res_id );

  m_pMenuCursor->PutNonVisible( lytwk,  pane );

  m_cursorPosIconID = id;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleCursor
 * @brief   カーソル表示切り替え
 * @date    2016.01.08
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetVisibleCursor( bool flg )
{
  m_pMenuCursor->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitCatchMenuIcon
 * @brief   メニュー項目キャッチムーブ初期化
 * @date    2015.06.04
 *
 * @param   メニュー項目ID
 */
//-----------------------------------------------------------------------------
void MenuDraw::InitCatchMenuIcon( Savedata::FieldMenu::IconID id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::math::VEC3 trans = g2d->GetPanePos( lytwk->GetPane(IconPartsPaneTable[id]) );

  m_catchIconPage = m_saveFieldMenu->GetMenuPage();
  m_catchIconID   = id;
  m_lastTouchX    = static_cast<s32>(trans.x) + DISPLAY_LOWER_WIDTH / 2;
  m_lastTouchY    = DISPLAY_LOWER_HEIGHT / 2 - static_cast<s32>(trans.y);
  m_iconPutPosID  = PUT_ID_CANCEL;

  g2d->AppendChild( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_ALL, IconPartsPaneTable[id] );

  m_isIconCatch = true;

  // メッセージ
  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_FIELDMENU_MIN_LOW_000_PANE_MESSAGE );
  gfl2::lyt::LytTextBox * text_box = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, &res_id );
  g2d->SetTextBoxPaneString( text_box, field_menu_10_01 );
  g2d->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__MESSAGE_IN );
  text_box->SetVisible( true );
  parts->SetVisible( true );

  // 影表示
  g2d->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_BG_BUTTONAREA, true );
  for( u32 i=0; i<Savedata::FieldMenu::PAGE_ICON_MAX; i++ )
  {
    g2d->SetPaneVisible( LYTID_MENU, IconCathcBgPaneTable[i], true );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCatchMenuIconID
 * @brief   キャッチムーブ中のメニュー項目IDを取得
 * @date    2015.06.04
 *
 * @return  メニュー項目ID
 */
//-----------------------------------------------------------------------------
Savedata::FieldMenu::IconID MenuDraw::GetCatchMenuIconID(void)
{
  return m_catchIconID;
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveCatchMenuIcon
 * @brief   キャッチムーブ中のメニュー項目移動
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void MenuDraw::MoveCatchMenuIcon(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytPane * pane = lytwk->GetPane( IconPartsPaneTable[m_catchIconID] );
  gfl2::math::VEC3 trans = pane->GetTranslate();
  trans.x = static_cast<f32>( m_lastTouchX - ( DISPLAY_LOWER_WIDTH / 2 ) );
  trans.y = static_cast<f32>( ( DISPLAY_LOWER_HEIGHT / 2 ) - m_lastTouchY );
  pane->SetTranslate( trans );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetCatchMenuIconStatus
 * @brief   キャッチムーブ中の状態設定
 * @date    2015.06.04
 *
 * @param   x   X座標
 * @param   y   Y座標
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetCatchMenuIconStatus( u16 x, u16 y )
{
  m_lastTouchX = x;
  m_lastTouchY = y;

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  { // 矢印
    static const gfl2::lyt::LytPaneIndex tbl[] =
    {
      PANENAME_FIELDMENU_MIN_LOW_000_PANE_WIPE_R,
      PANENAME_FIELDMENU_MIN_LOW_000_PANE_WIPE_L,
    };
    app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      if( man->IsButtonVisible( BUTTON_ID_PAGE_R+i ) == false )
      {
        continue;
      }
      if( g2d->GetHitPane( LYTID_MENU, tbl[i], x, y, false ) != false )
      {
        m_iconPutPosID = PUT_ID_ARROW;
        m_iconPutIndex = i;
        return;
      }
    }
  }

  // アイコン
  for( s32 i=0; i<static_cast<s32>(Savedata::FieldMenu::MENU_ICON_MAX); i++ )
  {
    if( i == m_catchIconID )
    {
      continue;
    }
    gfl2::lyt::LytPane * pane = lytwk->GetPane( lytwk->GetPartsPane(IconPartsPaneTable[i]), PANENAME_FIELDMENU_BTN_LOW_001_PANE_BOUND_00, &res_id );
    if( g2d->GetHitPane( LYTID_MENU, pane, x, y, false ) != false )
    {
      m_iconPutPosID = PUT_ID_ICON;
      m_iconPutIndex = i;
      return;
    }
  }

  m_iconPutPosID = PUT_ID_CANCEL;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsIconCatch
 * @brief   メニュー項目をキャッチしているか
 * @date    2015.06.04
 *
 * @retval  true  = キャッチ中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsIconCatch(void)
{
  return m_isIconCatch;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetIconCatchPage
 * @brief   キャッチ中のメニュー項目のページを取得
 * @date    2015.06.04
 *
 * @return  ページ番号
 */
//-----------------------------------------------------------------------------
u32 MenuDraw::GetIconCatchPage(void)
{
  return m_catchIconPage;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetIconCatchPutID
 * @brief   キャッチしたメニュー項目の配置先ID
 * @date    2015.06.04
 *
 * @return  配置先ID
 */
//-----------------------------------------------------------------------------
MenuDraw::IconPutID MenuDraw::GetIconCatchPutID(void)
{
  return static_cast<IconPutID>( m_iconPutPosID );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetIconCatchPutIndex
 * @brief   キャッチしたメニュー項目の配置先インデックスを取得
 * @date    2015.06.04
 *
 * @return  配置先インデックス
 */
//-----------------------------------------------------------------------------
u32 MenuDraw::GetIconCatchPutIndex(void)
{
  return m_iconPutIndex;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleMenuIconInfo
 * @brief   メニュー項目の情報表示切り替え
 * @date    2015.06.04
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetVisibleMenuIconInfo( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( int i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
  {
    for( int j=0; j<Savedata::FieldMenu::PAGE_ICON_MAX; j++ )
    {
      bool visible = flg;
      Savedata::FieldMenu::IconID id = m_saveFieldMenu->GetMenuIconID( i, j );
      if( m_saveFieldMenu->IsMenuIconEnable(id) == false )
      {
        visible = false;
      }
      g2d->SetPaneVisible(
        lytwk->GetPane( lytwk->GetPartsPane(IconPartsPaneTable[id]), PANENAME_FIELDMENU_BTN_LOW_001_PANE_NULL_VISIBLE, &res_id ),
        visible );
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    EndCatchMenuIcon
 * @brief   メニュー項目キッチムーブ終了
 * @date    2015.06.03
 */
//-----------------------------------------------------------------------------
void MenuDraw::EndCatchMenuIcon(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );

  // 取得中のアイコン
  gfl2::math::VEC3 catch_trans = g2d->GetPanePos( lytwk->GetPane(IconPartsPaneTable[m_catchIconID]) );
  // 取得した場所
  gfl2::math::VEC3 catch_pos_trans = g2d->GetPanePos( lytwk->GetPane(MenuIconParentTable[m_catchIconPage][m_saveFieldMenu->GetMenuIconPos(m_catchIconID)]) );

  if( m_iconPutPosID == PUT_ID_ICON )
  {
    // 配置場所のアイコン
    gfl2::lyt::LytPane * put_pane = lytwk->GetPane( IconPartsPaneTable[m_iconPutIndex] );
    gfl2::math::VEC3 put_trans = g2d->GetPanePos( put_pane );
    // 配置場所のアイコンの親を変更
    g2d->AppendChild( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_ALL, IconPartsPaneTable[m_iconPutIndex] );
    // 配置する場所
    u16 put_page = m_saveFieldMenu->GetMenuPage();
    u16 put_pos  = m_saveFieldMenu->GetMenuIconPos( static_cast<Savedata::FieldMenu::IconID>(m_iconPutIndex) );
    gfl2::math::VEC3 put_pos_trans = g2d->GetPanePos( lytwk->GetPane(MenuIconParentTable[put_page][put_pos]) );
    put_pane->SetTranslate( put_pos_trans );

    m_getIconCalcX = ( static_cast<f32>( put_pos_trans.x ) - catch_trans.x ) / CATCH_ICON_MOVE_COUNT;
    m_getIconCalcY = ( static_cast<f32>( put_pos_trans.y ) - catch_trans.y ) / CATCH_ICON_MOVE_COUNT;

    m_putIconCalcX = ( static_cast<f32>( catch_pos_trans.x ) - put_trans.x ) / CATCH_ICON_MOVE_COUNT;
    m_putIconCalcY = ( static_cast<f32>( catch_pos_trans.y ) - put_trans.y ) / CATCH_ICON_MOVE_COUNT;
  }
  else
  {
    m_getIconCalcX = ( static_cast<f32>( catch_pos_trans.x ) - catch_trans.x ) / CATCH_ICON_MOVE_COUNT;
    m_getIconCalcY = ( static_cast<f32>( catch_pos_trans.y ) - catch_trans.y ) / CATCH_ICON_MOVE_COUNT;
  }

  m_releaseIconMoveCount = 0;

  app::ui::UIResponder::GetButtonManager()->SetInputEnable( true );   // @note ボタンを有効にしても入力が無効になっているので問題ない

  // 影非表示
  g2d->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_BG_BUTTONAREA, false );
  for( u32 i=0; i<Savedata::FieldMenu::PAGE_ICON_MAX; i++ )
  {
    g2d->SetPaneVisible( LYTID_MENU, IconCathcBgPaneTable[i], false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ReleaseMenuIcon
 * @brief   メニュー項目リリース動作
 * @date    2015.06.03
 *
 * @retval  true  = 動作中
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool MenuDraw::ReleaseMenuIcon(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( m_releaseIconMoveCount != CATCH_ICON_MOVE_COUNT )
  {
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );

    // 取得中のアイコン
    gfl2::lyt::LytPane * pane = lytwk->GetPane( IconPartsPaneTable[m_catchIconID] );
    gfl2::math::VEC3 trans = pane->GetTranslate();
    trans.x += m_getIconCalcX;
    trans.y += m_getIconCalcY;
    pane->SetTranslate( trans );

    // 配置先のアイコン
    if( m_iconPutPosID == PUT_ID_ICON )
    {
      pane = lytwk->GetPane( IconPartsPaneTable[m_iconPutIndex] );
      trans = pane->GetTranslate();
      trans.x += m_putIconCalcX;
      trans.y += m_putIconCalcY;
      pane->SetTranslate( trans );
    }

    m_releaseIconMoveCount++;
  }
  else
  {
    g2d->StopAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__MESSAGE_IN );
    g2d->StartAnime( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__MESSAGE_OUT );
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsReleaseMenuIconEnd
 * @brief   メニュー項目のリリース動作が終了したか
 * @date    2015.06.04
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsReleaseMenuIconEnd(void)
{
  return GetG2DUtil()->IsAnimeEnd( LYTID_MENU, LA_MAINLOWER_FIELDMENU_MIN_LOW_000__MESSAGE_OUT );
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetReleaseMenuIconPos
 * @brief   メニュー項目リリース動作後の位置設定
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void MenuDraw::ResetReleaseMenuIconPos(void)
{
  for( int i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
  {
    for( int j=0; j<Savedata::FieldMenu::PAGE_ICON_MAX; j++ )
    {
      Savedata::FieldMenu::IconID id = m_saveFieldMenu->GetMenuIconID( i, j );
      if( id == m_catchIconID )
      {
        SetMenuIconPos( m_catchIconID, i, j );
      }
      else if( m_iconPutPosID == PUT_ID_ICON && id == m_iconPutIndex )
      {
        SetMenuIconPos( static_cast<Savedata::FieldMenu::IconID>(m_iconPutIndex), i, j );
      }
    }
  }

  PutPageMark();
}

//-----------------------------------------------------------------------------
/**
 * @func    ForceEndCatchMenuIcon
 * @brief   キャッチムーブを強制終了
 * @date    2015.06.23
 */
//-----------------------------------------------------------------------------
void MenuDraw::ForceEndCatchMenuIcon(void)
{
  m_isIconCatch = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DragMenuPage
 * @brief   メニューページドラッブ処理
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void MenuDraw::DragMenuPage(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::math::VEC3 pos = g2d->GetPaneLocalPos( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_MENUICON );
  pos.x = m_pSlideScroll->GetDragOffset();
  if( m_saveFieldMenu->GetMenuPage() == 0 )
  {
    if( pos.x > 0.0f )
    {
      return;
    }
  }
  else
  {
    if( pos.x < 0.0f )
    {
      return;
    }
  }
  g2d->SetPanePos( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_MENUICON, &pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    ReleaseMenuPage
 * @brief   メニューページリリース処理
 * @date    2015.06.04
 *
 * @return  ページ切り替え方向
 */
//-----------------------------------------------------------------------------
int MenuDraw::ReleaseMenuPage(void)
{
  int offs = m_pSlideScroll->GetDragOffset();
  int mv = 0;

  if( gfl2::math::Abs(offs) >= PAGE_RELEASE_SCROLL_ABS )
  {
    if( offs < 0 )
    {
      m_pageReleaseScrollSpeed = -PAGE_RELEASE_SCROLL_SPEED;
      m_pageReleaseScrollEnd = static_cast<f32>( -DISPLAY_LOWER_WIDTH );
      mv = 1;
      if( m_saveFieldMenu->GetMenuPage() == m_saveFieldMenu->GetMoveMenuPage(mv) )
      {
        m_pageReleaseScrollSpeed = PAGE_RELEASE_SCROLL_SPEED;
        m_pageReleaseScrollEnd = 0.0f;
        mv = 0;
      }
    }
    else
    {
      m_pageReleaseScrollSpeed = PAGE_RELEASE_SCROLL_SPEED;
      m_pageReleaseScrollEnd = static_cast<f32>( DISPLAY_LOWER_WIDTH );
      mv = -1;
      if( m_saveFieldMenu->GetMenuPage() == m_saveFieldMenu->GetMoveMenuPage(mv) )
      {
        m_pageReleaseScrollSpeed = -PAGE_RELEASE_SCROLL_SPEED;
        m_pageReleaseScrollEnd = 0.0f;
        mv = 0;
      }
    }
  }
  else
  {
    if( offs < 0 )
    {
      m_pageReleaseScrollSpeed = PAGE_RELEASE_SCROLL_SPEED;
    }
    else
    {
      m_pageReleaseScrollSpeed = -PAGE_RELEASE_SCROLL_SPEED;
    }
    m_pageReleaseScrollEnd = 0.0f;
  }

  return mv;
}

//-----------------------------------------------------------------------------
/**
 * @func    SlideMenuPage
 * @brief   メニューページスライド処理
 * @date    2015.06.04
 *
 * @return  ページ切り替え方向
 */
//-----------------------------------------------------------------------------
int MenuDraw::SlideMenuPage(void)
{
  App::Tool::SlideScroll::Result result = m_pSlideScroll->GetResult();
  int mv = 0;

  if( result == App::Tool::SlideScroll::RESULT_SLIDE_LEFT )
  {
    m_pageReleaseScrollSpeed = -PAGE_RELEASE_SCROLL_SPEED;
    m_pageReleaseScrollEnd = static_cast<f32>( -DISPLAY_LOWER_WIDTH );
    mv = 1;
    if( m_saveFieldMenu->GetMenuPage() == m_saveFieldMenu->GetMoveMenuPage(mv) )
    {
      m_pageReleaseScrollSpeed = PAGE_RELEASE_SCROLL_SPEED;
      m_pageReleaseScrollEnd = 0.0f;
      mv = 0;
    }
  }
  else if( result == App::Tool::SlideScroll::RESULT_SLIDE_RIGHT )
  {
    m_pageReleaseScrollSpeed = PAGE_RELEASE_SCROLL_SPEED;
    m_pageReleaseScrollEnd = static_cast<f32>( DISPLAY_LOWER_WIDTH );
    mv = -1;
    if( m_saveFieldMenu->GetMenuPage() == m_saveFieldMenu->GetMoveMenuPage(mv) )
    {
      m_pageReleaseScrollSpeed = -PAGE_RELEASE_SCROLL_SPEED;
      m_pageReleaseScrollEnd = 0.0f;
      mv = 0;
    }
  }
  else
  {
    GFL_ASSERT( 0 );
    m_pageReleaseScrollSpeed = -PAGE_RELEASE_SCROLL_SPEED;
    m_pageReleaseScrollEnd = 0.0f;
    mv = 0;
  }

  return mv;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateSlideMenuPageScroll
 * @brief   メニューページスライドスクロール更新処理
 * @date    2015.06.04
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::UpdateSlideMenuPageScroll(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::math::VEC3 pos = g2d->GetPaneLocalPos( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_MENUICON );

  if( pos.x == m_pageReleaseScrollEnd )
  {
    return true;
  }

  pos.x += m_pageReleaseScrollSpeed;

  if( m_pageReleaseScrollSpeed == -PAGE_RELEASE_SCROLL_SPEED )
  {
    if( pos.x <= m_pageReleaseScrollEnd )
    {
      pos.x = m_pageReleaseScrollEnd;
    }
  }
  else
  {
    if( pos.x >= m_pageReleaseScrollEnd )
    {
      pos.x = m_pageReleaseScrollEnd;
    }
  }

  g2d->SetPanePos( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_MENUICON, &pos );

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndSlideMenuPageScroll
 * @brief   メニューページスライドスクロール終了
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void MenuDraw::EndSlideMenuPageScroll(void)
{
  app::ui::UIResponder::GetButtonManager()->SetInputEnable( true );
}

//-----------------------------------------------------------------------------
/**
 * @func    ForceEndSlideMenuPageScroll
 * @brief   メニューページスライドスクロール強制終了
 * @date    2015.06.23
 */
//-----------------------------------------------------------------------------
void MenuDraw::ForceEndSlideMenuPageScroll(void)
{
  App::Tool::SlideScroll::Result result = m_pSlideScroll->GetResult();
  if( result == App::Tool::SlideScroll::RESULT_DRAG_START )
  {
    app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
    man->ResetHoldState();
    man->SetInputEnable( false );
    m_frameListener->OnAction( FrameListener::ACTION_MENU_PAGE_RELEASE );
  }
  else if( result == App::Tool::SlideScroll::RESULT_DRAG )
  {
    m_frameListener->OnAction( FrameListener::ACTION_MENU_PAGE_RELEASE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleIconArea
 * @brief   項目配置エリア表示切り替え
 * @date    2015.09.17
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetVisibleIconArea( bool flg )
{
  GetG2DUtil()->SetPaneVisible( LYTID_MENU, PANENAME_FIELDMENU_MIN_LOW_000_PANE_BG_BUTTONAREA, flg );
  SetVisibleIconBg( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleIconBg
 * @brief   項目配置エリア表示切り替え
 * @date    2015.09.17
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetVisibleIconBg( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( flg == false )
  {
    for( u32 i=0; i<Savedata::FieldMenu::PAGE_ICON_MAX; i++ )
    {
      g2d->SetPaneVisible( LYTID_MENU, IconCathcBgPaneTable[i], false );
    }
  }
  else
  {
    u32 page = m_saveFieldMenu->GetMenuPage();
    for( u32 i=0; i<Savedata::FieldMenu::PAGE_ICON_MAX; i++ )
    {
      Savedata::FieldMenu::IconID id = m_saveFieldMenu->GetMenuIconID( page, i );
      if( m_saveFieldMenu->IsMenuIconEnable(id) != false )
      {
        g2d->SetPaneVisible( LYTID_MENU, IconCathcBgPaneTable[i], false );
      }
      else
      {
        g2d->SetPaneVisible( LYTID_MENU, IconCathcBgPaneTable[i], true );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateTempCoreParam
 * @brief   汎用CoreParam生成
 * @date    2016.03.04
 */
//-----------------------------------------------------------------------------
void MenuDraw::CreateTempCoreParam(void)
{
  m_pTmpCoreParam = GFL_NEW(m_heap->GetDeviceHeap()) pml::pokepara::CoreParam( m_heap->GetDeviceHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteTempCoreParam
 * @brief   汎用CoreParam削除
 * @date    2016.03.04
 */
//-----------------------------------------------------------------------------
void MenuDraw::DeleteTempCoreParam(void)
{
  GFL_SAFE_DELETE( m_pTmpCoreParam );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMenuOpen
 * @brief   メニューが開いているか
 * @date    2015.05.29
 *
 * @retval  true  = 開いている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsMenuOpen(void)
{
  return *m_isMenuOpen;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsInputTrigger
 * @brief   トリガー入力か
 * @date    2015.10.10
 *
 * @retval  true  = トリガー入力
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuDraw::IsInputTrigger(void)
{
  return m_isInputTrigger;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputLock
 * @brief   入力制御フラグ設定
 * @date    2016.06.10
 *
 * @param   flg   true = 入力可, false = 入力不可
 */
//-----------------------------------------------------------------------------
void MenuDraw::SetInputLock( bool flg )
{
  m_isInputLock = flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMenuInfo
 * @brief   メニューアイコンの情報更新
 * @date    2016.05.12
 */
//-----------------------------------------------------------------------------
void MenuDraw::UpdateMenuInfo(void)
{
  // 描画不可、未初期化、メニュークローズ時
  if( m_isDrawEnable == false || m_isInitEnd == false || IsMenuOpen() == false )
  {
    return;
  }

  SetIconInfoMoveAnime( true );

  { // フェスタ数値表示
    bool is_new;
    u32 num;
    IsInfoIcon( Savedata::FieldMenu::ICON_ID_JOIN_FESTA, &is_new, NULL, &num );
    if( is_new == false && num != m_festaInfoValue )
    {
      app::util::G2DUtil * g2d = GetG2DUtil();
      gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MENU );
      gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
      gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( IconPartsPaneTable[Savedata::FieldMenu::ICON_ID_JOIN_FESTA] );
      PutMenuInfoValue( lytwk, &res_id, parts, num );
      m_festaInfoValue = num;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutMenuInfoValue
 * @brief   メニューアイコン上の数値表示
 * @date    2016.05.12
 *
 * @param   lytwk   レイアウトワーク
 * @param   res_id  マルチリソースID
 * @param   parts   メニューアイコンペイン
 * @param   num     数値
 */
//-----------------------------------------------------------------------------
void MenuDraw::PutMenuInfoValue( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID * res_id, gfl2::lyt::LytParts * parts, u32 num )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( num != 0 )
  {
    g2d->SetTextBoxPaneNumber(
      lytwk->GetTextBoxPane( parts, PANENAME_FIELDMENU_BTN_LOW_001_PANE_TEXTBOX_01, res_id ), field_menu_05_01, num, 3 );
    g2d->SetPaneVisible( lytwk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_001_PANE_INFONUMB, res_id ), true );
  }
  else
  {
    g2d->SetPaneVisible( lytwk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_001_PANE_INFONUMB, res_id ), false );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent
 * @brief   タッチパネルイベントの検知
 * @date    2015.06.02
 *
 * @param   touch_panel タッチパネル
 * @param   is_touch    true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult MenuDraw::OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch )
{
  if( m_isIconCatch != false )
  {
    if( is_touch != false )
    {
      u16 x, y;
      touch_panel->GetXY( &x, &y );
      SetCatchMenuIconStatus( x, y );
    }
    else
    {
      m_isIconCatch = false;
    }
    return DISABLE_ACCESS;
  }
  else
  {
    if( IsMenuOpen() != false && IsPageChange() != false && m_isInputLock == false )
    {
      app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
      if( man->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE )
      {
        return ENABLE_ACCESS;
      }
      App::Tool::SlideScroll::Result result = m_pSlideScroll->UpdateSlide( touch_panel, is_touch );
      if( result == App::Tool::SlideScroll::RESULT_DRAG_START )
      {
        man->ResetHoldState();
        man->SetInputEnable( false );
        m_frameListener->OnAction( FrameListener::ACTION_MENU_PAGE_DRAG_START );
        return DISABLE_ACCESS;
      }
      else if( result == App::Tool::SlideScroll::RESULT_DRAG )
      {
        m_frameListener->OnAction( FrameListener::ACTION_MENU_PAGE_DRAG );
        return DISABLE_ACCESS;
      }
      else if( result == App::Tool::SlideScroll::RESULT_RELEASE )
      {
        m_frameListener->OnAction( FrameListener::ACTION_MENU_PAGE_RELEASE );
        return DISABLE_ACCESS;
      }
      else if( result == App::Tool::SlideScroll::RESULT_SLIDE_LEFT || result == App::Tool::SlideScroll::RESULT_SLIDE_RIGHT )
      {
        m_frameListener->OnAction( FrameListener::ACTION_MENU_PAGE_SCROLL );
        return DISABLE_ACCESS;
      }
    }
  }

  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.03.05
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
app::ui::UIInputListener::ListenerResult MenuDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( m_isIconCatch != false )
  {
    return ENABLE_ACCESS;
  }

  if( button_id >= BUTTON_ID_ICON_00 && button_id <= BUTTON_ID_ICON_11 )
  {
    m_frameListener->OnAction( FrameListener::ACTION_POKELIST+(button_id-BUTTON_ID_ICON_00) );
    return DISABLE_ACCESS;
  }

  if( button_id == BUTTON_ID_PAGE_R )
  {
    m_frameListener->OnAction( FrameListener::ACTION_CHANGE_PAGE_RIGHT );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_PAGE_L )
  {
    m_frameListener->OnAction( FrameListener::ACTION_CHANGE_PAGE_LEFT );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_PAGE_00 )
  {
    m_frameListener->OnAction( FrameListener::ACTION_PAGE_00 );
    return DISABLE_ACCESS;
  }
  if( button_id == BUTTON_ID_PAGE_01 )
  {
    m_frameListener->OnAction( FrameListener::ACTION_PAGE_01 );
    return DISABLE_ACCESS;
  }

  if( button_id == BUTTON_ID_CLOSE )
  {
    m_frameListener->OnAction( FrameListener::ACTION_CLOSE_MENU );
    return DISABLE_ACCESS;
  }

  if( button_id == BUTTON_ID_REPORT )
  {
    m_frameListener->OnAction( FrameListener::ACTION_REPORT );
    return DISABLE_ACCESS;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneLongPressEvent
 * @brief   ペイン長押しイベントの検知
 * @date    2015.06.02
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
app::ui::UIInputListener::ListenerResult MenuDraw::OnLayoutPaneLongPressEvent( const u32 button_id )
{
  if( m_isIconCatch != false )
  {
    return ENABLE_ACCESS;
  }

  if( button_id >= BUTTON_ID_ICON_00 && button_id <= BUTTON_ID_ICON_11 )
  {
    m_pSlideScroll->InitUpdateWork();   // スライド処理のワークを初期化

    app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
    man->ResetHoldState();
    man->SetInputEnable( false );
    m_frameListener->OnAction( FrameListener::ACTION_CATCH_POKELIST+(button_id-BUTTON_ID_ICON_00) );
    return DISABLE_ACCESS;
  }
  return ENABLE_ACCESS;
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
app::ui::UIInputListener::ListenerResult MenuDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_isIconCatch != false )
  {
    return ENABLE_ACCESS;
  }

  if( m_isInputLock != false )
  {
    return ENABLE_ACCESS;
  }

  if( IsMenuOpen() != false )
  {
    if( button->IsTrigger(gfl2::ui::BUTTON_X) )
    {
      m_frameListener->OnAction( FrameListener::ACTION_CALL_MENU );
      return DISABLE_ACCESS;
    }
    else if( button->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      if( m_saveFieldMenu->IsMenuIconEnable( m_cursorPosIconID ) != false )
      {
        app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_ID_ICON_00+m_cursorPosIconID );
        return DISABLE_ACCESS;
      }
    }
    else if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_ID_REPORT );
      return DISABLE_ACCESS;
    }
    else if( key->IsTrigger(gfl2::ui::DIR_UP) )
    {
      m_isInputTrigger = true;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_UP );
      return DISABLE_ACCESS;
    }
    else if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
    {
      m_isInputTrigger = true;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_DOWN );
      return DISABLE_ACCESS;
    }
    else if( key->IsTrigger(gfl2::ui::DIR_LEFT) )
    {
      m_isInputTrigger = true;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_LEFT );
      return DISABLE_ACCESS;
    }
    else if( key->IsTrigger(gfl2::ui::DIR_RIGHT) )
    {
      m_isInputTrigger = true;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_RIGHT );
      return DISABLE_ACCESS;
    }
    else if( key->IsRepeat(gfl2::ui::DIR_UP) )
    {
      m_isInputTrigger = false;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_UP );
      return DISABLE_ACCESS;
    }
    else if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      m_isInputTrigger = false;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_DOWN );
      return DISABLE_ACCESS;
    }
    else if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
    {
      m_isInputTrigger = false;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_LEFT );
      return DISABLE_ACCESS;
    }
    else if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
    {
      m_isInputTrigger = false;
      m_frameListener->OnAction( FrameListener::ACTION_MENU_POS_RIGHT );
      return DISABLE_ACCESS;
    }
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

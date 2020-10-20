//=============================================================================
/**
 * @file    RideDraw.cpp
 * @brief   ライドポケモン選択画面
 * @author  Hiroyuki Nakamura
 * @date    2015.07.15
 */
//=============================================================================

// module
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#include "App/FieldMenu/include/RideDrawParam.h"
#include "App/FieldMenu/include/RideDrawListener.h"
#include "RideDraw.h"
#include "MenuDefine.h"
// niji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "PokeTool/include/PokeTool.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_ride_pokelist.h"
#include "niji_conv_header/message/msg_ride_poke_name.h"
#include "arc_index/FieldMenu.gaix"
#include "niji_conv_header/app/field_menu/RideLower.h"
#include "niji_conv_header/app/field_menu/RideLower_pane.h"
#include "niji_conv_header/app/field_menu/RideLower_anim_list.h"
#include "niji_conv_header/app/field_menu/MainUpper_pane.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//! メインシーケンス
enum
{
  MAINSEQ_START = 0,  //!< 開始
  MAINSEQ_MAIN,       //!< メイン
  MAINSEQ_END_SET,    //!< 終了設定
  MAINSEQ_END,        //!< 終了
  MAINSEQ_END_INFO,   //!< 説明を選択
};

//! リストデータ
const RideDraw::ListData RideDraw::RideList[Savedata::FieldMenu::RIDE_ID_MAX] =
{
  {
    2,  // ケンタロス
    ride_poke_name_00
  },
  {
    5,  // ムーランド
    ride_poke_name_02
  },
  {
    6,  // 新ポケ
    ride_poke_name_05
  },
  {
    1,  // カイリキー
    ride_poke_name_03
  },
  {
    3,  // ラプラス
    ride_poke_name_06
  },
  {
    4,  // サメハダー
    ride_poke_name_07
  },
  {
    0,  // リザードン
    ride_poke_name_08
  }
};

const f32 RideDraw::LIST_ITEM_PX = 26.0f;   //!< リスト開始X座標
const f32 RideDraw::LIST_ITEM_PY = 103.0f;  //!< リスト開始Y座標
const f32 RideDraw::LIST_ITEM_SX = 254.0f;  //!< リスト項目Xサイズ
const f32 RideDraw::LIST_ITEM_SY = 30.0f;   //!< リスト項目Yサイズ

//! 項目ペインテーブル
const gfl2::lyt::LytPaneIndex RideDraw::ItemPartsTable[RideDraw::LIST_PANE_MAX] =
{
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_00,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_01,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_02,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_03,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_04,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_05,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_06,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_07,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_08,
  PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_09,
};

//! ショートカット登録ペインテーブル
const gfl2::lyt::LytPaneIndex RideDraw::ShortcutCrossKeyPaneTable[Savedata::FieldMenu::KEY_ID_MAX] =
{
  PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_00,
  PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_02,
  PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_03,
  PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_01,
};

//! ショートカット登録アニメテーブル
const u32 RideDraw::ShortcutCrossKeyPokeIconAnmTable[Savedata::FieldMenu::KEY_ID_MAX] =
{
  LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_00_POKE_RIDE,
  LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_02_POKE_RIDE,
  LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_03_POKE_RIDE,
  LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_01_POKE_RIDE,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   param       外部設定パラメータ
 * @param   msg_lyt     メッセージレイアウト
 * @param   msg_res_id  メッセージレイアウトのマルチリソースID
 */
//-----------------------------------------------------------------------------
RideDraw::RideDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const RIDE_PARAM * param, gfl2::lyt::LytWk * msg_lyt, gfl2::lyt::LytMultiResID msg_res_id )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_rideParam( *param )
  , m_msgLytWk( msg_lyt )
  , m_msgLytMultiResID( msg_res_id )
  , m_saveFieldMenu( NULL )
  , m_playData( NULL )
  , m_eventWork( NULL )
  , m_listener( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pNameMsgBuff( NULL )
  , m_pMsgData( NULL )
  , m_pNameMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_cursorPos( 0 )
  , m_pCommonIcon( NULL )
  , m_rideMax( 0 )
  , m_selectRideID( Savedata::FieldMenu::RIDE_ID_NONE )
  , m_setRideID( Savedata::FieldMenu::RIDE_ID_NONE )
  , m_isSetShortcut( false )
  , m_mainSeq( MAINSEQ_START )
  , m_subSeq( 0 )
  , m_setupSeq( 0 )
  , m_isDrawEnable( true )
{
  GetGameData();
  InitListData();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
RideDraw::~RideDraw()
{
  SaveMenuCursorPos();
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize
 * @brief   初期化
 * @date    2015.07.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::Initialize(void)
{
  switch( m_setupSeq )
  {
  case 0:
    if( LoadMessage() == false )
    {
      break;
    }
    m_setupSeq++;
    /* FALL THROUGH */
  case 1:
    if( LoadLayout() == false )
    {
      break;
    }
    m_setupSeq++;
    /* FALL THROUGH */
  case 2:
    if( CreateMenuCursor() == false )
    {
      break;
    }
    CreateLayout();
    m_setupSeq++;
    /* FALL THROUGH */
  case 3:
    if( CreateCommonIcon() == false )
    {
      break;
    }
    CreateButton();
    PutList();
    PutUpperMessage( true );
    m_setupSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate
 * @brief   終了処理
 * @date    2015.07.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::Terminate(void)
{
  switch( m_setupSeq )
  {
  case 0:
    m_isDrawEnable = false;
    if( IsDrawing() != false )
    {
      return false;
    }
    DeleteCommonIcon();
    if( DeleteLayout() == false )
    {
      return false;
    }
    PutUpperMessage( false );
    m_setupSeq++;
    /* FALL THROUGH */
  case 1:
    if( DeleteMenuCursor() == false )
    {
      break;
    }
    GflHeapFreeMemory( m_pLytResBuff );
    UnloadMessage();
    m_setupSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.07.15
 */
//-----------------------------------------------------------------------------
void RideDraw::Update(void)
{
  app::ui::UIView::Update();
  m_pMenuCursor->Update();

  switch( m_mainSeq )
  {
  case MAINSEQ_START:     // 開始
    SetInputListener( this );
    GetG2DUtil()->SetLayoutDrawEnable( LYTID_LIST, true );
    m_pMenuCursor->SetVisible( true );
    m_mainSeq = MAINSEQ_MAIN;
    break;

  case MAINSEQ_MAIN:      // メイン
    break;

  case MAINSEQ_END_SET:   // 終了設定
    m_listener->SelectRide( m_selectRideID );
    m_mainSeq = MAINSEQ_END;
    break;

  case MAINSEQ_END:       // 終了
    break;

  case MAINSEQ_END_INFO:  //!< 説明を選択
    m_listener->SelectRideInfo( m_selectRideID );
    m_mainSeq = MAINSEQ_END;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画処理
 * @date    2015.07.15
 *
 * @param   displayNo   描画ディスプレイ番号
 */
//-----------------------------------------------------------------------------
void RideDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_LIST, App::FieldMenu::DRAW_PRI_APP );
    m_pMenuCursor->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER, App::FieldMenu::DRAW_PRI_APP );
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_SHORTCUT, App::FieldMenu::DRAW_PRI_APP );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ取得
 * @date    2015.08.24
 */
//-----------------------------------------------------------------------------
void RideDraw::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  m_saveFieldMenu = gd->GetFieldMenu();
  m_playData      = gd->GetFieldMenuPlayData();
  m_eventWork     = gd->GetEventWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayout
 * @brief   レイアウトリソース読み込み
 * @date    2015.08.04
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::LoadLayout(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルロード
    app::util::FileAccessor::FileLoad(
      ARCID_FIELD_MENU,
      GARC_FieldMenu_RideLower_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
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
 * @func    CreateLayout
 * @brief   レイアウト生成
 * @date    2015.07.15
 */
//-----------------------------------------------------------------------------
void RideDraw::CreateLayout(void)
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
      LYTRES_RIDELOWER_FIELDMENU_RID_LOW_000_BFLYT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
    {
      0,
      LYTRES_RIDELOWER_FIELDMENU_RID_LOW_001_BFLYT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
  };

  m_pMsgData     = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( m_pMsgBuff, m_heap->GetDeviceHeap() );
  m_pNameMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( m_pNameMsgBuff, m_heap->GetDeviceHeap() );
  m_pWordSet     = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

  SetDefaultString();
  InitShortcutCrossKeyIcon();

  CreateMenuCursorLayout();

  GetG2DUtil()->StartAnime( LYTID_LIST, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BG_00_BG_LOOP_00 );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteLayout
 * @brief   レイアウト削除
 * @date    2015.07.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::DeleteLayout(void)
{
  if( DeleteMenuCursorLayout() == false )
  {
    return false;
  }
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pNameMsgData );
  GFL_SAFE_DELETE( m_pMsgData );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.08.04
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::LoadMessage(void)
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
      GARC_message_ride_pokelist_DAT,
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
      GARC_message_ride_poke_name_DAT,
      &m_pNameMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileLoad( &m_pNameMsgBuff ) == false )
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
 * @brief   メッセージ削除
 * @date    2015.08.04
 */
//-----------------------------------------------------------------------------
void RideDraw::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pNameMsgBuff );
  GflHeapFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   メニューカーソル生成
 * @date    2015.08.04
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::CreateMenuCursor(void)
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
 * @date    2015.08.04
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::DeleteMenuCursor(void)
{
  if( m_pMenuCursor == NULL )
  {
    return true;
  }
  if( m_pMenuCursor->IsModuleFree() == false )
  {
    return false;
  }
  m_pMenuCursor->DeleteResource();
  GFL_SAFE_DELETE( m_pMenuCursor );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMenuCursor
 * @brief   メニューカーソル初期化
 * @date    2015.08.06
 */
//-----------------------------------------------------------------------------
void RideDraw::InitMenuCursor(void)
{
  SetMenuCursor( m_playData->GetRideCursorPos() );
};

//-----------------------------------------------------------------------------
/**
 * @func    SaveMenuCursorPos
 * @brief   メニューカーソル位置を記憶
 * @date    2015.08.06
 */
//-----------------------------------------------------------------------------
void RideDraw::SaveMenuCursorPos(void)
{
  m_playData->SetRideCursorPos( m_cursorPos );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuCursor
 * @brief   メニューカーソルセット
 * @date    2015.08.06
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void RideDraw::SetMenuCursor( s32 pos )
{
  m_cursorPos = pos;
  MoveMenuCursor( 0, false );
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveMenuCursor
 * @brief   メニューカーソル移動
 * @date    2015.08.06
 *
 * @param   mv      移動方向
 * @param   is_trg  true = トリガー入力
 *
 * @retval  true  = 移動した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::MoveMenuCursor( s32 mv, bool is_trg )
{
  s32 tmp = m_cursorPos;

  m_cursorPos += mv;
  if( m_cursorPos < 0 )
  {
    if( is_trg != false )
    {
      m_cursorPos = m_rideMax - 1;
    }
    else
    {
      m_cursorPos = 0;
    }
  }
  else if( m_cursorPos >= m_rideMax )
  {
    if( is_trg != false )
    {
      m_cursorPos = 0;
    }
    else
    {
      m_cursorPos = m_rideMax - 1;
    }
  }

  if( mv != 0 && m_cursorPos == tmp )
  {
    return false;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LIST );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  m_pMenuCursor->PutNonVisible(
    lytwk,
    lytwk->GetPane( lytwk->GetPartsPane( ItemPartsTable[m_cursorPos] ), PANENAME_FIELDMENU_BTN_LOW_007_PANE_NULL_CUR, &res_id ) );

  PutItemInfo( m_cursorPos );

  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursorLayout
 * @brief   メニューカーソル用レイアウト作成
 * @date    2015.08.04
 */
//-----------------------------------------------------------------------------
void RideDraw::CreateMenuCursorLayout(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  m_pMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  m_pMenuCursor->SetVisible( false );
  m_pMenuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursorLayout
 * @brief   メニューカーソル用レイアウト削除
 * @date    2015.08.04
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::DeleteMenuCursorLayout(void)
{
  return m_pMenuCursor->DeleteLayoutWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateCommonIcon
 * @brief   汎用アイコン生成
 * @date    2016.01.16
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RideDraw::CreateCommonIcon(void)
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
 * @brief   汎用アイコン削除
 * @date    2016.01.16
 */
//-----------------------------------------------------------------------------
void RideDraw::DeleteCommonIcon(void)
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
 * @brief   Newアイコンのテクスチャを切り替え
 * @date    2016.01.16
 */
//-----------------------------------------------------------------------------
void RideDraw::ChangeNewIconTexture(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LIST );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<LIST_PANE_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( lytwk->GetPartsPane(ItemPartsTable[i]), PANENAME_FIELDMENU_BTN_LOW_007_PANE_NEWICON, &res_id );
    m_pCommonIcon->ReplacePaneTextureByNewIcon( lytwk->GetPicturePane(parts,PANENAME_COMMON_ICN_013_PANE_NEW_ICON_01,&res_id) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン作成
 * @date    2015.08.06
 */
//-----------------------------------------------------------------------------
void RideDraw::CreateButton(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  app::ui::ButtonInfoLytWk * info = GFL_NEW_LOW_ARRAY(m_heap->GetDeviceHeap()) app::ui::ButtonInfoLytWk[BUTTON_MAX];

  CreateListButton( &info[0] );
  CreateKeyButton( &info[BUTTON_KEY_UP] );

  app::ui::UIResponder::CreateButtonManager( m_heap, info, BUTTON_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  for( u32 i=0; i<BUTTON_MAX; i++ )
  {
    if( i == BUTTON_EXIT || i == BUTTON_KEY_CANCEL )
    {
      man->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else
    {
      man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateListButton
 * @brief   通常ボタンの情報をセット
 * @date    2015.08.06
 *
 * @param   info  情報格納場所
 */
//-----------------------------------------------------------------------------
void RideDraw::CreateListButton( app::ui::ButtonInfoLytWk * info )
{
  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BUTTON_ITEM_00, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00_PASSIVE,
    },
    {
      BUTTON_ITEM_01, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01_PASSIVE,
    },
    {
      BUTTON_ITEM_02, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02_PASSIVE,
    },
    {
      BUTTON_ITEM_03, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03_PASSIVE,
    },
    {
      BUTTON_ITEM_04, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04_PASSIVE,
    },
    {
      BUTTON_ITEM_05, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05_PASSIVE,
    },
    {
      BUTTON_ITEM_06, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06_PASSIVE,
    },
    {
      BUTTON_ITEM_07, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07_PASSIVE,
    },
    {
      BUTTON_ITEM_08, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08_PASSIVE,
    },
    {
      BUTTON_ITEM_09, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09_PASSIVE,
    },

    {
      BUTTON_SHORTCUT_00, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_01, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_02, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_03, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_04, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_05, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_06, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_07, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_08, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_PASSIVE,
    },
    {
      BUTTON_SHORTCUT_09, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_PASSIVE,
    },

    {
      BUTTON_EXIT, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BACKBUTTON_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BACKBUTTON_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BACKBUTTON_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BACKBUTTON_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_00,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_01,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_02,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_03,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_04,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_05,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_06,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_07,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_08,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_09,

    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_00,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_01,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_02,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_03,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_04,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_05,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_06,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_07,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_08,
    PANENAME_FIELDMENU_RID_LOW_000_PANE_BUTTON_09,

    PANENAME_FIELDMENU_RID_LOW_000_PANE_BACKBUTTON,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_007_PANE_BOUND_00,

    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,
    PANENAME_FIELDMENU_BTN_LOW_010_PANE_BOUND_01,

    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LIST );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<GFL_NELEMS(parts_tbl); i++ )
  {
    gfl2::lyt::LytParts * parts;
    if( i >= BUTTON_SHORTCUT_00 && i <= BUTTON_SHORTCUT_09 )
    {
      parts = lytwk->GetPartsPane( lytwk->GetPartsPane(parts_tbl[i]), PANENAME_FIELDMENU_BTN_LOW_007_PANE_PARTS_00, &res_id );
    }
    else
    {
      parts = lytwk->GetPartsPane( parts_tbl[i] );
    }
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound_tbl[i], &res_id );
    info[i].lyt_wk       = lytwk;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateKeyButton
 * @brief   ショートカットボタンの情報をセット
 * @date    2015.08.06
 *
 * @param   info  情報格納場所
 */
//-----------------------------------------------------------------------------
void RideDraw::CreateKeyButton( app::ui::ButtonInfoLytWk * info )
{
  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BUTTON_KEY_UP, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_00_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_00_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_00_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_00_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_00_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_00_PASSIVE,
    },
    {
      BUTTON_KEY_DOWN, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_02_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_02_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_02_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_02_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_02_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_02_PASSIVE,
    },
    {
      BUTTON_KEY_LEFT, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_03_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_03_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_03_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_03_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_03_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_03_PASSIVE,
    },
    {
      BUTTON_KEY_RIGHT, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_01_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_01_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_01_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_01_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_01_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BUTTON_01_PASSIVE,
    },

    {
      BUTTON_KEY_CANCEL, NULL, NULL, NULL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BACKBUTTON_TOUCH,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BACKBUTTON_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BACKBUTTON_CANSEL,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BACKBUTTON_TOUCH_RELEASE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BACKBUTTON_ACTIVE,
      LA_RIDELOWER_FIELDMENU_RID_LOW_001__BACKBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_00,
    PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_02,
    PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_03,
    PANENAME_FIELDMENU_RID_LOW_001_PANE_BUTTON_01,
    PANENAME_FIELDMENU_RID_LOW_001_PANE_BACKBUTTON,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_FIELDMENU_BTN_LOW_009_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_009_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_009_PANE_BOUND_00,
    PANENAME_FIELDMENU_BTN_LOW_009_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_SHORTCUT );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<GFL_NELEMS(parts_tbl); i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound_tbl[i], &res_id );
    info[i].lyt_wk       = lytwk;
  }
}

/*
void MenuCustomizeDraw::SetVisibleButton( bool flg )
{
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( flg == false )
  {
    for( u32 i=0; i<BUTTON_MAX; i++ )
    {
      man->SetButtonPassive( i, false );
      man->SetButtonInvisible( i );
    }
  }
  else
  {
    for( u32 i=0; i<BUTTON_MAX; i++ )
    {
      man->SetButtonActive( i, false );
      man->SetButtonVisible( i, false );
    }
  }
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    InitListData
 * @brief   リストデータ初期化
 * @date    2015.08.06
 */
//-----------------------------------------------------------------------------
void RideDraw::InitListData(void)
{
  static const u32 tbl[] =
  {
    SYS_FLAG_KENTAROSU_RIDE_OPEN,
    SYS_FLAG_MUURANDO_RIDE_OPEN,
    SYS_FLAG_ROBA2_RIDE_OPEN,
    SYS_FLAG_KAIRIKII_RIDE_OPEN,
    SYS_FLAG_RAPURASU_RIDE_OPEN,
    SYS_FLAG_SAMEHADAA_RIDE_OPEN,
    SYS_FLAG_RIZAADON_RIDE_OPEN,
  };

  m_rideMax = 0;

  for( u32 i=0; i<Savedata::FieldMenu::RIDE_ID_MAX; i++ )
  {
    if( m_eventWork->CheckEventFlag(tbl[i]) != false )
    {
      m_listData[m_rideMax] = static_cast<Savedata::FieldMenu::RideID>( i );
      m_rideMax++;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDefaultString
 * @brief   デフォルト文字列書き込み
 * @date    2015.08.04
 */
//-----------------------------------------------------------------------------
void RideDraw::SetDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  {
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_SHORTCUT );

    g2d->SetTextBoxPaneString(
      lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_FIELDMENU_RID_LOW_001_PANE_MESSAGE),PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00,&res_id),
      static_cast<u32>(ride_pokelist_01_02) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutList
 * @brief   リスト配置
 * @date    2015.08.06
 */
//-----------------------------------------------------------------------------
void RideDraw::PutList(void)
{
  static const u32 anm[LIST_PANE_MAX][2] =
  { // Newアイコン, ポケモン
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08_POKE_RIDE },
    { LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__NEWICON_LOOP, LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09_POKE_RIDE },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LIST );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  for( u32 i=0; i<m_rideMax; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ItemPartsTable[i] );

    { // ボタンの座標変更
      gfl2::math::VEC3 pos = parts->GetTranslate();
      pos.y = LIST_ITEM_PY - LIST_ITEM_SY * i;
      parts->SetTranslate( pos );
    }

    // 使用できるか
    if( m_rideParam.check[m_listData[i]] != Field::RCR_OK )
    {
      man->SetButtonPassive( BUTTON_ITEM_00+i );
    }

    // 能力名
    m_pNameMsgData->GetString( RideList[ m_listData[i] ].text, *str );
    print::SetStringForLytTextBox(
      lytwk->GetTextBoxPane( parts, PANENAME_FIELDMENU_BTN_LOW_007_PANE_TEXTBOX_00, &res_id ),
      str );

    // ポケモンアイコン
    g2d->StartAnimeFrame( LYTID_LIST, anm[i][1], RideList[ m_listData[i] ].anm_frame, false, false );

    { // Newアイコン
      gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_007_PANE_NEWICON, &res_id );
      pane->SetVisible( m_saveFieldMenu->IsRideNewFlag(m_listData[i]) );
      g2d->StartAnime( LYTID_LIST, anm[i][0] );
    }

    // ショートカットボタン設定
    SetShortcutKeyAnime( i, m_saveFieldMenu->GetRideShortcutKey(m_listData[i]) );
  }

  // 未使用のボタンを非表示にする
  for( u32 i=m_rideMax; i<LIST_PANE_MAX; i++ )
  {
    man->SetButtonInvisible( BUTTON_ITEM_00+i );
    man->SetButtonInvisible( BUTTON_SHORTCUT_00+i );
  }

  InitMenuCursor();
}

//-----------------------------------------------------------------------------
/**
 * @func    PutItemInfo
 * @brief   項目情報表示
 * @date    2015.10.10
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void RideDraw::PutItemInfo( u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LIST );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  u32 text = ride_pokelist_01_04;

  // 使用できる
  if( m_rideParam.check[m_listData[pos]] == Field::RCR_OK )
  {
    if( m_rideParam.on_id == m_listData[pos] )
    {
      text = ride_pokelist_01_03;
    }
    else
    {
      static const u32 tbl[] =
      {
        ride_pokelist_info_00,
        ride_pokelist_info_02,
        ride_pokelist_info_05,
        ride_pokelist_info_03,
        ride_pokelist_info_06,
        ride_pokelist_info_07,
        ride_pokelist_info_08,
      };
      text = tbl[ m_listData[pos] ];
    }
  }
  // 使用できない
  else
  {
    if( m_rideParam.on_id == m_listData[pos] )
    {
      text = Field::EventPokemonRideOff::GetRcrMessageId( m_rideParam.check[m_listData[pos]] );
    }
    else
    {
      text = Field::EventPokemonRideOn::GetRcrMessageId( m_rideParam.check[m_listData[pos]] );
    }
  }

  g2d->SetTextBoxPaneString(
    lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_FIELDMENU_RID_LOW_000_PANE_MESSAGE),PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00,&res_id), text );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleShortcut
 * @brief   ショートカット登録レイアウト表示切り替え
 * @date    2015.08.06
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void RideDraw::SetVisibleShortcut( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( flg == false )
  {
    // レイアウト表示切り替え
    g2d->SetLayoutDrawEnable( LYTID_SHORTCUT, false );
    m_pMenuCursor->SetVisible( true );
    // ボタン切り替え
    // リスト用
    for( u32 i=0; i<m_rideMax; i++ )
    {
      man->SetButtonInputEnable( BUTTON_ITEM_00+i, true );
      man->SetButtonInputEnable( BUTTON_SHORTCUT_00+i, true );
    }
    man->SetButtonInputEnable( BUTTON_EXIT, true );
    // ショートカット用
    for( u32 i=BUTTON_KEY_UP; i<=BUTTON_KEY_CANCEL; i++ )
    {
      man->SetButtonInputEnable( i, false );
    }
  }
  else
  {
    // レイアウト表示切り替え
    g2d->SetLayoutDrawEnable( LYTID_SHORTCUT, true );
    m_pMenuCursor->SetVisible( false );
    // ボタン切り替え
    for( u32 i=BUTTON_ITEM_00; i<=BUTTON_EXIT; i++ )
    {
      man->SetButtonInputEnable( i, false );
    }
    for( u32 i=BUTTON_KEY_UP; i<=BUTTON_KEY_CANCEL; i++ )
    {
      man->SetButtonInputEnable( i, true );
    }
  }

  m_isSetShortcut = flg;

  SetStickEmulationMode( !flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSelectListener
 * @brief   リスナーをセット
 * @date    2015.07.15
 *
 * @param   listener リスナー
 */
//-----------------------------------------------------------------------------
void RideDraw::SetSelectListener( RideDrawListener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetShortcutKey
 * @brief   ショートカット登録
 * @date    2015.08.06
 *
 * @param   key_id  登録先のID
 */
//-----------------------------------------------------------------------------
void RideDraw::SetShortcutKey( Savedata::FieldMenu::RideKeyID key_id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_SHORTCUT );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  Savedata::FieldMenu::RideID old = m_saveFieldMenu->GetRideShortcut( key_id );
  // 同じ位置
  if( old == m_setRideID )
  {
    gfl2::lyt::LytPicture * pane = lytwk->GetPicturePane( lytwk->GetPartsPane(ShortcutCrossKeyPaneTable[key_id]), PANENAME_FIELDMENU_BTN_LOW_009_PANE_PICON, &res_id );
    pane->SetVisible( false );
    SetShortcutKeyAnime( GetShortcutButtonIDIndex(m_setRideID), Savedata::FieldMenu::KEY_ID_MAX );
    m_saveFieldMenu->SetRideShortcut( key_id, Savedata::FieldMenu::RIDE_ID_NONE );
    return;
  }

  // 指定位置にあるデータをいれかえ
  Savedata::FieldMenu::RideKeyID chg = m_saveFieldMenu->GetRideShortcutKey( m_setRideID );
  if( chg != Savedata::FieldMenu::KEY_ID_MAX )
  {
    m_saveFieldMenu->SetRideShortcut( chg, old );
  }

  // 配置
  m_saveFieldMenu->SetRideShortcut( key_id, m_setRideID );

  // ボタン変更
  // 配置先
  // ショートカット
  SetShortcutKeyAnime( GetShortcutButtonIDIndex(m_setRideID), key_id );
  { // ポケモンアイコン
    gfl2::lyt::LytPicture * pane = lytwk->GetPicturePane( lytwk->GetPartsPane(ShortcutCrossKeyPaneTable[key_id]), PANENAME_FIELDMENU_BTN_LOW_009_PANE_PICON, &res_id );
    pane->SetVisible( true );
    g2d->StartAnimeFrame( LYTID_SHORTCUT, ShortcutCrossKeyPokeIconAnmTable[key_id], RideList[m_setRideID].anm_frame, false, false );
  }
  // 元からあったもの
  if( old != Savedata::FieldMenu::RIDE_ID_NONE )
  {
    // ショートカット
    SetShortcutKeyAnime( GetShortcutButtonIDIndex(old), chg );
    // ポケモンアイコン
    if( chg != Savedata::FieldMenu::KEY_ID_MAX )
    {
      gfl2::lyt::LytPicture * pane = lytwk->GetPicturePane( lytwk->GetPartsPane(ShortcutCrossKeyPaneTable[chg]), PANENAME_FIELDMENU_BTN_LOW_009_PANE_PICON, &res_id );
      pane->SetVisible( true );
      g2d->StartAnimeFrame( LYTID_SHORTCUT, ShortcutCrossKeyPokeIconAnmTable[chg], RideList[old].anm_frame, false, false );
    }
  }
  else
  {
    if( chg != Savedata::FieldMenu::KEY_ID_MAX )
    {
      gfl2::lyt::LytPicture * pane = lytwk->GetPicturePane( lytwk->GetPartsPane(ShortcutCrossKeyPaneTable[chg]), PANENAME_FIELDMENU_BTN_LOW_009_PANE_PICON, &res_id );
      pane->SetVisible( false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetShortcutKeyAnime
 * @brief   項目上のショートカットボタンのアニメを変更
 * @date    2015.08.06
 *
 * @param   button_no   ボタン番号
 * @param   key_id      登録先のID
 */
//-----------------------------------------------------------------------------
void RideDraw::SetShortcutKeyAnime( u32 button_no, Savedata::FieldMenu::RideKeyID key_id )
{
  static const u32 tbl[LIST_PANE_MAX][SHORTCUT_ANIME_MAX] =
  {
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_00__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_01__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_02__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_03__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_04__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_05__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_06__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_07__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_08__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
    {
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_BUTTONICON_CURSORKEY_UP,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_BUTTONICON_CURSORKEY_DOWN,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_BUTTONICON_CURSORKEY_LEFT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_BUTTONICON_CURSORKEY_RIGHT,
      LA_RIDELOWER_FIELDMENU_RID_LOW_000__BUTTON_09__PARTS_00_BUTTONICON_CURSORKEY_COLOR01,
    },
  };

  GetG2DUtil()->StartAnime( LYTID_LIST, tbl[button_no][key_id] );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetShortcutButtonIDIndex
 * @brief   項目のボタンインデックスを取得
 * @date    2015.08.06
 *
 * @param   id  ライドID
 *
 * @return  ライドが表示されているボタンのインデックス
 */
//-----------------------------------------------------------------------------
u32 RideDraw::GetShortcutButtonIDIndex( Savedata::FieldMenu::RideID id )
{
  for( u32 i=0; i<m_rideMax; i++ )
  {
    if( m_listData[i] == id )
    {
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitShortcutCrossKeyIcon
 * @brief   ショートカット登録レイアウトのアイコン表示を初期化
 * @date    2015.08.06
 */
//-----------------------------------------------------------------------------
void RideDraw::InitShortcutCrossKeyIcon(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_SHORTCUT );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<Savedata::FieldMenu::KEY_ID_MAX; i++ )
  {
    gfl2::lyt::LytPicture * pane = lytwk->GetPicturePane( lytwk->GetPartsPane(ShortcutCrossKeyPaneTable[i]), PANENAME_FIELDMENU_BTN_LOW_009_PANE_PICON, &res_id );
    Savedata::FieldMenu::RideID id = m_saveFieldMenu->GetRideShortcut( static_cast<Savedata::FieldMenu::RideKeyID>(i) );
    if( id == Savedata::FieldMenu::RIDE_ID_NONE )
    {
      pane->SetVisible( false );
    }
    else
    {
      g2d->StartAnimeFrame( LYTID_SHORTCUT, ShortcutCrossKeyPokeIconAnmTable[i], RideList[id].anm_frame, false, false );
      pane->SetVisible( true );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSelect
 * @brief   選択したライドIDをセット
 * @date    2015.08.24
 *
 * @param   id  ライドID
 */
//-----------------------------------------------------------------------------
void RideDraw::SetSelect( Savedata::FieldMenu::RideID id )
{
  SetInputEnabled( false );
  m_selectRideID = id;
  m_mainSeq = MAINSEQ_END_SET;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutUpperMessage
 * @brief   上画面に情報表示切り替え
 * @date    2016.03.28
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void RideDraw::PutUpperMessage( bool flg )
{
  if( m_msgLytWk == NULL )
  {
    return;
  }

  gfl2::lyt::LytWindow * win  = m_msgLytWk->GetWindowPane( PANENAME_FIELDMENU_MIN_UPP_000_PANE_BG_01 );
  gfl2::lyt::LytTextBox * text = m_msgLytWk->GetTextBoxPane( PANENAME_FIELDMENU_MIN_UPP_000_PANE_TEXTBOX_00 );

  if( flg != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
    g2d->GetString( str, ride_pokelist_info_01 );
    print::SetStringForLytTextBox( text, str );
  }

  text->SetVisible( flg );
  win->SetVisible( flg );
}


//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneStartingAnimeEvent
 * @brief   ペイン選択アニメーションの開始を検知
 * @date    2015.12.16
 *
 * @param   button_id   通知されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void RideDraw::OnLayoutPaneStartingAnimeEvent( const u32 button_id )
{
  if( button_id == BUTTON_KEY_UP ||
      button_id == BUTTON_KEY_DOWN ||
      button_id == BUTTON_KEY_LEFT ||
      button_id == BUTTON_KEY_RIGHT )
  {
    SetShortcutKey( static_cast<Savedata::FieldMenu::RideKeyID>(button_id-BUTTON_KEY_UP) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.08.06
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
app::ui::UIInputListener::ListenerResult RideDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( m_mainSeq != MAINSEQ_MAIN )
  {
    return ENABLE_ACCESS;
  }

  switch( button_id )
  {
  case BUTTON_ITEM_00:
  case BUTTON_ITEM_01:
  case BUTTON_ITEM_02:
  case BUTTON_ITEM_03:
  case BUTTON_ITEM_04:
  case BUTTON_ITEM_05:
  case BUTTON_ITEM_06:
  case BUTTON_ITEM_07:
  case BUTTON_ITEM_08:
  case BUTTON_ITEM_09:
    SetSelect( m_listData[button_id-BUTTON_ITEM_00] );
    break;

  case BUTTON_SHORTCUT_00:
  case BUTTON_SHORTCUT_01:
  case BUTTON_SHORTCUT_02:
  case BUTTON_SHORTCUT_03:
  case BUTTON_SHORTCUT_04:
  case BUTTON_SHORTCUT_05:
  case BUTTON_SHORTCUT_06:
  case BUTTON_SHORTCUT_07:
  case BUTTON_SHORTCUT_08:
  case BUTTON_SHORTCUT_09:
    m_setRideID = m_listData[button_id-BUTTON_SHORTCUT_00];
    SetVisibleShortcut( true );
    break;

  case BUTTON_EXIT:
    SetSelect( Savedata::FieldMenu::RIDE_ID_NONE );
    break;

  case BUTTON_KEY_UP:
  case BUTTON_KEY_DOWN:
  case BUTTON_KEY_LEFT:
  case BUTTON_KEY_RIGHT:
//    SetShortcutKey( static_cast<Savedata::FieldMenu::RideKeyID>(button_id-BUTTON_KEY_UP) );
    SetVisibleShortcut( false );
    break;

  case BUTTON_KEY_CANCEL:
    SetVisibleShortcut( false );
    break;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ボタンタッチを検知
 * @date    2015.08.06
 *
 * @param   button_id   通知されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void RideDraw::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
  if( m_mainSeq != MAINSEQ_MAIN )
  {
    return;
  }

  if( button_id >= BUTTON_ITEM_00 && button_id <= BUTTON_ITEM_09 )
  {
    SetMenuCursor( button_id-BUTTON_ITEM_00 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.07.15
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
app::ui::UIInputListener::ListenerResult RideDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_mainSeq != MAINSEQ_MAIN )
  {
    return ENABLE_ACCESS;
  }

  // リスト選択時
  if( m_isSetShortcut == false )
  {
    if( key->IsTrigger(gfl2::ui::DIR_UP) )
    {
      if( MoveMenuCursor( -1, true ) != false )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
      return DISABLE_ACCESS;
    }
    if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
    {
      if( MoveMenuCursor( 1, true ) != false )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_UP) )
    {
      if( MoveMenuCursor( -1, false ) != false )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      if( MoveMenuCursor( 1, false ) != false )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
      return DISABLE_ACCESS;
    }

    if( button->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_Y) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_ITEM_00+m_cursorPos );
      return DISABLE_ACCESS;
    }
    if( button->IsTrigger(gfl2::ui::BUTTON_X) )
    {
      Sound::PlaySE( SEQ_SE_DECIDE1 );
      SetInputEnabled( false );
      m_selectRideID = m_listData[m_cursorPos];
      m_mainSeq = MAINSEQ_END_INFO;
      return DISABLE_ACCESS;
    }
    if( button->IsTrigger(gfl2::ui::BUTTON_B) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_EXIT );
      return DISABLE_ACCESS;
    }
  }
  // ショートカット登録時
  else
  {
    if( key->IsTrigger(gfl2::ui::DIR_UP) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_KEY_UP );
      return DISABLE_ACCESS;
    }
    if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_KEY_DOWN );
      return DISABLE_ACCESS;
    }
    if( key->IsTrigger(gfl2::ui::DIR_LEFT) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_KEY_LEFT );
      return DISABLE_ACCESS;
    }
    if( key->IsTrigger(gfl2::ui::DIR_RIGHT) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_KEY_RIGHT );
      return DISABLE_ACCESS;
    }

    if( button->IsTrigger(gfl2::ui::BUTTON_B) )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_KEY_CANCEL );
      return DISABLE_ACCESS;
    }
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

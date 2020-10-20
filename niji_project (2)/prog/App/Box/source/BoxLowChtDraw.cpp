//=============================================================================
/**
 * @file    BoxLowChtDraw.cpp
 * @brief   チャット下画面描画
 * @author  ohno
 * @data    February 17, 2015
 *
 * @author	Hiroyuki Nakamura
 * @data	  2015.10.28
 */
//=============================================================================

// module
#include "BoxLowChtDraw.h"
// niji
#include "Sound/include/Sound.h"
// resource
#include "arc_index/boxLyt.gaix"
#include "niji_conv_header/app/box/res2d/chtlow.h"
#include "niji_conv_header/app/box/res2d/chtlow_pane.h"
#include "niji_conv_header/app/box/res2d/chtlow_anim_list.h"
#include "niji_conv_header/message/msg_box.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

const f32 BoxLowChtDraw::LIST_ITEM_PY = 92.0f;    //!< ペインリストの表示開始Y座標
const f32 BoxLowChtDraw::LIST_ITEM_SX = 294.0f;   //!< ペインリストの項目Xサイズ
const f32 BoxLowChtDraw::LIST_ITEM_SY = 52.0f;    //!< ペインリストの項目Yサイズ


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap                  ヒープ
 * @param   pAppRenderingManager  描画マネージャ
 */
//-----------------------------------------------------------------------------
BoxLowChtDraw::BoxLowChtDraw( app::util::Heap * heap, app::util::AppRenderingManager * pAppRenderingManager )
  : app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
  , mpAppHeap( heap )
  , mpAppRenderingManager( pAppRenderingManager )
  , m_uiListener( NULL )
  , mpWordSet( NULL )
  , mpMenuCursor( NULL )
  , mpPaneListView( NULL )
  , mpPaneListData( NULL )
  , m_isDrawEnable( true )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BoxLowChtDraw::~BoxLowChtDraw( void )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化
 *
 * @param   pAppLytBuff     レイアウトリソース
 * @param   pMsgData        メッセージデータ
 * @param   pMenuCursorBuf  メッセージカーソル
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::Init( void * pAppLytBuff, gfl2::str::MsgData * pMsgData, void * pMenuCursorBuf )
{
//  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // @note 未使用
//  CreateGraphicsSystem( mpAppHeap, false );               // @note 未使用

  InitializeText( mpAppHeap );

  Initialize2D( mpAppHeap, pAppLytBuff, pMsgData );

  CreateMenuCursor( pMenuCursorBuf );

  CreatePaneList();

  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxLowChtDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  DeletePaneList();
  if( DeleteMenuCursor() == false )
  {
    return false;
  }
  Delete2D();
  FinalizeText();
//  DeleteGraphicsSystem();   // @note 未使用
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetUIListener
 * @brief   UIイベント用リスナーの登録
 *
 * @param   listener  リスナー
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::SetUIListener( BoxLowChtDrawListener * listener )
{
  m_uiListener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::Update(void)
{
  app::ui::UIView::Update();
  if( mpMenuCursor != NULL )
  {
    mpMenuCursor->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 *
 * @param   no  描画面
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_isDrawEnable != false )
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, LYT_WORK_NORMAL );
    if( mpMenuCursor != NULL )
    {
      mpMenuCursor->Draw( mpAppRenderingManager, no, gfl2::lyt::DISPLAY_LOWER );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsExit
 * @brief   破棄チェック
 *
 * @retval  true  = 破棄完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
/*
bool BoxLowChtDraw::IsExit( void )
{
  return true;
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連作成
 *
 * @param   heap          ヒープ
 * @param   pAppLytBuff   レイアウトリソース
 * @param   pMsgData      メッセージデータ
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::Initialize2D( app::util::Heap * heap, void * pAppLytBuff, gfl2::str::MsgData * pMsgData )
{
  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] =
  {
    { pAppLytBuff, 0, app::util::G2DUtil::ATTACH_ROOT },
  };

  app::util::G2DUtil::LytwkData setupData[] =
  {
    {
      0,
      LYTRES_CHTLOW_CHAT_SLC_LOW_000_BFLYT,
      LA_CHTLOW_CHAT_SLC_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  Create2D(
    heap,
    NULL,
    LYT_WORK_MAX,
    resTbl, GFL_NELEMS(resTbl),
    setupData, GFL_NELEMS(setupData),
    pMsgData,
    mpWordSet );

  GetG2DUtil()->StartAnime( LYT_WORK_NORMAL, LA_CHTLOW_CHAT_SLC_LOW_000__BG_00_BG_LOOP_00 );

  MyCreateButtonManager();
}

//-----------------------------------------------------------------------------
/**
 * @func    InitializeText
 * @brief   テキスト関連生成
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::InitializeText( app::util::Heap * heap )
{
  mpWordSet = GFL_NEW( heap->GetDeviceHeap() ) print::WordSet( heap->GetDeviceHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    FinalizeText
 * @brief   テキスト関連削除
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::FinalizeText(void)
{
  GFL_SAFE_DELETE( mpWordSet );
}

//-----------------------------------------------------------------------------
/**
 * @func    MyCreateButtonManager
 * @brief   ボタンマネージャの生成
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::MyCreateButtonManager(void)
{
  static const app::ui::ButtonInfoEx c_DefInfo[] =
  {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_CHTLOW_CHAT_SLC_LOW_000__BTN_BACK_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__BTN_BACK_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__BTN_BACK_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__BTN_BACK_ACTIVE,
      LA_CHTLOW_CHAT_SLC_LOW_000__BTN_BACK_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex paneNo[] =
  {
    PANENAME_CHAT_SLC_LOW_000_PANE_BTN_BACK,
  };

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(mpAppHeap->GetSystemHeap()) app::ui::ButtonInfoEx[BTN_ID_MAX];

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( 0 );

  for( u32 i=0; i<BTN_ID_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = pLytWk->GetPartsPane( paneNo[i] );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = parts;
    info[i].bound_pane = pLytWk->GetPane( parts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resID );
  }
  app::ui::UIResponder::CreateButtonManager( mpAppHeap, pLytWk, info, BTN_ID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   カーソル生成
 * @date    2015.10.28
 *
 * @param   buf   カーソルリソースバッファ
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::CreateMenuCursor( void * buf )
{
  mpMenuCursor = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::tool::MenuCursor( mpAppHeap, &buf );

  app::util::G2DUtil * g2d = GetG2DUtil();

  mpMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  mpMenuCursor->SetVisible( true );
  mpMenuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursor
 * @brief   カーソル削除
 * @date    2015.10.28
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxLowChtDraw::DeleteMenuCursor(void)
{
  if( mpMenuCursor != NULL )
  {
    if( mpMenuCursor->DeleteLayoutWork() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( mpMenuCursor );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetChatMessage
 * @brief   チャットメッセージ取得
 * @date    2015.10.28
 *
 * @param   idx   取得データインデックス
 * @param   str   文字列取得場所
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::GetChatMessage( u32 idx, gfl2::str::StrBuf * str )
{
  static const u32 tbl[] =
  {
    msg_chat_01_01,
    msg_chat_01_02,
    msg_chat_01_03,
    msg_chat_01_04,
    msg_chat_01_05,
    msg_chat_01_06,
    msg_chat_01_07,
    msg_chat_01_08,
  };

  if( idx >= BoxLowChtDraw::MSG_TBL_MAX )
  {
    GFL_ASSERT( 0 );
    idx = 0;
  }
  GetG2DUtil()->GetString( str, tbl[idx] );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePaneList
 * @brief   ペインリスト生成
 * @date    2015.10.28
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::CreatePaneList(void)
{
	mpPaneListView = GFL_NEW(mpAppHeap->GetDeviceHeap()) app::tool::PaneListView( mpAppHeap, true );

  CreatePaneListData();

  app::tool::PaneList::SETUP_PARAM list_setup;
  SetPaneListParam( &list_setup, 0, 0 );

  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup;
  SetScrollBarParam( &scroll_bar_setup, list_setup.valMax-list_setup.listPosMax );
  
  mpPaneListView->CreatePaneList( &list_setup );
  mpPaneListView->CreateScrollBar( &scroll_bar_setup );
  SetPaneListConfig();
  mpPaneListView->SetListener( this );

  AddSubView( mpPaneListView );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePaneList
 * @brief   ペインリスト削除
 * @date    2015.10.28
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::DeletePaneList(void)
{
  if( mpPaneListView == NULL )
  {
    return;
  }
  mpPaneListView->DeleteScrollBar();
  mpPaneListView->DeletePaneList();
  mpPaneListView->RemoveFromSuperView();
  GFL_SAFE_DELETE_ARRAY( mpPaneListData );
  GFL_SAFE_DELETE( mpPaneListView );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePaneListData
 * @brief   ペインリストデータ生成
 * @date    2015.10.28
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::CreatePaneListData(void)
{
  static const gfl2::lyt::LytPaneIndex pane[_MAX] =
  {
    PANENAME_CHAT_SLC_LOW_000_PANE_COMMAND_00,
    PANENAME_CHAT_SLC_LOW_000_PANE_COMMAND_01,
    PANENAME_CHAT_SLC_LOW_000_PANE_COMMAND_02,
    PANENAME_CHAT_SLC_LOW_000_PANE_COMMAND_03,
    PANENAME_CHAT_SLC_LOW_000_PANE_COMMAND_04,
    PANENAME_CHAT_SLC_LOW_000_PANE_COMMAND_05,
    PANENAME_CHAT_SLC_LOW_000_PANE_COMMAND_06,
  };

  static const u32 anm[_MAX][4] =
  {
    {
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_00_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_00_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_00_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_00_TOUCH_RELEASE,
    },
    {
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_01_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_01_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_01_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_01_TOUCH_RELEASE,
    },
    {
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_02_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_02_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_02_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_02_TOUCH_RELEASE,
    },
    {
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_03_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_03_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_03_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_03_TOUCH_RELEASE,
    },
    {
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_04_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_04_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_04_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_04_TOUCH_RELEASE,
    },
    {
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_05_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_05_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_05_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_05_TOUCH_RELEASE,
    },
    {
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_06_TOUCH,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_06_RELEASE,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_06_CANSEL,
      LA_CHTLOW_CHAT_SLC_LOW_000__COMMAND_06_TOUCH_RELEASE,
    },
  };

  mpPaneListData = GFL_NEW_LOW_ARRAY(mpAppHeap->GetDeviceHeap()) app::tool::PaneList::PANE_DATA[_MAX];

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( pane[i] );
    mpPaneListData[i].base       = parts;
    mpPaneListData[i].text       = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_BTN_LOW_003_PANE_TEXTBOX_00, &res_id );
    mpPaneListData[i].bound      = lytwk->GetBoundingPane( parts, PANENAME_COMMON_BTN_LOW_003_PANE_BOUND_00, &res_id );
    mpPaneListData[i].cursor_pos = lytwk->GetPane( parts, PANENAME_COMMON_BTN_LOW_003_PANE_NULL_CUR, &res_id );

    mpPaneListData[i].holdAnime    = anm[i][0];
    mpPaneListData[i].releaseAnime = anm[i][1];
    mpPaneListData[i].cancelAnime  = anm[i][2];
    mpPaneListData[i].selectAnime  = anm[i][3];
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListParam
 * @brief   ペインリストのパラメータ設定
 * @date    2015.10.28
 *
 * @param   param   設定場所
 * @param   pos     初期カーソル位置
 * @param   scroll  初期スクロール位置
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::SetPaneListParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, u32 scroll )
{
  gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( LYT_WORK_NORMAL );

  param->heap = mpAppHeap->GetDeviceHeap();   // ヒープ

  param->pLytWk = lytwk;              // レイアウトワーク
  param->paneList = mpPaneListData;   // ペインデータ
  param->paneListMax = _MAX;          // ペインデータ数

  param->valMax = MSG_TBL_MAX;              // 実際に使用するリストの項目数（ペインの数ではない）
  param->listPosMax = LIST_CURSOR_MOVE_MAX; // カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）

  param->listPos = pos;         // 初期カーソル位置 ( 0 〜 listPosMax-1 の範囲 )
  param->listScroll = scroll;   // 初期スクロール位置 ( 0 〜 valMax-listPosMax-1 の範囲 )
                                // 一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                                // valMax-1になるようにする設定する。

  param->defPY = LIST_ITEM_PY;  // ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                                // この座標で指定した場所にあるペインが描画開始位置のペインになる

  param->defSX = LIST_ITEM_SX;  // ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY = LIST_ITEM_SY;  // ペインのYサイズ(レイアウトデディターで厳密に確認）

  param->writeOffsetY = -1;   // 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  param->listStartPane = lytwk->GetPane( PANENAME_CHAT_SLC_LOW_000_PANE_ALL );  // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  param->cbFunc = this;   // コールバック関数
  param->cbWork = NULL;   // コールバック関数に渡すワーク

  param->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();   // UIデバイスマネージャ
  param->menuCursor = mpMenuCursor;   // カーソル
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListConfig
 * @brief   ペインリストサブ設定
 * @date    2015.10.28
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::SetPaneListConfig(void)
{
  app::tool::PaneList * list = mpPaneListView->GetPaneList();
  // サブ設定
  list->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_KEY, true, false, true, false );
  // SE
  list->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetScrollBarParam
 * @brief   スクロールバーパラメータ設定
 * @date    2015.10.28
 *
 * @param   setup   設定場所
 * @param   max     リスト項目数
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::SetScrollBarParam( app::util::ScrollBar::SETUP_PARAM * setup, u32 max )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  setup->heap = mpAppHeap->GetDeviceHeap();                       // ヒープ
  setup->dir  = app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL;  // スクロールの向き
  setup->max  = max;                                              // スクロール最大値

  setup->lytwk        = lytwk;                                        // レイアウトワーク
  setup->res_id       = res_id;                                       // リソースID
  setup->parts_index  = PANENAME_CHAT_SLC_LOW_000_PANE_SCROLL;        // 本体の部品ペインのインデックス
  setup->mark_index   = PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER; // ツマミ部分のペインのインデックス
  setup->top_index    = PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP;    // スクロール値最小位置のペインのインデックス
  setup->bottom_index = PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM; // スクロール値最大位置のペインのインデックス

  setup->parts_pane  = NULL;  // 本体の部品ペイン
  setup->mark_pane   = NULL;  // ツマミ部分のペイン
  setup->top_pane    = NULL;  // スクロール値最小位置のペイン
  setup->bottom_pane = NULL;  // スクロール値最大位置のペイン
};

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorMove
 * @brief   カーソル移動時のコールバック関数
 * @date    2015.10.28
 *
 * @param   work        外部から指定されたワーク
 * @param   cur_pane    ペインデータ
 * @param   pane_index  カーソル位置のペインデータインデックス
 * @param   pos         カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorOff
 * @brief		カーソルを非表示にするコールバック関数
 * @date    2015.10.28
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	カーソル位置のペインデータインデックス
 * @param		pos					カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ListWrite
 * @brief		リストの項目を描画するコールバック関数
 * @date    2015.10.28
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
  GetChatMessage( pos, str );
  g2d->SetTextBoxPaneString( pane[pane_index].text, str );
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ItemChange
 * @brief		入れ替え用コールバック関数
 * @date    2015.10.28
 *
 * @param		work	外部から指定されたワーク
 * @param		pos1	入れ替え位置１
 * @param		pos2	入れ替え位置２
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BoxLowChtDraw::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    PaneListItemSelectListener
 * @brief   ペインリスト決定時に呼ばれるリスナー
 * @date    2015.10.28
 *
 * @param   pos   カーソル位置
 *
 * @note  override PaneListView::Listener
*/
//-----------------------------------------------------------------------------
void BoxLowChtDraw::PaneListItemSelectListener( u32 pos )
{
  m_uiListener->OnButtonAction( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.10.28
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
::app::ui::UIInputListener::ListenerResult BoxLowChtDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( button_id == BTN_ID_CANCEL )
  {
    m_uiListener->OnButtonAction( BoxLowChtDrawListener::LOWCHT_ACTION_CANCEL );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

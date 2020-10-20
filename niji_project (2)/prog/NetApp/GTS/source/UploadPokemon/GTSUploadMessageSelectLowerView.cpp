// ============================================================================
/*
 * @file GTSUploadMessageSelectLowerView.cpp
 * @brief ポケモンを預ける際のメッセージを選択する画面を表示するクラスです。
 * @date 2015.05.11
 */
// ============================================================================
#include "NetApp/GTS/source/UploadPokemon/GTSUploadMessageSelectLowerView.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(UploadPokemon)

static const u32 LIST_MAX = 11;

static const u32 messageIDTable[ LIST_MAX ] =
{
  msg_gts_introtext_01,
  msg_gts_introtext_02,
  msg_gts_introtext_03,
  msg_gts_introtext_04,
  msg_gts_introtext_05,
  msg_gts_introtext_06,
  msg_gts_introtext_07,
  msg_gts_introtext_08,
  msg_gts_introtext_09,
  msg_gts_introtext_10,
  msg_gts_introtext_11
};


GTSUploadMessageSelectLowerView::GTSUploadMessageSelectLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGTSWork,
    NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS,
    LYTRES_GTS_GTS_LST_LOW_000_BFLYT,
    LA_GTS_GTS_LST_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pGTSWork->GetMessageUtility(),
    GARC_message_gts_DAT ),
  m_pGTSWork( pGTSWork ),
  m_pUploadMessageSelectDrawListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_pMenuCursor( NULL ),
  m_bPaneListInputEnable( false )
{
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_LST_LOW_000_PATERN_LARGE );

  setupButton();

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();
  m_pMenuCursor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppMenuCursor(
    pGTSWork, GetG2DUtil(), NetApp::GTS::GTS_RESOURCE_ID_MENU_CURSOR, LAYOUT_RESOURCE_ID_GTS, LAYOUT_WORK_ID_LOWER );

  createPaneList();

  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

    gfl2::lyt::LytPane* pPane = pLower->GetPane( PANENAME_GTS_LST_LOW_000_PANE_BG_00 );
  
    if( pPane )
    {
      pPane->SetVisible( false );
    }
  }

  SetInputListener( this );
}


GTSUploadMessageSelectLowerView::~GTSUploadMessageSelectLowerView()
{
  deletePaneList();

  GFL_SAFE_DELETE( m_pMenuCursor );
}


bool GTSUploadMessageSelectLowerView::IsDrawing(void) const
{
  bool result = false;
  
  if( m_pMenuCursor )
  {
    if( !m_pMenuCursor->CanDestroy() )
    {
      result = true;
    }
  }

  if( UIView::IsDrawing() )
  {
    result = true;
  }

  return result;
}


void GTSUploadMessageSelectLowerView::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void GTSUploadMessageSelectLowerView::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


void GTSUploadMessageSelectLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BTN_BACK, NULL, NULL,
      LA_GTS_GTS_LST_LOW_000__BTN_BACK_TOUCH,
      LA_GTS_GTS_LST_LOW_000__BTN_BACK_RELEASE,
      LA_GTS_GTS_LST_LOW_000__BTN_BACK_CANSEL,
      LA_GTS_GTS_LST_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_GTS_GTS_LST_LOW_000__BTN_BACK_ACTIVE,
      LA_GTS_GTS_LST_LOW_000__BTN_BACK_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTN_BACK );
    pBtnBack->SetVisible( true );
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  // 不要なボタンを非表示にする
  {
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_00 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_01 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_02 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_03 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_04 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_05 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_06 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_07 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_08 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNS_09 )->SetVisible( false );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );

}


void GTSUploadMessageSelectLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    static const gfl2::lyt::LytPaneIndex btns_tbl[LIST_ITEM_PANE_MAX] =
    {
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_00,
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_01,
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_02,
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_03,
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_04,
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_05,
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_06,
      PANENAME_GTS_LST_LOW_000_PANE_BTNL_07,
    };

    static const u32 anm[LIST_ITEM_PANE_MAX][4] =
    {
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_00_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_00_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_00_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_00_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_01_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_01_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_01_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_01_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_02_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_02_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_02_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_02_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_03_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_03_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_03_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_03_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_04_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_04_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_04_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_04_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_05_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_05_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_05_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_05_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_06_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_06_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_06_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_06_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNL_07_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNL_07_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNL_07_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNL_07_TOUCH_RELEASE,
      },
    };

    m_pPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
      m_pPaneListData[i].base       = pBTNS;
      m_pPaneListData[i].text       = pLower->GetTextBoxPane( pBTNS, PANENAME_GTS_BTN_LOW_005_PANE_TEXTBOX_00, &resourceID );
      m_pPaneListData[i].bound      = pLower->GetBoundingPane( pBTNS, PANENAME_GTS_BTN_LOW_005_PANE_BOUND_00, &resourceID );
      m_pPaneListData[i].cursor_pos = pLower->GetPane( pBTNS, PANENAME_GTS_BTN_LOW_005_PANE_NULL_CUR, &resourceID );

      m_pPaneListData[i].holdAnime    = anm[i][0];
      m_pPaneListData[i].releaseAnime = anm[i][1];
      m_pPaneListData[i].cancelAnime  = anm[i][2];
      m_pPaneListData[i].selectAnime  = anm[i][3];
    }
  } // m_pPaneListData 初期化 ここまで
  // -----------------------------------------------------------------------

  // -----------------------------------------------------------------------
  { // m_pPaneListView 初期化 ここから

    app::tool::PaneList::SETUP_PARAM list_setup;
    setPaneListParam( &list_setup, 0, 0 );

    app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
    {
      pAppHeap->GetDeviceHeap(),
      app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
      list_setup.valMax-list_setup.listPosMax,

      pLower,
      resourceID,
      PANENAME_GTS_LST_LOW_000_PANE_SCR_00,
      PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

      NULL,
      NULL,
      NULL,
      NULL
    };

    m_pPaneListView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::PaneListView( pAppHeap, true );
    m_pPaneListView->CreatePaneList( &list_setup );
    m_pPaneListView->CreateScrollBar( &scroll_bar_setup );
    m_pPaneListView->SetListener( this );
    setPaneListConfig( &list_setup );
    AddSubView( m_pPaneListView );
    setInputEnablePaneList( true );

  } // m_pPaneListView 初期化 ここまで
  // -----------------------------------------------------------------------
}


void GTSUploadMessageSelectLowerView::deletePaneList()
{
  {
    m_pPaneListView->RemoveFromSuperView();
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
    GFL_SAFE_DELETE( m_pPaneListView );
  }

  {
    GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
  }
}


void GTSUploadMessageSelectLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, u32 scroll )
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax = LIST_MAX;					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 4;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 92.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる
  pParam->defSX = 308.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 50.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  pParam->listStartPane = pLower->GetPane( PANENAME_GTS_LST_LOW_000_PANE_ITEM_CHANGE_OFF );   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	// コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_pMenuCursor->GetAppToolMenuCursor();		// カーソル
}


void GTSUploadMessageSelectLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
{
  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_NONE, true, false, true, false );
  pPaneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );

  { // タッチ範囲
    gfl2::math::VEC3 pos = m_pPaneListData[0].bound->GetTranslate();
    u32 pos_max = pParam->listPosMax;
    if( pParam->valMax < pos_max )
    {
      pos_max = pParam->valMax;
    }

    f32 lx = 4.0f - pParam->defSX / 2.0f;
    f32 rx = lx + pParam->defSX;
    f32 uy = pParam->defPY + pos.y + pParam->defSY / 2.0f;
    f32 dy = uy - pParam->defSY * pos_max;
    pPaneList->MakeTouchArea( lx, rx, uy, dy );
  }

}


void GTSUploadMessageSelectLowerView::setInputEnablePaneList( bool bInputEnable )
{
  m_bPaneListInputEnable = bInputEnable;

  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      pPaneList->SetInputEnabled( bInputEnable );
    }
  }
}


void GTSUploadMessageSelectLowerView::Update( void )
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_pMenuCursor->Update();
}


void GTSUploadMessageSelectLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_pMenuCursor->Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult GTSUploadMessageSelectLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    //『戻る』ボタン
    case BUTTON_ID_BTN_BACK:
    {
      if( m_pUploadMessageSelectDrawListener )
      {
        m_pUploadMessageSelectDrawListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult GTSUploadMessageSelectLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_BTN_BACK );
    result = DISABLE_ACCESS;
  }
  return result;
}


void GTSUploadMessageSelectLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
}


void GTSUploadMessageSelectLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void GTSUploadMessageSelectLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  pG2DUtil->SetTextBoxPaneStringExpand( pane[ pane_index ].text, messageIDTable[ pos ] );
}


void GTSUploadMessageSelectLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void GTSUploadMessageSelectLowerView::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  if( m_pUploadMessageSelectDrawListener )
  {
    m_pUploadMessageSelectDrawListener->SelectMessage( messageIDTable[ pos ] );
  }
}


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

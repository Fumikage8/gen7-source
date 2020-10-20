// ============================================================================
/*
 * @file GTSScrollMenuLowerView.cpp
 * @brief スクロールするメニューを表示するクラスです。
 * @date 2015.03.31
 */
// ============================================================================
#include "NetApp/GTS/source/ScrollMenu/GTSScrollMenuLowerView.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetApp/GTS/source/ScrollMenu/GTSInitialMessageIDTable.hpp"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"
#include "System/include/PokemonLanguage.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>
#include <niji_conv_header/message/msg_monsname.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ScrollMenu)


static const u32 active_passive_anm[GTSScrollMenuLowerView::LIST_ITEM_PANE_MAX][2] =
{
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_00_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_00_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_01_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_01_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_02_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_02_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_03_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_03_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_04_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_04_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_05_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_05_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_06_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_06_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_07_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_07_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_08_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_08_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_09_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_09_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_10_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_10_PASSIVE,
  },
  {
    LA_GTS_GTS_LST_LOW_000__BTNS_11_ACTIVE,
    LA_GTS_GTS_LST_LOW_000__BTNS_11_PASSIVE,
  },
};


GTSScrollMenuLowerView::GTSScrollMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGTSWork,
    NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS,
    LYTRES_GTS_GTS_LST_LOW_000_BFLYT,
    LA_GTS_GTS_LST_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pGTSWork->GetMessageUtility(),
    GARC_message_gts_DAT ),
  m_pGTSWork( pGTSWork ),
  m_pScrollMenuDrawListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_pMyPaneListData( NULL ),
  m_pMenuCursor( NULL ),
  m_eMenuType( MENU_TYPE_GENDER ),
  m_SelectInitial( 0 ),
  m_SelectInitialFirst( 0 ),
  m_PaneListMenuMax( NetApp::GTS::GTSUtility::GetGenderMessageIDTableSize() ),
  m_pMessageIDTable( NetApp::GTS::GTSUtility::GetGenderMessageIDTable() ),
  m_MonsNameMessageIDTable(),
  m_aPaneListCursorInfos(),
  m_PassiveInfoList(),
  m_pPokeIconToolEx( NULL ),
  m_PokemonParam( pGTSWork->GetDeviceHeap() ),
  m_bPaneListInputEnable( false )
{
  gfl2::std::MemClear( m_aPaneListCursorInfos, sizeof( m_aPaneListCursorInfos ) );

  setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_LST_LOW_000_PATERN_SMALL );

  createPassiveInfoList();

  setupButton();
  
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();
  m_pMenuCursor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppMenuCursor(
    pGTSWork, GetG2DUtil(), NetApp::GTS::GTS_RESOURCE_ID_MENU_CURSOR, LAYOUT_RESOURCE_ID_GTS, LAYOUT_WORK_ID_LOWER );

  createPaneList();

  {
    app::tool::PokeIconToolEx::PokeIconToolParam  pokeIconToolParam;

    pokeIconToolParam.pUIView   = this;
    pokeIconToolParam.pHeap     = m_pGTSWork->GetAppHeap();
    pokeIconToolParam.layout_id = LAYOUT_WORK_ID_LOWER;
    pokeIconToolParam.res_id    = LAYOUT_RESOURCE_ID_GTS;
    pokeIconToolParam.iconMax   = LIST_ITEM_PANE_MAX;
    pokeIconToolParam.sync      = true;

    m_pPokeIconToolEx = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::PokeIconToolEx( pokeIconToolParam );
  }


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


GTSScrollMenuLowerView::~GTSScrollMenuLowerView()
{
  GFL_SAFE_DELETE( m_pPokeIconToolEx );

  deletePaneList();

  GFL_SAFE_DELETE( m_pMenuCursor );

}


bool GTSScrollMenuLowerView::IsDrawing(void) const
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

  if( !m_pPokeIconToolEx->EndFunc() )
  {
    result = true;
  }

  return result;
}


void GTSScrollMenuLowerView::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void GTSScrollMenuLowerView::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


void GTSScrollMenuLowerView::setupButton()
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
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNL_00 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNL_01 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNL_02 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNL_03 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNL_04 )->SetVisible( false );
    pLower->GetPartsPane( PANENAME_GTS_LST_LOW_000_PANE_BTNL_05 )->SetVisible( false );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );

}


void GTSScrollMenuLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    static const gfl2::lyt::LytPaneIndex btns_tbl[LIST_ITEM_PANE_MAX] =
    {
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_00,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_01,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_02,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_03,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_04,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_05,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_06,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_07,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_08,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_09,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_10,
      PANENAME_GTS_LST_LOW_000_PANE_BTNS_11,
    };

    static const u32 anm[LIST_ITEM_PANE_MAX][4] =
    {
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_00_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_00_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_00_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_00_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_01_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_01_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_01_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_01_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_02_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_02_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_02_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_02_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_03_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_03_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_03_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_03_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_04_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_04_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_04_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_04_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_05_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_05_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_05_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_05_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_06_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_06_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_06_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_06_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_07_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_07_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_07_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_07_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_08_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_08_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_08_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_08_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_09_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_09_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_09_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_09_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_10_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_10_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_10_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_10_TOUCH_RELEASE,
      },
      {
        LA_GTS_GTS_LST_LOW_000__BTNS_11_TOUCH,
        LA_GTS_GTS_LST_LOW_000__BTNS_11_RELEASE,
        LA_GTS_GTS_LST_LOW_000__BTNS_11_CANSEL,
        LA_GTS_GTS_LST_LOW_000__BTNS_11_TOUCH_RELEASE,
      },
    };

    m_pPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
    m_pMyPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) MyPaneInfo[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
      gfl2::lyt::LytParts* pPokeIconParts = pLower->GetPartsPane( pBTNS, PANENAME_GTS_BTN_LOW_006_PANE_ICON_POKE, &resourceID );

      m_pPaneListData[i].base       = pBTNS;
      m_pPaneListData[i].text       = pLower->GetTextBoxPane( pBTNS, PANENAME_GTS_BTN_LOW_006_PANE_TEXTBOX_00, &resourceID );
      m_pPaneListData[i].bound      = pLower->GetBoundingPane( pBTNS, PANENAME_GTS_BTN_LOW_006_PANE_BOUND_00, &resourceID );
      m_pPaneListData[i].cursor_pos = pLower->GetPane( pBTNS, PANENAME_GTS_BTN_LOW_006_PANE_NULL_CUR, &resourceID );

      m_pPaneListData[i].holdAnime    = anm[i][0];
      m_pPaneListData[i].releaseAnime = anm[i][1];
      m_pPaneListData[i].cancelAnime  = anm[i][2];
      m_pPaneListData[i].selectAnime  = anm[i][3];

      m_pMyPaneListData[i].pBallIcon = pLower->GetPane( pBTNS, PANENAME_GTS_BTN_LOW_006_PANE_ICON_BALL, &resourceID );
      m_pMyPaneListData[i].pPokeIcon = pLower->GetPicturePane( pPokeIconParts, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &resourceID );
    }
  } // m_pPaneListData 初期化 ここまで
  // -----------------------------------------------------------------------

  // -----------------------------------------------------------------------
  { // m_pPaneListView 初期化 ここから

    app::tool::PaneList::SETUP_PARAM list_setup;
    setPaneListParam( &list_setup, 0, 0.0f );

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


void GTSScrollMenuLowerView::deletePaneList()
{
  {
    m_pPaneListView->RemoveFromSuperView();
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
    GFL_SAFE_DELETE( m_pPaneListView );
  }

  {
    GFL_SAFE_DELETE_ARRAY( m_pMyPaneListData );
    GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
  }
}


void GTSScrollMenuLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll )
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax     = ( m_PaneListMenuMax == 0 ) ? 1 : m_PaneListMenuMax;		// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = LIST_POS_MAX;		   	// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 103.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                                  // この座標で指定した場所にあるペインが描画開始位置のペインになる
  pParam->defSX = 184.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 28.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		  // 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  pParam->listStartPane = pLower->GetPane( PANENAME_GTS_LST_LOW_000_PANE_ITEM_CHANGE_OFF );   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	    // コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_pMenuCursor->GetAppToolMenuCursor();		// カーソル
}


void GTSScrollMenuLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
{
  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_BUTTON, true, false, true, false );
  pPaneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );

  { // タッチ範囲
    gfl2::math::VEC3 pos = m_pPaneListData[0].bound->GetTranslate();
    u32 pos_max = pParam->listPosMax;
    if( pParam->valMax < pos_max )
    {
      pos_max = pParam->valMax;
    }

    f32 lx = 67.0f - pParam->defSX / 2.0f;
    f32 rx = lx + pParam->defSX;
    f32 uy = pParam->defPY + pos.y + pParam->defSY / 2.0f;
    f32 dy = uy - pParam->defSY * pos_max;
    pPaneList->MakeTouchArea( lx, rx, uy, dy );
  }

}


void GTSScrollMenuLowerView::setInputEnablePaneList( bool bInputEnable )
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


void GTSScrollMenuLowerView::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_pMenuCursor->Update();
  m_pPokeIconToolEx->Update();

}


void GTSScrollMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_pMenuCursor->Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult GTSScrollMenuLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    //『戻る』ボタン
    case BUTTON_ID_BTN_BACK:
    {
      switch( m_eMenuType )
      {
        case MENU_TYPE_INITIAL_SECOND:
        {
          changeMenu( MENU_TYPE_INITIAL_FIRST, 0 );
        }
        break;
        
        case MENU_TYPE_POKEMON:
        {
          switch( System::GetLang() )
          {
            case POKEMON_LANG_JAPAN:
            {
              changeMenu( MENU_TYPE_INITIAL_SECOND, m_SelectInitial );
            }
            break;

            default:
            {
              changeMenu( MENU_TYPE_INITIAL_FIRST, m_SelectInitial );
            }
            break;
          }
        }
        break;

        default:
        {
          if( m_pScrollMenuDrawListener )
          {
            m_pScrollMenuDrawListener->TouchBackButton();
          }
        }
        break;
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult GTSScrollMenuLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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


void GTSScrollMenuLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
}


void GTSScrollMenuLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void GTSScrollMenuLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pGTSWork->GetMessageUtility();

  switch( m_eMenuType )
  {
    case MENU_TYPE_INITIAL_FIRST:
    case MENU_TYPE_INITIAL_SECOND:
    {
      // 「入力」を描画する時
      if( m_pMessageIDTable[ pos ] == msg_gts_sel_26 )
      {
        pG2DUtil->SetTextBoxPaneStringExpand( pane[ pane_index ].text, m_pMessageIDTable[ pos ] );
      }
      else
      {
        pG2DUtil->SetTextBoxPaneString( pane[ pane_index ].text, &pMessageUtility->GetString( GARC_message_initial_DAT, m_pMessageIDTable[ pos ] ) );
      }

      m_pMyPaneListData[ pane_index ].pBallIcon->SetVisible( false );
      m_pMyPaneListData[ pane_index ].pPokeIcon->SetVisible( false );
    }
    break;

    case MENU_TYPE_POKEMON:
    {
      MonsNo monsNo = (MonsNo)m_pMessageIDTable[ pos ];
      pG2DUtil->SetRegisterMonsNameNo( 0, monsNo );
      pG2DUtil->SetTextBoxPaneStringExpand( pane[ pane_index ].text, msg_gts_sel_05 );

      {
        Savedata::ZukanData* pZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
        bool bPokeGetFlag = pZukanData->GetPokeGetFlag( monsNo );
        m_PokemonParam.ChangeMonsNo( monsNo, 0 );
        m_pPokeIconToolEx->SetPokeIcon( pane_index, &m_PokemonParam, m_pMyPaneListData[ pane_index ].pPokeIcon );
        m_pMyPaneListData[ pane_index ].pBallIcon->SetVisible( bPokeGetFlag );
        m_pMyPaneListData[ pane_index ].pPokeIcon->SetVisible( true );
      }
    }
    break;

    case MENU_TYPE_GENDER:
    case MENU_TYPE_LEVEL_BAND:
    {
      pG2DUtil->SetTextBoxPaneStringExpand( pane[ pane_index ].text, m_pMessageIDTable[ pos ] );

      m_pMyPaneListData[ pane_index ].pBallIcon->SetVisible( false );
      m_pMyPaneListData[ pane_index ].pPokeIcon->SetVisible( false );
    }
    break;
  }

  if( isPassive( m_pMessageIDTable[ pos ] ) )
  {
    setPassive( pane_index );
  }
  else
  {
    setActive( pane_index );
  }
}


void GTSScrollMenuLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void GTSScrollMenuLowerView::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  if( isPassive( m_pMessageIDTable[ pos ] ) )
  {
    return;
  }

  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    
    if( pPaneList )
    {
      pPaneList->GetCursorData( &m_aPaneListCursorInfos[ m_eMenuType ].pos, &m_aPaneListCursorInfos[ m_eMenuType ].scroll );
    }
  }

  switch( m_eMenuType )
  {
    case MENU_TYPE_INITIAL_FIRST:
    {
      // 「入力」が選択された時
      if( m_pMessageIDTable[ pos ] == msg_gts_sel_26 )
      {
        if( m_pScrollMenuDrawListener )
        {
          m_pScrollMenuDrawListener->TouchInputButton();
        }
      }
      else
      {
        switch( System::GetLang() )
        {
          case POKEMON_LANG_JAPAN:
          {
            if( m_SelectInitialFirst != m_pMessageIDTable[ pos ] )
            {
              m_aPaneListCursorInfos[ MENU_TYPE_INITIAL_SECOND ].pos = 0;
              m_aPaneListCursorInfos[ MENU_TYPE_INITIAL_SECOND ].scroll = 0.0f;
            }

            m_SelectInitialFirst = m_pMessageIDTable[ pos ];

            changeMenu( MENU_TYPE_INITIAL_SECOND, pos );
          }
          break;

          default:
          {
            changeMenu( MENU_TYPE_POKEMON, pos );
          }
          break;
        }
      }
    }
    break;

    case MENU_TYPE_INITIAL_SECOND:
    {
      changeMenu( MENU_TYPE_POKEMON, pos );
    }
    break;

    case MENU_TYPE_POKEMON:
    {
      if( m_pScrollMenuDrawListener )
      {
        m_pScrollMenuDrawListener->SelectPokemon( (MonsNo)m_pMessageIDTable[ pos ], m_pMessageIDTable[ pos ] );
      }
    }
    break;

    case MENU_TYPE_GENDER:
    {
      u32 gender = NetApp::GTS::GENDER_UNKNOWN;

      switch( pos )
      {
        case 0: gender = NetApp::GTS::GENDER_UNKNOWN; break;
        case 1: gender = NetApp::GTS::GENDER_MALE;    break;
        case 2: gender = NetApp::GTS::GENDER_FMALE;   break;
      }

      if( m_pScrollMenuDrawListener )
      {
        m_pScrollMenuDrawListener->SelectGender( gender, m_pMessageIDTable[ pos ] );
      }
    }
    break;
    
    case MENU_TYPE_LEVEL_BAND:
    {
      if( m_pScrollMenuDrawListener )
      {
        m_pScrollMenuDrawListener->SelectLevelBand( pos, m_pMessageIDTable[ pos ] );
      }
    }
    break;
  }
}


void GTSScrollMenuLowerView::changeMenu( MenuType eMenuType, u32 param )
{
  m_eMenuType = eMenuType;

  switch( m_eMenuType )
  {
    case MENU_TYPE_INITIAL_FIRST:
    {
      switch( System::GetLang() )
      {
        case POKEMON_LANG_JAPAN:
        {
          m_PaneListMenuMax = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_JAPAN );
          m_pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_JAPAN;
        }
        break;
        
        case POKEMON_LANG_KOREA:
        {
          m_PaneListMenuMax = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_KOREA );
          m_pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_KOREA;
        }
        break;
        
        case POKEMON_LANG_CHINA:
        {
          m_PaneListMenuMax = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_CHINA );
          m_pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_CHINA;
        }
        break;
        
        case POKEMON_LANG_TAIWAN:
        {
          m_PaneListMenuMax = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_TAIWAN );
          m_pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_TAIWAN;
        }
        break;
        
        default:
        {
          m_PaneListMenuMax = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_ENGLISH );
          m_pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_ENGLISH;
        }
        break;
      }
    }
    break;

    case MENU_TYPE_INITIAL_SECOND:
    {
      m_pMessageIDTable = getInitialSecondMessageIDTable( m_PaneListMenuMax, param );
    }
    break;

    case MENU_TYPE_POKEMON:
    {
      m_pMessageIDTable = getMonsNameMessageIDTable( m_PaneListMenuMax, param );
    }
    break;

    case MENU_TYPE_GENDER:
    {
      m_PaneListMenuMax = NetApp::GTS::GTSUtility::GetGenderMessageIDTableSize();
      m_pMessageIDTable = NetApp::GTS::GTSUtility::GetGenderMessageIDTable();
    }
    break;
    
    case MENU_TYPE_LEVEL_BAND:
    {
      m_PaneListMenuMax = NetApp::GTS::GTSUtility::GetLevelBandMessageIDTableSize();
      m_pMessageIDTable = NetApp::GTS::GTSUtility::GetLevelBandMessageIDTable();
    }
    break;
  }

  // 同じリストが設定される時、リストサイズに変動があったらカーソル位置をリセットする
  if( m_aPaneListCursorInfos[ m_eMenuType ].listMax > 0 )
  {
    if( m_aPaneListCursorInfos[ m_eMenuType ].listMax != m_PaneListMenuMax )
    {
      m_aPaneListCursorInfos[ m_eMenuType ].pos = 0;
      m_aPaneListCursorInfos[ m_eMenuType ].scroll = 0.0f;
    }
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  setPaneListParam( &list_setup, m_aPaneListCursorInfos[ m_eMenuType ].pos, m_aPaneListCursorInfos[ m_eMenuType ].scroll );
  m_pPaneListView->ResetPaneList( &list_setup );
  setPaneListConfig( &list_setup );
  m_aPaneListCursorInfos[ m_eMenuType ].listMax = m_PaneListMenuMax;

  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->InitListPut();
}


const u32* GTSScrollMenuLowerView::getInitialSecondMessageIDTable( u32& tableSize, u32 initial )
{
  const u32* pMessageIDTable = NULL;

  m_SelectInitial = initial;
  
  switch( System::GetLang() )
  {
    case POKEMON_LANG_JAPAN:
    {
      switch( initial )
      {
        case 0:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_0_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_0_JAPAN;
        }
        break;

        case 1:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_1_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_1_JAPAN;
        }
        break;

        case 2:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_2_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_2_JAPAN;
        }
        break;

        case 3:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_3_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_3_JAPAN;
        }
        break;

        case 4:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_4_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_4_JAPAN;
        }
        break;

        case 5:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_5_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_5_JAPAN;
        }
        break;

        case 6:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_6_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_6_JAPAN;
        }
        break;

        case 7:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_7_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_7_JAPAN;
        }
        break;

        case 8:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_8_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_8_JAPAN;
        }
        break;

        case 9:
        {
          tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_9_JAPAN );
          pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_9_JAPAN;
        }
        break;
      }
    }
    break;

    case POKEMON_LANG_KOREA:
    {
      tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_KOREA );
      pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_KOREA;
    }
    break;

    case POKEMON_LANG_CHINA:
    {
      tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_CHINA );
      pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_CHINA;
    }
    break;

    case POKEMON_LANG_TAIWAN:
    {
      tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_TAIWAN );
      pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_TAIWAN;
    }
    break;

    default:
    {
      tableSize = GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_ENGLISH );
      pMessageIDTable = NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_ENGLISH;
    }
    break;
  }

  GFL_ASSERT_MSG( pMessageIDTable, "Error : GTSScrollMenuLowerView::getInitialSecondMessageIDTable() pMessageIDTable == NULL \n" );

  return pMessageIDTable;
}


const u32* GTSScrollMenuLowerView::getMonsNameMessageIDTable( u32& tableSize, u32 initial )
{
  u32 initialSecondMessageIDTableSize = 0;
  const u32* pInitialSecondMessageIDTable = getInitialSecondMessageIDTable( initialSecondMessageIDTableSize, m_SelectInitial );

  GameSys::GameData*      pGameData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Savedata::ZukanData*    pZukanData  = pGameData->GetZukanData();

  PokeTool::PersonalSort sort( System::GetLang(), pZukanData, m_pGTSWork->GetDeviceHeap(), PokeTool::PersonalSort::LOAD_TYPE_FULL );

  sort.SetConditionZukan( PokeTool::PokemonSort::CONDITION_ZUKAN_SEE_OR_CAPTURE );
  sort.OrderByMonsName( pInitialSecondMessageIDTable[ initial ] );

  u32 size = sort.GetSortedDataMax();

  for( u32 i = 0; i < size; ++i )
  {
    m_MonsNameMessageIDTable[i] = static_cast<u32>( sort.GetSortedMonsNo(i) );
  }

  tableSize = size;

  return m_MonsNameMessageIDTable;
}


void GTSScrollMenuLowerView::setActive( u32 paneIndex )
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      pPaneList->SetButtonSEPaneIndex( paneIndex, SEQ_SE_DECIDE1 );
      
      nw::ut::Color8 color = nw::ut::Color8::BLACK;
      
      m_pPaneListData[ paneIndex ].text->SetTextColor( color, color );
    }
  }
}


void GTSScrollMenuLowerView::setPassive( u32 paneIndex )
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      pPaneList->SetButtonSEPaneIndex( paneIndex, SEQ_SE_BEEP );
      
      nw::ut::Color8 color = nw::ut::Color8::GRAY;

      m_pPaneListData[ paneIndex ].text->SetTextColor( color, color );
    }
  }
}


bool GTSScrollMenuLowerView::isPassive( u32 initialMessageID )
{
  bool bIsPassive = false;

  switch( m_eMenuType )
  {
    case MENU_TYPE_INITIAL_FIRST:
    {
      switch( System::GetLang() )
      {
        case POKEMON_LANG_JAPAN:
        {
          switch( initialMessageID )
          {
            case initial_00: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_0_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_0_JAPAN ) ); break;
            case initial_05: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_1_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_1_JAPAN ) ); break;
            case initial_10: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_2_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_2_JAPAN ) ); break;
            case initial_15: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_3_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_3_JAPAN ) ); break;
            case initial_20: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_4_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_4_JAPAN ) ); break;
            case initial_25: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_5_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_5_JAPAN ) ); break;
            case initial_30: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_6_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_6_JAPAN ) ); break;
            case initial_35: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_7_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_7_JAPAN ) ); break;
            case initial_38: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_8_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_8_JAPAN ) ); break;
            case initial_43: bIsPassive = checkPassiveInfo( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_9_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_9_JAPAN ) ); break;
          }
        }
        break;
        
        default:
        {
          bIsPassive = checkPassiveInfo( initialMessageID );
        }
        break;
      }
    }
    break;

    case MENU_TYPE_INITIAL_SECOND:
    {
      switch( System::GetLang() )
      {
        case POKEMON_LANG_JAPAN:
        {
          bIsPassive = checkPassiveInfo( initialMessageID );
        }
        break;
      }
    }
    break;
  }

  return bIsPassive;
}


void GTSScrollMenuLowerView::createPassiveInfoList()
{
  GameSys::GameData*      pGameData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Savedata::ZukanData*    pZukanData  = pGameData->GetZukanData();

  PokeTool::PersonalSort sort( System::GetLang(), pZukanData, m_pGTSWork->GetDeviceHeap(), PokeTool::PersonalSort::LOAD_TYPE_FULL );

  sort.SetConditionZukan( PokeTool::PokemonSort::CONDITION_ZUKAN_SEE_OR_CAPTURE );

  switch( System::GetLang() )
  {
    case POKEMON_LANG_JAPAN:
    {
      u32 listSize = 0;
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_0_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_1_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_2_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_3_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_4_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_5_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_6_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_7_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_8_JAPAN );
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_9_JAPAN );

      m_PassiveInfoList.CreateBuffer( m_pGTSWork->GetDeviceHeap(), listSize );

      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_0_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_0_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_1_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_1_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_2_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_2_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_3_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_3_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_4_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_4_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_5_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_5_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_6_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_6_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_7_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_7_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_8_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_8_JAPAN ) );
      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_9_JAPAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_SECOND_TABLE_9_JAPAN ) );
    }
    break;
        
    case POKEMON_LANG_KOREA:
    {
      u32 listSize = 0;
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_KOREA );
      listSize -= 1; // 『msg_gts_sel_26, // 入力』この分 1 引く

      m_PassiveInfoList.CreateBuffer( m_pGTSWork->GetDeviceHeap(), listSize );

      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_KOREA, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_KOREA ) );
    }
    break;
        
    case POKEMON_LANG_CHINA:
    {
      u32 listSize = 0;
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_CHINA );
      listSize -= 1; // 『msg_gts_sel_26, // 入力』この分 1 引く

      m_PassiveInfoList.CreateBuffer( m_pGTSWork->GetDeviceHeap(), listSize );

      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_CHINA, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_CHINA ) );
    }
    break;
        
    case POKEMON_LANG_TAIWAN:
    {
      u32 listSize = 0;
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_TAIWAN );
      listSize -= 1; // 『msg_gts_sel_26, // 入力』この分 1 引く

      m_PassiveInfoList.CreateBuffer( m_pGTSWork->GetDeviceHeap(), listSize );

      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_TAIWAN, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_TAIWAN ) );
    }
    break;
        
    default:
    {
      u32 listSize = 0;
      listSize += GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_ENGLISH );
      listSize -= 1; // 『msg_gts_sel_26, // 入力』この分 1 引く

      m_PassiveInfoList.CreateBuffer( m_pGTSWork->GetDeviceHeap(), listSize );

      addPassiveInfo( sort, NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_ENGLISH, GFL_NELEMS( NetApp::GTS::ScrollMenu::MESSAGE_INITIAL_FIRST_TABLE_ENGLISH ) );
    }
    break;
  }
}


void GTSScrollMenuLowerView::addPassiveInfo( PokeTool::PersonalSort& sort, const u32* pMessageIDTable, u32 tableSize )
{
  PassiveInfo passiveInfo;

  for( u32 i = 0; i < tableSize; ++i )
  {
    if( pMessageIDTable[i] == msg_gts_sel_26 )
    {
      continue;
    }

    bool bResult = sort.OrderByMonsName( pMessageIDTable[i] ); // @fix NMCat #4244 指定したイニシャルメッセージのポケモンがいるかいないか、リザルト値を見ていなかった
    
    u32 size = 0;

    if( bResult )
    {
      size = sort.GetSortedDataMax(); // 指定したイニシャルメッセージのポケモンがいる場合は正しいデータが返ってくるのでそのまま代入
    }

    passiveInfo.initialMessageID = pMessageIDTable[i];
    passiveInfo.isPassive = ( size == 0 ) ? true : false;

    m_PassiveInfoList.PushBack( passiveInfo );
  }
}


bool GTSScrollMenuLowerView::checkPassiveInfo( const u32* pMessageIDTable, u32 tableSize )
{
  for( u32 i = 0; i < tableSize; ++i )
  {
    if( !checkPassiveInfo( pMessageIDTable[i] ) )
    {
      return false;
    }
  }

  return true;
}


bool GTSScrollMenuLowerView::checkPassiveInfo( u32 initialMessageID )
{
  for( gfl2::util::List<PassiveInfo>::Iterator it = m_PassiveInfoList.Begin(); it != m_PassiveInfoList.End(); ++it )
  {
    if( it->initialMessageID == initialMessageID )
    {
      return it->isPassive;
    }
  }

  return false;
}


GFL_NAMESPACE_END(ScrollMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

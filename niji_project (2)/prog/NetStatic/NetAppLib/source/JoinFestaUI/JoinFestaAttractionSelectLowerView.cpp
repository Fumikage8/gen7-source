// ============================================================================
/*
 * @file JoinFestaAttractionSelectLowerView.cpp
 * @brief アトラクション選択下画面
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include "Sound/include/Sound.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


static const gfl2::lyt::LytPaneIndex btns_tbl[JoinFestaAttractionSelectLowerView::LIST_ITEM_PANE_MAX] =
{
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_05,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_06,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_07,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_08,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_09,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_10,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_11,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_12,
};

static const u32 anm[JoinFestaAttractionSelectLowerView::LIST_ITEM_PANE_MAX][4] =
{
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_05_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_05_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_05_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_05_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_06_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_06_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_06_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_06_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_07_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_07_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_07_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_07_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_08_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_08_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_08_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_08_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_09_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_09_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_09_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_09_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_10_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_10_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_10_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_10_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_11_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_11_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_11_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_11_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_12_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_12_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_12_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_12_TOUCH_RELEASE
  },
};


static const u32 active_passive_anm[JoinFestaAttractionSelectLowerView::LIST_ITEM_PANE_MAX][2] =
{
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_05_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_05_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_06_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_06_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_07_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_07_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_08_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_08_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_09_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_09_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_10_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_10_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_11_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_11_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_12_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_12_PASSIVE,
  },
};


JoinFestaAttractionSelectLowerView::JoinFestaAttractionSelectLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::System::ResourceID layoutResourceID,
  NetAppLib::System::ResourceID menuCursorResourceID ) :
  NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBase( pWorkBase, layoutResourceID, menuCursorResourceID  ),
  m_aPaneListCursorInfos(),
  m_eListType( LIST_TYPE_OPEN ),
  m_pJoinFestaAttractionSelectLowerViewListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_pActiveList( NULL ),
  m_bPaneListInputEnable( false ),
  m_bIsResetPaneListCursorInfo( false )
{
  gfl2::std::MemClear( m_aPaneListCursorInfos, sizeof( m_aPaneListCursorInfos ) );

  setupLayout( LAYOUT_WORK_ID_LOWER, LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000_LAYOUT_01 );
  setupButton();

  createPaneList();

  SetInputListener( this );
}


JoinFestaAttractionSelectLowerView::~JoinFestaAttractionSelectLowerView()
{
  deletePaneList();
}


void JoinFestaAttractionSelectLowerView::Update()
{
  JoinFestaPlayerListLowerViewBase::Update();
}


void JoinFestaAttractionSelectLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  JoinFestaPlayerListLowerViewBase::Draw( displayNo );
}


bool JoinFestaAttractionSelectLowerView::IsDrawing(void) const
{
  return JoinFestaPlayerListLowerViewBase::IsDrawing();
}


void JoinFestaAttractionSelectLowerView::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void JoinFestaAttractionSelectLowerView::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


void JoinFestaAttractionSelectLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_PASSIVE,
    },

    {
      BUTTON_ID_UPDATE_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_PASSIVE,
    },

    {
      BUTTON_ID_ARROW_LEFT, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_PASSIVE,
    },

    {
      BUTTON_ID_ARROW_RIGHT, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_00 );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pUpdateButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_RELOADBUTTON );
    buttonInfoTable[ BUTTON_ID_UPDATE_BUTTON ].picture_pane = pUpdateButton;
    buttonInfoTable[ BUTTON_ID_UPDATE_BUTTON ].bound_pane = pLower->GetBoundingPane( pUpdateButton, PANENAME_FESTA_BTN_LOW_009_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pArrowLeft = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_04 );
    buttonInfoTable[ BUTTON_ID_ARROW_LEFT ].picture_pane = pArrowLeft;
    buttonInfoTable[ BUTTON_ID_ARROW_LEFT ].bound_pane = pLower->GetBoundingPane( pArrowLeft, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pArrowRight = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_03 );
    buttonInfoTable[ BUTTON_ID_ARROW_RIGHT ].picture_pane = pArrowRight;
    buttonInfoTable[ BUTTON_ID_ARROW_RIGHT ].bound_pane = pLower->GetBoundingPane( pArrowRight, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_UPDATE_BUTTON, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_LEFT, SEQ_SE_SELECT1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_RIGHT, SEQ_SE_SELECT1 );
}


void JoinFestaAttractionSelectLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    m_pPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
      m_pPaneListData[i].base       = pBTNS;
      m_pPaneListData[i].text       = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_004_PANE_TEXTBOX_00, &resourceID );
      m_pPaneListData[i].bound      = pLower->GetBoundingPane( pBTNS, PANENAME_FESTA_BTN_LOW_004_PANE_BOUND_00, &resourceID );
      m_pPaneListData[i].cursor_pos = pLower->GetPane( pBTNS, PANENAME_FESTA_BTN_LOW_004_PANE_NULL_CUR, &resourceID );

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
    setPaneListParam( &list_setup, 0, 0.0f );

    app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
    {
      pAppHeap->GetDeviceHeap(),
      app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
      list_setup.valMax-list_setup.listPosMax,

      pLower,
      resourceID,
      PANENAME_JFPL_MIN_LOW_000_PANE_SCROLL_BAR,
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


void JoinFestaAttractionSelectLowerView::deletePaneList()
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


void JoinFestaAttractionSelectLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll )
{
  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax = ( m_pActiveList ) ? m_pActiveList->Size() : 1;					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 6;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 73.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる
  pParam->defSX = 306.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 28.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  pParam->listStartPane = NULL;   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	// コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_MenuCursor.GetAppToolMenuCursor();		// カーソル
}


void JoinFestaAttractionSelectLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
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

    f32 lx = 5.0f - pParam->defSX / 2.0f;
    f32 rx = lx + pParam->defSX;
    f32 uy = pParam->defPY + pos.y + pParam->defSY / 2.0f;
    f32 dy = uy - pParam->defSY * pos_max;
    pPaneList->MakeTouchArea( lx, rx, uy, dy );
  }

}


void JoinFestaAttractionSelectLowerView::setInputEnablePaneList( bool bInputEnable )
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


void JoinFestaAttractionSelectLowerView::backupPaneListCursorInfo()
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    
    if( pPaneList )
    {
      pPaneList->GetCursorData( &m_aPaneListCursorInfos[ m_eListType ].pos, &m_aPaneListCursorInfos[ m_eListType ].scroll );
    }
  }
}


void JoinFestaAttractionSelectLowerView::ResetPaneListCursorInfo()
{
  gfl2::std::MemClear( m_aPaneListCursorInfos, sizeof( m_aPaneListCursorInfos ) );
  m_bIsResetPaneListCursorInfo = true;
}


void JoinFestaAttractionSelectLowerView::SetOpenList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList* pList )
{
  setScreenTitleName( jf_playerlist_cap_18 );

  m_pActiveList = pList;
  m_eListType = LIST_TYPE_OPEN;

  UpdateList();
}


void JoinFestaAttractionSelectLowerView::SetEntryList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList* pList )
{
  setScreenTitleName( jf_playerlist_cap_19 );

  m_pActiveList = pList;
  m_eListType = LIST_TYPE_ENTRY;

  UpdateList();
}


void JoinFestaAttractionSelectLowerView::UpdateList()
{
  // 同じリストが設定される時、リストサイズに変動があったらカーソル位置をリセットする
  if( m_aPaneListCursorInfos[ m_eListType ].listMax > 0 )
  {
    if( m_aPaneListCursorInfos[ m_eListType ].listMax != m_pActiveList->Size() )
    {
      m_aPaneListCursorInfos[ m_eListType ].pos = 0;
      m_aPaneListCursorInfos[ m_eListType ].scroll = 0.0f;
    }
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  setPaneListParam( &list_setup, m_aPaneListCursorInfos[ m_eListType ].pos, m_aPaneListCursorInfos[ m_eListType ].scroll );
  m_pPaneListView->ResetPaneList( &list_setup );
  setPaneListConfig( &list_setup );
  m_aPaneListCursorInfos[ m_eListType ].listMax = m_pActiveList->Size();

  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->InitListPut();

  u32 index = 0;

  if( m_bIsResetPaneListCursorInfo )
  {
    index = 0;
    m_bIsResetPaneListCursorInfo = false;
  }
  else
  {
    index = pPaneList->GetPosPaneIndex( m_aPaneListCursorInfos[ m_eListType ].pos );
  }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pJoinFestaAttractionData = getJoinFestaAttractionData( index );

  if( m_pJoinFestaAttractionSelectLowerViewListener )
  {
    m_pJoinFestaAttractionSelectLowerViewListener->SetActiveAttraction( pJoinFestaAttractionData );
  }

}


::app::ui::UIInputListener::ListenerResult JoinFestaAttractionSelectLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaAttractionSelectLowerViewListener )
      {
        m_pJoinFestaAttractionSelectLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_UPDATE_BUTTON:
    {
      if( m_pJoinFestaAttractionSelectLowerViewListener )
      {
        m_pJoinFestaAttractionSelectLowerViewListener->TouchUpdateButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_ARROW_LEFT:
    {
      if( m_pJoinFestaAttractionSelectLowerViewListener )
      {
        m_pJoinFestaAttractionSelectLowerViewListener->PushLButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;
    
    case BUTTON_ID_ARROW_RIGHT:
    {
      if( m_pJoinFestaAttractionSelectLowerViewListener )
      {
        m_pJoinFestaAttractionSelectLowerViewListener->PushRButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;

  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaAttractionSelectLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_UPDATE_BUTTON );
    result = DISABLE_ACCESS;
  }
  else
  if( pKey->IsTrigger( gfl2::ui::DIR_LEFT ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_ARROW_LEFT );
    result = DISABLE_ACCESS;
  }
  else
  if( pKey->IsTrigger( gfl2::ui::DIR_RIGHT ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_ARROW_RIGHT );
    result = DISABLE_ACCESS;
  }

  return result;
}


void JoinFestaAttractionSelectLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pJoinFestaAttractionData = getJoinFestaAttractionData( pos );
  if( pJoinFestaAttractionData )
  {
    if( m_pJoinFestaAttractionSelectLowerViewListener )
    {
      m_pJoinFestaAttractionSelectLowerViewListener->SetActiveAttraction( pJoinFestaAttractionData );
    }
  }
}


void JoinFestaAttractionSelectLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void JoinFestaAttractionSelectLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pJoinFestaAttractionData = getJoinFestaAttractionData( pos );

  if( pJoinFestaAttractionData )
  {
    NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( pJoinFestaAttractionData->GetAttractionID() );

    pG2DUtil->SetTextBoxPaneString( m_pPaneListData[ pane_index ].text, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, static_cast<u32>( attractionData.attractionName ) ) );

    if( pJoinFestaAttractionData->IsPassive() )
    {
      setPassive( pane_index );
    }
    else
    {
      setActive( pane_index );
    }
  }
}


void JoinFestaAttractionSelectLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void JoinFestaAttractionSelectLowerView::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  backupPaneListCursorInfo();

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pJoinFestaAttractionData = getJoinFestaAttractionData( pos );

  if( pJoinFestaAttractionData )
  {
    if( m_pJoinFestaAttractionSelectLowerViewListener )
    {
      m_pJoinFestaAttractionSelectLowerViewListener->SetSelectAttraction( pJoinFestaAttractionData );
      m_pJoinFestaAttractionSelectLowerViewListener->SetActiveAttraction( pJoinFestaAttractionData );
    }
  }
}


NetAppLib::JoinFestaUI::JoinFestaAttractionData* JoinFestaAttractionSelectLowerView::getJoinFestaAttractionData( u32 index )
{
  if( m_pActiveList )
  {
    u32 count = 0;
    for( NetAppLib::JoinFestaUI::ItJoinFestaAttractionDataList it = m_pActiveList->Begin(); it != m_pActiveList->End(); ++it )
    {
      if( count == index )
      {
        return &(*it);
      }
      ++count;
    }
  }
  return NULL;
}


void JoinFestaAttractionSelectLowerView::SetPassive()
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      u32 cursorPos = 0;
      pPaneList->GetCursorData( &cursorPos, NULL );
      u32 paneIndex = pPaneList->GetPosPaneIndex( cursorPos );
      if( paneIndex < LIST_ITEM_PANE_MAX )
      {
        setPassive( paneIndex );
      }
    }
  }
}


void JoinFestaAttractionSelectLowerView::SetVisibleArrowIcon( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pArrowL = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_03 );
  gfl2::lyt::LytParts* pArrowR = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_04 );

  pArrowL->SetVisible( bVisible );
  pArrowR->SetVisible( bVisible );
}


void JoinFestaAttractionSelectLowerView::setActive( u32 paneIndex )
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      app::tool::ButtonManager::ButtonId buttonID = static_cast<app::tool::ButtonManager::ButtonId>( paneIndex );

      app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, active_passive_anm[ paneIndex ][0] );
      pPaneList->SetButtonActive( buttonID, false );
      pPaneList->SetButtonInputEnable( buttonID, true );
    }
  }
}


void JoinFestaAttractionSelectLowerView::setPassive( u32 paneIndex )
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      app::tool::ButtonManager::ButtonId buttonID = static_cast<app::tool::ButtonManager::ButtonId>( paneIndex );

      app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, active_passive_anm[ paneIndex ][1] );
      pPaneList->SetButtonPassive( buttonID, false );
      pPaneList->SetButtonInputEnable( buttonID, false );
    }
  }
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

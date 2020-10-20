// ============================================================================
/*
 * @file JoinFestaGlobalAttractionSelectListLowerView.cpp
 * @brief グローバルアトラクションアプリの下画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionSelectListLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_anim_list.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_pane.h>
#include <niji_conv_header/message/msg_jf_attraction.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


static const gfl2::lyt::LytPaneIndex btns_tbl[JoinFestaGlobalAttractionSelectListLowerView::LIST_ITEM_PANE_MAX] =
{
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_05,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_06,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_07,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_08,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_09,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_10,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_11,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_12,
  PANENAME_JFA_MIN_LOW_000_PANE_PARTS_13
};

static const u32 anm[JoinFestaGlobalAttractionSelectListLowerView::LIST_ITEM_PANE_MAX][4] =
{
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_05_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_05_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_05_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_05_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_06_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_06_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_06_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_06_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_07_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_07_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_07_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_07_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_08_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_08_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_08_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_08_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_09_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_09_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_09_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_09_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_10_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_10_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_10_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_10_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_11_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_11_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_11_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_11_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_12_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_12_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_12_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_12_TOUCH_RELEASE,
  },
  {
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_13_TOUCH,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_13_RELEASE,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_13_CANSEL,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_13_TOUCH_RELEASE,
  }
};


JoinFestaGlobalAttractionSelectListLowerView::JoinFestaGlobalAttractionSelectListLowerView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaGlobalAttractionWork,
    NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000_BFLYT,
    LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaGlobalAttractionWork->GetMessageUtility(),
    GARC_message_jf_attraction_DAT ),
  m_pJoinFestaGlobalAttractionWork( pJoinFestaGlobalAttractionWork ),
  m_PaneListCursorInfo(),
  m_pJoinFestaGlobalAttractionSelectListLowerViewListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_MenuCursor( pJoinFestaGlobalAttractionWork ),
  m_pDownloadGlobalAttractionInfoList( NULL ),
  m_DownloadGlobalAttractionInfoListSize( 0 ),
  m_bPaneListInputEnable( false )
{
  gfl2::std::MemClear( &m_PaneListCursorInfo, sizeof( PaneListCursorInfo ) );

  m_MenuCursor.Create( GetG2DUtil(), LAYOUT_RESOURCE_ID_JOINFESTAGLOBALATTRACTION, LAYOUT_WORK_ID_LOWER, JOINFESTAGLOBALATTRACTION_RESOURCE_ID_MENU_CURSOR );

  setupButton();

  createPaneList();

  SetInputListener( this );
}


JoinFestaGlobalAttractionSelectListLowerView::~JoinFestaGlobalAttractionSelectListLowerView()
{
  deletePaneList();

  m_MenuCursor.Destroy();
}


void JoinFestaGlobalAttractionSelectListLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_00_TOUCH,
      LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_00_RELEASE,
      LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_00_CANSEL,
      LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
      LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_00_ACTIVE,
      LA_JOIN_FESTA_ATTRACTION_JFA_MIN_LOW_000__PARTS_00_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAGLOBALATTRACTION );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_JFA_MIN_LOW_000_PANE_PARTS_00 );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pJoinFestaGlobalAttractionWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
}


void JoinFestaGlobalAttractionSelectListLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pJoinFestaGlobalAttractionWork->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAGLOBALATTRACTION );

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
      PANENAME_JFA_MIN_LOW_000_PANE_SCROLL_BAR,
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


void JoinFestaGlobalAttractionSelectListLowerView::deletePaneList()
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


void JoinFestaGlobalAttractionSelectListLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll )
{
  app::util::Heap* pAppHeap = m_pJoinFestaGlobalAttractionWork->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax = m_DownloadGlobalAttractionInfoListSize;					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 7;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 103.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
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


void JoinFestaGlobalAttractionSelectListLowerView::setInputEnablePaneList( bool bInputEnable )
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


void JoinFestaGlobalAttractionSelectListLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
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


void JoinFestaGlobalAttractionSelectListLowerView::SetList( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pDownloadGlobalAttractionInfoList, u32 downloadGlobalAttractionInfoListSize )
{
  m_pDownloadGlobalAttractionInfoList = pDownloadGlobalAttractionInfoList;
  m_DownloadGlobalAttractionInfoListSize = downloadGlobalAttractionInfoListSize;

  if( m_pDownloadGlobalAttractionInfoList && m_DownloadGlobalAttractionInfoListSize > 0 )
  {
    // 同じリストが設定される時、リストサイズに変動があったらカーソル位置をリセットする
    if( m_PaneListCursorInfo.listMax > 0 )
    {
      if( m_PaneListCursorInfo.listMax != m_DownloadGlobalAttractionInfoListSize )
      {
        m_PaneListCursorInfo.pos = 0;
        m_PaneListCursorInfo.scroll = 0.0f;
      }
    }

    app::tool::PaneList::SETUP_PARAM list_setup;
    setPaneListParam( &list_setup, m_PaneListCursorInfo.pos, m_PaneListCursorInfo.scroll );
    m_pPaneListView->ResetPaneList( &list_setup );
    setPaneListConfig( &list_setup );
    m_PaneListCursorInfo.listMax = m_DownloadGlobalAttractionInfoListSize;
  }
}


void JoinFestaGlobalAttractionSelectListLowerView::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MenuCursor.Update();
}


void JoinFestaGlobalAttractionSelectListLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaGlobalAttractionWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_MenuCursor.Draw( displayNo );
}


bool JoinFestaGlobalAttractionSelectListLowerView::IsDrawing(void) const
{
  bool result = false;
  
  if( !m_MenuCursor.CanDestroy() )
  {
    result = true;
  }

  if( UIView::IsDrawing() )
  {
    result = true;
  }

  return result;
}


void JoinFestaGlobalAttractionSelectListLowerView::OnAddChild()
{
  setInputEnablePaneList( false );
}


void JoinFestaGlobalAttractionSelectListLowerView::OnRemoveChild()
{
  setInputEnablePaneList( true );
}


::app::ui::UIInputListener::ListenerResult JoinFestaGlobalAttractionSelectListLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaGlobalAttractionSelectListLowerViewListener )
      {
        m_pJoinFestaGlobalAttractionSelectListLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaGlobalAttractionSelectListLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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

  return result;
}


void JoinFestaGlobalAttractionSelectListLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo = getGlobalAttractionInfo( pos );

  if( pGlobalAttractionInfo )
  {
    if( m_pJoinFestaGlobalAttractionSelectListLowerViewListener )
    {
      m_pJoinFestaGlobalAttractionSelectListLowerViewListener->SetActiveGlobalAttractionInfo( pGlobalAttractionInfo );
    }
  }
}


void JoinFestaGlobalAttractionSelectListLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void JoinFestaGlobalAttractionSelectListLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo = getGlobalAttractionInfo( pos );

  if( pGlobalAttractionInfo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaGlobalAttractionWork->GetMessageUtility();

    pG2DUtil->SetTextBoxPaneString( m_pPaneListData[ pane_index ].text, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, m_pJoinFestaGlobalAttractionWork->GetAttractionNameMassageID( pGlobalAttractionInfo ) ) );
  }

}


void JoinFestaGlobalAttractionSelectListLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void JoinFestaGlobalAttractionSelectListLowerView::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    
    if( pPaneList )
    {
      pPaneList->GetCursorData( &m_PaneListCursorInfo.pos, &m_PaneListCursorInfo.scroll );
    }
  }

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo = getGlobalAttractionInfo( pos );

  if( pGlobalAttractionInfo )
  {
    if( m_pJoinFestaGlobalAttractionSelectListLowerViewListener )
    {
      m_pJoinFestaGlobalAttractionSelectListLowerViewListener->SelectGlobalAttractionInfo( pGlobalAttractionInfo );
      m_pJoinFestaGlobalAttractionSelectListLowerViewListener->SetActiveGlobalAttractionInfo( pGlobalAttractionInfo );
    }
  }

}


NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* JoinFestaGlobalAttractionSelectListLowerView::getGlobalAttractionInfo( u32 pos )
{
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo = NULL;

  if( m_pDownloadGlobalAttractionInfoList )
  {
    if( pos < m_DownloadGlobalAttractionInfoListSize )
    {
      pGlobalAttractionInfo = &m_pDownloadGlobalAttractionInfoList[ pos ];
    }
  }

  return pGlobalAttractionInfo;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

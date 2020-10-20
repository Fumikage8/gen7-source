// ============================================================================
/*
 * @file JoinFestaRankingLowerView.cpp
 * @brief アンケート下画面
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "Sound/include/Sound.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)

static const u32 RANK_ANIME_INVALID = 0xffffffff;

static const gfl2::lyt::LytPaneIndex BasePaneIDTable[JoinFestaRankingLowerView::LIST_ITEM_PANE_MAX] =
{
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_00,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_01,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_02,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_03,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_04,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_05,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_06,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_07,
  PANENAME_JFPL_MIN_LOW_000_PANE_RANKINGPLATE_08
};


JoinFestaRankingLowerView::JoinFestaRankingLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::System::ResourceID layoutResourceID,
  NetAppLib::System::ResourceID menuCursorResourceID ) :
  NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBase( pWorkBase, layoutResourceID, menuCursorResourceID  ),
  m_pJoinFestaRankingLowerViewListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_pMyPaneListData( NULL ),
  m_pActiveList( NULL ),
  m_bPaneListInputEnable( false )
{
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000_LAYOUT_04 );
  setupButton();

  createPaneList();

  // 不要な矢印アイコンを非表示
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

    gfl2::lyt::LytParts* pArrowL = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_03 );
    gfl2::lyt::LytParts* pArrowR = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_04 );
    gfl2::lyt::LytParts* pArrowU = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_13 );
    gfl2::lyt::LytParts* pArrowD = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_14 );

    pArrowL->SetVisible( false );
    pArrowR->SetVisible( false );
    pArrowU->SetVisible( false );
    pArrowD->SetVisible( false );
  }

  SetInputListener( this );
}


JoinFestaRankingLowerView::~JoinFestaRankingLowerView()
{
  deletePaneList();
}


void JoinFestaRankingLowerView::Update()
{
  JoinFestaPlayerListLowerViewBase::Update();
}


void JoinFestaRankingLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  JoinFestaPlayerListLowerViewBase::Draw( displayNo );
}


bool JoinFestaRankingLowerView::IsDrawing(void) const
{
  return JoinFestaPlayerListLowerViewBase::IsDrawing();
}


void JoinFestaRankingLowerView::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void JoinFestaRankingLowerView::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


void JoinFestaRankingLowerView::setupButton()
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
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_LEFT, SEQ_SE_SELECT1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_RIGHT, SEQ_SE_SELECT1 );

}


void JoinFestaRankingLowerView::createPaneList()
{
  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    m_pPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
    m_pMyPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) MyPaneData[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pPartsPane = pLower->GetPartsPane( BasePaneIDTable[i] );

      m_pPaneListData[i].base         = pPartsPane;
      m_pPaneListData[i].text         = NULL;
      m_pPaneListData[i].bound        = pLower->GetBoundingPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_BOUNDING_00, &resourceID );
      m_pPaneListData[i].cursor_pos   = pLower->GetPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_CUR_MAIN, &resourceID );

      m_pPaneListData[i].holdAnime    = app::tool::ButtonManager::ANIMATION_NULL;
      m_pPaneListData[i].releaseAnime = app::tool::ButtonManager::ANIMATION_NULL;
      m_pPaneListData[i].cancelAnime  = app::tool::ButtonManager::ANIMATION_NULL;
      m_pPaneListData[i].selectAnime  = app::tool::ButtonManager::ANIMATION_NULL;

      m_pMyPaneListData[i].pRankIcon1 = pLower->GetPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_FESTA_ICON_00, &resourceID ); // 1位:PANENAME_FESTA_PLATE_LOW_001_PANE_FESTA_ICON_00
      m_pMyPaneListData[i].pRankIcon2 = pLower->GetPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_FESTA_ICON_02, &resourceID ); // 2位:PANENAME_FESTA_PLATE_LOW_001_PANE_FESTA_ICON_02(2位と3位が逆なので注意)
      m_pMyPaneListData[i].pRankIcon3 = pLower->GetPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_FESTA_ICON_01, &resourceID ); // 3位:PANENAME_FESTA_PLATE_LOW_001_PANE_FESTA_ICON_01(2位と3位が逆なので注意)
      m_pMyPaneListData[i].pTextRank  = pLower->GetTextBoxPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_RANK_00, &resourceID );
      m_pMyPaneListData[i].pTextName  = pLower->GetTextBoxPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_PLAYER_NAME_00, &resourceID );
      m_pMyPaneListData[i].pTextScore = pLower->GetTextBoxPane( pPartsPane, PANENAME_FESTA_PLATE_LOW_001_PANE_SCORE_00, &resourceID );
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


void JoinFestaRankingLowerView::deletePaneList()
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


void JoinFestaRankingLowerView::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, u32 scroll )
{
  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax = ( m_pActiveList ) ? m_pActiveList->Size() : 1;					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 7;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 75.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる
  pParam->defSX = 220.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 30.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  pParam->listStartPane = NULL;   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	// コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_MenuCursor.GetAppToolMenuCursor();
}


void JoinFestaRankingLowerView::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
{
  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_BUTTON, true, false, true, false );
  pPaneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, app::tool::PaneList::SE_NONE );

  { // タッチ範囲
    gfl2::math::VEC3 pos = m_pPaneListData[0].bound->GetTranslate();
    u32 pos_max = pParam->listPosMax;
    if( pParam->valMax < pos_max )
    {
      pos_max = pParam->valMax;
    }

    f32 lx = 0.0f - pParam->defSX / 2.0f;
    f32 rx = lx + pParam->defSX;
    f32 uy = pParam->defPY + pos.y + pParam->defSY / 2.0f;
    f32 dy = uy - pParam->defSY * pos_max;
    pPaneList->MakeTouchArea( lx, rx, uy, dy );
  }

}


void JoinFestaRankingLowerView::setInputEnablePaneList( bool bInputEnable )
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


void JoinFestaRankingLowerView::SetList( NetAppLib::JoinFestaUI::JoinFestaRankingDataList* pList )
{
  setScreenTitleName( jf_playerlist_cap_59 );

  m_pActiveList = pList;

  UpdateList();

}


void JoinFestaRankingLowerView::UpdateList()
{
  app::tool::PaneList::SETUP_PARAM list_setup;
  setPaneListParam( &list_setup, 0, 0 );
  m_pPaneListView->ResetPaneList( &list_setup );
  setPaneListConfig( &list_setup );

  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->InitListPut();
}


::app::ui::UIInputListener::ListenerResult JoinFestaRankingLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaRankingLowerViewListener )
      {
        m_pJoinFestaRankingLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_ARROW_LEFT:
    {
      if( m_pJoinFestaRankingLowerViewListener )
      {
        m_pJoinFestaRankingLowerViewListener->PushLButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
    
    case BUTTON_ID_ARROW_RIGHT:
    {
      if( m_pJoinFestaRankingLowerViewListener )
      {
        m_pJoinFestaRankingLowerViewListener->PushRButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaRankingLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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


void JoinFestaRankingLowerView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
}


void JoinFestaRankingLowerView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void JoinFestaRankingLowerView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  NetAppLib::JoinFestaUI::JoinFestaRankingData* pJoinFestaRankingData = getJoinFestaRankingData( pos );
  
  if( pJoinFestaRankingData )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

    m_pMyPaneListData[ pane_index ].pRankIcon1->SetVisible( false );
    m_pMyPaneListData[ pane_index ].pRankIcon2->SetVisible( false );
    m_pMyPaneListData[ pane_index ].pRankIcon3->SetVisible( false );

    switch( pJoinFestaRankingData->rank )
    {
      case 1: m_pMyPaneListData[ pane_index ].pRankIcon1->SetVisible( true ); break;
      case 2: m_pMyPaneListData[ pane_index ].pRankIcon2->SetVisible( true ); break;
      case 3: m_pMyPaneListData[ pane_index ].pRankIcon3->SetVisible( true ); break;
    }

    setText( pJoinFestaRankingData->rank, pJoinFestaRankingData->score, &pJoinFestaRankingData->name[0],
      m_pMyPaneListData[ pane_index ].pTextRank, m_pMyPaneListData[ pane_index ].pTextScore, m_pMyPaneListData[ pane_index ].pTextName );


  }
}


void JoinFestaRankingLowerView::setText( s32 rank, s32 score, gfl2::str::STRCODE* pName,
  gfl2::lyt::LytTextBox* pRankText, gfl2::lyt::LytTextBox* pScoreText, gfl2::lyt::LytTextBox* pNameText )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  pG2DUtil->SetRegisterNumber( 0, static_cast<u32>( rank ), 3 );
  pG2DUtil->SetTextBoxPaneStringExpand( pRankText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_52 ) );

  gfl2::str::StrBuf* pTempStrBuf = pG2DUtil->GetTempStrBuf(0);
  pTempStrBuf->SetStr( pName );
  pG2DUtil->SetRegisterWord( 0, *pTempStrBuf );
  pG2DUtil->SetTextBoxPaneStringExpand( pNameText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_01 ) );

  pG2DUtil->SetRegisterNumber( 0, static_cast<u32>( score ), 3 );
  pG2DUtil->SetTextBoxPaneStringExpand( pScoreText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_54 ) );
}


void JoinFestaRankingLowerView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void JoinFestaRankingLowerView::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  if( m_pJoinFestaRankingLowerViewListener )
  {
    m_pJoinFestaRankingLowerViewListener->TouchBackButton();
  }

}


NetAppLib::JoinFestaUI::JoinFestaRankingData* JoinFestaRankingLowerView::getJoinFestaRankingData( u32 index )
{
  if( m_pActiveList )
  {
    u32 count = 0;
    for( NetAppLib::JoinFestaUI::ItJoinFestaRankingDataList it = m_pActiveList->Begin(); it != m_pActiveList->End(); ++it )
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


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

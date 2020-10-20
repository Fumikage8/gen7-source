// ============================================================================
/*
 * @file GTSSearchOptionLowerView.cpp
 * @brief 検索オプション画面を表示するクラスです。
 * @date 2015.05.11
 */
// ============================================================================
#include "NetApp/GTS/source/SearchPokemon/GTSSearchOptionLowerView.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SearchPokemon)

static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_GTS_SLC_LOW_000_PANE_BTNS_00,
  PANENAME_GTS_SLC_LOW_000_PANE_BTNS_01,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_GTS_BTN_LOW_001_PANE_NULL_CUR,
  PANENAME_GTS_BTN_LOW_001_PANE_NULL_CUR,
};


GTSSearchOptionLowerView::GTSSearchOptionLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pSearchOptionDrawListener( NULL )
{
  setupTextMessage();

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BTNS_00,
    BUTTON_ID_BTNS_01,
  };

  CreateCursor( NetApp::GTS::GTS_RESOURCE_ID_MENU_CURSOR,
                LAYOUT_RESOURCE_ID_GTS,
                LAYOUT_WORK_ID_LOWER,
                buttonPartsPaneIDTable,
                cursorPaneIDTable,
                buttonIDTable,
                GFL_NELEMS( buttonPartsPaneIDTable ) );


  setupButton();

  SetVisibleBackButton( false );
  SetVisibleBlackFilter( false );
  SetInputListener( this );
}


GTSSearchOptionLowerView::~GTSSearchOptionLowerView()
{
  ReleaseCursor();
}


void GTSSearchOptionLowerView::setupTextMessage()
{
  // 下画面のTextを置き換え
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    
    setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_SLC_LOW_000_PATERN_08 );

    // 画面上のテキスト
    {
      gfl2::lyt::LytParts* pMsgL2 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_MSG_L2 );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMsgL2, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, &gtsResourceID );
      pTextBox->SetVisible( true );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, msg_gts_win_12 );
    }

    // 『探す』ボタン
    {
      gfl2::lyt::LytParts* pBtnS_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_00 );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnS_00, PANENAME_GTS_BTN_LOW_001_PANE_TEXTBOX_00, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_27 );
    }

    // 『探さない』ボタン
    {
      gfl2::lyt::LytParts* pBtnS_01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_01 );
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnS_01, PANENAME_GTS_BTN_LOW_001_PANE_TEXTBOX_00, &gtsResourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_28 );
    }
  }
}


void GTSSearchOptionLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BTNS_00, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_00_PASSIVE,
    },
    {
      BUTTON_ID_BTNS_01, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNS_01_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNS_01_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_01_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNS_01_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_01_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNS_01_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnS00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_00 );
    buttonInfoTable[ BUTTON_ID_BTNS_00 ].picture_pane = pBtnS00;
    buttonInfoTable[ BUTTON_ID_BTNS_00 ].bound_pane = pLower->GetBoundingPane( pBtnS00, PANENAME_GTS_BTN_LOW_001_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnS01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNS_01 );
    buttonInfoTable[ BUTTON_ID_BTNS_01 ].picture_pane = pBtnS01;
    buttonInfoTable[ BUTTON_ID_BTNS_01 ].bound_pane = pLower->GetBoundingPane( pBtnS01, PANENAME_GTS_BTN_LOW_001_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNS_00, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNS_01, SEQ_SE_DECIDE1 );

}


void GTSSearchOptionLowerView::Update()
{
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Update();
}


void GTSSearchOptionLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult GTSSearchOptionLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    //『探す』ボタン
    case BUTTON_ID_BTNS_00:
    {
      if( m_pSearchOptionDrawListener )
      {
        m_pSearchOptionDrawListener->TouchSearchButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    //『探さない』ボタン
    case BUTTON_ID_BTNS_01:
    {
      if( m_pSearchOptionDrawListener )
      {
        m_pSearchOptionDrawListener->TouchNotSearchButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


GFL_NAMESPACE_END(SearchPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

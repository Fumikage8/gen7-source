// ============================================================================
/*
 * @file GTSTradePersonRefineSearchLowerView.cpp
 * @brief 交換相手を絞り込む画面を表示するクラスです。
 * @date 2015.04.22
 */
// ============================================================================
#include "NetApp/GTS/source/TradePersonRefineSearch/GTSTradePersonRefineSearchLowerView.h"
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
GFL_NAMESPACE_BEGIN(TradePersonRefineSearch)


static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_GTS_SLC_LOW_000_PANE_BTNL_00,
  PANENAME_GTS_SLC_LOW_000_PANE_BTNL_01,
  PANENAME_GTS_SLC_LOW_000_PANE_BTN_GREEN,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_GTS_BTN_LOW_003_PANE_NULL_CUR,
  PANENAME_GTS_BTN_LOW_003_PANE_NULL_CUR,
  PANENAME_GTS_BTN_LOW_001_PANE_NULL_CUR,
};


GTSTradePersonRefineSearchLowerView::GTSTradePersonRefineSearchLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pTradePersonRefineSearchDrawListener( NULL )
{
  setupTextMessage();

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BTNL_00,
    BUTTON_ID_BTNL_01,
    BUTTON_ID_BTN_GREEN,
  };

  CreateCursor( NetApp::GTS::GTS_RESOURCE_ID_MENU_CURSOR,
                LAYOUT_RESOURCE_ID_GTS,
                LAYOUT_WORK_ID_LOWER,
                buttonPartsPaneIDTable,
                cursorPaneIDTable,
                buttonIDTable,
                GFL_NELEMS( buttonPartsPaneIDTable ) );

  setupButton();

  SetVisibleBackButton( true );
  SetVisibleBlackFilter( false );
  SetInputListener( this );
}


GTSTradePersonRefineSearchLowerView::~GTSTradePersonRefineSearchLowerView()
{
  ReleaseCursor();
}


void GTSTradePersonRefineSearchLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_SLC_LOW_000_PATERN_05 );

  {
    gfl2::lyt::LytParts* pMsgL1 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_MSG_L1 );
    gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMsgL1, PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, &gtsResourceID );
    pTextBox->SetVisible( true );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, msg_gts_win_21 ); //『条件を選んで下さい』
  }

  {
    gfl2::lyt::LytParts* pBtnL_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_00 );
    gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnL_00, PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_00, &gtsResourceID );
    gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnL_00, PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_01, &gtsResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_29 ); //『相手が欲しいポケモン』
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, msg_gts_sel_30 ); //『気にしない』
  }

  {
    gfl2::lyt::LytParts* pBtnL_01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_01 );
    gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnL_01, PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_00, &gtsResourceID );
    gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnL_01, PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_01, &gtsResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_32 ); //『地域』
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, msg_gts_sel_30 ); //『気にしない』
  }

  {
    gfl2::lyt::LytParts* pBtnGreen = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_GREEN );
    gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pBtnGreen, PANENAME_GTS_BTN_LOW_001_PANE_TEXTBOX_00, &gtsResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sel_35 ); //『この条件で絞り込む』
  }

}


void GTSTradePersonRefineSearchLowerView::SetRequestPokemonMessage( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnL_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_00 );
  gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnL_00, PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_01, &gtsResourceID );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, messageID );
}


void GTSTradePersonRefineSearchLowerView::SetAreaMessage( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnL_01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_01 );
  gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( pBtnL_01, PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_01, &gtsResourceID );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_01, messageID );
}


void GTSTradePersonRefineSearchLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BTN_BACK, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTN_BACK_PASSIVE,
    },
    {
      BUTTON_ID_BTNL_00, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNL_00_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNL_00_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_00_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNL_00_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_00_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_00_PASSIVE,
    },
    {
      BUTTON_ID_BTNL_01, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTNL_01_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTNL_01_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_01_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTNL_01_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_01_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTNL_01_PASSIVE,
    },
    {
      BUTTON_ID_BTN_GREEN, NULL, NULL,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_TOUCH,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_CANSEL,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_TOUCH_RELEASE,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_ACTIVE,
      LA_GTS_GTS_SLC_LOW_000__BTN_GREEN_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_BACK );
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnL_00 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_00 );
    buttonInfoTable[ BUTTON_ID_BTNL_00 ].picture_pane = pBtnL_00;
    buttonInfoTable[ BUTTON_ID_BTNL_00 ].bound_pane = pLower->GetBoundingPane( pBtnL_00, PANENAME_GTS_BTN_LOW_003_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnL_01 = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTNL_01 );
    buttonInfoTable[ BUTTON_ID_BTNL_01 ].picture_pane = pBtnL_01;
    buttonInfoTable[ BUTTON_ID_BTNL_01 ].bound_pane = pLower->GetBoundingPane( pBtnL_01, PANENAME_GTS_BTN_LOW_003_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnGreen = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_GREEN );
    buttonInfoTable[ BUTTON_ID_BTN_GREEN ].picture_pane = pBtnGreen;
    buttonInfoTable[ BUTTON_ID_BTN_GREEN ].bound_pane = pLower->GetBoundingPane( pBtnGreen, PANENAME_GTS_BTN_LOW_001_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNL_00, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTNL_01, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_GREEN, SEQ_SE_DECIDE1 );

  RegisterShortcut_B( BUTTON_ID_BTN_BACK );
}


//-----------------------------------------------------------------------------
// UIView
//-----------------------------------------------------------------------------
void GTSTradePersonRefineSearchLowerView::OnAddChild()
{
  SetCursorVisible( false );
}


void GTSTradePersonRefineSearchLowerView::OnAddedToParent()
{
  SetCursorVisible( true );
}


void GTSTradePersonRefineSearchLowerView::OnRemoveChild()
{
  SetCursorVisible( true );
}


void GTSTradePersonRefineSearchLowerView::OnRemovedFromParent()
{
  SetCursorVisible( false );
}


void GTSTradePersonRefineSearchLowerView::Update( void )
{
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Update();
}


void GTSTradePersonRefineSearchLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Draw( displayNo );
}


//-----------------------------------------------------------------------------
// UIInputListener
//-----------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult GTSTradePersonRefineSearchLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    //『戻る』ボタン
    case BUTTON_ID_BTN_BACK:
    {
      if( m_pTradePersonRefineSearchDrawListener )
      {
        m_pTradePersonRefineSearchDrawListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BTNL_00:
    {
      if( m_pTradePersonRefineSearchDrawListener )
      {
        m_pTradePersonRefineSearchDrawListener->TouchRequestPokemonButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BTNL_01:
    {
      if( m_pTradePersonRefineSearchDrawListener )
      {
        m_pTradePersonRefineSearchDrawListener->TouchAreaButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BTN_GREEN:
    {
      if( m_pTradePersonRefineSearchDrawListener )
      {
        m_pTradePersonRefineSearchDrawListener->TouchRefineSearchButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


GFL_NAMESPACE_END(TradePersonRefineSearch)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file MiracleTradeStartConfirmLowerView.cpp
 * @brief ミラクル交換の開始確認を行う画面を表示するクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmLowerView.h"
#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/miracletrade/MiracleTrade.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_anim_list.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_pane.h>
#include <niji_conv_header/message/msg_miracletrade.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(StartConfirm)


static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_MIRACLETRADE_MIN_LOW_PANE_BUTTON_START,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_COMMON_BTN_LOW_004_PANE_NULL_CUR,
};


MiracleTradeStartConfirmLowerView::MiracleTradeStartConfirmLowerView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ) :
  NetAppLib::UI::NetAppCursorView( pMiracleTradeWork ),
  m_pMiracleTradeWork( pMiracleTradeWork ),
  m_pMiracleTradeStartConfirmLowerViewListener( NULL ),
  m_MessageWindowManipulator()
{
  Create2DOneLayout( pMiracleTradeWork,
                     NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_LAYOUT,
                     LYTRES_MIRACLETRADE_MIRACLETRADE_MIN_LOW_BFLYT,
                     LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW___NUM,
                     app::util::G2DUtil::SETUP_LOWER,
                     pMiracleTradeWork->GetMessageUtility(),
                     GARC_message_miracletrade_DAT );

  setupMessageWindowManipulator();
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW_PATERN_00 );
  setupTextMessage();

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BUTTON_START,
  };
  CreateCursor( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_MENU_CURSOR,
                LAYOUT_RESOURCE_ID_MIRACLE_TRADE,
                LAYOUT_WORK_ID_LOWER,
                buttonPartsPaneIDTable,
                cursorPaneIDTable,
                buttonIDTable,
                GFL_NELEMS( buttonPartsPaneIDTable ) );

  setupButton();

  SetInputListener( this );
}


MiracleTradeStartConfirmLowerView::~MiracleTradeStartConfirmLowerView()
{
  ReleaseCursor();
}


void MiracleTradeStartConfirmLowerView::setupMessageWindowManipulator()
{
  NetAppLib::UI::NetAppCommonMessageWindowManipulator::SSetupInfo setupInfo;

  setupInfo.pHeap                = m_pMiracleTradeWork->GetAppHeap();
  setupInfo.pTargetView          = this;
  setupInfo.layoutWorkID         = LAYOUT_WORK_ID_LOWER;
  setupInfo.pHandler             = this;
  setupInfo.animID_TimerIcon     = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__TIMERICON_KEEP;
  setupInfo.animID_TimerIcon_In  = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__TIMERICON_IN;
  setupInfo.animID_TimerIcon_Out = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__TIMERICON_OUT;
  setupInfo.animID_MsgCursor     = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__CURSOR_00_KEEP;
  setupInfo.paneID_WindowParts   = PANENAME_MIRACLETRADE_MIN_LOW_PANE_MESSAGE;
  setupInfo.paneID_TextBox2      = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
  setupInfo.paneID_NullText1     = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
  setupInfo.paneID_TextBox1a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
  setupInfo.paneID_TextBox2a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;
  setupInfo.paneID_TimerIcon     = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
  setupInfo.paneID_MsgCursor     = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

  m_MessageWindowManipulator.Setup( setupInfo );

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pMiracleTradeWork->GetMessageUtility();
  m_MessageWindowManipulator.ShowMessage( &pMessageUtility->GetString( GARC_message_miracletrade_DAT, msg_miracletrade_win_01 ), true, false );
}


void MiracleTradeStartConfirmLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID miracleTradeResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_MIRACLE_TRADE );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );


  // 開始ボタン
  {
    gfl2::lyt::LytParts* pButtonStart = pLower->GetPartsPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_BUTTON_START );
    gfl2::lyt::LytTextBox* pTextBox00 = pLower->GetTextBoxPane( pButtonStart, PANENAME_COMMON_BTN_LOW_004_PANE_TEXTBOX_00, &miracleTradeResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox00, msg_miracletrade_sel_01 );
  }

}


void MiracleTradeStartConfirmLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BUTTON_BACK, NULL, NULL,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_BACK_TOUCH,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_BACK_RELEASE,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_BACK_CANSEL,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_BACK_TOUCH_RELEASE,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_BACK_ACTIVE,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_BACK_PASSIVE,
    },
    {
      BUTTON_ID_BUTTON_START, NULL, NULL,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_START_TOUCH,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_START_RELEASE,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_START_CANSEL,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_START_TOUCH_RELEASE,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_START_ACTIVE,
      LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__BUTTON_START_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID miracleTradeResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_MIRACLE_TRADE );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pButtonBack = pLower->GetPartsPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_BUTTON_BACK );
    buttonInfoTable[ BUTTON_ID_BUTTON_BACK ].picture_pane = pButtonBack;
    buttonInfoTable[ BUTTON_ID_BUTTON_BACK ].bound_pane = pLower->GetBoundingPane( pButtonBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &miracleTradeResourceID );
  }

  {
    gfl2::lyt::LytParts* pButtonStart = pLower->GetPartsPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_BUTTON_START );
    buttonInfoTable[ BUTTON_ID_BUTTON_START ].picture_pane = pButtonStart;
    buttonInfoTable[ BUTTON_ID_BUTTON_START ].bound_pane = pLower->GetBoundingPane( pButtonStart, PANENAME_COMMON_BTN_LOW_004_PANE_BOUND_00, &miracleTradeResourceID );
  }

  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BUTTON_BACK, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BUTTON_START, SEQ_SE_DECIDE1 );

  RegisterShortcut_B( BUTTON_ID_BUTTON_BACK );
}


void MiracleTradeStartConfirmLowerView::Update( void )
{
  NetAppLib::UI::NetAppCursorView::Update();
  m_MessageWindowManipulator.UpdateWindow();
}


void MiracleTradeStartConfirmLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pMiracleTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetAppLib::UI::NetAppCursorView::Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult MiracleTradeStartConfirmLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BUTTON_BACK:
      if( m_pMiracleTradeStartConfirmLowerViewListener )
      {
        m_pMiracleTradeStartConfirmLowerViewListener->OnTouchBackButtonFromMiracleTradeStartConfirmLower();
      }
      result = DISABLE_ACCESS;
    break;

    case BUTTON_ID_BUTTON_START:
      if( m_pMiracleTradeStartConfirmLowerViewListener )
      {
        m_pMiracleTradeStartConfirmLowerViewListener->OnTouchStartButton();
      }
      result = DISABLE_ACCESS;
    break;

  }

  return result;
}


void MiracleTradeStartConfirmLowerView::CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID event )
{
}


GFL_NAMESPACE_END(StartConfirm)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

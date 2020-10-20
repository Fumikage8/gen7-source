// ============================================================================
/*
 * @file ConnectionLowerView.cpp
 * @brief インターネット接続アプリの下画面を表示するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/View/ConnectionLowerView.h"
#include "NetApp/Connection/source/ConnectionResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/net/net.h>
#include <niji_conv_header/app/net/net_anim_list.h>
#include <niji_conv_header/app/net/net_pane.h>
#include <niji_conv_header/message/msg_netconnect.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(View)


static const u32 buttonPartsPaneIDTable[] =
{
  PANENAME_NET_MIN_LOW_000_PANE_BUTTON_00,
  PANENAME_NET_MIN_LOW_000_PANE_BUTTON_01,
};

static const u32 cursorPaneIDTable[] =
{
  PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
  PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
};


ConnectionLowerView::ConnectionLowerView( NetApp::Connection::System::ConnectionWork* pConnectionWork ) :
  NetAppLib::UI::NetAppCursorView( pConnectionWork ),
  m_pConnectionWork( pConnectionWork ),
  m_pConnectionLowerViewListener( NULL ),
  m_MessageWindowManipulator(),
  m_MessageID( 0xffffffff ),
  m_bYesNoVisible( false ),
  m_bIsOpenAlertMessage( false ),
  m_bIsPushButtonA( false )
{
  Create2DOneLayout( pConnectionWork,
                     NetApp::Connection::CONNECTION_RESOURCE_ID_LAYOUT,
                     LYTRES_NET_NET_MIN_LOW_000_BFLYT,
                     LA_NET_NET_MIN_LOW_000___NUM,
                     app::util::G2DUtil::SETUP_LOWER,
                     pConnectionWork->GetMessageUtility(),
                     GARC_message_netconnect_DAT );


  setupMessageWindowManipulator();

  const app::tool::ButtonManager::ButtonId  buttonIDTable[] =
  {
    BUTTON_ID_BUTTON_00,
    BUTTON_ID_BUTTON_01,
  };

  CreateCursor( NetApp::Connection::CONNECTION_RESOURCE_ID_MENU_CURSOR,
                LAYOUT_RESOURCE_ID_CONNECTION,
                LAYOUT_WORK_ID_LOWER,
                buttonPartsPaneIDTable,
                cursorPaneIDTable,
                buttonIDTable,
                GFL_NELEMS( buttonPartsPaneIDTable ) );

  setupButton();

  setupLayout( LAYOUT_WORK_ID_LOWER, LA_NET_NET_MIN_LOW_000__BG_00_BG_LOOP_00 );

  SetInputListener( this );
}


ConnectionLowerView::~ConnectionLowerView()
{
  RemoveFromSuperView();

  ReleaseCursor();
}


void ConnectionLowerView::setupMessageWindowManipulator()
{
  NetAppLib::UI::NetAppCommonMessageWindowManipulator::SSetupInfo setupInfo;

  setupInfo.pHeap                = m_pConnectionWork->GetAppHeap();
  setupInfo.pTargetView          = this;
  setupInfo.layoutWorkID         = LAYOUT_WORK_ID_LOWER;
  setupInfo.pHandler             = this;
  setupInfo.animID_TimerIcon     = LA_NET_NET_MIN_LOW_000__MESSAGE_L2__TIMERICON_KEEP;
  setupInfo.animID_TimerIcon_In  = LA_NET_NET_MIN_LOW_000__MESSAGE_L2__TIMERICON_IN;
  setupInfo.animID_TimerIcon_Out = LA_NET_NET_MIN_LOW_000__MESSAGE_L2__TIMERICON_OUT;
  setupInfo.animID_MsgCursor     = LA_NET_NET_MIN_LOW_000__MESSAGE_L2__CURSOR_00_KEEP;
  setupInfo.paneID_WindowParts   = PANENAME_NET_MIN_LOW_000_PANE_MESSAGE_L2;
  setupInfo.paneID_TextBox2      = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
  setupInfo.paneID_NullText1     = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
  setupInfo.paneID_TextBox1a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
  setupInfo.paneID_TextBox2a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;
  setupInfo.paneID_TimerIcon     = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
  setupInfo.paneID_MsgCursor     = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

  m_MessageWindowManipulator.Setup( setupInfo );
}


void ConnectionLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BUTTON_00, NULL, NULL,
      LA_NET_NET_MIN_LOW_000__BUTTON_00_TOUCH,
      LA_NET_NET_MIN_LOW_000__BUTTON_00_RELEASE,
      LA_NET_NET_MIN_LOW_000__BUTTON_00_CANSEL,
      LA_NET_NET_MIN_LOW_000__BUTTON_00_TOUCH_RELEASE,
      LA_NET_NET_MIN_LOW_000__BUTTON_00_ACTIVE,
      LA_NET_NET_MIN_LOW_000__BUTTON_00_PASSIVE,
    },

    {
      BUTTON_ID_BUTTON_01, NULL, NULL,
      LA_NET_NET_MIN_LOW_000__BUTTON_01_TOUCH,
      LA_NET_NET_MIN_LOW_000__BUTTON_01_RELEASE,
      LA_NET_NET_MIN_LOW_000__BUTTON_01_CANSEL,
      LA_NET_NET_MIN_LOW_000__BUTTON_01_TOUCH_RELEASE,
      LA_NET_NET_MIN_LOW_000__BUTTON_01_ACTIVE,
      LA_NET_NET_MIN_LOW_000__BUTTON_01_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID connectionResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_CONNECTION );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pButton00 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_00 );
    buttonInfoTable[ BUTTON_ID_BUTTON_00 ].picture_pane = pButton00;
    buttonInfoTable[ BUTTON_ID_BUTTON_00 ].bound_pane = pLower->GetBoundingPane( pButton00, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &connectionResourceID );
  }

  {
    gfl2::lyt::LytParts* pButton01 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_01 );
    buttonInfoTable[ BUTTON_ID_BUTTON_01 ].picture_pane = pButton01;
    buttonInfoTable[ BUTTON_ID_BUTTON_01 ].bound_pane = pLower->GetBoundingPane( pButton01, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &connectionResourceID );
  }

  app::util::Heap* pAppHeap = m_pConnectionWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BUTTON_00, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BUTTON_01, SEQ_SE_CANCEL1 );

  RegisterShortcut_B( BUTTON_ID_BUTTON_01 );
}


void ConnectionLowerView::SetFatalErrorMode( u32 messageID )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pConnectionWork->GetMessageUtility();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID connectionResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_CONNECTION );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pButton00 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_00 );
  
  gfl2::lyt::LytParts* pButton01 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_01 );

  gfl2::lyt::LytParts* pMessage = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_MESSAGE_L10 );

  gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMessage, PANENAME_COMMON_MSG_LOW_006_PANE_TEXTBOX_00, &connectionResourceID );

  SetCursorVisible( false );

  pButton00->SetVisible( false );

  pButton01->SetVisible( false );

  pMessage->SetVisible( true );

  pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_message_error_DAT, messageID ) );
}


void ConnectionLowerView::SetMessage( u32 messageID )
{
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, true, true );
  showButton( false, 0xffffffff, 0xffffffff );
  m_bYesNoVisible = false;
}


void ConnectionLowerView::SetMessageYesNo( u32 messageID )
{
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, true, false );
  showButton( true, msg_netconnect_sel_01, msg_netconnect_sel_02 );
  m_bYesNoVisible = true;
}


void ConnectionLowerView::SetStreamMessage( u32 messageID )
{
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, false, false );
  showButton( false, 0xffffffff, 0xffffffff );
  m_bYesNoVisible = false;
}


void ConnectionLowerView::SetStreamMessageYesNo( u32 messageID )
{
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, false, false );
  showButton( false, 0xffffffff, 0xffffffff );
  m_bYesNoVisible = true;
}


void ConnectionLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pConnectionWork->GetMessageUtility();
  m_MessageWindowManipulator.ShowMessage( &pMessageUtility->GetString( GARC_message_netconnect_DAT, messageID ), bImmediate, isShowTimerIcon );
  m_MessageID = messageID;

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pMessage = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_MESSAGE_L2 );
  pMessage->SetVisible( true );
}


void ConnectionLowerView::showButton( bool isShowButton, u32 buttonMessageID0, u32 buttonMessageID1 )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pButton00 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_00 );
  gfl2::lyt::LytParts* pButton01 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_01 );
  
  pButton00->SetVisible( isShowButton );
  pButton01->SetVisible( isShowButton );
  SetCursorVisible( isShowButton );

  if( isShowButton )
  {
    gfl2::lyt::LytMultiResID connectionResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_CONNECTION );

    {
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pButton00, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &connectionResourceID );
      pG2DUtil->SetTextBoxPaneString( pTextBox_00, buttonMessageID0 );
    }

    {
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( pButton01, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &connectionResourceID );
      pG2DUtil->SetTextBoxPaneString( pTextBox_00, buttonMessageID1 );
    }
  }
}


void ConnectionLowerView::SetAlertMessage( u32 messageID, bool bShowTimerIcon )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID connectionResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_CONNECTION );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pButton00 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_00 );
  
  gfl2::lyt::LytParts* pButton01 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_01 );

  gfl2::lyt::LytParts* pMessageL2 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_MESSAGE_L2 );

  gfl2::lyt::LytParts* pMessageL10 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_MESSAGE_L10 );

  gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMessageL10, PANENAME_COMMON_MSG_LOW_006_PANE_TEXTBOX_00, &connectionResourceID );

  SetCursorVisible( false );

  pButton00->SetVisible( false );

  pButton01->SetVisible( false );

  pMessageL2->SetVisible( false );

  pMessageL10->SetVisible( true );

  pG2DUtil->SetTextBoxPaneString( pTextBox, messageID );

  m_bIsOpenAlertMessage = true;

  m_bIsPushButtonA = false;

  if( bShowTimerIcon )
  {
    gfl2::lyt::LytParts* pTimerIcon = pLower->GetPartsPane( pMessageL10, PANENAME_COMMON_MSG_LOW_006_PANE_TIMERICON, &connectionResourceID );
    pTimerIcon->SetVisible( true );
    pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_NET_NET_MIN_LOW_000__MESSAGE_L10__TIMERICON_IN );
    pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_NET_NET_MIN_LOW_000__MESSAGE_L10__TIMERICON_KEEP );
  }
}


void ConnectionLowerView::Update( void )
{
  NetAppLib::UI::NetAppCursorView::Update();
  m_MessageWindowManipulator.UpdateWindow();
}


void ConnectionLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pConnectionWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  NetAppLib::UI::NetAppCursorView::Draw( displayNo );
}


::app::ui::UIInputListener::ListenerResult ConnectionLowerView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( m_bIsOpenAlertMessage )
  {
    if( pTouchPanel->IsTouchTrigger() )
    {
      m_bIsPushButtonA = true;

      result = DISABLE_ACCESS;
    }
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult ConnectionLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BUTTON_00:
    {
      if( m_pConnectionLowerViewListener )
      {
        m_pConnectionLowerViewListener->TouchYesButton( m_MessageID );
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_BUTTON_01:
    {
      if( m_pConnectionLowerViewListener )
      {
        m_pConnectionLowerViewListener->TouchNoButton( m_MessageID );
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult ConnectionLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_bIsOpenAlertMessage )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      m_bIsPushButtonA = true;
    }
  }

  return NetAppLib::UI::NetAppCursorView::OnKeyAction( pButton, pKey, pStick );
}


void ConnectionLowerView::CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID )
{
  switch( eEventID )
  {
    case NetAppLib::UI::NetAppCommonMessageWindowManipulator::EVENT_OnEOM:
    {
      if( m_bYesNoVisible )
      {
        showButton( true, msg_netconnect_sel_01, msg_netconnect_sel_02 );
      }
      else
      {
        if( m_pConnectionLowerViewListener )
        {
          m_pConnectionLowerViewListener->EndStreamMessage( m_MessageID );
        }
      }
    }
    break;
  }
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

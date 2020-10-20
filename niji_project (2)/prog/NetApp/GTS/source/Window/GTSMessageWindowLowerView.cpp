// ============================================================================
/*
 * @file GTSMessageWindowLowerView.cpp
 * @brief メッセージウィンドウを表示するクラスです。
 * @date 2015.04.07
 */
// ============================================================================
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
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
GFL_NAMESPACE_BEGIN(Window)



GTSMessageWindowLowerView::GTSMessageWindowLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pLowerMessageWindowDrawListener( NULL ),
  m_MessageID( 0xffffffff ),
  m_MessageWindowManipulator()
{
  setupMessageWindowManipulator();
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_SLC_LOW_000_PATERN_06 );
  setupButton();

  SetVisibleBackButton( false );
  SetVisibleBlackFilter( true );
  SetInputListener( this );
}


GTSMessageWindowLowerView::~GTSMessageWindowLowerView()
{
}


void GTSMessageWindowLowerView::setupMessageWindowManipulator()
{
  NetAppLib::UI::NetAppCommonMessageWindowManipulator::SSetupInfo setupInfo;

  setupInfo.pHeap                = m_pGTSWork->GetAppHeap();
  setupInfo.pTargetView          = this;
  setupInfo.layoutWorkID         = LAYOUT_WORK_ID_LOWER;
  setupInfo.pHandler             = this;
  setupInfo.animID_TimerIcon     = LA_GTS_GTS_SLC_LOW_000__MSG_L2__TIMERICON_KEEP;
  setupInfo.animID_TimerIcon_In  = LA_GTS_GTS_SLC_LOW_000__MSG_L2__TIMERICON_IN;
  setupInfo.animID_TimerIcon_Out = LA_GTS_GTS_SLC_LOW_000__MSG_L2__TIMERICON_OUT;
  setupInfo.animID_MsgCursor     = LA_GTS_GTS_SLC_LOW_000__MSG_L2__CURSOR_00_KEEP;
  setupInfo.paneID_WindowParts   = PANENAME_GTS_SLC_LOW_000_PANE_MSG_L2;
  setupInfo.paneID_TextBox2      = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
  setupInfo.paneID_NullText1     = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
  setupInfo.paneID_TextBox1a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
  setupInfo.paneID_TextBox2a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;
  setupInfo.paneID_TimerIcon     = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
  setupInfo.paneID_MsgCursor     = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

  m_MessageWindowManipulator.Setup( setupInfo );
}


void GTSMessageWindowLowerView::setupButton()
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
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_BACK );
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );

}


void GTSMessageWindowLowerView::SetMessage( u32 messageID, bool isShowTimerIcon )
{
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, true, isShowTimerIcon );
}


void GTSMessageWindowLowerView::SetStreamMessage( u32 messageID, bool isShowTimerIcon )
{
//  m_MessageWindowManipulator.SetFinishMode_UserInput();
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon );
}

void GTSMessageWindowLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pGTSWork->GetMessageUtility();
  m_MessageWindowManipulator.ShowMessage( &pMessageUtility->GetString( GARC_message_gts_DAT, messageID ), bImmediate, isShowTimerIcon );
  m_MessageID = messageID;
}


//-----------------------------------------------------------------------------
// UIView
//-----------------------------------------------------------------------------
void GTSMessageWindowLowerView::Update()
{
  NetApp::GTS::ViewBase::GTSSlcLowerViewBase::Update();
  m_MessageWindowManipulator.UpdateWindow();
}


void GTSMessageWindowLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


::app::ui::UIInputListener::ListenerResult GTSMessageWindowLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsInputEnabled() )
  {
    switch( painId )
    {
      //『戻る』ボタン
      case BUTTON_ID_BTN_BACK:
      {
        if( m_pLowerMessageWindowDrawListener )
        {
          m_pLowerMessageWindowDrawListener->TouchBackButton( m_MessageID );
        }
        result = DISABLE_ACCESS;
      }
      break;
    }
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult GTSMessageWindowLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( IsInputEnabled() )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      m_button_manager->StartSelectedAct( BUTTON_ID_BTN_BACK );
      result = DISABLE_ACCESS;
    }
  }

  return result;
}


void GTSMessageWindowLowerView::CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID )
{
  switch( eEventID )
  {
//    case NetAppLib::UI::NetAppCommonMessageWindowManipulator::EVENT_OnFinish
    case NetAppLib::UI::NetAppCommonMessageWindowManipulator::EVENT_OnEOM:
    {
      if( m_pLowerMessageWindowDrawListener )
      {
        m_pLowerMessageWindowDrawListener->TouchScreen( m_MessageID );
      }
    }
    break;
  }
}


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

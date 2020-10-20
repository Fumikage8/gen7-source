// ============================================================================
/*
 * @file MiracleTradeMatchingWaitLowerView.cpp
 * @brief ミラクル交換のマッチング待機画面です。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/Matching/MiracleTradeMatchingWaitLowerView.h"
#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/miracletrade/MiracleTrade.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_anim_list.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_pane.h>
#include <niji_conv_header/message/msg_miracletrade.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(Matching)


MiracleTradeMatchingWaitLowerView::MiracleTradeMatchingWaitLowerView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pMiracleTradeWork,
    NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_LAYOUT,
    LYTRES_MIRACLETRADE_MIRACLETRADE_MIN_LOW_BFLYT,
    LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pMiracleTradeWork->GetMessageUtility(),
    GARC_message_miracletrade_DAT ),
  m_pMiracleTradeWork( pMiracleTradeWork ),
  m_MessageID( 0xffffffff ),
  m_MessageWindowManipulator(),
  m_bIsEndStreamMessage( false )
{
  setupMessageWindowManipulator();
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW_PATERN_01 );
  setupTextMessage();
  setVisibleBackButton( false );
}


MiracleTradeMatchingWaitLowerView::~MiracleTradeMatchingWaitLowerView()
{
}


void MiracleTradeMatchingWaitLowerView::setupMessageWindowManipulator()
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

  showMessage( msg_miracletrade_win_02, true, true );
}


void MiracleTradeMatchingWaitLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytTextBox* pTextBox00 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTBOX_00 );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox00, msg_miracletrade_cap_07 );

}


void MiracleTradeMatchingWaitLowerView::SetVisibleTimeOutCounter( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytTextBox* pTextBox00 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTBOX_00 );
  gfl2::lyt::LytTextBox* pTextBox01 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTBOX_01 );
  pTextBox00->SetVisible( bVisible );
  pTextBox01->SetVisible( bVisible );
}


void MiracleTradeMatchingWaitLowerView::SetTime( u32 second )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytTextBox* pTextBox01 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTBOX_01 );
  pG2DUtil->SetRegisterNumber( 0, second, 3 );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox01, msg_miracletrade_cap_08 );
}


void MiracleTradeMatchingWaitLowerView::SetStreamMessage( u32 messageID )
{
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, false, false );
}


void MiracleTradeMatchingWaitLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pMiracleTradeWork->GetMessageUtility();
  m_MessageWindowManipulator.ShowMessage( &pMessageUtility->GetString( GARC_message_miracletrade_DAT, messageID ), bImmediate, isShowTimerIcon );
  m_MessageID = messageID;
}


void MiracleTradeMatchingWaitLowerView::setVisibleBackButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pButtonBack = pLower->GetPartsPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_BUTTON_BACK );
  pButtonBack->SetVisible( bVisible );
}


void MiracleTradeMatchingWaitLowerView::Update( void )
{
 NetAppLib::System::NetApplicationViewBase::Update();
  m_MessageWindowManipulator.UpdateWindow();
}


void MiracleTradeMatchingWaitLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pMiracleTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


void MiracleTradeMatchingWaitLowerView::CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID )
{
  switch( eEventID )
  {
    case NetAppLib::UI::NetAppCommonMessageWindowManipulator::EVENT_OnEOM:
    {
      switch( m_MessageID )
      {
        case msg_miracletrade_win_03:
        case msg_miracletrade_win_04:
        case msg_miracletrade_win_09:
        {
          m_bIsEndStreamMessage = true;
        }
        break;
      }
    }
    break;
  }
}


GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file P2PTradeWarningMessageLowerView.cpp
 * @brief P2P交換アプリの下画面を表示するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/View/P2PTradeWarningMessageLowerView.h"
#include "NetApp/P2PTrade/source/P2PTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/net/net.h>
#include <niji_conv_header/app/net/net_anim_list.h>
#include <niji_conv_header/app/net/net_pane.h>
#include <niji_conv_header/message/msg_net_save.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(View)

P2PTradeWarningMessageLowerView::P2PTradeWarningMessageLowerView( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pP2PTradeWork,
    NetApp::P2PTrade::P2PTRADE_RESOURCE_ID_LAYOUT_CONNECTION,
    LYTRES_NET_NET_MIN_LOW_000_BFLYT,
    LA_NET_NET_MIN_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pP2PTradeWork->GetMessageUtility(),
    GARC_message_net_save_DAT ),
  m_pP2PTradeWork( pP2PTradeWork )
{
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_NET_NET_MIN_LOW_000__BG_00_BG_LOOP_00 );

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID connectionResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_P2PTRADE );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pButton00 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_00 );
  
  gfl2::lyt::LytParts* pButton01 = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_BUTTON_01 );

  gfl2::lyt::LytParts* pMessage = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_MESSAGE_L10 );

  gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMessage, PANENAME_COMMON_MSG_LOW_006_PANE_TEXTBOX_00, &connectionResourceID );

  pButton00->SetVisible( false );

  pButton01->SetVisible( false );

  pMessage->SetVisible( true );
}


P2PTradeWarningMessageLowerView::~P2PTradeWarningMessageLowerView()
{
}


void P2PTradeWarningMessageLowerView::SetMessage( gfl2::fs::ArcFile::ARCDATID garcID, u32 messageID )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pP2PTradeWork->GetMessageUtility();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID connectionResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_P2PTRADE );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pMessageWindow = pLower->GetPartsPane( PANENAME_NET_MIN_LOW_000_PANE_MESSAGE_L10 );

  gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMessageWindow, PANENAME_COMMON_MSG_LOW_006_PANE_TEXTBOX_00, &connectionResourceID );

  pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( garcID, messageID ) );


  gfl2::lyt::LytParts* pTimerIcon = pLower->GetPartsPane( pMessageWindow, PANENAME_COMMON_MSG_LOW_006_PANE_TIMERICON, &connectionResourceID );

  pTimerIcon->SetVisible( true );

  pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_NET_NET_MIN_LOW_000__MESSAGE_L10__TIMERICON_IN );
  pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_NET_NET_MIN_LOW_000__MESSAGE_L10__TIMERICON_KEEP );

}


void P2PTradeWarningMessageLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pP2PTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

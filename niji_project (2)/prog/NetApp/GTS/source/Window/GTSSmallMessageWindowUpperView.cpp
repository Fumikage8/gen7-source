// ============================================================================
/*
 * @file GTSSmallMessageWindowUpperView.cpp
 * @brief 上画面に小さいメッセージウィンドウを表示するクラスです。
 * @date 2015.05.22
 */
// ============================================================================
#include "NetApp/GTS/source/Window/GTSSmallMessageWindowUpperView.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)



GTSSmallMessageWindowUpperView::GTSSmallMessageWindowUpperView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGTSWork,
    NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS,
    LYTRES_GTS_GTS_MSG_UPP_000_BFLYT,
    LA_GTS_GTS_MSG_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pGTSWork->GetMessageUtility(),
    GARC_message_gts_DAT ),
  m_pGTSWork( pGTSWork )
{
  setupLayout( LAYOUT_WORK_ID_UPPER, LA_GTS_GTS_MSG_UPP_000_WIN_SMALL );
}


GTSSmallMessageWindowUpperView::~GTSSmallMessageWindowUpperView()
{
}


void GTSSmallMessageWindowUpperView::SetMessage( u32 messageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytParts* pMessageL2 = pUpper->GetPartsPane( PANENAME_GTS_MSG_UPP_000_PANE_MESSAGE_L2 );

  gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( pMessageL2, PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00, &gtsResourceID );
  
  pTextBox->SetVisible( true );

  pG2DUtil->SetTextBoxPaneString( pTextBox, messageID );
}


void GTSSmallMessageWindowUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

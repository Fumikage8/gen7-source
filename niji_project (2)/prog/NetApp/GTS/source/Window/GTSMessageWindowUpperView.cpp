// ============================================================================
/*
 * @file GTSMessageWindowUpperView.cpp
 * @brief 上画面に大きいメッセージウィンドウを表示するクラスです。
 * @date 2015.04.30
 */
// ============================================================================
#include "NetApp/GTS/source/Window/GTSMessageWindowUpperView.h"
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



GTSMessageWindowUpperView::GTSMessageWindowUpperView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
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
  setupLayout( LAYOUT_WORK_ID_UPPER, LA_GTS_GTS_MSG_UPP_000_WIN_LARGE );
}


GTSMessageWindowUpperView::~GTSMessageWindowUpperView()
{
}


void GTSMessageWindowUpperView::SetMessage( u32 messageID )
{
  UIView::SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_GTS_MSG_UPP_000_PANE_TEXT_L10, messageID );
}


void GTSMessageWindowUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

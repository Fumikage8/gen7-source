// ============================================================================
/*
 * @file GTSWarningMessageWindowLowerView.cpp
 * @brief 警告文ウィンドウを表示するクラスです。
 * @date 2015.04.28
 */
// ============================================================================
#include "NetApp/GTS/source/Window/GTSWarningMessageWindowLowerView.h"
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



GTSWarningMessageWindowLowerView::GTSWarningMessageWindowLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGTSWork,
    NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS,
    LYTRES_GTS_GTS_ART_LOW_000_BFLYT,
    LA_GTS_GTS_ART_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pGTSWork->GetMessageUtility(),
    GARC_message_gts_DAT ),
  m_pGTSWork( pGTSWork )
{
}


GTSWarningMessageWindowLowerView::~GTSWarningMessageWindowLowerView()
{
}


void GTSWarningMessageWindowLowerView::SetMessage( u32 messageID, bool isShowTimerIcon )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( PANENAME_GTS_ART_LOW_000_PANE_TEXTBOX_00 );
  pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, messageID );

  gfl2::lyt::LytParts* pTimerIcon = pLower->GetPartsPane( PANENAME_GTS_ART_LOW_000_PANE_ICON_TIMER );
  pTimerIcon->SetVisible( isShowTimerIcon );
  pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_ART_LOW_000__ICON_TIMER_IN );
  pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_ART_LOW_000__ICON_TIMER_KEEP );
}


void GTSWarningMessageWindowLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

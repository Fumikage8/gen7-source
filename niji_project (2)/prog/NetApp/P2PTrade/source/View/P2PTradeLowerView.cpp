// ============================================================================
/*
 * @file P2PTradeLowerView.cpp
 * @brief P2P交換アプリの下画面を表示するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/View/P2PTradeLowerView.h"
#include "NetApp/P2PTrade/source/P2PTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/net/net.h>
#include <niji_conv_header/app/net/net_anim_list.h>
#include <niji_conv_header/app/net/net_pane.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(View)

P2PTradeLowerView::P2PTradeLowerView( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork ) :
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
}


P2PTradeLowerView::~P2PTradeLowerView()
{
}


void P2PTradeLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pP2PTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

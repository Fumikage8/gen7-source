// ============================================================================
/*
 * @file ConnectionUpperView.cpp
 * @brief インターネット接続アプリの上画面を表示するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/View/ConnectionUpperView.h"
#include "NetApp/Connection/source/ConnectionResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/net/net.h>
#include <niji_conv_header/app/net/net_anim_list.h>
#include <niji_conv_header/app/net/net_pane.h>
#include <niji_conv_header/message/msg_netconnect.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(View)

ConnectionUpperView::ConnectionUpperView( NetApp::Connection::System::ConnectionWork* pConnectionWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pConnectionWork,
    NetApp::Connection::CONNECTION_RESOURCE_ID_LAYOUT,
    LYTRES_NET_NET_MIN_UPP_000_BFLYT,
    LA_NET_NET_MIN_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pConnectionWork->GetMessageUtility(),
    GARC_message_netconnect_DAT ),
  m_pConnectionWork( pConnectionWork )
{
  setupLayout( LAYOUT_WORK_ID_UPPER, LA_NET_NET_MIN_UPP_000__BG_WAVE_BLUE_UPP_00_BG_LOOP_00 );
}


ConnectionUpperView::~ConnectionUpperView()
{
  RemoveFromSuperView();
}


void ConnectionUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pConnectionWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}

gfl2::lyt::LytPicture* ConnectionUpperView::GetIconPane(void)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  return pUpper->GetPicturePane( PANENAME_NET_MIN_UPP_000_PANE_PLAYERPHOTO_04 );
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

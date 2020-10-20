// ============================================================================
/*
 * @file JoinFestaPlayerListBGLowerView.cpp
 * @brief プレイヤーリストアプリの下画面を表示するクラスです。
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaPlayerListBGLowerView::JoinFestaPlayerListBGLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::System::ResourceID layoutResourceID ) :
  NetAppLib::System::NetApplicationViewBase(
    pWorkBase,
    layoutResourceID,
    LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_BGR_LOW_000_BFLYT,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_BGR_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pWorkBase->GetMessageUtility(),
    GARC_message_jf_playerlist_DAT ),
  m_pWorkBase( pWorkBase )
{
}


JoinFestaPlayerListBGLowerView::~JoinFestaPlayerListBGLowerView()
{
}


void JoinFestaPlayerListBGLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pWorkBase->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

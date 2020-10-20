// ============================================================================
/*
 * @file JoinFestaGlobalAttractionBGUpperView.cpp
 * @brief グローバルアトラクションアプリの上画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionBGUpperView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_anim_list.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_pane.h>
#include <niji_conv_header/message/msg_jf_attraction.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)

JoinFestaGlobalAttractionBGUpperView::JoinFestaGlobalAttractionBGUpperView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaGlobalAttractionWork,
    NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_ATTRACTION_JFA_BGR_UPP_000_BFLYT,
    LA_JOIN_FESTA_ATTRACTION_JFA_BGR_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pJoinFestaGlobalAttractionWork->GetMessageUtility(),
    GARC_message_jf_attraction_DAT ),
  m_pJoinFestaGlobalAttractionWork( pJoinFestaGlobalAttractionWork )
{
}


JoinFestaGlobalAttractionBGUpperView::~JoinFestaGlobalAttractionBGUpperView()
{
}


void JoinFestaGlobalAttractionBGUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaGlobalAttractionWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

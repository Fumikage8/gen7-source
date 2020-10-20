// ============================================================================
/*
 * @file JoinFestaSubScreenBgLowerView.cpp
 * @brief ジョインフェスタ下画面の背景
 * @date 2015.10.19
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenBgLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenSoundDefine.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_anim_list.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_pane.h>
#include <niji_conv_header/message/msg_jf_menu.h>

// Pane取得ヘルパー
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


JoinFestaSubScreenBgLowerView::JoinFestaSubScreenBgLowerView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  NetAppLib::System::NetApplicationViewBase( 
    pJoinFestaSubScreenWork,
    JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SUBSCREEN_JFSUB_BGR_LOW_000_BFLYT,
    LA_JOIN_FESTA_SUBSCREEN_JFSUB_BGR_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSubScreenWork->GetMessageUtility(),
    GARC_message_jf_menu_DAT ),
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork )
{
}


JoinFestaSubScreenBgLowerView::~JoinFestaSubScreenBgLowerView()
{
}


void JoinFestaSubScreenBgLowerView::Update( void )
{
  app::ui::UIView::Update();
}


void JoinFestaSubScreenBgLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSubScreenWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

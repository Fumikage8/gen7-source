// ============================================================================
/*
 * @file JoinFestaSelectWordBGLowerView.cpp
 * @brief 挨拶選択アプリの下画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGLowerView.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_anim_list.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_pane.h>
#include <niji_conv_header/message/msg_setphrase.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)

JoinFestaSelectWordBGLowerView::JoinFestaSelectWordBGLowerView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SELECT_WORD_JFSW_BGR_LOW_000_BFLYT,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_BGR_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSelectWordWork->GetMessageUtility(),
    GARC_message_setphrase_DAT ),
  m_pJoinFestaSelectWordWork( pJoinFestaSelectWordWork )
{
}


JoinFestaSelectWordBGLowerView::~JoinFestaSelectWordBGLowerView()
{
}


void JoinFestaSelectWordBGLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSelectWordWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file JoinFestaSelectWordBGUpperView.cpp
 * @brief 挨拶選択アプリの上画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGUpperView.h"
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

JoinFestaSelectWordBGUpperView::JoinFestaSelectWordBGUpperView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SELECT_WORD_JFSW_BGR_UPP_000_BFLYT,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_BGR_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pJoinFestaSelectWordWork->GetMessageUtility(),
    GARC_message_setphrase_DAT ),
  m_pJoinFestaSelectWordWork( pJoinFestaSelectWordWork )
{
}


JoinFestaSelectWordBGUpperView::~JoinFestaSelectWordBGUpperView()
{
}


void JoinFestaSelectWordBGUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSelectWordWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file JoinFestaSelectWordUpperView.cpp
 * @brief 挨拶選択アプリの上画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordUpperView.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaSelectWordTable.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_anim_list.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_pane.h>
#include <niji_conv_header/message/msg_setphrase.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)

JoinFestaSelectWordUpperView::JoinFestaSelectWordUpperView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SELECT_WORD_JFSW_WIN_UPP_000_BFLYT,
    LA_JOIN_FESTA_SELECT_WORD_JFSW_WIN_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pJoinFestaSelectWordWork->GetMessageUtility(),
    GARC_message_setphrase_DAT ),
  m_pJoinFestaSelectWordWork( pJoinFestaSelectWordWork )
{
}


JoinFestaSelectWordUpperView::~JoinFestaSelectWordUpperView()
{
}


void JoinFestaSelectWordUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSelectWordWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


void JoinFestaSelectWordUpperView::SetCategory( u32 categoryMessageID, u32 selectMessageID )
{
  u32 messageID = msg_setphrase_cap_01;

  switch( m_pJoinFestaSelectWordWork->GetEditMenuID() )
  {
    case EDIT_MENU_ID_MEETING_MESSAGE:  messageID = msg_setphrase_cap_01; break;
    case EDIT_MENU_ID_THROB:            messageID = msg_setphrase_cap_03; break;
    case EDIT_MENU_ID_REGRETFUL:        messageID = msg_setphrase_cap_07; break;
    case EDIT_MENU_ID_GOODBY_MESSAGE:   messageID = msg_setphrase_cap_05; break;
    case EDIT_MENU_ID_MY_SECRET:        messageID = msg_setphrase_cap_09; break;
  }

  setTextMessage( messageID, categoryMessageID, selectMessageID );
}


void JoinFestaSelectWordUpperView::SetMessage( u32 categoryMessageID, u32 selectMessageID )
{
  u32 messageID = msg_setphrase_cap_01;

  switch( m_pJoinFestaSelectWordWork->GetEditMenuID() )
  {
    case EDIT_MENU_ID_MEETING_MESSAGE:  messageID = msg_setphrase_cap_02; break;
    case EDIT_MENU_ID_THROB:            messageID = msg_setphrase_cap_04; break;
    case EDIT_MENU_ID_REGRETFUL:        messageID = msg_setphrase_cap_08; break;
    case EDIT_MENU_ID_GOODBY_MESSAGE:   messageID = msg_setphrase_cap_06; break;
    case EDIT_MENU_ID_MY_SECRET:        messageID = msg_setphrase_cap_10; break;
  }

  setTextMessage( messageID, categoryMessageID, selectMessageID );
}


void JoinFestaSelectWordUpperView::setTextMessage( u32 windowMessageID, u32 categoryMessageID, u32 selectMessageID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
      
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaSelectWordWork->GetMessageUtility();

  pG2DUtil->SetRegisterWord( 0, pMessageUtility->GetString( GARC_message_jf_phrase_DAT, selectMessageID ) );
  pG2DUtil->SetRegisterWord( 1, pMessageUtility->GetString( GARC_message_setphrase_DAT, categoryMessageID ) );

  u32 windowMessageIDOffset = 0;

  if( -1 != NetAppLib::JoinFesta::JoinFestaSelectWordTable::IsOpenMessageId( selectMessageID ) )
  {
    windowMessageIDOffset = 10;
  }

  pG2DUtil->SetTextBoxPaneStringExpand( pUpper->GetTextBoxPane( PANENAME_JFSW_WIN_UPP_000_PANE_TEXTBOX_00 ), pMessageUtility->GetString( GARC_message_setphrase_DAT, windowMessageID + windowMessageIDOffset ) );
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file JoinFestaProfileMessageMenuLowerView.cpp
 * @brief メッセージメニュー表示
 * @date 2015.11.04
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileMessageMenuLowerView.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "AppLib/include/Tool/app_tool_MenuWindow.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_profile/join_festa_profile.h>
#include <niji_conv_header/app/join_festa_profile/join_festa_profile_anim_list.h>
#include <niji_conv_header/app/join_festa_profile/join_festa_profile_pane.h>
#include <niji_conv_header/message/msg_jf_myprofile.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)

static const u32 WINDOW_MESSAGE_STRING_MAX    = 1024;

JoinFestaProfileMessageMenuLowerView::JoinFestaProfileMessageMenuLowerView(
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
  NetAppLib::UI::NetAppMessageMenu&                       netAppMessageMenu ) :
  NetAppLib::System::NetApplicationViewBase( pJoinFestaProfileWork->GetAppHeap() ),
  m_pJoinFestaProfileWork( pJoinFestaProfileWork ),
  m_pJoinFestaProfileMessageMenuLowerViewListener( NULL ),
  m_NetAppMessageMenu( netAppMessageMenu ),
  m_WindowMessage( WINDOW_MESSAGE_STRING_MAX, pJoinFestaProfileWork->GetDeviceHeap() ),
  m_MessageID( 0xffffffff ),
  m_MenuIDs()
{
  m_NetAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_NetAppMessageMenu );
}


JoinFestaProfileMessageMenuLowerView::~JoinFestaProfileMessageMenuLowerView()
{
  m_NetAppMessageMenu.RemoveFromSuperView();
  m_NetAppMessageMenu.RemoveEventHandler();
}


void JoinFestaProfileMessageMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}


void JoinFestaProfileMessageMenuLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode )
  {
    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      if( m_pJoinFestaProfileMessageMenuLowerViewListener )
      {
        m_pJoinFestaProfileMessageMenuLowerViewListener->TouchScreen( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedItem:
    {
      int index = m_NetAppMessageMenu.GetSelectedItemIndex();
      if( index >= MENU_MAX )
      {
        index = 0;
      }

      if( m_pJoinFestaProfileMessageMenuLowerViewListener )
      {
        m_pJoinFestaProfileMessageMenuLowerViewListener->SelectMenu( m_MessageID, m_MenuIDs[ index ] );
      }
    }
    break;
  }
}


void JoinFestaProfileMessageMenuLowerView::SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, true );
}


void JoinFestaProfileMessageMenuLowerView::SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 )
{
  m_MessageID = messageID;
  m_MenuIDs[0] = menuID1;
  m_MenuIDs[1] = menuID2;
  m_MenuIDs[2] = menuID3;

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();

  m_WindowMessage.SetStr( pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ) );

  m_NetAppMessageMenu.SetYNListMode( false );
  m_NetAppMessageMenu.MessageMenuView::ShowMessageWithListMenu( &m_WindowMessage, false, this );
}


void JoinFestaProfileMessageMenuLowerView::MessageMenuView_OnAddItem( app::tool::MenuWindow* pMenuWindow )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();

  pMenuWindow->AddItem( pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, m_MenuIDs[0] ), false, false, SEQ_SE_COMPLETE1 );
  pMenuWindow->AddItem( pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, m_MenuIDs[1] ), false, false, SEQ_SE_COMPLETE1 );
  pMenuWindow->AddItem( pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, m_MenuIDs[2] ), false, false, SEQ_SE_DECIDE1 );
}


void JoinFestaProfileMessageMenuLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

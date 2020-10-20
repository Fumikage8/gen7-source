// ============================================================================
/*
 * @file JoinFestaPlayerListMessageMenuLowerView.cpp
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "AppLib/include/Tool/app_tool_MenuWindow.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)

static const u32 PLAYER_NAME_STRING_MAX       =   64;
static const u32 WINDOW_MESSAGE_STRING_MAX    = 1024;
static const u32 LIST_MESSAGE_STRING_MAX      =   64;

JoinFestaPlayerListMessageMenuLowerView::JoinFestaPlayerListMessageMenuLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu ) :
  NetAppLib::System::NetApplicationViewBase( pWorkBase->GetAppHeap() ),
  m_pWorkBase( pWorkBase ),
  m_pJoinFestaPlayerListMessageMenuLowerViewListener( NULL ),
  m_NetAppMessageMenu( netAppMessageMenu ),
  m_MessageID( 0xffffffff ),
  m_MenuIDs(),
  m_MenuNum( 0 ),
  m_PlayerName( PLAYER_NAME_STRING_MAX, pWorkBase->GetDeviceHeap() ),
  m_WindowMessage( WINDOW_MESSAGE_STRING_MAX, pWorkBase->GetDeviceHeap() ),
  m_ListMessage0( LIST_MESSAGE_STRING_MAX, pWorkBase->GetDeviceHeap() ),
  m_ListMessage1( LIST_MESSAGE_STRING_MAX, pWorkBase->GetDeviceHeap() ),
  m_ListMessage2( LIST_MESSAGE_STRING_MAX, pWorkBase->GetDeviceHeap() )
{
  m_NetAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_NetAppMessageMenu );
}


JoinFestaPlayerListMessageMenuLowerView::~JoinFestaPlayerListMessageMenuLowerView()
{
  m_NetAppMessageMenu.RemoveFromSuperView();
  m_NetAppMessageMenu.RemoveEventHandler();
}


void JoinFestaPlayerListMessageMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}


void JoinFestaPlayerListMessageMenuLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode )
  {
    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      if( m_pJoinFestaPlayerListMessageMenuLowerViewListener )
      {
        m_pJoinFestaPlayerListMessageMenuLowerViewListener->TouchScreen( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:
    {
      if( m_pJoinFestaPlayerListMessageMenuLowerViewListener )
      {
        m_pJoinFestaPlayerListMessageMenuLowerViewListener->TouchYesButton( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:
    {
      if( m_pJoinFestaPlayerListMessageMenuLowerViewListener )
      {
        m_pJoinFestaPlayerListMessageMenuLowerViewListener->TouchNoButton( m_MessageID );
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

      if( m_pJoinFestaPlayerListMessageMenuLowerViewListener )
      {
        m_pJoinFestaPlayerListMessageMenuLowerViewListener->SelectMenu( m_MessageID, m_MenuIDs[ index ] );
      }
    }
    break;
  }
}


void JoinFestaPlayerListMessageMenuLowerView::SetMessage( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, true, isShowTimerIcon, false );
}


void JoinFestaPlayerListMessageMenuLowerView::SetMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, true, isShowTimerIcon, true );
}


void JoinFestaPlayerListMessageMenuLowerView::SetStreamMessage( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, false );
}


void JoinFestaPlayerListMessageMenuLowerView::SetStreamMessage( u32 messageID, gfl2::str::StrBuf& strbuf, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, strbuf, false, isShowTimerIcon, false );
}


void JoinFestaPlayerListMessageMenuLowerView::SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, true );
}


void JoinFestaPlayerListMessageMenuLowerView::SetStreamMessageBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( pPlayerName, messageID, false, isShowTimerIcon, true );
}


void JoinFestaPlayerListMessageMenuLowerView::SetMessageYesNoBlackFilter( u32 messageID )
{
  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( GARC_message_jf_playerlist_DAT, messageID, true, GARC_message_jf_playerlist_DAT, jf_playerlist_sel_06, jf_playerlist_sel_07 );
  m_MessageID = messageID;
}


void JoinFestaPlayerListMessageMenuLowerView::SetMessageYesNoBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  expandPlayerName( pPlayerName, messageID );

  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_sel_06 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_sel_07 ) );

  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( &m_WindowMessage, true, &m_ListMessage0, &m_ListMessage1 );
  m_MessageID = messageID;
}


void JoinFestaPlayerListMessageMenuLowerView::SetStreamMessageYesNoBlackFilter( u32 messageID )
{
  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( GARC_message_jf_playerlist_DAT, messageID, false, GARC_message_jf_playerlist_DAT, jf_playerlist_sel_06, jf_playerlist_sel_07 );
  m_MessageID = messageID;
}


void JoinFestaPlayerListMessageMenuLowerView::SetStreamMessageYesNoBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  expandPlayerName( pPlayerName, messageID );

  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_sel_06 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_sel_07 ) );

  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( &m_WindowMessage, false, &m_ListMessage0, &m_ListMessage1 );
  m_MessageID = messageID;
}


void JoinFestaPlayerListMessageMenuLowerView::SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2 )
{
  m_MessageID  = messageID;
  m_MenuIDs[0] = menuID1;
  m_MenuIDs[1] = menuID2;
  m_MenuNum    = 2;

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  m_WindowMessage.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ) );
  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID1 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID2 ) );

  m_NetAppMessageMenu.SetYNListMode( false );
  m_NetAppMessageMenu.MessageMenuView::ShowMessageWithListMenu( &m_WindowMessage, false, this );
}


void JoinFestaPlayerListMessageMenuLowerView::SetMessageMenuBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName, u32 menuID1, u32 menuID2 )
{
  m_MessageID  = messageID;
  m_MenuIDs[0] = menuID1;
  m_MenuIDs[1] = menuID2;
  m_MenuNum    = 2;

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  expandPlayerName( pPlayerName, messageID );
  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID1 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID2 ) );

  m_NetAppMessageMenu.SetYNListMode( false );
  m_NetAppMessageMenu.MessageMenuView::ShowMessageWithListMenu( &m_WindowMessage, false, this );

}


void JoinFestaPlayerListMessageMenuLowerView::SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 )
{
  m_MessageID  = messageID;
  m_MenuIDs[0] = menuID1;
  m_MenuIDs[1] = menuID2;
  m_MenuIDs[2] = menuID3;
  m_MenuNum    = 3;

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  m_WindowMessage.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ) );
  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID1 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID2 ) );
  m_ListMessage2.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID3 ) );

  m_NetAppMessageMenu.SetYNListMode( false );
  m_NetAppMessageMenu.MessageMenuView::ShowMessageWithListMenu( &m_WindowMessage, false, this );
}


void JoinFestaPlayerListMessageMenuLowerView::SetMessageMenuBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName, u32 menuID1, u32 menuID2, u32 menuID3 )
{
  m_MessageID  = messageID;
  m_MenuIDs[0] = menuID1;
  m_MenuIDs[1] = menuID2;
  m_MenuIDs[2] = menuID3;
  m_MenuNum    = 3;

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  expandPlayerName( pPlayerName, messageID );
  m_ListMessage0.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID1 ) );
  m_ListMessage1.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID2 ) );
  m_ListMessage2.SetStr( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, menuID3 ) );

  m_NetAppMessageMenu.SetYNListMode( false );
  m_NetAppMessageMenu.MessageMenuView::ShowMessageWithListMenu( &m_WindowMessage, false, this );
}


void JoinFestaPlayerListMessageMenuLowerView::MessageMenuView_OnAddItem( app::tool::MenuWindow* pMenuWindow )
{
  if( m_MenuNum == 2 )
  {
    pMenuWindow->AddItem( m_ListMessage0, false );
    pMenuWindow->AddItem( m_ListMessage1, true );
  }
  else
  if( m_MenuNum == 3 )
  {
    pMenuWindow->AddItem( m_ListMessage0, false );
    pMenuWindow->AddItem( m_ListMessage1, false );
    pMenuWindow->AddItem( m_ListMessage2, true );
  }
}


void JoinFestaPlayerListMessageMenuLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


void JoinFestaPlayerListMessageMenuLowerView::showMessage( const gfl2::str::STRCODE* pPlayerName, u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  expandPlayerName( pPlayerName, messageID );

  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &m_WindowMessage, bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


void JoinFestaPlayerListMessageMenuLowerView::showMessage( u32 messageID, gfl2::str::StrBuf& strbuf, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &strbuf, bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


void JoinFestaPlayerListMessageMenuLowerView::expandPlayerName( const gfl2::str::STRCODE* pPlayerName, u32 messageID )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  m_PlayerName.SetStr( pPlayerName );

  gfl2::str::StrBuf temp( pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ), m_pWorkBase->GetDeviceHeap() );
  print::WordSet* pWordSet = pMessageUtility->GetWordSet();
  pWordSet->ClearAllBuffer();
  pWordSet->RegisterWord( 0, m_PlayerName );
  pWordSet->Expand( &m_WindowMessage, &temp );
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

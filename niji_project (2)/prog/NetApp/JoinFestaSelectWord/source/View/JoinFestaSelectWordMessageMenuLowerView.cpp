// ============================================================================
/*
 * @file JoinFestaSelectWordMessageMenuLowerView.cpp
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_anim_list.h>
#include <niji_conv_header/app/join_festa_select_word/join_festa_select_word_pane.h>
#include <niji_conv_header/message/msg_setphrase.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)

static const u32 MESSAGE_STRING_MAX    = 1024;

JoinFestaSelectWordMessageMenuLowerView::JoinFestaSelectWordMessageMenuLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu ) :
  NetAppLib::System::NetApplicationViewBase( pWorkBase->GetAppHeap() ),
  m_pWorkBase( pWorkBase ),
  m_pJoinFestaSelectWordMessageMenuLowerViewListener( NULL ),
  m_NetAppMessageMenu( netAppMessageMenu ),
  m_WindowMessage( MESSAGE_STRING_MAX, m_pWorkBase->GetDeviceHeap() ),
  m_SelectMessage( MESSAGE_STRING_MAX, m_pWorkBase->GetDeviceHeap() ),
  m_MessageID( 0xffffffff )
{
  m_NetAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_NetAppMessageMenu );
}


JoinFestaSelectWordMessageMenuLowerView::~JoinFestaSelectWordMessageMenuLowerView()
{
  m_NetAppMessageMenu.RemoveFromSuperView();
  m_NetAppMessageMenu.RemoveEventHandler();
}


void JoinFestaSelectWordMessageMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}


void JoinFestaSelectWordMessageMenuLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode )
  {
    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      if( m_pJoinFestaSelectWordMessageMenuLowerViewListener )
      {
        m_pJoinFestaSelectWordMessageMenuLowerViewListener->TouchScreen( m_MessageID );
      }
    }
    break;
  }
}


void JoinFestaSelectWordMessageMenuLowerView::SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, true );
}


void JoinFestaSelectWordMessageMenuLowerView::SetStreamMessageBlackFilter( u32 windowMessageID, u32 selectMessageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  m_SelectMessage.SetStr( pMessageUtility->GetString( GARC_message_jf_phrase_DAT, selectMessageID ) );

  gfl2::str::StrBuf temp( pMessageUtility->GetString( GARC_message_setphrase_DAT, windowMessageID ), m_pWorkBase->GetDeviceHeap() );
  print::WordSet* pWordSet = pMessageUtility->GetWordSet();
  pWordSet->ClearAllBuffer();
  pWordSet->RegisterWord( 0, m_SelectMessage );
  pWordSet->Expand( &m_WindowMessage, &temp );

  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &m_WindowMessage, false, isShowTimerIcon, true );
  m_MessageID = windowMessageID;
}


void JoinFestaSelectWordMessageMenuLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetString( GARC_message_setphrase_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file JoinFestaGlobalAttractionMessageMenuLowerView.cpp
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_anim_list.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_pane.h>
#include <niji_conv_header/message/msg_jf_attraction.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


JoinFestaGlobalAttractionMessageMenuLowerView::JoinFestaGlobalAttractionMessageMenuLowerView(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu ) :
  NetAppLib::System::NetApplicationViewBase( pWorkBase->GetAppHeap() ),
  m_pWorkBase( pWorkBase ),
  m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener( NULL ),
  m_NetAppMessageMenu( netAppMessageMenu ),
  m_MessageID( 0xffffffff )
{
  m_NetAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_NetAppMessageMenu );
}


JoinFestaGlobalAttractionMessageMenuLowerView::~JoinFestaGlobalAttractionMessageMenuLowerView()
{
  m_NetAppMessageMenu.RemoveFromSuperView();
  m_NetAppMessageMenu.RemoveEventHandler();
}


void JoinFestaGlobalAttractionMessageMenuLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}


void JoinFestaGlobalAttractionMessageMenuLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode )
  {
    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      if( m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener )
      {
        m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener->TouchScreen( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:
    {
      if( m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener )
      {
        m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener->TouchYesButton( m_MessageID );
      }
    }
    break;

    case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:
    {
      if( m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener )
      {
        m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener->TouchNoButton( m_MessageID );
      }
    }
    break;
  }
}


void JoinFestaGlobalAttractionMessageMenuLowerView::SetMessage( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, true, isShowTimerIcon, false );
}


void JoinFestaGlobalAttractionMessageMenuLowerView::SetStreamMessage( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, false );
}


void JoinFestaGlobalAttractionMessageMenuLowerView::SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon )
{
  m_NetAppMessageMenu.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon, true );
}


void JoinFestaGlobalAttractionMessageMenuLowerView::SetMessageYesNoBlackFilter( u32 messageID )
{
  m_NetAppMessageMenu.SetYNListMode( true );
  m_NetAppMessageMenu.ShowMessageWithListMenu( GARC_message_jf_attraction_DAT, messageID, false, GARC_message_jf_attraction_DAT, jf_attraction_sel_yes, jf_attraction_sel_no );
  m_MessageID = messageID;
}


void JoinFestaGlobalAttractionMessageMenuLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
  m_MessageID = messageID;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file GTSTopMenuFrame.cpp
 * @brief GTSのトップメニューのプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#include "NetApp/GTS/source/TopMenu/GTSTopMenuFrame.h"
#include "NetApp/GTS/source/TopMenu/GTSTopMenuLowerView.h"
#include "NetApp/GTS/source/SelectMenu/GTSOneSelectMenuLowerView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowUpperView.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include <niji_conv_header/message/msg_gts.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TopMenu)


GTSTopMenuFrame::GTSTopMenuFrame(
  NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
  NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects ),
  m_GTSTopMenuLowerViewListener(),
  m_GTSOneSelectMenuLowerViewListener()
{
}


GTSTopMenuFrame::~GTSTopMenuFrame()
{
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSTopMenuFrame::startup()
{
  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowUpperView() );
  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSTopMenuLowerView() );

  m_pGTSViewObjects->GetGTSOneSelectMenuLowerView()->SetMessage( msg_gts_win_06, msg_gts_sel_10 );
  m_pGTSViewObjects->GetGTSMessageWindowUpperView()->SetMessage( msg_gts_exp_01 );

  return true;
}


void GTSTopMenuFrame::setListener()
{
  m_pGTSViewObjects->GetGTSTopMenuLowerView()->SetListener( &m_GTSTopMenuLowerViewListener );
  m_pGTSViewObjects->GetGTSOneSelectMenuLowerView()->SetListener( &m_GTSOneSelectMenuLowerViewListener );

  m_pGTSViewObjects->GetGTSTopMenuLowerView()->SetInputEnabled( true );
}


void GTSTopMenuFrame::removeListener()
{
  m_pGTSViewObjects->GetGTSTopMenuLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSOneSelectMenuLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSTopMenuFrame::updateSequence()
{
  switch( m_GTSTopMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::TopMenu::GTSTopMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      removeUpperView();
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSOneSelectMenuLowerView() );
    }
    break;

    case NetApp::GTS::TopMenu::GTSTopMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_UPLOAD_POKEMON_BUTTON:
    {
      m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_BOX );

      exitFrame( NetApp::GTS::TOP_MENU_RESULT_UPLOAD_POKEMON );
    }
    break;

    case NetApp::GTS::TopMenu::GTSTopMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SEARCH_POKEMON_BUTTON:
    {
      exitFrame( NetApp::GTS::TOP_MENU_RESULT_SEARCH_POKEMON );
    }
    break;

    case NetApp::GTS::TopMenu::GTSTopMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_DOWNLOAD_MY_POKEMON_BUTTON:
    {
      exitFrame( NetApp::GTS::TOP_MENU_RESULT_DOWNLOAD_MY_POKEMON );
    }
    break;
  }

  switch( m_GTSOneSelectMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTopMenuLowerView() );
      addUpperViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowUpperView() );

    }
    break;

    case NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_BUTTON:
    {
      exitFrame( NetApp::GTS::TOP_MENU_RESULT_BACK );
    }
    break;
  }

  m_GTSTopMenuLowerViewListener.ResetInputEvent();
  m_GTSOneSelectMenuLowerViewListener.ResetInputEvent();

}


GFL_NAMESPACE_END(TopMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file GTSStateConfirmFrame.cpp
 * @brief GTSアプリを起動した時にGTSの状態を確認するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#include "NetApp/GTS/source/StateConfirm/GTSStateConfirmFrame.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSWarningMessageWindowLowerView.h"
#include "NetApp/GTS/source/RequestSequence/GTSRequestFacade.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include <niji_conv_header/message/msg_gts.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(StateConfirm)


GTSStateConfirmFrame::GTSStateConfirmFrame(
  NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
  NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects ),
  m_GTSMessageWindowLowerViewListener(),
  m_Timer(),
  m_eRequestState( REQUEST_STATE_NULL )
{
}


GTSStateConfirmFrame::~GTSStateConfirmFrame()
{
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSStateConfirmFrame::startup()
{
  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_01 );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBlackFilter( false );

  NetApp::GTS::RequestSequence::GTSRequestFacade::AddStartStateConfirmRequest( this, m_pGTSWork );

  return true;
}


void GTSStateConfirmFrame::setListener()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetListener( &m_GTSMessageWindowLowerViewListener );
}


void GTSStateConfirmFrame::removeListener()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSStateConfirmFrame::updateSequence()
{
  switch( m_eRequestState )
  {
    case REQUEST_STATE_NULL:
    {
      defaultSequence();
    }
    break;

    case REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_SUCCESS:
    {
      if( m_Timer.IsTimeOut() )
      {
        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD_TRADE );

        exitFrame( NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_TRADE_SUCCESS );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_ERROR:
    {
      if( m_Timer.IsTimeOut() )
      {
        exitFrame( NetApp::GTS::STATE_CONFIRM_RESULT_ERROR );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_RESTORE_SUCCESS:
    {
      if( m_Timer.IsTimeOut() )
      {
        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD );

        exitFrame( NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_RETURN );
      }
    }
    break;
  }

  m_GTSMessageWindowLowerViewListener.ResetInputEvent();
}


void GTSStateConfirmFrame::defaultSequence()
{
  switch( m_GTSMessageWindowLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_02: //『預けていたポケモンが交換されました』
        {
          removeAllView();
          addLowerViewInputDisable( m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView() );
          m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView()->SetMessage( msg_gts_win_05 );
          NetApp::GTS::RequestSequence::GTSRequestFacade::AddDownloadTradePokemonRequest( this, m_pGTSWork );
          m_eRequestState = REQUEST_STATE_EXECUTE;
          m_Timer.TimeOutStart( NetApp::GTS::GTS_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND );
        }
        break;

        case msg_gts_win_04: //『預けていたポケモンが帰ってきました』
        {
          removeAllView();
          addLowerViewInputDisable( m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView() );
          m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView()->SetMessage( msg_gts_win_35 );
          NetApp::GTS::RequestSequence::GTSRequestFacade::AddRestorePokemonRequest( this, m_pGTSWork );
          m_eRequestState = REQUEST_STATE_EXECUTE;
          m_Timer.TimeOutStart( NetApp::GTS::GTS_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND );
        }
        break;
      }
    }
    break;
  }
}


//-----------------------------------------------------------------------------
// GTSStartStateConfirmResponseListener
//-----------------------------------------------------------------------------
void GTSStateConfirmFrame::OnStartStateConfirmPokemonNotUpload()
{
  exitFrame( NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_NOT_UPLOAD );
}


void GTSStateConfirmFrame::OnStartStateConfirmPokemonUpload()
{
  exitFrame( NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_UPLOAD );
}


void GTSStateConfirmFrame::OnStartStateConfrimPokemonTrade()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_02 );//『預けていたポケモンが交換されました』
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetInputEnabled( true );
}

void GTSStateConfirmFrame::OnStartStateConfirmPokemonDelete()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_04 );//『預けていたポケモンが帰ってきました』
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetInputEnabled( true );
}


void GTSStateConfirmFrame::OnStartStateConfirmError()
{
  exitFrame( NetApp::GTS::STATE_CONFIRM_RESULT_ERROR );
}



//-----------------------------------------------------------------------------
// GTSDownloadTradePokemonResponseListener
//-----------------------------------------------------------------------------
void GTSStateConfirmFrame::OnDownloadTradePokemonSuccess()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_SUCCESS;
}


void GTSStateConfirmFrame::OnDownloadTradePokemonError()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_ERROR;
}


//-----------------------------------------------------------------------------
// GTSRestorePokemonResponseListener
//-----------------------------------------------------------------------------
void GTSStateConfirmFrame::OnRestorePokemonSuccess()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_RESTORE_SUCCESS;
}


GFL_NAMESPACE_END(StateConfirm)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

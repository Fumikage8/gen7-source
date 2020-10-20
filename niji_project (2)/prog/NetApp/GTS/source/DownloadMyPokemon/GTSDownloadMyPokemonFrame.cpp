// ============================================================================
/*
 * @file GTSDownloadMyPokemonFrame.cpp
 * @brief ポケモンを引き取るプロセスです。
 * @date 2015.04.13
 */
// ============================================================================
#include "NetApp/GTS/source/DownloadMyPokemon/GTSDownloadMyPokemonFrame.h"
#include "NetApp/GTS/source/SelectMenu/GTSOneSelectMenuLowerView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSWarningMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSRequestInfoWindowUpperView.h"
#include "NetApp/GTS/source/RequestSequence/GTSRequestFacade.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include <niji_conv_header/message/msg_gts.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(DownloadMyPokemon)


GTSDownloadMyPokemonFrame::GTSDownloadMyPokemonFrame(
  NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
  NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects ),
  m_bDeletePokemon( false ),
  m_GTSOneSelectMenuLowerViewListener(),
  m_GTSMessageWindowLowerViewListener(),
  m_Timer(),
  m_eRequestState( REQUEST_STATE_NULL )
{
}


GTSDownloadMyPokemonFrame::~GTSDownloadMyPokemonFrame()
{
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSDownloadMyPokemonFrame::startup()
{
  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  m_pGTSViewObjects->GetGTSOneSelectMenuLowerView()->SetMessageFromDownloadMyPokemon();

  addUpperViewInputDisable( m_pGTSViewObjects->GetGTSRequestInfoWindowUpperView() );
  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSOneSelectMenuLowerView() );

  m_pGTSViewObjects->GetGTSRequestInfoWindowUpperView()->SetGTSIndexData( m_pGTSWork->GetUploadGTSIndexData(), true );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBlackFilter( true );

  return true;
}


void GTSDownloadMyPokemonFrame::setListener()
{
  m_pGTSViewObjects->GetGTSOneSelectMenuLowerView()->SetListener( &m_GTSOneSelectMenuLowerViewListener );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetListener( &m_GTSMessageWindowLowerViewListener );

  m_pGTSViewObjects->GetGTSOneSelectMenuLowerView()->SetInputEnabled( true );
}


void GTSDownloadMyPokemonFrame::removeListener()
{
  m_pGTSViewObjects->GetGTSOneSelectMenuLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSDownloadMyPokemonFrame::updateSequence()
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

        exitFrame( NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_POKEMON_TRADE_SUCCESS );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_ERROR:
    {
      if( m_Timer.IsTimeOut() )
      {
        removeLowerView();

        exitFrame( NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_ERROR );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_DOWNLOAD_SUCCESS:
    {
      if( m_Timer.IsTimeOut() )
      {
        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD );

        exitFrame( NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_POKEMON_RETURN );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_DOWNLOAD_ERROR:
    {
      if( m_Timer.IsTimeOut() )
      {
        removeLowerView();

        exitFrame( NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_ERROR );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_RESTORE_SUCCESS:
    {
      if( m_Timer.IsTimeOut() )
      {
        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD );

        exitFrame( NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_POKEMON_RETURN );
      }
    }
    break;
  }

  m_GTSOneSelectMenuLowerViewListener.ResetInputEvent();
  m_GTSMessageWindowLowerViewListener.ResetInputEvent();
}


void GTSDownloadMyPokemonFrame::defaultSequence()
{
  switch( m_GTSOneSelectMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      exitFrame( NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_BACK );
    }
    break;

    case NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_BUTTON:
    {
      NetApp::GTS::RequestSequence::GTSRequestFacade::AddDownloadStateConfirmRequest( this, m_pGTSWork );

      addLowerViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
      m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_01 );
    }
    break;
  }

  switch( m_GTSMessageWindowLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_02: //『預けていたポケモンが交換されました』
        {
          removeAllView();
          addLowerViewInputEnable( m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView() );
          m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView()->SetMessage( msg_gts_win_05 );
          NetApp::GTS::RequestSequence::GTSRequestFacade::AddDownloadTradePokemonRequest( this, m_pGTSWork );
          m_eRequestState = REQUEST_STATE_EXECUTE;
          m_Timer.TimeOutStart( NetApp::GTS::GTS_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND );
        }
        break;

        case msg_gts_win_04: //『預けていたポケモンが帰ってきました』
        {
          removeAllView();
          addLowerViewInputEnable( m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView() );
          m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView()->SetMessage( msg_gts_win_35 );
          if( m_bDeletePokemon )
          {
            NetApp::GTS::RequestSequence::GTSRequestFacade::AddRestorePokemonRequest( this, m_pGTSWork );
          }
          else
          {
            NetApp::GTS::RequestSequence::GTSRequestFacade::AddDownloadPokemonRequest( this, m_pGTSWork );
          }
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
// GTSDownloadStateConfirmResponseListener
//-----------------------------------------------------------------------------
void GTSDownloadMyPokemonFrame::OnDownloadStateConfirmPokemonTrade()
{
  addLowerViewInputEnable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_02 );//『預けていたポケモンが交換されました』
}


void GTSDownloadMyPokemonFrame::OnDownloadStateConfirmPokemonNotTrade()
{
  m_bDeletePokemon = false;
  addLowerViewInputEnable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_04 );//『預けていたポケモンが帰ってきました』
}


void GTSDownloadMyPokemonFrame::OnDownloadStateConfirmPokemonDelete()
{
  m_bDeletePokemon = true;
  addLowerViewInputEnable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_04 );//『預けていたポケモンが帰ってきました』
}


void GTSDownloadMyPokemonFrame::OnDownloadStateConfirmError()
{
  removeLowerView();

  exitFrame( NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_ERROR );
}


//-----------------------------------------------------------------------------
// GTSDownloadTradePokemonResponseListener
//-----------------------------------------------------------------------------
void GTSDownloadMyPokemonFrame::OnDownloadTradePokemonSuccess()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_SUCCESS;
}


void GTSDownloadMyPokemonFrame::OnDownloadTradePokemonError()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_DOWNLOAD_TRADE_ERROR;
}


//-----------------------------------------------------------------------------
// GTSDownloadPokemonResponseListener
//-----------------------------------------------------------------------------
void GTSDownloadMyPokemonFrame::OnDownloadPokemonSuccess()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_DOWNLOAD_SUCCESS;
}


void GTSDownloadMyPokemonFrame::OnDownloadPokemonError()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_DOWNLOAD_ERROR;
}


//-----------------------------------------------------------------------------
// GTSRestorePokemonResponseListener
//-----------------------------------------------------------------------------
void GTSDownloadMyPokemonFrame::OnRestorePokemonSuccess()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_RESTORE_SUCCESS;
}


GFL_NAMESPACE_END(DownloadMyPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file GTSTradePokemonFrame.cpp
 * @brief ポケモンを交換するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#include "NetApp/GTS/source/TradePokemon/GTSTradePokemonFrame.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSWarningMessageWindowLowerView.h"
#include "NetApp/GTS/source/RequestSequence/GTSRequestFacade.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <niji_conv_header/message/msg_gts.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePokemon)


GTSTradePokemonFrame::GTSTradePokemonFrame(
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


GTSTradePokemonFrame::~GTSTradePokemonFrame()
{
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSTradePokemonFrame::startup()
{
  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_15 );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBlackFilter( true );


  NetApp::GTS::RequestSequence::GTSRequestFacade::AddTradePokemonVerifyRequest( this, m_pGTSWork );

  return true;
}


void GTSTradePokemonFrame::setListener()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetListener( &m_GTSMessageWindowLowerViewListener );
}


void GTSTradePokemonFrame::removeListener()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSTradePokemonFrame::updateSequence()
{
  switch( m_eRequestState )
  {
    case REQUEST_STATE_NULL:
    {
      defaultSequence();
    }
    break;

    case REQUEST_STATE_COMPLETE_TRADE_SUCCESS:
    {
      if( m_Timer.IsTimeOut() )
      {
        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_TRADE );
 
        exitFrame( NetApp::GTS::TRADE_POKEMON_RESULT_SUCCESS );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_TRADE_ERROR:
    {
      if( m_Timer.IsTimeOut() )
      {
        exitFrame( NetApp::GTS::TRADE_POKEMON_RESULT_ERROR );
      }
    }
    break;
  }

  m_GTSMessageWindowLowerViewListener.ResetInputEvent();
}


void GTSTradePokemonFrame::defaultSequence()
{
  switch( m_GTSMessageWindowLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_17: // 『このポケモンには問題があります』
        {
          exitFrame( NetApp::GTS::TRADE_POKEMON_RESULT_ERROR );
        }
        break;

        case msg_gts_win_18: // 『このポケモンは既に交換されていました』
        {
          exitFrame( NetApp::GTS::TRADE_POKEMON_RESULT_ALREADY_TRADE_POKEMON );
        }
        break;
      }
    }
    break;
  }
}


//-----------------------------------------------------------------------------
// GTSTradePokemonVerifyResponseListener
//-----------------------------------------------------------------------------
void GTSTradePokemonFrame::OnTradePokemonVerifyOK()
{
  NetApp::GTS::RequestSequence::GTSRequestFacade::AddTradePokemonConfirmRequest( this, m_pGTSWork );
}


void GTSTradePokemonFrame::OnTradePokemonVerifyNG()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetInputEnabled( true );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_17 ); // 『このポケモンには問題があります』

  // @fix MMCat[35]：自分がアップロードしているデータが書き換わっているので、セーブにあるデータに戻す
  {
    Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetGtsData();
    if( pGTSSaveData->GetDepositFlag() )
    {
      m_pGTSWork->SetUploadGTSIndexData( pGTSSaveData->GetUploadGtsIndexData() );
    }
  }
}


void GTSTradePokemonFrame::OnTradePokemonVerifyError()
{
  exitFrame( NetApp::GTS::TRADE_POKEMON_RESULT_ERROR );
}


//-----------------------------------------------------------------------------
// GTSTradePokemonConfirmResponseListener
//-----------------------------------------------------------------------------
void GTSTradePokemonFrame::OnTradePokemonAlreadyTradePokemon()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetInputEnabled( true );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_18 ); // 『このポケモンは既に交換されていました』

  // @fix MMCat[35]：自分がアップロードしているデータが書き換わっているので、セーブにあるデータに戻す
  {
    Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetGtsData();
    if( pGTSSaveData->GetDepositFlag() )
    {
      m_pGTSWork->SetUploadGTSIndexData( pGTSSaveData->GetUploadGtsIndexData() );
    }
  }
}


void GTSTradePokemonFrame::OnTradePokemonConfirmSuccess()
{
  removeAllView();
  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView() );
  m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView()->SetMessage( msg_gts_win_05 );
  NetApp::GTS::RequestSequence::GTSRequestFacade::AddTradePokemonRequest( this, m_pGTSWork );
  m_eRequestState = REQUEST_STATE_EXECUTE;
  m_Timer.TimeOutStart( NetApp::GTS::GTS_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND );
}


void GTSTradePokemonFrame::OnTradePokemonConfirmError()
{
  exitFrame( NetApp::GTS::TRADE_POKEMON_RESULT_ERROR );
}


//-----------------------------------------------------------------------------
// GTSTradePokemonResponseListener
//-----------------------------------------------------------------------------
void GTSTradePokemonFrame::OnTradePokemonSuccess()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_TRADE_SUCCESS;
}


void GTSTradePokemonFrame::OnTradePokemonError()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_TRADE_ERROR;
}


GFL_NAMESPACE_END(TradePokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

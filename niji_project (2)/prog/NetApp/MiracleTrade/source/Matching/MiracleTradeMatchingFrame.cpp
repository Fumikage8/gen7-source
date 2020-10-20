// ============================================================================
/*
 * @file MiracleTradeMatchingFrame.cpp
 * @brief ミラクル交換のマッチングプロセスです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/Matching/MiracleTradeMatchingFrame.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePokemonInfoUpperView.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradeMatchingWaitLowerView.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePlayerInfoLowerView.h"
#include "NetApp/MiracleTrade/source/MiracleTradeFrameResult.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeViewObjects.h"
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeRequestFacade.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include <niji_conv_header/message/msg_miracletrade.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(Matching)

static const u32 AUTO_EXIT_TIME =  5;


MiracleTradeMatchingFrame::MiracleTradeMatchingFrame(
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork,
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects* pMiracleTradeViewObjects ) :
  NetApp::MiracleTrade::MiracleTradeFrameBase( pMiracleTradeWork ),
  m_pMiracleTradeWork( pMiracleTradeWork ),
  m_pMiracleTradeViewObjects( pMiracleTradeViewObjects ),
  m_ExitTimer(),
  m_bExitMatchingSuccess( false )
{
}


MiracleTradeMatchingFrame::~MiracleTradeMatchingFrame()
{
}


//-----------------------------------------------------------------------------
// MiracleTradeFrameBase
//-----------------------------------------------------------------------------
bool MiracleTradeMatchingFrame::startup()
{
  NetAppLib::Util::NetAppPokeModelUtility* pPokeModelUtility = m_pMiracleTradeWork->GetPokeModelUtility();
  if( pPokeModelUtility )
  {
     pml::pokepara::CoreParam& selectPokemonCoreParam = m_pMiracleTradeWork->GetSelectPokemonCoreParam();

     {
       PokeTool::SimpleParam  simpleParam;

       PokeTool::GetSimpleParam(&simpleParam, &selectPokemonCoreParam);
       pPokeModelUtility->RequestEx( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 0, simpleParam, 1 );
     }
  }

  m_pMiracleTradeViewObjects->InputDisableAll();
  m_pMiracleTradeWork->GetRootUpperView()->RemoveAllSubView();
  m_pMiracleTradeWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pMiracleTradeViewObjects->GetMiracleTradePokemonInfoUpperView() );
  addLowerViewInputDisable( m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView() );

  m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView()->SetTime( MIRACLETRADE_TIME_OUT );

  NetApp::MiracleTrade::RequestSequence::MiracleTradeRequestFacade::AddVerifyRequest( this, m_pMiracleTradeWork );

  return true;
}


void MiracleTradeMatchingFrame::setListener()
{
  m_pMiracleTradeViewObjects->GetMiracleTradePlayerInfoLowerView()->SetListener( this );
}


void MiracleTradeMatchingFrame::removeListener()
{
  m_pMiracleTradeViewObjects->GetMiracleTradePlayerInfoLowerView()->RemoveListener();

  m_pMiracleTradeViewObjects->InputDisableAll();
}


void MiracleTradeMatchingFrame::updateSequence()
{
  if( m_bExitMatchingSuccess )
  {
    if( m_ExitTimer.IsTimeOut() )
    {
      m_pMiracleTradeWork->SetBootProcID( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork::BOOT_PROC_ID_DEMO );
      exitFrame( NetApp::MiracleTrade::MATCHING_RESULT_TRADE_SUCCESS );
    }
  }

  if( m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView()->IsEndStreamMessage() )
  {
    exitFrame( NetApp::MiracleTrade::MATCHING_RESULT_TRADE_ERROR );
  }
}


//-----------------------------------------------------------------------------
// MiracleTradeVerifyResponseListener
//-----------------------------------------------------------------------------
void MiracleTradeMatchingFrame::OnMiracleTradeVerifyOK()
{
  GFL_PRINT( "MiracleTradeMatchingFrame::OnMiracleTradeVerifyOK()\n" );

  NetApp::MiracleTrade::RequestSequence::MiracleTradeRequestFacade::AddMatchingRequest( this, m_pMiracleTradeWork );
}


void MiracleTradeMatchingFrame::OnMiracleTradeVerifyNG()
{
  GFL_PRINT( "MiracleTradeMatchingFrame::OnMiracleTradeVerifyNG()\n" );

  m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView()->SetStreamMessage( msg_miracletrade_win_03 );
}


void MiracleTradeMatchingFrame::OnMiracleTradeVerifyError()
{
  GFL_PRINT( "MiracleTradeMatchingFrame::OnMiracleTradeVerifyError()\n" );

  exitFrame( NetApp::MiracleTrade::MATCHING_RESULT_TRADE_ERROR );
}


//-----------------------------------------------------------------------------
// MiracleTradeMatchingResponseListener
//-----------------------------------------------------------------------------
void MiracleTradeMatchingFrame::OnMiracleTradeMatchingSuccess()
{
  GFL_PRINT( "MiracleTradeMatchingFrame::OnMiracleTradeMatchingSuccess()\n" );

  m_bExitMatchingSuccess = true;

  m_ExitTimer.TimeOutStart( AUTO_EXIT_TIME * 1000 );

  removeLowerView();
  m_pMiracleTradeViewObjects->GetMiracleTradePlayerInfoLowerView()->SetupTextMessage();
  addLowerViewInputEnable( m_pMiracleTradeViewObjects->GetMiracleTradePlayerInfoLowerView() );
}


void MiracleTradeMatchingFrame::OnMiracleTradeMatchingError()
{
  GFL_PRINT( "MiracleTradeMatchingFrame::OnMiracleTradeMatchingError()\n" );

  exitFrame( NetApp::MiracleTrade::MATCHING_RESULT_TRADE_ERROR );
}


void MiracleTradeMatchingFrame::OnMiracleTradeMatchingTimeOut()
{
  GFL_PRINT( "MiracleTradeMatchingFrame::OnMiracleTradeMatchingTimeOut()\n" );

  m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView()->SetStreamMessage( msg_miracletrade_win_04 );
}


void MiracleTradeMatchingFrame::OnMiracleTradeMatchingUpdateTime( s32 nTime )
{
  if( nTime >= 0 )
  {
    m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView()->SetTime( static_cast<u32>( nTime ) );
  }
}


void MiracleTradeMatchingFrame::OnMiracleTradeMatchingTimeCounterOff()
{
  m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView()->SetVisibleTimeOutCounter( false );
}


void MiracleTradeMatchingFrame::OnMiracleTradeMatchingVerifyNG()
{
  GFL_PRINT( "MiracleTradeMatchingFrame::OnMiracleTradeMatchingVerifyNG()\n" );

  m_pMiracleTradeViewObjects->GetMiracleTradeMatchingWaitLowerView()->SetStreamMessage( msg_miracletrade_win_09 );
}


//-----------------------------------------------------------------------------
// MiracleTradePlayerInfoLowerViewListener
//-----------------------------------------------------------------------------
void MiracleTradeMatchingFrame::OnTouchScreen()
{
}



GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

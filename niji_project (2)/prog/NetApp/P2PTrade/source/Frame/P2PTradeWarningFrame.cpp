// ============================================================================
/*
 * @file P2PTradeWarningFrame.cpp
 * @brief P2P交換アプリの○○プロセスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/Frame/P2PTradeWarningFrame.h"
#include "NetApp/P2PTrade/source/P2PTradeFrameResult.h"

#include "NetApp/P2PTrade/source/View/P2PTradeUpperView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeLowerView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeWarningMessageLowerView.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_net_save.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(Frame)

static const int SEQUENCE_WAIT_FINAL_TRADE_SUCCESS  = 0;
static const int SEQUENCE_WAIT_FINAL_TRADE_COMPLETE = 1;

static const u32  P2PTRADE_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND = 3000; // 3.0秒(3000ミリ秒)


P2PTradeWarningFrame::P2PTradeWarningFrame(
  NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
  NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor ) :
  NetApp::P2PTrade::Frame::P2PTradeFrameBase( pP2PTradeWork, pP2PTradeViewSystemAccessor ),
  m_Timer()
{
}


P2PTradeWarningFrame::~P2PTradeWarningFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool P2PTradeWarningFrame::startup()
{
  m_pP2PTradeViewObjects->InputDisableAll();
  m_pP2PTradeViewList->RemoveAllView();

  m_pP2PTradeViewList->AddUpperView( m_pP2PTradeViewObjects->GetP2PTradeUpperView() );
  m_pP2PTradeViewList->AddLowerView( m_pP2PTradeViewObjects->GetP2PTradeLowerView() );

  return true;
}


void P2PTradeWarningFrame::setListener()
{
}


void P2PTradeWarningFrame::removeListener()
{
}


void P2PTradeWarningFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    case SEQUENCE_WAIT_FINAL_TRADE_SUCCESS:
    {
      if( m_pP2PTradeWork->IsFinalTradeSuccess() )
      {
        m_pP2PTradeViewList->AddLowerView( m_pP2PTradeViewObjects->GetP2PTradeWarningMessageLowerView() );
        m_pP2PTradeViewObjects->GetP2PTradeWarningMessageLowerView()->SetMessage( GARC_message_net_save_DAT, net_save_win_02 );

        m_Timer.TimeOutStart( P2PTRADE_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND );

        SetSubSeq( SEQUENCE_WAIT_FINAL_TRADE_COMPLETE );
      }
    }
    break;

    case SEQUENCE_WAIT_FINAL_TRADE_COMPLETE:
    {
      if( m_Timer.IsTimeOut() )
      {
        if( m_pP2PTradeWork->IsFinalTradeComplete() )
        {
          exitFrame( NetApp::P2PTrade::WARNING_FRAME_RESULT_BACK );
        }
      }
    }
    break;
  }
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

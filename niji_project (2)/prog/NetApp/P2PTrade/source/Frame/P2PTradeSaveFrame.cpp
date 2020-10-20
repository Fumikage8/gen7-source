// ============================================================================
/*
 * @file P2PTradeSaveFrame.cpp
 * @brief P2P交換アプリの○○プロセスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/Frame/P2PTradeSaveFrame.h"
#include "NetApp/P2PTrade/source/P2PTradeFrameResult.h"

#include "NetApp/P2PTrade/source/View/P2PTradeUpperView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeLowerView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeMessageLowerView.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_net_save.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(Frame)

static const int SEQUENCE_SAVE        = 0;


P2PTradeSaveFrame::P2PTradeSaveFrame(
  NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
  NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor ) :
  NetApp::P2PTrade::Frame::P2PTradeFrameBase( pP2PTradeWork, pP2PTradeViewSystemAccessor ),
  m_NetAppCommonSaveUtility()
{
}


P2PTradeSaveFrame::~P2PTradeSaveFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool P2PTradeSaveFrame::startup()
{
  m_pP2PTradeViewObjects->InputDisableAll();
  m_pP2PTradeViewList->RemoveAllView();

  m_pP2PTradeViewList->AddUpperView( m_pP2PTradeViewObjects->GetP2PTradeUpperView() );
  m_pP2PTradeViewList->AddLowerView( m_pP2PTradeViewObjects->GetP2PTradeLowerView() );

  m_pP2PTradeViewList->AddLowerView( m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView() );
  m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetMessage( GARC_message_net_save_DAT, net_save_win_01, true );

  return true;
}


void P2PTradeSaveFrame::setListener()
{
}


void P2PTradeSaveFrame::removeListener()
{
}


void P2PTradeSaveFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    case SEQUENCE_SAVE:
    {
      bool bSaveComplete = m_NetAppCommonSaveUtility.SaveWait();
      
      if( bSaveComplete )
      {
        exitFrame( NetApp::P2PTrade::SAVE_FRAME_RESULT_BACK );
      }
    }
    break;
  }
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

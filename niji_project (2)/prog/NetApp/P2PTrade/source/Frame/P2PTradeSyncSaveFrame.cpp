// ============================================================================
/*
 * @file P2PTradeSyncSaveFrame.cpp
 * @brief P2P交換アプリの○○プロセスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/Frame/P2PTradeSyncSaveFrame.h"
#include "NetApp/P2PTrade/source/P2PTradeFrameResult.h"

#include "NetApp/P2PTrade/source/View/P2PTradeUpperView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeLowerView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeMessageLowerView.h"

#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_net_save.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(Frame)

static const int SEQUENCE_SAVE                   = 0;
static const int SEQUENCE_SYNC                   = 1;
static const int SEQUENCE_YES_NO_WAIT            = 2;
static const int SEQUENCE_SYNC_CANCEL            = 3;
static const int SEQUENCE_CANCEL_MESSAGE_START   = 4;
static const int SEQUENCE_CANCEL_MESSAGE_WAIT    = 5;


P2PTradeSyncSaveFrame::P2PTradeSyncSaveFrame(
  NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
  NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor ) :
  NetApp::P2PTrade::Frame::P2PTradeFrameBase( pP2PTradeWork, pP2PTradeViewSystemAccessor ),
  m_NetAppCommonSaveUtility(),
  m_P2PTradeMessageLowerViewListener(),
  m_bMyCancelFlag( false )
{
}


P2PTradeSyncSaveFrame::~P2PTradeSyncSaveFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool P2PTradeSyncSaveFrame::startup()
{
  m_pP2PTradeViewObjects->InputDisableAll();
  m_pP2PTradeViewList->RemoveAllView();

  m_pP2PTradeViewList->AddUpperView( m_pP2PTradeViewObjects->GetP2PTradeUpperView() );
  m_pP2PTradeViewList->AddLowerView( m_pP2PTradeViewObjects->GetP2PTradeLowerView() );

  m_pP2PTradeViewList->AddLowerView( m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView() );
  m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetMessage( GARC_message_net_save_DAT, net_save_win_01, true );

  return true;
}


void P2PTradeSyncSaveFrame::setListener()
{
  m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetListener( &m_P2PTradeMessageLowerViewListener );
}


void P2PTradeSyncSaveFrame::removeListener()
{
  m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->RemoveListener();
}


void P2PTradeSyncSaveFrame::updateSequence()
{
  u32  inputEventID       = m_P2PTradeMessageLowerViewListener.GetInputEventID();
  u32  messageID          = m_P2PTradeMessageLowerViewListener.GetMessageID();
  bool bTouchBackButton   = ( inputEventID == NetApp::P2PTrade::View::P2PTradeMessageLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bTouchScreen       = ( inputEventID == NetApp::P2PTrade::View::P2PTradeMessageLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
  bool bTouchYesButton    = ( inputEventID == NetApp::P2PTrade::View::P2PTradeMessageLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
  bool bTouchNoButton     = ( inputEventID == NetApp::P2PTrade::View::P2PTradeMessageLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;

  switch( GetSubSeq() )
  {
    case SEQUENCE_SAVE:
    {
      bool bSaveComplete = m_NetAppCommonSaveUtility.SaveWait();
      
      if( bSaveComplete )
      {
        m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetMessage( GARC_message_net_save_DAT, net_save_win_03, true );
        m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetVisibleBackButton( true );
        m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetInputEnabled( true );

        NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient = m_pP2PTradeWork->GetP2PTradeRequestClient();
        pP2PTradeRequestClient->AddFinalSyncRequest();

        SetSubSeq( SEQUENCE_SYNC );
      }
    }
    break;

    case SEQUENCE_SYNC:
    {
      if( m_pP2PTradeWork->IsFinalSyncCancelComplete() )
      {
        m_pP2PTradeWork->SetSyncSaveCancel( true );

        if( m_bMyCancelFlag )
        {
          exitFrame( NetApp::P2PTrade::SYNC_SAVE_FRAME_RESULT_BACK );
        }
        else
        {
          SetSubSeq( SEQUENCE_CANCEL_MESSAGE_START );
        }
      }
      else
      if( m_pP2PTradeWork->IsFinalSyncComplete() || GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        exitFrame( NetApp::P2PTrade::SYNC_SAVE_FRAME_RESULT_BACK );
      }
      else
      if( bTouchBackButton )
      {
        if( messageID == net_save_win_03 )
        {
          m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetMessageYesNoBlackFilter( GARC_message_net_save_DAT, net_save_win_04 );
          SetSubSeq( SEQUENCE_YES_NO_WAIT );
        }
      }
    }
    break;

    case SEQUENCE_YES_NO_WAIT:
    {
      if( m_pP2PTradeWork->IsFinalSyncComplete() || GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        exitFrame( NetApp::P2PTrade::SYNC_SAVE_FRAME_RESULT_BACK );
      }
      else
      {
        if( messageID == net_save_win_04 )
        {
          if( bTouchYesButton )
          {
            NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient = m_pP2PTradeWork->GetP2PTradeRequestClient();
            pP2PTradeRequestClient->AddFinalSyncCancelRequest();

            m_bMyCancelFlag = true;

            SetSubSeq( SEQUENCE_SYNC_CANCEL );
          }
          else
          if( bTouchNoButton )
          {
            m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetMessage( GARC_message_net_save_DAT, net_save_win_03, true );

            SetSubSeq( SEQUENCE_SYNC );
          }
        }
      }
    }
    break;

    case SEQUENCE_SYNC_CANCEL:
    {
      if( m_pP2PTradeWork->IsFinalSyncCancelComplete() )
      {
        m_pP2PTradeWork->SetSyncSaveCancel( true );

        if( m_bMyCancelFlag )
        {
          exitFrame( NetApp::P2PTrade::SYNC_SAVE_FRAME_RESULT_BACK );
        }
        else
        {
          SetSubSeq( SEQUENCE_CANCEL_MESSAGE_START );
        }
      }
      else
      if( m_pP2PTradeWork->IsFinalSyncComplete() || GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
      {
        exitFrame( NetApp::P2PTrade::SYNC_SAVE_FRAME_RESULT_BACK );
      }
    }
    break;

    case SEQUENCE_CANCEL_MESSAGE_START:
    {
      m_pP2PTradeViewObjects->GetP2PTradeMessageLowerView()->SetStreamMessage( GARC_message_net_save_DAT, net_save_win_05 );
      SetSubSeq( SEQUENCE_CANCEL_MESSAGE_WAIT );
    }
    break;

    case SEQUENCE_CANCEL_MESSAGE_WAIT:
    {
      if( bTouchScreen )
      {
        if( messageID == net_save_win_05 )
        {
          exitFrame( NetApp::P2PTrade::SYNC_SAVE_FRAME_RESULT_BACK );
        }
      }
    }
    break;
  }

  m_P2PTradeMessageLowerViewListener.ResetInputEvent();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

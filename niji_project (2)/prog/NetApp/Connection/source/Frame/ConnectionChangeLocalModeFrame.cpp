// ============================================================================
/*
 * @file ConnectionChangeLocalModeFrame.cpp
 * @brief インターネット接続アプリのローカル通信モードに切り替えるプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/Frame/ConnectionChangeLocalModeFrame.h"
#include "NetApp/Connection/source/View/ConnectionUpperView.h"
#include "NetApp/Connection/source/View/ConnectionLowerView.h"
#include "NetApp/Connection/source/ConnectionFrameResult.h"
#include "NetApp/Connection/source/RequestSequence/ConnectionRequestFacade.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "Sound/include/Sound.h"
#include <niji_conv_header/message/msg_netconnect.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


ConnectionChangeLocalModeFrame::ConnectionChangeLocalModeFrame(
  NetApp::Connection::System::ConnectionWork* pConnectionWork,
  NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor ) :
  NetApp::Connection::Frame::ConnectionFrameBase( pConnectionWork, pConnectionViewSystemAccessor ),
  m_ConnectionLowerViewListener(),
  m_bIsDisconnection( false )
{
}


ConnectionChangeLocalModeFrame::~ConnectionChangeLocalModeFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool ConnectionChangeLocalModeFrame::startup()
{
  m_pConnectionViewObjects->InputDisableAll();
  m_pConnectionViewList->RemoveAllView();

  m_pConnectionViewList->AddUpperView( m_pConnectionViewObjects->GetConnectionUpperView() );
  m_pConnectionViewList->AddLowerView( m_pConnectionViewObjects->GetConnectionLowerView() );

  m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessageYesNo( msg_netconnect_win_10 );

  return true;
}


void ConnectionChangeLocalModeFrame::setListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->SetListener( &m_ConnectionLowerViewListener );
  m_pConnectionViewObjects->GetConnectionLowerView()->SetInputEnabled( true );
}


void ConnectionChangeLocalModeFrame::removeListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->RemoveListener();
}


bool ConnectionChangeLocalModeFrame::cleanup()
{
#if defined( GF_PLATFORM_CTR )
  
  if( m_bIsDisconnection )
  {
    // ネットワーク終了
    if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI ) 
    { 
      NetLib::NijiNetworkSystem::TerminateNetwork(); 
    } 
  }

#endif // defined( GF_PLATFORM_CTR )

  return true;
}


void ConnectionChangeLocalModeFrame::updateSequence()
{
  u32 messageID = m_ConnectionLowerViewListener.GetMessageID();
  u32 inputEventID = m_ConnectionLowerViewListener.GetInputEventID();
  bool bTouchYesButton = ( inputEventID == NetApp::Connection::View::ConnectionLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
  bool bTouchNoButton = ( inputEventID == NetApp::Connection::View::ConnectionLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;
  bool bEndStreamMessage = ( inputEventID == NetApp::Connection::View::ConnectionLowerViewListener::INPUT_EVENT_ID_END_STREAM_MESSAGE ) ? true : false;

  switch( messageID )
  {
    // -----------------------------------------------------------------------------
    // インターネットを切断します。よろしいですか？
    case msg_netconnect_win_10:
    {
      if( bTouchYesButton )
      {
        m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_03 );

        NetApp::Connection::RequestSequence::ConnectionRequestFacade::AddDisconnectionRequest( this, m_pConnectionWork );
      }
      else
      if( bTouchNoButton )
      {
        // ユーザーの手によってキャンセルした
        m_pConnectionWork->SetCancel( true );
        exitFrame( NetApp::Connection::CHANGE_LOCAL_MODE_RESULT_BACK );
      }
    }
    break;

    // -----------------------------------------------------------------------------
    // ローカル通信に切り替えました。
    case msg_netconnect_win_04:
    {
      if( bEndStreamMessage )
      {
        exitFrame( NetApp::Connection::CHANGE_LOCAL_MODE_RESULT_BACK );
      }
    }
    break;

  }

  m_ConnectionLowerViewListener.ResetInputEvent();

}


//-----------------------------------------------------------------------------
// DisconnectionResponseListener
//-----------------------------------------------------------------------------
void ConnectionChangeLocalModeFrame::OnDisconnectionSuccess()
{
  m_bIsDisconnection = true;

  m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_04 );

  Sound::PlaySE( SEQ_SE_SAVE );
}


void ConnectionChangeLocalModeFrame::OnDisconnectionError()
{
  m_bIsDisconnection = true;

  exitFrame( NetApp::Connection::CHANGE_LOCAL_MODE_RESULT_BACK );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

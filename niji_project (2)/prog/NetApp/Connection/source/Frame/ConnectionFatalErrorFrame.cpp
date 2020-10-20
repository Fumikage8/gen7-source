// ============================================================================
/*
 * @file ConnectionFatalErrorFrame.cpp
 * @brief フェイタルエラーが発生した時のプロセスです。
 * @date 2015.12.25
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/Frame/ConnectionFatalErrorFrame.h"
#include "NetApp/Connection/source/View/ConnectionUpperView.h"
#include "NetApp/Connection/source/View/ConnectionLowerView.h"
#include <niji_conv_header/message/msg_netconnect.h>
#include <niji_conv_header/message/msg_message_error.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


ConnectionFatalErrorFrame::ConnectionFatalErrorFrame(
  NetApp::Connection::System::ConnectionWork* pConnectionWork,
  NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor ) :
  NetApp::Connection::Frame::ConnectionFrameBase( pConnectionWork, pConnectionViewSystemAccessor ),
  m_ConnectionLowerViewListener()
{
}


ConnectionFatalErrorFrame::~ConnectionFatalErrorFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool ConnectionFatalErrorFrame::startup()
{
  m_pConnectionViewObjects->InputDisableAll();
  m_pConnectionViewList->RemoveAllView();

  m_pConnectionViewList->AddUpperView( m_pConnectionViewObjects->GetConnectionUpperView() );
  m_pConnectionViewList->AddLowerView( m_pConnectionViewObjects->GetConnectionLowerView() );

  u32 errorMessageID = error_fatal_sd;

  switch( m_pConnectionWork->GetFatalErrorNo() )
  {
    case NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED:  errorMessageID = error_fatal_sd;    break;
    case NetEvent::FatalError::FATAL_ERROR_NO_NET_ERROR:               errorMessageID = error_fatal_save;  break;
  }

  m_pConnectionViewObjects->GetConnectionLowerView()->SetFatalErrorMode( errorMessageID );

  return true;
}


void ConnectionFatalErrorFrame::setListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->SetListener( &m_ConnectionLowerViewListener );
  m_pConnectionViewObjects->GetConnectionLowerView()->SetInputEnabled( true );
}


void ConnectionFatalErrorFrame::removeListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->RemoveListener();
}


void ConnectionFatalErrorFrame::updateSequence()
{
  m_ConnectionLowerViewListener.ResetInputEvent();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

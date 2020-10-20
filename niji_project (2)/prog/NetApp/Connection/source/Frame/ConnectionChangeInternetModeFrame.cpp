// ============================================================================
/*
 * @file ConnectionChangeInternetModeFrame.cpp
 * @brief インターネット接続アプリのインターネット通信モードに切り替えるプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/Frame/ConnectionChangeInternetModeFrame.h"
#include "NetApp/Connection/source/View/ConnectionUpperView.h"
#include "NetApp/Connection/source/View/ConnectionLowerView.h"
#include "NetApp/Connection/source/ConnectionFrameResult.h"
#include <niji_conv_header/message/msg_netconnect.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


ConnectionChangeInternetModeFrame::ConnectionChangeInternetModeFrame(
  NetApp::Connection::System::ConnectionWork* pConnectionWork,
  NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor ) :
  NetApp::Connection::Frame::ConnectionFrameBase( pConnectionWork, pConnectionViewSystemAccessor ),
  m_ConnectionLowerViewListener()
{
}


ConnectionChangeInternetModeFrame::~ConnectionChangeInternetModeFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool ConnectionChangeInternetModeFrame::startup()
{
  return true;
}


void ConnectionChangeInternetModeFrame::setListener()
{
}


void ConnectionChangeInternetModeFrame::removeListener()
{
}


void ConnectionChangeInternetModeFrame::updateSequence()
{
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

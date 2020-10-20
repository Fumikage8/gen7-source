// ============================================================================
/*
 * @file ConnectionInternetDisconnectionFrame.h
 * @brief インターネット接続アプリのインターネット切断を行うプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_INTERNET_DISCONNECTION_FRAME_H_INCLUDE )
#define CONNECTION_INTERNET_DISCONNECTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/Connection/source/Frame/ConnectionFrameBase.h"
#include "NetApp/Connection/source/View/ConnectionLowerViewListener.h"
#include "NetApp/Connection/source/RequestSequence/DisconnectionResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


class ConnectionInternetDisconnectionFrame :
  public NetApp::Connection::Frame::ConnectionFrameBase,
  public NetApp::Connection::RequestSequence::DisconnectionResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionInternetDisconnectionFrame );
public:
  ConnectionInternetDisconnectionFrame(
    NetApp::Connection::System::ConnectionWork* pConnectionWork,
    NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor );
  virtual~ConnectionInternetDisconnectionFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

public:

  // DisconnectionResponseListener
  virtual void OnDisconnectionSuccess();
  virtual void OnDisconnectionError();

private:

  NetApp::Connection::View::ConnectionLowerViewListener   m_ConnectionLowerViewListener;
  NetAppLib::Util::NetAppTimeout                          m_Timer;
  bool                                                    m_bIsDisconnection;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_INTERNET_DISCONNECTION_FRAME_H_INCLUDE

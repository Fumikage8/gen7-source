// ============================================================================
/*
 * @file ConnectionChangeLocalModeFrame.h
 * @brief インターネット接続アプリのローカル通信モードに切り替えるプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_CHANGE_LOCAL_MODE_FRAME_H_INCLUDE )
#define CONNECTION_CHANGE_LOCAL_MODE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/Connection/source/Frame/ConnectionFrameBase.h"
#include "NetApp/Connection/source/View/ConnectionLowerViewListener.h"
#include "NetApp/Connection/source/RequestSequence/DisconnectionResponseListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


class ConnectionChangeLocalModeFrame :
  public NetApp::Connection::Frame::ConnectionFrameBase,
  public NetApp::Connection::RequestSequence::DisconnectionResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionChangeLocalModeFrame );
public:
  ConnectionChangeLocalModeFrame(
    NetApp::Connection::System::ConnectionWork* pConnectionWork,
    NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor );
  virtual~ConnectionChangeLocalModeFrame();

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
  bool                                                    m_bIsDisconnection;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_CHANGE_LOCAL_MODE_FRAME_H_INCLUDE

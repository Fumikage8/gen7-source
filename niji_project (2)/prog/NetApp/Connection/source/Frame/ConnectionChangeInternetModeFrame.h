// ============================================================================
/*
 * @file ConnectionChangeInternetModeFrame.h
 * @brief インターネット接続アプリのインターネット通信モードに切り替えるプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_CHANGE_INTERNET_MODE_FRAME_H_INCLUDE )
#define CONNECTION_CHANGE_INTERNET_MODE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/Connection/source/Frame/ConnectionFrameBase.h"
#include "NetApp/Connection/source/View/ConnectionLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


class ConnectionChangeInternetModeFrame : public NetApp::Connection::Frame::ConnectionFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionChangeInternetModeFrame );
public:
  ConnectionChangeInternetModeFrame(
    NetApp::Connection::System::ConnectionWork* pConnectionWork,
    NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor );
  virtual~ConnectionChangeInternetModeFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  NetApp::Connection::View::ConnectionLowerViewListener   m_ConnectionLowerViewListener;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_CHANGE_INTERNET_MODE_FRAME_H_INCLUDE

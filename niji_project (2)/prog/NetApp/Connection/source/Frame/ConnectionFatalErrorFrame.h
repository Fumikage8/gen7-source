// ============================================================================
/*
 * @file ConnectionFatalErrorFrame.h
 * @brief フェイタルエラーが発生した時のプロセスです。
 * @date 2015.12.25
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_FATAL_ERROR_FRAME_H_INCLUDE )
#define CONNECTION_FATAL_ERROR_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/Connection/source/Frame/ConnectionFrameBase.h"
#include "NetApp/Connection/source/View/ConnectionLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


class ConnectionFatalErrorFrame : public NetApp::Connection::Frame::ConnectionFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionFatalErrorFrame );
public:
  ConnectionFatalErrorFrame(
    NetApp::Connection::System::ConnectionWork* pConnectionWork,
    NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor );
  virtual~ConnectionFatalErrorFrame();

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

#endif // CONNECTION_FATAL_ERROR_FRAME_H_INCLUDE

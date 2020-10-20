// ============================================================================
/*
 * @file ConnectionProc.h
 * @brief インターネット接続アプリのプロッククラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_PROC_H_INCLUDE )
#define CONNECTION_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/Connection/include/ConnectionAppParam.h"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)

GFL_NAMESPACE_BEGIN(System)
class ConnectionApplicationSystem;
class ConnectionWork;
GFL_NAMESPACE_END(System)


class ConnectionProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionProc );
public:
  ConnectionProc( NetApp::Connection::APP_PARAM* pAppParam );
  virtual~ConnectionProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetAppResult( NetApp::Connection::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

  void SetFatalErrorEventParam( const NetEvent::FatalError::EventParam& fataErrorEventParam ){ m_FatalErrorEventParam = fataErrorEventParam; }

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::Connection::APP_PARAM                                m_AppParam;
  NetApp::Connection::APP_RESULT*                              m_pAppResult;
  NetApp::Connection::System::ConnectionWork*                  m_pConnectionWork;
  NetApp::Connection::System::ConnectionApplicationSystem*     m_pApplicationSystem;
  NetEvent::FatalError::EventParam                             m_FatalErrorEventParam;

};


GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_PROC_H_INCLUDE

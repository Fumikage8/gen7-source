// ============================================================================
/*
 * @file ConnectionEvent.h
 * @brief インターネット接続アプリのイベント
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTIONEVENT_H_INCLUDE )
#define CONNECTIONEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/Connection/include/ConnectionAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
class ConnectionProc;
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(Connection)


class ConnectionEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, const NetApp::Connection::APP_PARAM& appParam );
  
  // APP_RESULT を指定する版は pAppResult を保持し続けておく必要があります
  static void StartEvent( GameSys::GameManager* pGameManager, const NetApp::Connection::APP_PARAM& appParam, NetApp::Connection::APP_RESULT* pAppResult );

  ConnectionEvent( gfl2::heap::HeapBase* pHeap );
  virtual~ConnectionEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppParam( const NetApp::Connection::APP_PARAM& appParam ){ m_AppParam = appParam; }
  void SetAppResult( NetApp::Connection::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  NetApp::Connection::APP_PARAM                       m_AppParam;
  NetApp::Connection::APP_RESULT*                     m_pAppResult;
  NetApp::Connection::ConnectionProc*                 m_pConnectionProc;

};


GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetEvent)

#endif // CONNECTIONEVENT_H_INCLUDE

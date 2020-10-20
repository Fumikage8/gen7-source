// ============================================================================
/*
 * @file ConnectionRequestFacade.h
 * @brief インターネット接続アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( CONNECTION_REQUEST_FACADE_H_INCLUDE )
#define CONNECTION_REQUEST_FACADE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/Connection/source/System/ConnectionWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(RequestSequence)


class ConnectionResponseListener;
class DisconnectionResponseListener;



class ConnectionRequestFacade
{
public:
  static gflnet2::ServerClient::RequestHandle AddConnectionRequest( NetApp::Connection::RequestSequence::ConnectionResponseListener* pConnectionResponseListener, NetApp::Connection::System::ConnectionWork* pConnectionWork );
  static gflnet2::ServerClient::RequestHandle AddDisconnectionRequest( NetApp::Connection::RequestSequence::DisconnectionResponseListener* pDisconnectionResponseListener, NetApp::Connection::System::ConnectionWork* pConnectionWork );


};



GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_REQUEST_FACADE_H_INCLUDE

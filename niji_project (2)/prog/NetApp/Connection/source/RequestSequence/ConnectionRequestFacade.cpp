// ============================================================================
/*
 * @file ConnectionRequestFacade.cpp
 * @brief インターネット接続アプリで必要な通信リクエストを投げる為のFacadeクラスです。
 * @date 2015.08.21
 */
// ============================================================================
#include "NetApp/Connection/source/RequestSequence/ConnectionRequestFacade.h"
#include "NetApp/Connection/source/RequestSequence/ConnectionRequestSequence.h"
#include "NetApp/Connection/source/RequestSequence/ConnectionResponseListener.h"
#include "NetApp/Connection/source/RequestSequence/DisconnectionRequestSequence.h"
#include "NetApp/Connection/source/RequestSequence/DisconnectionResponseListener.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(RequestSequence)



gflnet2::ServerClient::RequestHandle ConnectionRequestFacade::AddConnectionRequest( NetApp::Connection::RequestSequence::ConnectionResponseListener* pConnectionResponseListener, NetApp::Connection::System::ConnectionWork* pConnectionWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::Connection::RequestSequence::ConnectionRequestSequence, NetApp::Connection::RequestSequence::ConnectionResponseListener >( pConnectionResponseListener, pConnectionWork );
}


gflnet2::ServerClient::RequestHandle ConnectionRequestFacade::AddDisconnectionRequest( NetApp::Connection::RequestSequence::DisconnectionResponseListener* pDisconnectionResponseListener, NetApp::Connection::System::ConnectionWork* pConnectionWork )
{
  return gflnet2::ServerClient::ServerClientRequestManager::AddRequest< NetApp::Connection::RequestSequence::DisconnectionRequestSequence, NetApp::Connection::RequestSequence::DisconnectionResponseListener >( pDisconnectionResponseListener, pConnectionWork );
}


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

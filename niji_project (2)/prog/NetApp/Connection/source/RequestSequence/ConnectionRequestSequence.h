// ============================================================================
/*
 * @file ConnectionRequestSequence.h
 * @brief インターネット接続を行うリクエストシーケンスです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( CONNECTION_REQUEST_SEQUENCE_H_INCLUDE )
#define CONNECTION_REQUEST_SEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/Connection/source/RequestSequence/ConnectionResponseListener.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"

#if defined( GF_PLATFORM_CTR )

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)
class WifiConnectRunner;
GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)

#endif // defined( GF_PLATFORM_CTR )

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(RequestSequence)


class ConnectionRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionRequestSequence );


public:

  ConnectionRequestSequence();
  virtual~ConnectionRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetApp::Connection::System::ConnectionWork* pConnectionWork ){ m_pConnectionWork = pConnectionWork; }
  virtual void RemoveListener(){ m_pConnectionResponseListener = NULL; }
  void SetListener( ConnectionResponseListener* pConnectionResponseListener ){ m_pConnectionResponseListener = pConnectionResponseListener; }

private:


  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    SEQUENCE_CONNECTION_WAIT,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
    SEQUENCE_TERMINATE
  };

#if defined( GF_PLATFORM_CTR )
  NetLib::Wifi::WifiConnectRunner*                m_pWifiConnectRunner;
#endif // defined( GF_PLATFORM_CTR )

  NetApp::Connection::System::ConnectionWork*     m_pConnectionWork;
  ConnectionResponseListener*                     m_pConnectionResponseListener;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_REQUEST_SEQUENCE_H_INCLUDE

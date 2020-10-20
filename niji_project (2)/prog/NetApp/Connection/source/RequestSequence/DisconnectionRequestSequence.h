// ============================================================================
/*
 * @file DisconnectionRequestSequence.h
 * @brief インターネット切断を行うリクエストシーケンスです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( DISCONNECTION_REQUEST_SEQUENCE_H_INCLUDE )
#define DISCONNECTION_REQUEST_SEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/Connection/source/RequestSequence/DisconnectionResponseListener.h"
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


class DisconnectionRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( DisconnectionRequestSequence );


public:

  DisconnectionRequestSequence();
  virtual~DisconnectionRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetApp::Connection::System::ConnectionWork* pConnectionWork ){ m_pConnectionWork = pConnectionWork; }
  virtual void RemoveListener(){ m_pDisconnectionResponseListener = NULL; }
  void SetListener( DisconnectionResponseListener* pDisconnectionResponseListener ){ m_pDisconnectionResponseListener = pDisconnectionResponseListener; }

private:


  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    SEQUENCE_DISCONNECTION_WAIT,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
    SEQUENCE_TERMINATE
  };

#if defined( GF_PLATFORM_CTR )
  NetLib::Wifi::WifiConnectRunner*               m_pWifiConnectRunner;
#endif // defined( GF_PLATFORM_CTR )

  NetApp::Connection::System::ConnectionWork*    m_pConnectionWork;
  DisconnectionResponseListener*                 m_pDisconnectionResponseListener;

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // DISCONNECTION_REQUEST_SEQUENCE_H_INCLUDE

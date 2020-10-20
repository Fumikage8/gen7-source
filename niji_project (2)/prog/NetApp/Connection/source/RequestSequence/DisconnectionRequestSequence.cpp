// ============================================================================
/*
 * @file DisConnectionRequestSequence.cpp
 * @brief インターネット切断を行うリクエストシーケンスです。
 * @date 2015.08.21
 */
// ============================================================================
#include "NetApp/Connection/source/RequestSequence/DisconnectionRequestSequence.h"
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(RequestSequence)


DisconnectionRequestSequence::DisconnectionRequestSequence() :
#if defined( GF_PLATFORM_CTR )
  m_pWifiConnectRunner( NULL ),
#endif // defined( GF_PLATFORM_CTR )
  m_pConnectionWork( NULL ),
  m_pDisconnectionResponseListener( NULL )
{
}


DisconnectionRequestSequence::~DisconnectionRequestSequence()
{
}

#if defined( GF_PLATFORM_CTR )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE DisconnectionRequestSequence::Update()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 
    case SEQUENCE_INITIALIZE:
    {
      m_pWifiConnectRunner = GFL_NEW( m_pConnectionWork->GetDeviceHeap() ) NetLib::Wifi::WifiConnectRunner( NetLib::Wifi::WifiConnectRunner::E_MODE_DISCONNECT );

      SetSequenceNo( SEQUENCE_DISCONNECTION_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_DISCONNECTION_WAIT:
    {
      NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = m_pWifiConnectRunner->Update();

      if( ret == NetLib::Wifi::WifiConnectRunner::E_RET_SUCCESS )
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
      }
      else
      if( ret == NetLib::Wifi::WifiConnectRunner::E_RET_FAILED )
      {
        SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( m_pDisconnectionResponseListener )
      {
        m_pDisconnectionResponseListener->OnDisconnectionSuccess();
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( m_pDisconnectionResponseListener )
      {
        m_pDisconnectionResponseListener->OnDisconnectionError();
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_TERMINATE:
    {
      GFL_SAFE_DELETE( m_pWifiConnectRunner );

      //パーソナルを全てオフラインに
      NetAppLib::JoinFesta::JoinFestaPacketUtil::SetOfflineJoinFestaPersonalAll();

      result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
    break;

  }

  return result;
}

#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE DisconnectionRequestSequence::Update()
{
  if( m_pDisconnectionResponseListener )
  {
    m_pDisconnectionResponseListener->OnDisconnectionError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE DisconnectionRequestSequence::UpdateCancel()
{
  return Update();
}


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

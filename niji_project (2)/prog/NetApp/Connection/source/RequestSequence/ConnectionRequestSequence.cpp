// ============================================================================
/*
 * @file ConnectionRequestSequence.cpp
 * @brief インターネット接続を行うリクエストシーケンスです。
 * @date 2015.08.21
 */
// ============================================================================
#include "NetApp/Connection/source/RequestSequence/ConnectionRequestSequence.h"
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(RequestSequence)


ConnectionRequestSequence::ConnectionRequestSequence() :
#if defined( GF_PLATFORM_CTR )
  m_pWifiConnectRunner( NULL ),
#endif // defined( GF_PLATFORM_CTR )
  m_pConnectionWork( NULL ),
  m_pConnectionResponseListener( NULL )
{
}


ConnectionRequestSequence::~ConnectionRequestSequence()
{
}

#if defined( GF_PLATFORM_CTR )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE ConnectionRequestSequence::Update()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 
    case SEQUENCE_INITIALIZE:
    {
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE mode = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT;

      bool bFirstConnection = ( pMyStatus->IsValidNexUniqueID() ) ? false : true;

      if( bFirstConnection )
      {
        mode = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT_AND_CREATE_NEXID;
      }

      m_pWifiConnectRunner = GFL_NEW( m_pConnectionWork->GetDeviceHeap() ) NetLib::Wifi::WifiConnectRunner( mode );

      SetSequenceNo( SEQUENCE_CONNECTION_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_CONNECTION_WAIT:
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
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      bool bFirstConnection = ( pMyStatus->IsValidNexUniqueID() ) ? false : true;

      if( bFirstConnection )
      {
        pMyStatus->SetNexUniqueID( m_pWifiConnectRunner->GetNexUniqueId() );
      }

      if( m_pConnectionResponseListener )
      {
        m_pConnectionResponseListener->OnConnectionSuccess();
      }

      //パーソナルを全てオフラインに
      NetAppLib::JoinFesta::JoinFestaPacketUtil::SetOfflineJoinFestaPersonalAll();

      //サブスクリプションの初回全更新
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->UnlockListener();//リスナー再設定
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->RequestAllUpdatingWithSubscription();
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();

      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( m_pConnectionResponseListener )
      {
        m_pConnectionResponseListener->OnConnectionError();
      }
      SetSequenceNo( SEQUENCE_TERMINATE );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_TERMINATE:
    {
      GFL_SAFE_DELETE( m_pWifiConnectRunner );

      result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
    break;

  }

  return result;
}

#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE ConnectionRequestSequence::Update()
{
  if( m_pConnectionResponseListener )
  {
    m_pConnectionResponseListener->OnConnectionError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE ConnectionRequestSequence::UpdateCancel()
{
#if defined( GF_PLATFORM_CTR )
  if( m_pWifiConnectRunner )
  {
    m_pWifiConnectRunner->Finalize();
  }

  GFL_SAFE_DELETE( m_pWifiConnectRunner );

  NetLib::Wifi::ForceDisconnectWifi();
  NetLib::NijiNetworkSystem::TerminateNetwork();

  if( m_pConnectionResponseListener )
  {
    m_pConnectionResponseListener->OnConnectionError();
  }
#endif
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

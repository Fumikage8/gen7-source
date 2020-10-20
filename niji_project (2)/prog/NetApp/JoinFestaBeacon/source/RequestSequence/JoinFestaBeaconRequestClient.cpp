// ============================================================================
/*
 * @file JoinFestaBeaconRequestClient.cpp
 * @brief JoinFestaBeaconアプリで必要な通信リクエストを投げる為のクラスです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/RequestSequence/JoinFestaBeaconRequestClient.h"
#include "NetApp/JoinFestaBeacon/source/RequestSequence/SequenceModule/JoinFestaBeaconInviteSequence.h"
#include "NetApp/JoinFestaBeacon/source/RequestSequence/SequenceModule/JoinFestaBeaconJoinSequence.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ/デストラクタ
 */
//--------------------------------------------------------------------------------------------
JoinFestaBeaconRequestClient::JoinFestaBeaconRequestClient( JoinFestaBeaconResponseListener* pListener )
: m_handle( 0 )
, m_pListener( pListener )
{
  GFL_ASSERT( pListener );//@fix
}
JoinFestaBeaconRequestClient::~JoinFestaBeaconRequestClient()
{
  gflnet2::ServerClient::ServerClientRequestManager::RemoveListener( m_handle );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   接続申し込みリクエスト
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconRequestClient::InviteRequest( REQUEST_PARAM& requestParam )
{
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< JoinFestaBeaconInviteSequence, JoinFestaBeaconResponseListener , REQUEST_PARAM >( m_pListener, requestParam );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   参加リクエスト
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconRequestClient::JoinRequest( REQUEST_PARAM& requestParam )
{
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< JoinFestaBeaconJoinSequence, JoinFestaBeaconResponseListener , REQUEST_PARAM >( m_pListener, requestParam );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   募集リクエスト
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconRequestClient::RecruitRequest( REQUEST_PARAM& requestParam )
{
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   接続中止リクエスト
  * @param   requestParam リクエストパラメータ
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconRequestClient::CancelConnectRequest( void )
{
  gflnet2::ServerClient::ServerClientRequestManager::CancelRequest( m_handle );
}


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

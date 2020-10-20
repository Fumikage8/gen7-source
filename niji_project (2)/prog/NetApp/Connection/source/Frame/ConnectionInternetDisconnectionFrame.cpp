// ============================================================================
/*
 * @file ConnectionInternetDisconnectionFrame.cpp
 * @brief インターネット接続アプリのインターネット切断を行うプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/Frame/ConnectionInternetDisconnectionFrame.h"
#include "NetApp/Connection/source/View/ConnectionUpperView.h"
#include "NetApp/Connection/source/View/ConnectionLowerView.h"
#include "NetApp/Connection/source/ConnectionFrameResult.h"
#include "NetApp/Connection/source/RequestSequence/ConnectionRequestFacade.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include <niji_conv_header/message/msg_netconnect.h>
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


ConnectionInternetDisconnectionFrame::ConnectionInternetDisconnectionFrame(
  NetApp::Connection::System::ConnectionWork* pConnectionWork,
  NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor ) :
  NetApp::Connection::Frame::ConnectionFrameBase( pConnectionWork, pConnectionViewSystemAccessor ),
  m_ConnectionLowerViewListener(),
  m_Timer(),
  m_bIsDisconnection( false )
{
}


ConnectionInternetDisconnectionFrame::~ConnectionInternetDisconnectionFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool ConnectionInternetDisconnectionFrame::startup()
{
  m_pConnectionViewObjects->InputDisableAll();
  m_pConnectionViewList->RemoveAllView();

  m_pConnectionViewList->AddUpperView( m_pConnectionViewObjects->GetConnectionUpperView() );
  m_pConnectionViewList->AddLowerView( m_pConnectionViewObjects->GetConnectionLowerView() );

  m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_08 );

  NetApp::Connection::RequestSequence::ConnectionRequestFacade::AddDisconnectionRequest( this, m_pConnectionWork );

  m_Timer.TimeOutStart();

  return true;
}


void ConnectionInternetDisconnectionFrame::setListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->SetListener( &m_ConnectionLowerViewListener );
  m_pConnectionViewObjects->GetConnectionLowerView()->SetInputEnabled( true );
}


void ConnectionInternetDisconnectionFrame::removeListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->RemoveListener();
}


bool ConnectionInternetDisconnectionFrame::cleanup()
{
#if defined( GF_PLATFORM_CTR )

  if( m_bIsDisconnection )
  {
    // ネットワーク終了
    if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI ) 
    { 
      NetLib::NijiNetworkSystem::TerminateNetwork(); 
    } 
    else 
    { 
      GFL_ASSERT_MSG(0,"NetworkMode Error!!!\n");
    }
  }

#endif // defined( GF_PLATFORM_CTR )

  return true;
}


void ConnectionInternetDisconnectionFrame::updateSequence()
{
  if( m_Timer.IsTimeOut() )
  {
    if( m_bIsDisconnection )
    {
      exitFrame( NetApp::Connection::INTERNET_DISCONNECTION_RESULT_BACK ); 
    }
  }

  m_ConnectionLowerViewListener.ResetInputEvent();
}


//-----------------------------------------------------------------------------
// DisconnectionResponseListener
//-----------------------------------------------------------------------------
void ConnectionInternetDisconnectionFrame::OnDisconnectionSuccess()
{
  m_bIsDisconnection = true;
}


void ConnectionInternetDisconnectionFrame::OnDisconnectionError()
{
  m_bIsDisconnection = true;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

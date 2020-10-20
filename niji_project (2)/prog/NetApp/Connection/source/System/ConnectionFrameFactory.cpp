// ============================================================================
/*
 * @file ConnectionFrameFactory.cpp
 * @brief インターネット接続アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionFrameFactory.h"
#include "NetApp/Connection/source/ConnectionFrameID.h"
#include "NetApp/Connection/source/Frame/ConnectionChangeInternetModeFrame.h"
#include "NetApp/Connection/source/Frame/ConnectionChangeLocalModeFrame.h"
#include "NetApp/Connection/source/Frame/ConnectionInternetConnectionFrame.h"
#include "NetApp/Connection/source/Frame/ConnectionInternetDisconnectionFrame.h"
#include "NetApp/Connection/source/Frame/ConnectionFatalErrorFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


ConnectionFrameFactory::ConnectionFrameFactory( NetApp::Connection::System::ConnectionWork* pConnectionWork, NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor ) :
  m_pConnectionWork( pConnectionWork ),
  m_pConnectionViewSystemAccessor( pConnectionViewSystemAccessor )
{
}


ConnectionFrameFactory::~ConnectionFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* ConnectionFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pConnectionWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::Connection::CONNECTION_FRAME_ID_CHANGE_INTERNET_MODE:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::Connection::Frame::ConnectionChangeInternetModeFrame( m_pConnectionWork, m_pConnectionViewSystemAccessor );
    break;

    case NetApp::Connection::CONNECTION_FRAME_ID_CHANGE_LOCAL_MODE:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::Connection::Frame::ConnectionChangeLocalModeFrame( m_pConnectionWork, m_pConnectionViewSystemAccessor );
    break;

    case NetApp::Connection::CONNECTION_FRAME_ID_INTERNET_CONNECTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::Connection::Frame::ConnectionInternetConnectionFrame( m_pConnectionWork, m_pConnectionViewSystemAccessor );
    break;

    case NetApp::Connection::CONNECTION_FRAME_ID_INTERNET_DISCONNECTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::Connection::Frame::ConnectionInternetDisconnectionFrame( m_pConnectionWork, m_pConnectionViewSystemAccessor );
    break;

    case NetApp::Connection::CONNECTION_FRAME_ID_FATAL_ERROR:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::Connection::Frame::ConnectionFatalErrorFrame( m_pConnectionWork, m_pConnectionViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

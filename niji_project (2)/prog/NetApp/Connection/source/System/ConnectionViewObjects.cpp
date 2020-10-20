// ============================================================================
/*
 * @file ConnectionViewObjects.cpp
 * @brief インターネット接続アプリのViewクラスを持っているクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionViewObjects.h"
#include "NetApp/Connection/source/View/ConnectionUpperView.h"
#include "NetApp/Connection/source/View/ConnectionLowerView.h"
#include "NetApp/Connection/source/ConnectionResourceID.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


ConnectionViewObjects::ConnectionViewObjects() :
  m_pConnectionUpperView( NULL ),
  m_pConnectionLowerView( NULL )
{
}


ConnectionViewObjects::~ConnectionViewObjects()
{
  GFL_SAFE_DELETE( m_pConnectionLowerView );
  GFL_SAFE_DELETE( m_pConnectionUpperView );
}


void ConnectionViewObjects::CreateView( NetApp::Connection::System::ConnectionWork* pConnectionWork )
{
  app::util::Heap* pAppHeap = pConnectionWork->GetAppHeap();

  m_pConnectionUpperView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::View::ConnectionUpperView( pConnectionWork );
  m_pConnectionLowerView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::View::ConnectionLowerView( pConnectionWork );

  m_pConnectionUpperView->SetModal();
  m_pConnectionLowerView->SetModal();
}


void ConnectionViewObjects::InputDisableAll()
{
  m_pConnectionUpperView->SetInputEnabled( false );
  m_pConnectionLowerView->SetInputEnabled( false );
}


bool ConnectionViewObjects::IsDrawing()
{
  if( m_pConnectionUpperView->IsDrawing()
   || m_pConnectionLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

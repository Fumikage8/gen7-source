// ============================================================================
/*
 * @file ConnectionApplicationSystem.cpp
 * @brief インターネット接続アプリのアプリケーションシステムです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionApplicationSystem.h"
#include "NetApp/Connection/source/System/ConnectionViewList.h"
#include "NetApp/Connection/source/System/ConnectionViewObjects.h"
#include "NetApp/Connection/source/System/ConnectionViewSystemAccessor.h"
#include "NetApp/Connection/source/System/ConnectionBuilder.h"
#include "NetApp/Connection/source/ConnectionResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/net.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


ConnectionApplicationSystem::ConnectionApplicationSystem() :
  m_pConnectionWork( NULL ),
  m_pConnectionViewList( NULL ),
  m_pConnectionViewObjects( NULL ),
  m_pConnectionViewSystemAccessor( NULL )
{
}


ConnectionApplicationSystem::~ConnectionApplicationSystem()
{
  GFL_SAFE_DELETE( m_pConnectionViewSystemAccessor );
  GFL_SAFE_DELETE( m_pConnectionViewObjects );
  GFL_SAFE_DELETE( m_pConnectionViewList );
}


void ConnectionApplicationSystem::Build( NetApp::Connection::System::ConnectionWork* pConnectionWork, NetApp::Connection::System::ConnectionBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pConnectionWork->GetAppHeap();

  m_pConnectionWork               = pConnectionWork;
  m_pConnectionViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::System::ConnectionViewList( pAppHeap );
  m_pConnectionViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::System::ConnectionViewObjects();
  m_pConnectionViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::System::ConnectionViewSystemAccessor( m_pConnectionViewList, m_pConnectionViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pConnectionViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void ConnectionApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::Connection::CONNECTION_RESOURCE_ID_LAYOUT, ARCID_NET_CONNECT, GARC_net_net_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::Connection::CONNECTION_RESOURCE_ID_MESSAGE_CONNECTION, print::GetMessageArcId(), GARC_message_netconnect_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::Connection::CONNECTION_RESOURCE_ID_MESSAGE_FATAL_ERROR, print::GetMessageArcId(), GARC_message_message_error_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::Connection::CONNECTION_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
}


void ConnectionApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_netconnect_DAT, m_pResourceManager->GetResourceBuffer( NetApp::Connection::CONNECTION_RESOURCE_ID_MESSAGE_CONNECTION ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_message_error_DAT, m_pResourceManager->GetResourceBuffer( NetApp::Connection::CONNECTION_RESOURCE_ID_MESSAGE_FATAL_ERROR ) );
  m_pConnectionWork->SetMessageUtility( m_pMessageUtility );
}


void ConnectionApplicationSystem::CreateView()
{
  m_pConnectionWork->SetTrainerIconUtility( m_pNetAppTrainerIconUtility );

  m_pConnectionViewObjects->CreateView( m_pConnectionWork );
}


void ConnectionApplicationSystem::UpdateView()
{
  m_pConnectionViewList->Update();
}


void ConnectionApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pConnectionViewList->Draw( displayNo );
}


bool ConnectionApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pConnectionViewObjects )
  {
    if( m_pConnectionViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

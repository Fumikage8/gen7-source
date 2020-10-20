// ============================================================================
/*
 * @file P2PTradeApplicationSystem.cpp
 * @brief P2P交換アプリのアプリケーションシステムです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/System/P2PTradeApplicationSystem.h"
#include "NetApp/P2PTrade/source/System/P2PTradeViewList.h"
#include "NetApp/P2PTrade/source/System/P2PTradeViewObjects.h"
#include "NetApp/P2PTrade/source/System/P2PTradeViewSystemAccessor.h"
#include "NetApp/P2PTrade/source/System/P2PTradeBuilder.h"
#include "NetApp/P2PTrade/source/P2PTradeResourceID.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/net.gaix>
#include <arc_index/gts.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


P2PTradeApplicationSystem::P2PTradeApplicationSystem() :
  m_pP2PTradeWork( NULL ),
  m_pP2PTradeViewList( NULL ),
  m_pP2PTradeViewObjects( NULL ),
  m_pP2PTradeViewSystemAccessor( NULL )
{
}


P2PTradeApplicationSystem::~P2PTradeApplicationSystem()
{
  if( m_pP2PTradeViewList )
  {
    m_pP2PTradeViewList->RemoveAllView();
  }

  GFL_SAFE_DELETE( m_pP2PTradeViewSystemAccessor );
  GFL_SAFE_DELETE( m_pP2PTradeViewObjects );
  GFL_SAFE_DELETE( m_pP2PTradeViewList );
}


void P2PTradeApplicationSystem::Build( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork, NetApp::P2PTrade::System::P2PTradeBuilder* pApplicationSystemBuilder )
{
  app::util::Heap* pAppHeap = pP2PTradeWork->GetAppHeap();

  m_pP2PTradeWork               = pP2PTradeWork;
  m_pP2PTradeViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::System::P2PTradeViewList( pAppHeap );
  m_pP2PTradeViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::System::P2PTradeViewObjects();
  m_pP2PTradeViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::System::P2PTradeViewSystemAccessor( m_pP2PTradeViewList, m_pP2PTradeViewObjects );

  pApplicationSystemBuilder->SetViewSystemAccessor( m_pP2PTradeViewSystemAccessor );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void P2PTradeApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::P2PTrade::P2PTRADE_RESOURCE_ID_LAYOUT_CONNECTION, ARCID_NET_CONNECT, GARC_net_net_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::P2PTrade::P2PTRADE_RESOURCE_ID_MESSAGE_NET_SAVE, print::GetMessageArcId(), GARC_message_net_save_DAT, false, true );
}


void P2PTradeApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_net_save_DAT, m_pResourceManager->GetResourceBuffer( NetApp::P2PTrade::P2PTRADE_RESOURCE_ID_MESSAGE_NET_SAVE ) );
  m_pP2PTradeWork->SetMessageUtility( m_pMessageUtility );
}


void P2PTradeApplicationSystem::CreateView()
{
  m_pP2PTradeViewObjects->CreateView( m_pP2PTradeWork, *m_pNetAppMessageMenu );
}


void P2PTradeApplicationSystem::UpdateView()
{
  m_pP2PTradeViewList->Update();
}


void P2PTradeApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pP2PTradeViewList->Draw( displayNo );
}


bool P2PTradeApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pP2PTradeViewObjects )
  {
    if( m_pP2PTradeViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

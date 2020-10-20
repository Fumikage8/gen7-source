// ============================================================================
/*
 * @file P2PTradeViewObjects.cpp
 * @brief P2P交換アプリのViewクラスを持っているクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/System/P2PTradeViewObjects.h"
#include "NetApp/P2PTrade/source/View/P2PTradeUpperView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeLowerView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeMessageLowerView.h"
#include "NetApp/P2PTrade/source/View/P2PTradeWarningMessageLowerView.h"
#include "NetApp/P2PTrade/source/P2PTradeResourceID.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


P2PTradeViewObjects::P2PTradeViewObjects() :
  m_pP2PTradeUpperView( NULL ),
  m_pP2PTradeLowerView( NULL ),
  m_pP2PTradeMessageLowerView( NULL ),
  m_pP2PTradeWarningMessageLowerView( NULL )
{
}


P2PTradeViewObjects::~P2PTradeViewObjects()
{
  GFL_SAFE_DELETE( m_pP2PTradeWarningMessageLowerView );
  GFL_SAFE_DELETE( m_pP2PTradeMessageLowerView );
  GFL_SAFE_DELETE( m_pP2PTradeLowerView );
  GFL_SAFE_DELETE( m_pP2PTradeUpperView );
}


void P2PTradeViewObjects::CreateView( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu )
{
  app::util::Heap* pAppHeap = pP2PTradeWork->GetAppHeap();

  m_pP2PTradeUpperView                 = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::View::P2PTradeUpperView( pP2PTradeWork );
  m_pP2PTradeLowerView                 = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::View::P2PTradeLowerView( pP2PTradeWork );
  m_pP2PTradeMessageLowerView          = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::View::P2PTradeMessageLowerView( pP2PTradeWork, netAppMessageMenu );
  m_pP2PTradeWarningMessageLowerView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::View::P2PTradeWarningMessageLowerView( pP2PTradeWork );

  m_pP2PTradeUpperView->SetModal();
  m_pP2PTradeLowerView->SetModal();
  m_pP2PTradeMessageLowerView->SetModal();
  m_pP2PTradeWarningMessageLowerView->SetModal();
}


void P2PTradeViewObjects::InputDisableAll()
{
  m_pP2PTradeUpperView->SetInputEnabled( false );
  m_pP2PTradeLowerView->SetInputEnabled( false );
  m_pP2PTradeMessageLowerView->SetInputEnabled( false );
  m_pP2PTradeWarningMessageLowerView->SetInputEnabled( false );
}


bool P2PTradeViewObjects::IsDrawing()
{
  if( m_pP2PTradeUpperView->IsDrawing()
   || m_pP2PTradeLowerView->IsDrawing()
   || m_pP2PTradeMessageLowerView->IsDrawing()
   || m_pP2PTradeWarningMessageLowerView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

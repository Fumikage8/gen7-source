// ============================================================================
/*
 * @file MiracleTradeViewObjects.cpp
 * @brief ミラクル交換のDrawクラスを管理するクラスです。
 * @date 2015.05.29
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeViewObjects.h"
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmUpperView.h"
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmLowerView.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradeMatchingWaitLowerView.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePlayerInfoLowerView.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePokemonInfoUpperView.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


MiracleTradeViewObjects::MiracleTradeViewObjects() :
  m_pMiracleTradeStartConfirmUpperView( NULL ),
  m_pMiracleTradeStartConfirmLowerView( NULL ),
  m_pMiracleTradeMatchingWaitLowerView( NULL ),
  m_pMiracleTradePlayerInfoLowerView( NULL ),
  m_pMiracleTradePokemonInfoUpperView( NULL )
{
}


MiracleTradeViewObjects::~MiracleTradeViewObjects()
{
  GFL_SAFE_DELETE( m_pMiracleTradePokemonInfoUpperView );
  GFL_SAFE_DELETE( m_pMiracleTradePlayerInfoLowerView );
  GFL_SAFE_DELETE( m_pMiracleTradeMatchingWaitLowerView );
  GFL_SAFE_DELETE( m_pMiracleTradeStartConfirmLowerView );
  GFL_SAFE_DELETE( m_pMiracleTradeStartConfirmUpperView );
}


void MiracleTradeViewObjects::RemoveFromSuperViewAll()
{
  m_pMiracleTradeStartConfirmUpperView->RemoveFromSuperView();
  m_pMiracleTradeStartConfirmLowerView->RemoveFromSuperView();
  m_pMiracleTradeMatchingWaitLowerView->RemoveFromSuperView();
  m_pMiracleTradePlayerInfoLowerView->RemoveFromSuperView();
  m_pMiracleTradePokemonInfoUpperView->RemoveFromSuperView();
}


void MiracleTradeViewObjects::CreateDraw( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork )
{
  app::util::Heap* pAppHeap = pMiracleTradeWork->GetAppHeap();

  m_pMiracleTradeStartConfirmUpperView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmUpperView( pMiracleTradeWork );
  m_pMiracleTradeStartConfirmLowerView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmLowerView( pMiracleTradeWork );
  m_pMiracleTradeMatchingWaitLowerView   = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::Matching::MiracleTradeMatchingWaitLowerView( pMiracleTradeWork );
  m_pMiracleTradePlayerInfoLowerView     = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::Matching::MiracleTradePlayerInfoLowerView( pMiracleTradeWork );
  m_pMiracleTradePokemonInfoUpperView    = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::Matching::MiracleTradePokemonInfoUpperView( pMiracleTradeWork );
}


void MiracleTradeViewObjects::InputDisableAll()
{
  m_pMiracleTradeStartConfirmUpperView->SetInputEnabled( false );
  m_pMiracleTradeStartConfirmLowerView->SetInputEnabled( false );
  m_pMiracleTradeMatchingWaitLowerView->SetInputEnabled( false );
  m_pMiracleTradePlayerInfoLowerView->SetInputEnabled( false );
  m_pMiracleTradePokemonInfoUpperView->SetInputEnabled( false );
}


bool MiracleTradeViewObjects::IsDrawing()
{
  if( m_pMiracleTradeStartConfirmUpperView->IsDrawing()
   || m_pMiracleTradeStartConfirmLowerView->IsDrawing()
   || m_pMiracleTradeMatchingWaitLowerView->IsDrawing()
   || m_pMiracleTradePlayerInfoLowerView->IsDrawing()
   || m_pMiracleTradePokemonInfoUpperView->IsDrawing() )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

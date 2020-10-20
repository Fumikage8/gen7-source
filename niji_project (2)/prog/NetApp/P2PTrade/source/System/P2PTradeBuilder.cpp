// ============================================================================
/*
 * @file P2PTradeBuilder.cpp
 * @brief P2P交換アプリを構築するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/System/P2PTradeBuilder.h"
#include "NetApp/P2PTrade/source/System/P2PTradeFrameSwitcher.h"
#include "NetApp/P2PTrade/source/System/P2PTradeFrameFactory.h"
#include "NetApp/P2PTrade/source/P2PTradeResourceID.h"
#include "NetApp/P2PTrade/source/P2PTradeFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


P2PTradeBuilder::P2PTradeBuilder( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork ) :
  m_pP2PTradeWork( pP2PTradeWork ),
  m_pP2PTradeViewSystemAccessor( NULL )
{
}


P2PTradeBuilder::~P2PTradeBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* P2PTradeBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pP2PTradeWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::System::P2PTradeFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* P2PTradeBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pP2PTradeWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::P2PTrade::System::P2PTradeFrameFactory( m_pP2PTradeWork, m_pP2PTradeViewSystemAccessor );
}


NetAppLib::System::ApplicationWorkBase* P2PTradeBuilder::GetApplicationWork()
{
  return m_pP2PTradeWork;
}


NetAppLib::System::FrameID P2PTradeBuilder::GetApplicationStartFrameID()
{
  NetAppLib::System::FrameID startFrameID = NetApp::P2PTrade::BOOT_MODE_WARNING;

  switch( m_pP2PTradeWork->GetBootMode() )
  {
    case NetApp::P2PTrade::BOOT_MODE_WARNING:
      startFrameID = NetApp::P2PTrade::P2PTRADE_FRAME_ID_WARNING;
    break;

    case NetApp::P2PTrade::BOOT_MODE_SYNC_SAVE:
      startFrameID = NetApp::P2PTrade::P2PTRADE_FRAME_ID_SYNC_SAVE;
    break;

    case NetApp::P2PTrade::BOOT_MODE_SAVE:
      startFrameID = NetApp::P2PTrade::P2PTRADE_FRAME_ID_SAVE;
    break;
  }

  return startFrameID;
}


u32 P2PTradeBuilder::GetApplicationResourceCount()
{
  return NetApp::P2PTrade::P2PTRADE_RESOURCE_MAX;
}


bool P2PTradeBuilder::IsCreateMessageUtility()
{
  return true;
}


bool P2PTradeBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

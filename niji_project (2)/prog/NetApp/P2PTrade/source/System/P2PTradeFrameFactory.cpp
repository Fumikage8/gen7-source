// ============================================================================
/*
 * @file P2PTradeFrameFactory.cpp
 * @brief P2P交換アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/System/P2PTradeFrameFactory.h"
#include "NetApp/P2PTrade/source/P2PTradeFrameID.h"
#include "NetApp/P2PTrade/source/Frame/P2PTradeWarningFrame.h"
#include "NetApp/P2PTrade/source/Frame/P2PTradeSaveFrame.h"
#include "NetApp/P2PTrade/source/Frame/P2PTradeSyncSaveFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


P2PTradeFrameFactory::P2PTradeFrameFactory( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork, NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor ) :
  m_pP2PTradeWork( pP2PTradeWork ),
  m_pP2PTradeViewSystemAccessor( pP2PTradeViewSystemAccessor )
{
}


P2PTradeFrameFactory::~P2PTradeFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* P2PTradeFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pP2PTradeWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::P2PTrade::P2PTRADE_FRAME_ID_WARNING:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::P2PTrade::Frame::P2PTradeWarningFrame( m_pP2PTradeWork, m_pP2PTradeViewSystemAccessor );
    break;

    case NetApp::P2PTrade::P2PTRADE_FRAME_ID_SAVE:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::P2PTrade::Frame::P2PTradeSaveFrame( m_pP2PTradeWork, m_pP2PTradeViewSystemAccessor );
    break;

    case NetApp::P2PTrade::P2PTRADE_FRAME_ID_SYNC_SAVE:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::P2PTrade::Frame::P2PTradeSyncSaveFrame( m_pP2PTradeWork, m_pP2PTradeViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

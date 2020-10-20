// ============================================================================
/*
 * @file P2PTradeWork.cpp
 * @brief P2P交換アプリのワーククラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


P2PTradeWork::P2PTradeWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_eBootMode( NetApp::P2PTrade::BOOT_MODE_WARNING ),
  m_bIsFinalTradeSuccess( false ),
  m_bIsFinalTradeComplete( false ),
  m_bSyncSaveCancel( false ),
  m_bIsFinalSyncComplete( false ),
  m_bIsFinalSyncCancelComplete( false ),
  m_pP2PTradeRequestClient( NULL )
{
}


P2PTradeWork::~P2PTradeWork()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

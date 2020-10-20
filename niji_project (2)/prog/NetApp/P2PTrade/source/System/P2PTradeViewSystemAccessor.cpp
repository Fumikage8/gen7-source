// ============================================================================
/*
 * @file P2PTradeViewSystemAccessor.cpp
 * @brief P2P交換アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/System/P2PTradeViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


P2PTradeViewSystemAccessor::P2PTradeViewSystemAccessor(
  NetApp::P2PTrade::System::P2PTradeViewList* pP2PTradeViewList,
  NetApp::P2PTrade::System::P2PTradeViewObjects* pP2PTradeViewObjects ) :
  m_pP2PTradeViewList( pP2PTradeViewList ),
  m_pP2PTradeViewObjects( pP2PTradeViewObjects )
{
}


P2PTradeViewSystemAccessor::~P2PTradeViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

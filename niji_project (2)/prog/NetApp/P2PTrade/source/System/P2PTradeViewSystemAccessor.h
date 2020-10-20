// ============================================================================
/*
 * @file P2PTradeViewSystemAccessor.h
 * @brief P2P交換アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define P2PTRADE_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)

class P2PTradeViewList;
class P2PTradeViewObjects;

class P2PTradeViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeViewSystemAccessor );

public:

  P2PTradeViewSystemAccessor(
    NetApp::P2PTrade::System::P2PTradeViewList* pP2PTradeViewList,
    NetApp::P2PTrade::System::P2PTradeViewObjects* pP2PTradeViewObjects );

  virtual~P2PTradeViewSystemAccessor();

  NetApp::P2PTrade::System::P2PTradeViewList*  GetP2PTradeViewList() { return m_pP2PTradeViewList; }
  NetApp::P2PTrade::System::P2PTradeViewObjects*  GetP2PTradeViewObjects() { return m_pP2PTradeViewObjects; }

private:

  NetApp::P2PTrade::System::P2PTradeViewList*  m_pP2PTradeViewList;
  NetApp::P2PTrade::System::P2PTradeViewObjects*  m_pP2PTradeViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)


#endif // P2PTRADE_VIEWSYSTEMACCESSOR_H_INCLUDE

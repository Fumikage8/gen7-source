// ============================================================================
/*
 * @file P2PTradeFrameFactory.h
 * @brief P2P交換アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_FRAMEFACTORY_H_INCLUDE )
#define P2PTRADE_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


class P2PTradeViewSystemAccessor;


class P2PTradeFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeFrameFactory );
public:
  P2PTradeFrameFactory(
    NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
    NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor );
  virtual~P2PTradeFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::P2PTrade::System::P2PTradeWork*                m_pP2PTradeWork;
  NetApp::P2PTrade::System::P2PTradeViewSystemAccessor*  m_pP2PTradeViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADE_FRAMEFACTORY_H_INCLUDE

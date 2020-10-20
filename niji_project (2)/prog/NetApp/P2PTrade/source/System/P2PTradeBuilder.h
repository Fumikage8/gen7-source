// ============================================================================
/*
 * @file P2PTradeBuilder.h
 * @brief P2P交換アプリを構築するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_BUILDER_H_INCLUDE )
#define P2PTRADE_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


class P2PTradeViewSystemAccessor;


class P2PTradeBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeBuilder );
public:
  P2PTradeBuilder( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork );
  virtual~P2PTradeBuilder();

  void SetViewSystemAccessor( NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor )
  {
    m_pP2PTradeViewSystemAccessor = pP2PTradeViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();
  virtual bool IsCreateNetAppMessageMenu();

private:
  NetApp::P2PTrade::System::P2PTradeWork*                m_pP2PTradeWork;
  NetApp::P2PTrade::System::P2PTradeViewSystemAccessor*  m_pP2PTradeViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

// ============================================================================
/*
 * @file P2PTradeApplicationSystem.h
 * @brief P2P交換アプリのアプリケーションシステムです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_APPLICATIONSYSTEM_H_INCLUDE )
#define P2PTRADE_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)

class P2PTradeWork;
class P2PTradeBuilder;
class P2PTradeViewList;
class P2PTradeViewObjects;
class P2PTradeViewSystemAccessor;

class P2PTradeApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeApplicationSystem );

public:
  P2PTradeApplicationSystem();
  virtual~P2PTradeApplicationSystem();

  void Build( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork, NetApp::P2PTrade::System::P2PTradeBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  NetApp::P2PTrade::System::P2PTradeWork*                  m_pP2PTradeWork;
  NetApp::P2PTrade::System::P2PTradeViewList*              m_pP2PTradeViewList;
  NetApp::P2PTrade::System::P2PTradeViewObjects*           m_pP2PTradeViewObjects;
  NetApp::P2PTrade::System::P2PTradeViewSystemAccessor*    m_pP2PTradeViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)


#endif // P2PTRADE_APPLICATIONSYSTEM_H_INCLUDE

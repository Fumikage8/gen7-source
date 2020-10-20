// ============================================================================
/*
 * @file P2PTradeUpperView.h
 * @brief P2P交換アプリの上画面を表示するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADEUPPERVIEW_H_INCLUDE )
#define P2PTRADEUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(View)


class P2PTradeUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeUpperView );
public:
  P2PTradeUpperView( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork );
  virtual~P2PTradeUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_P2PTrade,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetApp::P2PTrade::System::P2PTradeWork*                         m_pP2PTradeWork;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADEUPPERVIEW_H_INCLUDE

// ============================================================================
/*
 * @file P2PTradeLowerView.h
 * @brief P2P交換アプリの下画面を表示するクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADELOWERVIEW_H_INCLUDE )
#define P2PTRADELOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(View)


class P2PTradeLowerView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeLowerView );
public:
  P2PTradeLowerView( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork );
  virtual~P2PTradeLowerView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );



private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_P2PTRADE,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };


private:

  NetApp::P2PTrade::System::P2PTradeWork*                         m_pP2PTradeWork;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADELOWERVIEW_H_INCLUDE

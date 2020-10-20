// ============================================================================
/*
 * @file P2PTradeViewObjects.h
 * @brief P2P交換アプリのViewクラスを持っているクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_VIEWOBJECTS_H_INCLUDE )
#define P2PTRADE_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppMessageMenu;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)

GFL_NAMESPACE_BEGIN(View)
class P2PTradeUpperView;
class P2PTradeLowerView;
class P2PTradeMessageLowerView;
class P2PTradeWarningMessageLowerView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class P2PTradeWork;

class P2PTradeViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeViewObjects );
public:
  P2PTradeViewObjects();
  virtual~P2PTradeViewObjects();

  void CreateView( NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetApp::P2PTrade::View::P2PTradeUpperView*                   GetP2PTradeUpperView() { return m_pP2PTradeUpperView; }
  NetApp::P2PTrade::View::P2PTradeLowerView*                   GetP2PTradeLowerView() { return m_pP2PTradeLowerView; }
  NetApp::P2PTrade::View::P2PTradeMessageLowerView*            GetP2PTradeMessageLowerView() { return m_pP2PTradeMessageLowerView; }
  NetApp::P2PTrade::View::P2PTradeWarningMessageLowerView*     GetP2PTradeWarningMessageLowerView() { return m_pP2PTradeWarningMessageLowerView; }

private:

  NetApp::P2PTrade::View::P2PTradeUpperView*                   m_pP2PTradeUpperView;
  NetApp::P2PTrade::View::P2PTradeLowerView*                   m_pP2PTradeLowerView;
  NetApp::P2PTrade::View::P2PTradeMessageLowerView*            m_pP2PTradeMessageLowerView;
  NetApp::P2PTrade::View::P2PTradeWarningMessageLowerView*     m_pP2PTradeWarningMessageLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)


#endif // P2PTRADE_VIEWOBJECTS_H_INCLUDE

// ============================================================================
/*
 * @file P2PTradeFrameBase.h
 * @brief P2P交換アプリのフレームの基底クラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_FRAMEBASE_H_INCLUDE )
#define P2PTRADE_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"
#include "NetApp/P2PTrade/source/System/P2PTradeViewList.h"
#include "NetApp/P2PTrade/source/System/P2PTradeViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)

GFL_NAMESPACE_BEGIN(System)
class P2PTradeViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class P2PTradeFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeFrameBase );
public:
  P2PTradeFrameBase(
    NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
    NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor );
  virtual~P2PTradeFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::P2PTrade::System::P2PTradeWork*            m_pP2PTradeWork;
  NetApp::P2PTrade::System::P2PTradeViewList*        m_pP2PTradeViewList;
  NetApp::P2PTrade::System::P2PTradeViewObjects*     m_pP2PTradeViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADE_FRAMEBASE_H_INCLUDE

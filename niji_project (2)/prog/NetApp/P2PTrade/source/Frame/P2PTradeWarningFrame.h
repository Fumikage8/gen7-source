// ============================================================================
/*
 * @file P2PTradeWarningFrame.h
 * @brief P2P交換アプリの○○プロセスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADEWARNING_FRAME_H_INCLUDE )
#define P2PTRADEWARNING_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/P2PTrade/source/Frame/P2PTradeFrameBase.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(Frame)


class P2PTradeWarningFrame : public NetApp::P2PTrade::Frame::P2PTradeFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeWarningFrame );
public:
  P2PTradeWarningFrame(
    NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
    NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor );
  virtual~P2PTradeWarningFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  NetAppLib::Util::NetAppTimeout      m_Timer;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADEWARNING_FRAME_H_INCLUDE

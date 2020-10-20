// ============================================================================
/*
 * @file P2PTradeSyncSaveFrame.h
 * @brief P2P交換アプリの○○プロセスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADESYNCSAVE_FRAME_H_INCLUDE )
#define P2PTRADESYNCSAVE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/P2PTrade/source/Frame/P2PTradeFrameBase.h"
#include "NetApp/P2PTrade/source/System/P2PTradeWork.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"
#include "NetApp/P2PTrade/source/View/P2PTradeMessageLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(Frame)


class P2PTradeSyncSaveFrame : public NetApp::P2PTrade::Frame::P2PTradeFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeSyncSaveFrame );
public:
  P2PTradeSyncSaveFrame(
    NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
    NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor );
  virtual~P2PTradeSyncSaveFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  NetAppLib::Util::NetAppCommonSaveUtility                       m_NetAppCommonSaveUtility;
  NetApp::P2PTrade::View::P2PTradeMessageLowerViewListener       m_P2PTradeMessageLowerViewListener;
  bool                                                           m_bMyCancelFlag;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADESYNCSAVE_FRAME_H_INCLUDE

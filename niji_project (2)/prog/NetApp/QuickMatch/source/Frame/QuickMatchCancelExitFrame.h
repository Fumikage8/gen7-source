// ============================================================================
/*
 * @file QuickMatchCancelExitFrame.h
 * @brief クイックマッチアプリのキャンセル終了プロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCHCANCEL_FRAME_H_INCLUDE )
#define QUICKMATCHCANCEL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


class QuickMatchCancelExitFrame : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchCancelExitFrame );
public:
  QuickMatchCancelExitFrame(
    NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork );
  virtual~QuickMatchCancelExitFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  NetApp::QuickMatch::System::QuickMatchWork*                    m_pQuickMatchWork;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCHCANCEL_FRAME_H_INCLUDE

// ============================================================================
/*
 * @file QuickMatchClientFrame.h
 * @brief クイックマッチアプリのクライアントプロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCHCLIENT_FRAME_H_INCLUDE )
#define QUICKMATCHCLIENT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


class QuickMatchClientFrame : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchClientFrame );
private:

  enum
  {
    INVALID_MESSAGE_ID = -1,
  };

public:

  QuickMatchClientFrame(
    NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork );
  virtual~QuickMatchClientFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  s32 CheckTimingSeq();
  void LastUpdateSeq();

private:

  NetApp::QuickMatch::System::QuickMatchWork*                    m_pQuickMatchWork;
  bool m_isLastUpdate;
  u32 m_lastTimingNo;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCHCLIENT_FRAME_H_INCLUDE

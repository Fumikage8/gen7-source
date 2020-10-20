// ============================================================================
/*
 * @file QuickMatchHostFrame.h
 * @brief クイックマッチアプリのホストプロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCHHOST_FRAME_H_INCLUDE )
#define QUICKMATCHHOST_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


class QuickMatchHostFrame : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchHostFrame );
private:

  enum MAIN_SEQ
  {
    SELECT_MODE =0,
    SELECT_EXIT,
    MESSAGE_STREAM_WAIT,
    MESSAGE_TIMER_WAIT,
    EXIT_SEQ
  };

public:
  QuickMatchHostFrame(
    NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork );
  virtual~QuickMatchHostFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  void UpdateSelectMode();
  void UpdateSelectExit();
  void MessageStreamWait();
  void MessageTimerWait();

  void CheckTimingSeq();
  void LastUpdateSeq();

  void SetMainSeq( u32 mainSeq );

private:

  NetApp::QuickMatch::System::QuickMatchWork*                    m_pQuickMatchWork;

  u32 m_mainSeq;
  u32 m_subSeq;

  u32 m_messageNextSeq;
  u32 m_messageId;
  bool m_isTimerEnd;
  bool m_isLastUpdate;
  u32 m_timingNetGameIndex;
  u32 m_timingNo;

  s32 m_tradeNGMessageNo;
  s32 m_tradeNGPlayerIndex;
  s32 m_doubleNGMessageNo;
  s32 m_doubleNGPlayerIndex;
  u32 m_lastTimingNo;
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCHHOST_FRAME_H_INCLUDE

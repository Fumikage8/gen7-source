// ============================================================================
/*
 * @file EndRollFrame.h
 * @brief エンドロールアプリのプロセスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APPENDROLLSAMPLE_FRAME_H_INCLUDE )
#define APPENDROLLSAMPLE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "App/EndRoll/source/System/EndRollWork.h"


//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
  class TimeLimit;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(Frame)


class EndRollFrame : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( EndRollFrame );
public:
  EndRollFrame(
    App::EndRoll::System::EndRollWork* pEndRollWork );
  virtual~EndRollFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  void _Clear()
  {
    m_pEndRollWork  = NULL;
    m_pEnterInputTimeLimit  = NULL;
    m_updateSeq = 0;
  }

  App::EndRoll::System::EndRollWork*                    m_pEndRollWork;
  App::Tool::TimeLimit*                                 m_pEnterInputTimeLimit;

  u32 m_updateSeq;
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // APPENDROLLSAMPLE_FRAME_H_INCLUDE

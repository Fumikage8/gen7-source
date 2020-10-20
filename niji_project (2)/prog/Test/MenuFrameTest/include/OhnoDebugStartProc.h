//==============================================================================
/**
 @file    OhnoDebugStartProc.h REQID_CALL
 @author  k.ohno
 @date    2015.12.22
 */
//==============================================================================
#if PM_DEBUG

#ifndef __OHNO_DEBUG_START_PROC_H__
#define __OHNO_DEBUG_START_PROC_H__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>



GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(MenuFrameTest)






//==============================================================================
/**
 @class     KawadaTestProc
 @brief     KawadaTest
 */
//==============================================================================
class OhnoDebugStartProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(OhnoDebugStartProc);

public:
  OhnoDebugStartProc(void);
  virtual ~OhnoDebugStartProc();

  void SetNo(int no);

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );


private:
 int mNo;
  int mSeqCnt;

};


GFL_NAMESPACE_END(MenuFrameTest)
GFL_NAMESPACE_END(Test)

#endif  // __OHNO_DEBUG_START_PROC_H__

#endif // PM_DEBUG

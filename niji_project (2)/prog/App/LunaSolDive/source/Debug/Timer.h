//======================================================================
/**
 * @file Timer.h
 * @date 2017/02/27 9:21:33
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#if !defined __LUNA_SOL_DIVE__DEBUG__HEADER_H_INCLUDED__
#define __LUNA_SOL_DIVE__DEBUG__HEADER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

namespace LunaSolDive{
namespace Debug{
class Timer
{
  GFL_FORBID_COPY_AND_ASSIGN(Timer);
public:
  // constructor
  Timer();
  // destructor
  ~Timer();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  void Terminate();
  // Update
  void Update();
  // Number of frames from game start
  const u32& ElapsedFrames() const;
  // String representing elapsed time
  const wchar_t* ElapsedTimeStr() const;
private:
  // Number of frames from game start
  u32 m_ElapsedFrames;
  // String representing elapsed time
  wchar_t* m_ElapsedTimeStr;
};
}// namespace Debug
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__DEBUG__HEADER_H_INCLUDED__
#endif // PM_DEBUG

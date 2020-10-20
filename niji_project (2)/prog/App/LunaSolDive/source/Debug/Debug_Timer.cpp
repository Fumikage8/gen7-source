//======================================================================
/**
 * @file Debug_Timer.cpp
 * @date 2017/02/27 9:21:57
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#include "./Timer.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Debug{
// parameter
static const u32 STR_SIZE = 15;// "MM...:SS.SSS"
// constructor
Timer::Timer()
    : m_ElapsedFrames(0),
      m_ElapsedTimeStr(NULL){}
// destructor
Timer::~Timer()
{
  Terminate();
}
// Initialize
void Timer::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  m_ElapsedFrames = 0;
  m_ElapsedTimeStr = GFL_NEW(pHeap) wchar_t[STR_SIZE];
}
// Terminate
void Timer::Terminate()
{
  m_ElapsedFrames = 0;
  GFL_SAFE_DELETE(m_ElapsedTimeStr);
}
// Update
void Timer::Update()
{
  // incremant count
  ++m_ElapsedFrames;
  // string
  u32 frames = m_ElapsedFrames;
  const u32 FPS = 30;
  const u32 minute = frames / (FPS * 60);
  frames %= FPS * 60;
  const f32 second = static_cast<f32>(frames) / FPS;
  std::swprintf(
              m_ElapsedTimeStr,
              STR_SIZE,
              L"%u:%06.3f",
              minute,
              second);
}
// Number of frames from game start
const u32& Timer::ElapsedFrames() const
{
  return m_ElapsedFrames;
}
// String representing elapsed time
const wchar_t* Timer::ElapsedTimeStr() const
{
  LUNA_SOL_DIVE_ASSERT(m_ElapsedTimeStr != NULL);
  return m_ElapsedTimeStr;
}
}// namespace Debug
}// namespace LunaSolDive
#endif// PM_DEBUG

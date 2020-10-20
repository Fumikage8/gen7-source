//=============================================================================
/**
 * @file	 gfl_threadstatic_adapter_3ds.cpp
 * @brief	 3dsでのスレッド実装部
 * @author Hiroyuki Nakamura->k.ohno
 * @date	 2014.12.24
 */
//=============================================================================
#ifdef GFL_PLATFORM_3DS

#include <thread/include/gfl_threadstatic.h>
#include <thread/source/gfl_threadstatic_adapter.h>
#include <thread/source/gfl_threadstatic_adapter_3ds.h>

#include <precompile/Precompile.hpp>
#include <util/Thread.hpp>
#include <mem/HeapExp.hpp>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


ThreadStaticAdapter3DS::ThreadStaticAdapter3DS(void)
{
}

ThreadStaticAdapter3DS::~ThreadStaticAdapter3DS()
{
}


void ThreadStaticAdapter3DS::CurrentYield( void )
{
  nn::os::Thread::Yield();
}

void ThreadStaticAdapter3DS::CurrentSleep( s32 milliSeconds )
{
  nn::fnd::TimeSpan::FromMilliSeconds mil(milliSeconds);
  nn::os::Thread::Sleep(mil);
}

void ThreadStaticAdapter3DS::CurrentSleepMicro( s32 microSeconds )
{
  nn::fnd::TimeSpan::FromMicroSeconds  mic(microSeconds);
  nn::os::Thread::Sleep(mic);
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL_PLATFORM_3DS

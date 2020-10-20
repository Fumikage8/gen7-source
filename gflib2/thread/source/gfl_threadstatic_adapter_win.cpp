//=============================================================================
/**
 * @file    gfl_threadstatic_adapter_win.cpp
 * @brief   std::threadが使用できる環境のプログラム
 * @author  ohno_katsumi
 * @date    2014.05.13
 */
//=============================================================================
#ifndef GFL_PLATFORM_3DS

#include <thread/include/gfl_threadstatic.h>
#include <thread/source/gfl_threadstatic_adapter.h>
#include <thread/source/gfl_threadstatic_adapter_win.h>



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


ThreadStaticAdapterWin::ThreadStaticAdapterWin(void)
{
}

ThreadStaticAdapterWin::~ThreadStaticAdapterWin()
{
}

void ThreadStaticAdapterWin::CurrentYield( void )
{
  ::std::this_thread::yield();
}

void ThreadStaticAdapterWin::CurrentSleep( s32 milliSeconds )
{
  ::std::this_thread::sleep_for( std::chrono::milliseconds(milliSeconds) );
}

void ThreadStaticAdapterWin::CurrentSleepMicro( s32 microSeconds )
{
  ::std::this_thread::sleep_for( std::chrono::microseconds(microSeconds) );
}



GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL_PLATFORM_FRAT

//=============================================================================
/**
 * @file    gfl_threadstatic.cpp
 * @brief   スレッド
 * @author  ohno_katsumi
 * @date    2014.12.24
 */
//=============================================================================
#include <thread/include/gfl2_ThreadStatic.h>

#if defined(GF_PLATFORM_CTR)

#include "nn/os.h"

#elif defined(GF_PLATFORM_WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)

void ThreadStatic::CurrentYield( void )
{

#if defined(GF_PLATFORM_CTR)

  nn::os::Thread::Yield();

#elif defined(GF_PLATFORM_WIN32)

  ::std::this_thread::yield();

#endif


}

void ThreadStatic::CurrentSleep( s32 milliSeconds )
{
#if defined(GF_PLATFORM_CTR)
  nn::fnd::TimeSpan mil =  nn::fnd::TimeSpan::FromMilliSeconds(milliSeconds);
  nn::os::Thread::Sleep(mil);
#elif defined(GF_PLATFORM_WIN32)

  ::std::this_thread::sleep_for( std::chrono::milliseconds(milliSeconds) );

#endif

}

void ThreadStatic::CurrentSleepMicro( s32 microSeconds )
{
#if defined(GF_PLATFORM_CTR)

  nn::fnd::TimeSpan mic = nn::fnd::TimeSpan::FromMicroSeconds(microSeconds);
  nn::os::Thread::Sleep(mic);

#elif defined(GF_PLATFORM_WIN32)

  ::std::this_thread::sleep_for( std::chrono::microseconds(microSeconds) );

#endif

}


b32 ThreadStatic::IsMainThread( bit32 thread_id )
{
#if defined(GF_PLATFORM_CTR)
  const nn::os::Thread& thread = nn::os::Thread::GetMainThread();
  if( thread_id == thread.GetId() ){
    return true;
  }
  return false;
#else
  GFL_ASSERT( 0 );
  return false;
#endif
}

bit32 ThreadStatic::GetCurrentId( void )
{
#if defined(GF_PLATFORM_CTR)
  return nn::os::Thread::GetCurrentId();
#else
  GFL_ASSERT( 0 );
  return 0;
#endif
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)


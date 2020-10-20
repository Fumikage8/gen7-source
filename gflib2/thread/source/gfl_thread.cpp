//=============================================================================
/**
 * @file    gfl_thread.cpp
 * @brief   スレッド
 * @author  ohno_katsumi
 * @date    2014.05.13
 */
//=============================================================================
#include <thread/include/gfl2_Thread.h>
#include <thread/source/gfl_thread_adapter.h>


#if defined(GF_PLATFORM_WIN32)
#include "thread/source/gfl_thread_adapter_win.h"
#elif defined(GF_PLATFORM_CTR)
#include "thread/source/gfl_thread_adapter_3ds.h"

#endif  // GFL_PLATFORM_


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)

Thread::Thread(ThreadInterface* pInterFace, gfl2::heap::HeapBase* pHeap, bool bMainLoop, u32 stackSize )
{
#if defined(GF_PLATFORM_WIN32)
  mpThreadAdapter = GFL_NEW(pHeap)ThreadAdapterWin(pInterFace, pHeap, bMainLoop);
#elif defined(GF_PLATFORM_CTR)
  mpThreadAdapter = GFL_NEW(pHeap)ThreadAdapter3ds(pInterFace, pHeap, bMainLoop, stackSize);
#else
  mpThreadAdapter = NULL;
  STATIC_ASSERT(0);  ///増やす場合はここについか
#endif
}

Thread::~Thread()
{
  GFL_SAFE_DELETE(mpThreadAdapter);
}

int Thread::GetPriority(void)
{
  return mpThreadAdapter->GetPriority();
}

void Thread::SetPriority(int no)
{
  mpThreadAdapter->SetPriority(no);
}

void Thread::Start(void)
{
  mpThreadAdapter->Start();
}

void Thread::Start(int priority)
{
  mpThreadAdapter->Start(priority);
}


void Thread::Wait(void)
{
  mpThreadAdapter->Wait();
}

void Thread::CurrentYield( void )
{
  mpThreadAdapter->CurrentYield();
}

void Thread::CurrentSleep( s32 milliSeconds )
{
  mpThreadAdapter->CurrentSleep( milliSeconds );
}

void Thread::CurrentSleepMicro( s32 microSeconds )
{
  mpThreadAdapter->CurrentSleepMicro( microSeconds );
}

bool Thread::IsAlive(void)
{
  return mpThreadAdapter->IsAlive();
}

void Thread::Kill(void)
{
  mpThreadAdapter->Kill();
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)


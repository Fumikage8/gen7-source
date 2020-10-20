//=============================================================================
/**
 * @file    gfl_thread_adapter_win.cpp
 * @brief   std::threadが使用できる環境のプログラム
 * @author  ohno_katsumi
 * @date    2014.05.13
 */
//=============================================================================
#if defined(GF_PLATFORM_WIN32)

#include <windows.h>
#include <thread/include/gfl2_Thread.h>
#include <thread/source/gfl_thread_adapter.h>
#include <thread/source/gfl_thread_adapter_win.h>



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)



ThreadAdapterWin::ThreadAdapterWin(ThreadInterface* pInterFace,gfl2::heap::HeapBase* pHeap,bool bMainLoop)
{
  mpHeap = pHeap;
  mpThread = NULL;
  mpInterFace = pInterFace;
  mbMainLoop = bMainLoop;

  mpMyDead = GFL_NEW(mpHeap) Event(pHeap);
  mpKillReq = GFL_NEW(mpHeap) Event(pHeap);
}

ThreadAdapterWin::~ThreadAdapterWin()
{
  mpThread->join();  //一回は呼ぶ必要がある
  mpMyDead->Finalize();
  mpKillReq->Finalize();

  GFL_SAFE_DELETE(mpThread);
  GFL_SAFE_DELETE(mpKillReq);
  GFL_SAFE_DELETE(mpMyDead);
}



int ThreadAdapterWin::GetPriority(void)
{
  return GetThreadPriority(mpThread->native_handle());
}


void ThreadAdapterWin::SetPriority(int no)
{
  // ※スレッドのプライオリティー操作は開発時に必ずなくす必要がある不具合である
  GFL_ASSERT(SetThreadPriority(mpThread->native_handle(), no ));
}


void ThreadAdapterWin::Start(void)
{
  mpMyDead->Initialize();
  mpKillReq->Initialize();
  mpThread = GFL_NEW(mpHeap) ::std::thread(::std::ref(*this));
}

void ThreadAdapterWin::Start(int priority)
{
  int priorityOffset = priority - Thread::THREAD_PRI_NORMAL;//WIN版に変換するためオフセット取得
  Start();
  SetThreadPriority(mpThread->native_handle(), THREAD_PRIORITY_NORMAL + (-priorityOffset) );
}



void ThreadAdapterWin::Wait(void)
{
  mpThread->join();
}

void ThreadAdapterWin::operator()()
{
  while(1){

    mpInterFace->Function();

    if(!mbMainLoop){
      break;
    }
    if(mpKillReq->TryWait()){
      break;
    }
    CurrentYield();
  }
  mpMyDead->Signal();
}

void ThreadAdapterWin::CurrentYield( void )
{
  ::std::this_thread::yield();
}


void ThreadAdapterWin::CurrentSleep( s32 milliSeconds )
{
  ::std::this_thread::sleep_for( std::chrono::milliseconds(milliSeconds) );
}

void ThreadAdapterWin::CurrentSleepMicro( s32 microSeconds )
{
  ::std::this_thread::sleep_for( std::chrono::microseconds(microSeconds) );
}

bool ThreadAdapterWin::IsAlive(void)
{
  return mpMyDead->TryWait();
}

void ThreadAdapterWin::Kill(void)
{
  mpKillReq->Signal();
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL_PLATFORM_FRAT

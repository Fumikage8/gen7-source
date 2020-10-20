//=============================================================================
/**
 * @file	 gfl_thread_adapter_3ds.cpp
 * @brief	 Fratでのスレッド実装部
 * @author Hiroyuki Nakamura
 * @date	 2014.10.17
 */
//=============================================================================
#if defined(GF_PLATFORM_CTR)

#include <thread/include/gfl2_Thread.h>
#include <thread/source/gfl_thread_adapter.h>
#include <thread/source/gfl_thread_adapter_3ds.h>



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


ThreadAdapter3ds::ThreadAdapter3ds(ThreadInterface* pInterFace, gfl2::heap::HeapBase* pHeap, bool bMainLoop, u32 stackSize )
{
  mpHeap = pHeap;
  mpThread = NULL;
  mpInterFace = pInterFace;
  mbMainLoop = bMainLoop;

  mEndFlag = false;

  mpKillReq = GFL_NEW(mpHeap) Event(pHeap);


  GFL_ASSERT(stackSize);
	mpStack = GFL_NEW(mpHeap) SimpleStackArea(mpHeap,stackSize);


  mpThread = GFL_NEW(mpHeap) nn::os::Thread();	

}

ThreadAdapter3ds::~ThreadAdapter3ds()
{
  mpThread->Join();  //一回は呼ぶ必要がある
  mpKillReq->Finalize();
  
  GFL_SAFE_DELETE(mpKillReq);
  GFL_SAFE_DELETE(mpThread);
  GFL_SAFE_DELETE(mpStack);
}



int ThreadAdapter3ds::GetPriority(void)
{
  return mpThread->GetPriority();
}


void ThreadAdapter3ds::SetPriority(int no)
{
  mpThread->ChangePriority( no );
}

void ThreadAdapter3ds::Start(void)
{
  mpKillReq->Initialize();
  mpThread->Start(&Function,this,  *mpStack );

}


void ThreadAdapter3ds::Start(int no)
{
  mpKillReq->Initialize();
  mpThread->Start(&Function,this,  *mpStack , no);
}

void ThreadAdapter3ds::Wait(void)
{
  mpThread->Join();
}

void ThreadAdapter3ds::Function( void * arg_ptr )
{
  ThreadAdapter3ds * thread_ptr = static_cast<gfl2::thread::ThreadAdapter3ds*>(arg_ptr);

  while(1){

    thread_ptr->mpInterFace->Function();

    if(!thread_ptr->mbMainLoop){
      break;
    }
    if(thread_ptr->mpKillReq->TryWait()){
      break;
    }
    nn::os::Thread::Yield();
  }

}

void ThreadAdapter3ds::CurrentYield( void )
{
	nn::os::Thread::Yield();
}

void ThreadAdapter3ds::CurrentSleep( s32 milliSeconds )
{
  nn::fnd::TimeSpan mil =  nn::fnd::TimeSpan::FromMilliSeconds(milliSeconds);
  nn::os::Thread::Sleep(mil);
}

void ThreadAdapter3ds::CurrentSleepMicro( s32 microSeconds )
{
  nn::fnd::TimeSpan mic = nn::fnd::TimeSpan::FromMicroSeconds(microSeconds);
  nn::os::Thread::Sleep(mic);
}

bool ThreadAdapter3ds::IsAlive(void)
{
  return mpThread->IsAlive();
}

void ThreadAdapter3ds::Kill(void)
{
  mpKillReq->Signal();
}









GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL_PLATFORM_3DS

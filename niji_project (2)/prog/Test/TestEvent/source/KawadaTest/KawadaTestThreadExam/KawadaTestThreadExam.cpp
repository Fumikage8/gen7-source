//==============================================================================
/**
 @file    KawadaTestThreadExam.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.15
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_thread.h>
#include <thread/include/gfl2_threadstatic.h>

// KawadaTestのインクルード
#include <Test/TestEvent/include/KawadaTest/KawadaTestThreadExam/KawadaTestThreadExam.h>




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//! @class  KawadaTestCountThread
class KawadaTestCountThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestCountThread);

public:
  KawadaTestCountThread(gfl2::heap::HeapBase* heap, KawadaTestThreadExam* exam)
    : m_exam(exam),
      m_thread(NULL),
      m_endReq(false),
      m_hasEnded(false)
  {
    m_thread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, false, DEFAULT_STACK_SIZE);
    m_thread->Start(gfl2::thread::Thread::THREAD_PRI_NORMAL);
  }
  virtual ~KawadaTestCountThread()
  {
    for ever
    {
      if(m_hasEnded) break;
    }
    GFL_SAFE_DELETE(m_thread);
  }

  virtual void Function(void)
  {
    for ever
    {
      m_exam->DecreaseTestCount(1);

      if(m_endReq) break;
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
    m_hasEnded = true;
  }

public:
  void RequestEnd(void)
  {
    m_endReq = true;
  }

private:
  KawadaTestThreadExam*  m_exam;
  gfl2::thread::Thread*  m_thread;
  bool                   m_endReq;
  bool                   m_hasEnded;
  static const size_t DEFAULT_STACK_SIZE = 4096; ///< デフォルトスタックサイズ
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


KawadaTestThreadExam::KawadaTestThreadExam(gfl2::heap::HeapBase* heap)
  : m_testCount(0),
    m_testCountCriticalSection(NULL),
    m_testCountThread(NULL)
{
  m_testCountCriticalSection = GFL_NEW(heap) gfl2::thread::CriticalSection();
  m_testCountCriticalSection->Initialize();

  m_testCountThread = GFL_NEW(heap) KawadaTestCountThread(heap, this);
}
KawadaTestThreadExam::~KawadaTestThreadExam()
{
  GFL_SAFE_DELETE(m_testCountThread);

  GFL_SAFE_DELETE(m_testCountCriticalSection);
}

void KawadaTestThreadExam::Update(void)
{
  u32 loopCount = 0;
  for ever
  {
    this->IncreaseTestCount(100);

    if(loopCount >= 30) break;
    ++loopCount;
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  m_testCountThread->RequestEnd();
  gfl2::thread::ThreadStatic::CurrentSleep(1);
}

#define KAWADA_TEST_THREAD_EXAM_INCREASE_TEST_COUNT() \
  { \
    gfl2::thread::ScopedLock testCountScopedLock(*m_testCountCriticalSection); \
    m_testCount += 1; \
  }
 

void KawadaTestThreadExam::IncreaseTestCount(s32 val)
{
  KAWADA_TEST_THREAD_EXAM_INCREASE_TEST_COUNT()

  {
    gfl2::thread::ScopedLock testCountScopedLock(*m_testCountCriticalSection);
  }
  {
    gfl2::thread::ScopedLock testCountScopedLock(*m_testCountCriticalSection);

    m_testCount += val;
  }
}
void KawadaTestThreadExam::DecreaseTestCount(s32 val)
{
  {
    gfl2::thread::ScopedLock testCountScopedLock(*m_testCountCriticalSection);
  }
  {
    gfl2::thread::ScopedLock testCountScopedLock(*m_testCountCriticalSection);
  
    m_testCount -= val;
  }
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


//==============================================================================
/**
 @file    KawadaTestInfoWindowSample.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.01
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

// nijiのインクルード
#include <Test/TestEvent/include/KawadaTest/KawadaTestInfoWindowSample/KawadaTestInfoWindowSampleEvent.h>

// KawadaTestのインクルード
#include "KawadaTestInfoWindowSample.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


KawadaTestInfoWindowSample::KawadaTestInfoWindowSample(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
  : KawadaTestSampleBase(gameManager, parentHeapMem, parentDevMem)
{
  this->initialize(gameManager, parentHeapMem, parentDevMem);
}
KawadaTestInfoWindowSample::~KawadaTestInfoWindowSample()
{
  this->terminate();
}

void KawadaTestInfoWindowSample::Update(void)
{}

void KawadaTestInfoWindowSample::PreDraw(void)
{}

void KawadaTestInfoWindowSample::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{}

void KawadaTestInfoWindowSample::initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
{
  static const s32 HEAP_MEM_SIZE  =  0x100000 - 0x1000;
  static const s32 DEV_MEM_SIZE   = 0x1000000 - 0x1000;
 
  m_enableToEnd = false;
  m_gameManager = gameManager;

  m_heapMem     = GFL_CREATE_LOCAL_HEAP(parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_devMem      = GFL_CREATE_LOCAL_HEAP(parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_uniMem      = m_devMem;

  // イベントを起動し直ちに終了する
  KawadaTestInfoWindowSampleEvent::StartEvent(gameManager);
  m_enableToEnd = true;
}

void KawadaTestInfoWindowSample::terminate(void)
{
  if(m_enableToEnd)
  {
    m_uniMem = NULL;
    if(m_devMem)
    {
      GFL_DELETE_HEAP(m_devMem);
      m_devMem = NULL;
    }
    if(m_heapMem)
    {
      GFL_DELETE_HEAP(m_heapMem);
      m_heapMem = NULL;
    }

    m_gameManager = NULL;
  }
  else
  {
    GFL_ASSERT(0);
  }
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


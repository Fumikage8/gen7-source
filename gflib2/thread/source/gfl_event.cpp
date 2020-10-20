//=============================================================================
/**
 * @file    gfl_event.cpp
 * @brief   イベントクラス
 * @author  ohno_katsumi
 * @date    2014.05.13
 */
//=============================================================================


#include "../include/gfl2_Event.h"
#include "gfl_event_adapter.h"

#if defined(GF_PLATFORM_WIN32)
#include "gfl_event_adapter_win.h"
#elif defined(GF_PLATFORM_CTR)
#include "gfl_event_adapter_3ds.h"
#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)

Event::Event(gfl2::heap::HeapBase* pHeap)
{
  mpHeap = pHeap;
  mpEventAdapter = NULL;
}


void Event::Initialize(void)
{
#if defined(GF_PLATFORM_WIN32)
  mpEventAdapter = GFL_NEW(mpHeap) EventAdapterWin();
#elif defined(GF_PLATFORM_CTR)
  mpEventAdapter = GFL_NEW(mpHeap) EventAdapter3ds();
#else
  mpEventAdapter = NULL;
  STATIC_ASSERT(0);  ///  機種依存を追加する場合にここに書く
#endif


}

void Event::Finalize(void)
{
  GFL_SAFE_DELETE( mpEventAdapter );
}


//-----------------------------------------------------------------------
/**
 * @brief フラグがセットされているかを確認します
 */
//-----------------------------------------------------------------------
bool Event::TryWait(void)
{
  return mpEventAdapter->TryWait();
}

//-----------------------------------------------------------------------
/**
 * @brief フラグがセットされているかを確認します
 * @return きちんとシグナルされている場合のみTRUE 
 */
//-----------------------------------------------------------------------
bool Event::Wait(void)
{
  return mpEventAdapter->Wait();
}

void Event::Signal(void)
{
  mpEventAdapter->Signal();
}


void Event::Clear(void)
{
  mpEventAdapter->Clear();
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)


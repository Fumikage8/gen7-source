//=============================================================================
/**
 * @file    gfl_event_adapter_3ds.cpp
 * @brief   イベントの機種依存コード 3ds
 * @author  ohno_katsumi
 * @date    2014.05.13
 */
//=============================================================================


#include "../include/gfl2_Event.h"
#include "gfl_event_adapter_3ds.h"

#if defined(GF_PLATFORM_CTR)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)





EventAdapter3ds::EventAdapter3ds(void)
{
  mEvent.Initialize(true);
}

EventAdapter3ds::~EventAdapter3ds(void)
{
  mEvent.Finalize();
}


void EventAdapter3ds::Signal(void)
{
  mEvent.Signal();
}

//-----------------------------------------------------------------------
/**
 * @brief     フラグがセットされるまで待ちます。
 * @return     同期が取れたらTRUE
 */
//-----------------------------------------------------------------------


bool EventAdapter3ds::Wait(void)
{
  mEvent.Wait();
  return true;
}

//-----------------------------------------------------------------------
/**
 * @brief      フラグがセットされるか確認する
 * @return     同期が取れたらTRUE
 */
//-----------------------------------------------------------------------

bool EventAdapter3ds::TryWait(void)
{
  return mEvent.TryWait();
}

//-----------------------------------------------------------------------
/**
  * @brief フラグをリセットします
  */
//-----------------------------------------------------------------------
void EventAdapter3ds::Clear(void)
{
  mEvent.ClearSignal();
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif// defined(GF_PLATFORM_CTR)

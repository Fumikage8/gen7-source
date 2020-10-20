#ifndef __GFL_EVENT_ADAPTER_CTR_H__
#define __GFL_EVENT_ADAPTER_CTR_H__
//=============================================================================
/**
 * @file	 gfl_event_adapter_ctr.h
 * @brief	 完了したことを伝えるクラス ctrで
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once


#if defined(GF_PLATFORM_CTR)

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include "gfl_event_adapter.h"



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


class EventAdapter3ds : public EventAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(EventAdapter3ds);

private:

  static const int RET_TRUE = 1;

  nn::os::LightEvent mEvent;

  
public:
  EventAdapter3ds(void);
  virtual ~EventAdapter3ds(void);

  virtual void Signal(void);
  virtual bool Wait(void);
  virtual bool TryWait(void);
  virtual void Clear(void);

  
};





GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif //defined(GF_PLATFORM_CTR)


#endif//__GFL_EVENT_ADAPTER_CTR_H__

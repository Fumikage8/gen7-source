#ifndef __GFL_EVENT_ADAPTER_WIN_H__
#define __GFL_EVENT_ADAPTER_WIN_H__
//=============================================================================
/**
 * @file	 gfl_event_adapter_win.h
 * @brief	 完了したことを伝えるクラス std実装
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once


#if defined(GF_PLATFORM_WIN32)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include "gfl_event_adapter.h"



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


class EventAdapterWin : public EventAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(EventAdapterWin);

private:

  HANDLE m_handle;

public:
  EventAdapterWin(void);
  virtual ~EventAdapterWin(void);

  virtual void Signal(void);
  virtual bool Wait(void);
  virtual bool TryWait(void);
  virtual void Clear(void);
};





GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif //defined(GF_PLATFORM_WIN32)


#endif//__GFL_EVENT_ADAPTER_WIN_H__

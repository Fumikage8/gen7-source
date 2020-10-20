//=============================================================================
/**
 * @file    gfl_event_adapter_win.cpp
 * @brief   イベントの機種依存コード std 実装
 * @author  ohno_katsumi
 * @date    2014.05.13
 */
//=============================================================================


#include "../include/gfl2_Event.h"
#include "gfl_event_adapter_win.h"

#if defined(GF_PLATFORM_WIN32)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)





EventAdapterWin::EventAdapterWin(void)
{
  m_handle = CreateEvent( 0, true, false, 0 );
}

EventAdapterWin::~EventAdapterWin(void)
{
  ResetEvent( m_handle );
  CloseHandle( m_handle );
}

void EventAdapterWin::Signal(void)
{
  SetEvent( m_handle );
}

//-----------------------------------------------------------------------
/**
 * @brief     フラグがセットされるまで待ちます。
 * @return     同期が取れたらTRUE
 */
//-----------------------------------------------------------------------
bool EventAdapterWin::Wait(void)
{
  return WaitForSingleObject( m_handle, INFINITE ) == WAIT_OBJECT_0;
}

//-----------------------------------------------------------------------
/**
 * @brief      フラグがセットされるか確認する
 * @return     同期が取れたらTRUE
 */
//-----------------------------------------------------------------------

bool EventAdapterWin::TryWait(void)
{
  return WaitForSingleObject( m_handle, 0 ) == WAIT_OBJECT_0;
}

//-----------------------------------------------------------------------
/**
  * @brief フラグをリセットします
  */
//-----------------------------------------------------------------------
void EventAdapterWin::Clear(void)
{
  ResetEvent( m_handle );
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif// defined(GF_PLATFORM_WIN32)

//==============================================================================
/**
 @file    ResortDebugInfo.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.03.25
 @brief   ポケリゾートのデバッグに役立つ情報を扱うクラス
 */
//==============================================================================


#ifdef GF_PLATFORM_WIN32  // windows.hのmin/max対策
#define NOMINMAX
#endif


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <ui/include/gfl2_UI_DeviceManager.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>

// ポケリゾートのインクルード
#include "ResortDebugInfo.h"


#define  NIJI_PROJECT_APP_RESORT_DEBUG_INFO_PRINT_ENABLE  (0)  // 0でコミットして下さい


#if  NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)


// ポケリゾートのデバッグに役立つ情報を扱うクラス

ResortDebugInfo ResortDebugInfo::s_ResortDebugInfo;

ResortDebugInfo::ResortDebugInfo(void)
{
  STATIC_ASSERT(TIME_PHASE_RESORT_MAX <= TIME_NUM_MAX);
  STATIC_ASSERT(HEAP_DEF_RESORT_MAX <= HEAP_NUM_MAX);

  this->Init();
}
void ResortDebugInfo::Init(void)
{
  for(u32 i=0; i<TIME_NUM_MAX; ++i)
  {
    m_time[i] = 0;
  }
  for(u32 i=0; i<HEAP_NUM_MAX; ++i)
  {
    m_heap[i] = NULL;
    m_heapName[i] = NULL;
  }
  m_userVal = 0;
}
void ResortDebugInfo::SetCurrTime(u32 index)
{
  m_time[index] = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
}
void ResortDebugInfo::SetCurrTimeOnce(u32 index)
{
  if(m_time[index] == 0)
  {
    m_time[index] = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
  }
}
s64 ResortDebugInfo::GetTime(u32 index) const
{
  return m_time[index];
}
void ResortDebugInfo::SetHeap(u32 index, gfl2::heap::HeapBase* heap, const char* heapName)
{
  m_heap[index] = heap;
  m_heapName[index] = heapName;
}
void ResortDebugInfo::SetUserVal(s32 val)
{
  m_userVal = val;
}
s32  ResortDebugInfo::GetUserVal(void) const
{
  return m_userVal;
}
void ResortDebugInfo::PrintTime(void) const
{
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_PRINT_ENABLE
  GFL_PRINT("time[micro second]\n");
  for(u32 i=0; i<TIME_NUM_MAX; ++i)
  {
    GFL_PRINT("[%03d] %lld\n", i, m_time[i]);
  }
#endif
}
void ResortDebugInfo::ReleasePrintTime(void) const
{
  GFL_RELEASE_PRINT("time[micro second]\n");
  for(u32 i=0; i<TIME_NUM_MAX; ++i)
  {
    GFL_RELEASE_PRINT("[%03d] %lld\n", i, m_time[i]);
  }
}
void ResortDebugInfo::PrintHeap(void) const
{
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_PRINT_ENABLE
  GFL_PRINT("heap ================================\n");
  for(u32 i=0; i<HEAP_NUM_MAX; ++i)
  {
    const char* heapName = "";
    if(m_heapName[i]) heapName = m_heapName[i];
    if(m_heap[i])
    {
      GFL_PRINT("[%03d] %s %p\n", i, heapName, m_heap[i]);
      m_heap[i]->Dump();
    }
    else
    {
      GFL_PRINT("[%03d] %s NULL\n", i, heapName);
    }
  }
#endif
}
void ResortDebugInfo::Update(void)
{
  gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  const gfl2::ui::Button*  button = uiDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( button->IsTrigger(gfl2::ui::BUTTON_L) && button->IsTrigger(gfl2::ui::BUTTON_R) )
  {
    this->PrintTime();
    this->ReleasePrintTime();
    this->PrintHeap();
  }
}


GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif  // NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE


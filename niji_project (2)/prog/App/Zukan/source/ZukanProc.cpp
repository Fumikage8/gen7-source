//==============================================================================
/**
 @file    ZukanProc.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のProc
 */
//==============================================================================


// nijiのインクルード
#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProc.h>
#include "ZukanDefine.h"
#include "ZukanType.h"
#include "ZukanImpl.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
static bool s_debugInfoInitFlag = false;
static ZukanDebugInfo s_debugInfo;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE


ZukanProc::ZukanProc(ZukanProcParam* param)
  : GameSys::GameProc(),
    m_param(param),
    m_heapMem(NULL),
    m_devMem(NULL),
    m_appHeap(NULL),
    m_impl(NULL)
{
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
  s_debugInfoInitFlag = true;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
}

ZukanProc::~ZukanProc()
{}

gfl2::proc::Result ZukanProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
  if(s_debugInfoInitFlag)
  {
    s_debugInfo.Init();
    s_debugInfo.SetCurrTime(ZukanDebugInfo::TIME_PHASE_ZUKAN_PROC_INIT_FUNC_START);
    s_debugInfoInitFlag = false;
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE

  if(m_impl == NULL)
  {
    // メモリ
    if(m_param->caller == ZukanProcParam::CALLER_ZUKAN_EVENT)
    {
      gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_heapMem = GFL_CREATE_LOCAL_HEAP(parentHeapMem, ZUKAN_MEM_SIZE_PROC_HEAP_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
    
      gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_devMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, ZUKAN_MEM_SIZE_PROC_DEV_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
 
      m_appHeap = GFL_NEW(m_devMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_devMem, m_devMem, ZUKAN_MEM_SIZE_IMPL_HEAP_MEM, ZUKAN_MEM_SIZE_IMPL_DEV_MEM);  // m_heapMemではなくm_devMemからヒープメモリを確保することにした。
                                                                                                                  // デバイスメモリを使ってはいけないところがあるときにはこれはダメ。
    }
    else
    {
      gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_heapMem = GFL_CREATE_LOCAL_HEAP(parentHeapMem, ZUKAN_REGISTER_MEM_SIZE_PROC_HEAP_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
    
      gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_devMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, ZUKAN_REGISTER_MEM_SIZE_PROC_DEV_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
 
      m_appHeap = GFL_NEW(m_devMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_devMem, m_devMem, ZUKAN_REGISTER_MEM_SIZE_IMPL_HEAP_MEM, ZUKAN_REGISTER_MEM_SIZE_IMPL_DEV_MEM);  // m_heapMemではなくm_devMemからヒープメモリを確保することにした。
    }

    // 「図鑑を残したまま起動するもので使用するメモリ」が確保できるかここで確認しておく
    {
      // StrInputのメモリ  // StrInputFrame  // niji_project/prog/App/StrInput/source/StrInputFrame.cpp
      if(m_param->caller != ZukanProcParam::CALLER_ZUKAN_EVENT)
      {
        gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
#if PM_DEBUG
        // ZukanImplでZUKAN_MEM_SIZE_DEBUG_DEV_MEMだけ確保しているので、ここで一旦確保する。
        gfl2::heap::HeapBase* debugDevMem = GFL_CREATE_LOCAL_HEAP(parentHeapMem, ZUKAN_MEM_SIZE_DEBUG_DEV_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
#endif
        gfl2::heap::HeapBase* tempMem = GFL_CREATE_LOCAL_HEAP(parentHeapMem, ZUKAN_MEM_SIZE_STR_INPUT_DEV_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
        GFL_DELETE_HEAP(tempMem);
#if PM_DEBUG
        GFL_DELETE_HEAP(debugDevMem);
#endif
      }
    }

    void* debugInfo = NULL;
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
    debugInfo = &s_debugInfo;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
    m_impl = GFL_NEW(m_devMem) ZukanImpl(m_param, m_appHeap, this->GetGameManager(), debugInfo);  // 親であるGameSys::GameProcのメンバ
  }

  return m_impl->InitFunc();
}
gfl2::proc::Result ZukanProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  return m_impl->UpdateFunc();
}
void ZukanProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
  m_impl->PreDrawFunc();
}
void ZukanProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_impl->DrawFunc(displayNo);
}
gfl2::proc::Result ZukanProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if(m_impl != NULL)
  {
    gfl2::proc::Result res = m_impl->EndFunc();
    if(res == gfl2::proc::RES_FINISH)
    {
      GFL_SAFE_DELETE(m_impl);
    }
    else
    {
      return res;
    }
  }

  // メモリ
  {
    GFL_SAFE_DELETE(m_appHeap);

    GFL_DELETE_HEAP(m_devMem);
    m_devMem = NULL;
    GFL_DELETE_HEAP(m_heapMem);
    m_heapMem = NULL;
  }

  return gfl2::proc::RES_FINISH;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


//======================================================================
/**
 * @file CharaViewerProc.cpp
 * @date 2015/07/21 20:57:02
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

// nijiのインクルード
#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

// 図鑑のインクルード
#include <Test/CharaViewer/include/CharaViewerProc.h>
#include "../include/CharaViewerImpl.h"
#include "../include/CharaViewerDefine.h"


GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

CharaViewerProc::CharaViewerProc(void)
  : GameSys::GameProc(),
    m_param(),
    m_heapMem(NULL),
    m_devMem(NULL),
    m_appHeap(NULL),
    m_impl(NULL)
{
}

//CharaViewerProc::CharaViewerProc(CharaViewerProcParam* param)
//  : GameSys::GameProc(),
//    m_param(),
//    m_heapMem(NULL),
//    m_devMem(NULL),
//    m_appHeap(NULL),
//    m_impl(NULL)
//{
//  m_param = *param;
//}

CharaViewerProc::~CharaViewerProc()
{}

gfl2::proc::Result CharaViewerProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if(m_impl == NULL)
  {
    // メモリ
    {
      gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_heapMem = GFL_CREATE_LOCAL_HEAP(parentHeapMem, CHARA_VIEWER_MEM_PROC_HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
    
      gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_devMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, CHARA_VIEWER_MEM_PROC_DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
 
      m_appHeap = GFL_NEW(m_devMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_devMem, m_devMem, CHARA_VIEWER_MEM_IMPL_HEAP_MEM_SIZE, CHARA_VIEWER_MEM_IMPL_DEV_MEM_SIZE);  // デバイスメモリを使ってはいけないところがあるときにはこれはダメ。
    }
  
    m_impl = GFL_NEW(m_devMem) CharaViewerImpl(&m_param, m_appHeap, GetGameManager());
  }

  return m_impl->InitFunc();
}

gfl2::proc::Result CharaViewerProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  return m_impl->UpdateFunc();
}
void CharaViewerProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
  m_impl->PreDrawFunc();
}
void CharaViewerProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_impl->DrawFunc(displayNo);
}
gfl2::proc::Result CharaViewerProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
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


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG

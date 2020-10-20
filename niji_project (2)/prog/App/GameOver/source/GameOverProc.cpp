//==============================================================================
/**
 @file    GameOverProc.cpp
 @author  takeda nobutaka
 @date    2015.06.10
 @brief   ゲームオーバー　図鑑からコピー
 */
//==============================================================================


// nijiのインクルード
#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <App/GameOver/include/GameOverProc.h>
#include "GameOverDefine.h"
#include "GameOverImpl.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


GameOverProc::GameOverProc(GameOverProcParam* param)
  : GameSys::GameProc(),
    m_param(),
    m_devMem(NULL),
    m_appHeap(NULL),
    m_impl(NULL)
{
  m_param = *param;
}
GameOverProc::~GameOverProc()
{}

gfl2::proc::Result GameOverProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if(m_impl == NULL)
  {
    // メモリ
    {
      gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_devMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, GAMEOVER_MEM_PROC_DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
 
      m_appHeap = GFL_NEW(m_devMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_devMem, m_devMem, GAMEOVER_MEM_IMPL_HEAP_MEM_SIZE, GAMEOVER_MEM_IMPL_DEV_MEM_SIZE);
    }
  
    m_impl = GFL_NEW(m_devMem) GameOverImpl(&m_param, m_appHeap, GetGameManager());
  }

  return m_impl->InitFunc();
}
gfl2::proc::Result GameOverProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  return m_impl->UpdateFunc();
}
void GameOverProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if( m_impl )  m_impl->PreDrawFunc();
}
void GameOverProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_impl )  m_impl->DrawFunc(displayNo);
}
gfl2::proc::Result GameOverProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
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
  }
  return gfl2::proc::RES_FINISH;
}


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)


//==============================================================================
/**
 @file    ZukanSearchThread.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.08
 @brief   図鑑の検索のスレッド
 */
//==============================================================================


// gfl2のインクルード
#include <thread/include/gfl2_thread.h>
#include <thread/include/gfl2_threadstatic.h>
#include <thread/include/gfl2_Event.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanViewSearch.h"

#include "ZukanSearchThread.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


ZukanSearchThread::ZukanSearchThread(app::util::Heap* appHeap, ZukanViewSearch* viewSearch, int threadPrio)
  : m_thread(NULL),
    m_endReq(false),
    m_hasEnded(false),
    m_viewSearch(viewSearch)
{
  m_thread = GFL_NEW(appHeap->GetDeviceHeap()) gfl2::thread::Thread(this, appHeap->GetDeviceHeap(), false, DEFAULT_STACK_SIZE );
  m_thread->Start(threadPrio);
}
ZukanSearchThread::~ZukanSearchThread()
{
  this->RequestEnd();
  u32 endlessLoopCount = 0;
  while(!(this->HasEnded()))
  {
    gfl2::thread::ThreadStatic::CurrentSleep(1);

    if(++endlessLoopCount >= ZUKAN_CONST_ENDLESS_LOOP_COUNT_MAX_LONG)
    {
      GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
      break;
    }
  }
  GFL_SAFE_DELETE(m_thread);

  // Functionが終了しているのでKillやIsAliveは使っていません。もしFunctionを終了させずに破棄する場合は
  // m_thread->Kill();
  // while(m_thread->IsAlive())
  // {
  //   gfl2::thread::ThreadStatic::CurrentSleep(1);
  // }
  // GFL_SAFE_DELETE(m_thread);
  // となる。
}

void ZukanSearchThread::Function(void)
{
  for ever
  {
    // リクエストが追加されるまで待つ
    m_viewSearch->GetSearchEvent()->Wait();
    m_viewSearch->GetSearchEvent()->Clear();

    // 終了リクエストか検索リクエストか
    if(m_endReq)
    {
      // 終了
      break;
    }
    else
    {
      // 検索
      m_viewSearch->SearchFunction();
    }
  }

  m_hasEnded = true;
}

void ZukanSearchThread::RequestEnd(void)
{
  m_endReq = true;
}
bool ZukanSearchThread::HasEnded(void) const
{
  return m_hasEnded;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


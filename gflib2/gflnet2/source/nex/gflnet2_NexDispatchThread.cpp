#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_NexDispatchThread.cpp
 * @brief  Wifi接続・切断用用スレッド
 * @author h.suzuki
 */
//=============================================================================


#include "gflnet2_NexDispatchThread.h"


namespace gflnet2 {
namespace nex {

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param nexMgr      NexManagerへのポインタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 */
//--------------------------------------------------------------------------------
NexDispatchThread::NexDispatchThread( NexManager* nexMgr, gfl2::heap::HeapBase* heap, u32 stack_size) :
  m_nexMgr(nexMgr), m_isLoop(true),m_pThread(NULL)
{
  m_criticalSection.Initialize();

  m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true, stack_size );
}


//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
NexDispatchThread::~NexDispatchThread()
{
  GFL_SAFE_DELETE( m_pThread );
  m_criticalSection.Finalize();
}





//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理
 */
//--------------------------------------------------------------------------------
void NexDispatchThread::Function( void )
{
  while( IsLoop() )
  {
    if( m_nexMgr != NULL )
    {
      m_nexMgr->Dispatch(gflnet2::nex::NexManager::D_DISPATCH_TIMEOUT, false); //DispatchAllしない
      m_nexMgr->CallDispatchThreadEventListener();                            //ポーリング用イベント
    }

    gfl2::thread::ThreadStatic::CurrentSleep( 100 );
  }
  m_pThread->Kill();
}


//--------------------------------------------------------------------------------
/**
 * @brief 処理継続フラグの取得
 *
 * @return 処理を継続するならtrueを返却する
 */
//--------------------------------------------------------------------------------
bool NexDispatchThread::IsLoop( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  return m_isLoop;
}


//--------------------------------------------------------------------------------
/**
 * @brief Dispatch処理の停止
 */
//--------------------------------------------------------------------------------
void NexDispatchThread::StopDispatch( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  m_isLoop = false;
}


} // nex
} // gflnet


#endif

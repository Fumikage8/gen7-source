#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_FriendLoginThread.cpp
 * @brief  フレンドサーバログイン通知用スレッド
 * @author h.suzuki
 * @date   2012.2.15
 */
//=============================================================================


#include "gflnet2_FriendLoginThread.h"


namespace gflnet2 {
namespace friends {

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 * @param filter      フィルター用クラスへのポインタ
 * @param timeout     タイムアウト時間
 */
//--------------------------------------------------------------------------------
FriendLoginThread::FriendLoginThread( gfl2::heap::HeapBase* heap, u32 stack_size, FriendLoginListener* listener, nn::os::Event* event, nn::fnd::TimeSpan* timeout ) :
  m_event(event),  m_listener(listener), m_timeout(timeout), m_isCancel(false), m_pThread(NULL)
{
  m_criticalSection.Initialize();

  m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true, stack_size);
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
FriendLoginThread::~FriendLoginThread()
{
  GFL_SAFE_DELETE( m_pThread );

  m_criticalSection.Finalize();
}

//--------------------------------------------------------------------------------
/**
  * @brief 開始
  */
//--------------------------------------------------------------------------------
void FriendLoginThread::Start( void )
{
  m_pThread->Start();
}

//--------------------------------------------------------------------------------
/**
 * @brief キャンセル要求
 */
//--------------------------------------------------------------------------------
void FriendLoginThread::Cancel( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  m_listener = NULL;
  m_isCancel = true;
}


//--------------------------------------------------------------------------------
/**
 * @brief ログイン終了リスナをコールする
 */
//--------------------------------------------------------------------------------
void FriendLoginThread::CallFinishListener( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( !m_isCancel )
  {
    if( m_listener != NULL ){
      m_listener->OnFriendServerLoginFinish( nn::friends::CTR::GetLastResponseResult() );
    }
  }
}


//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理
 */
//--------------------------------------------------------------------------------
void FriendLoginThread::Function( void )
{
  if( m_event != NULL )
  {
    while( !m_isCancel )
    {
      if( m_event->Wait( 0 ) ){
        break;
      }
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(50) );
    }
  }
  CallFinishListener();

  m_pThread->Kill();
}



} // friend
} // gflnet


#endif

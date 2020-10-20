//=============================================================================
/**
 * @file gfl_Thread.cpp
 * @brief スレッド
 * @author obata_toshihiro
 * @date 2010.11.09
 */
//=============================================================================
#include <nn.h>
#include <gfl_Debug.h>
#include <base/gfl_Thread.h>
#include "gfl_SimpleStackArea.h"
#include "gfl_SafeStackArea.h"

namespace gfl {
  namespace base {


    u32 Thread::s_runningThreadCount = 0;
    Thread* Thread::s_runningThread[ MAX_RUNNING_THREAD_COUNT ];
    nn::os::CriticalSection* Thread::s_criticalSectionForThreadManagement = NULL;

    //-------------------------------------------------------------------------
    /**
     * @brief スレッド管理情報の初期化
     * @param heap スレッドの管理領域として使用するヒープ
     */
    //-------------------------------------------------------------------------
    void Thread::InitializeThreadManagement( gfl::heap::HeapBase* heap )
    {
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ ) {
        s_runningThread[i] = NULL;
      }
      s_criticalSectionForThreadManagement = GFL_NEW( heap ) nn::os::CriticalSection();
      s_criticalSectionForThreadManagement->Initialize();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッド管理の後片付け
     *
     * @note
     * 管理対象のスレッドが生き残っている場合,
     * 強制的に kill し, スレッドの終了を待ちます.
     */
    //-------------------------------------------------------------------------
    void Thread::FinalizeThreadManagement( void )
    {
      // スレッドの終了を待つ
      while( 0 < s_runningThreadCount )
      {
        GFL_PRINT( "WARNING!! スレッドが%d個生きています。"\
            "スレッドの終了を待ちます。\n", s_runningThreadCount );
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
      }

      // 後片付け
      s_criticalSectionForThreadManagement->Finalize();
      GFL_DELETE s_criticalSectionForThreadManagement;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 動作中のスレッドの数を取得する
     * @return 動作が完了していない, gfl::base::Threadインスタンスの数
     *
     * @attention
     * メインスレッドやサウンドスレッドなど
     * システムが立てたスレッドの数は含まれません。
     */
    //-------------------------------------------------------------------------
    u32 Thread::GetValidUserThreadCount( void )
    {
      return s_runningThreadCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 同じ優先度の他スレッドに実行機会を与える
     */
    //-------------------------------------------------------------------------
    void Thread::Yield( void )
    {
      nn::os::Thread::Yield();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief カレントスレッドを指定時間停止させる
     *
     * @param milliSeconds スレッドを停止させる時間（ミリ秒）
     */
    //-------------------------------------------------------------------------
    void Thread::Sleep( s64 milliSeconds )
    {
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(milliSeconds) );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 全ての動作中のスレッドを kill する
     *
     * @note
     * ソフトウェアリセットなど, 大本から一括制御する目的で作成。
     * 本来は, スレッドの所有者であるオブジェクトが正しくスレッドを kill するべき。
     */
    //-------------------------------------------------------------------------
    void Thread::KillAllRunningThread( void )
    {
      // この関数実行中に, 他のスレッドにより
      // スレッドの管理情報が書き換わることを防ぐためクリティカルセクションに入る.
      nn::os::CriticalSection::ScopedLock lock( *s_criticalSectionForThreadManagement );

      // 全てのスレッドを殺す
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ ) {
        if( s_runningThread[i] ) {
          s_runningThread[i]->Kill();
        }
      }
    }

    /**
     * @brief スレッドIDがメインスレッドかチェック
     */
    bool Thread::IsMainThread( bit32 thread_id )
    {
      const nn::os::Thread& thread = nn::os::Thread::GetMainThread();
      if( thread_id == thread.GetId() ){
        return true;
      }
      return false;
    }

    /**
     * @brief スレッドを管理対象として追加する
     * @param thread 新たに管理するスレッド
     */
    void Thread::AddManagedThread( Thread* thread )
    {
      GFL_ASSERT_MSG( ( s_runningThreadCount < MAX_RUNNING_THREAD_COUNT ),
          "これ以上スレッドを管理できません。スレッドの同時起動数を調整してください。\n" ); //@check

      // この関数実行中に, 他のスレッドにより
      // スレッドの管理情報が書き換わることを防ぐためクリティカルセクションに入る.
      nn::os::CriticalSection::ScopedLock lock( *s_criticalSectionForThreadManagement );

      // 空いている場所に管理スレッドを追加
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ )
      {
        if( s_runningThread[i] == NULL ) {
          s_runningThread[i] = thread;
          s_runningThreadCount++;
          break;
        }
      }
    }

    /**
     * @brief スレッドを管理対象から除外する
     * @param thread 除外するスレッド
     */
    void Thread::RemoveManagedThread( Thread* thread )
    {
      GFL_ASSERT_MSG( ( 0 < s_runningThreadCount ), "想定外のエラーです。by obata\n" ); //@check

      // この関数実行中に, 他のスレッドにより
      // スレッドの管理情報が書き換わることを防ぐためクリティカルセクションに入る.
      nn::os::CriticalSection::ScopedLock lock( *s_criticalSectionForThreadManagement );

      // スレッドを管理から除外
      for( u8 i=0; i<MAX_RUNNING_THREAD_COUNT; i++ )
      {
        if( s_runningThread[i] == thread ) {
          s_runningThread[i] = NULL;
          s_runningThreadCount--;
          break;
        }
      }
    }



    //=========================================================================
    /**
     * @brief スレッド制御関数
     */
    //=========================================================================
    void ThreadFunc( gfl::base::Thread* thread )
    {
      Thread::AddManagedThread( thread );
      thread->Initialize();

      while( !thread->m_killFlag )
      {
        thread->Main();

        if( thread->m_pauseRequest )
        {
          thread->m_isPausing = true;
          thread->m_pauseEvent.Wait();

          thread->m_isPausing = false;
          thread->m_pauseRequest = false;
        }
      }

      thread->Finalize();
      Thread::RemoveManagedThread( thread );
    }



    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap       スタック領域に使用するヒープ
     * @param stackSize  スタック領域のサイズ
     * @param stackType  スタック領域のタイプ
     */
    //-------------------------------------------------------------------------
    Thread::Thread( gfl::heap::HeapBase* heap, u32 stackSize, StackType stackType ) :
      m_nnosThread(),
      m_simpleStackArea( NULL ),
      m_safeStackArea( NULL ),
      m_pauseEvent(),
      m_waitEvent(),
      m_isPausing( false ),
      m_pauseRequest( false ),
      m_killFlag( false )
    {

      switch( stackType )
      {
      case STACK_TYPE_SIMPLE:
        m_simpleStackArea = GFL_NEW( heap ) SimpleStackArea( heap, stackSize );
        break;

      case STACK_TYPE_SAFE:
        m_safeStackArea = GFL_NEW( heap ) SafeStackArea( heap, stackSize );
        break;

      default:
        GFL_ASSERT_STOP(0); //@check
        break;
      }

      m_pauseEvent.Initialize( false );
      m_waitEvent.Initialize( false );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    Thread::~Thread()
    {
      if( m_nnosThread.IsValid() ) {
        this->Kill();
        this->PauseOff();
        m_nnosThread.Join();
        m_nnosThread.Finalize();
      }
      GFL_SAFE_DELETE( m_simpleStackArea );
      GFL_SAFE_DELETE( m_safeStackArea );
      m_pauseEvent.Finalize();
      m_waitEvent.Finalize();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッドを開始する
     * @param Priority priority スレッドの優先度
     */
    //-------------------------------------------------------------------------
    void Thread::Start( Priority priority )
    {
      if( m_nnosThread.IsValid() ) {
        GFL_ASSERT(0); //@fix Start 済み
        return;
      }

      if( m_killFlag ) {
        GFL_ASSERT(0); //@fix Kill 済み
        return;
      }

      priority = gfl::math::Max( priority, HighestPriority );
      priority = gfl::math::Min( priority, LowestPriority );

      if( m_safeStackArea )
      {
        m_nnosThread.Start( ThreadFunc, this, *m_safeStackArea, priority );
      }
      else if( m_simpleStackArea )
      {
        m_nnosThread.Start( ThreadFunc, this, *m_simpleStackArea, priority );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッドを一時停止させる
     */
    //-------------------------------------------------------------------------
    void Thread::PauseOn( void )
    {
      m_pauseRequest = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッドの一時停止を解除する
     */
    //-------------------------------------------------------------------------
    void Thread::PauseOff( void )
    {
      if( this->m_isPausing )
      {
        m_pauseEvent.Signal();
      }
    }

    //-----------------------------------------------------------------------
    /**
     * @brief スレッドが一時停止中かどうかを調べる
     * @retval true   一時停止中
     * @retval false  動作中
     */
    //-----------------------------------------------------------------------
    bool Thread::IsPausing( void ) const
    {
      return m_isPausing;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッドを終了させる
     */
    //-------------------------------------------------------------------------
    void Thread::Kill( void )
    {
      if( !m_killFlag ) {
        this->OnKill();
        this->PauseOff();
        m_killFlag = true;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッドが生きているかどうかを調べる
     * @retval true  生きている
     * @retval false 死んでいる
     */
    //-------------------------------------------------------------------------
    bool Thread::IsAlive( void ) const
    {
      if( !m_nnosThread.IsValid() ) { // 開始されていない
        return false;
      }
      return m_nnosThread.IsAlive();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 自身を休止状態にする
     */
    //-------------------------------------------------------------------------
    void Thread::Wait( void )
    {
      m_waitEvent.Wait();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief Wait()による休止状態を解除する
     */
    //-------------------------------------------------------------------------
    void Thread::Signal( void )
    {
      m_waitEvent.Signal();
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 待ち状態か？
     * @retval true  待ち状態
     * @retval false 待ち状態でない
     */
    //-----------------------------------------------------------------------
    bool Thread::IsWaiting( void )
    {
      return m_waitEvent.TryWait();
    }



  } // namespace base
} // namespace gfl

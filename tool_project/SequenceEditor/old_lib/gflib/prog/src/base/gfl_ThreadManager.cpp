//=============================================================================
/**
 * @brief スレッド管理
 * @file gfl_ThreadManager.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include <base/gfl_ThreadManager.h>

namespace gfl {
  namespace base {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_thread_management スレッドの管理領域として使用するヒープ
     * @param max_running_thread_count   同時に起動できるスレッドの数
     * @param max_waiting_request_count  登録できるリクエストの最大数
     */
    //-------------------------------------------------------------------------
    ThreadManager::ThreadManager( 
        gfl::heap::HeapBase* heap_for_thread_management,
        u32 max_running_thread_count,
        u32 max_waiting_request_count ) :
      m_heap( heap_for_thread_management ),
      m_waitingRequest( NULL ),
      m_waitingRequestCount( 0 ),
      m_maxWaitingRequestCount( max_waiting_request_count ),
      m_runningRequest( NULL ),
      m_runningRequestCount( 0 ),
      m_maxRunningRequestCount( max_running_thread_count )
    {
      m_waitingRequest = GFL_NEW_ARRAY( m_heap ) Request[ m_maxWaitingRequestCount ];
      m_runningRequest = GFL_NEW_ARRAY( m_heap ) Request[ m_maxRunningRequestCount ];
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     *
     * @attention
     * カレントスレッドを休止することで, 他スレッドの完了を待ちます.
     * IsAllRequestFinished() で全てのリクエストが完了してから破棄する方が望ましいです.
     */
    //-------------------------------------------------------------------------
    ThreadManager::~ThreadManager()
    {
      // 実行中のリクエストの終了を待つ
      this->WaitAllRunningRequest();

      // 実行されなかったリクエストを破棄
      this->CancelAllWaitingRequest();

      GFL_ASSERT_MSG( ( this->GetRunningRequestCount() == 0 ), "想定外のエラーです。小幡までご連絡ください。\n" ); //@check
      GFL_ASSERT_MSG( ( this->GetWaitingRequestCount() == 0 ), "想定外のエラーです。小幡までご連絡ください。\n" ); //@check

      GFL_DELETE_ARRAY m_waitingRequest;
      GFL_DELETE_ARRAY m_runningRequest;
    }

    /**
     * @brief 実行中の全リクエストの完了を待つ
     */
    void ThreadManager::WaitAllRunningRequest( void )
    {
      while( 0 < this->GetRunningRequestCount() ) 
      {
        GFL_PRINT( "WARNING!! 実行中のスレッドが残っています！スレッドの終了を待ちます。\n" );
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) ); 
        this->RemoveDeadThread(); 
      }
    }

    /**
     * @brief 待機中の全リクエストを破棄する
     */
    void ThreadManager::CancelAllWaitingRequest( void )
    {
      while( 0 < this->GetWaitingRequestCount() )
      {
        GFL_PRINT( "WARNING!! 待機中のリクエストを破棄します。\n" );

        u32 req_index = this->GetWaitingRequestCount() - 1;
        this->CancelRequest( &m_waitingRequest[ req_index ] );
        this->RemoveWaitingRequestByIndex( req_index );
      }
    }

    /**
     * @brief 指定したリクエストをキャンセルする
     * @param request キャンセル対象のリクエスト
     */
    void ThreadManager::CancelRequest( const Request* request )
    {
      if( request->callbackTarget ) {
        request->callbackTarget->OnRequestCanceled( *request );
      }
    }

    /**
     * @brief 指定した待機中のスレッドを管理から取り除く
     * @param req_index 取り除くリクエストのインデックス
     */
    void ThreadManager::RemoveWaitingRequestByIndex( u32 req_index )
    {
      for( u32 i=req_index; i<(m_waitingRequestCount-1); i++ ) {
        m_waitingRequest[i] = m_waitingRequest[i+1];
      }
      --m_waitingRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッド管理処理
     * 実行中のスレッドを監視し, 終了したスレッドを管理から除外する.
     * 実行条件が整ったスレッドを起動する.
     */
    //-------------------------------------------------------------------------
    void ThreadManager::UpdateThreadManagement( void )
    {
      // 終了したスレッドを除外
      this->RemoveDeadThread(); 

      // 新しいスレッドを起動
      if( this->IsNewThreadLaunchEnable() ) {
        this->StartHighestPriorityRequest();
      }
    }

    /**
     * @brief 実行中のスレッドの内, 動作が終了したスレッドを管理から取り除く
     */
    void ThreadManager::RemoveDeadThread( void )
    {
      for( u32 i=0; i<m_runningRequestCount; i++ ) 
      {
        if( m_runningRequest[i].thread->IsAlive() == false ) 
        {
#if GFLTHREADMAN_TICK_ENABLE
          // チック計測を終了
          this->EndTick( &m_runningRequest[i] );
#endif
          // コールバック
          if( m_runningRequest[i].callbackTarget ) {
            m_runningRequest[i].callbackTarget->OnRequestFinished( m_runningRequest[i] );
          }

          // 管理から除外
          this->RemoveRunningRequestByIndex( i ); 
        }
      }
    }

    /**
     * @brief 指定した実行中のスレッドを管理から取り除く
     * @param req_index 取り除くリクエストのインデックス
     */
    void ThreadManager::RemoveRunningRequestByIndex( u32 req_index )
    {
      for( u32 i=req_index; i<(m_runningRequestCount-1); i++ ) {
        m_runningRequest[i] = m_runningRequest[i+1];
      }
      --m_runningRequestCount;
    }

#if GFLTHREADMAN_TICK_ENABLE

    /**
     * @brief チックの計測を終了する
     * @param request 計測を終了するリクエスト
     */
    void ThreadManager::EndTick( Request* request )
    {
      request->endTick = this->GetCurrentTick();
    }

    /**
     * @brief 現在のチック値を取得する
     */
    s64 ThreadManager::GetCurrentTick( void ) const
    {
      return nn::os::Tick::GetSystemCurrent();
    }

#endif

    /**
     * @brief 新しいスレッドを起動できるかどうか？
     * @retval true  新しいスレッドを起動できる
     * @retval false 新しいスレッドを起動できない
     */
    bool ThreadManager::IsNewThreadLaunchEnable( void ) const
    {
      u32 wait = this->GetWaitingRequestCount();
      u32 run = this->GetRunningRequestCount();
      u32 max_run = this->GetMaxRunningRequestCount();
      return ( ( 0 < wait ) && ( run < max_run ) );
    }

    /**
     * @brief 最も高い優先度のリクエスト処理を開始する
     */
    void ThreadManager::StartHighestPriorityRequest( void )
    {
      Request& req = m_waitingRequest[0]; 
#if GFLTHREADMAN_TICK_ENABLE
      this->StartTick( &req );
#endif 
      this->RegisterRunningRequest( req );
      this->StartRequest( req );
      this->RemoveWaitingRequestByIndex( 0 ); 
    }

#if GFLTHREADMAN_TICK_ENABLE
    /**
     * @brief チックの計測を開始する
     * @param request チック計測を開始するリクエスト
     */
    void ThreadManager::StartTick( Request* request )
    {
      request->startTick = this->GetCurrentTick();
    }
#endif 

    /**
     * @brief 指定したリクエストを実行中のリクエストとして管理対象に登録する
     * @param req 登録するリクエスト
     */
    void ThreadManager::RegisterRunningRequest( const Request& req )
    {
      if( m_maxRunningRequestCount <= m_runningRequestCount ) {
        GFL_ASSERT(0); //@fix
        return;
      }
      m_runningRequest[ m_runningRequestCount ] = req;
      ++m_runningRequestCount;
    }

    /**
     * @brief 指定したリクエストに対する処理を開始する
     * @param req 処理を開始するリクエスト
     */
    void ThreadManager::StartRequest( const Request& req )
    {
      req.thread->Start( req.threadPriority );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッド起動リクエストを登録する
     * @param req  登録するリクエスト
     * @retval true  登録できた
     * @retval false 登録できなかった
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::AddThreadRequest( const Request& req )
    {
      if( m_waitingRequestCount < m_maxWaitingRequestCount ) 
      { 
        // 待機優先度の降順になるように適切な位置に挿入する
        u32 insert_pos = 0;
        for( insert_pos=0; insert_pos<m_waitingRequestCount; insert_pos++ ) {
          if( req.requestPriority < m_waitingRequest[ insert_pos ].requestPriority ) {
            break;
          }
        }
        for( s32 i=m_waitingRequestCount; insert_pos<i; i-- ) {
          m_waitingRequest[i] = m_waitingRequest[i-1];
        }
        m_waitingRequest[ insert_pos ] = req;
        ++m_waitingRequestCount;
        return true;
      }
      else
      {
        GFL_PRINT( "WARNING!! gfl::base::ThreadManager 登録可能な最大数をオーバーしています\n" );
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 待機中のリクエストの内, 指定したIDのリクエストを削除する
     * @param requestID 削除するリクエストのID
     */
    //-------------------------------------------------------------------------
    void ThreadManager::CancelWaitingThreadRequest( u32 requestID )
    {
      for( s32 i=m_waitingRequestCount-1; 0<=i; i-- )
      {
        if( m_waitingRequest[i].ID == requestID ) {
          this->CancelRequest( &m_waitingRequest[i] );
          this->RemoveWaitingRequestByIndex(i);
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのリクエストの実行が終了したか？
     * @retval true  全てのスレッドが終了した
     * @retval false 終了していないスレッドが存在する
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsAllRequestFinished( void ) const
    {
      u32 wait = this->GetWaitingRequestCount();
      u32 run = this->GetRunningRequestCount();
      return ( ( wait == 0 ) && ( run == 0 ) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したIDのリクエストが終了したか？
     * @param requestID チェックするリクエストのID
     * @retval true  終了した
     * @retval false まだ終了していない
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsRequestFinished( u32 requestID ) const
    {
      return ( 
          !this->IsRequestRunning( requestID ) && 
          !this->IsRequestWaiting( requestID ) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したIDのリクエストが実行中か？
     * @param requestID チェックするリクエストのID
     * @retval true  実行中
     * @retval false 待機中 or リクエストが存在しない
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsRequestRunning( u32 requestID ) const
    {
      u32 req_count = this->GetRunningRequestCount();
      for( u32 i=0; i<req_count; i++ ) {
        if( m_runningRequest[i].ID == requestID ) {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したIDのリクエストが待機中か？
     * @retval true  待機中
     * @retval false 実行中 or リクエストが存在しない
     */
    //-------------------------------------------------------------------------
    bool ThreadManager::IsRequestWaiting( u32 requestID ) const
    {
      u32 req_count = this->GetWaitingRequestCount();
      for( u32 i=0; i<req_count; i++ ) {
        if( m_waitingRequest[i].ID == requestID ) {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実行中のリクエストの数を取得する
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetRunningRequestCount( void ) const
    {
      return m_runningRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 同時に起動できるリクエストの数を取得する
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetMaxRunningRequestCount( void ) const
    {
      return m_maxRunningRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 同時に保持できるリクエストの数を取得する
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetMaxWaitingRequestCount( void ) const
    {
      return m_maxWaitingRequestCount;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 起動待ちのリクエストの数を取得する
     */
    //-------------------------------------------------------------------------
    u32 ThreadManager::GetWaitingRequestCount( void ) const
    {
      return m_waitingRequestCount;
    }



  }  //namespace base
}  //namespace gfl

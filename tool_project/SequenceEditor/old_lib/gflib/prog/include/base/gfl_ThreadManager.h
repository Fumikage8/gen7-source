//=============================================================================
/**
 * @brief スレッド管理
 * @file gfl_ThreadManager.h
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#ifndef __GFL_THREADMANAGER_H__
#define __GFL_THREADMANAGER_H__
#pragma once

// チック計測を行うか？
#define GFLTHREADMAN_TICK_ENABLE (0)


namespace gfl {
  namespace base {

    class Thread; 
    class IThreadManageCallback;

    /**
     * @brief スレッド管理クラス
     * 設定されたスレッドの同時実行数を超えないよう,
     * 受け付けたリクエストのうち最も優先度の高いスレッドから順に実行します.
     * なお, 3DSのプラットフォーム上ではスレッドの最大同時起動数は
     * ( SDK, NintendoWare が利用するスレッドを含めて )32です.
     */
    class ThreadManager
    {
      GFL_FORBID_COPY_AND_ASSIGN( ThreadManager );

      public:

      /**
       * @brief スレッド起動リクエスト
       *
       * @note
       * 実行したいスレッドを thread にセットしてください.
       * ThreadManager は, requestPriority の値が小さいスレッドから順に実行します.
       * requestPriority のが同じ場合は登録した順に実行します.
       *
       * スレッドが終了した際や, スレッドがキャンセルされた際など,
       * スレッドを管理対象から除外するタイミングで callbackTarget に対して通知を行います.
       * それ以降, ThreadManager では対象スレッドに対する操作を一切行いませんので,
       * callbackTarget 側でスレッドの破棄などの終了処理を行ってください.
       * ( ThreadManager 側では thread の破棄を行いません )
       */
      class Request 
      {
        public:
          Thread* thread;                        // 登録するスレッド
          Thread::Priority threadPriority;       // スレッドの優先度( 0 ～ 31 ) ※0が最も高い優先度
          u8 requestPriority;                    // リクエストの優先度( 0 ～ 31 ) ※0が最も高い優先度
          u32 ID;                                // リクエスト識別ID
          IThreadManageCallback* callbackTarget; // コールバックターゲット( 不要なら NULL ) 
          void* pUserWork;                       // 任意のワーク

          Request( void ) : 
            thread( NULL ),
            threadPriority( gfl::base::Thread::DefaultPriority ),
            requestPriority( 16 ),
            ID( 0 ),
            callbackTarget( NULL ),
            pUserWork( NULL )
        {
#if GFLTHREADMAN_TICK_ENABLE
          startTick = 0;
          endTick = 0;
#endif
        }

#if GFLTHREADMAN_TICK_ENABLE
        public:
          s64 startTick;  // 開始時のチック
          s64 endTick;    // 終了時のチック

        public:
          /**
           * @brief スレッドの開始から終了までに掛かったチック数を取得する
           */
          s64 GetRunningTick( void ) const { return endTick - startTick; }
#endif
      };


      //-----------------------------------------------------------------------
      // 基本動作
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap_for_thread_management スレッドの管理領域として使用するヒープ
       * @param max_running_thread_count   同時に起動できるスレッドの数
       * @param max_waiting_request_count  登録できるリクエストの最大数
       */
      ThreadManager( 
          gfl::heap::HeapBase* heap_for_thread_management,
          u32 max_running_thread_count,
          u32 max_waiting_request_count );

      /**
       * @brief デストラクタ
       *
       * @attention
       * カレントスレッドを休止することで, 他スレッドの完了を待ちます.
       * IsAllRequestFinished() で全てのリクエストが完了してから破棄する方が望ましいです.
       */
      virtual ~ThreadManager();

      /**
       * @brief スレッド管理処理( 毎フレーム呼んでください )
       * 実行中のスレッドを監視し, 終了したスレッドを管理から除外する.
       * 実行条件が整ったスレッドを起動する.
       */
      void UpdateThreadManagement( void );


      //-----------------------------------------------------------------------
      // リクエストの登録・キャンセル
      //-----------------------------------------------------------------------
      /**
       * @brief スレッド起動リクエストを登録する
       * @param req  登録するリクエスト
       * @retval true  登録できた
       * @retval false 登録できなかった
       */
      bool AddThreadRequest( const Request& req );

      /**
       * @brief 待機中のリクエストの内, 指定したIDのリクエストを削除する
       * @param requestID 削除するリクエストのID
       */
      void CancelWaitingThreadRequest( u32 requestID );


      //-----------------------------------------------------------------------
      // リクエストの状態取得
      //-----------------------------------------------------------------------
      /**
       * @brief 全てのリクエストの実行が終了したか？
       * @retval true  全てのスレッドが終了した
       * @retval false 終了していないスレッドが存在する
       */
      bool IsAllRequestFinished( void ) const;

      /**
       * @brief 指定したIDのリクエストが終了したか？
       * @param requestID チェックするリクエストのID
       * @retval true  終了した
       * @retval false まだ終了していない
       */
      bool IsRequestFinished( u32 requestID ) const;

      /**
       * @brief 指定したIDのリクエストが待機中か？
       * @retval true  待機中
       * @retval false 実行中 or リクエストが存在しない
       */
      bool IsRequestWaiting( u32 ) const;

      /**
       * @brief 指定したIDのリクエストが実行中か？
       * @param requestID チェックするリクエストのID
       * @retval true  実行中
       * @retval false 待機中 or リクエストが存在しない
       */
      bool IsRequestRunning( u32 requestID ) const;

      /**
       * @brief 同時に起動できるリクエストの数を取得する
       */
      u32 GetMaxRunningRequestCount( void ) const;

      /**
       * @brief 実行中のリクエストの数を取得する
       */
      u32 GetRunningRequestCount( void ) const;

      /**
       * @brief 同時に保持できるリクエストの数を取得する
       */
      u32 GetMaxWaitingRequestCount( void ) const;

      /**
       * @brief 待機中のリクエストの数を取得する
       */
      u32 GetWaitingRequestCount( void ) const;



      protected: 
      //-----------------------------------------------------------------------
      // 派生クラスに公開するメソッド
      //-----------------------------------------------------------------------
      /**
       * @brief スレッド管理用ヒープを取得する
       */
      gfl::heap::HeapBase* GetHeapForThreadManagement( void ) const { return m_heap; }

      /**
       * @brief 実行中の全リクエストの完了を待つ
       */
      void WaitAllRunningRequest( void );

      /**
       * @brief 待機中の全リクエストをキャンセルする
       */
      void CancelAllWaitingRequest( void );



      private: 

      gfl::heap::HeapBase* m_heap;

      Request* m_waitingRequest;
      u32 m_waitingRequestCount;
      u32 m_maxWaitingRequestCount;

      Request* m_runningRequest;
      u32 m_runningRequestCount;
      u32 m_maxRunningRequestCount;

      void CancelRequest( const Request* );
      void RemoveDeadThread( void );
      void RemoveRunningRequestByIndex( u32 );
      void RemoveWaitingRequestByIndex( u32 );
      bool IsNewThreadLaunchEnable( void ) const;
      void StartHighestPriorityRequest( void );
      void RegisterRunningRequest( const Request& );
      void StartRequest( const Request& );
#if GFLTHREADMAN_TICK_ENABLE
      void StartTick( Request* );
      void EndTick( Request* );
      s64 GetCurrentTick( void ) const;
#endif
    };






    /**
     * @brief リクエストについての通知を受け取るためのインターフェース
     *
     * @note
     * ThreadManager による管理が終了した時点で何れかのメソッドが呼ばれる.
     */
    class IThreadManageCallback
    {
      friend class ThreadManager;

      private:

      /**
       * @brief リクエスト終了時のコールバック
       * @param finished_req  終了したリクエスト
       *
       * @note 
       * リクエストされたスレッドの動作が完了した( スレッドが死んだ )際に通知されます.
       * 以降, ThreadManager では finished_req に対する操作は一切行いません.
       * ( スレッドの破棄も行いません ) 
       * このメソッドをオーバーライドし, 適切な終了処理を行ってください.
       */
      virtual void OnRequestFinished( const ThreadManager::Request& finished_req ) { }

      /**
       * @brief リクエストが実行されずに終了した際のコールバック
       * @param canceled_req  実行されなかったリクエスト
       *
       * @note 
       * リクエストが実行されずに破棄された際に通知されます.
       * 以降, ThreadManager では canceled_req に対する操作は一切行いません.
       * ( スレッドの破棄も行いません ) 
       * このメソッドをオーバーライドし, 適切な終了処理を行ってください.
       */
      virtual void OnRequestCanceled( const ThreadManager::Request& canceled_req ) { }
    };





  }  // namespace base
}  // namespace gfl

#endif  // __GFL_THREADMANAGER_H__

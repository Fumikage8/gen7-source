//=============================================================================
/**
 * @file   gfl_Thread.h
 * @brief  スレッド
 * @author obata_toshihiro
 * @date   2010.11.09
 */
//=============================================================================
#ifndef __GFL_THREAD_H__
#define __GFL_THREAD_H__
#pragma once

#include <nn.h>
#include <gfl_Heap.h> 
#include <gfl_Macros.h>

namespace gfl {
  namespace base {

    class SimpleStackArea;
    class SafeStackArea;

    class Thread 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( Thread );
      friend void ThreadFunc( Thread* thread );  // スレッド制御関数


      public: 

      /**
       * @brief スレッド管理情報の初期化
       * @param heap スレッドの管理領域として使用するヒープ
       */
      static void InitializeThreadManagement( gfl::heap::HeapBase* heap );

      /**
       * @brief スレッド管理の後片付け
       *
       * @note
       * 管理対象のスレッドが生き残っている場合
       * 強制的に kill し, スレッドの終了を待ちます。
       */
      static void FinalizeThreadManagement( void );

      /**
       * @brief 動作中のスレッドの数を取得する
       * @return 動作が完了していない, gfl::base::Threadインスタンスの数
       *
       * @attention 
       * メインスレッドやサウンドスレッドなど
       * システムが立てたスレッドの数は含まれません。
       */
      static u32 GetValidUserThreadCount( void ); 

      /**
       * @brief 同じ優先度の他スレッドに実行機会を与える
       */
      static void Yield( void ); 

      /**
       * @brief カレントスレッドを指定時間停止させる
       *
       * @param milliSeconds スレッドを停止させる時間（ミリ秒）
       */
      static void Sleep( s64 milliSeconds );

      /**
       * @brief 全ての動作中のスレッドを kill する
       *
       * @note
       * ソフトウェアリセットなど, 大本から一括制御する目的で作成。
       * 本来は, スレッドの所有者であるオブジェクトが正しくスレッドを kill するべき。
       */
      static void KillAllRunningThread( void );


      /**
       * @brief スレッドIDがメインスレッドかチェック
       */
      static bool IsMainThread( bit32 thread_id );

      private:

      // 動作中のスレッド管理
      static const u8 MAX_RUNNING_THREAD_COUNT = 32;
      static u32 s_runningThreadCount;
      static Thread* s_runningThread[ MAX_RUNNING_THREAD_COUNT ];
      static nn::os::CriticalSection* s_criticalSectionForThreadManagement;

      static void AddManagedThread( Thread* );
      static void RemoveManagedThread( Thread* );






      public: 


      enum StackType
      {
        // スタックオーバーフロー・スタックアンダーフロー検出なし
        // スタック領域のアラインメントなし
        STACK_TYPE_SIMPLE,

        // スタックオーバーフロー・スタックアンダーフロー検出あり
        // スタック領域は4096バイトアラインメント
        // スタック領域とは別に4096*2バイト必要
        STACK_TYPE_SAFE,
      };

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap       スタック領域に使用するヒープ
       * @param stackSize  スタック領域のサイズ
       * @param stackType  スタック領域のタイプ
       */
      //-----------------------------------------------------------------------
      Thread( gfl::heap::HeapBase* heap, u32 stackSize, StackType stackType = STACK_TYPE_SIMPLE );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~Thread(); 

      /**
       * @brief スレッドの優先度
       *
       * @note 
       * 0が最も高い優先度
       * 31が最も低い優先度
       */
      typedef u8 Priority;
      static const Priority HighestPriority = nn::os::HIGHEST_THREAD_PRIORITY;  // ( == 0 )
      static const Priority DefaultPriority = nn::os::DEFAULT_THREAD_PRIORITY;  // ( == 16 )
      static const Priority LowestPriority  = nn::os::LOWEST_THREAD_PRIORITY;   // ( == 31 )

      //-----------------------------------------------------------------------
      /**
       * @brief スレッドを開始する
       * @param priority  スレッドの優先度
       */
      //-----------------------------------------------------------------------
      void Start( Priority priority );

      //-----------------------------------------------------------------------
      /**
       * @brief スレッドを一時停止させる
       */
      //-----------------------------------------------------------------------
      void PauseOn( void );

      //-----------------------------------------------------------------------
      /**
       * @brief スレッドの一時停止を解除する
       */
      //-----------------------------------------------------------------------
      void PauseOff( void );

      //-----------------------------------------------------------------------
      /**
       * @brief スレッドが一時停止中かどうかを調べる
       * @retval true   一時停止中
       * @retval false  動作中
       */
      //-----------------------------------------------------------------------
      bool IsPausing( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief スレッドを終了させる
       */
      //-----------------------------------------------------------------------
      void Kill( void );

      //-----------------------------------------------------------------------
      /**
       * @brief スレッドが生きているかどうかを調べる
       * @retval true  生きている
       * @retval false 死んでいる
       */
      //-----------------------------------------------------------------------
      bool IsAlive( void ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief 自身を休止状態にする
       */
      //-----------------------------------------------------------------------
      void Wait( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 待ち状態か？
       * @retval true  待ち状態
       * @retval false 待ち状態でない
       */
      //-----------------------------------------------------------------------
      bool IsWaiting( void );

      //-----------------------------------------------------------------------
      /**
       * @brief Wait()による休止状態を解除する
       */
      //-----------------------------------------------------------------------
      void Signal( void );



      private:

      /**
       * @brief 開始処理
       *
       * @note
       * スレッドの開始直後に呼ばれる関数です。
       * 派生クラスで定義してください。
       */
      virtual void Initialize( void ) {}

      /**
       * @brief メイン処理
       *
       * @note
       * スレッド関数のループ内で呼ばれ続ける関数です。
       * 派生クラスで定義してください。
       */
      virtual void Main( void ) = 0;

      /**
       * @brief 終了処理
       *
       * @note
       * スレッドの終了直前に呼ばれる関数です。
       * 派生クラスで定義してください。
       */
      virtual void Finalize( void ) {} 

      /**
       * @brief スレッド終了時の処理
       *
       * @note
       * Kill() が呼ばれた直後に実行される処理です。
       * 派生クラスで定義してください。
       */
      virtual void OnKill( void ) {}



      private:

      nn::os::Thread m_nnosThread;
      SimpleStackArea* m_simpleStackArea;
      SafeStackArea* m_safeStackArea;
      nn::os::LightEvent m_pauseEvent;
      nn::os::LightEvent m_waitEvent;
      bool m_isPausing;    // ポーズ中かどうか
      bool m_pauseRequest; // ポーズリクエストの有無
      bool m_killFlag;     // スレッドを終わらせるかどうか
      bool dummy;
    };

  } // namespace base
} // namespace gfl

#endif // __GFL_THREAD_H__

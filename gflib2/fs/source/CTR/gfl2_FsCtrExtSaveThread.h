#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @brief  外部セーブデータファイルの読み込みを行うスレッド
 * @file   gfl_ExtSaveThread.h
 * @author obata_toshihiro
 * @author  tamada
 * @date   2012.09.15
 */
//=============================================================================
#ifndef __GFL_EXTSAVETHREAD_H__
#define __GFL_EXTSAVETHREAD_H__
#include <heap/include/gfl2_heap_base.h>
#include <thread/include/gfl2_Thread.h>


namespace gfl2 {
  namespace fs { 

    class ExtSaveSystem;


    class ExtSaveThread : public gfl2::thread::ThreadInterface
    {
      public:

        //---------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param heap           スレッドのスタック領域に使用するヒープ
         * @param stack_size     スレッドのスタック領域のサイズ
         * @param extsave_system  外部セーブデータシステム
         *
         * @note インスタンス生成後, 自動的にスレッドを起動する.
         */
        ExtSaveThread( 
            gfl2::heap::HeapBase* heap,
            u32 stack_size,
            ExtSaveSystem* extsave_system );

        /**
         * @brief デストラクタ
         */
        virtual ~ExtSaveThread(); 

      private: 

        ExtSaveSystem* m_extsave_system;
        gfl2::thread::Thread* m_pThread;
      public:

        virtual void Function( void );
        void Kill( void ){ m_pThread->Kill(); }
        bool IsAlive( void ){ return m_pThread->IsAlive(); }
        void Start( int priority ) { m_pThread->Start( priority ); }
    }; 


  }  // namespace fs
}  // namespace gfl


#endif // __GFL_EXTSAVETHREAD_H__ 
#endif
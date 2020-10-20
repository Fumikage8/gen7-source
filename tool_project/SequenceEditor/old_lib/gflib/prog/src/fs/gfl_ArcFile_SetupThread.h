//========================================================================================
/**
 * @brief  ArcFileインスタンスを高速モードへ移行させるスレッド
 * @file   gfl_ArcFile_SetupThreadForFastMode.h
 * @author obata_toshihiro
 * @date   2011.05.10
 */
//========================================================================================
#ifndef __GFL_ARCFILE_SETUP_THREAD_FOR_FAST_MODE_H__
#define __GFL_ARCFILE_SETUP_THREAD_FOR_FAST_MODE_H__
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>

namespace gfl {
  namespace fs { 

    class ArcFile;


    class SetupThread : public gfl::base::Thread
    {
      public:
        //--------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param arcfile                 高速モードへ移行させるインスタンス
         * @param heap_for_arcfile_table  高速アクセステーブルを確保するヒープ
         * @param heap_for_stack_area     スレッドのスタック領域を確保するヒープ
         * @param stack_size              スレッドのスタック領域のサイズ
         */
        //--------------------------------------------------------------------------------
        SetupThread( 
            ArcFile* arcfile,
            gfl::heap::HeapBase* heap_for_arcfile_table,
            gfl::heap::HeapBase* heap_for_stack_area,
            u32 stack_size );

        //--------------------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //--------------------------------------------------------------------------------
        virtual ~SetupThread(); 



      private:
        gfl::heap::HeapBase* m_heapForFastAccessTable; // 高速アクセスのためのテーブルを確保するヒープ
        gfl::fs::ArcFile* m_arcfile;


        /**
         * @brief スレッドメイン処理
         * @note 高速モードへの移行が完了したら自殺する
         * @note gfl::base::Thread::Main()をオーバーライド
         */
        virtual void Main( void ); 
    }; 


  } // namespace fs
} // namespace gfl

#endif // __GFL_ARCFILE_SETUP_THREAD_FOR_FAST_MODE_H__

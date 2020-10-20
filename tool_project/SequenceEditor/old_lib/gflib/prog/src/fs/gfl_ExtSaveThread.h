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
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>


namespace gfl {
  namespace fs { 

    class ExtSaveSystem;


    class ExtSaveThread : public gfl::base::Thread
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
            gfl::heap::HeapBase* heap,
            u32 stack_size,
            ExtSaveSystem* extsave_system );

        /**
         * @brief デストラクタ
         */
        virtual ~ExtSaveThread(); 

      private: 

        ExtSaveSystem* m_extsave_system;

        virtual void Main( void );
    }; 


  }  // namespace fs
}  // namespace gfl


#endif // __GFL_EXTSAVETHREAD_H__ 

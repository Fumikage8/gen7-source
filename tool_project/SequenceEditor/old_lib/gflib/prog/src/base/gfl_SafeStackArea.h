//=============================================================================
/**
 * @file gfl_SafeStackArea.h
 * @brief オーバーフローの検出を行うスタック領域
 * @author obata_toshihiro
 * @date 2012.04.09
 */
//=============================================================================
#ifndef __GFL_SAFESTACKAREA_H__
#define __GFL_SAFESTACKAREA_H__
#include <gfl_Heap.h>

namespace gfl {
  namespace base {


    class SafeStackArea : public nn::os::StackMemory
    {
      GFL_FORBID_COPY_AND_ASSIGN( SafeStackArea );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap      スタック領域の確保に使用するヒープ
       * @param stackSize スタックサイズ
       */
      //-----------------------------------------------------------------------
      SafeStackArea( gfl::heap::HeapBase* heap, u32 stackSize ) : 
        StackMemory(),
        m_stackArea( NULL ),
        m_stackSize( stackSize )
      {
        GFL_ASSERT( m_stackSize % 4096 == 0 );  //@check
        m_stackArea = GflHeapAllocMemoryAlign4096( heap, m_stackSize );
        this->Initialize( m_stackArea, m_stackSize );
      }

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~SafeStackArea()
      {
        this->Finalize();
        if( m_stackArea != NULL )
        {
          GflHeapFreeMemory( m_stackArea );
          m_stackArea = NULL;
        }
      }


      protected:
      void* m_stackArea;
      u32 m_stackSize;
    };

  } // namespace base
} // namespace gfl

#endif //__GFL_SAFESTACKAREA_H__

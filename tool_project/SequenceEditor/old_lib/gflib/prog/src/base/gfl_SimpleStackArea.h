//=============================================================================
/**
 * @file gfl_SimpleStackArea.h
 * @brief スレッドのスタック領域
 * @author obata_toshihiro
 * @date 2010.11.09
 */
//=============================================================================
#pragma once
#include <gfl_Heap.h>

namespace gfl {
  namespace base {


    class SimpleStackArea 
    {
      GFL_FORBID_COPY_AND_ASSIGN( SimpleStackArea );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap      スタック領域の確保に使用するヒープ
       * @param stackSize スタックサイズ
       */
      //-----------------------------------------------------------------------
      SimpleStackArea( gfl::heap::HeapBase* heap, u32 stackSize ) : 
        m_stackArea( NULL ),
        m_stackSize( stackSize )
      {
        m_stackArea = GflHeapAllocMemory( heap, stackSize );
      }

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~SimpleStackArea()
      {
        GflHeapFreeMemory( m_stackArea );
      }

      //-----------------------------------------------------------------------
      /**
       * @brief スタック領域の底辺のアドレスを取得する
       */
      //-----------------------------------------------------------------------
      inline uptr GetStackBottom( void )
      {
        return reinterpret_cast<uptr>(m_stackArea) + m_stackSize;
      }


      protected:
      void* m_stackArea;
      u32 m_stackSize;
    };

  } // namespace base
} // namespace gfl

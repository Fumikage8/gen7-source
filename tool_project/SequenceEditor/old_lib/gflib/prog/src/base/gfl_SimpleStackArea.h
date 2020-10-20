//=============================================================================
/**
 * @file gfl_SimpleStackArea.h
 * @brief �X���b�h�̃X�^�b�N�̈�
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
       * @brief �R���X�g���N�^
       * @param heap      �X�^�b�N�̈�̊m�ۂɎg�p����q�[�v
       * @param stackSize �X�^�b�N�T�C�Y
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
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~SimpleStackArea()
      {
        GflHeapFreeMemory( m_stackArea );
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �X�^�b�N�̈�̒�ӂ̃A�h���X���擾����
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

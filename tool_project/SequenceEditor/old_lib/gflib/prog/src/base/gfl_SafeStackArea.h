//=============================================================================
/**
 * @file gfl_SafeStackArea.h
 * @brief �I�[�o�[�t���[�̌��o���s���X�^�b�N�̈�
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
       * @brief �R���X�g���N�^
       * @param heap      �X�^�b�N�̈�̊m�ۂɎg�p����q�[�v
       * @param stackSize �X�^�b�N�T�C�Y
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
       * @brief �f�X�g���N�^
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

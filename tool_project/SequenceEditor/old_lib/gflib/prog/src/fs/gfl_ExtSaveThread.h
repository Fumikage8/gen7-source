//=============================================================================
/**
 * @brief  �O���Z�[�u�f�[�^�t�@�C���̓ǂݍ��݂��s���X���b�h
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
        // �R���X�g���N�^�E�f�X�g���N�^
        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         * @param heap           �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
         * @param stack_size     �X���b�h�̃X�^�b�N�̈�̃T�C�Y
         * @param extsave_system  �O���Z�[�u�f�[�^�V�X�e��
         *
         * @note �C���X�^���X������, �����I�ɃX���b�h���N������.
         */
        ExtSaveThread( 
            gfl::heap::HeapBase* heap,
            u32 stack_size,
            ExtSaveSystem* extsave_system );

        /**
         * @brief �f�X�g���N�^
         */
        virtual ~ExtSaveThread(); 

      private: 

        ExtSaveSystem* m_extsave_system;

        virtual void Main( void );
    }; 


  }  // namespace fs
}  // namespace gfl


#endif // __GFL_EXTSAVETHREAD_H__ 

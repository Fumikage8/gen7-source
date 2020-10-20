//========================================================================================
/**
 * @brief  ArcFile�C���X�^���X���������[�h�ֈڍs������X���b�h
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
         * @brief �R���X�g���N�^
         * @param arcfile                 �������[�h�ֈڍs������C���X�^���X
         * @param heap_for_arcfile_table  �����A�N�Z�X�e�[�u�����m�ۂ���q�[�v
         * @param heap_for_stack_area     �X���b�h�̃X�^�b�N�̈���m�ۂ���q�[�v
         * @param stack_size              �X���b�h�̃X�^�b�N�̈�̃T�C�Y
         */
        //--------------------------------------------------------------------------------
        SetupThread( 
            ArcFile* arcfile,
            gfl::heap::HeapBase* heap_for_arcfile_table,
            gfl::heap::HeapBase* heap_for_stack_area,
            u32 stack_size );

        //--------------------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //--------------------------------------------------------------------------------
        virtual ~SetupThread(); 



      private:
        gfl::heap::HeapBase* m_heapForFastAccessTable; // �����A�N�Z�X�̂��߂̃e�[�u�����m�ۂ���q�[�v
        gfl::fs::ArcFile* m_arcfile;


        /**
         * @brief �X���b�h���C������
         * @note �������[�h�ւ̈ڍs�����������玩�E����
         * @note gfl::base::Thread::Main()���I�[�o�[���C�h
         */
        virtual void Main( void ); 
    }; 


  } // namespace fs
} // namespace gfl

#endif // __GFL_ARCFILE_SETUP_THREAD_FOR_FAST_MODE_H__

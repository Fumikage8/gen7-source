//========================================================================================
/**
 * @brief  ArcFile�C���X�^���X���������[�h�ֈڍs������X���b�h
 * @file   gfl_ArcFile_SetupThreadForFastMode.cpp
 * @author obata_toshihiro
 * @date   2011.05.10
 */
//========================================================================================
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_ArcFile.h>
#include "gfl_ArcFile_SetupThread.h"

namespace gfl {
  namespace fs { 


    //------------------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param arcfile                 �������[�h�ֈڍs������C���X�^���X
     * @param heap_for_arcfile_table  �����A�N�Z�X�e�[�u�����m�ۂ���q�[�v
     * @param heap_for_stack_area     �X���b�h�̃X�^�b�N�̈���m�ۂ���q�[�v
     * @param stack_size              �X���b�h�̃X�^�b�N�̈�̃T�C�Y
     */
    //------------------------------------------------------------------------------------
    SetupThread::SetupThread(
        ArcFile* arcfile,
        gfl::heap::HeapBase* heap_for_arcfile_table,
        gfl::heap::HeapBase* heap_for_stack_area,
        u32 stack_size ) :
      gfl::base::Thread( heap_for_stack_area, stack_size ),
      m_heapForFastAccessTable( heap_for_arcfile_table ),
      m_arcfile( arcfile )
    {
    } 

    //------------------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //------------------------------------------------------------------------------------
    SetupThread::~SetupThread()
    {
    }

    /**
     * @brief �X���b�h���C������
     * @note �������[�h�ւ̈ڍs�����������玩�E����
     * @note gfl::base::Thread::Main()���I�[�o�[���C�h
     *
     * SBTS[1038]:�r�������ǉ����� by takahashi & iwasawa
     *
     * ArcFile::StartSetupForFastMode()�ł��̃X���b�h���N��������A
     * WaitSetupForFastMode()�Ŗ����I�ɃX���b�h�j����҂��Ă���
     * AddReadRequest()���̃t�@�C���A�N�Z�X�X���b�h���N�����Ȃ���
     * ���̃X���b�h���r��������s���Ă��Ȃ��������߂�
     * �t�@�C���ւ̓����A�N�Z�X���������s��������N�����Ă����B
     *
     * SBTS[1038]�̎���ł̓A�[�J�C�u�t�@�C���̃��[�h�̈悪�j�󂳂�
     * NULL�Q�Ƃ�w�b�_�̈�j��ɂ��A�T�[�g��U�����Ă����B
     */
    void SetupThread::Main( void )
    {
      // �t�@�C�����J��
      if( m_arcfile->IsOpen() == false ) {
        m_arcfile->Open();
      }
      // �t�@�C���I�[�v����҂�
      nn::os::LightEvent* open_event = m_arcfile->GetOpenEvent();
      if( !open_event->IsSignaled() ) {
        open_event->Wait();
      }

      // �������[�h�ֈڍs
      if( m_arcfile->IsFastMode() == false ) {
        m_arcfile->SetupForFastMode( m_heapForFastAccessTable );
      }

      // �������[�h�ւ̈ڍs��҂�
      nn::os::LightEvent* fast_event = m_arcfile->GetFastSetupEvent();
      if( !fast_event->IsSignaled() ) {
        fast_event->Wait();
      }

      this->Kill();
    }


  } // namespace fs
} // namespace gfl

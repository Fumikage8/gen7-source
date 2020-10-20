//=============================================================================
/**
 * @brief  �O���Z�[�u�f�[�^�t�@�C���̓ǂݏ������s���X���b�h
 * @file   gfl_ExtSaveThread.cpp
 * @author obata_toshihiro
 * @author  tamada
 * @date    2012.09.15
 *
 * @note    �X���b�h�̐����E�폜���J��Ԃ��Ȃ��悤�ɁA�X���[�v������悤�ɂ������B
 */
//=============================================================================
#include <nw.h>
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_ExtSaveSystem.h>
#include "gfl_ExtSaveThread.h" 

namespace gfl {
  namespace fs {



    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap           �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
     * @param stack_size     �X���b�h�̃X�^�b�N�̈�̃T�C�Y
     * @param extsave_system  �O���Z�[�u�f�[�^�V�X�e��
     * @param mode           ���샂�[�h
     *
     * @note �C���X�^���X������, �����I�ɃX���b�h���N������.
     */
    //-------------------------------------------------------------------------
    ExtSaveThread::ExtSaveThread(
        gfl::heap::HeapBase* heap,
        u32 stack_size,
        ExtSaveSystem* extsave_system ):
      Thread( heap, stack_size ),
      m_extsave_system( extsave_system )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    ExtSaveThread::~ExtSaveThread()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �X���b�h���C������
     * @note �ǂݍ���( �������� ) >> ���E
     */
    //-------------------------------------------------------------------------
    void ExtSaveThread::Main( void )
    {
      m_extsave_system->ThreadCallBack();
      this->Kill();
    }



  } // namespace fs
} // namespace gfl

//=============================================================================
/**
 * @brief �A�[�J�C�u�ǂݍ��݃X���b�h
 * @file gfl_ArcFile_ReadThread.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include "gfl_ArcFile_ReadThreadStatic.h"

namespace gfl {
  namespace fs {


    // �X���b�h�̃X�^�b�N�T�C�Y
    const u32 ArcFileReadThreadStatic::STACK_SIZE = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap_for_thread �X���b�h�̓���Ɏg�p����q�[�v
     * @param desc            �A�[�J�C�u�ǂݍ��݂̏ڍ׏��
     */
    //-------------------------------------------------------------------------
    ArcFileReadThreadStatic::ArcFileReadThreadStatic( 
        gfl::heap::HeapBase* heap_for_thread,
        const Description& desc ) :
      base::Thread( heap_for_thread, STACK_SIZE ),
      m_desc( desc )
    {
      GFL_ASSERT_MSG( m_desc.dest_buf, "�ǂݍ��񂾃f�[�^�̊i�[����w�肵�Ă�������\n" );  //@check
      GFL_ASSERT_MSG( m_desc.arcfile, "�ǂݍ��ݑΏۂ̃A�[�J�C�u���w�肵�Ă�������\n" );  //@check
      GFL_ASSERT_MSG( m_desc.heap_for_arc, "�A�[�J�C�u�t�@�C���̍������e�[�u���̂��߂̃q�[�v���w�肵�Ă�������\n" ); //@check
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    ArcFileReadThreadStatic::~ArcFileReadThreadStatic()
    {
    }



    /**
     * @brief ���C������
     * @note gfl::base::Thread ���I�[�o�[���C�h
     */
    void ArcFileReadThreadStatic::Main( void )
    {
      ArcFile* arcfile = m_desc.arcfile;

      // �t�@�C�����J��
      if( arcfile->IsOpen() == false ) {
        arcfile->Open();
      }

      // �t�@�C���I�[�v����҂�
      nn::os::LightEvent* open_event = arcfile->GetOpenEvent();
      if( !open_event->IsSignaled() ) {
        open_event->Wait();
      }

      // �������[�h�ֈڍs
      if( arcfile->IsFastMode() == false ) {
        arcfile->SetupForFastMode( m_desc.heap_for_arc );
      }

      // �������[�h�ւ̈ڍs��҂�
      nn::os::LightEvent* fast_event = arcfile->GetFastSetupEvent();
      if( !fast_event->IsSignaled() ) {
        fast_event->Wait();
      }

      // �ǂݍ���
			size_t readSize = 0;
      if( m_desc.compressed ) 
      {
        arcfile->LoadCompressedData
					(m_desc.datID, m_desc.heap_for_uncomp, m_desc.dest_buf, m_desc.dest_size, &readSize);
      }
      else 
      {
        // �񈳏k�f�[�^
        readSize = arcfile->GetDataSize( m_desc.datID );
        if( readSize <= m_desc.dest_size ) {
          arcfile->LoadData( m_desc.datID, m_desc.dest_buf );
        }
      }
      if( m_desc.read_size != NULL ){
        *m_desc.read_size = readSize;
      }

      // ���g�̃X���b�h��KILL
      this->Kill();
    }



  }  // namespace fs
}  // namespace gfl

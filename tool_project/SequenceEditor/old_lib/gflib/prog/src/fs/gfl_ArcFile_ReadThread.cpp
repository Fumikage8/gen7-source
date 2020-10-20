//=============================================================================
/**
 * @brief �A�[�J�C�u�ǂݍ��݃X���b�h
 * @file gfl_ArcFile_ReadThread.cpp
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#include <base/gfl_Thread.h>
#include "gfl_ArcFile_ReadThread.h"

namespace gfl {
  namespace fs {


    // �X���b�h�̃X�^�b�N�T�C�Y
    const u32 ArcFileReadThread::STACK_SIZE = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap_for_thread �X���b�h�̓���Ɏg�p����q�[�v
     * @param desc            �A�[�J�C�u�ǂݍ��݂̏ڍ׏��
     */
    //-------------------------------------------------------------------------
    ArcFileReadThread::ArcFileReadThread( 
        gfl::heap::HeapBase* heap_for_thread,
        const Description& desc ) :
      base::Thread( heap_for_thread, STACK_SIZE ),
      m_desc( desc )
    {
      GFL_ASSERT_MSG( m_desc.dest_buf, "�ǂݍ��񂾃f�[�^�̊i�[����w�肵�Ă�������\n" );  //@check
      GFL_ASSERT_MSG( m_desc.arcfile, "�ǂݍ��ݑΏۂ̃A�[�J�C�u���w�肵�Ă�������\n" );  //@check
      GFL_ASSERT_MSG( m_desc.heap_for_buf, "�o�b�t�@�m�ۂ̂��߂̃q�[�v���w�肵�Ă�������\n" );  //@check
      GFL_ASSERT_MSG( m_desc.heap_for_arc, "�A�[�J�C�u�t�@�C���̍������e�[�u���̂��߂̃q�[�v���w�肵�Ă�������\n" ); //@check
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    ArcFileReadThread::~ArcFileReadThread()
    {
    }



    /**
     * @brief ���C������
     * @note gfl::base::Thread ���I�[�o�[���C�h
     */
    void ArcFileReadThread::Main( void )
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

      // HIO �ǂݍ��݂łȂ����, �������[�h�ֈڍs
      if( arcfile->IsHioFile() == false )
      {
        if( arcfile->IsFastMode() == false ) {
          arcfile->SetupForFastMode( m_desc.heap_for_arc );
        }

        // �������[�h�ւ̈ڍs��҂�
        nn::os::LightEvent* fast_event = arcfile->GetFastSetupEvent();
        if( !fast_event->IsSignaled() ) {
          fast_event->Wait();
        }
      }

      // �ǂݍ���
      void* data = NULL;
      u32 data_size = 0; 
      if( m_desc.compressed ) 
      {
        // ���k�f�[�^
        gfl::heap::HeapBase* heap_for_uncomp = ( m_desc.heap_for_uncomp == NULL ) ? m_desc.heap_for_buf : m_desc.heap_for_uncomp;
        data = arcfile->LoadCompressedDataAlloc( m_desc.datID, heap_for_uncomp, m_desc.heap_for_buf, m_desc.buf_alignment, &data_size );
      }
      else 
      {
        // �񈳏k�f�[�^
        data_size = arcfile->GetDataSize( m_desc.datID );
        if( 0 < data_size ) {
          data = GflHeapAllocMemoryAlign( m_desc.heap_for_buf, data_size, m_desc.buf_alignment );
          arcfile->LoadData( m_desc.datID, data );
        }
      }

      // �ǂݍ��񂾃f�[�^��Ԃ�
      *( m_desc.dest_buf ) = data;
      if( m_desc.dest_size ) {
        *( m_desc.dest_size ) = data_size;
      }

      // ���g�̃X���b�h��KILL
      this->Kill();
    }



  }  // namespace fs
}  // namespace gfl

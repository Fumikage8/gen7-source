//========================================================================================
/**
 * @brief  �t�@�C���ւ̓ǂݏ������s���X���b�h
 * @file   gfl_FileAccessThread.cpp
 * @author obata_toshihiro
 * @date   2011.04.22
 */
//========================================================================================
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_FileBase.h> 
#include "gfl_FileAccessThread.h"

namespace gfl {
  namespace fs { 


    //------------------------------------------------------------------------------------
    // �R���X�g���N�^
    //------------------------------------------------------------------------------------
    FileAccessThread::FileAccessThread( 
        gfl::heap::HeapBase* heap, u32 stack_size, const OperationSetting& setting ) : 
      gfl::base::Thread( heap, stack_size ),
      m_setting( setting ),
      m_operated_size( 0 ),
      m_file_close_flag( false )
    {
      GFL_ASSERT( 0 < m_setting.one_time_access_size );
    } 

    //------------------------------------------------------------------------------------
    // �f�X�g���N�^
    //------------------------------------------------------------------------------------
    FileAccessThread::~FileAccessThread()
    {
    }

    // �X���b�h�J�n����
    void FileAccessThread::Initialize( void )
    {
      // �o�b�t�@NULL�`�F�b�N
      if( m_setting.data_buffer == NULL ) {
        GFL_ASSERT(0);  //@check
        this->Kill();
        return;
      }

      // �t�@�C�����J��
      gfl::fs::File* file = m_setting.file;
      if( file->IsOpen() == false ) {
        bool open_result = file->Open( m_setting.file_path, m_setting.open_mode );
        if( !open_result ) {
          this->Kill();
        }
        m_file_close_flag = true;  // ���X���Ă����Ȃ�, �I�����ɕ���
      }
      // @note ���ɃI�[�v���ς݂̃t�@�C�����w�肳��Ă���ꍇ,
      // setting.open_mode�Ƃ̐����������Ă��Ȃ��Ǝ��s����.

      m_operated_size = 0; 
    }

    // �X���b�h���C������
    void FileAccessThread::Main( void )
    {
      gfl::fs::File* file = m_setting.file;
      GFL_ASSERT( file->IsOpen() );  //@check

      switch( m_setting.open_mode ) {
      case OPEN_MODE_READ:  
        {
          u32 read_pos = m_setting.file_offset + m_operated_size;
          void* dest_buffer = gfl::std::AddAddress( m_setting.data_buffer, m_operated_size );
          file->Seek( SEEK_BASE_BEGIN, read_pos );
          m_operated_size += file->Read( dest_buffer, m_setting.one_time_access_size );  
        }
        break;
      case OPEN_MODE_WRITE_NEW:  
      case OPEN_MODE_WRITE_ADD:  
        {
          u32 write_pos = m_setting.file_offset + m_operated_size;
          void* source_buffer = gfl::std::AddAddress( m_setting.data_buffer, m_operated_size );
          file->Seek( SEEK_BASE_BEGIN, write_pos );
          m_operated_size += file->Write( source_buffer, m_setting.one_time_access_size );  
        }
        break;
      default:
        GFL_ASSERT(0);  //@fix
				this->Kill();
        break;  
      }

      if( m_setting.total_data_size <= m_operated_size ) {
				this->Kill();
      }
    }

    // �X���b�h�I������
    void FileAccessThread::Finalize( void )
    {
      if( m_file_close_flag ) {
        m_setting.file->Close();
      }

      if( m_setting.callback_func ) {
        m_setting.callback_func( m_operated_size, m_setting.callback_work );
      }
    } 



#if 0		// Uncomp���d���̂Ŏg��Ȃ������ŁB
		//====================================================================================
		//====================================================================================
		// ���k�t�@�C���W�J		2011/10/07 by nakahiro
		//====================================================================================
		//====================================================================================

    //------------------------------------------------------------------------------------
    // �R���X�g���N�^
    //------------------------------------------------------------------------------------
    CompFileReadThread::CompFileReadThread( 
        gfl::heap::HeapBase* heap, u32 stack_size, const OperationSetting& setting ) : 
      gfl::base::Thread( heap, stack_size ),
      m_setting( setting ),
      m_operated_size( 0 ),
      m_file_close_flag( false ),
			comp_flag( true ),
			comp_sequence( 0 )
    {
      GFL_ASSERT( 0 < m_setting.one_time_access_size );
    } 

    //------------------------------------------------------------------------------------
    // �f�X�g���N�^
    //------------------------------------------------------------------------------------
    CompFileReadThread::~CompFileReadThread()
    {
    }

    // �X���b�h�J�n����
    void CompFileReadThread::Initialize( void )
    {
      // �t�@�C�����J��
      gfl::fs::File* file = m_setting.file;
      if( file->IsOpen() == false ) {
        bool open_result = file->Open( m_setting.file_path, OPEN_MODE_READ );
        if( !open_result ) {
          this->Kill();
        }
        m_file_close_flag = true;  // ���X���Ă����Ȃ�, �I�����ɕ���
      }
      // @note ���ɃI�[�v���ς݂̃t�@�C�����w�肳��Ă���ꍇ,
      // setting.open_mode�Ƃ̐����������Ă��Ȃ��Ǝ��s����.

      m_operated_size = 0; 
    }

    // �X���b�h���C������
    void CompFileReadThread::Main( void )
    {
      gfl::fs::File* file = m_setting.file;
      GFL_ASSERT( file->IsOpen() );

			switch( comp_sequence ){
			case 0:		// ���k�f�[�^�W�J�̈�m��
				tmpBuff = GflHeapAllocMemoryLowAlign( m_setting.heap, m_setting.total_data_size, 4 );
				comp_sequence = 1;
			case 1:		// ���k�f�[�^�ǂݍ���
				{
					u32 read_pos = m_setting.file_offset + m_operated_size;
					void* dest_buffer = gfl::std::AddAddress( tmpBuff, m_operated_size );
					file->Seek( SEEK_BASE_BEGIN, read_pos );
					m_operated_size += file->Read( dest_buffer, m_setting.one_time_access_size );
				}
	      if( m_setting.total_data_size <= m_operated_size ) {
					*m_setting.data_buffer = GflHeapAllocMemoryLowAlign(
																		m_setting.heap,
																		nn::cx::GetUncompressedSize( tmpBuff ),
																		m_setting.data_buffer_align );
					nn::cx::InitUncompContextLZ( &context, *m_setting.data_buffer );
					comp_sequence = 2;
				}
				break;

			case 2:		// ���k�f�[�^�W�J
				s32	ret = nn::cx::ReadUncompLZ( &context, tmpBuff, m_setting.total_data_size );
				// �I��
				if( ret == 0 ){
					GflHeapFreeMemory( tmpBuff );
					this->Kill();
				// �G���[
				}else if( ret < 0 ){
					GFL_ASSERT(0);
					GflHeapFreeMemory( tmpBuff );
					this->Kill();
				}
				break;
			}
    }

    // �X���b�h�I������
    void CompFileReadThread::Finalize( void )
    {
      if( m_file_close_flag ) {
        m_setting.file->Close();
      }

      if( m_setting.callback_func ) {
        m_setting.callback_func( m_operated_size, m_setting.callback_work );
      }
    } 
#endif

  } // namespace fs
} // namespace gfl

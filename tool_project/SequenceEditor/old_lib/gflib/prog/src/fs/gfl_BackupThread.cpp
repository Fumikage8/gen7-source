//=============================================================================
/**
 * @brief  �o�b�N�A�b�v�t�@�C���̓ǂݏ������s���X���b�h
 * @file   gfl_BackupThread.cpp
 * @author obata_toshihiro
 * @date   2011.02.17
 */
//=============================================================================
#include <nw.h>
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_BackupSystem.h>
#include "gfl_BackupThread.h" 

namespace gfl {
  namespace fs {



    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap           �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
     * @param stack_size     �X���b�h�̃X�^�b�N�̈�̃T�C�Y
     * @param backup_system  �o�b�N�A�b�v�V�X�e��
     * @param mode           ���샂�[�h
     *
     * @note �C���X�^���X������, �����I�ɃX���b�h���N������.
     */
    //-------------------------------------------------------------------------
    BackupThread::BackupThread(
        gfl::heap::HeapBase* heap,
        u32 stack_size,
        BackupSystem* backup_system,
        Mode mode ) :
      Thread( heap, stack_size ),
      m_mode( mode ),
      m_format_param(),
      m_create_param(),
      m_backup_system( backup_system ),
      m_result( BackupSystem::RESULT_OK )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    BackupThread::~BackupThread()
    {
    }


    //-------------------------------------------------------------------------
    /**
     * @brief MODE_FORMAT �̓���p�����[�^���Z�b�g����
     * @param param �t�H�[�}�b�g�ݒ�
     */
    //-------------------------------------------------------------------------
    void BackupThread::SetFormatParam( const FormatParam& param )
    {
      m_format_param = param;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void BackupThread::SetCreateParam( const CreateParam& cparam )
    {
      m_create_param = cparam;
      wcsncpy( m_path, cparam.filePath, MAX_FILE_PATH_LENGTH - 1 );
      if ( wcslen(cparam.filePath) == MAX_FILE_PATH_LENGTH - 1 )
      {
        m_path[MAX_FILE_PATH_LENGTH - 1] = L'\0';
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s���ʂ��擾����
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupThread::GetResult( void ) const
    {
      return m_result;
    }


    /**
     * @brief �X���b�h���C������
     * @note �ǂݍ���( �������� ) >> ���E
     */
    void BackupThread::Main( void )
    {
      switch( m_mode )
      {
      case MODE_SAVE:
        m_result = m_backup_system->Save();
        break;

      case MODE_LOAD:
        m_result = m_backup_system->Load();
        break;

      case MODE_FORMAT:
        {
          m_result = m_backup_system->Format( 
              m_format_param.maxDirectoryCount,
              m_format_param.maxFileCount,
              m_format_param.isDuplicate );
        }
        break;

      case MODE_CREATE:
        {
          m_result = m_backup_system->CreateBackupFile(
              m_path,
              m_create_param.fileSize );
        }
        break;
      }

      this->Kill();
    }



  } // namespace fs
} // namespace gfl

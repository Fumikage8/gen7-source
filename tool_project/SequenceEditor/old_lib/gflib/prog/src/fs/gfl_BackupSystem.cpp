//=============================================================================
/**
 * @file   gfl_BackupSystem.cpp
 * @brief  �o�b�N�A�b�v�V�X�e��
 * @author obata_toshihiro
 * @date   2011.02.16
 */
//==============================================================================
#include <nn.h>
#include <gfl_Heap.h>
#include <fs/gfl_FileApi.h>
#include <str/gfl_StrSys.h>
#include "gfl_BackupThread.h"
#include <fs/gfl_BackupSystem.h>
#include <ro/gfl_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::fs::BackupSystem);
template class gfl::base::SingletonAccessor<gfl::fs::BackupSystem>;
#endif


namespace gfl {
  namespace fs {


    const char* BackupSystem::SAVEDATA_ARCHIVE_NAME = "data:";
    const u32 BackupSystem::STACK_SIZE_FOR_FORMAT_THREAD = 4096;
    const u32 BackupSystem::STACK_SIZE_FOR_LOAD_THREAD = 4096;
    const u32 BackupSystem::STACK_SIZE_FOR_SAVE_THREAD = 4096;
    const u32 BackupSystem::STACK_SIZE_FOR_CREATE_THREAD = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param heap  �V�X�e���̍�Ɨ̈�Ɏg�p����q�[�v
     */
    //-------------------------------------------------------------------------
    BackupSystem::BackupSystem( gfl::heap::HeapBase* heap ) :
      m_heap( heap ),
      m_backup_entries(),
      m_backup_entry_count( 0 ),
      m_backup_finish_count( 0 ),
      m_file(),
      m_backup_thread( NULL ),
      //m_last_result(),
      m_is_mount(false)
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    BackupSystem::~BackupSystem( void )
    {
    }


    //-------------------------------------------------------------------------
    /**
     * @brief ���ʔ񓯊������҂�
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::WaitAsync( u32 wait_mode )
    {
      BackupThread::Mode mode = static_cast<BackupThread::Mode>(wait_mode);
      if( !m_backup_thread )
      {
        GFL_PRINT( "BackupSystem::WaitAsync �񓯊��������J�n����Ă��܂���\n" );
        return RESULT_NG_NOT_START;
      }
      else if( m_backup_thread->GetMode() != mode )
      {
        GFL_PRINT("%d!=%d:�Ⴄ�ړI�̃X���b�h�����쒆�ł�\n", m_backup_thread->GetMode(), mode );
        GFL_ASSERT( 0 ); //@check
        return RESULT_NG_ERROR;
      }
      else if( m_backup_thread->IsAlive() )
      {
        return RESULT_CONTINUE;  // �X���b�h���쒆
      }
      else
      {
        Result result = m_backup_thread->GetResult();
        GFL_DELETE m_backup_thread;
        m_backup_thread = NULL;
        return result;
      }
    }





    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�f�[�^�̈���t�H�[�}�b�g����
     *
     * @param max_dir_count   �f�B���N�g���̍ő吔
     * @param max_file_count  �t�@�C���̍ő吔
     * @param duplicate       ��d�����邩�ǂ���
     * @retval true   �t�H�[�}�b�g�ɐ���
     * @retval false  �t�H�[�}�b�g�Ɏ��s
     *
     * @attention �����̃o�b�N�A�b�v�t�@�C���͍폜����܂�.
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::Format(
        size_t max_dir_count, size_t max_file_count, DuplicateFlag duplicate )
    {
      nn::Result nn_result = nn::fs::FormatSaveData( max_file_count, max_dir_count, duplicate );
      if ( nn_result.IsSuccess() ) return RESULT_OK;

      NN_DBG_PRINT_RESULT( nn_result );
      if ( nn::fs::ResultNotEnoughSpace::Includes(nn_result) )
      { //�A�[�J�C�u�e�ʂ�����Ȃ��F�����w�肪�ԈႢ
        GFL_PRINT("ResultNotEnoughSpace: Program ERROR!!\n");
        GFL_ASSERT( 0 ); //@check
      }
      //�t�H�[�}�b�g�̎��s�͏�ɒv���I
      NN_ERR_THROW_FATAL_ALL( nn_result );
      return RESULT_NG_ERROR;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�f�[�^�̈�̔񓯊��t�H�[�}�b�g���J�n����
     *
     * @param thread_priority  �t�H�[�}�b�g�X���b�h�̃v���C�I���e�B
     * @param max_dir_count    �f�B���N�g���̍ő吔
     * @param max_file_count   �t�@�C���̍ő吔
     * @param duplicate        ��d�����邩�ǂ���
     *
     * @retval RESULT_OK       �t�H�[�}�b�g�J�n
     * @retval RESULT_NG_XXXX  �t�H�[�}�b�g�J�n�Ɏ��s
     *
     * @attention �����̃o�b�N�A�b�v�t�@�C���͑S�č폜����܂�.
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::StartAsyncFormat( 
        u8 thread_priority,
        size_t max_directories,
        size_t max_files,
        DuplicateFlag duplicate )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      } 

      m_backup_thread = GFL_NEW( m_heap )
        BackupThread( m_heap, STACK_SIZE_FOR_FORMAT_THREAD, this, BackupThread::MODE_FORMAT );

      BackupThread::FormatParam format_param;
      format_param.maxDirectoryCount = max_directories;
      format_param.maxFileCount      = max_files;
      format_param.isDuplicate       = duplicate;
      m_backup_thread->SetFormatParam( format_param );

      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�f�[�^�̈�̔񓯊��t�H�[�}�b�g�̊�����҂�
     *
     * @retval RESULT_CONTINUE  �t�H�[�}�b�g��
     * @retval RESULT_OK        �t�H�[�}�b�g�I��
     * @retval RESULT_NG_XXXX   �t�H�[�}�b�g�Ɏ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::WaitAsyncFormat( void )
    {
      return WaitAsync( BackupThread::MODE_FORMAT );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C�����쐬����
     *
     * @param file_path  �쐬����t�@�C���̃p�X
     * @param file_size  �쐬����t�@�C���̃T�C�Y
     * @retval RESULT_OK       �t�@�C���̍쐬�ɐ���
     * @retval RESULT_NG_XXXX  �t�@�C���̍쐬�Ɏ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::CreateBackupFile( const wchar_t* file_path, size_t file_size )
    {
      Result result;

      // �}�E���g
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // �t�@�C�����쐬
      result = this->CreateFile( file_path, file_size );
      if( result != RESULT_OK ) {
        this->UnMountSaveData();
        return result;
      }

      // �R�~�b�g
      this->CommitSaveData();

      // �}�E���g������
      result = this->UnMountSaveData();
      return result;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::StartAsyncCreateFile( u8 thread_priority, const wchar_t* file_path, size_t file_size )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      } 
      struct BackupThread::CreateParam cparam;
      cparam.filePath = file_path;
      cparam.fileSize = file_size;
      m_backup_thread = GFL_NEW( m_heap )
        BackupThread( m_heap, STACK_SIZE_FOR_CREATE_THREAD, this, BackupThread::MODE_CREATE );
      m_backup_thread->SetCreateParam( cparam );
      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::WaitAsyncCreateFile( void )
    {
      return WaitAsync( BackupThread::MODE_CREATE );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�f�[�^�A�[�J�C�u���}�E���g����
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::MountSaveData( void )
    {
      nn::Result nn_result = nn::fs::MountSaveData( SAVEDATA_ARCHIVE_NAME );
      m_last_result = nn_result;

      if( nn_result.IsSuccess() )
      {
        m_is_mount = true;
        return RESULT_OK;
      }
      else
      {
        NN_DBG_PRINT_RESULT( nn_result );
        if(nn_result <= nn::fs::ResultNotFound() ){
          // ���f�B�A�����݂��܂���
          return RESULT_NG_MEDIA_NOT_EXIST;
        }
        else if( nn_result <= nn::fs::ResultNotFormatted() ) {
          return RESULT_NG_NOT_FORMATTED;
          // �t�H�[�}�b�g���s�Ȃ��K�v������܂��B
          // ( ���R ) �Z�[�u�f�[�^�̈悪�t�H�[�}�b�g����Ă��Ȃ���Ԃł��B
          // ( �⑫ ) �Z�[�u�f�[�^�̈�����߂ă}�E���g����Ƃ��́A
          //          �K���t�H�[�}�b�g���K�v�ɂȂ�܂��B
        }
        else if(nn_result <= nn::fs::ResultBadFormat()) {
          return RESULT_NG_BAD_FORMAT;
          // �t�H�[�}�b�g���s�Ȃ��K�v������܂��B
          // ( ���R ) �t�@�C���t�H�[�}�b�g���s���ȏ�ԂɂȂ��Ă��܂��B
        }
        else if(nn_result <= nn::fs::ResultVerificationFailed()) {
          return RESULT_NG_BROKEN_DATA;
          // �t�H�[�}�b�g���s�Ȃ��K�v������܂��B
          // ( ���R ) �Z�[�u�f�[�^�����Ă��邩�A��₂���Ă��܂��B
          // ( �⑫ ) ��d������Ă���ꍇ�́A���[�U����ł��̏�ԂɂȂ邱�Ƃ͂���܂���B
          //          ��d������Ă��Ȃ��ꍇ�́A
          //          ���[�U����( �������ݒ��̓d���f��J�[�h������ )�ł����̃G���[�͔������܂��B
        }
        else if(nn_result <= nn::fs::ResultOperationDenied()) {
          return RESULT_NG_ERROR;
          // ��{�I�ɁA���̃G���[�͔������Ȃ����߁A�n���h�����O�͕K�{�ł͂���܂���B
          // ( �⑫ ) �A���A�ڐG�s�Ǔ��ɂ��n�[�h�E�F�G�A�I�ȗv���ɂ�莸�s�����ꍇ�A
          //          nn::fs::ResultMediaAccessError ���Ԃ�܂��B
          //          ���̂Ƃ��́A���g���C�����A�Q�[���J�[�h�̑}�������A
          //          �{�̂̍ċN���Ȃǂŕ��A����\��������܂��B
        }
        else {
          GFL_ASSERT_STOP(0); //@check ��L�ȊO�̃G���[�͔������܂���B
          return RESULT_NG_UNKNOWN;
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C�����쐬����
     * @param file_path  �쐬����t�@�C���̃p�X
     * @param file_size  �쐬����t�@�C���̃T�C�Y
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::CreateFile( const wchar_t* file_path, size_t file_size )
    {
      nn::Result nn_result = nn::fs::TryCreateFile( file_path, file_size );
      return GetSystemResult( nn_result );
    }
    //-------------------------------------------------------------------------

    /**
     * @brief SDK�̃t�@�C���A�N�Z�X�̌��ʂ���, �V�X�e���Ƃ��Ă̎��s���ʂ��擾����
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::GetSystemResult( nn::Result nn_result )
    {
      BackupFile::Result file_result = BackupFile::GetAccessResult( nn_result );
      return GetSystemResult( file_result );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �t�@�C���A�N�Z�X�̌��ʂ���, �V�X�e���Ƃ��Ă̎��s���ʂ��擾����
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::GetSystemResult( BackupFile::Result file_result )
    {
      Result result = RESULT_NG_UNKNOWN;
      switch( file_result ) {
      case BackupFile::ACCESS_RESULT_OK:
        result = RESULT_OK;
        break;
      case BackupFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:
        result = RESULT_NG_PATH_NOT_EXIST;
        break;
      case BackupFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:
        result = RESULT_NG_PATH_ALREADY_EXIST;
        break;
      case BackupFile::ACCESS_RESULT_NG_BAD_FORMAT:
        result = RESULT_NG_BAD_FORMAT;
        break;
      case BackupFile::ACCESS_RESULT_NG_BROKEN_DATA:
        result = RESULT_NG_BROKEN_DATA;
        break;
      case BackupFile::ACCESS_RESULT_NG_ERROR:
        result = RESULT_NG_ERROR;
        break;
      case BackupFile::ACCESS_RESULT_NG_LACK_OF_SPACE:
        result = RESULT_NG_LACK_OF_SPACE;
        break;
      default: result = RESULT_NG_UNKNOWN; break;
      }
      return result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�f�[�^�̕ύX���R�~�b�g����
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::CommitSaveData( void )
    {
      if ( !m_is_mount )
      {
        GFL_ASSERT( 0 );  //@check
        return RESULT_NG_UNMOUNT;
      }
      nn::Result nn_result = nn::fs::CommitSaveData();

      if( nn_result.IsSuccess() )
      {
        return RESULT_OK;
      }
      else if( nn_result <= nn::fs::ResultNotFound() )
      {
        GFL_PRINT( "WARNING! �R�~�b�g���s\n" );
        GFL_PRINT( "�Z�[�u�f�[�^�A�[�J�C�u�̓}�E���g����Ă��܂���.\n" );
        return RESULT_NG_UNMOUNT;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "�z��O�̃G���[�ł��B\n" );  //@check
        return RESULT_NG_UNKNOWN;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�f�[�^�A�[�J�C�u�̃}�E���g����������
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::UnMountSaveData( void )
    {
      if ( !m_is_mount )
      {
        GFL_ASSERT( 0 );  //@check
        return RESULT_NG_UNMOUNT;
      }
      nn::Result nn_result = nn::fs::Unmount( SAVEDATA_ARCHIVE_NAME );
      m_last_result = nn_result;

      if( nn_result.IsSuccess() )
      {
        m_is_mount = false;
        return RESULT_OK;
      }
      else if( nn_result <= nn::fs::ResultNotFound() )
      {
        NN_DBG_PRINT_RESULT( nn_result );
        GFL_PRINT( "WARNING! �A���}�E���g���s\n" );
        GFL_PRINT( "�Z�[�u�f�[�^�A�[�J�C�u�̓}�E���g����Ă��܂���.\n" );
        return RESULT_NG_UNMOUNT;
      }
      else
      {
        NN_DBG_PRINT_RESULT( nn_result );
        GFL_ASSERT_MSG( 0, "�z��O�̃G���[�ł��B\n" );  //@check
        return RESULT_NG_UNKNOWN;
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C�����폜����
     *
     * @param file_path  �폜����t�@�C���̃p�X
     *
     * @retval RESULT_OK       �t�@�C���̍폜�ɐ���
     * @retval RESULT_OK �ȊO  �t�@�C���̍폜�Ɏ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::DeleteBackupFile( const wchar_t* file_path )
    {
      Result result;

      // �}�E���g
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // �t�@�C�����폜
      result = this->DeleteFile( file_path );
      if( result != RESULT_OK ) {
        this->UnMountSaveData();
        return result;
      }

      // �R�~�b�g
      this->CommitSaveData();

      // �}�E���g������
      result = this->UnMountSaveData();
      return result;
    }

    /**
     * @brief �t�@�C�����폜����
     * @param file_path  �폜����t�@�C���̃p�X
     */
    BackupSystem::Result BackupSystem::DeleteFile( const wchar_t* file_path )
    {
      nn::Result nn_result = nn::fs::TryDeleteFile( file_path );
      BackupFile::Result access_result = BackupFile::GetAccessResult( nn_result );
      return GetSystemResult( access_result );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���̗L���𒲂ׂ�
     *
     * @param file_path   ���ׂ�t�@�C���̃p�X
     *
     * @retval true   �t�@�C���͑��݂���
     * @retval false  �t�@�C���͑��݂��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool BackupSystem::CheckBackupFileExist( const wchar_t* file_path )
    {
      bool found = false;
      this->MountSaveData();

      nn::Result nn_result;
      nn::fs::Directory directory;
      wchar_t directory_name[ MAX_FILE_PATH_LENGTH ];
      wchar_t file_name[ MAX_FILE_PATH_LENGTH ];

      gfl::fs::FileSystem::GetPathAndFileName( file_path, directory_name, file_name );
      nn_result = directory.TryInitialize( directory_name );
      if( nn_result.IsSuccess() )
      {
        s32 read_entry_num = 0;  // �ǂݍ��񂾐�
        do {
          const s32 max_read_entry_num = 10;  // ��x�ɓǂݍ��ސ��̏��
          nn::fs::DirectoryEntry entry[ max_read_entry_num ];
          nn_result = directory.TryRead( &read_entry_num, entry, max_read_entry_num );
          if( nn_result.IsSuccess() )
          {
            // �ǂݍ��񂾍��ڂ̒���, �T���Ă���t�@�C�����܂܂�Ă��邩�ǂ������ׂ�
            for( int i=0; i<read_entry_num; i++ ) {
              if( gfl::str::StrComp( file_name, entry[i].entryName ) == true ) {
                found = true;
                break;
              }
            }
          }
          else
          {
            NN_DBG_PRINT_RESULT( nn_result );
            break; // ���s�����璆�f
          }
        } while( ( found == false ) && ( 0 < read_entry_num ) );
      }

      this->UnMountSaveData();
      return found;
    }









    //-------------------------------------------------------------------------
    /**
     * @brief ����Ώۂ̃o�b�N�A�b�v�f�[�^��o�^����
     * @param[in]     file_path  �ǂݏ�������t�@�C���ւ̃p�X
     * @param[in,out] data       �������ރf�[�^ or �ǂݍ��񂾃f�[�^�̊i�[��
     * @param[in]     datasize   �������� or �ǂݍ��ރf�[�^�̃T�C�Y
     * @retval true   �o�^����
     * @retval false  �o�^���s
     */
    //-------------------------------------------------------------------------
    bool BackupSystem::AddBackupEntry(
        const wchar_t* file_path, void* data, u32 datasize )
    {
      if( this->IsBackupEntryFull() )
      {
        GFL_ASSERT_MSG( 0, "���ɍő吔�܂œo�^�ς݂ł��B\n" ); //@check
        return false;
      }
      else
      {
        gfl::str::StrCopy( m_backup_entries[ m_backup_entry_count ].file_path, file_path );
        m_backup_entries[ m_backup_entry_count ].data = data;
        m_backup_entries[ m_backup_entry_count ].datasize = datasize;
        m_backup_entry_count++;
        return true;
      }
    }

    /**
     * @brief �o�b�N�A�b�v�f�[�^���ő吔�܂œo�^�ς݂��ǂ����𒲂ׂ�
     * @retval true   �ő吔�܂œo�^�ς�
     * @retval false  �󂫗v�f������
     */
    bool BackupSystem::IsBackupEntryFull( void ) const
    {
      return ( MAX_BACKUP_ENTRY_NUM <= m_backup_entry_count );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �o�^����Ă���o�b�N�A�b�v����Ώۂ��N���A����
     */
    //-------------------------------------------------------------------------
    void BackupSystem::ClearBackupEntry( void )
    {
      m_backup_entry_count = 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�E���[�h�Ώۂ̃t�@�C�������擾����
     */
    //-------------------------------------------------------------------------
    u32 BackupSystem::GetBackupEntryCount( void ) const
    {
      return m_backup_entry_count;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �Z�[�u�E���[�h�����������t�@�C���̐����擾����
     */
    //-------------------------------------------------------------------------
    u32 BackupSystem::GetBackupFinishedCount( void ) const
    {
      return m_backup_finish_count;
    }









    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C����ǂݍ���
     *
     * @retval RESULT_OK       �ǂݍ��ݐ���
     * @retval RESULT_NG_XXXX  �ǂݍ��ݎ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::Load( void )
    {
      Result result = RESULT_OK;
      m_backup_finish_count = 0;

      // �}�E���g
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // �ǂݍ���
      for( u32 i=0; i<m_backup_entry_count; i++ )
      {
        const BackupEntry* entry = &m_backup_entries[i];
        bool open_result = m_file.Open( entry->file_path, OPEN_MODE_READ );
        if( !open_result ) {  // �I�[�v�����s
          BackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        size_t read_size = m_file.Read( entry->data, entry->datasize );
        if( read_size != entry->datasize ) {  // �ǂݍ��ݎ��s
          BackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        m_backup_finish_count++;
      }

      // �}�E���g������
      Result unmount_result = this->UnMountSaveData();
      return ( result == RESULT_OK ) ? unmount_result : result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���̔񓯊��ǂݍ��݂��J�n����
     * @param thread_priority  �o�b�N�A�b�v�X���b�h�̃v���C�I���e�B
     * @retval RESULT_OK       �ǂݍ��݊J�n
     * @retval RESULT_NG_XXXX  �ǂݍ��݊J�n�Ɏ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::StartAsyncLoad( u8 thread_priority )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      }
      m_backup_thread = GFL_NEW( m_heap )
        BackupThread( m_heap, STACK_SIZE_FOR_LOAD_THREAD, this, BackupThread::MODE_LOAD );
      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���̔񓯊��ǂݍ��݂̊�����҂�
     * @retval RESULT_CONTINUE  �ǂݍ��ݒ�
     * @retval RESULT_OK        �ǂݍ��ݏI��
     * @retval RESULT_NG_XXXX   �ǂݍ��݂Ɏ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::WaitAsyncLoad( void )
    {
      return WaitAsync( BackupThread::MODE_LOAD );
    }








    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���ɏ�������
     * @retval RESULT_OK       �������ݐ���
     * @retval RESULT_NG_XXXX  �������ݎ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::Save( void )
    {
      Result result = RESULT_OK;
      m_backup_finish_count = 0;

      // �}�E���g
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // ��������
      for( u32 i=0; i<m_backup_entry_count; i++ )
      {
        const BackupEntry* entry = &m_backup_entries[i];
        bool open_result = m_file.Open( entry->file_path, OPEN_MODE_WRITE_NEW );
        if( !open_result ) {  // �I�[�v�����s
          BackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        size_t written_size = m_file.Write( entry->data, entry->datasize );
        if( written_size != entry->datasize ) {  // �������ݎ��s
          BackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        m_backup_finish_count++;
      }

      // �R�~�b�g
      if( result == RESULT_OK ) {
        result = this->CommitSaveData();
      }

      // �}�E���g����
      Result unmount_result = this->UnMountSaveData();
      return ( result == RESULT_OK ) ? unmount_result : result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���ւ̔񓯊��������݂��J�n����
     * @param thread_priority  �o�b�N�A�b�v�X���b�h�̃v���C�I���e�B
     * @retval RESULT_OK       �������݊J�n
     * @retval RESULT_NG_XXXX  �������݊J�n�Ɏ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::StartAsyncSave( u8 thread_priority )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      }
      m_backup_thread = GFL_NEW( m_heap )
        BackupThread( m_heap, STACK_SIZE_FOR_SAVE_THREAD, this, BackupThread::MODE_SAVE );
      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �o�b�N�A�b�v�t�@�C���ւ̔񓯊��������݂̊�����҂�
     *
     * @retval RESULT_CONTINUE  �������ݒ�
     * @retval RESULT_OK        �������ݏI��
     * @retval RESULT_NG_XXXX   �������݂Ɏ��s
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::WaitAsyncSave( void )
    {
      return WaitAsync( BackupThread::MODE_SAVE );
    }








    //-------------------------------------------------------------------------
    /**
     * @brief �󂫗e�ʂ��擾����
     * @param[out] free_bytes  �󂫗e�ʂ̊i�[��
     * @retval RESULT_OK       �����Bfree_bytes �ɗL���Ȓl���i�[����܂��B
     * @retval RESULT_NG_XXXX  ���s�Bfree_bytes �͗L���Ȓl�ł͂���܂���B
     */
    //-------------------------------------------------------------------------
    BackupSystem::Result BackupSystem::GetFreeBytes( s64* free_bytes ) const
    {
      nn::Result nn_result =
        nn::fs::GetArchiveFreeBytes( free_bytes, BackupSystem::SAVEDATA_ARCHIVE_NAME );

      if( nn_result.IsSuccess() )
      {
        return RESULT_OK;
      }
      else if( nn_result <= nn::fs::ResultNotFound() )
      {
        GFL_PRINT( "WARNING! �󂫗e�ʂ̎擾�Ɏ��s\n" );
        GFL_PRINT( "�Z�[�u�f�[�^�A�[�J�C�u�̓}�E���g����Ă��܂���.\n" );
        return RESULT_NG_UNMOUNT;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "�z��O�̃G���[�ł��B\n" ); //@check
        return RESULT_NG_UNKNOWN;
      }
    }





  }  // namespace fs
}  // namespace gfl

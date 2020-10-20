/*---------------------------------------------------------------------------*
Project:  Horizon
File:     init_Alloc.cpp

Copyright (C)2009 Nintendo Co., Ltd.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Rev: 20442 $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------

#include <nn/fs/fs_Api.h>
#include <gfl_heap.h>
#include <gfl_fs.h>
#include <gfl_str.h>
#include <base/gfl_Thread.h>




namespace gfl {
  namespace fs  { 


    /**
     * @brief ���C���X���b�h�D�揇��
     */
    s32 FileSystem::m_MainThreadPriority = PRIORITY_APP_NORMAL;

    //------------------------------------------------------------------------------------
    // fs���C�u����������������
    //------------------------------------------------------------------------------------
    void FileSystem::Initialize( heap::HeapBase* heap, heap::HeapBase* device_heap, bool rom_mount_flag )
    {
      GFL_UNUSED( device_heap );

      nn::fs::Initialize();

      if( rom_mount_flag ) {
        FileSystem::MountRomArchive( heap ); 
      }

#if GFL_FILE_DEBUG
      //FileSystem::MountSdmcArchive();
      m_criticalSectionForProfilerAccess.Initialize();
#endif
    }


    // ROM�A�[�J�C�u���}�E���g����
    void FileSystem::MountRomArchive( gfl::heap::HeapBase* heap )
    {
      enum {
        MAX_FILE = 128,
        MAX_DIRECTORY = 16,
      };

      s32 work_memory_size = nn::fs::GetRomRequiredMemorySize( MAX_FILE, MAX_DIRECTORY, true ); 
      void* work_memory = GflHeapAllocMemory( heap, work_memory_size ); 
      nn::Result result = nn::fs::MountRom( MAX_FILE, MAX_DIRECTORY, work_memory, work_memory_size );

      GFL_PRINT( "File System Initialized..maxFile=%d, maxDirs=%d, workMemorySize=%d, workMem=%p\n",
          MAX_FILE, MAX_DIRECTORY, work_memory_size, work_memory );
      GFL_PRINT( " result=%d\n", result.IsSuccess() );
      GFL_ASSERT_STOP( result.IsSuccess() );
    }


#if GFL_FILE_DEBUG
    // SD�J�[�h�A�[�J�C�u���}�E���g����
    void FileSystem::MountSdmcArchive( void )
    {
      nn::Result result = nn::fs::MountSdmc();

      if( result.IsFailure() )
      {
        if( result <= nn::fs::ResultMediaNotFound() ) {
          GFL_PRINT( "SD �J�[�h��������Ȃ����A�������͔F���ł��܂���B\n" );
        }
        else if( result <= nn::fs::ResultArchiveInvalidated() ) {
          GFL_PRINT( "�}�E���g���� SD �J�[�h�������ꂽ�\��������܂��B\n" );
        }
        else if( result <= nn::fs::ResultMediaAccessError() ) {
          GFL_PRINT( "�ڐG�s�ǂȂǂ̗v���ɂ��ASD �J�[�h�A�N�Z�X���ɃG���[���������܂����B\n" );
        }
        else if( result <= nn::fs::ResultBadFormat() ) {
          GFL_PRINT( "SD �J�[�h�̃t�H�[�}�b�g���s���ł��BSD �J�[�h���t�H�[�}�b�g����K�v������܂��B\n" );
        }
        else {
          GFL_PRINT( "�A�v���P�[�V�������̕s��A�������͑z��O�̃G���[�ł��B\n" );
          GFL_ASSERT(0);  //@check
        }
      }
    }
#endif





    //------------------------------------------------------------------------------------
    // fs���C�u�������I������
    //------------------------------------------------------------------------------------
    void FileSystem::Finalize()
    {
      //@note SDK3.3 Finalize������J�� Finalize�͎g�p���Ȃ�
#if GFL_FILE_DEBUG
      m_criticalSectionForProfilerAccess.Finalize();
#endif
    }





    //------------------------------------------------------------------------------------
    // fs���C�u����������������Ă��邩�ǂ����𒲂ׂ�
    //------------------------------------------------------------------------------------
    bool FileSystem::IsInitialized()
    {
      return nn::fs::IsInitialized();
    }





    //----------------------------------------------------------------------------------------
    /**
     * @brief "�p�X + �t�@�C����" �̕����񂩂�, "�p�X" �� "�t�@�C����" �����ꂼ�ꒊ�o����.
     *
     * @param[in]  src_string      ���o�Ώۂ̕�����
     * @param[out] dest_path       ���o�����p�X�̊i�[��o�b�t�@
     * @param[out] dest_file_name  ���o�����t�@�C�����̊i�[��o�b�t�@
     *
     * @retval true   ����
     * @retval false  ���s
     *
     * @note
     * ���Ƃ���, "rom:/test_data.bin" �̏ꍇ,
     * �p�X�� = "rom:/"
     * �t�@�C���� = "test_data.bin"
     */
    //----------------------------------------------------------------------------------------
    bool FileSystem::GetPathAndFileName( 
        const wchar_t* src_string,
        wchar_t* dest_path,
        wchar_t* dest_file_name )
    {
      const wchar_t separator = L'/';
      const wchar_t end_code = L'\0';
      const size_t src_length = gfl::str::StrLen( src_string );
      GFL_ASSERT_STOP( src_length < MAX_PATH_LENGTH );

      // '/'�̈ʒu������
      s32 separator_pos = src_length - 1;
      while( 0 < separator_pos ) {
        separator_pos--;
        if( src_string[ separator_pos ] == separator ) {
          break;
        }
      }

      // �p�X�����R�s�[
      for( int i=0; i<=separator_pos; i++ ) {
        dest_path[i] = src_string[i];
      }
      dest_path[ separator_pos + 1 ] = end_code;

      // �t�@�C�������R�s�[
      u32 filename_len = src_length - separator_pos - 1;
      for( int i=0; i<filename_len; i++ ) {
        dest_file_name[i] = src_string[ separator_pos + 1 + i ];
      }
      dest_file_name[ filename_len ] = end_code;

      return true;
    } 


    //------------------------------------------------------------------------------------
    /**
     * @brief �����A�[�J�C�u�ȉ��Ƀt�@�C�������݂��邩�ǂ����𒲂ׂ�
     * @param filename �`�F�b�N�Ώۂ̃t�@�C����( rom:/xxxx )
     * @retval true  �t�@�C�������݂���
     * @retval false �t�@�C�������݂��Ȃ�
     */
    //------------------------------------------------------------------------------------
    bool FileSystem::CheckRomFileExist( const wchar_t* filename )
    {
      wchar_t dir_name[ MAX_PATH_LENGTH ];
      wchar_t file_name[ MAX_PATH_LENGTH ]; 
      gfl::fs::FileSystem::GetPathAndFileName( filename, dir_name, file_name );

      // �f�B���N�g�����J��
      nn::Result nn_result;
      nn::fs::Directory directory;
      nn_result = directory.TryInitialize( dir_name );
      if( nn_result.IsFailure() ) {
        return false;
      }

      s32 read_count = 0;  // �ǂݍ��񂾐�
      const s32 max_read_num = 10;  // ��x�ɓǂݍ��ސ��̏��
      nn::fs::DirectoryEntry entry[ max_read_num ];

      do 
      {
        nn_result = directory.TryRead( &read_count, entry, max_read_num );

        // �ǂݍ��񂾍��ڂ̒��ɒT���Ă���t�@�C�����܂܂�Ă��邩�ǂ������ׂ�
        if( nn_result.IsSuccess() ) {
          for( int i=0; i<read_count; i++ ) {
            if( gfl::str::StrComp( file_name, entry[i].entryName ) == true ) {
              return true;
            }
          }
        }
      } while( 0 < read_count );

      return false;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  �t�@�C���V�X�e���S�̂̓ǂݍ��ݗD�揇�ʂ�ݒ�
     *	
     *	@param  base_priority           �t�@�C���ǂݍ��ݗD�揇��
     *	@param  main_thread_priority    ���C���X���b�h�ł́A�t�@�C���ǂݍ��ݗD�揇��
     *
     *   PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// ���p���ӁI�I�I�@�h�L�������g��ǂ��ǂނ���
     *   PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// ����
     *   PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// �Ⴂ
     */
    //-----------------------------------------------------------------------------
    void FileSystem::SetPriority( s32 base_priority, s32 main_thread_priority )
    {
      nn::fs::SetPriority( base_priority );
      m_MainThreadPriority = main_thread_priority;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  ���C���X���b�h�ł̃t�@�C���ǂݍ��ݗD�揇�ʂ��擾
     */
    //-----------------------------------------------------------------------------
    s32 FileSystem::GetMainThreadPriority( void )
    {
      return m_MainThreadPriority;
    }






    // HIO 

#if GFL_HIO_ENABLE
    bool FileSystem::m_hio_initialize_flag = false;
    void* FileSystem::m_hio_library_work = NULL; 
    //------------------------------------------------------------------------------------
    // HIO���C�u����������������
    //------------------------------------------------------------------------------------
    void FileSystem::InitializeHio( gfl::heap::HeapBase* device_heap )
    {
      if( !m_hio_initialize_flag ) {
        m_hio_initialize_flag = true;
        m_hio_library_work = GflHeapAllocMemory( device_heap, nn::hio::WORKMEMORY_SIZE );
        nn::Result result = nn::hio::Initialize( m_hio_library_work );
        GFL_ASSERT( result.IsSuccess() );  //@check
      }
    }



    u8 FileSystem::m_hio_mount_count = 0;
    void* FileSystem::m_hio_mount_buffer[ MAX_HIO_MOUNT_COUNT ];

    char FileSystem::m_hio_mount_archive_name[ MAX_HIO_MOUNT_COUNT ][ 256 ];
    wchar_t FileSystem::m_hio_mount_root_dir[ MAX_HIO_MOUNT_COUNT ][ 256 ];

    //------------------------------------------------------------------------------------
    // HIO�A�[�J�C�u���}�E���g����
    //------------------------------------------------------------------------------------
    void FileSystem::MountHioArchive( gfl::heap::HeapBase* device_heap, char* archive_name, char* root_directory )
    {
      GFL_ASSERT( m_hio_mount_count < MAX_HIO_MOUNT_COUNT );  //@check
      if( m_hio_mount_count < MAX_HIO_MOUNT_COUNT ) {
        const u32 max_dirname_len = 256;
        wchar_t root_dir_wide[ max_dirname_len ];
        size_t ret = gfl::std::MultiByteStr2WideStr( root_dir_wide, root_directory, max_dirname_len );
        GFL_ASSERT( ret != size_t(-1) );  //@check

        const u32 max_file_num = 128;
        const u32 max_dir_num = 32;
        u32 mount_buffer_size = nn::fs::hio::GetHioRequiredMemorySize( max_file_num, max_dir_num );
        m_hio_mount_buffer[ m_hio_mount_count ] = GflHeapAllocMemory( device_heap, mount_buffer_size );
        nn::Result result = nn::fs::hio::MountHioArchive( archive_name, root_dir_wide, max_file_num, max_dir_num, m_hio_mount_buffer[ m_hio_mount_count ], mount_buffer_size );
        GFL_ASSERT( result.IsSuccess() );   //@check

        size_t len = ::std::strlen( archive_name ) + 1; 
        GFL_ASSERT_STOP( len <= 256 );
        gfl::std::MemCopy( archive_name, m_hio_mount_archive_name[m_hio_mount_count], sizeof(char)*len );
        len = ::std::wcslen( root_dir_wide ) + 1; 
        gfl::std::MemCopy( root_dir_wide, m_hio_mount_root_dir[m_hio_mount_count], sizeof(wchar_t)*len );

        m_hio_mount_count++;
      }
    }



    //------------------------------------------------------------------------------------
    // HIO���C�u�������I������
    //------------------------------------------------------------------------------------
    void FileSystem::FinalizeHio( void )
    {
      if( m_hio_initialize_flag ) {
        m_hio_initialize_flag = false;
        nn::hio::Finalize();
      } 

      if( m_hio_library_work ) {
        GflHeapFreeMemory( m_hio_library_work ); 
        m_hio_library_work = NULL;
      }

      for( u8 i=0; i<m_hio_mount_count; i++ ) {
        if( m_hio_mount_buffer[ m_hio_mount_count ] ) {
          GflHeapFreeMemory( m_hio_mount_buffer[ m_hio_mount_count ] );
          m_hio_mount_buffer[ m_hio_mount_count ] = NULL;
        }
      }
      m_hio_mount_count = 0;
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief �z�X�g�t�@�C���V�X�e���Ƀt�@�C�������݂��邩�ǂ����𒲂ׂ�
     * @param filename �`�F�b�N�Ώۂ̃t�@�C����( �t���p�X�Ŏw�� )( Mount����Ă���A�[�J�C�u���Ȃ�Γ����Ńt���p�X�ɒu������ )
     * @retval true  �t�@�C�������݂���
     * @retval false �t�@�C�������݂��Ȃ�
     */
    //------------------------------------------------------------------------------------
    bool FileSystem::CheckHioFileExist( const wchar_t* filename )
    {
      const u32 MAX_PATH_LENGTH = 256;
      wchar_t dir_name[ MAX_PATH_LENGTH ];
      wchar_t file_name[ MAX_PATH_LENGTH ]; 
      gfl::fs::FileSystem::GetPathAndFileName( filename, dir_name, file_name );

      //----------------------------------------------------
      // �����Adir_name���}�E���g����Ă���A�[�J�C�u���ł���΁A
      // ���[�g�f�B���N�g���[�ɕύX
      for( u8 i=0; i<m_hio_mount_count; i++ ) {
        wchar_t archive_name[ MAX_PATH_LENGTH ];
        size_t ret = gfl::std::MultiByteStr2WideStr( archive_name, m_hio_mount_archive_name[i], MAX_PATH_LENGTH );
        GFL_ASSERT( ret != size_t(-1) );  //@check

        size_t len = ::std::wcslen( dir_name ); 
        //TOMOYA_PRINT( "check archive %ls dir_name %ls\n", archive_name, dir_name );
        if( ::std::wcsncmp( archive_name, dir_name, len-1 ) == 0 )  // �Ō��/�������Ă���̂ŁA-1���Ă���B
        {
          gfl::std::MemCopy( m_hio_mount_root_dir[i], dir_name, sizeof(wchar_t)*MAX_PATH_LENGTH );
          //TOMOYA_PRINT( "dir_name %ls\n", dir_name );
          break;
        }
      }

      // �f�B���N�g�����J��
      nn::Result nn_result;
      nn::hio::HostDirectoryW directory;
      nn_result = directory.Open( dir_name );
      if( nn_result.IsFailure() ) {
        return false;
      }

      nn::hio::DirectoryEntryW entry;
      do {
        nn_result = directory.GetNextEntry( &entry ); 
        if( nn_result.IsFailure() ) {
          return false;
        } 
        else if( gfl::str::StrComp( file_name, entry.filename ) == true ) {
          return true;
        }
      } while( nn_result.IsSuccess() );

      return false;
    }

#endif

#if GFL_FILE_DEBUG

    /// 1�t���[�����̓ǂݍ��ݏ��̃o�b�t�@
    bool FileSystem::m_ProfilerFlag = false;
    FileSystem::ProfilerParam FileSystem::m_ProfilerParam;
    wchar_t FileSystem::m_ProfilerFileNameBuffer[FileSystem::PR_FILE_BUFFER_COUNT][FileSystem::PR_FILE_BUFFER_STRNUM];
    s32 FileSystem::m_ProfilerFileSizeBuffer[FileSystem::PR_FILE_BUFFER_COUNT];
    s32 FileSystem::m_ProfilerFileOffsetBuffer[FileSystem::PR_FILE_BUFFER_COUNT];
    s32 FileSystem::m_ProfilerFileUsBuffer[FileSystem::PR_FILE_BUFFER_COUNT];
    s32 FileSystem::m_ProfilerTotalSize;
    s32 FileSystem::m_ProfilerTotalUs;
    s32 FileSystem::m_ProfilerFileCount;
    nn::os::CriticalSection FileSystem::m_criticalSectionForProfilerAccess;
    bool FileSystem::m_ProfilerLastError = false;
    s32 FileSystem::m_ProfilerLastTotalUs = 0;

    //----------------------------------------------------------------------------
    /**
     *	@brief  �v���t�@�C���[�̐ݒ�
     *
     *	@param	flag      �v���t�@�C���[��ON�AOFF
     *	@param	cp_param  �v���t�@�C���[�̐ݒ�f�[�^
     */
    //-----------------------------------------------------------------------------
    void FileSystem::SetProfiler( bool flag, const ProfilerParam* cp_param )
    {
      m_ProfilerFlag  = flag;
      if( cp_param ){
        m_ProfilerParam = *cp_param;
      }
      else{
        m_ProfilerFlag  = false;
      }
    }

    /// �v���t�@�C���[�����쒆���`�F�b�N
    bool FileSystem::IsProfiling( void )
    {
      return m_ProfilerFlag;
    }
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  �v���t�@�C���[�̍X�V
     */
    //-----------------------------------------------------------------------------
    void FileSystem::UpdateProfiler( void )
    {
      m_ProfilerLastError   = false;
      m_ProfilerLastTotalUs = 0;
      
      if( !m_ProfilerFlag ){
        return ;
      }

      // �v���t�@�C�����O���ʂ��`�F�b�N
      if( m_ProfilerTotalUs > m_ProfilerParam.main_thread_us ){
        // ���e�ʂ𒴂����̂ŁA���O�o��
        GFL_PRINT( "----- FileSystem Profiler !OVER! ------\n" );

        GFL_PRINT( "MainThread FileReadTime %d us\n", m_ProfilerTotalUs );
        GFL_PRINT( "MainThread FileReadSize 0x%x byte\n", m_ProfilerTotalSize );
        GFL_PRINT( " - - - - - - - - - LOG - - - - - - - - \n" );
        for( u32 i=0; i<m_ProfilerFileCount; ++i ){
          GFL_PRINT( "  [[path (%ls)] pos %d size %d time %d us]\n", m_ProfilerFileNameBuffer[i], m_ProfilerFileOffsetBuffer[i], m_ProfilerFileSizeBuffer[i], m_ProfilerFileUsBuffer[i] );
        }
        GFL_PRINT( "----- FileSystem Profiler END    ------\n" );

        if( m_ProfilerParam.main_thread_us_over_assert ){
          GFL_ASSERT( m_ProfilerTotalUs <= m_ProfilerParam.main_thread_us );
        }

        m_ProfilerLastError   = true;
        m_ProfilerLastTotalUs = m_ProfilerTotalUs;
      }

      // Enter
      m_criticalSectionForProfilerAccess.Enter();
      {
        m_ProfilerTotalUs = 0;
        m_ProfilerTotalSize = 0;
        m_ProfilerFileCount = 0;
      }
      m_criticalSectionForProfilerAccess.Leave();
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  �P�ǂݍ��݃f�[�^���Ď�
     *
     *	@param	  path        �t�@�C���p�X
     *	@param	  thread_id   �X���b�hID
     *	@param	  size        �T�C�Y
     *	@param    offset      �I�t�Z�b�g
     *	@param    time        �ǂݍ��݂ɂ��������}�C�N���b
     */
    //-----------------------------------------------------------------------------
    void FileSystem::AddProfilerData( const wchar_t& path, bit32 thread_id, s32 size, s32 offset, s32 time )
    {
      if( !m_ProfilerFlag ){
        return ;
      }

      // Enter
      m_criticalSectionForProfilerAccess.Enter();

      bool main_thread = base::Thread::IsMainThread( thread_id );

      // ���C���X���b�h�ł̓ǂݍ��ݏ���ۑ�
      if( main_thread ){

        if(m_ProfilerFileCount < PR_FILE_BUFFER_COUNT){

          // ���C���X���b�h�ł̓ǂݍ��ݓ��e��ۑ�
          swprintf( m_ProfilerFileNameBuffer[ m_ProfilerFileCount ], PR_FILE_BUFFER_STRNUM, L"%ls", &path );
          m_ProfilerFileSizeBuffer[m_ProfilerFileCount]     = size;
          m_ProfilerFileOffsetBuffer[m_ProfilerFileCount]   = offset;
          m_ProfilerFileUsBuffer[m_ProfilerFileCount]       = time;

          ++m_ProfilerFileCount;
          
        }

        // �g�[�^���T�C�Y
        m_ProfilerTotalSize += size;

        // �g�[�^���o�ߎ���
        m_ProfilerTotalUs += time;
      }

      // ���O�o��
      switch( m_ProfilerParam.debug_print ){
      case DEBUG_PRINT_MAIN_THREAD:    ///< ���C���X���b�h�ǂݍ��݂�\��
        if( main_thread ){
          GFL_PRINT( "[thread_main [path (%ls)] pos %d size %d time %d us]\n", &path, offset, size, time );
        }
        break;

      case DEBUG_PRINT_OTHER_THREAD:   ///< ���C���X���b�h�ȊO�̓ǂݍ��݂�\��
        if( !main_thread ){
          GFL_PRINT( "[thread      [path (%ls)] pos %d size %d time %d us]\n", &path, offset, size, time );
        }
        break;

      case DEBUG_PRINT_ALL:            ///< �S�ǂݍ��݂�\��
        if( main_thread ){
          GFL_PRINT( "[thread_main [path (%ls)] pos %d size %d time %d us]\n", &path, offset, size, time );
        }else{
          GFL_PRINT( "[thread      [path (%ls)] pos %d size %d time %d us]\n", &path, offset, size, time );
        }
        break;

      default:
        break;
      
      }

      // Leave
      m_criticalSectionForProfilerAccess.Leave();
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  �Ō�̃v���t�@�C�����G���[���������`�F�b�N
     */
    //-----------------------------------------------------------------------------
    bool FileSystem::IsLastProfilerError( void )
    {
      return m_ProfilerLastError;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  �Ō�̃v���t�@�C�����G���[�̏ꍇ�A�g�[�^���̃}�C�N���b���擾
     */
    //-----------------------------------------------------------------------------
    s32 FileSystem::GetLastProfilerTotalUs( void )
    {
      return m_ProfilerLastTotalUs;
    }

#endif // GFL_FILE_DEBUG


  } // namespace fs
} // namespace gfl

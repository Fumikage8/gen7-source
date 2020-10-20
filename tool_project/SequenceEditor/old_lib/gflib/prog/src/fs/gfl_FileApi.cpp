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
     * @brief メインスレッド優先順位
     */
    s32 FileSystem::m_MainThreadPriority = PRIORITY_APP_NORMAL;

    //------------------------------------------------------------------------------------
    // fsライブラリを初期化する
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


    // ROMアーカイブをマウントする
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
    // SDカードアーカイブをマウントする
    void FileSystem::MountSdmcArchive( void )
    {
      nn::Result result = nn::fs::MountSdmc();

      if( result.IsFailure() )
      {
        if( result <= nn::fs::ResultMediaNotFound() ) {
          GFL_PRINT( "SD カードが見つからないか、もしくは認識できません。\n" );
        }
        else if( result <= nn::fs::ResultArchiveInvalidated() ) {
          GFL_PRINT( "マウント中に SD カードが抜かれた可能性があります。\n" );
        }
        else if( result <= nn::fs::ResultMediaAccessError() ) {
          GFL_PRINT( "接触不良などの要因により、SD カードアクセス中にエラーが発生しました。\n" );
        }
        else if( result <= nn::fs::ResultBadFormat() ) {
          GFL_PRINT( "SD カードのフォーマットが不正です。SD カードをフォーマットする必要があります。\n" );
        }
        else {
          GFL_PRINT( "アプリケーション側の不具合、もしくは想定外のエラーです。\n" );
          GFL_ASSERT(0);  //@check
        }
      }
    }
#endif





    //------------------------------------------------------------------------------------
    // fsライブラリを終了する
    //------------------------------------------------------------------------------------
    void FileSystem::Finalize()
    {
      //@note SDK3.3 Finalizeが非公開化 Finalizeは使用しない
#if GFL_FILE_DEBUG
      m_criticalSectionForProfilerAccess.Finalize();
#endif
    }





    //------------------------------------------------------------------------------------
    // fsライブラリが初期化されているかどうかを調べる
    //------------------------------------------------------------------------------------
    bool FileSystem::IsInitialized()
    {
      return nn::fs::IsInitialized();
    }





    //----------------------------------------------------------------------------------------
    /**
     * @brief "パス + ファイル名" の文字列から, "パス" と "ファイル名" をそれぞれ抽出する.
     *
     * @param[in]  src_string      抽出対象の文字列
     * @param[out] dest_path       抽出したパスの格納先バッファ
     * @param[out] dest_file_name  抽出したファイル名の格納先バッファ
     *
     * @retval true   成功
     * @retval false  失敗
     *
     * @note
     * たとえば, "rom:/test_data.bin" の場合,
     * パス名 = "rom:/"
     * ファイル名 = "test_data.bin"
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

      // '/'の位置を検索
      s32 separator_pos = src_length - 1;
      while( 0 < separator_pos ) {
        separator_pos--;
        if( src_string[ separator_pos ] == separator ) {
          break;
        }
      }

      // パス名をコピー
      for( int i=0; i<=separator_pos; i++ ) {
        dest_path[i] = src_string[i];
      }
      dest_path[ separator_pos + 1 ] = end_code;

      // ファイル名をコピー
      u32 filename_len = src_length - separator_pos - 1;
      for( int i=0; i<filename_len; i++ ) {
        dest_file_name[i] = src_string[ separator_pos + 1 + i ];
      }
      dest_file_name[ filename_len ] = end_code;

      return true;
    } 


    //------------------------------------------------------------------------------------
    /**
     * @brief ロムアーカイブ以下にファイルが存在するかどうかを調べる
     * @param filename チェック対象のファイル名( rom:/xxxx )
     * @retval true  ファイルが存在する
     * @retval false ファイルが存在しない
     */
    //------------------------------------------------------------------------------------
    bool FileSystem::CheckRomFileExist( const wchar_t* filename )
    {
      wchar_t dir_name[ MAX_PATH_LENGTH ];
      wchar_t file_name[ MAX_PATH_LENGTH ]; 
      gfl::fs::FileSystem::GetPathAndFileName( filename, dir_name, file_name );

      // ディレクトリを開く
      nn::Result nn_result;
      nn::fs::Directory directory;
      nn_result = directory.TryInitialize( dir_name );
      if( nn_result.IsFailure() ) {
        return false;
      }

      s32 read_count = 0;  // 読み込んだ数
      const s32 max_read_num = 10;  // 一度に読み込む数の上限
      nn::fs::DirectoryEntry entry[ max_read_num ];

      do 
      {
        nn_result = directory.TryRead( &read_count, entry, max_read_num );

        // 読み込んだ項目の中に探しているファイルが含まれているかどうか調べる
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
     *	@brief  ファイルシステム全体の読み込み優先順位を設定
     *	
     *	@param  base_priority           ファイル読み込み優先順位
     *	@param  main_thread_priority    メインスレッドでの、ファイル読み込み優先順位
     *
     *   PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// 利用注意！！！　ドキュメントを良く読むこと
     *   PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// 普通
     *   PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// 低い
     */
    //-----------------------------------------------------------------------------
    void FileSystem::SetPriority( s32 base_priority, s32 main_thread_priority )
    {
      nn::fs::SetPriority( base_priority );
      m_MainThreadPriority = main_thread_priority;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  メインスレッドでのファイル読み込み優先順位を取得
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
    // HIOライブラリを初期化する
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
    // HIOアーカイブをマウントする
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
    // HIOライブラリを終了する
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
     * @brief ホストファイルシステムにファイルが存在するかどうかを調べる
     * @param filename チェック対象のファイル名( フルパスで指定 )( Mountされているアーカイブ名ならば内部でフルパスに置換する )
     * @retval true  ファイルが存在する
     * @retval false ファイルが存在しない
     */
    //------------------------------------------------------------------------------------
    bool FileSystem::CheckHioFileExist( const wchar_t* filename )
    {
      const u32 MAX_PATH_LENGTH = 256;
      wchar_t dir_name[ MAX_PATH_LENGTH ];
      wchar_t file_name[ MAX_PATH_LENGTH ]; 
      gfl::fs::FileSystem::GetPathAndFileName( filename, dir_name, file_name );

      //----------------------------------------------------
      // もし、dir_nameがマウントされているアーカイブ名であれば、
      // ルートディレクトリーに変更
      for( u8 i=0; i<m_hio_mount_count; i++ ) {
        wchar_t archive_name[ MAX_PATH_LENGTH ];
        size_t ret = gfl::std::MultiByteStr2WideStr( archive_name, m_hio_mount_archive_name[i], MAX_PATH_LENGTH );
        GFL_ASSERT( ret != size_t(-1) );  //@check

        size_t len = ::std::wcslen( dir_name ); 
        //TOMOYA_PRINT( "check archive %ls dir_name %ls\n", archive_name, dir_name );
        if( ::std::wcsncmp( archive_name, dir_name, len-1 ) == 0 )  // 最後に/が入っているので、-1している。
        {
          gfl::std::MemCopy( m_hio_mount_root_dir[i], dir_name, sizeof(wchar_t)*MAX_PATH_LENGTH );
          //TOMOYA_PRINT( "dir_name %ls\n", dir_name );
          break;
        }
      }

      // ディレクトリを開く
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

    /// 1フレーム内の読み込み情報のバッファ
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
     *	@brief  プロファイラーの設定
     *
     *	@param	flag      プロファイラーのON、OFF
     *	@param	cp_param  プロファイラーの設定データ
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

    /// プロファイラーが動作中かチェック
    bool FileSystem::IsProfiling( void )
    {
      return m_ProfilerFlag;
    }
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  プロファイラーの更新
     */
    //-----------------------------------------------------------------------------
    void FileSystem::UpdateProfiler( void )
    {
      m_ProfilerLastError   = false;
      m_ProfilerLastTotalUs = 0;
      
      if( !m_ProfilerFlag ){
        return ;
      }

      // プロファイリング結果をチェック
      if( m_ProfilerTotalUs > m_ProfilerParam.main_thread_us ){
        // 許容量を超えたので、ログ出力
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
     *	@brief  １読み込みデータを監視
     *
     *	@param	  path        ファイルパス
     *	@param	  thread_id   スレッドID
     *	@param	  size        サイズ
     *	@param    offset      オフセット
     *	@param    time        読み込みにかかったマイクロ秒
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

      // メインスレッドでの読み込み情報を保存
      if( main_thread ){

        if(m_ProfilerFileCount < PR_FILE_BUFFER_COUNT){

          // メインスレッドでの読み込み内容を保存
          swprintf( m_ProfilerFileNameBuffer[ m_ProfilerFileCount ], PR_FILE_BUFFER_STRNUM, L"%ls", &path );
          m_ProfilerFileSizeBuffer[m_ProfilerFileCount]     = size;
          m_ProfilerFileOffsetBuffer[m_ProfilerFileCount]   = offset;
          m_ProfilerFileUsBuffer[m_ProfilerFileCount]       = time;

          ++m_ProfilerFileCount;
          
        }

        // トータルサイズ
        m_ProfilerTotalSize += size;

        // トータル経過時間
        m_ProfilerTotalUs += time;
      }

      // ログ出力
      switch( m_ProfilerParam.debug_print ){
      case DEBUG_PRINT_MAIN_THREAD:    ///< メインスレッド読み込みを表示
        if( main_thread ){
          GFL_PRINT( "[thread_main [path (%ls)] pos %d size %d time %d us]\n", &path, offset, size, time );
        }
        break;

      case DEBUG_PRINT_OTHER_THREAD:   ///< メインスレッド以外の読み込みを表示
        if( !main_thread ){
          GFL_PRINT( "[thread      [path (%ls)] pos %d size %d time %d us]\n", &path, offset, size, time );
        }
        break;

      case DEBUG_PRINT_ALL:            ///< 全読み込みを表示
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
     *	@brief  最後のプロファイルがエラーだったかチェック
     */
    //-----------------------------------------------------------------------------
    bool FileSystem::IsLastProfilerError( void )
    {
      return m_ProfilerLastError;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  最後のプロファイルがエラーの場合、トータルのマイクロ秒を取得
     */
    //-----------------------------------------------------------------------------
    s32 FileSystem::GetLastProfilerTotalUs( void )
    {
      return m_ProfilerLastTotalUs;
    }

#endif // GFL_FILE_DEBUG


  } // namespace fs
} // namespace gfl

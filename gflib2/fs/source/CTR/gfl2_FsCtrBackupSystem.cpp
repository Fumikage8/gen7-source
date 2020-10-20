#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @file   gfl2_FsCtrBackupSystem.cpp
 * @brief  バックアップシステム
 * @author obata_toshihiro
 * @date   2011.02.16
 */
//==============================================================================
#include <nn.h>

//#include <fs/gfl2_FileApi.h>
#include <str/include/gfl2_StrSys.h>
#include "gfl2_FsCtrBackupThread.h"
#include <fs/include/CTR/gfl2_FsCtrBackupSystem.h>
#include <ro/include/gfl2_RoManager.h>
#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(gfl2::fs::FsCtrBackupSystem);
template class gfl2::base::SingletonAccessor<gfl2::fs::FsCtrBackupSystem>;


namespace gfl2 {
  namespace fs {


    const char* FsCtrBackupSystem::SAVEDATA_ARCHIVE_NAME = "data:";
    const u32 FsCtrBackupSystem::STACK_SIZE_FOR_FORMAT_THREAD = 4096;
    const u32 FsCtrBackupSystem::STACK_SIZE_FOR_LOAD_THREAD = 4096;
    const u32 FsCtrBackupSystem::STACK_SIZE_FOR_SAVE_THREAD = 4096;
    const u32 FsCtrBackupSystem::STACK_SIZE_FOR_CREATE_THREAD = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param heap  システムの作業領域に使用するヒープ
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::FsCtrBackupSystem( gfl2::heap::HeapBase* heap ) :
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
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::~FsCtrBackupSystem( void )
    {
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 共通非同期処理待ち
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::WaitAsync( u32 wait_mode )
    {
      FsCtrBackupThread::Mode mode = static_cast<FsCtrBackupThread::Mode>(wait_mode);
      if( !m_backup_thread )
      {
        GFL_PRINT( "FsCtrBackupSystem::WaitAsync 非同期処理が開始されていません\n" );
        return RESULT_NG_NOT_START;
      }
      else if( m_backup_thread->GetMode() != mode )
      {
        GFL_PRINT("%d!=%d:違う目的のスレッドが動作中です\n", m_backup_thread->GetMode(), mode );
        GFL_ASSERT( 0 ); //@check
        return RESULT_NG_ERROR;
      }
      else if( m_backup_thread->IsAlive() )
      {
        return RESULT_CONTINUE;  // スレッド動作中
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
     * @brief セーブデータ領域をフォーマットする
     *
     * @param max_dir_count   ディレクトリの最大数
     * @param max_file_count  ファイルの最大数
     * @param duplicate       二重化するかどうか
     * @retval true   フォーマットに成功
     * @retval false  フォーマットに失敗
     *
     * @attention 既存のバックアップファイルは削除されます.
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::Format(
        size_t max_dir_count, size_t max_file_count, DuplicateFlag duplicate )
    {
      nn::Result nn_result = nn::fs::FormatSaveData( max_file_count, max_dir_count, duplicate );
      if ( nn_result.IsSuccess() ) return RESULT_OK;

      NN_DBG_PRINT_RESULT( nn_result );
      if ( nn::fs::ResultNotEnoughSpace::Includes(nn_result) )
      { //アーカイブ容量が足りない：引数指定が間違い
        GFL_PRINT("ResultNotEnoughSpace: Program ERROR!!\n");
        GFL_ASSERT( 0 ); //@check
      }
      //フォーマットの失敗は常に致命的
      NN_ERR_THROW_FATAL_ALL( nn_result );
      return RESULT_NG_ERROR;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブデータ領域の非同期フォーマットを開始する
     *
     * @param thread_priority  フォーマットスレッドのプライオリティ
     * @param max_dir_count    ディレクトリの最大数
     * @param max_file_count   ファイルの最大数
     * @param duplicate        二重化するかどうか
     *
     * @retval RESULT_OK       フォーマット開始
     * @retval RESULT_NG_XXXX  フォーマット開始に失敗
     *
     * @attention 既存のバックアップファイルは全て削除されます.
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::StartAsyncFormat( 
        u8 thread_priority,
        size_t max_directories,
        size_t max_files,
        DuplicateFlag duplicate )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      } 

      m_backup_thread = GFL_NEW( m_heap )
        FsCtrBackupThread( m_heap, STACK_SIZE_FOR_FORMAT_THREAD, this, FsCtrBackupThread::MODE_FORMAT );

      FsCtrBackupThread::FormatParam format_param;
      format_param.maxDirectoryCount = max_directories;
      format_param.maxFileCount      = max_files;
      format_param.isDuplicate       = duplicate;
      m_backup_thread->SetFormatParam( format_param );

      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブデータ領域の非同期フォーマットの完了を待つ
     *
     * @retval RESULT_CONTINUE  フォーマット中
     * @retval RESULT_OK        フォーマット終了
     * @retval RESULT_NG_XXXX   フォーマットに失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::WaitAsyncFormat( void )
    {
      return WaitAsync( FsCtrBackupThread::MODE_FORMAT );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルを作成する
     *
     * @param file_path  作成するファイルのパス
     * @param file_size  作成するファイルのサイズ
     * @retval RESULT_OK       ファイルの作成に成功
     * @retval RESULT_NG_XXXX  ファイルの作成に失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::CreateFsCtrBackupFile( const wchar_t* file_path, size_t file_size )
    {
      Result result;

      // マウント
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // ファイルを作成
      result = this->CreateFile( file_path, file_size );
      if( result != RESULT_OK ) {
        this->UnMountSaveData();
        return result;
      }

      // コミット
      this->CommitSaveData();

      // マウントを解除
      result = this->UnMountSaveData();
      return result;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::StartAsyncCreateFile( u8 thread_priority, const wchar_t* file_path, size_t file_size )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      } 
      struct FsCtrBackupThread::CreateParam cparam;
      cparam.filePath = file_path;
      cparam.fileSize = file_size;
      m_backup_thread = GFL_NEW( m_heap )
        FsCtrBackupThread( m_heap, STACK_SIZE_FOR_CREATE_THREAD, this, FsCtrBackupThread::MODE_CREATE );
      m_backup_thread->SetCreateParam( cparam );
      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::WaitAsyncCreateFile( void )
    {
      return WaitAsync( FsCtrBackupThread::MODE_CREATE );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブデータアーカイブをマウントする
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::MountSaveData( void )
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
          // メディアが存在しません
          return RESULT_NG_MEDIA_NOT_EXIST;
        }
        else if( nn_result <= nn::fs::ResultNotFormatted() ) {
          return RESULT_NG_NOT_FORMATTED;
          // フォーマットを行なう必要があります。
          // ( 理由 ) セーブデータ領域がフォーマットされていない状態です。
          // ( 補足 ) セーブデータ領域を初めてマウントするときは、
          //          必ずフォーマットが必要になります。
        }
        else if(nn_result <= nn::fs::ResultBadFormat()) {
          return RESULT_NG_BAD_FORMAT;
          // フォーマットを行なう必要があります。
          // ( 理由 ) ファイルフォーマットが不正な状態になっています。
        }
        else if(nn_result <= nn::fs::ResultVerificationFailed()) {
          return RESULT_NG_BROKEN_DATA;
          // フォーマットを行なう必要があります。
          // ( 理由 ) セーブデータが壊れているか、改竄されています。
          // ( 補足 ) 二重化されている場合は、ユーザ操作でこの状態になることはありません。
          //          二重化されていない場合は、
          //          ユーザ操作( 書き込み中の電源断やカード抜かれ )でもこのエラーは発生します。
        }
        else if(nn_result <= nn::fs::ResultOperationDenied()) {
          return RESULT_NG_ERROR;
          // 基本的に、このエラーは発生しないため、ハンドリングは必須ではありません。
          // ( 補足 ) 但し、接触不良等によるハードウェエア的な要因により失敗した場合、
          //          nn::fs::ResultMediaAccessError が返ります。
          //          そのときは、リトライ処理、ゲームカードの挿し直し、
          //          本体の再起動などで復帰する可能性があります。
        }
        else {
          GFL_ASSERT_STOP(0); //@check 上記以外のエラーは発生しません。
          return RESULT_NG_UNKNOWN;
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルを作成する
     * @param file_path  作成するファイルのパス
     * @param file_size  作成するファイルのサイズ
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::CreateFile( const wchar_t* file_path, size_t file_size )
    {
      nn::Result nn_result = nn::fs::TryCreateFile( file_path, file_size );
      return GetSystemResult( nn_result );
    }
    //-------------------------------------------------------------------------

    /**
     * @brief SDKのファイルアクセスの結果から, システムとしての実行結果を取得する
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::GetSystemResult( nn::Result nn_result )
    {
      FsCtrBackupFile::Result file_result = FsCtrBackupFile::GetAccessResult( nn_result );
      return GetSystemResult( file_result );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルアクセスの結果から, システムとしての実行結果を取得する
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::GetSystemResult( FsCtrBackupFile::Result file_result )
    {
      Result result = RESULT_NG_UNKNOWN;
      switch( file_result ) {
      case FsCtrBackupFile::ACCESS_RESULT_OK:
        result = RESULT_OK;
        break;
      case FsCtrBackupFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:
        result = RESULT_NG_PATH_NOT_EXIST;
        break;
      case FsCtrBackupFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:
        result = RESULT_NG_PATH_ALREADY_EXIST;
        break;
      case FsCtrBackupFile::ACCESS_RESULT_NG_BAD_FORMAT:
        result = RESULT_NG_BAD_FORMAT;
        break;
      case FsCtrBackupFile::ACCESS_RESULT_NG_BROKEN_DATA:
        result = RESULT_NG_BROKEN_DATA;
        break;
      case FsCtrBackupFile::ACCESS_RESULT_NG_ERROR:
        result = RESULT_NG_ERROR;
        break;
      case FsCtrBackupFile::ACCESS_RESULT_NG_LACK_OF_SPACE:
        result = RESULT_NG_LACK_OF_SPACE;
        break;
      default: result = RESULT_NG_UNKNOWN; break;
      }
      return result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブデータの変更をコミットする
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::CommitSaveData( void )
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
        GFL_PRINT( "WARNING! コミット失敗\n" );
        GFL_PRINT( "セーブデータアーカイブはマウントされていません.\n" );
        return RESULT_NG_UNMOUNT;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "想定外のエラーです。\n" );  //@check
        return RESULT_NG_UNKNOWN;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブデータアーカイブのマウントを解除する
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::UnMountSaveData( void )
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
        GFL_PRINT( "WARNING! アンマウント失敗\n" );
        GFL_PRINT( "セーブデータアーカイブはマウントされていません.\n" );
        return RESULT_NG_UNMOUNT;
      }
      else
      {
        NN_DBG_PRINT_RESULT( nn_result );
        GFL_ASSERT_MSG( 0, "想定外のエラーです。\n" );  //@check
        return RESULT_NG_UNKNOWN;
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルを削除する
     *
     * @param file_path  削除するファイルのパス
     *
     * @retval RESULT_OK       ファイルの削除に成功
     * @retval RESULT_OK 以外  ファイルの削除に失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::DeleteFsCtrBackupFile( const wchar_t* file_path )
    {
      Result result;

      // マウント
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // ファイルを削除
      result = this->DeleteFile( file_path );
      if( result != RESULT_OK ) {
        this->UnMountSaveData();
        return result;
      }

      // コミット
      this->CommitSaveData();

      // マウントを解除
      result = this->UnMountSaveData();
      return result;
    }

    /**
     * @brief ファイルを削除する
     * @param file_path  削除するファイルのパス
     */
    FsCtrBackupSystem::Result FsCtrBackupSystem::DeleteFile( const wchar_t* file_path )
    {
      nn::Result nn_result = nn::fs::TryDeleteFile( file_path );
      FsCtrBackupFile::Result access_result = FsCtrBackupFile::GetAccessResult( nn_result );
      return GetSystemResult( access_result );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルの有無を調べる
     *
     * @param file_path   調べるファイルのパス
     *
     * @retval true   ファイルは存在する
     * @retval false  ファイルは存在しない
     */
    //-------------------------------------------------------------------------
    bool FsCtrBackupSystem::CheckFsCtrBackupFileExist( const wchar_t* file_path )
    {
      bool found = false;
      this->MountSaveData();

      nn::Result nn_result;
      nn::fs::Directory directory;
      wchar_t directory_name[ MAX_FILE_PATH_LENGTH ];
      wchar_t file_name[ MAX_FILE_PATH_LENGTH ];

      GetPathAndFileName( file_path, directory_name, file_name );
      nn_result = directory.TryInitialize( directory_name );
      if( nn_result.IsSuccess() )
      {
        s32 read_entry_num = 0;  // 読み込んだ数
        do {
          const s32 max_read_entry_num = 10;  // 一度に読み込む数の上限
          nn::fs::DirectoryEntry entry[ max_read_entry_num ];
          nn_result = directory.TryRead( &read_entry_num, entry, max_read_entry_num );
          if( nn_result.IsSuccess() )
          {
            // 読み込んだ項目の中に, 探しているファイルが含まれているかどうか調べる
            for( int i=0; i<read_entry_num; i++ ) {
              if( gfl2::str::StrComp( file_name, entry[i].entryName ) == true ) {
                found = true;
                break;
              }
            }
          }
          else
          {
            NN_DBG_PRINT_RESULT( nn_result );
            break; // 失敗したら中断
          }
        } while( ( found == false ) && ( 0 < read_entry_num ) );
      }

      this->UnMountSaveData();
      return found;
    }









    //-------------------------------------------------------------------------
    /**
     * @brief 操作対象のバックアップデータを登録する
     * @param[in]     file_path  読み書きするファイルへのパス
     * @param[in,out] data       書き込むデータ or 読み込んだデータの格納先
     * @param[in]     datasize   書き込む or 読み込むデータのサイズ
     * @retval true   登録成功
     * @retval false  登録失敗
     */
    //-------------------------------------------------------------------------
    bool FsCtrBackupSystem::AddBackupEntry(
        const wchar_t* file_path, void* data, u32 datasize )
    {
      if( this->IsBackupEntryFull() )
      {
        GFL_ASSERT_MSG( 0, "既に最大数まで登録済みです。\n" ); //@check
        return false;
      }
      else
      {
        gfl2::str::StrNCopy( m_backup_entries[ m_backup_entry_count ].file_path, file_path , MAX_FILE_PATH_LENGTH );
        m_backup_entries[ m_backup_entry_count ].data = data;
        m_backup_entries[ m_backup_entry_count ].datasize = datasize;
        m_backup_entry_count++;
        return true;
      }
    }

    /**
     * @brief バックアップデータが最大数まで登録済みかどうかを調べる
     * @retval true   最大数まで登録済み
     * @retval false  空き要素がある
     */
    bool FsCtrBackupSystem::IsBackupEntryFull( void ) const
    {
      return ( MAX_BACKUP_ENTRY_NUM <= m_backup_entry_count );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 登録されているバックアップ操作対象をクリアする
     */
    //-------------------------------------------------------------------------
    void FsCtrBackupSystem::ClearBackupEntry( void )
    {
      m_backup_entry_count = 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブ・ロード対象のファイル数を取得する
     */
    //-------------------------------------------------------------------------
    u32 FsCtrBackupSystem::GetBackupEntryCount( void ) const
    {
      return m_backup_entry_count;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブ・ロードが完了したファイルの数を取得する
     */
    //-------------------------------------------------------------------------
    u32 FsCtrBackupSystem::GetBackupFinishedCount( void ) const
    {
      return m_backup_finish_count;
    }









    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルを読み込む
     *
     * @retval RESULT_OK       読み込み成功
     * @retval RESULT_NG_XXXX  読み込み失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::Load( void )
    {
      Result result = RESULT_OK;
      m_backup_finish_count = 0;

      // マウント
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // 読み込み
      for( u32 i=0; i<m_backup_entry_count; i++ )
      {
        const BackupEntry* entry = &m_backup_entries[i];
        bool open_result = m_file.Open( entry->file_path, FsCtrBackupFile::OPEN_MODE_READ );
        if( !open_result ) {  // オープン失敗
          FsCtrBackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        size_t read_size = m_file.Read( entry->data, entry->datasize );
        if( read_size != entry->datasize ) {  // 読み込み失敗
          FsCtrBackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        m_backup_finish_count++;
      }

      // マウントを解除
      Result unmount_result = this->UnMountSaveData();
      return ( result == RESULT_OK ) ? unmount_result : result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルの非同期読み込みを開始する
     * @param thread_priority  バックアップスレッドのプライオリティ
     * @retval RESULT_OK       読み込み開始
     * @retval RESULT_NG_XXXX  読み込み開始に失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::StartAsyncLoad( u8 thread_priority )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      }
      m_backup_thread = GFL_NEW( m_heap )
        FsCtrBackupThread( m_heap, STACK_SIZE_FOR_LOAD_THREAD, this, FsCtrBackupThread::MODE_LOAD );
      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルの非同期読み込みの完了を待つ
     * @retval RESULT_CONTINUE  読み込み中
     * @retval RESULT_OK        読み込み終了
     * @retval RESULT_NG_XXXX   読み込みに失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::WaitAsyncLoad( void )
    {
      return WaitAsync( FsCtrBackupThread::MODE_LOAD );
    }








    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルに書き込む
     * @retval RESULT_OK       書き込み成功
     * @retval RESULT_NG_XXXX  書き込み失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::Save( void )
    {
      Result result = RESULT_OK;
      m_backup_finish_count = 0;

      // マウント
      result = this->MountSaveData();
      if( result != RESULT_OK ) {
        return result;
      }

      // 書き込み
      for( u32 i=0; i<m_backup_entry_count; i++ )
      {
        const BackupEntry* entry = &m_backup_entries[i];
        bool open_result = m_file.Open( entry->file_path, FsCtrBackupFile::OPEN_MODE_WRITE_NEW );
        if( !open_result ) {  // オープン失敗
          FsCtrBackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        size_t written_size = m_file.Write( entry->data, entry->datasize );
        if( written_size != entry->datasize ) {  // 書き込み失敗
          FsCtrBackupFile::Result file_result = m_file.GetLastAccessResult();
          result = GetSystemResult( file_result );
          break;
        }
        m_backup_finish_count++;
      }

      // コミット
      if( result == RESULT_OK ) {
        result = this->CommitSaveData();
      }

      // マウント解除
      Result unmount_result = this->UnMountSaveData();
      return ( result == RESULT_OK ) ? unmount_result : result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルへの非同期書き込みを開始する
     * @param thread_priority  バックアップスレッドのプライオリティ
     * @retval RESULT_OK       書き込み開始
     * @retval RESULT_NG_XXXX  書き込み開始に失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::StartAsyncSave( u8 thread_priority )
    {
      if( m_backup_thread ) {
        return RESULT_NG_ALREADY;
      }
      m_backup_thread = GFL_NEW( m_heap )
        FsCtrBackupThread( m_heap, STACK_SIZE_FOR_SAVE_THREAD, this, FsCtrBackupThread::MODE_SAVE );
      m_backup_thread->Start( thread_priority );
      return RESULT_OK;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルへの非同期書き込みの完了を待つ
     *
     * @retval RESULT_CONTINUE  書き込み中
     * @retval RESULT_OK        書き込み終了
     * @retval RESULT_NG_XXXX   書き込みに失敗
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::WaitAsyncSave( void )
    {
      return WaitAsync( FsCtrBackupThread::MODE_SAVE );
    }








    //-------------------------------------------------------------------------
    /**
     * @brief 空き容量を取得する
     * @param[out] free_bytes  空き容量の格納先
     * @retval RESULT_OK       成功。free_bytes に有効な値が格納されます。
     * @retval RESULT_NG_XXXX  失敗。free_bytes は有効な値ではありません。
     */
    //-------------------------------------------------------------------------
    FsCtrBackupSystem::Result FsCtrBackupSystem::GetFreeBytes( s64* free_bytes ) const
    {
      nn::Result nn_result =
        nn::fs::GetArchiveFreeBytes( free_bytes, FsCtrBackupSystem::SAVEDATA_ARCHIVE_NAME );

      if( nn_result.IsSuccess() )
      {
        return RESULT_OK;
      }
      else if( nn_result <= nn::fs::ResultNotFound() )
      {
        GFL_PRINT( "WARNING! 空き容量の取得に失敗\n" );
        GFL_PRINT( "セーブデータアーカイブはマウントされていません.\n" );
        return RESULT_NG_UNMOUNT;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "想定外のエラーです。\n" ); //@check
        return RESULT_NG_UNKNOWN;
      }
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
    bool FsCtrBackupSystem::GetPathAndFileName( 
        const wchar_t* src_string,
        wchar_t* dest_path,
        wchar_t* dest_file_name )
    {
      const wchar_t separator = L'/';
      const wchar_t end_code = L'\0';
      const size_t src_length = gfl2::str::StrLen( src_string );
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


  }  // namespace fs
}  // namespace gfl
#endif

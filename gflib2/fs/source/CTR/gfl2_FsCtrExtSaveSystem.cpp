#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @file   gfl_ExtSaveSystem.cpp
 * @brief  拡張データ領域アクセス
 * @author tamada
 * @date   2012.09.11
 *
 * @note 
 */
//=============================================================================

#include <fs/include/CTR/gfl2_FsCtrExtSaveSystem.h>
#include <fs/source/CTR/gfl2_FsCtrExtSaveThread.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 {
namespace fs {
//=============================================================================
//=============================================================================
const char * ExtSaveSystem::MOUNT_NAME_SCHAR = "exdt:";
const wchar_t * ExtSaveSystem::MOUNT_NAME_WCHAR = L"exdt:";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  ExtSaveSystem::ExtSaveSystem( gfl2::heap::HeapBase * pHeap ) :
    m_extSaveDataId(0),
    m_entryDirectory(0),
    m_entryFile(0),
    m_thread(NULL),
    m_thread_mode(THREAD_MODE_NOTHING),
    m_thread_heap(NULL),
    m_thread_use_path(),
    m_thread_use_size(0),
    //m_temp_nn_result(0),
    m_thread_result(RESULT_OK),
    m_thread_file_result(ExtSaveFile::ACCESS_RESULT_OK)

  {
    initThreadParams();
  }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  ExtSaveSystem::~ExtSaveSystem()
  {
  }

//-----------------------------------------------------------------------------
/**
 */
//-----------------------------------------------------------------------------
  void ExtSaveSystem::Initialize( 
      nn::fs::ExtSaveDataId id,
      const void * iconData,
      size_t  iconDataSize,
      u32 entryDirectory,
      u32 entryFile
      )
  {
    m_extSaveDataId   = id;
    m_entryDirectory  = entryDirectory;
    m_entryFile       = entryFile;
  }

  void ExtSaveSystem::Initialize(
      nn::fs::ExtSaveDataId id,
      u32 entryDirectory,
      u32 entryFile
      )
  {
    m_extSaveDataId   = id;
    m_entryDirectory  = entryDirectory;
    m_entryFile       = entryFile;
  }

//=============================================================================
//
//
//              同期処理API
//
//
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータのマウント
 *
 * @note  存在しない場合、内部でフォーマットも行う
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::Mount( void )
{
  m_last_result = nn::fs::MountExtSaveData(MOUNT_NAME_SCHAR, m_extSaveDataId);
  if ( !m_last_result.IsFailure() ) return RESULT_OK;

  //初回マウントエラーの場合
  Result mount_res = CheckMountError( m_last_result );
  //GFL_PRINT("ExtSaveSystem::MountResult=%d\n", mount_res );
  return mount_res;
}

//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータのマウント
 *
 * @note  存在しない場合、内部でフォーマットも行う
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::MountAndFormat( const void * iconBuffer, size_t iconSize )
{
  m_last_result = nn::fs::MountExtSaveData(MOUNT_NAME_SCHAR, m_extSaveDataId);
  if ( !m_last_result.IsFailure() ) return RESULT_OK;

  //初回マウントエラーの場合
  Result mount_res = CheckMountError( m_last_result );
  //GFL_PRINT("ExtSaveSystem::MountResult=%d\n", mount_res );
  if ( mount_res == RESULT_NG_NOT_FOUND )
  { //存在していない→Createする
    m_last_result = nn::fs::CreateExtSaveData( m_extSaveDataId, iconBuffer, iconSize, m_entryDirectory, m_entryFile );
  }
  else if ( mount_res == RESULT_NG_VERIFICATION_FAILED || mount_res == RESULT_NG_NOT_FORMATTED )
  { //壊れているor改ざんされたor前回作成失敗→Delete＆Createする
    m_last_result = nn::fs::DeleteExtSaveData( m_extSaveDataId );
    if(m_last_result.IsSuccess())
    {
      m_last_result = nn::fs::CreateExtSaveData( m_extSaveDataId, iconBuffer, iconSize, m_entryDirectory, m_entryFile );
    }
  }
  else
  { //上記以外→失敗終了
    return mount_res;
  }

  // 作成後の判定 → 作成失敗なら終了
  Result create_res = CheckCreateError( m_last_result );
  //GFL_PRINT("ExtSaveSystem::CreateResult=%d\n", create_res );
  if ( create_res != RESULT_OK ) return create_res;

  {
    // 再マウント
    m_last_result = nn::fs::MountExtSaveData(MOUNT_NAME_SCHAR, m_extSaveDataId);
    if(m_last_result.IsFailure())
    {
      // 始めの呼び出し時と同様。
      // (補足) SD カードが抜かれない限り、作成直後にマウントが失敗することは、まずありません。
      mount_res = CheckMountError( m_last_result );
      //GFL_PRINT("ExtSaveSystem::MountResult=%d\n", mount_res );
      return mount_res;
    }
  }

  return RESULT_OK;
}
//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータのマウント解除
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::Unmount( void )
{
  m_last_result = nn::fs::Unmount( GetMountName() );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
    if ( m_last_result <= nn::fs::ResultNotFound() )
    {
      return RESULT_NG_NOT_FOUND;
    }
    return RESULT_NG_UNKNOWN;
  }
  return RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータ領域の情報を取得する
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::GetArchiveInfo( u32 * entryDirectory, u32 * entryFile )
{
  m_last_result = nn::fs::GetExtSaveDataFormatInfo( entryDirectory, entryFile, m_extSaveDataId );
  if ( m_last_result.IsFailure() )
  {
    return CheckInfoError( m_last_result );
  }
  return RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータ領域の削除
 *
 * @return  システムAPI操作結果
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::DeleteArchive( void )
{
  m_last_result = nn::fs::DeleteExtSaveData( m_extSaveDataId );
  if ( m_last_result.IsFailure() )
  {
    return CheckDeleteError( m_last_result );
  }
  return RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @brief ディレクトリ内の情報を読み出す
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::ReadDirectory(
    gfl2::heap::HeapBase * pHeap, const wchar_t * dir_path,
    s32 * pReadNum, nn::fs::DirectoryEntry * pEntries, s32 numEntries )
{
  nn::fs::Directory * dir = GFL_NEW( pHeap ) nn::fs::Directory();
  m_last_result = dir->TryInitialize( dir_path );
  if ( m_last_result.IsSuccess() )
  {
    m_last_result = dir->TryRead( pReadNum, pEntries, numEntries );
  }
  dir->Finalize();
  GFL_DELETE dir;
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ディレクトリ作成
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::CreateDirectory( const wchar_t* dir_path )
{
  GFL_PRINT("ExtSaveSystem::CreateDirectory()\n");

  m_last_result = nn::fs::TryCreateDirectory( dir_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ディレクトリ削除
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::DeleteDirectory( const wchar_t* dir_path )
{
  GFL_PRINT("ExtSaveSystem::DeleteDirectory()\n");

  m_last_result = nn::fs::TryDeleteDirectory( dir_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief     ディレクトリ名変更
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::RenameDirectory( const wchar_t* current_path, const wchar_t* new_path )
{
  GFL_PRINT("ExtSaveSystem::RenameDirectory()\n");
  m_last_result = nn::fs::TryRenameDirectory( current_path, new_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル作成
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::CreateFile( const wchar_t* file_path, s64 size )
{
  GFL_PRINT("ExtSaveSystem::CreateFile()\n");
  GFL_ASSERT_STOP( size >= 0 );
  m_last_result = nn::fs::TryCreateFile( file_path, size );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
    GFL_PRINT("CreateFileError\n");
    //すでに存在する→なにもせず戻る
    if ( nn::fs::ResultAlreadyExists::Includes(m_last_result) )
    {
      return ExtSaveFile::GetAccessResult( m_last_result );
    }

    //エラーの場合、ごみファイルが残っている可能性がある→削除
    nn::Result delete_nn_result = nn::fs::TryDeleteFile( file_path );
    ExtSaveFile::Result delete_result;
    delete_result = ExtSaveFile::GetAccessResult(delete_nn_result);
    if ( delete_result == ExtSaveFile::ACCESS_RESULT_OK )
    { // 成功した場合、ゴミファイルが消える
      GFL_PRINT("ExtSaveSystem:: Delete Dust File\n");
    }
    else if ( delete_result == ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST )
    { // ResultNotFoundの場合はごみファイルがなかった。
      GFL_PRINT("ExtSaveSystem:: Dust File Not Exists\n");
    }
    else
    { // それ以外はなにかのエラー。
      NN_DBG_PRINT_RESULT( delete_nn_result );
      GFL_PRINT("DeleteFileError\n");
    }
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief ファイル作成（強制上書きあり）
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::CreateFileOW( const wchar_t* file_path, s64 size )
{
  ExtSaveFile::Result create_1st_result;
  create_1st_result = CreateFile( file_path, size );
  if (create_1st_result != ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST )
  {
    //「ResultAlreadyExists」以外の場合、そのまま返す
    return create_1st_result;
  }
  //ファイルが存在していて失敗した場合、削除する
  ExtSaveFile::Result delete_result = DeleteFile( file_path );
  if ( delete_result != ExtSaveFile::ACCESS_RESULT_OK 
      && delete_result != ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST )
  {
    return delete_result;
  }
  //ACCESS_RESULT_NG_PATH_NOT_EXIST：メタデータに存在しないが実際には存在した場合。
  //→エラーだが削除はされている。作成を試みる
  //ファイルの削除に成功したら、再度作成してみる
  return CreateFile( file_path, size );

}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル削除
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::DeleteFile( const wchar_t* file_path )
{
  GFL_PRINT("ExtSaveSystem::DeleteFile()\n");
  m_last_result = nn::fs::TryDeleteFile( file_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル名変更
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::RenameFile( const wchar_t* current_path, const wchar_t* new_path )
{
  GFL_PRINT("ExtSaveSystem::RenameFile()\n");
  m_last_result = nn::fs::TryRenameFile( current_path, new_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル読み込み
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::ReadFile( const wchar_t* file_path, void * dst, size_t size )
{
  GFL_PRINT("ExtSaveSystem::ReadFile()\n");
  gfl2::fs::ExtSaveFile file;
  file.Open(file_path, ExtSaveFile::OPEN_MODE_READ);
  gfl2::fs::ExtSaveFile::Result access_result;
  access_result = file.GetLastAccessResult();
  m_last_result = file.GetLastAccessNNResult();
  if ( access_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
  {
    size_t filesize = file.GetSize();
    if ( filesize < size )
    {
      GFL_PRINT("ExtSaveSystem:: filesize(%d)<read_size(%d)\n", filesize, size );
      size = filesize;
    }
    file.Read( dst, size );
    file.Close();
    access_result = file.GetLastAccessResult();
    m_last_result = file.GetLastAccessNNResult();
  }
  return access_result;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル書込み
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::WriteFile( const wchar_t* file_path, const void * src, size_t size )
{
  GFL_PRINT("ExtSaveSystem::WriteFile()\n");
  gfl2::fs::ExtSaveFile file;
  file.Open(file_path, ExtSaveFile::OPEN_MODE_WRITE_ADD);
  gfl2::fs::ExtSaveFile::Result access_result;
  access_result = file.GetLastAccessResult();
  m_last_result = file.GetLastAccessNNResult();
  if ( access_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
  {
    size_t filesize = file.GetSize();
    if ( filesize < size )
    {
      GFL_PRINT("ExtSaveSystem:: filesize(%d)<write_size(%d)\n", filesize, size );
      size = filesize;
    }
    file.Write( src, size );
    file.Close();
    access_result = file.GetLastAccessResult();
    m_last_result = file.GetLastAccessNNResult();
  }
  return access_result;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル存在チェック
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::IsFileExists( const wchar_t* file_path, bool *exist_flag )
{
  GFL_PRINT("ExtSaveSystem::IsFileExists()\n");
  gfl2::fs::ExtSaveFile file;
  *exist_flag  = (file.Open(file_path, ExtSaveFile::OPEN_MODE_READ) == true );
  gfl2::fs::ExtSaveFile::Result access_result;
  access_result = file.GetLastAccessResult();
  m_last_result = file.GetLastAccessNNResult();
  if ( access_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
  {
    file.Close();
    access_result = file.GetLastAccessResult();
    m_last_result = file.GetLastAccessNNResult();
  }
  else if ( access_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA )
  { // nn::fs::ResultVerificationFailed(==ACCESS_RESULT_NG_BROKEN_DATA)の場合は
    // ファイルOpenが失敗ではあるが、存在はしている
    *exist_flag = true;
  }
  return access_result;
}

//=============================================================================
//
//
//              非同期リクエスト
//
//
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncMountAndFormat( gfl2::heap::HeapBase * pHeap, u8 priority, const void * iconBuffer, size_t iconSize )
{
  if ( m_thread ) return false;
  initThreadParams();
  m_thread_icon_buffer  = iconBuffer;
  m_thread_icon_size    = iconSize;
  this->startThread( pHeap, priority, THREAD_MODE_MOUNT_AND_FORMAT );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncMount( gfl2::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_MOUNT );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncUnmount( gfl2::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_UNMOUNT );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncGetArchiveInfo( gfl2::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_GET_ARCHIVE_INFO );
  return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncDeleteArchive( gfl2::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_DELETE_ARCHIVE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncReadDirectory( gfl2::heap::HeapBase * pHeap,
    u8 priority, const wchar_t * dir_path, s32 * pReadNum,
    nn::fs::DirectoryEntry * pEntries, s32 numEntries )
{
  if ( m_thread ) return false;
  initThreadParams();
  m_thread_read_num     = pReadNum;
  m_thread_num_entries  = numEntries;
  m_thread_entries      = pEntries;
  setPath( m_thread_use_path, dir_path );
  this->startThread( pHeap, priority, THREAD_MODE_READ_DIRECTORY );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncCreateFile(
    gfl2::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, s64 size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_CREATE_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncCreateFileOW(
    gfl2::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, s64 size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_CREATE_FILE_OW );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncDeleteFile(
    gfl2::heap::HeapBase * pHeap, u8 priority, const wchar_t * file_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  this->startThread( pHeap, priority, THREAD_MODE_DELETE_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncRenameFile(
    gfl2::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * current_path, const wchar_t * new_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, current_path );
  setPath( m_thread_use_path2, new_path );
  this->startThread( pHeap, priority, THREAD_MODE_RENAME_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncReadFile(
    gfl2::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, void * dst, size_t size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_dst_buffer = dst;
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_READ_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncWriteFile(
    gfl2::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, const void * src, size_t size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_src_buffer = src;
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_WRITE_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncIsFileExists(
    gfl2::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  this->startThread( pHeap, priority, THREAD_MODE_IS_FILE_EXISTS );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncCreateDirectory( gfl2::heap::HeapBase * pHeap, u8 priority, const wchar_t * dir_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, dir_path );
  this->startThread( pHeap, priority, THREAD_MODE_CREATE_DIRECTORY );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncDeleteDirectory( gfl2::heap::HeapBase * pHeap, u8 priority, const wchar_t * dir_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, dir_path );
  this->startThread( pHeap, priority, THREAD_MODE_DELETE_DIRECTORY );
  return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncRenameDirectory(
    gfl2::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * current_path, const wchar_t * new_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, current_path );
  setPath( m_thread_use_path2, new_path );
  this->startThread( pHeap, priority, THREAD_MODE_RENAME_DIRECTORY );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   スレッド終了チェック
 */
//-----------------------------------------------------------------------------
bool ExtSaveSystem::IsAsyncFinished( void )
{
  if ( !m_thread ) return true;
  if ( m_thread->IsAlive() ) return false;

  GFL_DELETE m_thread;
  m_thread = NULL;
  m_thread_mode = THREAD_MODE_NOTHING;
  return true;
}
//-----------------------------------------------------------------------------
/*
 * @brief   システム操作APIの同期終了待ち
 */
//-----------------------------------------------------------------------------
bool ExtSaveSystem::WaitSystemAPI( ExtSaveSystem::Result * result )
{
  if (IsAsyncFinished())
  {
    *result = m_thread_result;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル操作APIの同期終了待ち
 */
//-----------------------------------------------------------------------------
bool ExtSaveSystem::WaitFileAPI( ExtSaveFile::Result * result )
{
  if (IsAsyncFinished())
  {
    *result = m_thread_file_result;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::GetLastFileResult( void ) const
{
  return m_thread_file_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::GetLastSystemResult( void ) const
{
  return m_thread_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nn::Result ExtSaveSystem::GetLastNNResult( void ) const
{
  return m_last_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ExtSaveSystem::GetArchiveInfoResult( u32 * entryDirectory, u32 * entryFile )
{
  *entryDirectory = m_thread_dir_count_result;
  *entryFile      = m_thread_file_count_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ExtSaveSystem::GetFileExistsResult( bool * exists_flag )
{
  *exists_flag = m_thread_use_bool;
}

//=============================================================================
//
//
//        非公開部分
//
//
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief 製品版に残してはいけないエラーのチェック
 */
//-----------------------------------------------------------------------------
 ExtSaveSystem::Result ExtSaveSystem::CheckProgrammersError( nn::Result result )
 {
   GFL_ASSERT_STOP ( nn::fs::ResultOutOfResource::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultAccessDenied::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultInvalidArgument::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultNotInitialized::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultAlreadyInitialized::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultUnsupportedOperation::Includes( result ) );
   return RESULT_OK;
 }

//-----------------------------------------------------------------------------
/**
 * @brief マウント時のエラーチェック
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckMountError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result <= nn::fs::ResultNotFound())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD カードが挿さっていません。
        // (補足) 壊れた SD カードや SD カードではないメディアが挿されているときにも、このエラーが返ります。
        //        但し、その場合は RegisterSdmcInsertedEvent で登録した挿入イベントはシグナルされます。
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else
      {
        // 指定した拡張セーブデータは存在していないので、作成する必要があります。
        // (補足) 開発中は、指定した ID が正しいか確認をしてください。
        return RESULT_NG_NOT_FOUND;
      }
    }
    else if(result <= nn::fs::ResultNotFormatted())
    {
      // 拡張セーブデータを作り直す必要があります。
      // (理由) 拡張セーブデータの作成に失敗した状態です。
      return RESULT_NG_NOT_FORMATTED;
    }
    else if(result <= nn::fs::ResultBadFormat())
    {
      // SD カードをフォーマットする必要があります。
      return RESULT_NG_BAD_FORMAT;
    }
    else if(result <= nn::fs::ResultVerificationFailed())
    {
      // 拡張セーブデータを作り直す必要があります。
      // (理由) セーブデータが壊れているか、改竄されています。
      return RESULT_NG_VERIFICATION_FAILED;
    }
    else if(result <= nn::fs::ResultOperationDenied())
    {
      if(result <= nn::fs::ResultWriteProtected())
      {
        // SD カードが書き込み禁止にされています。
        // (補足) マウント時にデータの復旧シーケンスが実行されることがあり、そのときに発生します。
        return RESULT_NG_WRITE_PROTECTED;
      }
      else if(result <= nn::fs::ResultMediaAccessError())
      {
        // 接触不良等によるハードウェア的な要因のときのみ、このエラーが返ります。
        // そのときは、SD カードの挿し直し、本体の再起動などで復帰する可能性があります。
        return RESULT_NG_ERROR;
      }
      else
      {
        // SD カード上のファイル、もしくはディレクトリが読み取り専用にされている可能性があります。
        // (補足) マウント時にデータの復旧シーケンスが実行されることがあり、そのときに発生します。
        return RESULT_NG_WRITE_PROTECTED;
      }
    }
    else
    {
      // 上記以外は、想定外のエラーです。この拡張セーブデータ、もしくは SD カードは認識できませんでした。
      // NN_ERR_THROW_FATAL 系のマクロは呼び出さないようにしてください。
      CheckProgrammersError( result );
      //GFL_ASSERT_STOP( 0 );
      return RESULT_NG_UNKNOWN;
    }
  }

//-----------------------------------------------------------------------------
/**
 * @brief     拡張セーブデータ削除時のエラーチェック
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckDeleteError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result <= nn::fs::ResultNotFound())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD カードが挿さっていません。
        // (補足) 壊れた SD カードや SD カードではないメディアが挿されているときにも、このエラーが返ります。
        //        但し、その場合は RegisterSdmcInsertedEvent で登録した挿入イベントはシグナルされます。
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else
      {
        // 指定した拡張セーブデータは存在していないので、作成する必要があります。
        // (補足) 開発中は、指定した ID が正しいか確認をしてください。
        return RESULT_NG_NOT_FOUND;
      }
    }
    else if(result <= nn::fs::ResultBadFormat())
    {
      // SD カードをフォーマットする必要があります。
      return RESULT_NG_BAD_FORMAT;
    }
    else if(result <= nn::fs::ResultOperationDenied())
    {
      if(result <= nn::fs::ResultWriteProtected())
      {
        // SD カードが書き込み禁止にされています。
        // (補足) マウント時にデータの復旧シーケンスが実行されることがあり、そのときに発生します。
        return RESULT_NG_WRITE_PROTECTED;
      }
      else if(result <= nn::fs::ResultMediaAccessError())
      {
        // 接触不良等によるハードウェア的な要因のときのみ、このエラーが返ります。
        // そのときは、SD カードの挿し直し、本体の再起動などで復帰する可能性があります。
        return RESULT_NG_ERROR;
      }
      else
      {
        //指定した拡張セーブデータは現在マウント中のため、操作は途中で失敗しました。 
        return RESULT_NG_MOUNTING;
      }
    }
    else
    {
      // 上記以外は、想定外のエラーです。この拡張セーブデータ、もしくは SD カードは認識できませんでした。
      // NN_ERR_THROW_FATAL 系のマクロは呼び出さないようにしてください。
      CheckProgrammersError( result );
      //GFL_ASSERT_STOP( 0 );
      return RESULT_NG_UNKNOWN;
    }
  }


//-----------------------------------------------------------------------------
/**
 * @brief   拡張セーブデータ領域作成時のエラーチェック
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckCreateError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result.IsFailure())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD カードが挿さっていません。
        // (補足) 壊れた SD カードや SD カードではないメディアが挿されているときにも、このエラーが返ります。
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else if(result <= nn::fs::ResultBadFormat())
      {
        // SD カードをフォーマットする必要があります。
        return RESULT_NG_BAD_FORMAT;
      }
      if(result <= nn::fs::ResultNotEnoughSpace())
      {
        // SD カードに必要な空き容量がありません。
        return RESULT_NG_NOT_ENOUGH_SPACE;
      }
      else if(result <= nn::fs::ResultNotFormatted())
      {
        // エラーが発生し、作成が途中で中断されました。
        return RESULT_NG_ERROR;
      }
      else if(result <= nn::fs::ResultOperationDenied())
      {
        if(result <= nn::fs::ResultWriteProtected())
        {
          // SD カードが書き込み禁止にされています。
          return RESULT_NG_WRITE_PROTECTED;
        }
        else if(result <= nn::fs::ResultMediaAccessError())
        {
          // 接触不良等によるハードウェア的な要因のときのみ、このエラーが返ります。
          // そのときは、リトライ処理、ゲームカードの挿し直し、本体の再起動などで復帰する可能性があります。
          return RESULT_NG_ERROR;
        }
        else
        {
          // SD カード上のディレクトリが読み取り専用にされている可能性があります。
          // (補足) マウント時にデータの復旧シーケンスが実行されることがあり、そのときに発生します。
          return RESULT_NG_WRITE_PROTECTED;
        }
      }
      else
      {
        // この拡張セーブデータ、もしくは SD カードは認識できませんでした。
        // ここに該当するエラーは、まとめてハンドリングを行ってください。(FATAL エラー表示画面には遷移させないでください。)
        // (補足) 開発中は、iconData, iconDataSize に不正な値を渡していないか確かめてください。
        CheckProgrammersError( result );
        return RESULT_NG_UNKNOWN;
      }
    }
    return RESULT_OK;
  }

//-----------------------------------------------------------------------------
/**
 * @brief   拡張セーブデータ領域の情報取得時のエラーチェック
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckInfoError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result <= nn::fs::ResultNotFound())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD カードが挿さっていません。
        // (補足) 壊れた SD カードや SD カードではないメディアが挿されているときにも、このエラーが返ります。
        //        但し、その場合は RegisterSdmcInsertedEvent で登録した挿入イベントはシグナルされます。
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else
      {
        // 指定した拡張セーブデータは存在していないので、作成する必要があります。
        // (補足) 開発中は、指定した ID が正しいか確認をしてください。
        return RESULT_NG_NOT_FOUND;
      }
    }
    else if(result <= nn::fs::ResultBadFormat())
    {
      // SD カードをフォーマットする必要があります。
      return RESULT_NG_BAD_FORMAT;
    }
    else if(result <= nn::fs::ResultOperationDenied())
    {
      if(result <= nn::fs::ResultWriteProtected())
      {
        // SD カードが書き込み禁止にされています。
        // (補足) マウント時にデータの復旧シーケンスが実行されることがあり、そのときに発生します。
        return RESULT_NG_WRITE_PROTECTED;
      }
      else if(result <= nn::fs::ResultMediaAccessError())
      {
        // 接触不良等によるハードウェア的な要因のときのみ、このエラーが返ります。
        // そのときは、SD カードの挿し直し、本体の再起動などで復帰する可能性があります。
        return RESULT_NG_ERROR;
      }
      else
      {
        // SD カード上のディレクトリが読み取り専用にされている可能性があります。
        // (補足) マウント時にデータの復旧シーケンスが実行されることがあり、そのときに発生します。
        return RESULT_NG_WRITE_PROTECTED;
      }
    }
    else
    {
      // 上記以外は、想定外のエラーです。この拡張セーブデータ、もしくは SD カードは認識できませんでした。
      // NN_ERR_THROW_FATAL 系のマクロは呼び出さないようにしてください。
      CheckProgrammersError( result );
      //GFL_ASSERT_STOP( 0 );
      return RESULT_NG_UNKNOWN;
    }
  }


//-----------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理
 *
 * ExtSaveThreadからCallBackとして呼ばれるメイン処理。
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::ThreadCallBack( void )
{
  switch ( m_thread_mode )
  {
  case THREAD_MODE_NOTHING:
    return;
  case THREAD_MODE_MOUNT_AND_FORMAT:
    m_thread_result = this->MountAndFormat( m_thread_icon_buffer, m_thread_icon_size );
    return;
  case THREAD_MODE_MOUNT:
    m_thread_result = this->Mount();
    return;
  case THREAD_MODE_UNMOUNT:
    m_thread_result = this->Unmount();
    return;
  case THREAD_MODE_GET_ARCHIVE_INFO:
    m_thread_result = this->GetArchiveInfo(
        &m_thread_dir_count_result, &m_thread_file_count_result );
    return;
  case THREAD_MODE_DELETE_ARCHIVE:
    m_thread_result = this->DeleteArchive();
    return;
  case THREAD_MODE_READ_DIRECTORY:
    m_thread_file_result = this->ReadDirectory(
        m_thread_heap, m_thread_use_path,
        m_thread_read_num, m_thread_entries, m_thread_num_entries );
    return;

  case THREAD_MODE_CREATE_FILE:
    m_thread_file_result = this->CreateFile(
        m_thread_use_path, m_thread_use_size );
    return;
  case THREAD_MODE_CREATE_FILE_OW:
    m_thread_file_result = this->CreateFileOW(
        m_thread_use_path, m_thread_use_size );
    return;
  case THREAD_MODE_DELETE_FILE:
    m_thread_file_result = this->DeleteFile( m_thread_use_path );
    return;
  case THREAD_MODE_RENAME_FILE:
    m_thread_file_result = this->RenameFile(
        m_thread_use_path, m_thread_use_path2 );
    return;
  case THREAD_MODE_READ_FILE:
    m_thread_file_result = this->ReadFile(
        m_thread_use_path, m_thread_dst_buffer, m_thread_use_size );
    return;
  case THREAD_MODE_WRITE_FILE:
    m_thread_file_result = this->WriteFile(
        m_thread_use_path, m_thread_src_buffer, m_thread_use_size );
    return;
  case THREAD_MODE_IS_FILE_EXISTS:
    m_thread_file_result = this->IsFileExists(
        m_thread_use_path, &m_thread_use_bool );
    return;
  case THREAD_MODE_CREATE_DIRECTORY:
    m_thread_file_result = this->CreateDirectory( m_thread_use_path );
    return;
  case THREAD_MODE_DELETE_DIRECTORY:
    m_thread_file_result = this->DeleteDirectory( m_thread_use_path );
    return;
  case THREAD_MODE_RENAME_DIRECTORY:
    m_thread_file_result = this->RenameDirectory(
        m_thread_use_path, m_thread_use_path2 );
    return;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 非同期処理用ワークの初期化処理
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::initThreadParams( void )
{
  GFL_ASSERT_STOP( m_thread == NULL );

  m_thread_use_path[0]        = L'\0';
  m_thread_use_path2[0]       = L'\0';
  m_thread_use_size           = 0;
  m_thread_use_bool           = false;

  m_thread_result             = gfl2::fs::ExtSaveSystem::RESULT_OK;;
  m_thread_file_result        = gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK;
  m_thread_dir_count_result   = 0;
  m_thread_file_count_result  = 0;
  m_thread_entries            = NULL;
  m_thread_read_num           = NULL;
  m_thread_num_entries        = 0;
  m_thread_dst_buffer         = NULL;
  m_thread_src_buffer         = NULL;

  m_thread_icon_buffer        = NULL;
  m_thread_icon_size          = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief パスを格納する
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::setPath( wchar_t * dst, const wchar_t * src )
{
  if ( src )
  {
    ::std::wcsncpy( dst, src, MAX_FILE_PATH_LENGTH - 1 );
    if ( ::std::wcslen(src) >= MAX_FILE_PATH_LENGTH - 1 )
    {
      dst[MAX_FILE_PATH_LENGTH - 1] = L'\0';
    }
  }
  else
  {
    dst[0] = L'\0';
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief スレッド開始処理
 * @param[in] pHeap       スレッド生成に使用するヒープ
 * @param[in] priority    スレッドの動作プライオリティ
 * @param[in] mode        スレッドの動作モード
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::startThread(
    gfl2::heap::HeapBase * pHeap, u8 priority, THREAD_MODE mode )
{
  if ( m_thread ) return;

  m_thread_heap = pHeap;
  m_thread_mode = mode;
  m_thread      = GFL_NEW( pHeap ) ExtSaveThread( pHeap, THREAD_STACK_SIZE, this );
  m_thread->Start( priority );
}



  }  // namespace fs
}  // namespace gfl
#endif
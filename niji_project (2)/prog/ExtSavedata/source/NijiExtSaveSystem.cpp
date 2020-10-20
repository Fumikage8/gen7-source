//==============================================================================
/**
 * @file    NijiExtSaveSystem.cpp
 * @brief   拡張セーブロードのコントロールを行うクラス
 * @author  tamada
 * @date    2012.09.25
*/
//==============================================================================
#include <nw/ut.h>

#include "ExtSavedata/include/NijiExtSaveSystem.h"
#include "ExtSavedata/include/NijiExtSaveData.h"
#include <thread/include/gfl2_Thread.h>
#include <fs/include/CTR/gfl2_FsCtrDirectory.h>
#include <GameSys/include/GameManager.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include "System/include/ThreadPriority.h"
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <gfx/include/gfl2_GFGL.h>
//#include "system/sleep.h"
#include <arc_index/SystemIcon.gaix>
#include "arc_def_index/arc_def.h"
//#include "debug/DebugLogWindow.h"

#include "System/include/HomeSleepAndPowerSystem.h"

namespace ExtSavedata{

#if PM_DEBUG
  bool NijiExtSaveSystem::m_DebugExtSaveMustFatalError = false;
#endif
//===================================================================
//===================================================================
static void putLog( const wchar_t * str )
{
  //xy_debug::LogWindow_AddMsg(xy_debug::LOGWIN_ID_POKE_SAVE, str );
}


#if defined(GF_PLATFORM_CTR)

//===================================================================
//
//
//  セーブロード制御用スレッド
//
//
//===================================================================
  struct EXTFILE_PARAM {
    NIJI_EXTSAVE_ID id;
    u32 file_id;
    const char * filename;
    u32 size;
  };
//--------------------------------------------------------------
/**
 * @class ExtSaveLoadControlThread
 *
 * @note  基本的にはgflib2のExtSaveSystemでスレッドを制御。
 * @note  起動部分のみを自前スレッド制御している
 */
//--------------------------------------------------------------
class ExtSaveLoadControlThread: public gfl2::thread::ThreadInterface
{
  private:
    NijiExtSaveSystem *     m_pSaveLoadControl;
    EXTFILE_PARAM *       m_pParams;
    gfl2::heap::HeapBase * m_pHeap;
    gfl2::thread::Thread* m_pThread;

  public:
    //--------------------------------------------------------------
    /// Constructor
    //--------------------------------------------------------------
    ExtSaveLoadControlThread(
        gfl2::heap::HeapBase * heap,
        u32 stack_size,
        NijiExtSaveSystem* svld_control,
        EXTFILE_PARAM * file_param) :
      m_pSaveLoadControl(svld_control),
      m_pParams(file_param),
      m_pHeap(heap),
      m_pThread(NULL)
    {
      m_pThread = GFL_NEW(m_pHeap) gfl2::thread::Thread(this, m_pHeap, true , stack_size );
    }

    //--------------------------------------------------------------
    /// Destructor
    //--------------------------------------------------------------
    virtual ~ExtSaveLoadControlThread()
    {
      GFL_SAFE_DELETE( m_pThread );
    }

    void Kill( void ){ m_pThread->Kill(); }
    bool IsAlive( void ){ return m_pThread->IsAlive(); }
    void Start( int priority){ m_pThread->Start(priority); }

    //--------------------------------------------------------------
    /// Main
    //--------------------------------------------------------------
    virtual void Function( void )
    {
      m_pSaveLoadControl->ThreadCallBack(m_pHeap, m_pParams);
      this->Kill();
    }
};


//--------------------------------------------------------------
/**
 * @brief sizeが0のときはデフォルトのサイズ、それ以外はそのままを返す
 */
//--------------------------------------------------------------
static size_t GetDefaultSizeIfZero( NIJI_EXTSAVE_ID id, size_t size )
{
  if ( size == 0 )
  {
    size = NijiExtSaveData::GetSaveFileSize(id);
  }
  return size;
}



//===================================================================
//===================================================================
//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------
NijiExtSaveSystem::NijiExtSaveSystem(gfl2::heap::HeapBase* heap) :
  mpExtSaveSystem(NULL),
  m_arcStatus(EXTARC_STATUS_OK),
  m_bBooted(false),
  m_threadMode(THREAD_MODE_NOTHING),
  m_pMyThread(NULL),
  m_threadSystemResult(gfl2::fs::ExtSaveSystem::RESULT_OK),
  m_systemAPIResult(gfl2::fs::ExtSaveSystem::RESULT_OK),
  m_fileAPIResult(gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK),
  m_iconBuffer(NULL),
  m_iconSize(0),
  m_IsWriting(false),
  m_pListener(NULL)
{
  mpExtSaveSystem = GFL_NEW(heap) gfl2::fs::ExtSaveSystem(heap);
  mpExtSaveSystem->Initialize(
      NijiExtSaveData::GetExtSaveDataId(),
      NijiExtSaveData::GetMaxDirectoryCount(), NijiExtSaveData::GetMaxFileCount() );

}


//--------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------
NijiExtSaveSystem::~NijiExtSaveSystem()
{
  GFL_DELETE(mpExtSaveSystem);
}


//--------------------------------------------------------------
/**
 * @brief   エラーが致命的か？の判定
 *
 * @note  ゲーム中はSDカード必須なので、起動時に事前チェックがある。
 * @note  アプリ内でSDカード無効エラーが発生した場合、致命的エラーとする
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::IsFatalError( gfl2::fs::ExtSaveFile::Result result )
{
#if PM_DEBUG
  if ( m_DebugExtSaveMustFatalError == true )
  {
    return true;
  }
#endif
  switch ( result )
  {
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                     // 成功
    return false;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:      // 失敗：指定したパスが存在しない
    return false; //パス間違いはアプリが自力で解決
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:  // 失敗：指定したパスが既に存在する
    return false; //パス間違いはアプリが自力で解決
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:         // 失敗：データが壊れているか: 改竄されている
    return false; //壊れている場合はアプリ側で対応
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ERROR:               // 失敗：接触不良など: ハードウェア要因
    return true;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:       // 失敗：メディアの空き容量不足
    return false; //容量不足はアプリ側で告知
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_WRITE_PROTECTED:     // 失敗：ライトプロテクトされている
    return true;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_NOT_FOUND:     // 失敗：SDカードが挿されていない
    return true;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR:  // 失敗：接触不良など
    return true;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ARCHIVE_INVALIDATED: // 失敗：アーカイブ無効（SDカード抜き等）
    return true;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_UNKNOWN:             // 失敗：想定外のエラー
    return true;
  }
  GFL_ASSERT( 0 );
  return true;
}

//===================================================================
//
//
//              同期処理
//
//
//===================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
NijiExtSaveSystem::EXTARC_STATUS NijiExtSaveSystem::CheckArchiveStatus( gfl2::heap::HeapBase * pTempHeap )
{
  int dir_max = NijiExtSaveData::GetMaxDirectoryCount();
  int file_max = NijiExtSaveData::GetMaxFileCount();
  gfl2::fs::ExtSaveSystem::Result result;

  //マウント
  result = mpExtSaveSystem->Mount();
  switch ( result )
  {
  case gfl2::fs::ExtSaveSystem::RESULT_OK:
    break;
  case gfl2::fs::ExtSaveSystem::RESULT_NG_MEDIA_NOT_FOUND:
  case gfl2::fs::ExtSaveSystem::RESULT_NG_WRITE_PROTECTED:
  case gfl2::fs::ExtSaveSystem::RESULT_NG_ERROR:
  case gfl2::fs::ExtSaveSystem::RESULT_NG_BAD_FORMAT:
    m_arcStatus = EXTARC_STATUS_MEDIA_NOT_FOUND;
    return m_arcStatus;

  case gfl2::fs::ExtSaveSystem::RESULT_NG_NOT_FOUND:
    m_arcStatus = EXTARC_STATUS_NEED_CREATE; //初回
    return m_arcStatus;

  case gfl2::fs::ExtSaveSystem::RESULT_NG_VERIFICATION_FAILED:
  case gfl2::fs::ExtSaveSystem::RESULT_NG_NOT_FORMATTED:
    m_arcStatus = EXTARC_STATUS_INCORRECT;
    return m_arcStatus;

  default:
    m_arcStatus = EXTARC_STATUS_ERROR;
    return m_arcStatus;
  }

  u32 dir, file;
  result = mpExtSaveSystem->GetArchiveInfo( &dir, &file );
  if ( dir != dir_max || file != file_max )
  { // アーカイブ生成条件が異なる場合
    mpExtSaveSystem->Unmount();
    m_arcStatus = EXTARC_STATUS_INCORRECT;
    return m_arcStatus;
  }


  nn::fs::DirectoryEntry * entries = GFL_NEW_ARRAY( pTempHeap ) nn::fs::DirectoryEntry[ dir_max ];
  gfl2::fs::Directory * rootDir = GFL_NEW( pTempHeap ) gfl2::fs::Directory();
  gfl2::fs::Directory::Result dir_result;
  do
  {
    //ルートディレクトリへパスを生成する
    CreateDirPathStr( m_tempPath, L"" );
    dir_result = rootDir->Open( m_tempPath );
    TAMADA_PRINT("rootDir->Open() = %d\n", dir_result );
    if ( dir_result != gfl2::fs::Directory::ACCESS_RESULT_OK ) break;

    s32 read_num;
    dir_result = rootDir->Read( &read_num, entries, dir_max );
    TAMADA_PRINT("rootDir->Read() = %d\n", dir_result );
    if ( dir_result != gfl2::fs::Directory::ACCESS_RESULT_OK ) break;

    {
      TAMADA_PRINT("Directory::Read() num = %d\n", read_num );
      int need_num = NijiExtSaveData::GetSaveDataFormatCount();
      int j;
      const ExtSaveDataFormat * form;
      for ( int i = 0; i < need_num; ++ i )
      {
        form = NijiExtSaveData::GetSaveDataFormat(i);
        for (j = 0; j < read_num; ++ j )
        {
          if ( std::wcsncmp( form->directory_name, entries[j].entryName, MAX_FILE_PATH_LENGTH ) == 0 )
          {
            break;
          }
        }
        if ( j == read_num )
        {
          dir_result = gfl2::fs::Directory::ACCESS_RESULT_NG_BROKEN_DATA;
        }
      }
    }
  }
  while (0);
  GFL_DELETE rootDir;
  GFL_DELETE_ARRAY entries;

  mpExtSaveSystem->Unmount();

  switch ( dir_result )
  {
  case gfl2::fs::Directory::ACCESS_RESULT_OK:
    m_arcStatus = EXTARC_STATUS_OK;
    return m_arcStatus;
  case gfl2::fs::Directory::ACCESS_RESULT_NG_WRITE_PROTECTED:
    m_arcStatus = EXTARC_STATUS_MEDIA_NOT_FOUND;
    return m_arcStatus;
  case gfl2::fs::Directory::ACCESS_RESULT_NG_BAD_FORMAT:
  default:
    m_arcStatus = EXTARC_STATUS_INCORRECT;
    return m_arcStatus;
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::BootProcess( gfl2::heap::HeapBase * pTempHeap )
{
  createIconBuffer( pTempHeap );
  bootSystem( pTempHeap );
  deleteIconBuffer();
}

//--------------------------------------------------------------
/**
 * @brief 起動時に必要なディレクトリを作成する
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::createDirectories( nn::fs::DirectoryEntry * entries, int dir_max )
{
  gfl2::fs::Directory rootDir;
  gfl2::fs::Directory::Result dir_result;
  s32 read_num;

  //ルートディレクトリへパスを生成する
  CreateDirPathStr( m_tempPath, L"" );
  dir_result = rootDir.Open( m_tempPath );
  //マウントできたのにルートディレクトリにアクセスできないのはまずい
  if ( dir_result != gfl2::fs::Directory::ACCESS_RESULT_OK )
  {
    m_fileAPIResult = gfl2::fs::ExtSaveFile::GetAccessResult( rootDir.GetLastAccessNNResult() );
    return false;
  }

  dir_result = rootDir.Read( &read_num, entries, dir_max );
  if ( dir_result != gfl2::fs::Directory::ACCESS_RESULT_OK )
  {
    m_fileAPIResult = gfl2::fs::ExtSaveFile::GetAccessResult( rootDir.GetLastAccessNNResult() );
    return false;
  }
  TAMADA_PRINT("Directory::Read() num = %d\n", read_num );

  int need_num = NijiExtSaveData::GetSaveDataFormatCount();
  for ( int i = 0; i < need_num; ++ i )
  {
    const ExtSaveDataFormat * form = NijiExtSaveData::GetSaveDataFormat(i);
    int j;
    for (j = 0; j < read_num; ++ j )
    {
      if ( std::wcsncmp( form->directory_name, entries[j].entryName, MAX_FILE_PATH_LENGTH ) == 0 )
      {
        break;
      }
    }
    if ( j == read_num )
    { //必要なディレクトリが見つからない場合は作成する
      GFL_PRINT("NijiExtSaveSystem::BootProcess::Create:(%d)", i);
      CreateDirPathStr( m_tempPath, form->directory_name );
      m_fileAPIResult = mpExtSaveSystem->CreateDirectory( m_tempPath );
      if ( m_fileAPIResult != gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
      {
        //容量不足でDirが作成できないことがある
        return false;
      }
    }
  }
  return true;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
gfl2::fs::ExtSaveSystem::Result NijiExtSaveSystem::GetBootResult( void ) const
{
  if ( m_systemAPIResult != gfl2::fs::ExtSaveSystem::RESULT_OK )
  {
    return m_systemAPIResult;
  }
  switch ( m_fileAPIResult )
  {
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                     // 成功
    return gfl2::fs::ExtSaveSystem::RESULT_OK;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:      // 失敗：指定したパスが存在しない
    return gfl2::fs::ExtSaveSystem::RESULT_NG_NOT_FOUND;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:  // 失敗：指定したパスが既に存在する
      GFL_ASSERT(0);
    return gfl2::fs::ExtSaveSystem::RESULT_NG_ERROR;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:         // 失敗：データが壊れているか, 改竄されている
    return gfl2::fs::ExtSaveSystem::RESULT_NG_VERIFICATION_FAILED;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ERROR:               // 失敗：接触不良など, ハードウェア要因
    return gfl2::fs::ExtSaveSystem::RESULT_NG_ERROR;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:       // 失敗：メディアの空き容量不足
    return gfl2::fs::ExtSaveSystem::RESULT_NG_NOT_ENOUGH_SPACE;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_WRITE_PROTECTED:     // 失敗：ライトプロテクトされている
    return gfl2::fs::ExtSaveSystem::RESULT_NG_WRITE_PROTECTED;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_NOT_FOUND:     // 失敗：SDカードが挿されていない
    return gfl2::fs::ExtSaveSystem::RESULT_NG_MEDIA_NOT_FOUND;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR:  // 失敗：接触不良など
    return gfl2::fs::ExtSaveSystem::RESULT_NG_ERROR;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ARCHIVE_INVALIDATED: // 失敗：アーカイブ無効（SDカード抜き等）
    return gfl2::fs::ExtSaveSystem::RESULT_NG_ERROR;
  case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_UNKNOWN:             // 失敗：想定外のエラー
  default:
    return gfl2::fs::ExtSaveSystem::RESULT_NG_UNKNOWN;
  }
}

//--------------------------------------------------------------
/**
 * @brief 起動処理
 */
//--------------------------------------------------------------
void NijiExtSaveSystem::bootSystem( gfl2::heap::HeapBase * pTempHeap )
{
  int dir_max = NijiExtSaveData::GetMaxDirectoryCount();
  int file_max = NijiExtSaveData::GetMaxFileCount();
  gfl2::fs::ExtSaveSystem::Result result;

  m_systemAPIResult = gfl2::fs::ExtSaveSystem::RESULT_OK;
  m_fileAPIResult   = gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK;

  //マウント（＋内部生成処理）
  m_systemAPIResult = mpExtSaveSystem->MountAndFormat( m_iconBuffer, m_iconSize );
  if ( m_systemAPIResult != gfl2::fs::ExtSaveSystem::RESULT_OK )
  {
    return;
  }

  //BTS6515 対処関連
  //下記のアーカイブ生成条件チェックは製品版では不要なコードだが
  //フォーマット変更への対応処理がないと仕様変更時に動作しなくなる
  u32 dir, file;
  m_systemAPIResult = mpExtSaveSystem->GetArchiveInfo( &dir, &file );
  if ( dir != dir_max || file != file_max )
  { // アーカイブ生成条件が異なる場合は削除→再作成
    mpExtSaveSystem->Unmount(); // Mount済みの場合のみなのでエラーチェックしない
    m_systemAPIResult = mpExtSaveSystem->DeleteArchive();
    if ( m_systemAPIResult != gfl2::fs::ExtSaveSystem::RESULT_OK )
    { //削除失敗
      return;
    }

    m_systemAPIResult = mpExtSaveSystem->MountAndFormat( m_iconBuffer, m_iconSize );
    if ( m_systemAPIResult != gfl2::fs::ExtSaveSystem::RESULT_OK )
    { //再作成失敗
      return;
    }
  }

  { //フォーマット直後、必要なディレクトリを作成する
    nn::fs::DirectoryEntry * entries = GFL_NEW_ARRAY( pTempHeap ) nn::fs::DirectoryEntry[ dir_max ];
    m_bBooted = createDirectories( entries, dir_max );
    GFL_DELETE_ARRAY entries;
  }
}

//--------------------------------------------------------------
///    拡張セーブデータ領域の削除
//--------------------------------------------------------------
gfl2::fs::ExtSaveSystem::Result NijiExtSaveSystem::DeleteArchive( void )
{
  gfl2::fs::ExtSaveSystem::Result res;
  if ( m_bBooted )
  {
    res = mpExtSaveSystem->Unmount();
    GFL_PRINT("NijiExtSaveSystem::Unmount=%d\n", res );
  }
  res = mpExtSaveSystem->DeleteArchive();
  GFL_PRINT("NijiExtSaveSystem::DeleteArchive=%d\n", res );
  return res;
}

//--------------------------------------------------------------
///     ファイル作成（パス指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::CreateExtFile( const wchar_t * path ,size_t size )
{
  m_fileAPIResult = mpExtSaveSystem->CreateFileOW( path, size );
  //マウント等、致命的な場合はゲーム中断
  checkFatalError( m_fileAPIResult );
  if ( m_fileAPIResult == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST )
  {
    //ResultNotFoundとして、アーカイブがない場合も返ることがある。
    //判別のため、ここでACCESS_RESULT_NG_ARCHIVE_INVALIDATEDに書き換えて
    //アプリケーション側ではアーカイブ無効と判定させる。
    m_fileAPIResult = gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ARCHIVE_INVALIDATED;
  }
  return m_fileAPIResult;
}

//--------------------------------------------------------------
///     ファイル作成（ファイルID指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::CreateExtFile( NIJI_EXTSAVE_ID id, int fileindex, size_t size )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  size = GetDefaultSizeIfZero( id, size );
  return CreateExtFile( m_tempPath, size );
}

//--------------------------------------------------------------
///     ファイル削除（パス指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::DeleteExtFile( const wchar_t * path )
{
  m_fileAPIResult = mpExtSaveSystem->DeleteFile( path );
  //マウント等、致命的な場合はゲーム中断
  checkFatalError( m_fileAPIResult );
  return m_fileAPIResult;
}

//--------------------------------------------------------------
///     ファイル削除（ファイルID指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::DeleteExtFile( NIJI_EXTSAVE_ID id, int fileindex )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  return DeleteExtFile( m_tempPath );
}

//--------------------------------------------------------------
///   ファイル読み込み（パス指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::ReadFile(
    const wchar_t * path, void * dst, size_t size )
{
  m_fileAPIResult = mpExtSaveSystem->ReadFile( path, dst, size );
  //マウント等、致命的な場合はゲーム中断
  checkFatalError( m_fileAPIResult );
  return m_fileAPIResult;
}

//--------------------------------------------------------------
///   ファイル読み込み（ファイルID指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::ReadFile(
    NIJI_EXTSAVE_ID id, int fileindex, void * dst, size_t size )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  size = GetDefaultSizeIfZero( id, size );
  return ReadFile( m_tempPath, dst, size );
}

//--------------------------------------------------------------
///   ファイル書込み（パス指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::WriteFile(
    const wchar_t * path, const void * src, size_t size )
{
  m_fileAPIResult = mpExtSaveSystem->WriteFile( path, src, size );
  //マウント等、致命的な場合はゲーム中断
  checkFatalError( m_fileAPIResult );
  return m_fileAPIResult;
}

//--------------------------------------------------------------
///   ファイル書込み（ファイルID指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::WriteFile(
    NIJI_EXTSAVE_ID id, int fileindex, const void * src, size_t size )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  size = GetDefaultSizeIfZero( id, size );
  return WriteFile( m_tempPath, src, size );
}

//--------------------------------------------------------------
///   ファイル名変更（パス指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::RenameFile(
    const wchar_t * current_name, const wchar_t * new_name )
{
  m_fileAPIResult = mpExtSaveSystem->RenameFile( current_name, new_name );
  //マウント等、致命的な場合はゲーム中断
  checkFatalError( m_fileAPIResult );
  return m_fileAPIResult;
}
//--------------------------------------------------------------
///   ファイル名変更（ファイルID指定版）
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::RenameFile(
    NIJI_EXTSAVE_ID id, int current_index, int new_index )
{
  CreateFilePathStr( m_tempPath, id, current_index );
  CreateFilePathStr( m_tempPath2, id, new_index );
  return RenameFile( m_tempPath, m_tempPath2 );
}

//--------------------------------------------------------------
/**
 * @brief   ファイルの存在チェック（パス指定版）
 *
 * @attention    ファイル数、サイズなどの検証はアプリ側で対応
 * @attention    壊れていた場合の処理もアプリ側で対応
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::IsFileExists( const wchar_t * path )
{
  bool flag;
  m_fileAPIResult = mpExtSaveSystem->IsFileExists( path, &flag );
  //マウント等、致命的な場合はゲーム中断
  checkFatalError( m_fileAPIResult );
  return flag;
}
//--------------------------------------------------------------
/**
 * @brief   ファイルの存在チェック（ファイルID指定版）
 *
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::IsFileExists( NIJI_EXTSAVE_ID id, int fileindex )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  return IsFileExists( m_tempPath );
}

//--------------------------------------------------------------
/// ディレクトリ情報読み取り
//--------------------------------------------------------------
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::ReadDirectory(
    gfl2::heap::HeapBase * pHeap,
    NIJI_EXTSAVE_ID id, s32 * pReadNum,
    nn::fs::DirectoryEntry * pEntries, s32 numEntries )
{
  CreateDirPathStr( m_tempPath, id );
  m_fileAPIResult = mpExtSaveSystem->ReadDirectory(
      pHeap, m_tempPath, pReadNum, pEntries, numEntries );
  //マウント等、致命的な場合はゲーム中断
  checkFatalError( m_fileAPIResult );
  return m_fileAPIResult;
}

//===================================================================
//
//
//    非同期処理
//
//
//===================================================================
//--------------------------------------------------------------
/**
 * @brief   システム起動処理の非同期開始
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartBootProcess( gfl2::heap::HeapBase * pHeap )
{
  EXTFILE_PARAM file_param; //dummy

  createIconBuffer( pHeap );
  this->startThread(
      System::ThreadPriority::EXTSAVE_THREAD_PRIORITY, THREAD_MODE_BOOT_PROCESS, pHeap, &file_param );
  return true;
}
//--------------------------------------------------------------
/**
 * @brief   システム起動処理待ち
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::WaitBootProcess( void )
{
  bool finished = this->isAsyncFinished();
  if (finished)
  {
    deleteIconBuffer();
  }
  return finished;
}

//--------------------------------------------------------------
/**
 * @brief   ファイル作成の非同期初期化処理開始
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartCreateFile(
    gfl2::heap::HeapBase *pHeap,
    NIJI_EXTSAVE_ID id, int fileindex, size_t size )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  size = GetDefaultSizeIfZero( id, size );
  return StartCreateFile( pHeap, m_tempPath, size );
}

//--------------------------------------------------------------
/**
 * @brief   ファイル作成の非同期初期化処理開始
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartCreateFile(
    gfl2::heap::HeapBase *pHeap, const wchar_t * path, size_t size )
{
  bool result;
  result = mpExtSaveSystem->StartAsyncCreateFileOW(
      pHeap, System::ThreadPriority::EXTSAVE_THREAD_PRIORITY, path, size );
  GFL_PRINT("Create File Start\n");
  if ( result )
  {
    //スリープ＆HOMEボタン禁止
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    m_IsWriting = true;
  }
  return result;
}

//--------------------------------------------------------------
/**
 * @brief   ファイル作成の非同期初期化処理メイン
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::WaitCreateFile( gfl2::fs::ExtSaveFile::Result * result )
{
  bool is_end = waitFileAPI( result );
  if ( is_end )
  {
    //スリープ＆HOMEボタン許可
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    //マウント等、致命的な場合はゲーム中断
    checkFatalError( *result );
    m_IsWriting = false;
  }
  return is_end;
}

//--------------------------------------------------------------
/**
 * @brief   ファイル削除の非同期初期化処理開始
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartDeleteFile(
    gfl2::heap::HeapBase *pHeap, NIJI_EXTSAVE_ID id, int fileindex )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  return StartDeleteFile( pHeap, m_tempPath );
}

//--------------------------------------------------------------
/**
 * @brief   ファイル削除の非同期初期化処理開始
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartDeleteFile(
    gfl2::heap::HeapBase *pHeap, const wchar_t * path )
{
  bool result;
  result = mpExtSaveSystem->StartAsyncDeleteFile(
      pHeap, System::ThreadPriority::EXTSAVE_THREAD_PRIORITY, path );
  GFL_PRINT("Delete File Start\n");
  if ( result )
  {
    //スリープ＆HOMEボタン禁止
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    m_IsWriting = true;
  }
  return result;
}

//--------------------------------------------------------------
/**
 * @brief   ファイル削除の非同期初期化処理メイン
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::WaitDeleteFile( gfl2::fs::ExtSaveFile::Result * result )
{
  bool is_end = waitFileAPI( result );
  if ( is_end )
  {
    //スリープ＆HOMEボタン許可
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    //マウント等、致命的な場合はゲーム中断
    checkFatalError( *result );
    m_IsWriting = false;
  }
  return is_end;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartReadFile( gfl2::heap::HeapBase * pHeap,
    NIJI_EXTSAVE_ID id, int fileindex, void * dst, size_t size )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  size = GetDefaultSizeIfZero( id, size );
  return this->StartReadFile( pHeap, m_tempPath, dst, size );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartReadFile( gfl2::heap::HeapBase * pHeap,
    const wchar_t * path, void * dst, size_t size )
{
  bool result = mpExtSaveSystem->StartAsyncReadFile(
      pHeap, System::ThreadPriority::EXTSAVE_THREAD_PRIORITY, path, dst, size );
  if ( result )
  {
    //スリープ＆HOMEボタン禁止
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
  }
  return result;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::WaitReadFile( gfl2::fs::ExtSaveFile::Result * result )
{
  bool is_end = waitFileAPI( result );
  if ( is_end )
  {
    //スリープ＆HOMEボタン許可
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    //マウント等、致命的な場合はゲーム中断
    checkFatalError( *result );
  }
  return is_end;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartWriteFile( gfl2::heap::HeapBase * pHeap,
    NIJI_EXTSAVE_ID id, int fileindex, const void * src, size_t size )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  size = GetDefaultSizeIfZero( id, size );
  return this->StartWriteFile( pHeap, m_tempPath, src, size );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartWriteFile( gfl2::heap::HeapBase * pHeap,
    const wchar_t * path, const void * src, size_t size )
{
  bool result = mpExtSaveSystem->StartAsyncWriteFile(
      pHeap, System::ThreadPriority::EXTSAVE_THREAD_PRIORITY, path, src, size );
  if ( result )
  {
    putLog(L"StartWriteFile\n");
    //スリープ＆HOMEボタン禁止
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    m_IsWriting = true;
  }
  return result;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::WaitWriteFile( gfl2::fs::ExtSaveFile::Result * result )
{
  bool is_end = waitFileAPI( result );
  if ( is_end )
  {
    putLog(L"StartWriteFile:End\n");
    //スリープ＆HOMEボタン許可
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    //マウント等、致命的な場合はゲーム中断
    checkFatalError( *result );
    m_IsWriting = false;
  }
  return is_end;
}

//--------------------------------------------------------------
/**
 * @brief   セーブ終了処理（電源切断時用）
 * @retval  true  セーブ処理終了
 * @retval  false セーブ処理継続中
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::FinalizeSaveForShutdown( void )
{
  gfl2::fs::ExtSaveFile::Result result;
  return WaitWriteFile( &result );
}

//--------------------------------------------------------------
/**
 * @brief   セーブ終了待ち（スリープ用）
 */
//--------------------------------------------------------------
void NijiExtSaveSystem::WaitSaveForSleep( void )
{
  while( m_IsWriting )
  {
    gfl2::fs::ExtSaveFile::Result result;
    if( WaitWriteFile( &result ) )
    {
      return;
    }
    gfl2::gfx::GFGL::SwapDisplay_WaitVSync();
  }
}

//--------------------------------------------------------------
/**
 * @brief   ファイルの存在チェックの非同期開始（ファイルID指定版）
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartIsFileExists( gfl2::heap::HeapBase * pHeap,
    NIJI_EXTSAVE_ID id, int fileindex )
{
  CreateFilePathStr( m_tempPath, id, fileindex );
  return this->StartIsFileExists( pHeap, m_tempPath );
}
//--------------------------------------------------------------
/**
 * @brief   ファイルの存在チェックの非同期開始（パス指定版）
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartIsFileExists(
    gfl2::heap::HeapBase * pHeap, const wchar_t * path )
{
  bool result = mpExtSaveSystem->StartAsyncIsFileExists(
      pHeap, System::ThreadPriority::EXTSAVE_THREAD_PRIORITY, path );
  if ( result )
  {
    //スリープ＆HOMEボタン禁止
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
  }
  return result;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::WaitIsFileExists( gfl2::fs::ExtSaveFile::Result * result )
{
  bool is_end = waitFileAPI( result );
  if ( is_end )
  {
    //スリープ＆HOMEボタン許可
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    //マウント等、致命的な場合はゲーム中断
    checkFatalError( *result );
  }
  return is_end;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::StartReadDirectory(
    gfl2::heap::HeapBase * pHeap,
    NIJI_EXTSAVE_ID id, s32 * pReadNum,
    nn::fs::DirectoryEntry * pEntries, s32 numEntries )
{
  bool result;
  const ExtSaveDataFormat * form = NijiExtSaveData::GetSaveDataFormat(id);
  GFL_ASSERT( numEntries == form->max_file_num );
  CreateDirPathStr( m_tempPath, id );
  result = mpExtSaveSystem->StartAsyncReadDirectory(
      pHeap, System::ThreadPriority::EXTSAVE_THREAD_PRIORITY,
      m_tempPath, pReadNum, pEntries, numEntries );
  if ( result )
  {
    //スリープ＆HOMEボタン禁止
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
  }
  return result;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
bool NijiExtSaveSystem::WaitReadDirectory( gfl2::fs::ExtSaveFile::Result * result )
{
  bool is_end = waitFileAPI( result );
  if ( is_end )
  {
    //スリープ＆HOMEボタン許可
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
    //マウント等、致命的な場合はゲーム中断
    checkFatalError( *result );
  }
  return is_end;
}

//===================================================================
//
//
//                      Privateな関数
//
//
//===================================================================
//--------------------------------------------------------------
/**
 * @brief System操作APIの非同期処理待ち
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::waitSystemAPI( gfl2::fs::ExtSaveSystem::Result * result )
{
  if (mpExtSaveSystem->IsAsyncFinished())
  {
    m_systemAPIResult = mpExtSaveSystem->GetLastSystemResult();
    *result = m_systemAPIResult;
    return true;
  }
  return false;
}

//--------------------------------------------------------------
/**
 * @brief File操作APIの非同期処理待ち
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::waitFileAPI( gfl2::fs::ExtSaveFile::Result * result )
{
  if (mpExtSaveSystem->IsAsyncFinished())
  {
    m_fileAPIResult = mpExtSaveSystem->GetLastFileResult();
    *result = m_fileAPIResult;
    return true;
  }
  return false;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::CreateDirPathStr(
    wchar_t * dst, const wchar_t * dir_name )
{
  nw::ut::snwprintf(
      dst, MAX_FILE_PATH_LENGTH, MAX_FILE_PATH_LENGTH - 1,
      L"%ls/%ls",
      gfl2::fs::ExtSaveSystem::GetMountNameWChar(), dir_name );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::CreateDirPathStr(
    wchar_t * dst, NIJI_EXTSAVE_ID id )
{
  GFL_ASSERT_STOP( id < EXTID_NUM );
  nw::ut::snwprintf(
      dst, MAX_FILE_PATH_LENGTH, MAX_FILE_PATH_LENGTH - 1,
      L"%ls/%ls",
      gfl2::fs::ExtSaveSystem::GetMountNameWChar(),
      NijiExtSaveData::GetSaveDataFormat(id)->directory_name );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::CreateFilePathStr(
      wchar_t * dst, const wchar_t * dir_name, const wchar_t * file_name )
{
  nw::ut::snwprintf(
      dst, MAX_FILE_PATH_LENGTH, MAX_FILE_PATH_LENGTH - 1,
      L"%ls/%ls/%ls",
      gfl2::fs::ExtSaveSystem::GetMountNameWChar(), dir_name, file_name );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::CreateFilePathStr(
    wchar_t * dst, NIJI_EXTSAVE_ID id, int fileindex )
{
  GFL_ASSERT_STOP( id < EXTID_NUM );
  GFL_ASSERT_STOP( fileindex < NijiExtSaveData::GetSaveFileMaxNum( id ) );

  nw::ut::snwprintf(
      dst, MAX_FILE_PATH_LENGTH, MAX_FILE_PATH_LENGTH - 1,
      L"%ls/%ls/%03d",
      gfl2::fs::ExtSaveSystem::GetMountNameWChar(),
      NijiExtSaveData::GetSaveDataFormat(id)->directory_name,
      fileindex );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::createIconBuffer( gfl2::heap::HeapBase * pTempHeap )
{
  // リソース読み込み
  app::util::FileAccessor::FileOpenSync( ARCID_SYSTEM_ICON, pTempHeap );

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

  req.arcId      = ARCID_SYSTEM_ICON;                  // ARCID。ARCID_NULLのときはfileNameが使われる。
  req.datId      = GARC_SystemIcon_extSaveIcon_ICN;                  // アーカイブファイル内データID
  req.ppBuf      = &m_iconBuffer;                     // heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
  req.heapForBuf = pTempHeap;                    // ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
  req.align      = 128;                   // heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。圧縮ファイルも同じアラインメントで読みます。
  req.pRealReadSize = &m_iconSize;
  req.heapForReq = NULL;  // リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
  req.heapForCompressed = NULL;  // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
  req.lang       = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG;

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->SyncArcFileLoadData( req );

  app::util::FileAccessor::FileCloseSync( ARCID_SYSTEM_ICON );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::deleteIconBuffer( void )
{
  GflHeapFreeMemory( m_iconBuffer );
  m_iconSize = 0;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void NijiExtSaveSystem::checkFatalError( gfl2::fs::ExtSaveFile::Result result )
{
  if (IsFatalError( result ) )
  {
    //致命的な場合、最後のnn:Resultをgflibから取得
#if PM_DEBUG
    nn::Result err = mpExtSaveSystem->GetLastNNResult();
    NN_DBG_PRINT_RESULT( err );
#endif
    if( m_pListener )
    {
      m_pListener->OnExtSaveFatalError();
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   ResultMediaNotFoundでFATALERRORを起こす
 */
//--------------------------------------------------------------
void NijiExtSaveSystem::ThrowMediaNotFoundError( void )
{
  nn::Result resultMediaNotFound(
      nn::Result::LEVEL_FATAL,
      nn::Result::SUMMARY_NOT_FOUND,
      nn::Result::MODULE_NN_FS,
      171
      );
  NN_ERR_THROW_FATAL_ALL( resultMediaNotFound );
}

//--------------------------------------------------------------
/**
 * @brief バックアップ処理スレッド生成
 * @param priority    スレッドのプライオリティ
 * @param mode        モード
 * @param pHeap       使用する作業用ヒープ
 * @param file_param   セーブデータへのポインタ
 */
//--------------------------------------------------------------
void NijiExtSaveSystem::startThread(
    u8 priority, NijiExtSaveSystem::EXT_SYS_THREAD_MODE mode, gfl2::heap::HeapBase * pHeap, EXTFILE_PARAM * file_param )
{
  //スリープ＆HOMEボタン禁止
  System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
  m_threadMode = mode;
  m_pMyThread  = GFL_NEW( pHeap ) ExtSaveLoadControlThread( pHeap, 4096, this, file_param );
  m_pMyThread->Start( priority );
}

//--------------------------------------------------------------
/**
 * @brief スレッド終了待ち
 * @return  bool  trueのとき終了している
 */
//--------------------------------------------------------------
bool NijiExtSaveSystem::isAsyncFinished( void )
{
  if (!m_pMyThread) return true;
  if (m_pMyThread->IsAlive() ) return false;

  //スリープ＆HOMEボタン許可
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SDCARD_ACC );
  GFL_DELETE m_pMyThread;
  m_pMyThread = NULL;
  m_threadMode = THREAD_MODE_NOTHING;
  return true;
}

//--------------------------------------------------------------
/**
 * @brief スレッドから呼ばれるバックアップ処理メイン
 *
 * @note  モードにより内部で分岐している。
 */
//--------------------------------------------------------------
void NijiExtSaveSystem::ThreadCallBack(gfl2::heap::HeapBase * pHeap, EXTFILE_PARAM * file_param )
{
  switch ( m_threadMode )
  {
  case THREAD_MODE_NOTHING:
    GFL_ASSERT(0);
    return;

  case THREAD_MODE_BOOT_PROCESS:
    {
      bootSystem( pHeap );
    }
    break;

  }
}
#else
NijiExtSaveSystem::NijiExtSaveSystem(gfl2::heap::HeapBase* heap){}
NijiExtSaveSystem::~NijiExtSaveSystem(){};
NijiExtSaveSystem::EXTARC_STATUS NijiExtSaveSystem::CheckArchiveStatus( gfl2::heap::HeapBase * pTempHeap ){ return EXTARC_STATUS_OK; };
void NijiExtSaveSystem::BootProcess( gfl2::heap::HeapBase * pTempHeap ){};
bool NijiExtSaveSystem::StartBootProcess( gfl2::heap::HeapBase * pHeap ){ return true; };
bool NijiExtSaveSystem::WaitBootProcess( void ){ return true; };
gfl2::fs::ExtSaveSystem::Result NijiExtSaveSystem::GetBootResult( void ) const{ return gfl2::fs::ExtSaveSystem::RESULT_OK; };
bool NijiExtSaveSystem::IsFatalError( gfl2::fs::ExtSaveFile::Result result ){ return false; };
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::CreateExtFile( NIJI_EXTSAVE_ID id, int fileindex, size_t size ){ return gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK; };
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::DeleteExtFile( NIJI_EXTSAVE_ID id, int fileindex ){ return gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK; };
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::ReadFile( NIJI_EXTSAVE_ID id, int fileindex, void * dst, size_t size ){ return gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK; };
gfl2::fs::ExtSaveFile::Result NijiExtSaveSystem::WriteFile( NIJI_EXTSAVE_ID id, int fileindex, const void * src, size_t size ){ return gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK; };
bool NijiExtSaveSystem::IsFileExists( NIJI_EXTSAVE_ID id, int fileindex ){ return true; }
bool NijiExtSaveSystem::StartWriteFile( gfl2::heap::HeapBase * pHeap , NIJI_EXTSAVE_ID id, int fileindex, const void * src, size_t size ){ return true; }
bool NijiExtSaveSystem::WaitWriteFile( gfl2::fs::ExtSaveFile::Result * result ){ return true; }
bool NijiExtSaveSystem::StartCreateFile( gfl2::heap::HeapBase *pHeap, NIJI_EXTSAVE_ID id, int fileindex, size_t size ){ return true; }
bool NijiExtSaveSystem::WaitCreateFile( gfl2::fs::ExtSaveFile::Result * result ){ return true; }
bool NijiExtSaveSystem::StartDeleteFile( gfl2::heap::HeapBase *pHeap, NIJI_EXTSAVE_ID id, int fileindex ){ return true; }
bool NijiExtSaveSystem::WaitDeleteFile( gfl2::fs::ExtSaveFile::Result * result ){ return true; }
bool NijiExtSaveSystem::StartReadFile( gfl2::heap::HeapBase * pHeap, NIJI_EXTSAVE_ID id, int fileindex, void * dst, size_t size ){ return true; }
bool NijiExtSaveSystem::WaitReadFile( gfl2::fs::ExtSaveFile::Result * result ){ return true; }
bool NijiExtSaveSystem::StartIsFileExists( gfl2::heap::HeapBase * pHeap, NIJI_EXTSAVE_ID id, int fileindex ){ return true; }
bool NijiExtSaveSystem::WaitIsFileExists( gfl2::fs::ExtSaveFile::Result * result ){ return true; }

#endif

} //namespace extsavedata


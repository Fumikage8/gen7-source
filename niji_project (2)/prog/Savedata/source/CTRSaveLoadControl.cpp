#if   defined(GF_PLATFORM_CTR)
//===================================================================
/**
 * @file    CTRSaveLoadControl.cpp
 * @brief   セーブ＆ロードのコントロールを行うクラス
 * @author  k.ohno
 * @date    2011.3.8
*/
//===================================================================
#include <Savedata/include/CTRSaveLoadControl.h>
#include <Savedata/include/ISaveDataTotal.h>
#include <Savedata/include/SaveDataInterface.h>
#include <Savedata/include/SaveData.h>

#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "System/include/HeapDefine.h"
#include "System/include/ThreadPriority.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "System/include/HomeSleepAndPowerSystem.h"

namespace Savedata{

//===================================================================
//===================================================================
//--------------------------------------------------------------
/**
 * @brief ファイルの情報保持構造体
 */
//--------------------------------------------------------------
struct FileInfo
{
  ///ファイル名の最長
  static const size_t FILENAME_LEN = 16;


  ISaveDataTotal * save_total;  ///<格納するセーブ全体データへのポインタ
  s64 size;                     ///<ファイルサイズ
  wchar_t name[ FILENAME_LEN ]; ///<ファイル名
};


//===================================================================
//
//
//  セーブロード制御用スレッド
//
//
//===================================================================
//--------------------------------------------------------------
/**
 * @class CTRSaveLoadControlThread
 */
//--------------------------------------------------------------
class CTRSaveLoadControlThread: public gfl2::thread::ThreadInterface
{
  private:
    CTRSaveLoadControl *     m_pCTRSaveLoadControl;
    ISaveDataTotal *            m_pSaveData;
    gfl2::heap::HeapBase * m_pHeap;

    gfl2::thread::Thread* m_pThread;



  public:
    //--------------------------------------------------------------
    /// Constructor
    //--------------------------------------------------------------
    CTRSaveLoadControlThread(
        gfl2::heap::HeapBase * heap,
        u32 stack_size,
        CTRSaveLoadControl* svld_control,
        ISaveDataTotal * save_total) :
      m_pCTRSaveLoadControl(svld_control),
      m_pSaveData(save_total),
      m_pHeap(heap),
      m_pThread(NULL)
    {
      m_pThread = GFL_NEW(m_pHeap) gfl2::thread::Thread(this, m_pHeap, true , stack_size );
    }

    //--------------------------------------------------------------
    /// Destructor
    //--------------------------------------------------------------
    virtual ~CTRSaveLoadControlThread()
    {
      GFL_SAFE_DELETE( m_pThread );
    }

    //--------------------------------------------------------------
    /// Main
    //--------------------------------------------------------------
    virtual void Function( void )
    {
      m_pCTRSaveLoadControl->ThreadCallBack(m_pHeap, m_pSaveData);
      m_pThread->Kill();
    }

    
    bool IsAlive( void ){ return m_pThread->IsAlive(); };
    void Start( int priority ){ m_pThread->Start(priority); };
};





//===================================================================
//===================================================================
//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------
CTRSaveLoadControl::CTRSaveLoadControl(gfl2::heap::HeapBase* heap, int indexmax) :
  mpBackupSystem(NULL),
  mFileIndex(0),
  mFileIndexMax(indexmax),
  mFileInfo(NULL),
  mArcStatus(ARC_STATUS_NEED_INIT),
  m_threadMode(THREAD_MODE_NOTHING),
  m_pMyThread(NULL),
  m_threadFileResult(gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK),
  m_threadSystemResult(gfl2::fs::FsCtrBackupSystem::RESULT_OK),
  m_threadTarget(NULL)
{
  mpBackupSystem = GFL_NEW( heap ) gfl2::fs::FsCtrBackupSystem( heap );
  mFileInfo = GFL_NEW_ARRAY( heap ) FileInfo[ mFileIndexMax ];
  gfl2::std::MemClear( mFileInfo, sizeof(FileInfo) * mFileIndexMax );
#if PM_DEBUG
  m_isDebugDummySizeEnable = false;
  m_dummyDataSize = 0;
#endif
}



//--------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------
CTRSaveLoadControl::~CTRSaveLoadControl()
{
  GFL_DELETE_ARRAY( mFileInfo );
  GFL_DELETE(mpBackupSystem);
}



#if 0
//--------------------------------------------------------------
/**
 * @brief   初期化
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::Initialize(int indexmax)
{
  if(mpBackupSystem==NULL){
    mpBackupSystem = GFL_NEW(mpHeapBase) gfl2::fs::FsCtrBackupSystem(mpHeapBase);
  }
  mFileIndexMax = indexmax;
  mFileIndex    = 0;
  mFileInfo = GFL_NEW_ARRAY( mpHeapBase ) FileInfo[ mFileIndexMax ];
  gfl2::std::MemClear( mFileInfo, sizeof(FileInfo) * mFileIndexMax );
}
#endif

//--------------------------------------------------------------
/**
 * @brief セーブデータディレクトリの内容チェック
 * @param[in]   pTempHeap
 * @param[in]   pFileInfo
 * @param[in]   FileMax
 * @retval      true      内容に問題がない
 * @retval      false     サイズ、ファイル名などに問題がある
 */
//--------------------------------------------------------------
static bool CheckDirectory( gfl2::heap::HeapBase * pTempHeap,
     const FileInfo * pFileInfo, int FileMax )
{
  bool is_ok = true;
  gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pTempHeap ) gfl2::fs::FsCtrBackupFile();
  for ( int i = 0; i < FileMax; ++ i )
  {
    if ( pFile->Open( pFileInfo[i].name, gfl2::fs::FsCtrBackupFile::OPEN_MODE_READ ) == false )
    { //ファイルが開けない
      is_ok = false;
      break;
    }
    size_t size = pFile->GetSize();
    if ( size != pFileInfo[i].size )
    { //ファイルのサイズが違う
      is_ok = false;
      break;
    }
    pFile->Close();
  }
  GFL_DELETE pFile;
  return is_ok;
}

//--------------------------------------------------------------
/**
 * @brief セーブデータの状態を取得する
 * @param[in] pTempHeap 作業用ヒープ
 * @return  ARC_STATUS
 */
//--------------------------------------------------------------
SaveLoadControl::ARC_STATUS CTRSaveLoadControl::CheckArchiveStatus( gfl2::heap::HeapBase * pTempHeap )
{
  TAMADA_PRINT("CTRSaveLoadControl::CheckArchiveStatus\n");
  ARC_STATUS mArcStatus = ARC_STATUS_INCORRECT;

  nn::Result nn_result;

  size_t maxFiles, maxDirs;
  bool isDuplicate;
  TAMADA_PRINT("nn::fs::GetSaveDataFormatInfo()\n");
  nn_result = nn::fs::GetSaveDataFormatInfo( &maxFiles, &maxDirs, &isDuplicate );
  if ( nn_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( nn_result );
    if (nn::fs::ResultNotFormatted::Includes(nn_result))
    { // フォーマットしてない→初期化状態、フォーマットする
      mArcStatus = ARC_STATUS_NEED_INIT;
    }
    else if (nn::fs::ResultBadFormat::Includes(nn_result))
    { // 不正なフォーマット→フォーマットする
      mArcStatus = ARC_STATUS_INCORRECT;
    }
    else if (nn::fs::ResultVerificationFailed::Includes(nn_result))
    { // 検証失敗　or　改ざん→フォーマットする
      mArcStatus = ARC_STATUS_INCORRECT;
    }
    else
    {
      //上記以外のエラーは想定外→致命的エラーとする
      NN_ERR_THROW_FATAL(nn_result);
    }
    return mArcStatus;
  }
  TAMADA_PRINT("maxFiles = %d, maxDirs = %d, isDuplicate=%d\n",
      maxFiles, maxDirs, isDuplicate );
  // ディレクトリ数が１でない＝kujiraではありえない、フォーマットする
  if ( maxFiles != mFileIndexMax ) return mArcStatus;
  // ファイル数が一致しない＝kujiraではありえない、フォーマットする
  if ( maxDirs != 1 ) return mArcStatus;
  // 二重化されていない＝kujiraではありえない、フォーマットする
  if ( isDuplicate == false ) return mArcStatus;


  gfl2::fs::FsCtrBackupSystem::Result mount_result;
  mount_result = mpBackupSystem->MountSaveData();
  if ( mount_result != gfl2::fs::FsCtrBackupSystem::RESULT_OK )
  {
    if ( mount_result == gfl2::fs::FsCtrBackupSystem::RESULT_NG_NOT_FORMATTED )
    { //フォーマットされていない→初回フォーマットする
      mArcStatus = ARC_STATUS_NEED_INIT;
    }
    else
    { //それ以外→壊れている、フォーマットする
      mArcStatus = ARC_STATUS_INCORRECT;
    }
    return mArcStatus;
  }

  if (CheckDirectory( pTempHeap, mFileInfo, mFileIndexMax ) == true )
  {
    mArcStatus = ARC_STATUS_OK;
  }

  this->unmount();

  return mArcStatus;
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータアーカイブの非同期初期化処理開始
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::FormatStart( gfl2::heap::HeapBase * pTempHeap )
{
  //JFのセーブデータ書き換えを抑止:JFは動作していないはずだが、念のため
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->LockSavedataUpdating();

  //スリープ＆HOMEボタン禁止
  System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SAVE_ACC );
  m_threadSystemResult = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;
  this->StartThread( System::ThreadPriority::SAVE_THREAD_PRIORITY , THREAD_MODE_FORMAT, pTempHeap, NULL );
  TAMADA_PRINT("Save Format Start\n");
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータの非同期初期化処理メイン
 * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_OK       初期化処理完了
 * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE 初期化処理継続中
 * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_NG_xxxx  失敗
 */
//--------------------------------------------------------------
bool CTRSaveLoadControl::FormatMain( gfl2::fs::FsCtrBackupSystem::Result * res )
{
  if (this->IsAsyncFinished(THREAD_MODE_FORMAT) )
  {
    *res = m_threadSystemResult;
    //PSSのセーブデータ書き換え抑止を解除
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->UnlockSavedataUpdating();
    //スリープ＆HOMEボタン許可
    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SAVE_ACC );
    return true;
  }
  return false;
}

//--------------------------------------------------------------
/**
 * @brief   セーブ領域作成 もしくは作り直し
 * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_OK       セーブ開始
 * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_NG_xxxx  セーブ開始に失敗
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupSystem::Result CTRSaveLoadControl::FormatAll( gfl2::heap::HeapBase* pHeap )
{
  GFL_ASSERT( mFileIndexMax == mFileIndex );
  if ( pHeap == NULL ) pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );

  m_threadSystemResult = mpBackupSystem->Format(
      1, mFileIndexMax, gfl2::fs::FsCtrBackupSystem::DUPLICATE_ON );
  if ( m_threadSystemResult != gfl2::fs::FsCtrBackupSystem::RESULT_OK)
  {
    return m_threadSystemResult;
  }

  for(int i=0;i<mFileIndexMax;i++)
  {
    TAMADA_PRINT("Try Create SaveFile(%d)...\n", i );
    //ファイルを生成する
    mFileInfo[i].save_total->Clear( pHeap );
    m_threadSystemResult = this->CreateFile( pHeap, mFileInfo[i].save_total );
    if( gfl2::fs::FsCtrBackupSystem::RESULT_OK != m_threadSystemResult )
    {
      return m_threadSystemResult;
    }
  }
  m_threadSystemResult = gfl2::fs::FsCtrBackupSystem::RESULT_OK;
  return m_threadSystemResult;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupSystem::Result CTRSaveLoadControl::DeleteFile( gfl2::heap::HeapBase* pHeap, ISaveDataTotal * save_total )
{
  int file_id = GetEntryIndex( save_total );
  //Createの際にはまず削除
  m_threadSystemResult = mpBackupSystem->DeleteFsCtrBackupFile( mFileInfo[file_id].name );
    //成功でも存在しないでもない→致命的エラーのはず
    return m_threadSystemResult;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupSystem::Result CTRSaveLoadControl::CreateFile( gfl2::heap::HeapBase* pHeap, ISaveDataTotal * save_total )
{
  int file_id = GetEntryIndex( save_total );
  //Createの際にはまず削除
  m_threadSystemResult = this->DeleteFile( pHeap, save_total );
  if ( m_threadSystemResult != gfl2::fs::FsCtrBackupSystem::RESULT_NG_PATH_NOT_EXIST
      && m_threadSystemResult != gfl2::fs::FsCtrBackupSystem::RESULT_OK)
  {
    //成功でも存在しないでもない→致命的エラーのはず
    return m_threadSystemResult;
  }
  //ファイルを生成する
  m_threadSystemResult
    = mpBackupSystem->CreateFsCtrBackupFile( mFileInfo[file_id].name, mFileInfo[file_id].size );
  if( gfl2::fs::FsCtrBackupSystem::RESULT_OK != m_threadSystemResult )
  {
    return m_threadSystemResult;
  }
  //クリア済みデータをファイルに書き込む
  save_total->SetupNullData();
  // GFBTS1440
  //m_threadFileResult = this->SaveAtOnce( pHeap, save_total );
  {
    this->mount();
    gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pHeap ) gfl2::fs::FsCtrBackupFile();
    m_threadFileResult = this->Save( pFile, save_total, pHeap );
    GFL_DELETE pFile;
    this->commit( false );  //commitするがCRCはチェックしない
    this->unmount();
  }
  if ( gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK != m_threadFileResult )
  {
    //ファイルを作れた直後に失敗することは通常ありえない。
    GFL_ASSERT( 0 );
    m_threadSystemResult = gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR;
    return m_threadSystemResult;
  }
  return m_threadSystemResult;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::CreateStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  enterCS( save_total );  //セーブ中の変更禁止：開始
  this->StartThread( System::ThreadPriority::SAVE_THREAD_PRIORITY, THREAD_MODE_CREATE, pHeap, save_total );
  return true;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::CreateMain( gfl2::fs::FsCtrBackupSystem::Result * result )
{
  if (IsAsyncFinished(THREAD_MODE_CREATE))
  {
    leaveCS();  //セーブ中の変更禁止：解除
    *result = m_threadSystemResult;
    return true;
  }
  return false;
}


//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupFile::Result CTRSaveLoadControl::SaveAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  enterCS( save_total );  //セーブ中の変更禁止：開始

  save_total->SetupCheckData(); //整合性情報の生成

  this->mount();

  gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pHeap ) gfl2::fs::FsCtrBackupFile();
  m_threadFileResult = this->Save( pFile, save_total, pHeap );
  GFL_DELETE pFile;

  // 二重化している場合にはCommit操作が必要
  this->commit();

  this->unmount();

  leaveCS();  //セーブ中の変更禁止：解除

  return m_threadFileResult;
}

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupFile::Result CTRSaveLoadControl::LoadAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  this->mount();

  gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pHeap ) gfl2::fs::FsCtrBackupFile();
  m_threadFileResult = this->Load( pFile, save_total, pHeap );
  GFL_DELETE pFile;

  this->unmount();

  return m_threadFileResult;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::SaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  enterCS( save_total );  //セーブ中の変更禁止：開始

  save_total->SetupCheckData(); //整合性情報の生成

  this->StartThread( System::ThreadPriority::SAVE_THREAD_PRIORITY, THREAD_MODE_SAVE, pHeap, save_total );
  return true;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::SaveMain( gfl2::fs::FsCtrBackupFile::Result * result )
{
  if (IsAsyncFinished(THREAD_MODE_SAVE))
  {
    leaveCS();  //セーブ中の変更禁止：解除
    *result = m_threadFileResult;
    return true;
  }
  return false;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::LoadStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  this->StartThread( System::ThreadPriority::SAVE_THREAD_PRIORITY, THREAD_MODE_LOAD, pHeap, save_total );
  return true;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::LoadMain( gfl2::fs::FsCtrBackupFile::Result * result )
{
  if (IsAsyncFinished( THREAD_MODE_LOAD ))
  {
    *result = m_threadFileResult;
    return true;
  }
  return false;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::FirstSaveStart(
    gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  enterCS( save_total );  //セーブ中の変更禁止：開始

  save_total->SetupCheckData(); //整合性情報の生成

  this->StartThread( System::ThreadPriority::SAVE_THREAD_PRIORITY,
      THREAD_MODE_SAVE_FIRST, pHeap, save_total );
  return true;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::FirstSaveMain( gfl2::fs::FsCtrBackupFile::Result * result )
{
  if (IsAsyncFinished(THREAD_MODE_SAVE_FIRST))
  {
    *result = m_threadFileResult;
    return true;
  }
  return false;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void CTRSaveLoadControl::FirstSaveCancel( void )
{
  leaveCS();  //セーブ中の変更禁止：解除
  this->unmount();  //FirstSaveStartでマウントしているのでunmount
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::LastSaveStart(
    gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  this->StartThread( System::ThreadPriority::SAVE_THREAD_PRIORITY,
      THREAD_MODE_SAVE_LAST, pHeap, save_total );
  return true;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
bool CTRSaveLoadControl::LastSaveMain( gfl2::fs::FsCtrBackupFile::Result * result )
{
  if (IsAsyncFinished(THREAD_MODE_SAVE_LAST))
  {
    leaveCS();  //セーブ中の変更禁止：解除
    *result = m_threadFileResult;
    return true;
  }
  return false;
}

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::LastSave( ISaveDataTotal * save_total )
{
  // 二重化している場合にはCommit操作が必要
  this->commit();

  this->unmount();

  leaveCS();  //セーブ中の変更禁止：解除
}

//--------------------------------------------------------------
/**
 * @brief   ファイルチェック
 * @retval  ファイルが無ければfalse 初期化シーケンスにいく
 * @retval  ファイルがあればtrue 読み込み
 *
 */
//--------------------------------------------------------------
bool CTRSaveLoadControl::CheckFileExist( ISaveDataTotal * save_total )
{
  //int index = GetEntryIndex( save_total );
  //return mpBackupSystem->CheckBackupFileExist( mFileInfo[index].name );
  return ( save_total->GetStatus() == ISaveDataTotal::STATE_OK );
}

//--------------------------------------------------------------
/**
 * @brief   バックアップエントリ追加
 * @param   save_total  データ
 * @param   name        バックアップに使用するファイル名
 *
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::AddBackupEntry( ISaveDataTotal * save_total, const wchar_t * name )
{
  GFL_ASSERT( mFileIndex < mFileIndexMax );
  int num = save_total->GetSaveObjectNum();
  u32 size_count = 0;
  size_t align = save_total->GetAlignmentSize();
  for (int index = 0; index < num; ++ index )
  {
    SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );
    GFL_PRINT( "savedatasize[%d][%d][%d]\n",index,pSaveObj->GetDataSize(), nn::math::RoundUp( pSaveObj->GetDataSize(), align ));
    size_count += nn::math::RoundUp( pSaveObj->GetDataSize(), align );
  }
  GFL_PRINT("total[%d]\n",size_count);
  mFileInfo[ mFileIndex ].save_total = save_total;
  mFileInfo[ mFileIndex ].size = size_count;
  ::std::wcsncpy( mFileInfo[mFileIndex].name, L"data:/", FileInfo::FILENAME_LEN );
  ::std::wcsncat( mFileInfo[mFileIndex].name, name, FileInfo::FILENAME_LEN - 6 );
  mFileIndex++;
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
 * @brief セーブ中の変更禁止：開始
 *
 * @note enterCS == enter CriticalSession 的な命名
 * @attention PSSは別スレッドからのアクセスがあるので真っ先に抑止する
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::enterCS( ISaveDataTotal * save_total )
{
  GFL_ASSERT( m_threadTarget == NULL );

  //PSSのセーブデータ書き換えを抑止
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->LockSavedataUpdating();

  //スリープ＆HOMEボタン禁止
  System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SAVE_ACC );

  m_threadTarget = save_total;
  m_threadTarget->LockData();
}

//--------------------------------------------------------------
/**
 * @brief セーブ中の変更禁止：解除
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::leaveCS( void )
{
  GFL_ASSERT( m_threadTarget != NULL );

  //セーブ中に書き換えが発生していないか検証するためのAssert
  s32 error_info;
  if( m_threadTarget->GetStatus( &error_info ) != ISaveDataTotal::STATE_OK )
  {
    GFL_ASSERT_MSG(0, "Save Broken:%d\n", error_info );
  }

  m_threadTarget->UnlockData();
  m_threadTarget = NULL;

  //スリープ＆HOMEボタン許可
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_SAVE_ACC );


  //PSSのセーブデータ書き換え抑止を解除
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->UnlockSavedataUpdating();
}

//--------------------------------------------------------------
/**
 * @brief セーブデータのマウント
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::mount( void )
{
  gfl2::fs::FsCtrBackupSystem::Result result;
  result = mpBackupSystem->MountSaveData();
  if ( result != gfl2::fs::FsCtrBackupSystem::RESULT_OK )
  {
    //マウントの失敗は製品版に残すべきでない、想定外のエラー
    GFL_ASSERT( 0 );
    NN_ERR_THROW_FATAL_ALL( mpBackupSystem->GetLastNNResult() );
  }
}

//--------------------------------------------------------------
/**
 * @brief セーブデータのアンマウント
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::unmount( void )
{
  gfl2::fs::FsCtrBackupSystem::Result result;
  result = mpBackupSystem->UnMountSaveData();
  if ( result != gfl2::fs::FsCtrBackupSystem::RESULT_OK )
  {
    //マウント解除の失敗は製品版に残すべきでない、想定外のエラー
    GFL_ASSERT( 0 );
    NN_ERR_THROW_FATAL_ALL( mpBackupSystem->GetLastNNResult() );
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void CTRSaveLoadControl::commit( bool assert_crc_failure )
{
  //セーブ中に書き換えが発生していないか検証するためのAssert
  s32 error_info;
  if( assert_crc_failure
      && m_threadTarget->GetStatus( &error_info ) != ISaveDataTotal::STATE_OK )
  {
    //製品版でもCommitはしないようにここで止めてしまう
    GFL_ASSERT_STOP_MSG(0, "Save Broken:%d\n", error_info );
  }

  gfl2::fs::FsCtrBackupSystem::Result result;
  result = mpBackupSystem->CommitSaveData();
  if ( result != gfl2::fs::FsCtrBackupSystem::RESULT_OK )
  {
    //Commitの失敗は製品版に残すべきでない、想定外のエラー
    NN_ERR_THROW_FATAL_ALL( mpBackupSystem->GetLastNNResult() );
    GFL_ASSERT_STOP( 0 );
  }
}

//--------------------------------------------------------------
/**
 * @brief セーブ全体データのポインタをサーチ、インデックス取得
 */
//--------------------------------------------------------------
int CTRSaveLoadControl::GetEntryIndex( const ISaveDataTotal * save_total )
{
  for (int i = 0; i < mFileIndexMax; ++ i )
  {
    if ( mFileInfo[i].save_total == save_total ) return i;
  }
  GFL_ASSERT_STOP( 0 );
  return 0;
}
//--------------------------------------------------------------
/**
 * @brief バックアップ処理スレッド生成
 * @param priority    スレッドのプライオリティ
 * @param mode        モード
 * @param pHeap       使用する作業用ヒープ
 * @param save_total   セーブデータへのポインタ
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::StartThread(
    u8 priority, THREAD_MODE mode, gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  GFL_ASSERT_STOP( m_threadMode == THREAD_MODE_NOTHING );
  m_threadMode = mode;
  m_pMyThread  = GFL_NEW( pHeap ) CTRSaveLoadControlThread( pHeap, 4096, this, save_total );
  m_pMyThread->Start( priority );
}



//--------------------------------------------------------------
/**
 * @brief スレッド終了待ち
 * @return  bool  trueのとき終了している
 */
//--------------------------------------------------------------
bool CTRSaveLoadControl::IsAsyncFinished( THREAD_MODE mode )
{
  GFL_ASSERT_STOP( mode == m_threadMode );
  if (!m_pMyThread) return true;
  if (m_pMyThread->IsAlive() ) return false;

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
void CTRSaveLoadControl::ThreadCallBack(gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  switch ( m_threadMode )
  {
  case THREAD_MODE_NOTHING:
    GFL_ASSERT(0);
    return;

  case THREAD_MODE_FORMAT:
    m_threadSystemResult = this->FormatAll( pHeap );
    TAMADA_PRINT("FormatAll()[%d]", m_threadSystemResult);
    break;

  case THREAD_MODE_CREATE:
    {
      m_threadSystemResult = this->CreateFile( pHeap, save_total );
      TAMADA_PRINT("CTRSaveLoadControl::CreateFile()[%d]", m_threadSystemResult);
    }
    break;

  case THREAD_MODE_LOAD:
    {
      this->mount();

      gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pHeap ) gfl2::fs::FsCtrBackupFile();
      m_threadFileResult = this->Load( pFile, save_total, pHeap );
      GFL_DELETE pFile;

      this->unmount();
    }
    break;

  case THREAD_MODE_SAVE:
    {
      this->mount();

      gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pHeap ) gfl2::fs::FsCtrBackupFile();
      m_threadFileResult = this->Save( pFile, save_total, pHeap );
      GFL_DELETE pFile;

      // 二重化している場合にはCommit操作が必要
      this->commit();

      this->unmount();
    }
    break;

  case THREAD_MODE_SAVE_FIRST:
    {
      this->mount();

      gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pHeap ) gfl2::fs::FsCtrBackupFile();
      m_threadFileResult = this->Save( pFile, save_total, pHeap );
      GFL_DELETE pFile;
    }
    break;

  case THREAD_MODE_SAVE_LAST:
    {
      // 二重化している場合にはCommit操作が必要
      this->commit();

      this->unmount();
    }


  }
}

//--------------------------------------------------------------
/**
 * @brief ロード処理本体
 * @note  スレッドから呼び出される
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupFile::Result CTRSaveLoadControl::Load(
    gfl2::fs::FsCtrBackupFile * pFile, ISaveDataTotal * save_total, gfl2::heap::HeapBase * pHeap )
{
  int file_id = GetEntryIndex( save_total );
  if ( pFile->Open( mFileInfo[file_id].name, gfl2::fs::FsCtrBackupFile::OPEN_MODE_READ ) == false )
  {
    TAMADA_PRINT("gfl2::fs::FsCtrBackupFile::Open = %d\n", pFile->GetLastAccessResult() );
    return pFile->GetLastAccessResult();
  }

  int max = save_total->GetSaveObjectNum();
  size_t align = save_total->GetAlignmentSize();
  u32 offset = 0;
  for (int index = 0; index < max; ++ index )
  {
    SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );
    u32 size = pSaveObj->GetDataSize();
    void * data = pSaveObj->GetData();
    pFile->Seek( gfl2::fs::FsCtrBackupFile::SEEK_BASE_BEGIN, offset );
    if ( pFile->GetLastAccessResult() != gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK )
    {
      TAMADA_PRINT("gfl2::fs::FsCtrBackupFile::Seek = %d\n", pFile->GetLastAccessResult() );
      return pFile->GetLastAccessResult();
    }
    TAMADA_PRINT("Read (%2d) %06x: %06x\n", index, offset, size );
    u32 readsize = pFile->Read( data, size );
    if ( pFile->GetLastAccessResult() != gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK )
    {
      TAMADA_PRINT("gfl2::fs::FsCtrBackupFile::Read = %d\n", pFile->GetLastAccessResult() );
      return pFile->GetLastAccessResult();
    }
    else if ( readsize != size )
    { // セーブデータを修正するとサイズが違うのでエラーにする
      TAMADA_PRINT("gfl2::fs::FsCtrBackupFile::Read: size error(%d) %d != %d\n", index, readsize, size );
      return gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_NG_BROKEN_DATA;
    }
    offset += nn::math::RoundUp( size, align );
  }
  pFile->Close();
  TAMADA_PRINT("gfl2::fs::FsCtrBackupFile::Close = %d\n", pFile->GetLastAccessResult() );
  return pFile->GetLastAccessResult();
}

//--------------------------------------------------------------
/**
 * @brief セーブ処理本体
 * @note  スレッドから呼び出される
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupFile::Result CTRSaveLoadControl::Save(
    gfl2::fs::FsCtrBackupFile * pFile, ISaveDataTotal * save_total, gfl2::heap::HeapBase * pHeap )
{
  int file_id = GetEntryIndex( save_total );
  if ( pFile->Open( mFileInfo[file_id].name, gfl2::fs::FsCtrBackupFile::OPEN_MODE_WRITE_ADD ) == false )
  {
    TAMADA_PRINT("gfl2::fs::FsCtrBackupFile::Open = %d\n", pFile->GetLastAccessResult() );
    return pFile->GetLastAccessResult();
  }

  GFL_PRINT("CTRSaveLoadControl::Save : FsCtrBackupFile::GetSize[%d]\n",pFile->GetSize() );

  int max = save_total->GetSaveObjectNum();
  size_t align = save_total->GetAlignmentSize();
  u32 offset = 0;
  for (int index = 0; index < max; ++ index )
  {
    SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );
    u32 size = pSaveObj->GetDataSize();
    void * data = pSaveObj->GetData();
    GFL_PRINT("Write (%2d) %06x: %06x\n", index, offset, size );
    pFile->Seek( gfl2::fs::FsCtrBackupFile::SEEK_BASE_BEGIN, offset );
    if ( pFile->GetLastAccessResult() != gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK )
    {
      GFL_PRINT("gfl2::fs::FsCtrBackupFile::Seek = %d\n", pFile->GetLastAccessResult() );
      pFile->Close();
      return pFile->GetLastAccessResult();
    }
    bool flush = ( index + 1 == max );  //最後だけflushする
#if PM_DEBUG
    if( m_isDebugDummySizeEnable )
    {
      flush = false;
    }
#endif
    u32 writed = pFile->Write( data, size, flush );
    GFL_ASSERT_STOP( writed == size );
    if ( pFile->GetLastAccessResult() != gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK )
    {
      GFL_PRINT("gfl2::fs::FsCtrBackupFile::Write = %d\n", pFile->GetLastAccessResult() );
      pFile->Close();
      return pFile->GetLastAccessResult();
    }
    offset += nn::math::RoundUp( size, align );
  }
#if PM_DEBUG
  if( m_isDebugDummySizeEnable )
  {
    pFile->Seek( gfl2::fs::FsCtrBackupFile::SEEK_BASE_BEGIN, offset );
    if ( pFile->GetLastAccessResult() != gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK )
    {
      GFL_PRINT("gfl2::fs::FsCtrBackupFile::debug Seek = %d\n", pFile->GetLastAccessResult() );
      pFile->Close();
      return pFile->GetLastAccessResult();
    }
    s64 size = 0;
    nn::fs::GetArchiveFreeBytes( &size , "data:" );
    if( size > 0 )
    {
      m_dummyDataSize = size;
    }
    GFL_PRINT("DummySize[%lld]\n",m_dummyDataSize);
    u8* data = GFL_NEW_ARRAY( pHeap ) u8[m_dummyDataSize];
    GFL_PRINT("Write Dummy (%2d) %06x: %06x\n", save_total->GetSaveObjectNum() , offset, m_dummyDataSize );
    u32 writed = pFile->Write( data, m_dummyDataSize, true );
    GFL_SAFE_DELETE_ARRAY( data );
    GFL_ASSERT_STOP( writed == m_dummyDataSize );
    if ( pFile->GetLastAccessResult() != gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK )
    {
      GFL_PRINT("gfl2::fs::FsCtrBackupFile::debug Write = %d\n", pFile->GetLastAccessResult() );
      pFile->Close();
      return pFile->GetLastAccessResult();
    }
  }
#endif
  pFile->Close();
  TAMADA_PRINT("gfl2::fs::FsCtrBackupFile::Close = %d\n", pFile->GetLastAccessResult() );
  return pFile->GetLastAccessResult();
}


//--------------------------------------------------------------
/**
 * @brief セーブデータを書き込むためのバッファサイズを取得する
 *
 * @param   save_total         セーブデータアクセスクラス
 * @param   isEnablePokemonBox ボックスデータを含めるならtrueを指定
 */
//--------------------------------------------------------------
u32 CTRSaveLoadControl::GetSaveDataBufferSize( ISaveDataTotal * save_total, bool isEnablePokemonBox )
{
  int    max    = save_total->GetSaveObjectNum();
  size_t align  = save_total->GetAlignmentSize();
  u32    totalSize = 0;

  for (int index = 0; index < max; ++ index )
  {
    if( !isEnablePokemonBox && (index == Savedata::SaveData::SAVEDATA_ID_POKEMON_BOX) ){
      continue; // BOXデータを含めず
    }
    //if( index == savedata::SaveData::SAVEDATA_ID_PHOTO_DATA ){
    //  continue; // 写真データは含めず
    //}
    SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );

    totalSize += nn::math::RoundUp( pSaveObj->GetDataSize(), align );
  }
  return totalSize;
}


//--------------------------------------------------------------
/**
 * @brief バッファへセーブデータ書き込み（PGL通信用）
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::SaveToBuffer( u8* pBuffer, ISaveDataTotal * save_total, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox )
{
  int    max    = save_total->GetSaveObjectNum();
  size_t align  = save_total->GetAlignmentSize();

  for (int index = 0; index < max; ++ index )
  {
    if( !isEnablePokemonBox && (index == Savedata::SaveData::SAVEDATA_ID_POKEMON_BOX) ){
      continue; // BOXデータを含めず
    }
    //if( index == savedata::SaveData::SAVEDATA_ID_PHOTO_DATA ){
    //  continue; // 写真データは含めず
    //}
    SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );

    u32   size = pSaveObj->GetDataSize();
    void* data = pSaveObj->GetData();

    //if( index == savedata::SaveData::SAVEDATA_ID_MY_PHOTO_ICON ){
    //  ::std::memset( pBuffer, 0, size );  // COPPA対応の為、写真アイコン情報は0クリアする
    //}
    //else
    {
      ::std::memcpy( pBuffer, data, size );
    }
    GFL_PRINT("Write[%2d] %u\n", index, size );

    pBuffer += nn::math::RoundUp( size, align );
  }
}

#if PM_DEBUG
//--------------------------------------------------------------
/**
 * @brief バッファからセーブデータを読み込む（PGLデバッグ用）
 */
//--------------------------------------------------------------
void CTRSaveLoadControl::LoadFromBuffer( const u8* pBuffer, ISaveDataTotal * save_total, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox )
{
  int    max    = save_total->GetSaveObjectNum();
  size_t align  = save_total->GetAlignmentSize();

  for (int index = 0; index < max; ++ index )
  {
    if( !isEnablePokemonBox && (index == Savedata::SaveData::SAVEDATA_ID_POKEMON_BOX) ){
      continue; // BOXデータを含めず
    }
    //if( index == savedata::SaveData::SAVEDATA_ID_PHOTO_DATA ){
    //  continue; // 写真データは含めず
    //}
    SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );

    u32   size = pSaveObj->GetDataSize();
    void* data = pSaveObj->GetData();
    ::std::memcpy( data, pBuffer, size );

    GFL_PRINT("Load[%2d] %u\n", index, size );

    pBuffer += nn::math::RoundUp( size, align );
  }
}






//--------------------------------------------------------------
//--------------------------------------------------------------
wchar_t * CTRSaveLoadControl::GetFileName( int index ) const
{
  return mFileInfo[index].name;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
size_t CTRSaveLoadControl::GetFileSize( int index ) const
{
  return mFileInfo[index].size;
}

//--------------------------------------------------------------
/**
 * @brief セーブデータを保護せず書き換える
 * @attention デバッグ用：セーブ破壊のエミュレーションを想定して用意
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupFile::Result CTRSaveLoadControl::SaveRawDataAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  this->mount();

  gfl2::fs::FsCtrBackupFile * pFile = GFL_NEW( pHeap ) gfl2::fs::FsCtrBackupFile();
  m_threadFileResult = this->Save( pFile, save_total, pHeap );
  GFL_DELETE pFile;

  // 二重化している場合にはCommit操作が必要
  this->commit( false );  //commitするがCRCはチェックしない

  this->unmount();

  return m_threadFileResult;
}


#endif


} //namespace savedata

#endif
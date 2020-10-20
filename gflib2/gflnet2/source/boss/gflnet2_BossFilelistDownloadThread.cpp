#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossFilelistDownloadThread.cpp
 * @brief  BOSSサーバファイルリストダウンロードスレッド
 * @author Shin Kosaka
 * @date   2012.8.03
 */
//=============================================================================
#include "gflnet2_BossFilelistDownloadThread.h"

#include <nn/boss.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)


//static const char NSA_FILELIST_NAME[] = ".txt";
static const char NSA_FILELIST_NAME[] = "filelist.txt";

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 */
//--------------------------------------------------------------------------------
BossFilelistDownloadThread::BossFilelistDownloadThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , nn::boss::NsaList::NsaInformation* pNsaListInfo , u32 nsaListLength , const char* pTaskName , const BossManager::BOSS_SEARCH_ATTRIBUTES* pAttributes )
: m_pNsaListInfo( pNsaListInfo )
, m_nsaListLength( nsaListLength )
, m_isSuccess(false)
, m_fileNum(0)
, m_pNsaList(NULL)
, m_result()
, m_pThread(NULL)
, m_errorCode(0)
{
  ::std::strncpy( m_taskName , pTaskName , nn::boss::TASK_ID_LENGTH - 1 );
  ::std::memset( m_workBuffer , 0 , sizeof(m_workBuffer) );
  ::std::memset( m_filePath , 0 , sizeof(m_filePath) );
  ::std::memset( m_attribute1 , 0 , sizeof(m_attribute1) );
  ::std::memset( m_attribute2 , 0 , sizeof(m_attribute2) );
  ::std::memset( m_attribute3 , 0 , sizeof(m_attribute3) );

  if( pAttributes )
  {
    if( pAttributes->pAttribute1 )
    {
      ::std::memcpy( m_attribute1 , pAttributes->pAttribute1 , sizeof(m_attribute1)-1 );
      GFL_PRINT("BossFilelistDownloadThread::BossFilelistDownloadThread : attribute1[%s]\n",m_attribute1);
    }
    if( pAttributes->pAttribute2 )
    {
      ::std::memcpy( m_attribute2 , pAttributes->pAttribute2 , sizeof(m_attribute2)-1 );
      GFL_PRINT("BossFilelistDownloadThread::BossFilelistDownloadThread : attribute2[%s]\n",m_attribute2);
    }
    if( pAttributes->pAttribute3 )
    {
      ::std::memcpy( m_attribute3 , pAttributes->pAttribute3 , sizeof(m_attribute3)-1 );
      GFL_PRINT("BossFilelistDownloadThread::BossFilelistDownloadThread : attribute3[%s]\n",m_attribute3);
    }
  }

  const char* pExtSaveDataMountName = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->GetMountName();
  //const char* pBossCode = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->GetBossCode();
  //sprintf( (char*)m_filePath , "%s/%s%s" , pExtSaveDataMountName , pBossCode , NSA_FILELIST_NAME );
  ::std::sprintf( (char*)m_filePath , "%s/%s",pExtSaveDataMountName , NSA_FILELIST_NAME );

  m_pNsaList = GFL_NEW( pHeap ) nn::boss::NsaList((const char*)m_filePath);// nsaList((const char*)m_filePath);//[マウント名]/[BOSSコード]NsaFilelist.txt というファイルパスに保存

  m_pThread = GFL_NEW(pHeap) gfl2::thread::Thread(this, pHeap, true, stackSize);
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
BossFilelistDownloadThread::~BossFilelistDownloadThread()
{
  GFL_SAFE_DELETE( m_pNsaList );
  GFL_SAFE_DELETE( m_pThread );
}

//--------------------------------------------------------------------------------
/**
* @brief スレッド開始
*/
//--------------------------------------------------------------------------------
void BossFilelistDownloadThread::Start()
{
  m_pThread->Start();
}

//--------------------------------------------------------------------------------
/**
* @brief 成否取得
*/
//--------------------------------------------------------------------------------
bool BossFilelistDownloadThread::IsSuccess() const
{
  return m_isSuccess;
}

//--------------------------------------------------------------------------------
/**
* @brief リザルトコード取得
*/
//--------------------------------------------------------------------------------
nn::Result BossFilelistDownloadThread::GetResult() const
{
  return m_result;
}

//--------------------------------------------------------------------------------
/**
* @brief 実行タスク名取得
*/
//--------------------------------------------------------------------------------
const char* BossFilelistDownloadThread::GetTaskName() const
{
  return m_taskName;
}

//--------------------------------------------------------------------------------
/**
* @brief ファイル数取得
*/
//--------------------------------------------------------------------------------
u32 BossFilelistDownloadThread::GetFileNum() const
{
  return m_fileNum;
}

//--------------------------------------------------------------------------------
/**
* @brief ダウンロードのキャンセル
*/
//--------------------------------------------------------------------------------
nn::Result BossFilelistDownloadThread::CancelDownload()
{
  if( m_pNsaList )
  {
    return m_pNsaList->Cancel();
  }

  return nn::Result();
}


//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void BossFilelistDownloadThread::Function( void )
{
  if( !m_pNsaList )
  {
    GFL_PRINT("BossFilelistDownloadThread::Main : error m_pNsaList is NULL\n");
    m_pThread->Kill();

    return;
  }

  const char* pBossCode = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->GetBossCode();

  m_isSuccess = false;


  // NSA リストのダウンロード
  {
    // BOSS コードとタスク ID を指定して NSA リストをダウンロードします。
    nn::boss::NsaList::SearchAttributes attribute;
    ::std::memset( &attribute , 0 , sizeof( attribute ));
    nn::boss::NsaList::SearchAttributes* pAttribute = NULL;
    if( m_attribute1[0] != 0 )
    {
      attribute.attribute1 = m_attribute1;
      pAttribute = &attribute;
      GFL_PRINT("BossFilelistDownloadThread::Main : Set Attribute1[%s]\n",pAttribute->attribute1);
    }
    if( m_attribute2[0] != 0 )
    {
      attribute.attribute2 = m_attribute2;
      pAttribute = &attribute;
      GFL_PRINT("BossFilelistDownloadThread::Main : Set Attribute2[%s]\n",pAttribute->attribute2);
    }
    if( m_attribute3[0] != 0 )
    {
      attribute.attribute3 = m_attribute3;
      pAttribute = &attribute;
      GFL_PRINT("BossFilelistDownloadThread::Main : Set Attribute3[%s]\n",pAttribute->attribute3);
    }
    GFL_PRINT("BossFilelistDownloadThread::Main m_pNsaList->Download( [%s] , [%s] )\n", pBossCode , m_taskName );
    m_result = m_pNsaList->Download( pBossCode, m_taskName , pAttribute , nn::boss::NsaList::NN_BOSS_NSALIST_WAIT_FINISH_TIMEOUT_DEFAULT_SEC , nn::boss::NsaList::NN_BOSS_NSALIST_MAX_SIZE );
    if (m_result.IsFailure())
    {
      GFL_PRINT("BossFilelistDownloadThread::Main : error Download failed(0x%08x)\n", m_result.GetPrintableBits());
      m_pThread->Kill();

      nn::boss::GetErrorCode( &m_errorCode , m_pNsaList->GetResult() );

      return;
    }
  }


    // NSA リストのダウンロードエラーチェック
    {
        if( m_pNsaList->GetResult() != nn::boss::TASK_SUCCESS )
        {
            GFL_PRINT("nn::boss::NsaList::Download failed\n");
            GFL_PRINT("nn::boss::NsaList::GetResult(%d)\n", m_pNsaList->GetResult());
        }
    }

    // NSA リストの正当性のチェック
    {
        memset( m_workBuffer , 0 , sizeof(m_workBuffer) );
        if( !m_pNsaList->CheckValidity( (void *)m_workBuffer , sizeof(m_workBuffer) ) )
        {
            // エラー
            GFL_PRINT("nn::boss::NsaList::CheckValidity failed\n");
        }
    }

    // ダウンロードした NSA リストをパース
    m_fileNum = 0;
    {
        memset( m_workBuffer , 0 , sizeof(m_workBuffer) );
        s32 parseResult = m_pNsaList->Parse( &m_fileNum , m_pNsaListInfo , m_nsaListLength , (void *)m_workBuffer , sizeof(m_workBuffer) );
        if( parseResult < 0 )
        {
            // エラー
            GFL_PRINT("nn::boss::NsaList::Parse failed(%d)\n", parseResult);
          m_pThread->Kill();
          return;
        }
    }

  GFL_PRINT( "BossFilelistDownloadThread::Main NsaList Num = %d\n" , m_fileNum );


  //if( m_fileNum > 0 )
  {//ファイルが１つ以上ある
    m_isSuccess = true;
  }
  /*//ファイルが１つも無くても成功
  else
  {//ファイルが１つもない
    m_result = nn::Result();
  }
  */

  m_pThread->Kill();
}




GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif

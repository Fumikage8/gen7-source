#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossFileReadThread.cpp
 * @brief  BOSSファイル読み込みスレッド
 * @author Shin Kosaka
 * @date   2012.8.06
 */
//=============================================================================
#include "gflnet2_BossFileReadThread.h"

#include <nn/boss.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 */
//--------------------------------------------------------------------------------
BossFileReadThread::BossFileReadThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , const u32 serialId )
: m_pHeap(pHeap)
, m_isSuccess(false)
, m_serialId(serialId)
, m_pDataBody(NULL)
, m_readSize(0)
, m_result()
, m_contentsData()
, m_pThread(NULL)
//, m_pDataHeader(NULL)
{
  memset( &m_dataHeader , 0 , sizeof(m_dataHeader) );
  //memset( m_workBuffer , 0 , sizeof(m_workBuffer) );

  m_pThread = GFL_NEW(m_pHeap) gfl2::thread::Thread(this, pHeap, true, stackSize);
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
BossFileReadThread::~BossFileReadThread()
{
  if( m_pDataBody )
  {
    GflHeapFreeMemory( m_pDataBody );
    m_pDataBody=NULL;
  }

  GFL_SAFE_DELETE( m_pThread );
}


//--------------------------------------------------------------------------------
/**
* @brief スレッド開始
*/
//--------------------------------------------------------------------------------
void BossFileReadThread::Start()
{
  m_pThread->Start();
}

//--------------------------------------------------------------------------------
/**
* @brief 成否取得
*/
//--------------------------------------------------------------------------------
bool BossFileReadThread::IsSuccess() const
{
  return m_isSuccess;
}

//--------------------------------------------------------------------------------
/**
* @brief リザルトコード取得
*/
//--------------------------------------------------------------------------------
nn::Result BossFileReadThread::GetResult() const
{
  return m_result;
}

//--------------------------------------------------------------------------------
/**
* @brief データヘッダ取得
*/
//--------------------------------------------------------------------------------
const nn::boss::NsDataHeaderInfo* BossFileReadThread::GetDataHeader() const
{
  return &m_dataHeader;
}

//--------------------------------------------------------------------------------
/**
* @brief データ取得
*/
//--------------------------------------------------------------------------------
const void* BossFileReadThread::GetDataBody() const
{
  return m_pDataBody;
}

//--------------------------------------------------------------------------------
/**
* @brief データサイズ取得
*/
//--------------------------------------------------------------------------------
size_t BossFileReadThread::GetReadSize() const
{
  return m_readSize;
}

//--------------------------------------------------------------------------------
/**
* @brief コンテンツ削除
*/
//--------------------------------------------------------------------------------
void BossFileReadThread::DeleteContentsData()
{
  m_result = m_contentsData.Delete();
  if( !m_result.IsSuccess() )
  {
    GFL_PRINT("BossFileReadThread::DeleteContentsData():error m_contentsData.Delete() is failed\n");
  }
}



//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void BossFileReadThread::Function( void )
{
  m_isSuccess = false;

  m_result = m_contentsData.Initialize( m_serialId );
  if( !m_result.IsSuccess() )
  {
    GFL_PRINT("BossFileReadThread::Main : error m_contentsData.Initialize() failed\n");
    DeleteContentsData();
    m_pThread->Kill();
    return;
  }

  //ヘッダ内容の読み込み
  {
    memset( &m_dataHeader , 0 , sizeof(m_dataHeader) );
      m_result = m_contentsData.GetHeaderInfo( nn::boss::NSD_ALL , &m_dataHeader , sizeof(m_dataHeader) );
    if( !m_result.IsSuccess() )
    {
      GFL_PRINT("BossFileReadThread::Main : error m_contentsData.GetHeaderInfo() failed\n");
      DeleteContentsData();
      m_isSuccess = true;//無効データ削除時は成功
      m_pThread->Kill();
      return;
    }
  }

  //データ内容の読み込み
  {
    m_readSize = 0;
    m_pDataBody = GflHeapAllocMemoryAlign4096( m_pHeap , m_dataHeader.length );//ここだけヒープ変えるかも
    memset( m_pDataBody , 0 , m_dataHeader.length );
    size_t readSize = 0;
    u32 readCount = 0;
    u8* pReadPoint = (u8*)m_pDataBody;
    size_t tempSize = m_dataHeader.length;
    do
    {
        ++readCount;
        //GFL_PRINT("(Read %d)\n", readCount);
        readSize = m_contentsData.ReadData( pReadPoint , tempSize );
      switch( (s32)readSize )
      {
      case nn::boss::NsData::NN_BOSS_NSDATA_READ_ERROR_GET_HEADER://データヘッダの取得に失敗しました。
      case nn::boss::NsData::NN_BOSS_NSDATA_READ_ERROR_READ_DATA://データの読み込みに失敗しました。
      case nn::boss::NsData::NN_BOSS_NSDATA_READ_ERROR_IPC:// 内部エラーが発生しました。
        {
          GFL_PRINT("BossFileReadThread::Main : error m_contentsData.ReadData() failed[0x%x]\n" , readSize );
          DeleteContentsData();
          m_pThread->Kill();
          return;
        }
      //break;
      case nn::boss::NsData::NN_BOSS_NSDATA_READ_ERROR_UPDATED:// 前回のデータ取得時以降に、対象NSDのバージョンが更新されました。
        {
          GFL_PRINT("BossFileReadThread::Main : NN_BOSS_NSDATA_READ_ERROR_UPDATED\n" );
          /*
          //初期化して再度読み込み
          memset( m_pDataBody , 0 , m_dataHeader.length );
          pReadPoint = m_pDataBody;
          tempSize = m_dataHeader.length;
          */
          GFL_PRINT("BossFileReadThread::Main : error m_contentsData.ReadData() failed[0x%x]\n" , readSize );
          DeleteContentsData();
          m_pThread->Kill();
          return;
        }
      //break;
      default:
        {
          if( (s32)readSize < 0 )
          {//未知のエラー
            GFL_PRINT("BossFileReadThread::Main : error m_contentsData.ReadData() failed[0x%x]\n" , readSize );
            DeleteContentsData();
            m_pThread->Kill();
            return;
          }
          else
          {//正常進行
            pReadPoint += readSize;
            tempSize -= readSize;
            m_readSize += readSize;
          }
        }
      break;
      };

    }while(readSize != 0);
  }

  GFL_PRINT("BossFileReadThread::Main : FileRead Complete size[%u]addr[0x%x]\n" , m_readSize , m_pDataBody);
  m_isSuccess = true;

  m_pThread->Kill();
}




GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif

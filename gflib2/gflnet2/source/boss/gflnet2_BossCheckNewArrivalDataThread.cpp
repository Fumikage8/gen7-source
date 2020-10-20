#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_BossCheckNewArrivalDataThread.cpp
 * @brief  新着データ取得スレッド
 * @author Shin Kosaka
 * @date   2012.8.08
 */
//=============================================================================
#include "gflnet2_BossCheckNewArrivalDataThread.h"

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
BossCheckNewArrivalDataThread::BossCheckNewArrivalDataThread( gfl2::heap::HeapBase* pHeap, u32 stackSize , u32* pSerialIDList , u32 idListSize )
: m_pHeap(pHeap)
, m_isSuccess(false)
//, m_pSerialIDListBuffer(pSerialIDList)
//, m_idListBufferSize(idListSize)
, m_result(nn::ResultSuccess())
, m_serialIdList(pSerialIDList,idListSize)
, m_pThread(NULL)
{
  m_serialIdList.Initialize();

  m_pThread = GFL_NEW(m_pHeap) gfl2::thread::Thread(this, pHeap, true, stackSize);
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
BossCheckNewArrivalDataThread::~BossCheckNewArrivalDataThread()
{
  GFL_SAFE_DELETE( m_pThread );
}

//--------------------------------------------------------------------------------
/**
* @brief スレッド開始
*/
//--------------------------------------------------------------------------------
void BossCheckNewArrivalDataThread::Start()
{
  m_pThread->Start();
}

//--------------------------------------------------------------------------------
/**
* @brief 成否取得
*/
//--------------------------------------------------------------------------------
bool BossCheckNewArrivalDataThread::IsSuccess() const
{
  return m_isSuccess;
}

//--------------------------------------------------------------------------------
/**
* @brief リザルトコード取得
*/
//--------------------------------------------------------------------------------
nn::Result BossCheckNewArrivalDataThread::GetResult() const
{
  return m_result;
}

//--------------------------------------------------------------------------------
/**
* @brief ID数取得
*/
//--------------------------------------------------------------------------------
u32 BossCheckNewArrivalDataThread::GetIdNum()
{
  return m_serialIdList.GetSize();
}

//--------------------------------------------------------------------------------
/**
* @brief ID取得
*/
//--------------------------------------------------------------------------------
u32 BossCheckNewArrivalDataThread::GetNsDataId( u32 index )
{
  return m_serialIdList.GetNsDataId( index );
}



//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void BossCheckNewArrivalDataThread::Function( void )
{
  m_isSuccess = false;

  ////ダウンロードデータのシリアルID一覧を取得。(GetNsDataIdListの第一引数で取得データをフィルタリングできます。)
  //getNsDataIdListResult = nn::boss::GetNewDataNsDataIdList(nn::boss::DATA_TYPE_APPDATA|0x0000ffff, &serialIdList);//アプリケーション用追加データのみ取得したい場合。
  m_result = nn::boss::GetNewDataNsDataIdList( nn::boss::DATA_TYPE_ALL , &m_serialIdList );//全部タイプ指定
  if(m_result == nn::boss::ResultNsDataListUpdated())
  {
    GFL_PRINT("BossCheckNewArrivalDataThread::Main() : getNsDataIdListResult == nn::boss::ResultNsDataListUpdated()\n");
      //ResultNsDataListUpdated()が返された場合は、ID一覧取得中にBOSSストレージ内のNSデータに追加や削除があったケース。リストを初期化し、シリアルID一覧を初めから取り直してください。
    m_serialIdList.Initialize();
  }
  else if((m_result.IsFailure()) && (m_result != nn::boss::ResultNsDataListSizeShortage()))
  {
    GFL_PRINT("BossCheckNewArrivalDataThread::Main() : error [0x%x]\n",m_result.GetPrintableBits() );
      //ResultNsDataListUpdated()、ResultNsDataListSizeShortage()、ResultSuccess()以外のエラーは、予期せぬエラー。
      //ResultNsDataListSizeShortage()は「ID一覧取得に成功しているが、NsDataIdListにすべてのIDを格納できなかった(再実行が必要)」。ResultSuccess()は「ID一覧取得成功(全IDの取得完了)」を示す。

    m_pThread->Kill();
  }
  else if( m_result != nn::boss::ResultNsDataListSizeShortage() )
  {//取得完了

    GFL_PRINT("BossCheckNewArrivalDataThread::Function : result[0x%x][%d]\n",m_result.GetPrintableBits(),GetIdNum());

    if( GetIdNum() > 0 )
    {//１つ以上新着がある
      m_isSuccess = true;
    }
    m_pThread->Kill();
  }


  //ResultNsDataListSizeShortageの場合は、まだ全IDを取得できていないため、GetNsDataIdList()を再実行する。

  gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );
}


GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)


#endif

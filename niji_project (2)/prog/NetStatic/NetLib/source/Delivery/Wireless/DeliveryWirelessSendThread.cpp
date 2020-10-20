#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_DeliveryWirelessSendThread.cpp
 * @brief  ワイヤレス配信送信スレッド
 * @author Shin Kosaka
 * @date   2012.07.23
 */
//=============================================================================


#include "DeliveryWirelessSendThread.h"
#include "DeliveryWirelessSystem.h"
#include "DeliveryWirelessBeaconSplitter.h"

#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"
#include <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include <NetStatic/NetLib/include/NetDefine.h>
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <base/include/gfl2_Singleton.h>
 

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 * @param filter      フィルター用クラスへのポインタ
 * @param timeout     タイムアウト時間
 */
//--------------------------------------------------------------------------------
DeliveryWirelessSendThread::DeliveryWirelessSendThread( gfl2::heap::HeapBase* heap, u32 stack_size , DeliveryWirelessSystem* pSystem )
: m_pInitParam( NULL )
, m_pSystem( pSystem )
//m_TempBuff[nn::uds::NET_DESC_APPDATA_SIZE_MAX];  //一時的に読み込むバッファ
, m_pReceiveBuffer( NULL )
, m_aConnectEvent()
, m_index(0)
, m_time(0)
, m_pThread(NULL)
{
  memset( m_TempBuff , 0 , sizeof(m_TempBuff) );

  m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true, stack_size);
}
//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */

//--------------------------------------------------------------------------------
DeliveryWirelessSendThread::~DeliveryWirelessSendThread()
{
  FinalizeInternal();

  GFL_SAFE_DELETE( m_pThread );
}

//--------------------------------------------------------------------------------
/**
  * @brief 開始
  */
//--------------------------------------------------------------------------------
void DeliveryWirelessSendThread::Start()
{
  m_pThread->Start();
}


//--------------------------------------------------------------------------------
/**
* @brief 初期化処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
bool DeliveryWirelessSendThread::InitializeInternal( void )
{
  m_pInitParam = GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->GetInitParam();

  m_pReceiveBuffer = GflHeapAllocMemoryAlign4096( m_pInitParam->GetHeapBase() , UDSRECEIVE_BUFFER);

  // @attention スリープ禁止箇所（不思議なおくりものアプリ全体がスリープ禁止）
  //UDS初期化
  nn::Result result = nn::uds::Initialize( &m_aConnectEvent,
                              m_pReceiveBuffer ,
                              UDSRECEIVE_BUFFER );
  if( result.IsFailure() )
  {
    if( result == nn::uds::ResultAlreadyOccupiedWirelessDevice() )
    {
      // UDS は他の通信を行っている場合初期化に失敗します
    }
    else if( result == nn::uds::ResultWirelessOff() ){

      // 無線 OFF モードでした
    }
    else
    {
      //上記以外のエラー
    }

    GFL_PRINT("DeliveryWirelessRecvThread::Main : error nn::uds::Initialize faild\n");
    return false;
  }

  //ネットワークの構築
#if PM_DEBUG
  //kujira専用のASSERT ガイドライン2.5.1.検査
  GFL_ASSERT(NIJI_PRODUCT_CODE == m_pInitParam->GetProductID());  //@fix
#endif
  bit32 id = nn::uds::CreateLocalCommunicationId(m_pInitParam->GetProductID());
  result = nn::uds::CreateNetwork( BEACONTYPE_EACH,
                        m_pInitParam->GetLimitNum(), id,
                        PASSPHRASE, sizeof(PASSPHRASE) );
  if( result.IsFailure() )
  {//失敗
    return false;
  }

  return true;
}


//--------------------------------------------------------------------------------
/**
 * @brief 終了処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void DeliveryWirelessSendThread::FinalizeInternal( void )
{
  GFL_PRINT("DeliveryWirelessSendThread::Finalize\n");

  nn::uds::DestroyNetwork();
  nn::uds::Finalize();

  if( m_pReceiveBuffer )
  {
    GflHeapFreeMemory(m_pReceiveBuffer);
    m_pReceiveBuffer=NULL;
  }

  m_pInitParam = NULL;
  m_pSystem = NULL;
}

//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void DeliveryWirelessSendThread::Function( void )
{
  if(!m_pReceiveBuffer) {
    if( InitializeInternal() == false )
    {
      m_pThread->Kill();
      return;
    }
  }

  s64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  if( currentTime - m_time >= BEACON_ROTATE_MILLISECOND )
  {//ビーコンを切り替える
    DeliveryWirelessBeaconSplitter* pSplitter = m_pSystem->GetBeaconSplitter();
    if( pSplitter )
    {
      memset( m_TempBuff , 0 , sizeof(m_TempBuff) );
      size_t dataSize = pSplitter->GetSplitData( m_index , m_TempBuff );
      if( dataSize <= 0 || dataSize > sizeof(m_TempBuff) )
      {
        GFL_PRINT("DeliveryWirelessSendThread::Main: error invalid dataSize[%d]\n",dataSize);
        m_pThread->Kill();
        return;
      }

      u8 beaconID = NIJI_BEACON_TYPE_ID_DELIVERY;
      if( NetLib::NijiNetworkSystem::IsLiveRecSenderEnable() )
      {
        beaconID = NIJI_BEACON_TYPE_ID_LIVEREC;
      }

      m_pSystem->GetBeaconManager()->CreateBeaconData();//リセットしなければいけない
      m_pSystem->GetBeaconManager()->AddBeaconData( beaconID , BEACON_NO , m_TempBuff , sizeof(m_TempBuff) );
      m_pSystem->GetBeaconManager()->ReloadBeaconData();  //ビーコンをセットする関数

      m_time = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();//開始時刻

      m_index++;

      s32 splitNum = pSplitter->GetSplitNum();
      if( m_index >= splitNum )
      {//一巡した
        m_index = 0;
      }

      GFL_PRINT("DeliveryWirelessSendThread::Main : change beacon index[%d]\n" , m_index );
    }
    else
    {
      GFL_PRINT("DeliveryWirelessSendThread::Main : error pSplitter is NULL\n");
    }
  }

  gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );
}


GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)


#endif

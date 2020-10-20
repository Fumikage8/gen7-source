#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_DeliveryWirelessRecvThread.cpp
 * @brief  ワイヤレス配信受信スレッド
 * @author Shin Kosaka
 * @date   2012.7.23
 */
//=============================================================================


#include "DeliveryWirelessRecvThread.h"
#include "DeliveryWirelessSystem.h"

#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"
#include <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include <NetStatic/NetLib/include/NetDefine.h>
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


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
DeliveryWirelessRecvThread::DeliveryWirelessRecvThread( gfl2::heap::HeapBase* heap, u32 stack_size , DeliveryWirelessSystem* pSystem )
: m_pInitParam( NULL )
, m_pSystem( pSystem )
//m_TempBuff[nn::uds::NET_DESC_APPDATA_SIZE_MAX];  //一時的に読み込むバッファ
, m_pBeaconScanBuffer( NULL )
, m_pReceiveBuffer( NULL )
, m_aConnectEvent()
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
DeliveryWirelessRecvThread::~DeliveryWirelessRecvThread()
{
  nn::uds::Finalize();

  if( m_pReceiveBuffer )
  {
    GflHeapFreeMemory(m_pReceiveBuffer);
    m_pReceiveBuffer=NULL;
  }

  if( m_pBeaconScanBuffer )
  {
    GflHeapFreeMemory( m_pBeaconScanBuffer );
    m_pBeaconScanBuffer = NULL;
  }

  m_pInitParam = NULL;
  m_pSystem = NULL;

  GFL_SAFE_DELETE( m_pThread );
}

//--------------------------------------------------------------------------------
/**
 * @brief 開始
 */
//--------------------------------------------------------------------------------
void DeliveryWirelessRecvThread::Start()
{
  m_pThread->Start();
}


//--------------------------------------------------------------------------------
/**
 * @brief 初期化処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
bool DeliveryWirelessRecvThread::InitializeInternal( void )
{
  m_pInitParam = GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->GetInitParam();

  u8 beaconID = NIJI_BEACON_TYPE_ID_DELIVERY;
  if( NetLib::NijiNetworkSystem::IsLiveRecSenderEnable() )
  {
    beaconID = NIJI_BEACON_TYPE_ID_LIVEREC;
  }

  m_pInitParam->SetTypeDirect( beaconID );

  m_pBeaconScanBuffer = (u8*)GflHeapAllocMemoryLow( m_pInitParam->GetHeapBase() , BEACON_SCAN_BUFF );  //スキャン時に一度に探す数

  m_pReceiveBuffer = GflHeapAllocMemoryAlign4096( m_pInitParam->GetHeapBase() , UDSRECEIVE_BUFFER);

  {//UDS初期化

    // @attention スリープ禁止箇所（不思議なおくりものアプリ全体がスリープ禁止）
    nn::Result result;
    result = nn::uds::Initialize( &m_aConnectEvent,
                                m_pReceiveBuffer ,
                                UDSRECEIVE_BUFFER );
    if( result.IsFailure() )
    {//初期化しっぱい

      if( result == nn::uds::ResultAlreadyOccupiedWirelessDevice() )
      {
        // UDS は他の通信を行っている場合初期化に失敗します
        GFL_PRINT("DeliveryWirelessRecvThread::Main : result == nn::uds::ResultAlreadyOccupiedWirelessDevice()\n");
      }
      else if( result == nn::uds::ResultWirelessOff() ){

        // 無線 OFF モードでした
        //      GFL_PRINT("config is net-off mode\n");
        GFL_PRINT("DeliveryWirelessRecvThread::Main : result == nn::uds::ResultWirelessOff()\n");
      }
      else
      {
        //上記以外のエラー  初期化に失敗したのだから通信を終了しないといけない

      }

      GFL_PRINT("DeliveryWirelessRecvThread::Main : error nn::uds::Initialize faild\n");
      return false;
    }
  }

  return true;
}




//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理(この関数の中身はスレッドで実行される)
 */
//--------------------------------------------------------------------------------
void DeliveryWirelessRecvThread::Function( void )
{
  if(!m_pReceiveBuffer) {
    if( InitializeInternal() == false )
    {
      m_pThread->Kill();
      return;
    }
  }

  nn::Result result;
  gflnet2::InitParam* pInit = m_pInitParam;

  memset( m_pBeaconScanBuffer , 0 , BEACON_SCAN_BUFF );
  memset( m_TempBuff , 0 , sizeof(m_TempBuff) );

#if PM_DEBUG
  //kujira専用のASSERT ガイドライン2.5.1.検査
  GFL_ASSERT(NIJI_PRODUCT_CODE == pInit->GetProductID());  //@fix
#endif
  bit32 id = nn::uds::CreateLocalCommunicationId(pInit->GetProductID());
  u8* pBuffer = m_pBeaconScanBuffer;

  //SUBIDは全部取る！
  result =  nn::uds::Scan( pBuffer, BEACON_SCAN_BUFF , 0xff, id );

  if( !result.IsFailure() )
  {
    int ii;
    nn::uds::NetworkDescription         netDesc;
    nn::uds::ScanResultReader networkList(pBuffer);

    for(ii = 0 ; ii < networkList.GetCount() ; ii++)
    {
      nn::uds::NetworkDescriptionReader netDescReader = networkList.GetNextDescription();
      if (netDescReader.GetNetworkDescription(&netDesc).IsSuccess())
      {
        netDesc.GetApplicationData( m_TempBuff , sizeof(gflnet2::base::BeaconData));
        if( m_pSystem )
        {
          gflnet2::base::BeaconManager* pBeaconManager = m_pSystem->GetBeaconManager();
          if( pBeaconManager )
          {
            pBeaconManager->SetRecvBeaconData(netDesc.GetSubId(),
                                             m_TempBuff,sizeof(gflnet2::base::BeaconData),
                                             netDesc.GetNowEntry(),
                                             netDesc.CanConnectAsClient(),0);
          }
          else
          {
            GFL_PRINT("pBeaconManager is NULL\n");
          }
        }
      }
    }
  }

  gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_TIME );
}




GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)


#endif

#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gfl_NetWirelessCycleThread.cpp
 * @brief	 ワイヤレス通信 親子切り替えサイクルスレッド
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <nn.h>
#include <nn/Result.h>
#include <nn/hid.h>
#include <nn/uds.h>

#include "gflnet2_WirelessCycleThread.h"
#include <gflnet2/include/base/gflnet2_GlobalAccess.h>

#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------------------------

NetWirelessCycleThread::NetWirelessCycleThread( gflnet2::base::BeaconManager* pBeaconManager , u8 limitNum , u32 productID , gfl2::heap::HeapBase* pHeap )
     : gflnet2::base::NetThread( pHeap )
     , m_pBeaconManager( pBeaconManager )
     , m_limitNum( limitNum )
     , m_productID( productID )
     , m_pHeap( pHeap )
     , m_pUDSReceiveBuffer(NULL)
     , m_isFirstExec(true)
     , mpNetApScan(NULL)
     , mpAccessPointHistory(NULL)
{
  m_criticalSection.Initialize();
  mpTempBuff = (u8*)GflHeapAllocMemoryAlign(m_pHeap, nn::uds::NET_DESC_APPDATA_SIZE_MAX , 8 );  //一時的に読み込むバッファ
  //OHNO_PRINT("NetWirelessCycleThread::C\n");
  
  mpBeaconScanBuffer = (u8*)GflHeapAllocMemoryAlign(m_pHeap , BEACON_SCAN_BUFF , 4);  //スキャン時に一度に探す数

  mpSendBeaconData = (u8*)GflHeapAllocMemoryAlign(m_pHeap, m_pBeaconManager->GetSendBeaconDataSize() , 4 ); //送信ビーコンバッファ

  m_pUDSReceiveBuffer = GflHeapAllocMemoryAlign4096(m_pHeap , UDSRECEIVE_BUFFER);

  gfl2::std::MemFill(m_beaconData, 0x00, sizeof(m_beaconData) );
  m_beaconDataNum = 0;
  gfl2::std::MemFill( mNodeInfo , 0x00 , sizeof(mNodeInfo) );
}

//------------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//------------------------------------------------------------------------------------
NetWirelessCycleThread::~NetWirelessCycleThread()
{
  nn::uds::DestroyNetwork();

  m_criticalSection.Finalize();

  if(mpTempBuff){
    GflHeapFreeMemory( mpTempBuff );
    mpTempBuff = NULL;
  }

  if(mpBeaconScanBuffer){
    GflHeapFreeMemory( mpBeaconScanBuffer );
    mpBeaconScanBuffer = NULL;
  }

  if(mpSendBeaconData){
    GflHeapFreeMemory( mpSendBeaconData );
    mpSendBeaconData = NULL;
  }

  nn::uds::Finalize();

  if( m_pUDSReceiveBuffer )
  {
    GflHeapFreeMemory(m_pUDSReceiveBuffer);
    m_pUDSReceiveBuffer = NULL;
  }
}


//------------------------------------------------------------------
/**
 * @brief   前更新処理（この関数はメインスレッドから呼ばれる）
 */
//------------------------------------------------------------------
void NetWirelessCycleThread::UpdateBefore(void)
{
  MoveRecvBeaconDataInternal();
}

//------------------------------------------------------------------
/**
 * @brief   後更新処理（この関数はメインスレッドから呼ばれる）
 */
//------------------------------------------------------------------
void NetWirelessCycleThread::UpdateAfter(void)
{
  MoveSendBeaconDataInternal();
}

//------------------------------------------------------------------
/**
 * @brief   送信ビーコンをスレッドに移動（この関数はメインスレッドから呼ばれる）
 */
//------------------------------------------------------------------
void NetWirelessCycleThread::MoveSendBeaconDataInternal( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  u32 beaconSize = m_pBeaconManager->GetSendBeaconDataSize();

  gfl2::std::MemFill(mpSendBeaconData, 0x00, beaconSize );
  gfl2::std::MemCopy(m_pBeaconManager->GetSendBeaconData(), mpSendBeaconData, beaconSize );
}

//------------------------------------------------------------------
/**
 * @brief   受信ビーコンをビーコンマネージャに移動（この関数はメインスレッドから呼ばれる）
 */
//------------------------------------------------------------------
void NetWirelessCycleThread::MoveRecvBeaconDataInternal( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0 ; i < m_beaconDataNum ; ++i )
  {
   m_pBeaconManager->SetRecvBeaconData(m_beaconData[i].subId,
                                                     m_beaconData[i].pRecvBeacon,m_beaconData[i].size,
                                                     m_beaconData[i].nowEntry,
                                                     m_beaconData[i].ConnectAsClient,m_beaconData[i].lfc);
  }

  m_beaconDataNum = 0;
}


//----------------------------------------------------------------------------
/**
 * @brief 受信したビーコンをスレッド内のバッファに保存
 */
//----------------------------------------------------------------------------
void NetWirelessCycleThread::SetRecvBeaconDataInternal(u8 sudId,const void* pRecvBeacon, size_t size, u8 nowEntry, u8 ConnectAsClient, bit64 lfc)
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( m_beaconDataNum >= BEACON_DATA_NUM_MAX )
  {
    return;
  }

  m_beaconData[m_beaconDataNum].subId = sudId;
  m_beaconData[m_beaconDataNum].size = size;
  m_beaconData[m_beaconDataNum].nowEntry = nowEntry;
  m_beaconData[m_beaconDataNum].ConnectAsClient = ConnectAsClient;
  m_beaconData[m_beaconDataNum].lfc = lfc;
  gfl2::std::MemCopy(pRecvBeacon, m_beaconData[m_beaconDataNum].pRecvBeacon , sizeof(gflnet2::base::BeaconData) );

  m_beaconDataNum++;
}

//----------------------------------------------------------------------------
/**
 * @brief AC接続してアクセスポイントの検索を行う
 */
//----------------------------------------------------------------------------
nn::Result NetWirelessCycleThread::ScanAccessPointInternal( u8* pBuffer )
{
  nn::Result result = nn::ResultSuccess();

  if(mpNetApScan)
  {
    result = mpNetApScan->ScanAccessPoint(pBuffer, BEACON_SCAN_BUFF, mpAccessPointHistory);//mpAccessPointHistoryはメインスレッドからもアクセスされるため、内部で排他している
    //result = mpNetApScan->ScanAccessPointAC(pBuffer, BEACON_SCAN_BUFF, mpAccessPointHistory,mpNetBase);//mpAccessPointHistoryはメインスレッドからもアクセスされるため、内部で排他している
  }

  return result;
}

nn::Result NetWirelessCycleThread::CreateNetwork( int id )
{
  u32 beaconSize = m_pBeaconManager->GetSendBeaconDataSize();
  //ネットワークの構築
  gfl2::std::MemFill(mpBeaconScanBuffer, 0x00, BEACON_SCAN_BUFF );
  {
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御
    gfl2::std::MemCopy(mpSendBeaconData, mpBeaconScanBuffer, beaconSize );
  }

  return nn::uds::CreateNetwork(BEACONTYPE_EACH,
                                m_limitNum, id,
                                PASSPHRASE, sizeof(PASSPHRASE) ,
                                0,
                                mpBeaconScanBuffer ,
                                beaconSize //ここでビーコンを設定
                                );
}



//------------------------------------------------------------------
/**
 * @brief	  通信スレッド
 */
//------------------------------------------------------------------

void NetWirelessCycleThread::Function( void )
{
  if(GetKillNetThread()){
    Kill();
    return;
  }

  nn::Result result;

  if( m_isFirstExec )
  {
    m_isFirstExec = false;
    result = nn::uds::Initialize( &m_connectEvent,
                                  m_pUDSReceiveBuffer,
                                  UDSRECEIVE_BUFFER );
    if( result.IsFailure() )
    {
      Kill();
      return;
    }
  }

  if(GetKillNetThread()){
    Kill();
    return;
  }


  //u64 startTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  
  

  PauseCheckFunc();

  bit32 id = nn::uds::CreateLocalCommunicationId(m_productID);


  //送受信時間の決定
  srand( nn::os::Tick::GetSystemCurrent() );
  u32 transCount = BEACON_SEND_MIN + ( rand() % ( BEACON_SEND_MAX - BEACON_SEND_MIN + 1 ) );//受信回数
  u32 sendTime = ( BEACON_TRANS_BASE_TIME * transCount );//送信時間

  //u64 sendStartTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  
  //ネットワークの構築
  result = CreateNetwork(id); //ここでビーコンを設定
  if( result.IsFailure() )
  {
    //OHNO_PRINT("NetWirelessCycleThread::K1\n");
    Kill();
    return;
  }
  PauseCheckFunc();
  
  //u64 sleepStartTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();

  //ビーコン配信（親機）した状態で待機
  nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(sendTime) );
  
  //u64 sleepEndTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();

  PauseCheckFunc();
  //配信（親機）終了
  result = nn::uds::DestroyNetwork();
  if( result.IsFailure() )
  {
    Kill();
    return;
  }
  PauseCheckFunc();

  //u64 sendEndTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  
  ///------------------------ここからうけとり
  if(GetKillNetThread()){
    //OHNO_PRINT("NetWirelessCycleThread::K4\n");
    Kill();
    return;
  }

  u8* pBuffer = mpBeaconScanBuffer;
  ///---APスキャン
  result = ScanAccessPointInternal( pBuffer );
  ///---APスキャン

  if( result.IsFailure() )
  {
    //OHNO_PRINT("NetWirelessCycleThread::KM\n");
    Kill();
    return;
  }
  if(GetKillNetThread())
  {
    //OHNO_PRINT("NetWirelessCycleThread::K3\n");
    Kill();
    return;
  }

  //u64 recvStartTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  
  //ビーコン収集（子機）
  for( u32 count = 0 ; count < transCount ; ++count )
  {
    gfl2::std::MemFill(pBuffer, 0x00, BEACON_SCAN_BUFF );
    result =  nn::uds::Scan(pBuffer, BEACON_SCAN_BUFF, 0xFF, id,0);

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
          result = netDescReader.GetNodeInformationList(mNodeInfo);
          if( result.IsSuccess() )
          {
            nn::friends::LocalFriendCode lfc = 0;
            nn::Result res = nn::friends::CTR::UnscrambleLocalFriendCode( &lfc,&mNodeInfo[0].scrambledLocalFriendCode,1  );
            if(res.IsSuccess())
            {
              netDesc.GetApplicationData( mpTempBuff , sizeof(gflnet2::base::BeaconData));
              SetRecvBeaconDataInternal( netDesc.GetSubId(),
                                  mpTempBuff,sizeof(gflnet2::base::BeaconData),
                                  netDesc.GetNowEntry(),
                                  netDesc.CanConnectAsClient(),lfc );
            }
            else
            {
              GFL_ASSERT(0);  //テスト確認の為 後で外すかも
            }
          }
          else
          {
            GFL_PRINT( "NetWirelessCycleThread::Main : GetNodeInformationList failed[0x%x]\n" , result.GetPrintableBits());
          }
        }
      }
    }
    else
    {
      GFL_PRINT( "NetWirelessCycleThread::Main : scan failed[0x%x]\n" , result.GetPrintableBits());
      Kill();
      return;
    }

    PauseCheckFunc();
  }
  PauseCheckFunc();

  //u64 recvEndTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();

  if(GetKillNetThread()){
    //OHNO_PRINT("NetWirelessCycleThread::K2\n");
    Kill();
  }
  
  //u64 endTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  
  //GFL_PRINT("transCount[%u]\n",transCount);
  //GFL_PRINT("start[%llu]\nsendstart[%llu]\nsendend[%llu]\nrecvstart[%llu]\nrecvend[%llu]\nend[%llu]\n",startTime,sendStartTime,sendEndTime,recvStartTime,recvEndTime,endTime);
}

//------------------------------------------------------------------
/**
 * @brief	  ポーズ処理
 */
//------------------------------------------------------------------
void NetWirelessCycleThread::PauseCheckFunc( void )
{
  while(1){
    if(GFL_SINGLETON_INSTANCE(gflnet2::GlobalAccess) == NULL )
      break;

    if(!GFL_SINGLETON_INSTANCE(gflnet2::GlobalAccess)->IsPause())
      break;

    if(GetKillNetThread()){
      break;
    }
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}


GFL_NAMESPACE_END(gflnet2)

#endif

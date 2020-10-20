#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_UdsAdapter.cpp
 * @brief	 NEXUDSのアダプター
 * @author k.ohno
 * @date	 2011.5.18
 */
//=============================================================================


#include "gflnet2_PiaAdapter.h"

//#include <Utilities/ConsoleIO/UtilTrace.h>
//#include <Utilities/ConsoleIO/ConsoleIO.h>

#include <nex_P2p.h>
#include <NetZ/src/Core/DOCore/StationManager.h>

#include <nn/uds.h>
#include <nn/friends.h>
#include <pia/util.h>

#include <base/include/gfl2_Singleton.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>




GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(dev)



const int UDS_RECEIVE_BUFFER_SIZE = 4096 * 20;

const int UDS_SCAN_BUFFER_SIZE = 32 * 1024;

// 最初は余裕を持ったメモリサイズを指定。
const u32 MEMSIZE = 1024 * 128;


PiaAdapter* PiaAdapter::s_pInstance = NULL;



// インスタンス作成。
void PiaAdapter::CreateInstance(nn::nex::qByte sendOption,gfl2::heap::HeapBase* pHeapBase )
{
  if(s_pInstance==NULL) {
    //OHNO_PRINT("PIAインスタンス作成\n");
    s_pInstance = GFL_NEW(pHeapBase) PiaAdapter();//メモリ消費大
    s_pInstance->Initialize(sendOption,pHeapBase);
    //nn::Result re = s_pInstance->Initialize(sendOption,pHeapBase);
    //GFL_ASSERT( re.IsSuccess() );//無線LANOFFで発生する
  }
}


// インスタンス破棄。
void PiaAdapter::DestroyInstance(void)
{
  if(s_pInstance!=NULL) {
    GFL_DELETE( s_pInstance );
    s_pInstance = NULL;
  }
}


///< コンストラクタ
PiaAdapter::PiaAdapter(void)
{
  std::memset( mUdsGatheringHolderArray , 0 , sizeof(mUdsGatheringHolderArray) );
  mpUdsReceiveBuffer = NULL;
  mpUdsScanBuffer = NULL;
  mpPiaMem = NULL;
  mLocalCommunicationID = 0;
  s_pUdsNetwork = NULL;
}


///< 初期化
nn::Result PiaAdapter::Initialize(nn::nex::qByte sendOption, gfl2::heap::HeapBase* pHeapBase )
{
  nn::Result result;

  mpPiaMem = GflHeapAllocMemoryAlign(pHeapBase , MEMSIZE, 4);
  // Piaの土台となるcommonモジュールの初期化。ここでPiaライブラリにメモリを供給します。
  result = nn::pia::common::Initialize(  mpPiaMem, MEMSIZE);
  if(result.IsFailure()){
    GFL_PRINT("PiaAdapter::Initialize : error[0x%x]\n",result.GetPrintableBits());
    return result;
  }

  // commonモジュールのシングルトンなどの作成を開始する前に呼びます。
  result = nn::pia::common::BeginSetup();
  if(result.IsFailure()){
    //OHNO_PRINT("err%d\n",__LINE__);
    nn::pia::common::EndSetup();
    return result;
  }

    nn::pia::common::Trace::CreateInstance();
    /*//ログが出すぎてデバッガーが固まるのでOFF
    nn::pia::common::Trace::GetInstance()->SetFlag(
        nn::pia::common::TRACE_FLAG_ALWAYS
        | nn::pia::common::TRACE_FLAG_COMMON
        | nn::pia::common::TRACE_FLAG_LOCAL
    );
    */

  // commonモジュールのシングルトンなどの作成を終えてから呼びだします。
  result = nn::pia::common::EndSetup();
  if(result.IsFailure()){
    //OHNO_PRINT("err%d\n",__LINE__);
    return result;
  }

  // localモジュールの初期化
  result = nn::pia::local::Initialize();
  if(result.IsFailure()){
    //OHNO_PRINT("err%d\n",__LINE__);
    return result;
  }
  // localモジュール以下のセットアップ開始
  result = nn::pia::local::BeginSetup();
  if(result.IsFailure()){
    nn::pia::local::EndSetup();
    //OHNO_PRINT("err%d\n",__LINE__);
    return result;
  }

  //gfl2::heap::Manager::Dump();

  mpUdsReceiveBuffer = GflHeapAllocMemoryAlign4096(pHeapBase , UDS_RECEIVE_BUFFER_SIZE);
  GFL_ASSERT_STOP(mpUdsReceiveBuffer); //@fix

  mpUdsScanBuffer = GflHeapAllocMemoryAlign4096(pHeapBase , UDS_SCAN_BUFFER_SIZE);//メモリ消費大
  GFL_ASSERT_STOP(mpUdsScanBuffer); //@fix

  //n1589:NEXのUDS受信オプションは常にNO_WAITをONにする必要があります、OFFの場合はアサート停止します
  const nn::nex::qByte receiveOption = nn::uds::NO_WAIT;

  nn::pia::local::UdsNodeSetting udsNodeSetting = {
    mpUdsReceiveBuffer,
    UDS_RECEIVE_BUFFER_SIZE,
    mpUdsScanBuffer,
    UDS_SCAN_BUFFER_SIZE,
    sendOption,
    receiveOption,
    false   //ホストマイグレーション
    };


  result = nn::pia::local::UdsNode::CreateInstance(udsNodeSetting);
  if(result.IsFailure()){
    nn::pia::local::EndSetup();
    GFL_PRINT("CreateInstance:error[0x%x]\n",result.GetPrintableBits());
    return result;
  }
  //nn::nex::GlobalVariables::AcquireInstance();

  // UdsNetworkのインスタンスが生成される前にUdsNodeインスタンスが生成済みである必要がある
  s_pUdsNetwork = qNew nn::pia::local::UdsNetwork();
  // UdsNetworkインスタンスをVirtualSocketDriverに設定する
  nn::nex::VirtualSocketDriver::RegisterVirtualNetwork(s_pUdsNetwork);

  nn::pia::local::UdsNode::GetInstance()->EnableCrc(false);

  result = nn::pia::local::UdsNode::GetInstance()->Initialize(UDS_NODE_EVENTTHREAD_PRIORITY);
  if(result.IsFailure()){
    nn::pia::local::EndSetup();
    GFL_PRINT("Initialize:error[0x%x]\n",result.GetPrintableBits());
    return result;
  }
  result = nn::pia::local::EndSetup();

  return result;
}

///< デストラクタ
PiaAdapter::~PiaAdapter(void)
{

  if(nn::pia::local::UdsNode::GetInstance() == NULL){
    return;
  }


  if (s_pUdsNetwork) {
    nn::nex::VirtualSocketDriver::UnregisterVirtualNetwork();
    // UdsNetworkインスタンスの破壊前にVirtualSocketDriverの設定を解除する
    qDelete s_pUdsNetwork;
    s_pUdsNetwork = NULL;
  }

  // UdsNetworkインスタンスの破壊後にUdsNodeインスタンスを破壊する
  nn::pia::local::UdsNode::GetInstance()->Finalize();
  nn::pia::local::UdsNode::DestroyInstance();

  // localモジュール以下の片づけを終えてからlocalモジュールの終了処理
  nn::pia::local::Finalize();

  //nn::nex::GlobalVariables::ReleaseInstance();


  //  // AppTraceシングルトンの破棄。
  nn::pia::common::Trace::DestroyInstance();

  // Piaのcommonモジュール終了処理。
  nn::pia::common::Finalize();
  GflHeapFreeMemory(mpPiaMem);

  GflHeapFreeMemory(mpUdsReceiveBuffer);
  GflHeapFreeMemory(mpUdsScanBuffer);
}


// ローカル通信IDの生成
u32 PiaAdapter::CreateLocalCommunicationId(u32 uniqueId, bool isTrial)
{
  //mProductID = uniqueId;
  mLocalCommunicationID = uniqueId;

  //OHNO_PRINT("PiaAdapter::CreateLocalCommunicationId %d\n",uniqueId);

  return uniqueId;
}



// UDSネットワークの構築
bool PiaAdapter::RegisterGathering(nn::nex::qByte maxEntry, 
                                   const char passphrase[],
                                   nn::nex::qByte passphraseLendth,
                                   nn::nex::qByte channel,
                                   u8 gatheringID ,
                                   char* pData,
                                   u32 dataSize
                                   )
{


  //n2788:接続可能な状態以外ではRegisterGatheringをしない。
  if(nn::pia::local::UdsNode::GetInstance()->GetState() != nn::uds::CTR::STATE_DISCONNECTED)
  {
    return false;
  }


  nn::Result re = nn::pia::local::UdsNode::GetInstance()
    ->RegisterGathering(gatheringID,
                        maxEntry,
                        GetLocalCommunicationIdPIA(),
                        passphrase, passphraseLendth, channel,pData,dataSize);
  //OHNO_PRINT("Pia RegisterGathering maxEntry %d\n",maxEntry);
  GFL_PRINT("PiaAdapter::RegisterGathering %d %d \n",GetLocalCommunicationIdPIA(), channel);
  GFL_PRINT("PiaAdapter::RegisterGathering %s\n",passphrase);
  GFL_PRINT("PiaAdapter::RegisterGathering %d\n",passphraseLendth);

  GFL_PRINT("PiaAdapter::RegisterGathering beacon size[%d]\n",dataSize);

  if(re.IsFailure())
  {
    return false;
  }
  return true;
}


// 周辺をFindById()で探索し、ギャザリングリストを更新。関数の実行に失敗した場合はfalse。
bool PiaAdapter::UpdateGatheringList( u8 gatheringID )
{
  GFL_PRINT("PiaAdapter::UpdateGatheringList %d\n",GetLocalCommunicationIdPIA());

  nn::Result re = nn::pia::local::UdsNode::GetInstance()->FindById(gatheringID, GetLocalCommunicationIdPIA());
  if(!re.IsFailure()){
    return true;
  }
  //※この関数はネットワークが発見できなかったことを示すものしか返さないのでキャッチしない
  return false;
}


// ギャザリングリスト取得。取得のみ。リストの更新はしていない。
size_t PiaAdapter::GetGatheringList(void) const
{
  size_t getNum;

  nn::Result re = nn::pia::local::UdsNode::GetInstance()->GetGatheringList((nn::pia::local::UdsGatheringHolder*)mUdsGatheringHolderArray, &getNum, GATHERING_HOLDER_ARRAY_SIZE);
  if(!re.IsFailure())
  {
    return getNum;
  }
  
  GFL_ASSERT( 0 );

  return 0;
}



// UDSネットワークに参加
bool PiaAdapter::Participate(nn::pia::local::UdsGatheringHolder *pGathering, const char passphrase[], NAMESPACE_NEX qByte passphraseLendth)
{
  nn::Result re = nn::pia::local::UdsNode::GetInstance()
    ->Participate(pGathering, passphrase, passphraseLendth);

  GFL_PRINT("Participate %d %d %d\n",pGathering->GetGameId(),pGathering->GetSubId(), pGathering->GetTemporaryId());
  GFL_PRINT("%s\n",passphrase);
  GFL_PRINT("%d\n",passphraseLendth);


  if(!re.IsFailure())
  {//成功
    GFL_PRINT("Participate success\n");

    return true;
  }
  else
  {//失敗
    GFL_PRINT("Participate failed[0x%x]\n",re.GetPrintableBits());  
    return false;
  }
}


void PiaAdapter::Terminate(void)
{
  if(nn::pia::local::UdsNode::GetInstance()->IsMaster()) 
  {
    nn::Result re = nn::pia::local::UdsNode::GetInstance()->UnregisterGathering();
    if(re.IsFailure())  
    {
      GFL_PRINT("PiaAdapter::Terminate UnregisterGathering failed\n");
      //GFL_ASSERT(0);//失敗する場合もあるが正常に進行できる
    }
  }
  else
  {  //子機
    nn::Result re = nn::pia::local::UdsNode::GetInstance()->EndParticipation();
    if(re.IsFailure()) 
    {
      GFL_PRINT("PiaAdapter::Terminate EndParticipation failed\n");
      //GFL_ASSERT(0);//失敗する場合もあるが正常に進行できる
    }
  }
}


bool PiaAdapter::SetDataToBeacon(const char *pData, int size)
{
  if(pData==NULL){
    return false;
  }

  if(size > sizeof(gflnet2::base::BeaconData)){
    return false;
  }
  if(!nn::pia::local::UdsNode::GetInstance()){
    return false;
  }
  if(!nn::pia::local::UdsNode::GetInstance()->IsMaster()){
    return false;
  }

  nn::Result re = nn::pia::local::UdsNode::GetInstance()->SetApplicationDataToBeacon(pData, GFL_BEACON_HARD_SIZEMAX);
  if(re.IsFailure())
  {
    return false;
  }
  return true;
}


// 自分はUDSマスターか？
bool PiaAdapter::IsMaster(void) const
{
  return nn::pia::local::UdsNode::GetInstance()->IsMaster();
}


// 自分はUDSクライアントか？
bool PiaAdapter::IsClient(void) const
{
  return nn::pia::local::UdsNode::GetInstance()->IsClient();
}

//ProcessUds
nn::nex::StationURL* PiaAdapter::GetMasterUrl(void) const
{
  return s_pUdsNetwork->GetMasterUrl();
}


// UDSの接続状態を得ます。
bool PiaAdapter::GetConnectionStatus(nn::pia::local::UdsStatus *pStat) const
{
  if(pStat==NULL) {
    return false;
  }
  nn::Result re = nn::pia::local::UdsNode::GetInstance()->GetConnectionStatus(pStat);
  return re.IsSuccess();
}


void PiaAdapter::RegisterStationEventCallback(nn::nex::StationEventCallback callback)
{
  if(s_pInstance == NULL){
    return;
  }
  nn::nex::StationManager::GetInstance()->RegisterStationEventCallback(callback);
}


nn::pia::local::UdsGatheringHolder* PiaAdapter::GetGatheringClass(int index)
{
  return &mUdsGatheringHolderArray[index];
}

nn::pia::local::UdsLinkLevel PiaAdapter::GetRadioStrength( int index )
{
  nn::pia::local::UdsLinkLevel level = nn::uds::CTR::LINK_LEVEL_0;
  nn::Result ret = nn::pia::local::UdsNode::GetInstance()->GetRadioStrength( &level , index );
  if( ret.IsSuccess() )
  {
    return level;
  }
  
  return nn::uds::CTR::LINK_LEVEL_0;
}


ERROR_CHECK_RESULT PiaAdapter::ErrorCheck(void)
{
  nn::pia::local::UdsConnectionStatus s;
  nn::Result result = nn::pia::local::UdsNode::GetInstance()->GetConnectionStatus(&s);

  if( result.IsSuccess() ) {
    switch(s.nowState){
    case nn::uds::STATE_DISCONNECTED:
      GFL_PRINT("nn::uds::STATE_DISCONNECTED[%d]\n",s.disconnectReason);
      switch(s.disconnectReason){
      case nn::uds::REQUEST_FROM_SYSTEM:
      case nn::uds::DISCARDED_FROM_NETWORK:
      case nn::uds::CONNECTION_LOST:

        return RESULT_SESSION_LEFT;
      }
      break;
    case nn::uds::STATE_ERROR:
      //OHNO_PRINT("ErrorCheck %d %d\n", s.nowState, s.disconnectReason);
      
      return RESULT_ERROR;
    }
  }

  return RESULT_NONE;
}


bool PiaAdapter::DisconnectCheck(void)
{
  nn::pia::local::UdsConnectionStatus s;
  nn::Result result = nn::pia::local::UdsNode::GetInstance()->GetConnectionStatus(&s);
  if( result.IsSuccess() ) {
    switch(s.nowState){
    case nn::uds::STATE_DISCONNECTED:
      return true;
    }
  }
  return false;
}



/*
・nn::uds::Scan() で取得したバッファをScanResultReaderに格納。
・ScanResultReaderからGetNextDescription()などを使用して、
　NetworkDescriptionReaderを取得。
・nn::uds::CTR::NetworkDescriptionReader::GetNodeInformationList()
　によって NodeInformation を取得。
・NodeInformationにscrambledLocalFriendCodeが含まれているため
　nn::friends::CTR::UnscrambleLocalFriendCode()を使用して復号。
 */


GFL_NAMESPACE_END(dev)
GFL_NAMESPACE_END(gflnet2)
#endif

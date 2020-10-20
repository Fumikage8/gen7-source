#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	  gflnet_IrAdapter.cpp
 * @brief	  ir通信 ラッパークラス
 * @author  k.ohno
 * @date	  2012.3.10
 */
//=============================================================================

#include <nn.h>
#include <nn/Result.h>
#include <nn/ir.h>


#include "gflnet2_IrAdapter.h"

#include <base/include/gfl2_Singleton.h>

#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( dev )


IrAdapter* IrAdapter::mpInstance = NULL;
IrAdapterErrorListener* IrAdapter::m_errorListener = NULL;

// インスタンス作成。
void IrAdapter::CreateInstance(gfl2::heap::HeapBase* pHeapBase,const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix)
{
  if(mpInstance == NULL) {
    mpInstance = GFL_NEW(pHeapBase) IrAdapter(pIParam, pIParamFix);
    mpInstance->mpInitParam = &mpInstance->mInitParam;
  }
}


// インスタンス破棄。
void IrAdapter::DestroyInstance(void)
{
  if(mpInstance!=NULL)
    {
      GFL_DELETE mpInstance;
      mpInstance = NULL;
    }
}



IrAdapter::IrAdapter(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix)
: NetAdapter(pIParam, pIParamFix)
, mReceiveSize(0)
, mAddRecvSize(0)
, mpSendBuffer(NULL)
, mpRecvBuffer(NULL)
, mpInitBuffer(NULL)
, mpInitParam(NULL)  //初期化クラス
, mCountFailed(0)
, mRecvFailedCount(0)
, bConnectStatus(false)
, bInit(false)
{
}


bool IrAdapter::Initialize(gfl2::heap::HeapBase* pHeapSys)
{
  nn::Result result = nn::ResultSuccess();

  GFL_ASSERT_STOP(pHeapSys);  //@fix
  mpSendBuffer = GflHeapAllocMemoryAlign(pHeapSys , IRSEND_BUFFER,4);
  mpRecvBuffer = GflHeapAllocMemoryAlign(pHeapSys , IRSEND_BUFFER,4);
  mReceiveSize = 0;
  mCountFailed=COMFIRM_FAILED_MAX;

///-------------------------------

  u32 size = 4096*2;

  mpInitBuffer = GflHeapAllocMemoryAlign4096(pHeapSys , size);

  //---------------------------
  //送受信用のバッファサイズ設定
  //---------------------------
  //受信バッファには最大InputData1つを保存するが、ノイズなどを考慮して少し余裕をもたせる
  //ConfirmId のために最低限必要な大きさを指定する必要がある
  size_t receiveBufferSize     = nn::ir::Communicator::GetPacketSize(GFLNET_IR_MAXPACKET)*2;
  size_t receiveManagementSize = nn::ir::Communicator::GetManagementSize(1)*2;
  //こちらも余裕をもたせる
  //同様に、ConfirmId のために最低限必要な大きさを指定する必要がある
  size_t sendBufferSize        = nn::ir::Communicator::GetPacketSize(GFLNET_IR_MAXPACKET)*2;
  size_t sendManagementSize    = nn::ir::Communicator::GetManagementSize(1)*2;

  //管理用領域と送受信バッファ領域と予約領域の合計が与えるバッファサイズより小さくなければいけない
  NN_ASSERT(nn::ir::Communicator::GetReservedSize() + sendBufferSize + sendManagementSize + receiveBufferSize + receiveManagementSize <= size);

  result = nn::ir::Communicator::Initialize(
    mpInitBuffer, size,
    receiveBufferSize,
    receiveManagementSize,
    sendBufferSize,
    sendManagementSize);

  if( result.IsFailure() ){
    if( m_errorListener )
    {
      m_errorListener->OnIrAdapterErrorIrBroken();
    }
    return false;
  }

  mRecvFailedCount=0;


  // 各種イベントの取得
  // 送信完了イベント
  nn::ir::Communicator::GetSendEvent(&maSendEvent);
  // 受信完了イベント
  nn::ir::Communicator::GetReceiveEvent(&maReceiveEvent);
  // 接続状態イベント
  nn::ir::Communicator::GetConnectionStatusEvent(&maConnectEvent);

  bInit = true;
//--------------------
  //OHNO_PRINT("IrAdapter::Initialize %d %d %d %d\n",receiveBufferSize,receiveManagementSize,sendBufferSize,sendManagementSize);
   
  return true;
}



int IrAdapter::ConnectBase(u8 GameID)
{

  nn::Result result;

  nn::ir::ConnectionStatus status = nn::ir::Communicator::GetConnectionStatus();
  if(nn::ir::CONNECTION_STATUS_CONNECTED == status){
    
    if(!nn::ir::Communicator::IsConfirmedId()) {
      result = ConfirmID(GameID);
      if(result.IsSuccess()) {
        //  //OHNO_PRINT("Comfirm Success.\n");
        ICHI_PRINT("Comfirm Success.\n");
        if(IsMaster()){
          //OHNO_PRINT("Comfirm Success. Parent\n");
          ICHI_PRINT("Comfirm Success. Parent\n");
        }
        else{
          //OHNO_PRINT("Comfirm Success. Child\n");
          ICHI_PRINT("Comfirm Success. Child\n");
        }
        return 1;
      }
      else{
        ICHI_PRINT("Comfirm Failed.\n");
        mCountFailed--;
        nn::ir::Communicator::Disconnect();
        if(mCountFailed==0){
          if(result == nn::ir::ResultCannotConfirmId()){
            if( m_errorListener )
            {
              m_errorListener->OnIrAdapterErrorMissmatch();
            }
          }
          else{
            if( m_errorListener )
            {
              m_errorListener->OnIrAdapterErrorHandling();
            }
          }
          return -1;
        }
      }
    }
  }
  return 0;
}

int IrAdapter::ConnectNonBlock(u8 GameID)
{
  nn::ir::Communicator::AutoConnection();

  return ConnectBase(GameID);
}

bool IrAdapter::Connect(void)
{
  GFL_ASSERT(0);  //@fix
  //使用禁止
  return false;
}





bool IrAdapter::Scan(void)
{
  return true;
}

bool IrAdapter::SetBeacon(const u8* pData, const u32 size)
{
  return localSendData(pData, size, mpSendBuffer, IRSEND_BUFFER, &maSendEvent);
}

void* IrAdapter::GetBeacon(const int index, const u32  size)
{
#if 0
  if( localRecvDataNoLoop(mpRecvBuffer, size,  IRSEND_BUFFER, &maReceiveEvent)){
//  if( localRecvData(mpRecvBuffer, size,  IRSEND_BUFFER, &maReceiveEvent)){
//    RecvEnd(); //データを完全に受け取ったらこのフラグセット
    mAddRecvSize = 0;
    return mpRecvBuffer;
  }
#endif
  GFL_ASSERT(0);
  return NULL;
}

void* IrAdapter::GetBeaconTest(const int index, const u32  size,nn::fnd::TimeSpan waitnum)
{
  nn::Result result = nn::ResultSuccess();
  s32 usedSize = -1;
  size_t receiveSize = 0;
  size_t bufferSize = nn::ir::Communicator::CalculateBufferSizeToCommunicate(size);

  result = nn::ir::Communicator::GetLatestReceiveErrorResult(true);

#if GFL_DEBUG
  if(result == nn::ResultSuccess()){
//    //OHNO_PRINT("cResultSuccess\n");
  }
  else if(result == nn::ir::ResultSignatureNotFound()){
    NN_LOG("cResultSignatureNotFound\n");
  }
  else if(result == nn::ir::ResultInvalidCrc()){
    NN_LOG("cResultInvalidCrc\n");
  }
  else if(result == nn::ir::ResultFollowingDataNotExist()){
    NN_LOG("cResultFollowingDataNotExist\n");
  }
  else if(result == nn::ir::ResultFatalError()){
    NN_LOG("cResultFatalError\n");
  }
  else{
    NN_LOG("cOtherError %x\n",result.GetPrintableBits());
  }
#endif
  
  result = nn::ResultSuccess();

  
  bool bIsRecvFaild = false;

  // まずデータを受信しているか調べます
  if(!maReceiveEvent.Wait(waitnum)){
    //OHNO_PRINT("受信しなかった\n");
    mRecvFailedCount++;
    if(mRecvFailedCount > RECV_FAILED_MAX){
      mRecvFailedCount = 0;//受信があまりに無い場合Receiveを呼ぶ
      bIsRecvFaild = true;
    }
    else{
      maReceiveEvent.ClearSignal();
      nn::ir::Communicator::ClearReceiveBuffer();
      return NULL;
    }
  }

  
  
  // 受信されたデータを処理します
  do {
    maReceiveEvent.ClearSignal();
    result = nn::ir::Communicator::Receive(mpRecvBuffer,bufferSize ,&receiveSize, &usedSize);

    if(result.IsFailure()) {
      receiveSize = 0; 
        
      if(result == nn::ir::ResultInvalidData())  { 
        // データが受信されましたが、検証の結果、不正なデータでした。
        // このとき、受信バッファからデータは取り除かれているため、
        // (存在するならば) 続けて後続のパケットを処理してください。
        NN_LOG("不正データ\n");
      }
      else if(result == nn::ir::ResultBufferInsufficient())    { 
        // バッファサイズが不足しています。
        // このとき、データの検証処理を実行できないため、
        // データが不正かどうか判断できません。
        // このデータを受信処理するためには、GetNextReceiveDataSize() 関数で
        // 受信データのサイズを確認し、必要なサイズのバッファを用意してください。
        // このデモでは、想定するデータを受信するために必要なバッファは用意しているため、
        // 受信されたデータは不正なデータと思われます。
        
        // 受信バッファにはデータが残っています。
        // 不要なので、このデータは破棄して、続けて後続のパケットを処理します。
        nn::ir::Communicator::DropNextReceiveData(&usedSize);
        NN_LOG("DropNextReceiveData\n");
      }
      else if(result == nn::ir::ResultNotConnected()){ 
        NN_LOG("接続状態ではありません");
      }
      else if(result == nn::ir::ResultCannotConfirmId()){ 
         NN_LOG("ID 確認処理が行われていません");
      }
      else if(result == nn::ir::ResultNoData())    { 
        NN_LOG("データはない");
      }
      else if(result == nn::ir::ResultMachineSleep())    { 
        NN_LOG("本体スリープによるスリープ中");
      }
      else if(result == nn::ir::ResultFatalError())    { 
        NN_LOG("IR モジュールが故障");
      }
      else{
        // その他
        NN_LOG("Receiveその他のエラー %x\n",result.GetPrintableBits());
        break;
      }
    }
  }while(usedSize > 0);

  // 受信失敗だった場合
  if(bIsRecvFaild)
  {
    maReceiveEvent.ClearSignal();
    nn::ir::Communicator::ClearReceiveBuffer();
  }

  return mpRecvBuffer;
}


bool IrAdapter::CreateMaster(void)
{
  //OHNO_PRINT("スタートCreateMaster\n");
  bConnectStatus=true;
  return true;
}

bool IrAdapter::Matching(int TransferedID)
{
  //OHNO_PRINT("スタートMatching\n");
  bConnectStatus=true;
  return true;
}

bool IrAdapter::Update(void)
{
  if(!bInit){  //イニシャライズしている
    return false;
  }

#if GFL_DEBUG
  mDebugStatus = nn::ir::Communicator::GetConnectionStatus();
#endif

  // ステートの変化を監視します。
  // イベントは、以下の条件でシグナルされます。
  //
  // 1. ***Connection による接続の確立
  // 2. Disconnect 処理の完了
  // 3. 相手からの Disconnect による接続の切断
  // 4. IR の故障(nn::ir::ResultFatalError)の発生
  if(maConnectEvent.Wait(0)){
    switch(nn::ir::Communicator::GetConnectionStatus()) {
    case nn::ir::CONNECTION_STATUS_FATAL_ERROR:
      {
        // IR が故障した可能性があります。本体電源を切るまで、この状態は継続します。
        // IR ライブラリを終了し、ユーザに故障の可能性を通知することを推奨します
        nn::ir::Communicator::Finalize();
        NN_PANIC("IR Fatal Error occurred.\n");
      }
      break;
    }
  }
  return true;
}

bool IrAdapter::SendData(void* pData, const u32 size)
{
  return localSendData(pData, size, mpSendBuffer, IRSEND_BUFFER, &maSendEvent);
}

/*
void* IrAdapter::RecvData(int id)
{
  if( localRecvData(mpRecvBuffer, IRSEND_BUFFER,  IRSEND_BUFFER, &maReceiveEvent)){
    return mpRecvBuffer;
  }
  return NULL;
}
*/

void* IrAdapter::RecvDataFunc(int id, size_t* pSize)
{
#if 0
  if( localRecvDataNoLoop(mpRecvBuffer, GFLNET_IR_MAXPACKET,  IRSEND_BUFFER, &maReceiveEvent)){

//    RecvEnd(); //データを完全に受け取ったらこのフラグセット
    *pSize = mAddRecvSize;
    mAddRecvSize = 0;
    return mpRecvBuffer;
  }
  return NULL;
#endif
  GFL_ASSERT(0);
  return NULL;
}

bool IrAdapter::Disconnect(void)
{
  if( bInit == true )
  {
    nn::ir::Communicator::Disconnect();
  }
  return true;
}

bool IrAdapter::Finalize(void)
{
  //OHNO_PRINT("IrAdapter::Finalize\n");

  if( bInit == true )
  {
    nn::Result result = nn::ir::Communicator::Finalize();
    if(result.IsFailure()){
      GFL_ASSERT(0);  //@fix 確認の為
    }
  }
  bInit=false;

  
  GflHeapFreeMemory(mpSendBuffer);
  GflHeapFreeMemory(mpRecvBuffer);
  GflHeapFreeMemory(mpInitBuffer);
  mpInitBuffer=NULL;
  mpRecvBuffer=NULL;
  mpSendBuffer=NULL;
  return true;
}


bool IrAdapter::IsMaster(void)
{
  nn::ir::ConnectionRole role = nn::ir::Communicator::GetConnectionRole();
  if(role==nn::ir::CONNECTION_ROLE_TO_WAIT){
    return true;  //待った方が親
  }
  return false;
}



bool IrAdapter::localSendData(const void* pData,int sendsize, void* tempBuff, int tempsize,
                    nn::os::Event* pSendEvent)
{

  nn::Result result = nn::ResultSuccess();

  s32 usedSize = -1;
  size_t receiveSize = 0;

//  if(!SendMode()){
//    //OHNO_PRINT("送信モードでない\n");
   // return false;
//  }
  
  //送信データ残量を確認し、何も残っていなければ送信する
  nn::ir::Communicator::GetSendSizeFreeAndUsed(NULL,NULL,NULL,&usedSize);
  if(usedSize != 0){
    return false;
  }

  //送信/受信用バッファを作成する
  size_t bufferSize = nn::ir::Communicator::CalculateBufferSizeToCommunicate(sendsize);

  if(bufferSize > tempsize){
    GFL_ASSERT(0); //ここにきたら設計をみなおす //@fix
    return false;
  }

  //4byteアラインメントする必要がある
 // bit8* buffer = tempBuff;

  gfl2::std::MemClear(tempBuff,tempsize);
  gfl2::std::MemCopy(pData,tempBuff,sendsize);

  
  {
    pSendEvent->ClearSignal();

    result = nn::ir::Communicator::Send(tempBuff,sendsize,bufferSize,false);
    if(result.IsFailure()) {
      if(result == nn::ir::ResultNotConnected()){
        // 相手からの切断要求により接続が切断された可能性があります
      }
      NN_LOG("send failed\n");
    }

    // 送信は非同期に行われます。送信完了を知る必要が場合には、
    // 送信完了イベントを利用してください。
  }
  //SendEnd();//データを送ったらこのフラグセット
  return true;
}


bool IrAdapter::SetBeaconTest(const u8* pData, const u32 size)
{
  nn::Result result = nn::ResultSuccess();

  s32 usedSize = -1;
  size_t receiveSize = 0;

  
  //送信データ残量を確認し、何も残っていなければ送信する
  nn::ir::Communicator::GetSendSizeFreeAndUsed(NULL,NULL,NULL,&usedSize);
  if(usedSize != 0){
    NN_LOG("send used failed\n");
    return false;
  }
  //送信/受信用バッファを作成する
  size_t bufferSize = nn::ir::Communicator::CalculateBufferSizeToCommunicate(size);

  if(bufferSize > IRSEND_BUFFER){
    GFL_ASSERT(0); //ここにきたら設計をみなおす //@fix
    return false;
  }

  gfl2::std::MemClear(mpSendBuffer,IRSEND_BUFFER);
  gfl2::std::MemCopy(pData,mpSendBuffer,size);

  {
    maSendEvent.ClearSignal();

    result = nn::ir::Communicator::Send(mpSendBuffer,size,bufferSize,false);
    if(result.IsFailure()) {
      if(result == nn::ir::ResultNotConnected()){
        // 相手からの切断要求により接続が切断された可能性があります
        NN_LOG("send failed NotConnect\n");
      }
      else{
        NN_LOG("send failed\n");
      }
    }
  }
  return true;
}




bool IrAdapter::localRecvDataNoLoop(void* tempBuff, int size, int tempsize, nn::os::Event* pRecvEvent)
{
  //s32 usedSize = -1;
//  size_t receiveSize = 0;
  nn::Result result = nn::ResultSuccess();

//  if(!RecvMode()){
  //  //OHNO_PRINT("受信モードでない\n");
    //return false;
//  }
  // まずデータを受信しているか調べます
  if(!pRecvEvent->Wait(0)){
 //   //OHNO_PRINT("zyusinnsitena\n");
    return false;
  }
  //受信用バッファのサイズをえる
  size_t bufferSize = nn::ir::Communicator::CalculateBufferSizeToCommunicate( size );
//  //OHNO_PRINT("bufferSize %d\n",bufferSize);
  
  if(bufferSize > tempsize){
    GFL_ASSERT_STOP(0); //@fix  ここに来たら設計を見直す必要がある
  }
  
  //4byteアラインメントする必要がある
  bit8* buffer = (bit8*)tempBuff;

  // 受信されたデータを処理します
  noloopRecvData(buffer, bufferSize, pRecvEvent, &mReceiveSize);

  mAddRecvSize += mReceiveSize;

  //OHNO_PRINT("Get----  %d -- %d  add %d\n",usedSize, mReceiveSize,mAddRecvSize);


  
//  if(usedSize == 0){
    return true;   //もうデータはない
//  }
 // return false;  //まだデータはある
}



int IrAdapter::noloopRecvData(bit8* buffer, size_t bufferSize, nn::os::Event* pRecvEvent,size_t* pReceiveSize)
{
  s32 usedSize = -1;
  size_t receiveSize = 0;
  nn::Result result = nn::ResultSuccess();


  // 受信されたデータを処理します
  do{
    pRecvEvent->ClearSignal();

    result = nn::ir::Communicator::Receive(buffer,bufferSize ,pReceiveSize,&usedSize);

    if(result.IsFailure()){
      *pReceiveSize = 0;

      if(result == nn::ir::ResultInvalidData()) {
        // データが受信されましたが、検証の結果、不正なデータでした。
        // このとき、受信バッファからデータは取り除かれているため、
        // (存在するならば) 続けて後続のパケットを処理してください。
        //OHNO_PRINT("不正\n");
      }
      else if(result == nn::ir::ResultBufferInsufficient()) {
        // バッファサイズが不足しています。
        // このとき、データの検証処理を実行できないため、
        // データが不正かどうか判断できません。

        // このデータを受信処理するためには、GetNextReceiveDataSize() 関数で
        // 受信データのサイズを確認し、必要なサイズのバッファを用意してください。

        // このデモでは、想定するデータを受信するために必要なバッファは用意しているため、
        // 受信されたデータは不正なデータと思われます。

        // 受信バッファにはデータが残っています。
        // 不要なので、このデータは破棄して、続けて後続のパケットを処理します。
        GFL_ASSERT(0);  //どの程度起こるのか調査 //@fix
        nn::ir::Communicator::DropNextReceiveData(&usedSize);
      }
      else {
        // その他 ループを抜けます
        GFL_ASSERT(0);  //どの程度起こるのか調査 //@fix
        return usedSize;
      }
    }
//    //OHNO_PRINT("qqq\n");
  }while(usedSize > 0);
  return usedSize;
}

//---------------------------------------------------------------------------------
/**
 * @brief   受信データの破棄
 */
//---------------------------------------------------------------------------------

void IrAdapter::ClearReceiveBuffer(void)
{
  nn::ir::Communicator::ClearReceiveBuffer();
}


//---------------------------------------------------------------------------------
/**
 * @brief   接続後のID認証処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
// 接続後の ID 認証処理です
nn::Result IrAdapter::ConfirmID(u8 GameID)
{
  nn::Result result;

  // 接続時に要求側だったか応答側だったかを取得します。
  nn::ir::ConnectionRole role = nn::ir::Communicator::GetConnectionRole();

  // 通信ID を生成します。
  // 体験版を作成するとき、製品版と通信させたくない場合には、引数 isDemo を true としてください。
  bit32 communicationId = nn::ir::Communicator::CreateCommunicationId(mpInitParam->GetProductID());

  switch(role){
  case nn::ir::CONNECTION_ROLE_TO_REQUIRE:
    {
      // 要求側だった場合、ここでは RequireToConfirmID を実行します。
      //OHNO_PRINT("Require Confirm id..%d %d\n" , GameID,communicationId );

      // RequireToConfirmID は ReplyToConfirm より後に実行される必要があるため、
      // ウェイトをはさんでいます。
      nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(500));
      result = nn::ir::Communicator::RequireToConfirmId(GameID,
                                                        communicationId,
                                                        UDS_NETWORK_PASSPHRASE,
                                                        sizeof(UDS_NETWORK_PASSPHRASE));
    }
    break;
  case nn::ir::CONNECTION_ROLE_TO_WAIT:
    {
      // 応答側だった場合、ここでは WaitToConfirmID を実行します。
      //OHNO_PRINT("Wait Confirm id...%d %d\n" , GameID,communicationId);

      // 相手からの RequireToConfirmId を受け付けるまで Wait(1000 msec) します。
      result =
        nn::ir::Communicator::WaitToConfirmId(GameID, communicationId,
                                              UDS_NETWORK_PASSPHRASE,
                                              sizeof(UDS_NETWORK_PASSPHRASE),
                                              nn::fnd::TimeSpan::FromMilliSeconds(1000));
    }
    break;
  default:
    // 相手に切断されたので、ROLE_NONE となっている
    break;
  }
  return result;
}


//------------------------------------------------------------------
/**
 * @brief   エラーリスナー登録
 * @param   listener   通知を受け取るIrAdapterErrorListener
 */
//------------------------------------------------------------------
void IrAdapter::RegistErrorListener( IrAdapterErrorListener* listener )
{
  GFL_PRINT("IrAdapter::RegistErrorListener\n");
  m_errorListener = listener;
}

//------------------------------------------------------------------
/**
 * @brief   エラーリスナー解除
 */
//------------------------------------------------------------------
void IrAdapter::UnregistErrorListener( void )
{
  GFL_PRINT("IrAdapter::UnregistErrorListener\n");
  m_errorListener = NULL;
}

GFL_NAMESPACE_END( ir )
GFL_NAMESPACE_END( gflnet2 )

#endif

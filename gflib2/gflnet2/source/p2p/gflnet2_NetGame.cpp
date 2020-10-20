#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_NetGame.cpp
 * @brief	 送受信を管理するクラス
 * @author k.ohno
 * @date	 2011.5.24
 */
//=============================================================================

#include "gflnet2/include/p2p/gflnet2_RecvBuffer.h"
#include "gflnet2/include/p2p/gflnet2_NetGame.h"
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <nex.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)


NetGameBase* NetGame::mpNetGameBase = NULL;
int NetGame::mNetGameBaseCount = 0;
int NetGame::mNetGameTimingLoop = TIMING_MIN;


//---------------------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 */
//---------------------------------------------------------------------------------
NetGame::NetGame(gflnet2::InitParam* pInit)
//RecvHead mRecvHead[GFLNET_P2PCONNECT_MAXNUM][RECV_HEAD_MAX];  //受信をヘッダーで管理
: nowTimingMatchNo(0)  //全員が送ってきたタイミング番号
//, u16 timingNoBackup[GFLNET_P2PCONNECT_MAXNUM]; //タイミング番号Backup
, mTimingNoEnd(0) //終わりがきたタイミング番号  ここは
, mTimingNo(0) //タイミング番号
, mTimingNoReady(0) //タイミング番号準備
, mdmy16(0) //タイミング番号準備
, mBuffTimingNo(0)  //便利に一コマンドで終わるための取っておく数
, mMutualFlg(0) //相互通信用のフラグ
, mTimingInterval(0) // タイミングコマンドを連続させない
, mDmy2(0) //相互通信用のフラグ
, mDmy3(0) //相互通信用のフラグ
#if GFL_DEBUG
, debugTimingEnd(0)
, bDebugDelete(false)
#endif //GFL_DEBUG
{
  if(mpNetGameBase == NULL)
  {
    mpNetGameBase = GFL_NEW(pInit->GetHeapDev()) NetGameBase(pInit);
    mNetGameTimingLoop=TIMING_MIN;
    mNetGameBaseCount=0;
  }
  else
  {//引数付は通信システム以外が使用してはいけない
    GFL_ASSERT(0);//@fix
    mNetGameBaseCount++;
  }

  GFL_ASSERT(mpNetGameBase->GetMaxNum() <= GFLNET_P2PCONNECT_MAXNUM);  //開発段階でASSERT 本番では発生しない //@fix
  GFL_ASSERT(mpNetGameBase->GetMaxNum() != 1); //@fix

  GFL_PRINT("NetGame::NetGame %d\n",mpNetGameBase->GetMaxNum());
  ICHI_PRINT("NetGameBase を確保しました %d\n",mNetGameBaseCount);
#if GFL_DEBUG
    bDebugDelete=true;
#endif

}

//---------------------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 */
//---------------------------------------------------------------------------------
NetGame::NetGame()
//RecvHead mRecvHead[GFLNET_P2PCONNECT_MAXNUM][RECV_HEAD_MAX];  //受信をヘッダーで管理
: nowTimingMatchNo(0)  //全員が送ってきたタイミング番号
//, u16 timingNoBackup[GFLNET_P2PCONNECT_MAXNUM]; //タイミング番号Backup
, mTimingNoEnd(0) //終わりがきたタイミング番号  ここは
, mTimingNo(0) //タイミング番号
, mTimingNoReady(0) //タイミング番号準備
, mdmy16(0) //タイミング番号準備
, mBuffTimingNo(0)  //便利に一コマンドで終わるための取っておく数
, mMutualFlg(0) //相互通信用のフラグ
, mTimingInterval(0) // タイミングコマンドを連続させない
, mDmy2(0) //相互通信用のフラグ
, mDmy3(0) //相互通信用のフラグ
#if GFL_DEBUG
, debugTimingEnd(0)
, bDebugDelete(false)
#endif //GFL_DEBUG
{
  ::std::memset( mRecvHead , 0 , sizeof(mRecvHead) );
  ::std::memset( timingNoBackup , 0 , sizeof(timingNoBackup) );

  GFL_ASSERT(mpNetGameBase);//@fix
  mNetGameBaseCount++;

  ICHI_PRINT("NetGame を確保しました %d\n",mNetGameBaseCount);
#if GFL_DEBUG
    bDebugDelete=true;
#endif

}


NetGame::~NetGame()
{
#if GFL_DEBUG
  if( !bDebugDelete ){
    GFL_ASSERT_MSG(0,"解放前にFinalizeを呼んで下さい\n"); //@fix
  }
#endif


  //OHNO_PRINT("NetGame を解放しました %d\n",mNetGameBaseCount);
  ICHI_PRINT("NetGame を解放しました %d\n",mNetGameBaseCount);
  if(mNetGameBaseCount){
    mNetGameBaseCount--;
  }
  else{
    GFL_SAFE_DELETE( mpNetGameBase );
    mpNetGameBase = NULL;
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  初期化
 */
//---------------------------------------------------------------------------------
void NetGame::Initialize(void)
{
  int i;

  if(mpNetGameBase){
    
    //OHNO_PRINT("NetGame::Initialize %d\n", mNetGameBaseCount);
    ICHI_PRINT("NetGame::Initialize %d\n", mNetGameBaseCount);
    mpNetGameBase->SetNetGame(this);
    mpNetGameBase->ConnectSetting();
    if(mNetGameBaseCount>0){
      IncTimingNo();
      mpNetGameBase->TimingStart( mNetGameTimingLoop );
    }
  }
  mTimingInterval=0;
#if GFL_DEBUG
  bDebugDelete=false;
#endif
#if GFL_DEBUG
  debugTimingEnd = 0;
#endif //GFL_DEBUG
}



//---------------------------------------------------------------------------------
/**
 * @brief  初期化が終わったかどうか
 * @return 初期化できたらtrue
 */
//---------------------------------------------------------------------------------
bool NetGame::IsInitialize(void)
{
  int i;

  if(mNetGameBaseCount>0){
    if(!mpNetGameBase->IsTimingEnd( mNetGameTimingLoop )){
      return false;
    }
  }
  return true;
}


//---------------------------------------------------------------------------------
/**
 * @brief  終了   強制的に必ず終了します
 */
//---------------------------------------------------------------------------------
void NetGame::Finalize(void)
{
  if(mpNetGameBase){
    mpNetGameBase->RemoveNetGame(this);
  }
#if GFL_DEBUG
  bDebugDelete=true;
#endif

}


//---------------------------------------------------------------------------------
/**
 * @brief  終了同期開始
 */
//---------------------------------------------------------------------------------
void NetGame::FinalizeSync(void)
{
  if(mpNetGameBase){
    IncTimingNo();
    mpNetGameBase->TimingStart( mNetGameTimingLoop );
  }
}



//---------------------------------------------------------------------------------
/**
 * @brief  終了同期待ち
 */
//---------------------------------------------------------------------------------
bool NetGame::IsFinalizeSync(void)
{

  if(mNetGameBaseCount>0){
    if(!mpNetGameBase->IsTimingEnd( mNetGameTimingLoop )){
      return false;
    }
  }
  return true;
}


//---------------------------------------------------------------------------------
/**
* @brief  電源OFF強制終了前準備（NetGameBaseをDeleteしないとNexメモリリークにひっかかる）
*/
//---------------------------------------------------------------------------------
void NetGame::PreparePowerOff(void)
{
  mNetGameBaseCount=0;
}


//---------------------------------------------------------------------------------
/**
 * @brief  ヘッダーをクリアする
 */
//---------------------------------------------------------------------------------

void NetGame::ClearHeader(void)
{
  gfl2::std::MemClear(mRecvHead, sizeof(mRecvHead));
}

//---------------------------------------------------------------------------------
/**
 * @brief ヘッダーを登録する
 */
//---------------------------------------------------------------------------------

bool NetGame::PushHeader(int netid, u8 sysCommand, u8 gameNo,u8 targetBit,u8 userCommand, u16 size,u16 index)
{
  int i;

 // NN_LOG("mRecvHead %x\n", netid);

  int myid = mpNetGameBase->GetNetID();
  if(myid!=gflnet2::p2p::NetGameBase::INVALID_NET_ID){
    if(targetBit!=0){
      if(!(( 0x01 << myid ) &  targetBit)){
      //  NN_LOG("HEAD登録しない netid %x myid %x targetBit %x\n", netid, myid, targetBit);
        return false;
      }
    }
  }

  for(i = 0; i < RECV_HEAD_MAX; i++){
    if(mRecvHead[netid][i].sysCommand == GFLNET_NO_DATA){
//      NN_LOG("PushHeader %d %d %d\n",sysCommand,netid,userCommand);
      mRecvHead[netid][i].sysCommand = sysCommand;
      mRecvHead[netid][i].gameNo = gameNo;
      mRecvHead[netid][i].targetBit = targetBit;
      mRecvHead[netid][i].userCommand = userCommand;
      mRecvHead[netid][i].size = size;
      mRecvHead[netid][i].index = index;
      return true;
    }
  }
  return false;

}


//---------------------------------------------------------------------------------
/**
 * @brief  コマンドの解析処理
 * @param  netid        通信ID
 * @param  sysCommand   システムコマンド
 * @return ヘッダーポインタを返す
 */
//---------------------------------------------------------------------------------

RecvHead* NetGame::commandCheck(int netid,u8 sysCommand)
{
  int i;

  for(i = 0; i < RECV_HEAD_MAX; i++){
    if(mRecvHead[netid][i].sysCommand == sysCommand){
      return &mRecvHead[netid][i];
    }
  }
  return NULL;
}


//---------------------------------------------------------------------------------
/**
 * @brief  コマンドの消去処理
 * @param  netid     通信ID
 * @param  sysCommand   システムコマンド
 */
//---------------------------------------------------------------------------------

void NetGame::commandDelete(int netid,u8 sysCommand)
{
  int i;

  for(i = 0; i < RECV_HEAD_MAX; i++){
    if(mRecvHead[netid][i].sysCommand  == sysCommand){
      mRecvHead[netid][i].sysCommand = GFLNET_NO_DATA;
      break;
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  通信コマンドの引き出し処理（データを引き出し消す）
 * @param  pRecvBuff  受信バッファ管理構造体
 * @param  pHead  受信バッファヘッダ構造体
 * @param  sysCommand   システムコマンド
 * @return 受け取れたら true
 */
//---------------------------------------------------------------------------------

bool NetGame::queuePop(int netid, RecvHead* pHead, u8 sysCommand)
{

  RecvHead* pRHead = commandCheck(netid, sysCommand);
  if(pRHead){
    gfl2::std::MemCopy(pRHead, pHead, sizeof(RecvHead));
    commandDelete(netid, sysCommand);
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------
/**
 * @brief  タイミングコマンドの解析処理
 * @param  pRecvBuff  受け取ったデータの管理構造体
 */
//---------------------------------------------------------------------------------

void NetGame::timingCheck(int netid, RecvBuffer* pRecv)
{
  int i;
  bool flg =false;

  for(i = 0; i < RECV_HEAD_MAX; i++){
    if(mRecvHead[netid][i].sysCommand == GFLNET_TIMEST_DATA){

      pRecv->dataCopy(mRecvHead[netid][i].index,
                      reinterpret_cast<char*>(&timingNoBackup[netid]), sizeof(u16));

      GFL_ASSERT_MSG(flg==false, "A GFLNET_TIMEST_DATA 格納 %d NO %d %d\n",netid ,timingNoBackup[netid], i); //@fix

      mRecvHead[netid][i].sysCommand = GFLNET_NO_DATA;
      NET_TIMING("GFLNET_TIMEST_DATA 格納 %d NO %d %d\n",netid ,timingNoBackup[netid], i);

      flg = true;
      
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  親機でのタイミングコマンドの解析処理
 * @param  pRecvBuff  受け取ったデータの管理構造体
 * @return bool      タイミングがそろったらtrue
 */
//---------------------------------------------------------------------------------

bool NetGame::parentTimingCheck(void)
{
  int i;  //自分の設定したタイミング番号

  if(!mpNetGameBase->IsMaster()){
    return false;
  }
  if(mTimingNo == 0){
    return false;
  }

  if( mpNetGameBase == NULL )
  {//こないはず
    GFL_ASSERT(0);//@fix
    return false;
  }

  for( i = 0;i < mpNetGameBase->GetMaxNum() ; i++){
    nn::nex::StationID stationID = NetGameBase::RecvBufferIndex2ConnectionID(i);
    if( stationID == NetGameBase::INVALID_STATION_ID && NetGameBase::IsMigrationEnable() )
    {//マイグレーション有効の場合は切断済みはそろったことにする
      continue;
    }
    else if( stationID != 0 )
    {  //接続者だけ対応
      if( timingNoBackup[i] != mTimingNo )
      {  //番号不一致はそろっていない
        OHNO_PRINT("不一致%d %d %d \n",i, timingNoBackup[i] , mTimingNo);
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  if(mpNetGameBase){
    if(mpNetGameBase->SendData(this,GFLNET_TIMEED_DATA,0,0,
                               reinterpret_cast<char*>(&mTimingNo), 2)){
      NET_TIMING("GFLNET_TIMEED_DATA Send %d  \n",mTimingNo);
      mTimingNo=0;//親機のmTimingNoを０にする事で２回目を送信させない
    }
  }
  return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief  タイミング終了の到着チェック
 * @param  pRecvBuff  受け取ったデータの管理構造体
 * @return bool      タイミングがそろったらtrue
 */
//---------------------------------------------------------------------------------

bool NetGame::endTimingCheck(int index , RecvBuffer* pRecv)
{
  RecvHead* pHead = commandCheck(index, GFLNET_TIMEED_DATA);

  if(pHead){
    pRecv->dataCopy(pHead->index,
                      reinterpret_cast<char*>(&mTimingNoEnd), sizeof(mTimingNoEnd));
    commandDelete(index, GFLNET_TIMEED_DATA);
    NET_TIMING("GFLNET_TIMEED_DATA > mTimingNoEnd  %d index%d\n",mTimingNoEnd,index);
    mTimingNo = 0;
    mTimingInterval = 2;

#if GFL_DEBUG
    GFL_ASSERT(debugTimingEnd==0); //@fix
    debugTimingEnd = mTimingNoEnd;
#endif //GFL_DEBUG
    return true;

  }
  return false;
}

//---------------------------------------------------------------------------------
/**
 * @brief  データ送信 内部用
 * @param  command   ヘッダー
 * @param  sendData  送るデータ  UDS_PACKET_PAYLOAD_MAX_SIZE を超えると、関数の実行は失敗します。
 * @param  size      送信サイズ
 * @return bool      送る準備ができたらtrue だめならfalse
 */
//---------------------------------------------------------------------------------
bool NetGame::sendDataLocal(u8 sysCommand, u8 targetBit,u8 userCommand, char* pSendData, int size)
{
  if(mpNetGameBase){

    GFL_ASSERT(false == CheckBloken()); //@fix
    return mpNetGameBase->SendData(this, sysCommand, targetBit, userCommand, pSendData, size);
  }
  return false;
}


//---------------------------------------------------------------------------------
/**
 * @brief  データ受信汎用処理
 */
//---------------------------------------------------------------------------------

void NetGame::Func(gflnet2::p2p::RecvBuffer* mRecv[GFLNET_P2PCONNECT_MAXNUM])
{
  u8 data;

  //タイミング送信フェーズ
  if(mTimingInterval==0){
    if(mTimingNoReady != 0){
      if(sendDataLocal(GFLNET_TIMEST_DATA,0,0,
                       reinterpret_cast<char*>(&mTimingNoReady), 2)){
        NET_TIMING("GFLNET_TIMEST_DATA %d \n",mTimingNoReady);
        mTimingNo = mTimingNoReady;  //送信完了
        mTimingNoReady = 0;
      }
    }
  }
  else{
    mTimingInterval--;
  }

  if( mpNetGameBase == NULL )
  {//こないはず
    GFL_ASSERT(0);//@fix
    return;
  }

  //受信処理
  for(int i = 0;i < mpNetGameBase->GetMaxNum() ; i++){
    timingCheck(i, mRecv[i]);
  }
  //親機のみタイミング送信フェーズ
  parentTimingCheck();
  //タイミングエンドコマンドが親機からきたかどうか
  for(int i = 0;i < mpNetGameBase->GetMaxNum() ; i++){
    endTimingCheck(i,mRecv[i]);
  }
}




//---------------------------------------------------------------------------------
/**
 * @brief  データ送信 
 * @param  sendData  送るデータ  UDS_PACKET_PAYLOAD_MAX_SIZE を超えると、関数の実行は失敗します。
 * @param  size      送信サイズ
 * @return bool      送る準備ができたらtrue だめならfalse
 */
//---------------------------------------------------------------------------------
bool NetGame::SendData(char* pSendData, int size)
{
  NET_PRINT("SendData%d \n",size);
  return sendDataLocal(GFLNET_NORMAL_DATA,0,0, pSendData, size);
}

//---------------------------------------------------------------------------------
/**
 * @brief  データ送信 
 * @param  sendData  送るデータ  UDS_PACKET_PAYLOAD_MAX_SIZE を超えると、関数の実行は失敗します。
 * @param  size      送信サイズ
 * @return bool      送る準備ができたらtrue だめならfalse
 */
//---------------------------------------------------------------------------------
bool NetGame::SendDataCommand( char* pSendData,  int size,  u8 command)
{
  NET_PRINT("SendDataCommand %d \n",size);
  return sendDataLocal(GFLNET_NORMAL_DATA, 0, command, pSendData, size);
}
//---------------------------------------------------------------------------------
/**
 * @brief  コマンド付きデータ送信 送信指定BITつき
 * @param  sendData  送るデータ
 * @param  size      送信サイズ
 * @param  command   コマンドNO
 * @param  playerBit   送る先のBIT
 * @return bool      送る準備ができたらtrue だめならfalse
 */
//---------------------------------------------------------------------------------

bool NetGame::SendDataCommandSendBit( char* pSendData,  int size,  u8 command, u8 playerBit)
{
  NET_PRINT("SendDataCommand %d \n",size);
  return sendDataLocal(GFLNET_NORMAL_DATA, playerBit, command, pSendData, size);
}


//---------------------------------------------------------------------------------
/**
 * @brief  データ受信
 * @param  id        送ってきた人の通信ID
 * @param  RecvData  受け取るバッファ
 * @param  size      受信バッファサイズ これ以上は受け取りません
 * @return bool      データがあればtrue なければfalse
 */
//---------------------------------------------------------------------------------
bool NetGame::RecvData(int id, char* pRecvData, int size)
{
  RecvHead head;
  int buffid=0;

  GFL_ASSERT(mpNetGameBase); //@fix
  if(mpNetGameBase==NULL){
    return false;
  }

  buffid = mpNetGameBase->ParentIndex2RecvBuffer(id);

  
  if(buffid != -1){


    if(queuePop(buffid, &head, GFLNET_NORMAL_DATA)){

      NET_PRINT("queuePop NetId %d index %d  buffsize %d\n", id, head.index, size);

      mpNetGameBase->DataCopy(buffid, head.index, pRecvData, size);
      return true;
    }
  }
  return false;
}


//---------------------------------------------------------------------------------
/**
 * @brief  データ受信
 * @param  id        送ってきた人の通信ID
 * @param  RecvData  受け取るバッファ
 * @param  size      受信バッファサイズ これ以上は受け取りません
 * @param  pSize     受け取ったサイズを返す
 * @return bool      データがあればtrue なければfalse
 */
//---------------------------------------------------------------------------------
bool NetGame::RecvData(int id, char* pRecvData, int size, u16* pSize)
{
  RecvHead head;
  int buffid=0;

  GFL_ASSERT(mpNetGameBase);  //@fix
  if(mpNetGameBase==NULL){
    return false;
  }

  buffid = mpNetGameBase->ParentIndex2RecvBuffer(id);
  if(buffid != -1){
    if(queuePop(buffid, &head, GFLNET_NORMAL_DATA)){
      NET_PRINT("queuePop NetId %d index %d  buffsize %d\n", id, head.index, size);
      mpNetGameBase->DataCopy(buffid, head.index, pRecvData, size);
      if(pSize != NULL){
        *pSize = head.size;
      }
      return true;
    }
  }
  return false;
}




//---------------------------------------------------------------------------------
/**
 * @brief  データ受信
 * @param  id        送ってきた人の通信ID
 * @param  RecvData  受け取るバッファ
 * @param  size      受信バッファサイズ これ以上は受け取りません
 * @param  pCommand  ユーザーコマンドを返す
 * @param  pSize     受け取ったサイズを返す
 * @return bool      データがあればtrue なければfalse
 */
//---------------------------------------------------------------------------------
bool NetGame::RecvDataCommand(int* pId, char* pRecvData, int size , u8* pCommand, u16* pSize)
{
  RecvHead head;
  int buffid=0;
  int z;

  GFL_ASSERT(mpNetGameBase); //@fix
  if(mpNetGameBase==NULL){
    return false;
  }

  for(z = 0 ; z < GFLNET_P2PCONNECT_MAXNUM; z++){

    buffid = mpNetGameBase->ParentIndex2RecvBuffer(z);
    if(buffid != -1){
      if(queuePop(buffid, &head, GFLNET_NORMAL_DATA)){
        mpNetGameBase->DataCopy(buffid, head.index, pRecvData, size);
        *pCommand = (u8)head.userCommand;
        NET_SIZELOG("queuePop id %d index %d  size %d cmd %d\n", z,head.index, head.size, head.sysCommand);
        *pId = z;
        if(pSize != NULL){
          *pSize = head.size;
        }
        return true;
      }
    }
  }
  return false;
}



//---------------------------------------------------------------------------------
/**
 * @brief  データ受信  コマンド指定版
 * @param  id        送ってきた人の通信ID
 * @param  RecvData  受け取るバッファ
 * @param  size      受信バッファサイズ これ以上は受け取りません
 * @param  getCommand  ほしいユーザーコマンド
 * @param  pSize     受け取ったサイズを返す
 * @return bool      データがあればtrue なければfalse
 */
//---------------------------------------------------------------------------------
bool NetGame::RecvDataCommandLocalCommand(int* pId, char* pRecvData, int size , u8 getCommand, u16* pSize)
{
  RecvHead head;
  int buffid=0;
  int z;

  GFL_ASSERT(mpNetGameBase); //@fix
  if(mpNetGameBase==NULL){
    return false;
  }

  for(z = 0 ; z < GFLNET_P2PCONNECT_MAXNUM; z++){

    buffid = mpNetGameBase->ParentIndex2RecvBuffer(z);
    if(buffid != -1){
      RecvHead* pRHead = commandCheck(buffid, GFLNET_NORMAL_DATA);
      if(pRHead){
        if( (u8)pRHead->userCommand == getCommand){
       //   //OHNO_PRINT("こまんどいっしょ %d\n",getCommand);
          
          if(queuePop(buffid, &head, GFLNET_NORMAL_DATA)){
            mpNetGameBase->DataCopy(buffid, head.index, pRecvData, size);
            //OHNO_PRINT("queuePop id %d index %d  size %d cmd %d\n", z,head.index, head.size, head.sysCommand);
            *pId = z;
            if(pSize != NULL){
              *pSize = head.size;
            }
            return true;
          }
        }
      }
    }
  }
  return false;
}


//---------------------------------------------------------------------------------
/**
 * @brief  タイミングをそろえるコマンドを発行確認まで内部でやってくれる
 * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
 * @return bool              到着したタイミングから次のスタートを行うまでずっとtrue
 */
//---------------------------------------------------------------------------------
bool NetGame::IsTimingLoopCheck(int timingUniqueNo)
{
  if(mBuffTimingNo == 0){
    TimingStart(timingUniqueNo);
    mBuffTimingNo = timingUniqueNo;
    GFL_PRINT(">TIMING1 mBuffTimingNo[%d]\n", mBuffTimingNo);
    return false;
  }
  else{
    GFL_ASSERT( mBuffTimingNo == timingUniqueNo );
    if(IsTimingEnd(mBuffTimingNo)){
      GFL_PRINT("TIMING2 mBuffTimingNo[%d]\n", mBuffTimingNo);
      mBuffTimingNo=0;
      return true;
    }
  }
  return false;
}



//---------------------------------------------------------------------------------
/**
 * @brief  タイミングをそろえるコマンドの送信
 * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
 * ※必ず送るので戻り値は無いですが 同じ番号を連続したりすると挙動がおかしくなります
 * 通信の遷移をしっかり使う側で把握してください
 */
//---------------------------------------------------------------------------------
void NetGame::TimingStart(int timingUniqueNo)
{
  GFL_ASSERT(mTimingNoReady==0); //@fix
  GFL_ASSERT(mTimingNo==0); //@fix
  GFL_ASSERT(timingUniqueNo!=0); //@fix
  GFL_ASSERT(mBuffTimingNo==0); //@fix
  mTimingNoEnd = 0;
  mTimingNoReady = timingUniqueNo;
  
  NET_TIMING("TimingStart %d\n",timingUniqueNo);
}

//---------------------------------------------------------------------------------
/**
 * @brief  タイミングをそろえるコマンドが到着したかどうか
 * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
 * @return bool              到着したタイミングから次のスタートを行うまでずっとtrue
 */
//---------------------------------------------------------------------------------
bool NetGame::IsTimingEnd(int timingUniqueNo)
{
  GFL_ASSERT(timingUniqueNo != 0); //@fix
  u16 x = timingUniqueNo;

  if((mTimingNoEnd==x) && (x != 0)){
#if GFL_DEBUG
    if(debugTimingEnd == x){
      debugTimingEnd = 0;
    }
    else if(debugTimingEnd!=0){
      GFL_ASSERT_MSG(0,"debugTimingEnd  x  %d %d \n",debugTimingEnd,x); //@fix
    }
#endif //GFL_DEBUG
    return true;
  }
  return false;
}


//--------------------------------------------------------------------------
/**
 * @brief		同期NOのインクリメント
 */
//--------------------------------------------------------------------------
void NetGame::IncTimingNo( void )
{
  mNetGameTimingLoop++;
  if(mNetGameTimingLoop > TIMING_MAX){
    mNetGameTimingLoop = TIMING_MIN;
  }
}


//---------------------------------------------------------------------------------
/**
 * @brief   コネクションした人数を得る
 * @return  コネクション人数
 */
//---------------------------------------------------------------------------------
int NetGame::GetConnectionCount(void)
{
  return mpNetGameBase->GetConnectionCount();
}


bool NetGame::IsMaster(void)
{
  return mpNetGameBase->IsMaster();
}

//---------------------------------------------------------------------------------
/**
 * @brief   接続順番 = NetIDをかえす
 * @return  NetID
 */
//---------------------------------------------------------------------------------
int NetGame::GetNetID(void)
{
  return mpNetGameBase->GetNetID();
}


//--------------------------------------------------------------------------
/**
 * @brief		同期NOのバックアップをクリア
 */
//--------------------------------------------------------------------------
void NetGame::ClearTimingBackup( void )
{
  OHNO_PRINT("同期NOのバックアップをクリア\n");
  // タイミングバックアップは初期化しないと被る
	gfl2::std::MemClear( timingNoBackup, sizeof(u16)*GFLNET_P2PCONNECT_MAXNUM );
}


//--------------------------------------------------------------------------
/**
 * @brief		すでに壊れているかどうか
 */
//--------------------------------------------------------------------------

bool NetGame::CheckBloken( void )
{
  if(-1 == mpNetGameBase->CheckNetGameNo(this)){
    return true;
  }
  return false;
}

bool NetGame::IsDisconnectNetID( int netID )
{ 
  return mpNetGameBase->IsDisconnectNetID( netID ); 
}

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)


#endif

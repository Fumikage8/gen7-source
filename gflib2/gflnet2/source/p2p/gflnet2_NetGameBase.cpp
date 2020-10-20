#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_NetGameBase.cpp
 * @brief	 送受信を管理する基本クラス
 * @author k.ohno
 * @date	 2011.5.24
 */
//=============================================================================

#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"
#include "gflnet2/include/dev/gflnet2_NetZAdapter.h"

#include <nex.h>
#include "gflnet2/include/p2p/gflnet2_RecvBuffer.h"
#include "gflnet2/include/p2p/gflnet2_NetGame.h"
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include <gflnet2/source/dev/gflnet2_IrAdapter.h>

#include <math/include/gfl2_math_crc.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

#if GFL_DEBUG
u8 NetGameBase::mpDebugBuff[NetGameBase::DEBUGCOMMNAD_NUM][NetGameBase::DEBUGCOMMNAD_BUFF];
int NetGameBase::DebugBuffCount;
#endif

gflnet2::p2p::RecvBuffer* NetGameBase::mRecv[GFLNET_P2PCONNECT_MAXNUM]; // 受信バッファ管理構造体


u8 NetGameBase::mMigrationCallbackInc[GFLNET_P2PCONNECT_MAXNUM];    //マイグレーション復帰様インクリメント数
u8 NetGameBase::mCallbackInc[GFLNET_P2PCONNECT_MAXNUM];    //インクリメント数
u8 NetGameBase::mCallbackIncFlgBit;  //受け取りフラグBIT

u8 NetGameBase::mRecvInc[GFLNET_P2PCONNECT_MAXNUM];    //インクリメント数
u8 NetGameBase::mRecvIncFlgBit;  //受け取りフラグBIT

nn::nex::StationID NetGameBase::mParentCommunicationID[GFLNET_P2PCONNECT_MAXNUM];  //親機から送られてきたCommunicationID順番
nn::nex::StationID NetGameBase::mDisconnectCommunicationID[GFLNET_P2PCONNECT_MAXNUM];  //切断リスト
nn::nex::StationID NetGameBase::mJoinMember[GFLNET_P2PCONNECT_MAXNUM];  //親機から送られてきたCommunicationID順番

nn::nex::StationID NetGameBase::mMyConnectionID;
gflnet2::InitParam* NetGameBase::mpInitParam;

u8* NetGameBase::mIrTempBuff;


bool NetGameBase::mMigrationStopFlg;///
bool NetGameBase::mbMaster;
bool NetGameBase::mSetCallbackFlag;
bool NetGameBase::mbConnectionSend;
int NetGameBase::mSendTimer;
int NetGameBase::mMax;

proc::P2pMainIr* NetGameBase::mpP2pIrLoopProc;

NetGameErrorListener* NetGameBase::m_errorListener = NULL;
NetGameMigrationListener* NetGameBase::m_migrationListener = NULL;


//---------------------------------------------------------------------------------
/**
 * @brief  親機のindexから受信バッファindexを得る
 * @param  index
 * @return index
 */
//---------------------------------------------------------------------------------
int NetGameBase::ParentIndex2RecvBuffer(int index)
{
  int i;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if(mParentCommunicationID[index]!=0){
      if( mRecv[i] && mRecv[i]->GetConnectionID() == mParentCommunicationID[index]){
        return i;
      }
    }
  }
  return -1;
}


int NetGameBase::ConnectionID2RecvBufferIndex(nn::nex::StationID id)
{
  int i,okflg = 0;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if(mJoinMember[i] == id){
      break;
    }
  }
  if(i == GFLNET_P2PCONNECT_MAXNUM){
    return -1;
  }
  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if( mRecv[i] && mRecv[i]->GetConnectionID() == id){
      return i;
    }
  }
  return -1;

}

nn::nex::StationID NetGameBase::RecvBufferIndex2ConnectionID(int indexx)
{
  int index = indexx;
  GFL_ASSERT(index < GFLNET_P2PCONNECT_MAXNUM);//@fix
  if(index >=GFLNET_P2PCONNECT_MAXNUM){
    index = 0;
  }

  //切断していたら無効なIDを返す
  for(int i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++)
  {
    if( mRecv[index] && ( mDisconnectCommunicationID[i] == mRecv[index]->GetConnectionID() ) )
    {
      return INVALID_STATION_ID;
    }
  }

  if( mRecv[index] )
  {
    return mRecv[index]->GetConnectionID();
  }
  else
  {
    return INVALID_STATION_ID;
  }
}

int NetGameBase::SetConnectionIDRecvBuffer(nn::nex::StationID id,int line)
{
  int i;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if( mRecv[i] && mRecv[i]->GetConnectionID() == id){
      return i;
    }
    else if( mRecv[i] && mRecv[i]->GetConnectionID() == 0){
      //OHNO_PRINT(" インデックス %d はID %d   \n",i,id);
      mRecv[i]->SetConnectionID(id);
      ICHI_PRINT("ICHI>mRecv[0].SetConnectionID3(%x)\n", id );
//      if(IsMaster()){
//        mbConnectionSend=true;
//      }
      return i;
    }
  }
  NET_PRINT("%d -- %d\n",id,line);
  GFL_ASSERT(0);  //@fix
  return 0;

}

void NetGameBase::DirectSetConnectionIDRecvBuffer(nn::nex::StationID id)
{
  int i;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if(mJoinMember[i]==0){
      mJoinMember[i]=id;
      break;
    }
  }
  
  if(IsMaster()){
    i = SetNetID(id);
    if( i < GFLNET_P2PCONNECT_MAXNUM )
    {
      if( mRecv[i] )
      {
        mRecv[i]->SetConnectionID(id);
        ICHI_PRINT("ICHI>mRecv[%d].SetConnectionID1(%x)\n", i, id );
      }
    }
  }
  else{
    u32 x = id & 0xff;  ///親だけ固定 おやのみを先に登録する
    if(x==1){
      //OHNO_PRINT("子機セット %d %x\n", 0, id);
      SetNetID(id);
      if( mRecv[0] )
      {
        mRecv[0]->SetConnectionID(id);
        ICHI_PRINT("ICHI>mRecv[0].SetConnectionID1(%x)\n", id );
      }
    }
  }
}



void NetGameBase::SetConnectionIDRecvBufferChild(void)
{
  for(int i=1;i<GFLNET_P2PCONNECT_MAXNUM;i++){
    if(mParentCommunicationID[i]!=0){
      //OHNO_PRINT("子機セット %d %x\n",i,mParentCommunicationID[i]);
      if( mRecv[i] )
      {
        mRecv[i]->SetConnectionID(mParentCommunicationID[i]);
        ICHI_PRINT("ICHI>mRecv[%d].SetConnectionID2(%x)\n",i, mParentCommunicationID[i] );
      }
    }
  }
}




int NetGameBase::GetConnectionCount(void)
{
  int i,j=0;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if( mRecv[i] && mRecv[i]->GetConnectionID() != 0){
      j++;
    }
  }
  return j;

}



int NetGameBase::ResetConnectionIDRecvBuffer(nn::nex::StationID id)
{
  int i;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if( mRecv[i] && mRecv[i]->GetConnectionID() == id){
      mRecv[i]->SetConnectionID(0);
      return i;
    }
  }
#if GFL_DEBUG
  NN_LOG("NotResetConnectionID %d \n",id);
#endif
  return 0;

}


//---------------------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 */
//---------------------------------------------------------------------------------
NetGameBase::NetGameBase(gflnet2::InitParam* pInitParam)
{
  // 開発段階でASSERT 本番では発生しない
  gfl2::std::MemClear(mpNetGameBuffer,sizeof(mpNetGameBuffer));
  GFL_ASSERT(pInitParam->GetLimitNum() <= GFLNET_P2PCONNECT_MAXNUM);  //@fix
  GFL_ASSERT(pInitParam->GetLimitNum() != 1);  //@fix
  //OHNO_PRINT("NetGame::NetGame %d\n",mGameInitialize.max);
  ICHI_PRINT("ICHI>NetGameBase::NetGameBase %d\n",pInitParam->GetLimitNum());

  GFL_ASSERT(pInitParam);  //@fix
  mpInitParam = pInitParam;

  mSendTimer = WAIT_TIME_CONNECT;
  mSetCallbackFlag=false;
  mMigrationStopFlg=false;

  mMyConnectionID = 0;
  for(int j=0;j<GFLNET_P2PCONNECT_MAXNUM;j++){
    mRecv[j] = GFL_NEW( pInitParam->GetHeapDev() ) gflnet2::p2p::RecvBuffer();
    mRecv[j]->Clear();
    mCallbackInc[j] = 0;
    mParentCommunicationID[j]=0;
    mDisconnectCommunicationID[j]=0;
    mRecvInc[j]=0;
//    mSendType[j]=true;
    mSendTypeTimeoutCount[j]=0;
    mJoinMember[j]=0;
  }
  mCallbackIncFlgBit = 0;
  mRecvIncFlgBit = 0;
  mMyConnectionID = 0;
  mbMaster=false;
  mbConnectionSend=false;
  mSendNumber = 0;  //送信番号 連番管理のため

  mSendBufferIndex = 0;  //mSendのどこまでデータが入っているか
  mMySendInc = 0;
  mSendFlg = 0;  //送信したら1 送信したことが帰ってきたら０
  mSendFailedFlg=false;

  m_isEnableStartGame = false;
  
  mMax = pInitParam->GetLimitNum();
  mIrTempBuff=NULL;
  if(mpInitParam->IsIr()){
    mIrTempBuff = (u8*)GflHeapAllocMemoryAlign(pInitParam->GetHeapDev(), IR_TEMP_BUFF_MAX,4);
  }
#if GFL_DEBUG
  DebugBuffCount=0;
#endif
  std::memset( mMigrationCallbackInc , 0 , sizeof(mMigrationCallbackInc) );
}


//---------------------------------------------------------------------------------
/**
 * @brief  デストラクタ
 */
//---------------------------------------------------------------------------------
NetGameBase::~NetGameBase()
{
  for(int j=0;j<GFLNET_P2PCONNECT_MAXNUM;j++)
  {
    GFL_SAFE_DELETE( mRecv[j] );
  }

  if(mIrTempBuff){
    GflHeapFreeMemory( mIrTempBuff );
    mIrTempBuff = NULL;
  }
}



//---------------------------------------------------------------------------------
/**
 * @brief  NetGameの登録
 * @param  pNetGame NetGameクラスのポインタ
 */
//---------------------------------------------------------------------------------
void NetGameBase::SetNetGame(NetGame* pNetGame)
{

  for(int i = 0 ; i < NETGAME_NUM_MAX;i++){
    if(mpNetGameBuffer[i]==NULL){
      //OHNO_PRINT("SetNetGame %d %x\n",i,pNetGame);
      ICHI_PRINT("SetNetGame %d %x\n",i,pNetGame);
      mpNetGameBuffer[i] = pNetGame;

      return;
    }
  }
  GFL_ASSERT(0);  //@fix
  //もしここにきたら単純にNETGAME_NUM_MAXを増やしていいです
}


//---------------------------------------------------------------------------------
/**
 * @brief  NetGameの削除
 * @param  pNetGame NetGameクラスのポインタ
 */
//---------------------------------------------------------------------------------
void NetGameBase::RemoveNetGame(NetGame* pNetGame)
{
  for(int i = 0 ; i < NETGAME_NUM_MAX;i++){
    if( mpNetGameBuffer[i] != NULL ){
      //OHNO_PRINT("RemoveNetGame %d %x\n", i,mpNetGameBuffer[i]);
    }
  }

  for(int i = 0 ; i < NETGAME_NUM_MAX;i++){
    ICHI_PRINT("RemoveNetGame %d %x\n", i,mpNetGameBuffer[i]);

    if(mpNetGameBuffer[i] == pNetGame){
      //OHNO_PRINT("RemoveNetGame %d %x\n",i,pNetGame);
      ICHI_PRINT("RemoveNetGame %d %x\n",i,pNetGame);
      mpNetGameBuffer[i] = NULL;

      //解放の順番ミス NetGame::Finalize呼び出しが無い
      // 電源押し時の解放処理ではこのアサートをみないので、デバッグ時に出るようにした
      // もうだしません
//#ifdef ENABLE_NEX_DEBUG_CLIENT
//      GFL_ASSERT(mpNetGameBuffer[i+1] == NULL);  //@fix
//#endif
      if(i==0){  //ゼロなら削除
        DisConnect();
      }
      return;
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  NetGameが何番目の登録か得る
 * @param  pNetGame NetGameクラスのポインタ
 * @return 何番目なのか
 */
//---------------------------------------------------------------------------------

int NetGameBase::getNetGameNo(NetGame* pNetGame)
{
  if(pNetGame==NULL){
    return 0;
  }
  for(int i = 0 ; i < NETGAME_NUM_MAX;i++){
    if(mpNetGameBuffer[i] == pNetGame){
      return i;
    }
  }
  GFL_ASSERT(0);  // このアクセスは返すものが無い 通信エラーにする しかしここに来るのは設計ミス  //@fix
  return 0;
}


//---------------------------------------------------------------------------------
/**
 * @brief  NetGameが何番目の登録か得る
 * @param  pNetGame NetGameクラスのポインタ
 * @return 何番目なのか
 */
//---------------------------------------------------------------------------------

int NetGameBase::CheckNetGameNo(NetGame* pNetGame)
{
  if(pNetGame==NULL){
    return -1;
  }
  for(int i = 0 ; i < NETGAME_NUM_MAX;i++){
    if(mpNetGameBuffer[i] == pNetGame){
      return i;
    }
  }
  return -1;
}


//---------------------------------------------------------------------------------
/**
 * @brief  コマンドの解析処理
 * @param  pRecvBuff  受信バッファ管理構造体
 * @return 解析したときのコマンド数
 */
//---------------------------------------------------------------------------------

int NetGameBase::queueSet(int netid,RecvBuffer* pRecvBuff)
{
  int i,j=0;

  while(pRecvBuff->isBuffer()){
    u8 sysCommand = pRecvBuff->readBuffer8( 0 );
    u8 gameNo = pRecvBuff->readBuffer8( 1 );
    u8 targetBit = pRecvBuff->readBuffer8( 2 );
    u8 userCommand = pRecvBuff->readBuffer8( 3 );


    if(gameNo >= NETGAME_NUM_MAX){  // インデックス違反 データを捨てる
      //OHNO_PRINT("gameNo %d\n",gameNo);
      GFL_ASSERT_MSG(0,"gameNo %d\n",gameNo);     //@fix
      break;
    }
    u16 size = pRecvBuff->readBuffer16( 2 );
    u16 timmo = pRecvBuff->readBuffer16( 4 );
    u16 index = pRecvBuff->readBuffer16Index( 4 );
    switch(sysCommand){
    case GFLNET_CONNECTION_DATA:
      pRecvBuff->dataCopy( index, reinterpret_cast<char*>(mParentCommunicationID),  (GFLNET_P2PCONNECT_MAXNUM*sizeof(nn::nex::StationID)));
      pRecvBuff->incBuffer( size + COMMAND_MINI_HEADER_SIZE );
      pRecvBuff->refleshBuffer( size + COMMAND_MINI_HEADER_SIZE );
      //OHNO_PRINT("CID0 %x  CID1 %x CID2 %x\n", mParentCommunicationID[0],
      //          mParentCommunicationID[1],
      //          mParentCommunicationID[2]);
      if(!IsMaster()){
        SetConnectionIDRecvBufferChild();
      }
      break;
    case GFLNET_START_GAME_DATA:
      pRecvBuff->incBuffer( size + COMMAND_MINI_HEADER_SIZE );
      pRecvBuff->refleshBuffer( size + COMMAND_MINI_HEADER_SIZE );
      m_isEnableStartGame = true;
    break;
    default:
      GFL_ASSERT_MSG(0,"sysCommand %d\n",sysCommand);  //現状存在しないコマンド  //@fix
      // break throw;

    case GFLNET_NORMAL_DATA:

      pRecvBuff->incBuffer( size + COMMAND_MINI_HEADER_SIZE );

//      //OHNO_PRINT("N受信 %d %d %d %d %d %d\n",netid , sysCommand, gameNo, targetBit, userCommand, size);

     if(mpNetGameBuffer[gameNo]!=NULL){
        mpNetGameBuffer[gameNo]->PushHeader(netid , sysCommand, gameNo, targetBit, userCommand, size, index);
      }
      pRecvBuff->refleshBuffer( size + COMMAND_MINI_HEADER_SIZE );
      break;
    case GFLNET_TIMEST_DATA:
      NET_TIMING("GFLNET_TIMEST_DATA gameid %d size %d timno %d netid=%d targetBit %x \n", gameNo, size, timmo, netid, targetBit);
      pRecvBuff->incBuffer( size + COMMAND_MINI_HEADER_SIZE );

      if(mpNetGameBuffer[gameNo]!=NULL){
        mpNetGameBuffer[gameNo]->PushHeader(netid ,sysCommand, gameNo, targetBit, userCommand, size, index);
      }
      pRecvBuff->refleshBuffer( size + COMMAND_MINI_HEADER_SIZE );
      break;
    case GFLNET_TIMEED_DATA:
      NET_TIMING("GFLNET_TIMEED_DATA gameid %d size %d timno %d netid=%d\n", gameNo, size, timmo, netid );

      pRecvBuff->incBuffer( size + COMMAND_MINI_HEADER_SIZE );
      if(mpNetGameBuffer[gameNo]!=NULL){
        mpNetGameBuffer[gameNo]->PushHeader(netid ,sysCommand, gameNo, targetBit, userCommand, size, index);
      }
      pRecvBuff->refleshBuffer( size + COMMAND_MINI_HEADER_SIZE );
      break;

    }
    j++;
  }
  return j;
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
bool NetGameBase::SendData(NetGame *pNetGame ,u8 sysCommand,u8 targetBit,u8 userCommand, char* pSendData, int size)
{
  u8* pPnt = reinterpret_cast<u8*>(mSend);
  pPnt = pPnt + (mSendBufferIndex + COMMAND_ALL_HEADER_SIZE);
  u16* pBuff = reinterpret_cast<u16*>(pPnt);
  u32* pBuff32 = reinterpret_cast<u32*>(pPnt);

  if(ParentCommunicationIDNum()!=mMax){
    return false;
  }
  if((mSendBufferIndex + size + COMMAND_MINI_HEADER_SIZE) >= GFLNET_P2PCONNECT_MAXPACKET){
    //送信バッファが足りないので一旦失敗を返す
    //このワーニングは出ていても特に問題は無い
    GFL_PRINT("NetGameBase::SendData : warning!! MAXPACKETOVER!![%d][%d][%d][%d]\n",mSendBufferIndex,size,COMMAND_MINI_HEADER_SIZE,GFLNET_P2PCONNECT_MAXPACKET);
    return false;
  }
  else if((!mpInitParam->IsIr())&&(mSendFlg)){  //送信状態の場合には送れません
    return false;
  }
  else if(mSendTimer!=0){
    return false;
  }
  else if(mMigrationStopFlg){
    return false;
  }


  pPnt[0] = sysCommand;
  pPnt[1] = getNetGameNo(pNetGame);
  pPnt[2] = targetBit;
  pPnt[3] = userCommand;

  pBuff[2] = size;  //サイズ

  
  gfl2::std::MemCopy( pSendData, &pBuff[4], size);  //データ移動

  mSendBufferIndex += size + COMMAND_MINI_HEADER_SIZE;
  NET_PRINT("SendData %d %d  %d %d\n",sysCommand, size, targetBit ,userCommand);
  OHNO_PRINT("SendData %d %d  %d %d\n",sysCommand, size, targetBit ,userCommand);
  return true;
}



//---------------------------------------------------------------------------------
/**
 * @brief  送信をまとめた関数
 * @param
 * @param  pData   送るデータ
 * @param  size    送信サイズ
 */
//---------------------------------------------------------------------------------


bool NetGameBase::SendDataStream(const qByte* pData,qUnsignedInt32 size)
{
  bool bret = true;
  int i;

  nn::nex::DirectStream* pDS = gflnet2::dev::NetZAdapter::GetDirectStreamInstance();
  if(pDS==NULL){  //データストリームの準備ができていない
    for( i=0;i<GFLNET_P2PCONNECT_MAXNUM;i++){
//      mSendType[i]=false;
      mSendTypeTimeoutCount[i]=1;  //準備ができてないときはタイマーは進めない
    }
    mSendFailedFlg=true;
    return false;
  }



  if(mSendFailedFlg){
    mSendFailedFlg=false;
    for( i=0;i<GFLNET_P2PCONNECT_MAXNUM;i++){
      if(RecvBufferIndex2ConnectionID( i )==0){
        continue;
      }
      if(RecvBufferIndex2ConnectionID( i ) == mMyConnectionID){
        continue;
      }
      if(RecvBufferIndex2ConnectionID( i ) == INVALID_STATION_ID){
        continue;
      }
     // if(mSendType[i]==false){ // 送信出来ていない人がいる
      if( mSendTypeTimeoutCount[i] != 0){
        
        u32 id = RecvBufferIndex2ConnectionID(i);
        nn::nex::qResult qRet = pDS->Send( id, nn::nex::DirectStream::Reliable, pData, size );

        OHNO_PRINT("send %x %x %d\n",id, (nn::nex::qBool)qRet, (int)size);

        if((nn::nex::qBool)qRet){
        //  mSendType[i]=true;
          mSendTypeTimeoutCount[i]=0;
        }
        else{
//          mSendType[i]=false;
          mSendTypeTimeoutCount[i]++;
          bret = false;
          mSendFailedFlg=true;
          //OHNO_PRINT("xx SendFail %d %d\n", i, size);
          if(mSendTypeTimeoutCount[i] > SEND_TIMEOUT_CONNECT)
          {
            if( m_errorListener )
            {
              m_errorListener->OnNetGameSessionLeft(); //切断エラーとして処理
            }
          }
        }
      }
    }
  }
  else{
    for( i=0;i<GFLNET_P2PCONNECT_MAXNUM;i++){
      if(RecvBufferIndex2ConnectionID( i )==0){
        continue;
      }
      if( RecvBufferIndex2ConnectionID( i ) == mMyConnectionID){
        continue;
      }
      if(RecvBufferIndex2ConnectionID( i ) == INVALID_STATION_ID){
        continue;
      }
      {
        u32 id = RecvBufferIndex2ConnectionID(i);
        nn::nex::qResult qRet = pDS->Send( id, nn::nex::DirectStream::Reliable, pData, size );

        OHNO_PRINT("send %x %x %d\n",id, (nn::nex::qBool)qRet, (int)size);

        if((nn::nex::qBool)qRet){
//          mSendType[i]=true;
          mSendTypeTimeoutCount[i]=0;
        }
        else{
//          mSendType[i]=false;
          mSendTypeTimeoutCount[i]=1;
          bret = false;
          mSendFailedFlg=true;



          //OHNO_PRINT("xx SendFail %d %d\n", i, size);
        }
      }
    }
  }
  return bret;
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
bool NetGameBase::dataSendMain(void)
{
  SendHead* pSend = reinterpret_cast<SendHead*>(mSend);
  u16* pBuff16 = reinterpret_cast<u16*>(mSend);
  bool ret = false;
  int i;

  

  if(mMyConnectionID!=0){
    if(mSendTimer!=0){
      mSendTimer--;
    }
  }


  if(mpInitParam->IsIr()){
    if(gflnet2::dev::IrAdapter::GetInstance()==NULL){
//      //OHNO_PRINT("赤外線NULL\n");
      return true;  //前もしくは終了
    }
    if(!gflnet2::dev::IrAdapter::GetInstance()->IsConnected()){
//      //OHNO_PRINT("赤外線接続前\n");
      return true;  //前
    }
  }
  else{
    if(NULL == gflnet2::dev::NetZAdapter::GetStationInstance()){
      //OHNO_PRINT("セッション参加前\n");
      return true;  //セッション参加前
    }
    if(mSendBufferIndex==0){  //送信サイズが０
      for(i = 0;i < GFLNET_P2PCONNECT_MAXNUM; i++){
        if(mCallbackIncFlgBit & (0x01<<i)){
          break;
        }
      }
      if(i == GFLNET_P2PCONNECT_MAXNUM){
        return true;  //送るものが何にも無い時は送信しない
      }
    }
  }

//  if(GetConnectionCount() < mGameInitialize.max  ){
  if(ParentCommunicationIDNum() < mpInitParam->GetLimitNum()  ){
    return true;  //接続前
  }


  pSend->size = mSendBufferIndex;  //サイズ
  pSend->incno = mSendNumber;    //送信番号
  pSend->netver  = GFL_P2P_NETVERSION;
  pSend->type  = GFLNET_TYPE_NORMAL;

  for(i = 0 ; i < GFLNET_P2PCONNECT_MAXNUM ; i++){
    if(mCallbackIncFlgBit & (0x01<<i)){
      pSend->RecvInc[ i ] = mCallbackInc[ i ];
  //    OHNO_PRINT("RETCOM Set! RecvInc[%d] = %d\n",i,pSend->RecvInc[ i ] );
    }
    pSend->RecvConnectionID[ i ] = RecvBufferIndex2ConnectionID(i);
  }

  gfl2::math::Crc MathCrc;
  MathCrc.Initialize();

  pSend->CRC = MathCrc.Crc16(0,(u8*)&pBuff16[1], mSendBufferIndex + (sizeof(SendHead)-2) );
  //最新版で可能

  if(mpInitParam->IsIr()){
    if(mpP2pIrLoopProc){
      ret = mpP2pIrLoopProc->SendData(reinterpret_cast<nn::nex::qByte*>(mSend), mSendBufferIndex + sizeof(SendHead));
    }
  }
  else{
    ret = SendDataStream(reinterpret_cast<nn::nex::qByte*>(mSend), mSendBufferIndex + sizeof(SendHead));
    if(!ret){
      mMigrationStopFlg = true;
    }
  }
  if(!ret){
    OHNO_PRINT("送信できなかったsize %d inc %d \n",pSend->size, pSend->incno);
    return ret;   ///9/27改造
  }
//  OHNO_PRINT("送信した size %d inc %d %d\n", pSend->size, pSend->incno,mSendBufferIndex + sizeof(SendHead));

#if GFL_DEBUG
  if(mSendBufferIndex){
    memcpy(mpDebugBuff[DebugBuffCount],reinterpret_cast<nn::nex::qByte*>(mSend),DEBUGCOMMNAD_BUFF);
    DebugBuffCount++;
    if(DebugBuffCount >= DEBUGCOMMNAD_NUM){
      DebugBuffCount = 0;
    }
  }
#endif

  mCallbackIncFlgBit = 0;

  int idx = mSendBufferIndex;
  mSendBufferIndex = 0;
  mMySendInc = pSend->incno;
  if(pSend->size){
    mSendFlg = 1;   //レスポンスでない場合には送信したフラグをON
  }
  mMigrationStopFlg=false;

  OHNO_PRINT("送信 mSendNumber=%x size %d   mMySendInc= %x\n", mSendNumber , pSend->size, mMySendInc);

  onUserReceiveUpdated(mMyConnectionID,
                       reinterpret_cast<nn::nex::qByte*>(mSend), idx + sizeof(SendHead));
  return ret;
}



// n1589:受信コールバックは処理時間が長すぎると受信が安定しなくなる可能性がある
//  とサンプルに書かれていた
// 受信コールバック
// 送られた物を最低限チェックしてダイレクトに格納している
void NetGameBase::onUserReceiveUpdated(nn::nex::StationID sourceId, qByte* buffer, qUnsignedInt32 size)
{
  OHNO_PRINT("onUserReceiveUpdated %x %d\n",sourceId, size);

  nn::nex::StationID myid = 0;
  int i;
  int id = ConnectionID2RecvBufferIndex(sourceId);

  if(id == -1){
    NN_LOG("受信準備無し\n");
    return;
  }

  if(mpInitParam->IsIr()){
    myid = gflnet2::dev::IrAdapter::GetInstance()->IsMaster();
  }
  else {
    nn::nex::Station *pStasion = gflnet2::dev::NetZAdapter::GetStationInstance();
    if(pStasion==NULL){
      return;
    }
    myid = pStasion->GetStationID();
  }
  SendHead* pSH = reinterpret_cast<SendHead*>(buffer);
  u16* pBuff16 = reinterpret_cast<u16*>(buffer);

  gfl2::math::Crc MathCrc;
  MathCrc.Initialize();

  u16 crc = MathCrc.Crc16(0,(u8*)&pBuff16[1] , size - 2 );
  if( pSH->CRC != crc ){ //CRC
    //OHNO_PRINT("CRCERR %d size %d (%x %x) %d\n", id, size, pSH->CRC , crc,sourceId);
    GFL_ASSERT(0);  //ＣＲＣエラー//@fix
    return;
  }
  GFL_ASSERT(id < GFLNET_P2PCONNECT_MAXNUM);//@fix
  if(id >= GFLNET_P2PCONNECT_MAXNUM){
    //OHNO_PRINT("notid %d\n", id);
    GFL_ASSERT(0);  //NOTID//@fix
    return;
  }
  else{
    if(pSH->type == GFLNET_TYPE_VCT){
      //リスナーが無ければVCTは受け取らない
      return;
    }
  }
  if(pSH->type != GFLNET_TYPE_NORMAL){
    //OHNO_PRINT("nottype   %d\n",  pSH->type);
    GFL_ASSERT(0);  //NOTTYPE//@fix
    return;
  }

  /// データがある場合受信扱い レスポンスを返すのに使用
  if( pSH->size != 0 ){
//    OHNO_PRINT("受信扱いNetID %d INC(RecvInc) %x\n", id, pSH->incno%256);
    mCallbackInc[id] = (u8)pSH->incno;
    mCallbackIncFlgBit |= (0x01 << id);
  }
  {
    // //マイグレーションの時に使う為に別途格納
    mMigrationCallbackInc[id] = (u8)pSH->incno;
//    OHNO_PRINT("responce NetID %d INC(RecvInc) %x\n", id, pSH->incno%256);
  }

  
  /// 通信の戻りを検査している---
  for(i = 0;i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if((myid&0xff) == pSH->RecvConnectionID[i]){
      OHNO_PRINT("RETCOM Recv! RecvInc[%d]:%d myid:%x index %d\n",id , pSH->RecvInc[ i ], myid, i);
      mRecvInc[ id ] =  pSH->RecvInc[ i ];
      mRecvIncFlgBit |= (0x01<<i);
      break;  //自分のがあることを検査したら抜ける
    }
  }
  /// レスポンスだけの時があるのでその場合は抜ける
  if(pSH->size==0){
    return;
  }

  OHNO_PRINT("受信 %d %d %d\n",size,id, (u8)pSH->incno);
  if( mRecv[id] )
  {
    mRecv[id]->pushBuffer( reinterpret_cast<char*>(&buffer[sizeof(SendHead)]), pSH->size);
  }
}


void NetGameBase::RecvUpdate(void)
{
  mRecvData.clear();
  if(nn::nex::NetZ::GetInstance()){
    //if(GFL_SINGLETON_INSTANCE(gflnet2::err::ErrManager) == NULL || !GFL_SINGLETON_INSTANCE(gflnet2::err::ErrManager)->IsError()){
      nn::nex::DirectStream* pDS = gflnet2::dev::NetZAdapter::GetDirectStreamInstance();
      if(pDS){
        pDS->GetReceivedData(mRecvData);

        for(nn::nex::qVector<nn::nex::DirectStream::Data>::iterator it = mRecvData.begin();
            it != mRecvData.end(); ++it){
  //      //OHNO_PRINT("読み込みは発生している %d %d\n",it->id,it->buffer.GetSize());
          onUserReceiveUpdated(it->id, it->buffer.Get(), it->buffer.GetSize());
        }
      }
    //}
  }
}




void NetGameBase::GameStationEventCallbackIr(int bAdd)
{

  switch(bAdd){
  case 1:

    if(IsMaster()){
      if(mMyConnectionID == 0){
        mMyConnectionID = 1;
        mParentCommunicationID[0]=1;
        mParentCommunicationID[1]=2;
        mJoinMember[0]=1;
        mJoinMember[1]=2;
        SetConnectionIDRecvBuffer(1);
        SetConnectionIDRecvBuffer(2);
        mSendTimer = 1;//WAIT_TIME_CONNECT;
      }
    }
    else{
      if(mMyConnectionID == 0){
        mMyConnectionID = 2;
        mParentCommunicationID[0]=1;
        mParentCommunicationID[1]=2;
        mJoinMember[0]=1;
        mJoinMember[1]=2;
        SetConnectionIDRecvBuffer(1);
        SetConnectionIDRecvBuffer(2);
        mSendTimer = 1;//WAIT_TIME_CONNECT;
      }
    }
    break;
  }
  //OHNO_PRINT("GameStationEventCallbackIr mMyConnectionID%d\n",mMyConnectionID);

}

//-----------------------------------------------------------------------------
// ステーション参加、離脱の処理開始時のコールバックGetStationID
//-----------------------------------------------------------------------------
void NetGameBase::GameOperationStartCallback(nn::nex::StationOperation * pOperation)
{
  const nn::nex::Station* pStation = pOperation->GetTargetStation();

  if(pStation){
    nn::nex::StationID id = pStation->GetStationID();
    //OHNO_PRINT("GetStationID id%d\n",id);
    switch (pOperation->GetType())  {
    case nn::nex::Operation::JoinStation:
      //OHNO_PRINT("JoinStation\n");
      break;
    }
  }
}



//-----------------------------------------------------------------------------
// ステーション参加、離脱の処理開始時のコールバック
//-----------------------------------------------------------------------------
void NetGameBase::GameOperationEndCallback(nn::nex::StationOperation * pOperation)
{
  const nn::nex::Station* pStation = pOperation->GetTargetStation();

  ICHI_PRINT("NetGameBase::GameOperationEndCallback!\n");
  OHNO_PRINT("NetGameBase::GameOperationEndCallback!\n");

  if(pStation==NULL){
    ICHI_PRINT("NetGameBase::GameOperationEndCallback Session=NULL\n");
    OHNO_PRINT("NetGameBase::GameOperationEndCallback Session=NULL\n");
    return;
  }
  nn::nex::StationID id = pStation->GetStationID();

  
  switch (pOperation->GetType())  {
  case nn::nex::Operation::LeaveStation:
    OHNO_PRINT("---LeaveStation---\n");
    if( mpInitParam->GetEnableHostMigration() )
    {//マイグレーション有効
      if( mMyConnectionID == id || m_migrationListener == NULL )
      {//自分が切断 or マイグレーション期間中ではない
        GFL_PRINT("case nn::nex::Operation::LeaveStation  disconnect mine id=%x\n",id);
        m_errorListener->OnNetGameSessionLeft(); //切断エラーとして処理
        ResetConnectionIDRecvBuffer(id);
      }
      else
      {//自分以外が切断
        for( u32 index = 0 ; index < GFLNET_P2PCONNECT_MAXNUM ; ++index )
        {
          if( mDisconnectCommunicationID[index] == 0 )
          {
            mDisconnectCommunicationID[index] = id;
            break;
          }
        }
	      OHNO_PRINT("プレイヤーの切断を通知 %x \n",id);
        m_migrationListener->OnNetGameDisconnectPlayer();//プレイヤーの切断を通知（マイグレーション有効なのでエラーにしない）
      }
    }
    else
    {
      if( m_errorListener )
      {
        m_errorListener->OnNetGameSessionLeft(); //切断エラーとして処理
      }
      ICHI_PRINT("+++LeaveStation+++  %x \n",id);
      OHNO_PRINT("+++切断エラーとして処理LeaveStation+++  %x \n",id);
      ResetConnectionIDRecvBuffer(id);
    }
    break;
  case nn::nex::Operation::JoinStation:
    {
      nn::nex::Station* pLocalStation = nn::nex::Station::GetLocalInstance();
      if(pLocalStation){
        //OHNO_PRINT("LocalStation--- %x\n", pLocalStation->GetStationID());
        if(mMyConnectionID == 0){
          nn::nex::StationID myid = pLocalStation->GetStationID();
          if(myid){
            mMyConnectionID = myid;
            //OHNO_PRINT("+++MYIDとうろく  %x \n",mMyConnectionID);
            ICHI_PRINT("+++MYIDとうろく  %x \n",mMyConnectionID);
            mSendTimer = WAIT_TIME_CONNECT;
          }
        }
      }
    }
    ICHI_PRINT("+++JoinStation追加+++  %x \n",id);
    DirectSetConnectionIDRecvBuffer(id );
    break;
  default:
    break;
  }
}


//---------------------------------------------------------------------------------
/**
 * @brief  接続前に呼ぶ関数  コールバック関数を設定している
 */
//---------------------------------------------------------------------------------

void NetGameBase::ConnectSetting(void)
{
  if(mpInitParam->IsWifi()){
//    nn::nex::DirectTransport::GetInstance()->RegisterUserReceiveCallback(onUserReceiveUpdated);
    mSetCallbackFlag=true;
  }
  else if(mpInitParam->IsIr()){
  }
  else{

    nn::nex::Station::RegisterOperationBeginCallback(GameOperationStartCallback);

    nn::nex::Station::RegisterOperationEndCallback( GameOperationEndCallback );
//    nn::nex::DirectTransport::GetInstance()->RegisterUserReceiveCallback(onUserReceiveUpdated);
    mSetCallbackFlag=true;
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  接続前に呼ぶ関数  コールバック関数を設定している
 */
//---------------------------------------------------------------------------------

void NetGameBase::ConnectSettingWifi(void)
{
  nn::nex::Station::RegisterOperationEndCallback( GameOperationEndCallback );
}




//---------------------------------------------------------------------------------
/**
 * @brief  親機からコネクションを送信 内部用
 * @return bool  送る準備ができたらtrue だめならfalse
 */
//---------------------------------------------------------------------------------
bool NetGameBase::SendConnectionID(void)
{
  // 赤外線は自動接続なので、固定で決まっている
  if( mpInitParam->IsIr() )
  {
    return true;
  }

  if(!mbConnectionSend){
    return false;
  }

  //OHNO_PRINT("- - CID0 %x  CID1 %x CID2 %x %d\n", mParentCommunicationID[0],mParentCommunicationID[1],mParentCommunicationID[2],mbConnectionSend);

  return SendData(NULL ,GFLNET_CONNECTION_DATA, 0, 0, reinterpret_cast<char*>(mParentCommunicationID), (GFLNET_P2PCONNECT_MAXNUM * sizeof(nn::nex::StationID)));
}


//---------------------------------------------------------------------------------
/**
 * @brief  親機からの開始号令確認
 * @return bool  準備ができたらtrue 
 */
//---------------------------------------------------------------------------------
bool NetGameBase::IsEnableStartGame(void)
{
  // 赤外線は自動接続なので、固定で決まっている
  if( mpInitParam->IsIr() )
  {
    return true;
  }

  if( IsMaster() )
  {//データはダミー
    return SendData(NULL ,GFLNET_START_GAME_DATA, 0, 0, reinterpret_cast<char*>(mParentCommunicationID), (GFLNET_P2PCONNECT_MAXNUM * sizeof(nn::nex::StationID)));
  }
  else
  {
    return m_isEnableStartGame;
  }
}



//---------------------------------------------------------------------------------
/**
 * @brief  切断に呼ぶ関数  コールバック関数をリセット
 */
//---------------------------------------------------------------------------------

void NetGameBase::DisConnect(void)
{
  nn::nex::Session* pSession;
  
  if(mpInitParam->IsWifi()){
    pSession = nn::nex::Session::GetInstance();
    if(pSession){
      pSession->UnregisterStationEventCallback();
    }
  }
  else if(!mpInitParam->IsIr() && mSetCallbackFlag){
    mSetCallbackFlag=false;
    pSession = nn::nex::Session::GetInstance();
    if(pSession){
      pSession->UnregisterStationEventCallback();
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  inc番号が帰ってきたかを検査
 */
//---------------------------------------------------------------------------------

void NetGameBase::sendNumberCheck(void)
{
  bool flg = false;
  bool isConnectionExixts = false;
  int i;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM ; i++){
    if( RecvBufferIndex2ConnectionID( i ) != mMyConnectionID ){
      if( RecvBufferIndex2ConnectionID( i ) != 0 ){
        if( RecvBufferIndex2ConnectionID( i ) != INVALID_STATION_ID ){
          isConnectionExixts = true;//生きている接続が少なくとも１つはある
          if(mRecvInc[i] == (0xff & mSendNumber)){
            flg = true;  //一人以上の検査が通った
          }
          else{
            return;
          }
        }
      }
    }
  }

  if( flg || isConnectionExixts == false )//自分一人しかいない場合も完了として進行する
  {
    //OHNO_PRINT("全部からinc番号が帰ってきて一致した %d\n",mSendNumber);
    if(mSendFlg==1){//ログが出過ぎるので条件を絞る
      NET_TIMING("mSendFlag 送信完了 inc = %d \n", mSendNumber%256);
    }
    mSendFlg = 0;  //全部からinc番号が帰ってきて一致した
    mSendNumber++;
    mRecvIncFlgBit=0;
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  動作関数
 */
//---------------------------------------------------------------------------------

void NetGameBase::Func(void)
{
  u8 data;

  if(!mpInitParam->IsIr()){
    RecvUpdate();
  }
  if(mpInitParam->IsWifi()){
//    if(GFL_SINGLETON_INSTANCE(gflnet2::GlobalAccess)->IsConnectWifi()){
//      GameStationEventCallbackWifi(0);  //手動追加
///    }
  }
  else if(mpInitParam->IsIr()){
    int i;
    if(mpP2pIrLoopProc){
      u32 recvsize = mpP2pIrLoopProc->RecvData(mIrTempBuff,IR_TEMP_BUFF_MAX);
      if(recvsize!=0){
     //   //OHNO_PRINT("recv %d %d\n",recvsize,mMyConnectionID);
    //    //OHNO_PRINT("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n",
   //                pRecv[0],pRecv[1],pRecv[2],pRecv[3],pRecv[4],pRecv[5],pRecv[6],pRecv[7],
  //                 pRecv[8],pRecv[9],pRecv[0xa],pRecv[0xb],pRecv[0xc],pRecv[0xd],pRecv[0xe],pRecv[0xf]);
        int j=1;
        if(mMyConnectionID==1){
          j=2;
        }
        onUserReceiveUpdated(j,
                             reinterpret_cast<nn::nex::qByte*>(mIrTempBuff),
                             recvsize );
      }
    }
  }




  //受信処理
  for(int id = 0;id < mpInitParam->GetLimitNum() ; id++){
    if( mRecv[id] )
    {
      queueSet(id, mRecv[id]);
    }
  }

  for(int x = 0; x < NETGAME_NUM_MAX; x++){
    NetGame* pNetGame = mpNetGameBuffer[ x ];
    if(pNetGame && mRecv[0]){
      pNetGame->Func( mRecv );  //受信したものを裁く処理
    }
  }
  sendNumberCheck();

  //Main送信処理
  dataSendMain();

}


//---------------------------------------------------------------------------------
/**
 * @brief  データコピー
 * @param  id        送ってきた人の通信ID
 * @param  RecvData  受け取るバッファ
 * @param  size      受信バッファサイズ これ以上は受け取りません
 */
//---------------------------------------------------------------------------------
void NetGameBase::DataCopy(int id, int index, char* pRecvData, int size)
{
  if( mRecv[id] )
  {
    mRecv[id]->dataCopy(index, pRecvData, size);
  }
}


//---------------------------------------------------------------------------------
/**
 * @brief  ビーコンのデータを送る（ライブラリにセットする）
 * @param  pData       データ
 * @param  size        サイズ
 * @return  成功したら true
 */
//---------------------------------------------------------------------------------
bool NetGameBase::SetDataToBeacon(void* pData, int size)
{
  if(mpInitParam->IsWifi()){
  }
  else if(!mpInitParam->IsIr()){
    return gflnet2::dev::PiaAdapter::GetInstance()->SetDataToBeacon(reinterpret_cast<const char *>(pData),size);
  }
  return false;
}


void NetGameBase::TimingStart(int gameid)
{
  if(mpNetGameBuffer[0]){
    mpNetGameBuffer[0]->TimingStart(gameid);
    NET_TIMING("NetGameBase::TimingStart %d\n",gameid);
  }
}

bool NetGameBase::IsTimingEnd(int gameid)
{
  if(mpNetGameBuffer[0]){
    return mpNetGameBuffer[0]->IsTimingEnd(gameid);
  }
  return false;
}


int NetGameBase::GetNetID(void)
{
  if(mMyConnectionID!=0){
    int i;
    for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
      if(mMyConnectionID == mParentCommunicationID[i]){
        return i;
      }
    }
  }
  return INVALID_NET_ID;
}

bool NetGameBase::IsDisconnectNetID(int netID )
{
  for(int i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++)
  {
    if(mDisconnectCommunicationID[i] == mParentCommunicationID[netID])
    {
      return true;
    }
  }
  return false;
}


int NetGameBase::SetNetID(nn::nex::StationID id)
{
  int i=0;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if(mParentCommunicationID[i]==0){
      mParentCommunicationID[i] = id;
      //OHNO_PRINT(" mParentCommunicationID %d はID %x\n",i,id);
      break;
    }
  }
  if((i+1) == mMax){
    if(IsMaster()){
      //OHNO_PRINT(" mbConnectionSend %d \n",mMax);
      mbConnectionSend=true;
    }
  }
  return i;
}


//---------------------------------------------------------------------------------
/**
 * @brief  親機のindexから受信バッファindexを得る
 * @param  index
 * @return index
 */
//---------------------------------------------------------------------------------
int NetGameBase::ParentCommunicationIDNum(void)
{
  int i,k=0;

  for(i = 0; i < GFLNET_P2PCONNECT_MAXNUM; i++){
    if(mParentCommunicationID[i] && mJoinMember[i]){
      k++;
    }
  }
  return k;
}


//------------------------------------------------------------------
/**
 * @brief   エラーリスナー登録
 * @param   listener   通知を受け取るNetGameErrorListener
 */
//------------------------------------------------------------------
void NetGameBase::RegistErrorListener( NetGameErrorListener* listener )
{
  GFL_PRINT("NetGameBase::RegistErrorListener\n");
  m_errorListener = listener;
}

//------------------------------------------------------------------
/**
 * @brief   エラーリスナー解除
 */
//------------------------------------------------------------------
void NetGameBase::UnregistErrorListener( void )
{
  GFL_PRINT("NetGameBase::UnregistErrorListener\n");
  m_errorListener = NULL;
}

//------------------------------------------------------------------
/**
  * @brief   マイグレーション用リスナー登録
  * @param   listener   通知を受け取るNetGameMigrationListener
  */
//------------------------------------------------------------------
void NetGameBase::RegistMigrationListener( NetGameMigrationListener* listener )
{
  m_migrationListener = listener;
}

//------------------------------------------------------------------
/**
  * @brief   マイグレーション用リスナー解除
  */
//------------------------------------------------------------------
void NetGameBase::UnregistMigrationListener( void )
{
  m_migrationListener = NULL;
}

//------------------------------------------------------------------
/**
  * @brief   マイグレーション有効かどうか（リスナー設定されていたら有効）
  */
//------------------------------------------------------------------
bool NetGameBase::IsMigrationEnable( void )
{
  return m_migrationListener != NULL;
}

//------------------------------------------------------------------
/**
  * @brief   送信再開の為にダミーを送信する
  */
//------------------------------------------------------------------
bool NetGameBase::SetDummySend(void)
{
  for(int i=0;i<GFLNET_P2PCONNECT_MAXNUM;i++){
    OHNO_PRINT("SetDummySend mCallbackInc %x mMigrationCallbackInc %x\n",
               mCallbackInc[i], mMigrationCallbackInc[i]);
    mCallbackInc[i] = mMigrationCallbackInc[i];
  }
  mCallbackIncFlgBit=0xff;
  OHNO_PRINT("ダミー送信ON\n");
	return true;
}


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)


#endif

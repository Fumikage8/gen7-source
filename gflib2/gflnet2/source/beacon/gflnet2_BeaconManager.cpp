#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_BeaconManager.cpp
 * @brief	 ビーコンマネージャー管理クラス
 * @author k.ohno
 * @date	 2012.5.9
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <nn/uds.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>

#include <math/include/gfl2_math_crc.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


#define OLDVERSION (0)

BeaconManager::BeaconManager()
{
  mpInitParam=NULL;
  mTempDownCount=0;
  mTempSetTime = 0;
  m_tempGameID = 0;
  mpBeaconListener = NULL;
  mpBeaconSender = NULL;

  std::memset( mBeaconRecvData , 0 , sizeof(mBeaconRecvData) );
  std::memset( &mBeaconSendData , 0 , sizeof(mBeaconSendData) );
  std::memset( &mBackupBeaconData , 0 , sizeof(mBackupBeaconData) );
}

BeaconManager::~BeaconManager()
{
}





//------------------------------------------------------------------
/**
 * @brief	  ビーコンの生存を検査
 */
//------------------------------------------------------------------
void BeaconManager::Clear(void)
{
  int i;

  Beacon mBeacon(&mBeaconSendData);
  mBeacon.ResetCommand();

  for(i=0;i< BEACON_MAX_NUM ;i++){
    BeaconRecvCell* pBRD = &mBeaconRecvData[i];
    gfl2::std::MemClear(pBRD,sizeof(BeaconRecvCell));
  }
  //OHNO_PRINT("これが呼ばれたら消える\n");
}





//------------------------------------------------------------------
/**
 * @brief	  ビーコンデータを再送信
 * @param   セットできたらtrue
 */
//------------------------------------------------------------------

bool BeaconManager::ReloadBeaconData( void )
{
  bool result = true;  //mpBeaconSenderがない場合にはtrueになるように変更

  if(mpBeaconSender){
    result = mpBeaconSender->SetBeacon((void*)&mBeaconSendData, sizeof(BeaconData));
  }
  return result;
}

//------------------------------------------------------------------
/**
 * @brief	  ビーコンリスナーをセット
 * @param   pBeaconListener クラスのポインタ
 * @param   beaconType
 */
//------------------------------------------------------------------
void BeaconManager::SetBeaconListener( BeaconListener* pBeaconListener)
{
  mpBeaconListener = pBeaconListener;
}


//------------------------------------------------------------------
/**
 * @brief	  受信したビーコンをアドレスとして得る
 * @param   インデックス
 * @return  アドレス
 */
//------------------------------------------------------------------
BeaconData* BeaconManager::GetBeaconAddress(int i)
{
  BeaconRecvCell* pBRD = &mBeaconRecvData[i];

  if( pBRD->recv.ConnectAsClient){
    return &pBRD->data;
  }
  return NULL;
}



//------------------------------------------------------------------
/**
 * @brief	  受信したビーコンをアドレスとして得る
 * @param   インデックス
 * @return  アドレス
 */
//------------------------------------------------------------------
void BeaconManager::ResetBeaconAddress(int i)
{
  mBeaconRecvData[i].recv.ConnectAsClient = 0;
}



//------------------------------------------------------------------
/**
 * @brief	  受信したビーコンをアドレスとして得る
 * @param   TransferedID
 * @return  アドレス
 */
//------------------------------------------------------------------
BeaconData* BeaconManager::GetBeaconAddress(bit64 TransferedID)
{
  int i;

  for(i = 0; i < BEACON_MAX_NUM; i++){
    BeaconRecvCell* pBRD = reinterpret_cast<BeaconRecvCell*>(&mBeaconRecvData[i]);
    if(pBRD->data.header.TransferedID == TransferedID){
      return &pBRD->data;
    }
  }
  return NULL;
}



//------------------------------------------------------------------
/**
 * @brief	  受信したビーコンをクラスとして得る そのポインタ
 * @param   ビーコンクラス
 * @param   TransferedID
 * @return  あったらtrue なかったらfalse
 */
//------------------------------------------------------------------
bool BeaconManager::GetBeacon(Beacon* pBeacon,int i)
{
  if(i >= BEACON_MAX_NUM){
    return false;
  }
  if( GetBeaconAddress(i) == NULL )
  {
    return false;
  }

  pBeacon->SetBeaconData((BeaconData*)GetBeaconAddress(i));
  return true;
}


//------------------------------------------------------------------
/**
 * @brief	  受信したビーコンをクラスとして得る そのポインタ
 * @param   ビーコンクラス
 * @param   TransferedID
 * @return  あったらtrue なかったらfalse
 */
//------------------------------------------------------------------
bool BeaconManager::GetBeacon(Beacon* pBeacon, bit64 TransferedID)
{
  void* pData = GetBeaconAddress(TransferedID);

  if(pData){
    pBeacon->SetBeaconData((BeaconData*)pData);
    return true;
  }
  return false;
}




bool BeaconManager::SetRecvBeaconDataInternal(int i,const BeaconData* pBeaconData, BeaconRecvData* pRecv)
{
  BeaconRecvCell* pBRD =  reinterpret_cast<BeaconRecvCell*>(&mBeaconRecvData[i]);


  GFL_ASSERT(i >= 0);

  gfl2::std::MemCopy(pRecv, &pBRD->recv, sizeof(BeaconRecvData));

  pBRD->recv.ConnectAsClient += 1;
  pBRD->recv.DownCount = BEACON_DOWN_TIMER;

  int commandnum = pBeaconData->header.commandnum;

  if(commandnum == pBRD->data.header.commandnum){
    if(gfl2::std::MemComp(&pBRD->data , pBeaconData , sizeof(BeaconData))==0){
      return false;  //完全一致
    }
  }
  gfl2::std::MemClear(&pBRD->data,  sizeof(BeaconData));
  gfl2::std::MemCopy(pBeaconData, &pBRD->data,  sizeof(BeaconData));
  pBRD->NewFlg=1;  //橋渡しのフラグ

  return true;  //うわがいた
}

//------------------------------------------------------------------
/**
 * @brief	  ライブラリが拾ったビーコンをここに入れる
 * @param   データ
 * @param   サイズ
 */
//------------------------------------------------------------------
void BeaconManager::SetRecvBeaconData(const void* pRecvBeacon, size_t size, u8 nowEntry, u8 ConnectAsClient, bit64 lfc)
{
  BeaconRecvData beaconRecv;

  beaconRecv.lfc = lfc;
  beaconRecv.ConnectAsClient = ConnectAsClient;  ///< 接続が許可されているかどうか検査 許可＝２ 不許可＝１ データ無し＝０
  beaconRecv.nowEntry = nowEntry;         ///< 接続人数

  SetBeaconData((BeaconData*) pRecvBeacon, &beaconRecv);
}

//------------------------------------------------------------------
/**
 * @brief	  ライブラリが拾ったビーコンをここに入れる
 * @param   BeaconData構造体
 */
//------------------------------------------------------------------
void BeaconManager::SetBeaconData(BeaconData* pBeaconData, BeaconRecvData* pBeaconRecv)
{
  int i;
  const BeaconData* pBD = pBeaconData;
  u8* pData=(u8*)pBeaconData;
  
  gfl2::math::Crc MathCrc;
  MathCrc.Initialize();

  u16 crc = MathCrc.Crc16(0,&pData[2], sizeof(BeaconData) - 2);

  if(mpInitParam==NULL){
    return;
  }

  if( pBD->header.GameID != reinterpret_cast<u8>(mpInitParam->GetGameID()) &&
      pBD->header.GameID != m_tempGameID )
  {
//    //OHNO_PRINT("GAMEIDちがい違い %d %d\n",pBD->header.GameID,mpInitParam->GetGameID());
    return;
  }
  u8 v = pBD->header.GFLNETVersionAndProductOrDevelopment & GFLNET_VERSION_MASK;
  if(v !=  GFL_BEACON_NETVERSION){
//    //OHNO_PRINT("バージョン違い %d %d\n",v , GFL_BEACON_NETVERSION);
    return;
  }
  if(crc != pBD->header.CRC){
//    //OHNO_PRINT("CRC一致せず %x  recv %x %d\n",crc,pBD->header.CRC, sizeof(BeaconData) - 2);
    return;
  }
  int max = BEACON_MAX_NUM;
  for(i = 0;i < max; i++){
    if((mBeaconRecvData[i].recv.ConnectAsClient!=0 ) &&  (mBeaconRecvData[i].data.header.TransferedID == pBD->header.TransferedID))
    {
      if( ( pBD->header.connectNum != TEMP_BEACON_CONNECT_NUM && mBeaconRecvData[i].data.header.connectNum != TEMP_BEACON_CONNECT_NUM ) ||
          ( pBD->header.connectNum == TEMP_BEACON_CONNECT_NUM && mBeaconRecvData[i].data.header.connectNum == TEMP_BEACON_CONNECT_NUM )
          )
      {
       SetRecvBeaconDataInternal(i, pBeaconData, pBeaconRecv);
      return;
      }
    }
  }
  for(i = 0;i < max; i++){
    if(mBeaconRecvData[i].recv.ConnectAsClient==0)
    {
       SetRecvBeaconDataInternal(i, pBeaconData, pBeaconRecv);
      return;
    }
  }
  //最初のビーコンを消して最後にビーコンを追加
  for(i = 0;i < max - 1 ;i++){
    memcpy(&mBeaconRecvData[i], &mBeaconRecvData[i+1], sizeof(BeaconRecvCell));
  }
  i = max - 1;
  SetRecvBeaconDataInternal(i, pBeaconData, pBeaconRecv);
}



//------------------------------------------------------------------
/**
 * @brief	  レスポンダーに渡すビーコンをセット
 * @param   BeaconData構造体
 */
//------------------------------------------------------------------
void BeaconManager::onBeaconData(void)
{
  int i;

  if(mpInitParam==NULL){
    return;
  }

  int max = BEACON_MAX_NUM;

  for(i = 0;i < max; i++){
    if(mBeaconRecvData[i].NewFlg==1){
      if(mpBeaconListener){
        mpBeaconListener->OnUpdateBeacon(mBeaconRecvData[i].data.body,
                                         mBeaconRecvData[i].recv.lfc,
                                         mBeaconRecvData[i].data.header.TransferedID);
      }
      mBeaconRecvData[i].NewFlg=0;
    }
  }
}




//------------------------------------------------------------------
/**
 * @brief	  ビーコンの生存を検査
 */
//------------------------------------------------------------------
void BeaconManager::MainCheckRecvBeacon(void)
{
  int i,max;

  if(mpInitParam==NULL){
    return;
  }
  TemporaryBeaconUpdate();

  max = BEACON_MAX_NUM;

  BeaconListener* pBeacon;
  for(i=0;i< max ;i++){
    BeaconRecvCell* pBRD = &mBeaconRecvData[i];

    if(pBRD->recv.ConnectAsClient){

      if(pBRD->recv.DownCount != 0){
        pBRD->recv.DownCount--;
      }
      if(pBRD->recv.DownCount == 0){

        {

          pBRD->recv.ConnectAsClient = 0;
          //OHNO_PRINT("消去 %llu  %x\n",pBRD->data.header.TransferedID, &pBRD->data.header.TransferedID);
#if GFL_DEBUG
          //消したIDと同じ人がいないかどうか検査
          for(int x=0; x < max ; x++){
            if(mBeaconRecvData[x].recv.ConnectAsClient!=0){
              if( mBeaconRecvData[x].data.header.TransferedID == pBRD->data.header.TransferedID &&
                  mBeaconRecvData[x].data.header.connectNum == pBRD->data.header.connectNum
                  )
              {
                GFL_ASSERT(0);  //@fix
              }
            }
          }
#endif
          pBeacon = mpBeaconListener;
          if(pBeacon){
            pBeacon->OnDeleteBeacon(pBRD->data.body, pBRD->recv.lfc,pBRD->data.header.TransferedID);
          }
          gfl2::std::MemClear(pBRD,sizeof(BeaconRecvCell));
        }
      }
    }
  }
  onBeaconData();

}


//------------------------------------------------------------------
/**
 * @brief	  送信するビーコンデータを取得
 * @param   送信ビーコン
 */
//------------------------------------------------------------------
void* BeaconManager::GetSendBeaconData( void )
{
  return (void*)&mBeaconSendData;
}

//------------------------------------------------------------------
/**
 * @brief	  送信するビーコンデータのサイズを取得
 * @param   送信ビーコンサイズ
 */
//------------------------------------------------------------------
size_t BeaconManager::GetSendBeaconDataSize( void )
{
  return sizeof(BeaconData);
}


//------------------------------------------------------------------
/**
 * @brief	  ビーコンをセット
 * @param   sudId PIAライブラリの差異を今はここで吸収
 */
//------------------------------------------------------------------

void BeaconManager::SetRecvBeaconData(u8 sudId,const void* pRecvBeacon, size_t size, u8 nowEntry, u8 ConnectAsClient, bit64 lfc)
{
  //PIAがなくなればいらなくなるはず
  switch(sudId){
  case BEACONTYPE_MATCH:
    SetRecvBeaconData(pRecvBeacon, sizeof(gflnet2::base::BeaconData),
                      nowEntry, ConnectAsClient,lfc);
    break;
  case BEACONTYPE_EACH:  //一致するのはビーコン送受信用
    SetRecvBeaconData(pRecvBeacon,  sizeof(gflnet2::base::BeaconData),
                      nowEntry, ConnectAsClient,lfc);
    break;
  }
}



//------------------------------------------------------------------
/**
 * @brief	  ビーコンを追加する
 * @param   command  ビーコンコマンド
 * @param   id       ビーコンID番号
 * @param   pData    送信データ
 * @param   size     送信サイズ
 */
//------------------------------------------------------------------
void BeaconManager::AddBeaconData(u8 command ,u8 id ,const u8* pData, size_t size)
{
  Beacon mBeacon(&mBeaconSendData);
  mBeacon.AddBeaconCommand(command, id, pData, size);
  mTempDownCount = 0;//ロールバックを停止する
}


//------------------------------------------------------------------
/**
 * @brief	  呼び出しビーコンを作る
 */
//------------------------------------------------------------------
void BeaconManager::CreateBeaconData(void)
{
  if(mpInitParam==NULL){
    return;
  }
  Beacon mBeacon(&mBeaconSendData);
  mBeacon.CreateBeaconHeader( mpInitParam->GetGameID(),
                              mpInitParam->GetPMVer(),
                              mpInitParam->GetLangCode(),
                              mpInitParam->GetTransferedID(),
                              2  );
}




//------------------------------------------------------------------
/**
 * @brief	  呼び出しビーコンをセットする
 * @param   TransferedID
 * @param   接続人数
 */
//------------------------------------------------------------------
bool BeaconManager::SetBeacon(const void* pData, int max)
{
  if(mpInitParam==NULL){
    return false;
  }

  Beacon mBeacon(&mBeaconSendData);

  mBeacon.CreateBeaconHeader( mpInitParam->GetGameID(),
                              mpInitParam->GetPMVer(),
                              mpInitParam->GetLangCode(),
                              mpInitParam->GetTransferedID(),
                              2  );

  mBeacon.AddBeaconCommand(GFLNET_SYSTEM_COMMNAD, GFLNET_COMMNAD_FREE_PACKET, (const u8*)pData, max);
  mTempDownCount = 0;//ロールバックを停止する
  return ReloadBeaconData();
}





void BeaconManager::BackupBeacon(BeaconData* pBeaconData)
{
  ////OHNO_PRINT("----PUSH BackupBeacon----\n");
  memcpy( pBeaconData , &mBeaconSendData, sizeof(BeaconData));
}


void BeaconManager::RestoreBeacon(BeaconData* pBeaconData)
{
  ////OHNO_PRINT("----POP RestoreBeacon----\n");
  memcpy( &mBeaconSendData, pBeaconData ,  sizeof(BeaconData));
}



void BeaconManager::SetTemporaryBeaconData(int id,int no,const u8* pBeaconData,int size , int downMsec , u8 tempGameID )
{
  if(mpInitParam==NULL){
    return;
  }
  ////OHNO_PRINT("----PUSH SetTemporaryBeaconData----\n");
  memcpy( &mBackupBeaconData , &mBeaconSendData, sizeof(BeaconData));
  memset( &mBeaconSendData , 0 , sizeof(BeaconData) );

  Beacon mBeacon(&mBeaconSendData);

  //GFL_PRINT("gameid[%d][%d]\n",mpInitParam->GetGameID(),tempGameID);

  mBeacon.CreateBeaconHeader( tempGameID,
                              mpInitParam->GetPMVer(),
                              mpInitParam->GetLangCode(),
                              mpInitParam->GetTransferedID(),
                              TEMP_BEACON_CONNECT_NUM );

  mBeacon.AddBeaconCommand(id, no, (const u8*)pBeaconData, size);
  ReloadBeaconData();
  mTempDownCount = downMsec;
  mTempSetTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  m_tempGameID = tempGameID;
}


void BeaconManager::TemporaryBeaconUpdate(void)
{

  if( mTempDownCount == 0 )
  {
    return;
  }

  //ビーコンロールバック
  u64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();

  if( currentTime - mTempSetTime > mTempDownCount )
  {
    mTempDownCount = 0;
    memcpy(  &mBeaconSendData, &mBackupBeaconData , sizeof(BeaconData));
    ReloadBeaconData();
    //GFL_PRINT("----POP TemporaryBeacon RoleBack---- \n");
  }
}



GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)




#endif

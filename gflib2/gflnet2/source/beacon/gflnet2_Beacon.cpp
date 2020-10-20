#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_Beacon.cpp
 * @brief	 ビーコンクラス
 * @author k.ohno
 * @date	 2012.5.13
 */
//=============================================================================

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "gflnet2/include/beacon/gflnet2_Beacon.h"
#include "gflnet2/include/base/gflnet2_Base.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <util/include/gfl2_std_string.h>

#include <math/include/gfl2_math_crc.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)

#define GFL_NET_DEVEROPMENT 0

Beacon::Beacon(BeaconData* pBeaconData)
{
  mpBeaconData = pBeaconData;
}

//------------------------------------------------------------------
/**
 * @brief	  中身を集めてビーコンヘッダーを作る
 */
//------------------------------------------------------------------

void Beacon::CreateBeaconHeader(u8 GameID,u8 pmver,
                                u8 LangCode, const bit64 transferedID,
                                int connectNum )
{
  BeaconData* pBeacon = mpBeaconData;

  gfl2::std::MemClear(pBeacon, sizeof(BeaconData));

  pBeacon->header.GameID = GameID;
  pBeacon->header.PMVer = pmver;
  pBeacon->header.LangCode = LangCode;
  pBeacon->header.connectNum = connectNum;

#if GFL_NET_DEVEROPMENT  ///< 開発中のビーコンの場合
  pBeacon->header.GFLNETVersionAndProductOrDevelopment = (GFL_BEACON_NETVERSION|GFLNET_BEACON_DEVEROPMENT);
#else
  pBeacon->header.GFLNETVersionAndProductOrDevelopment = (GFL_BEACON_NETVERSION|GFLNET_BEACON_RELEASE);
#endif

  if(transferedID != 0){
    pBeacon->header.TransferedID = transferedID;
  }
  pBeacon->header.commandnum = 0;
  SetCRC();
 // //OHNO_PRINT("createが呼ばれた。");

}

//------------------------------------------------------------------
/**
 * @brief	  ボディーをダイレクトに書く
 */
//------------------------------------------------------------------

void Beacon::WriteBeaconBody(const u8* pData, const u32 sizex)
{
  u32 size = sizex;
  GFL_ASSERT(size <= BEACON_MAXBYTE); //fix
  if(size > BEACON_MAXBYTE){
    size = BEACON_MAXBYTE;
  }
  gfl2::std::MemCopy(pData, mpBeaconData->body, size);
  mpBeaconData->header.commandnum = 1;

  SetCRC();
}



u8 Beacon::GetBeaconCommandSize(BeaconData* pBeaconData)
{
  u8 num = pBeaconData->header.commandnum;
  u8 size = 0;
  
  if(pBeaconData->header.commandnum==0){
    return sizeof(BeaconHead);
  }
  BeaconCommand* pComm = (BeaconCommand* )&pBeaconData->body[0];
  for(u8 a = 0;a < num;a++){
    size += pComm->size;
    pComm = (BeaconCommand* )&pBeaconData->body[size];
  }
  return size + sizeof(BeaconHead);
}



//------------------------------------------------------------------
/**
 * @brief	  ビーコンコマンドを追加
 * @param   type          タイプ（アプリごとの番号）
 * @param   no            NO(アプリの中での使い分け)
 * @param   pData         データ
 * @param   size          サイズ
 */
//------------------------------------------------------------------
void Beacon::AddBeaconCommand( u8 type, u8 no, const u8* pData, size_t size)
{
  BeaconCommand cmd;
  u8* pDataAddr = (u8*)mpBeaconData;
  u8 nowsize = GetBeaconCommandSize(mpBeaconData);

  if( nowsize + size > sizeof(BeaconData) )
  {//サイズオーバー
    GFL_ASSERT(0);//@fix
    return;
  }

  u8* pDataStart = &pDataAddr[nowsize];

  gfl2::std::MemClear(&cmd, sizeof(BeaconCommand));
  cmd.size = size+sizeof(BeaconCommand);
  cmd.beaconType = type;
  cmd.beaconFunctionNo = no;
  gfl2::std::MemCopy(&cmd, pDataStart,  sizeof(BeaconCommand));
  gfl2::std::MemCopy(pData, &pDataStart[sizeof(BeaconCommand)],  size);


  
  mpBeaconData->header.commandnum++;
  SetCRC();
}


//------------------------------------------------------------------
/**
 * @brief	  ビーコンコマンドを全部消す
 */
//------------------------------------------------------------------
void Beacon::ResetCommand( void )
{

  gfl2::std::MemClear(mpBeaconData->body, BEACON_MAXBYTE);
  mpBeaconData->header.commandnum = 0;

  SetCRC();


}



//------------------------------------------------------------------
/**
 * @brief	  ビーコンデータをえる
 * @return  上記のもの
 */
//------------------------------------------------------------------
void* Beacon::GetBeaconAddress(void)
{
  return mpBeaconData;
}
//------------------------------------------------------------------
/**
 * @brief	  ビーコン生データボディをえる
 * @return  上記のもの
 */
//------------------------------------------------------------------
void* Beacon::GetBeaconBody(void)
{
  return mpBeaconData->body;


}



//------------------------------------------------------------------
/**
 * @brief	  CRCをえる
 */
//------------------------------------------------------------------
GFLNET_CRC_TYPE Beacon::GetCRC(void)
{
  return mpBeaconData->header.CRC;
}

//------------------------------------------------------------------
/**
 * @brief	  ビーコン生データサイズをえる
 * @return  上記のもの
 */
//------------------------------------------------------------------
size_t Beacon::GetBeaconSize(void)
{

  return Beacon::GetBeaconCommandSize(mpBeaconData);
}


//------------------------------------------------------------------
/**
 * @brief	  GameIDをえる
 */
//------------------------------------------------------------------
u8 Beacon::GetGameID(void)
{
  return mpBeaconData->header.GameID;
}

//------------------------------------------------------------------
/**
 * @brief	  外部バージョンを得る
 */
//------------------------------------------------------------------
u8 Beacon::GetPMVer(void)
{
  return mpBeaconData->header.PMVer;
}

//------------------------------------------------------------------
/**
 * @brief	  言語コードを得る
 */
//------------------------------------------------------------------
u8 Beacon::GetLangCode(void)
{
  return mpBeaconData->header.LangCode;
}

//------------------------------------------------------------------
/**
 * @brief	  接続数を得る
 */
//------------------------------------------------------------------
u8 Beacon::GetConnectNum(void)
{
  return mpBeaconData->header.connectNum;
}

//------------------------------------------------------------------
/**
 * @brief	  GFLNETバージョンを得る
 */
//------------------------------------------------------------------
u8 Beacon::GetGFLNETVersion(void)
{
  return mpBeaconData->header.GFLNETVersionAndProductOrDevelopment & GFLNET_VERSION_MASK;
}

//------------------------------------------------------------------
/**
 * @brief	  製品ビーコンかどうかを得る
 */
//------------------------------------------------------------------
bool Beacon::IsProduct(void)
{
  u8 ans = mpBeaconData->header.GFLNETVersionAndProductOrDevelopment & GFLNET_PRODEV_MASK;
  if(ans == GFLNET_BEACON_RELEASE){
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief	  TransferedIDを得る
 */
//------------------------------------------------------------------
bit64 Beacon::GetTransferedID(void)
{
  return mpBeaconData->header.TransferedID;
}

//------------------------------------------------------------------
/**
 * @brief	 CRC計算
 */
//------------------------------------------------------------------

void Beacon::SetCRC(void)
{
  gfl2::math::Crc MathCrc;
  MathCrc.Initialize();

  u16 crc = MathCrc.Crc16(0,&mpBeaconData->header.GameID, sizeof(BeaconData)-sizeof(GFLNET_CRC_TYPE));

  mpBeaconData->header.CRC = crc;
//  //OHNO_PRINT("SetCRC %x %d\n",mpBeaconData->header.CRC, sizeof(BeaconData)-sizeof(GFLNET_CRC_TYPE));
}


//------------------------------------------------------------------
/**
 * @brief	 データセット
 */
//------------------------------------------------------------------
void Beacon::SetBeaconData(BeaconData* pBeaconData)
{
  gfl2::std::MemCopy(mpBeaconData, pBeaconData, sizeof(BeaconData));
}
//------------------------------------------------------------------
/**
 * @brief	 データセット
 */
//------------------------------------------------------------------
void Beacon::ChangeBeaconData(BeaconData* pBeaconData)
{
  mpBeaconData = pBeaconData;
}

//------------------------------------------------------------------
/**
 * @brief	  呼び出されているかどうかをコマンド確認
 */
//------------------------------------------------------------------
int Beacon::CheckCallTransferedID(bit64 transferedID)
{
  u8* cbuff = (u8*)CheckBeaconCommand( GFLNET_SYSTEM_COMMNAD, GFLNET_COMMNAD_CALLID );
  bit64 buff[ 3 ];

  if(cbuff==NULL){
    return -1;
  }
  if(transferedID==0){
    return -1;
  }
  
  memcpy( buff,cbuff, sizeof(bit64)*3 );

  for(int x = 0;x < 3;x++){
    if(buff[x] == transferedID){
      return x;
    }
  }
  return -1;
}

//------------------------------------------------------------------
/**
 * @brief	  コマンドがあったらとりだす
 */
//------------------------------------------------------------------
void* Beacon::CheckBeaconCommand( u8 type, u8 no )
{
  BeaconCommandIterator mIterator(this);
  u32* pBody = NULL;
  u32 address;

  while( 1  ){
    BeaconCommand* pCommand = mIterator.GetNext(&address);
    if(pCommand==NULL){
      break;
    }
    if(pCommand->beaconType == type){
      if(pCommand->beaconFunctionNo == no){
        pBody = (u32*)address;
        break;
      }
    }
  }
  return pBody;
}




GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)
#endif

#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWirelessBeaconBinder.cpp
 * @brief  ワイヤレスビーコン結合クラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================

#include <NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessBeaconBinder.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <zorro.h>
#include "./DeliveryKeys.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)


//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
DeliveryWirelessBeaconBinder::DeliveryWirelessBeaconBinder( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam, const bool bDoVerifySign )
: m_pHeap( pHeap )
, m_lastProgress(0)			//!< 最新の進捗（%）
, m_isFinish(false)				//!< 受信終了したかどうか
, m_bDoVerifySign(bDoVerifySign)
, m_criticalSectionForBeadonData()
, m_pInitParam(pInitParam)
{
  m_criticalSectionForBeadonData.Initialize();
  std::memset( m_beaconDataBuffer , 0 , sizeof(m_beaconDataBuffer) );
  std::memset( m_uncompressBuf , 0 , sizeof(m_uncompressBuf) );
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
DeliveryWirelessBeaconBinder::~DeliveryWirelessBeaconBinder()
{
  m_criticalSectionForBeadonData.Finalize();
  m_pHeap = NULL;
}



//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
bool DeliveryWirelessBeaconBinder::Update( DeliveryRecvListener* pRecvListener )
{
  if( m_isFinish )
    return true;

  if( pRecvListener )
  {
    u32 progress = 0;		//進捗
    u32 maxProgress = 0;	//最大進捗

    {
      nn::os::CriticalSection::ScopedLock lock( m_criticalSectionForBeadonData );

      for( u32 index = 0 ; index < MAX_BEACON_SLOT_NUM ; ++index )
      {//データの受信状況をチェック
        if( m_beaconDataBuffer[index].recvNum > 0 && m_beaconDataBuffer[index].splitNum <= m_beaconDataBuffer[index].recvNum )
        {//データ完成
          bool  isFinish    = false;    /**<  受信が完了された  */
          bool  isLegalData = true;

          pRecvListener->OnUpdateProgress( COMPLETE_PROGRESS );//100%到達を通知

          /*
            {
              u32 datasize;     //  overhead not included
              u32 compressSize;
              u8  data[];
            }

            typedef struct
            {
              u16   deliveryID;		//配信データID
              s8    splitNum;		//分割数（データがいくつに分かれているか）
              s8    recvNum;			//いくつまで受信したか
              bit32 recvBit[RECV_BIT_SIZE];		//受信フラグ
              bit8  beaconSlot[MAX_SPLIT_NUM][SPLIT_BEACON_BODY_SIZE];
            } WIRELESS_BEACON_SLOT_DATA;
          */

          /*  署名確認  */
          {

            struct  BeaconBindidDataHeader
            {
              u32   dataBytes;
              u32   compressBytes;
              u8    data[1];        /*  データ先頭  */
            };

            u8*                       pReceivedData           = reinterpret_cast<u8*>(&(m_beaconDataBuffer[index].beaconSlot));   /*  受信データ先頭  */
            BeaconBindidDataHeader*   pBeaconBindedDataHeader = reinterpret_cast<BeaconBindidDataHeader*>(pReceivedData);

            /*  簡易サイズチェック  */
            {
              const u32   maxBytes            = sizeof(m_beaconDataBuffer[index].beaconSlot);
              const bool  dataBytesCheck      = (pBeaconBindedDataHeader->dataBytes     < maxBytes);
              const bool  compressBytesCheck  = (pBeaconBindedDataHeader->compressBytes < maxBytes);
              const bool  bytesCompareCheck   = (pBeaconBindedDataHeader->compressBytes <= pBeaconBindedDataHeader->dataBytes);

              isLegalData = (dataBytesCheck && compressBytesCheck && bytesCompareCheck);
            }

            /*  verify  */
            if(isLegalData && m_bDoVerifySign)
            {
              /*!
                  @brief OPSSR署名の検証

                  @param[out] recoveredMessage 復元されたメッセージへのポインタ
                  @param[inout] recoveredMessageLen 復元されたメッセージのバッファサイズ。(メッセージサイズ)-(オーバーヘッドサイズ)以下のサイズの場合、
                                             関数は失敗します。そうでなければメッセージサイズに設定されます。
                  @param[in] signature       署名へのポインタ
                  @param[in] signatureLen    署名長（バイト単位)
                  @param[in] overhead        オーバーヘッドサイズ
                  @param[in] publicData      公開鍵データへのポインタ
                  @param[in] publicDataLen   公開鍵データ長
                  @param[in] derKey          署名検証するために用いられる鍵を含むRSA公開鍵（DER形式）へのポインタ
                  @param[in] derKeyLen       RSA公開鍵データ長
                  @param[in] modulusBitLen   RSAの法パラメータの長さ（ビット単位）
                  @param[in] tempBuffer      Temporary buffer address.
                  @param[in] tempBufferLen   Temporary buffer length. Must be at least 3 times the length of the
                                             RSA modulus.
                  @return                    成功時はtrue、失敗時はfalse
               */

              const u8*   pPublicKey            = DeliveryKeys::PublicKey();
              const u32   publicKeyBytes        = DeliveryKeys::PublicKeyBytes();
              const u32   moduleBits            = DeliveryKeys::ModulesBits;
              const u32   tempbufBytes          = DeliveryKeys::TempBufBytes;
              const u32   overheadBytes         = DeliveryKeys::OverheadBytes;
              u32         recoveredBytes        = (pBeaconBindedDataHeader->compressBytes ? pBeaconBindedDataHeader->compressBytes : pBeaconBindedDataHeader->dataBytes);
              const u32   signatureBytes        = recoveredBytes + overheadBytes;
              const u8*   pSignatureData        = pBeaconBindedDataHeader->data;
              u8*         pTempSignatureBuffer  = GFL_NEW_ARRAY(m_pHeap)  u8[signatureBytes];
              bool        verifyResult;
              u8          tempBuf[tempbufBytes];

              memcpy(pTempSignatureBuffer, pSignatureData, signatureBytes);
              
              /*  recover pTempSignatureBuffer to pBeaconBindedDataHeader->data   */
              {
                
                verifyResult  = mw::zorro::OpssrCmcSha1_Verify(
                                  pBeaconBindedDataHeader->data,  recoveredBytes,
                                  pTempSignatureBuffer,           signatureBytes,
                                  overheadBytes,
                                  pPublicKey,                     publicKeyBytes,
                                  pPublicKey,                     publicKeyBytes,
                                  moduleBits,
                                  tempBuf,                        tempbufBytes
                                );
              }

              GFL_PRINT("DeliveryWirelessBeaconBinder: verify %s\n", verifyResult ? "success" : "failed");

              isLegalData = verifyResult;
              GFL_SAFE_DELETE_ARRAY(pTempSignatureBuffer);
            }
          }
              

          //解凍
          {
            if(isLegalData)
            {
              std::memset( m_uncompressBuf , 0 , sizeof(m_uncompressBuf) );
              u32 unCompressSize      = *(u32*)m_beaconDataBuffer[index].beaconSlot;
              u8* pCompressSizePoint  = (u8*)m_beaconDataBuffer[index].beaconSlot;
              pCompressSizePoint      += sizeof(u32);
              u32 compressSize        = *(u32*)pCompressSizePoint;
              u8* compressPoint       = (u8*)m_beaconDataBuffer[index].beaconSlot;
              compressPoint           += sizeof(u32) + sizeof(u32);
              GFL_PRINT("DeliveryWirelessBeaconBinder::Update : compressSize[%d][%d]\n",compressSize,unCompressSize);
              s32 unCompressResult    = 0;

              if( compressSize != 0 )
              {//圧縮済み
                unCompressResult = nn::cx::SecureUncompressLZ( compressPoint , compressSize , m_uncompressBuf );
                GFL_PRINT("DeliveryWirelessBeaconBinder::Update : uncompressResult[%d]\n",unCompressResult);
              }
              else
              {//非圧縮
                ::std::memcpy( m_uncompressBuf , compressPoint , unCompressSize );
                GFL_PRINT("DeliveryWirelessBeaconBinder::Update : not compress data\n");
              }

              /*  OnRecvComplete  */
              if(unCompressResult == 0)
              {
                isFinish  = !pRecvListener->OnRecvComplete( (const void*)m_uncompressBuf , unCompressSize );
              }
            }
          }

          if( isFinish )
          {//受信終了
            m_isFinish = true;
            return true;
          }
          else
          {//データを捨てた場合
            m_beaconDataBuffer[index].deliveryID = 0;
            m_beaconDataBuffer[index].recvBit[0] = 0;
            m_beaconDataBuffer[index].recvBit[1] = 0;
            m_beaconDataBuffer[index].recvBit[2] = 0;
            m_beaconDataBuffer[index].recvBit[3] = 0;
            m_beaconDataBuffer[index].recvNum = 0;
            m_beaconDataBuffer[index].splitNum = 0;

            m_lastProgress = 0;
          }
        }

        if( m_beaconDataBuffer[index].recvNum > 0 && m_beaconDataBuffer[index].splitNum > 0 )
        {//進捗確認
          progress = 1.0f * m_beaconDataBuffer[index].recvNum / m_beaconDataBuffer[index].splitNum * 100;
          if( maxProgress < progress )
          {
            maxProgress = progress;
          }
        }
      }
    }

    if( m_lastProgress < maxProgress )
    {//進捗が更新された（プログレスバーとかに使う想定）
      m_lastProgress = maxProgress;
      pRecvListener->OnUpdateProgress( m_lastProgress );
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
* @brief  ビーコン更新イベント
*
* @param  beacon 更新されたビーコン情報
*                ※引き渡されたデータアドレス領域は永続保障されない。
*                  ポインタを保持せず、自前のバッファにコピーすること。
*
* @param  localFriendCode ローカルフレンドコード。
*                         フレンドユーザーのみ有効な値がセットされ、
*                         その他のユーザーからのアクセス時には
*                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
*
* @param  transferdId     送信者のtransferdId
*/
//------------------------------------------------------------------
void DeliveryWirelessBeaconBinder::OnUpdateBeacon( const void* beacon, u64 localFriendCode, u64 tranfserdId )
{
  AddBeaconDataInternal( beacon );
}

//------------------------------------------------------------------
/**
* @brief  ビーコン削除前イベント
*
* @param  beacon 更新されるビーコン情報
*                ※引き渡されたデータアドレス領域は永続保障されない。
*                  ポインタを保持せず、自前のバッファにコピーすること。
*
*
* @param  localFriendCode ローカルフレンドコード。
*                         フレンドユーザーのみ有効な値がセットされ、
*                         その他のユーザーからのアクセス時には
*                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
*
* @param  transferdId     送信者のtransferdId
*/
//------------------------------------------------------------------
void DeliveryWirelessBeaconBinder::OnDeleteBeacon( const void* beacon, u64 localFriendCode, u64 tranfserdId )
{
  //特にやることなし
}


//------------------------------------------------------------------
/**
 * @brief   ビーコン保存処理
 */
//------------------------------------------------------------------
void DeliveryWirelessBeaconBinder::AddBeaconDataInternal( const void* pBeacon )
{
  //GFL_PRINT("DeliveryWirelessBeaconBinder::AddBeaconDataInternal : pBeacon[0x%x]\n",pBeacon);
  ICHI_PRINT("DeliveryWirelessBeaconBinder::AddBeaconDataInternal : pBeacon[0x%x]\n",pBeacon);

  if( m_isFinish )
  {//すでに終了している
    return;
  }

  if( !pBeacon )
  {
    GFL_PRINT("DeliveryWirelessBeaconBinder::AddBeaconDataInternal : error pBeacon is NULL\n");
    return;
  }

  const gflnet2::base::BeaconCommand*  pBeaconCmd  = static_cast<const gflnet2::base::BeaconCommand*>(pBeacon);
  if( pBeaconCmd->beaconType != m_pInitParam->GetType() )
  {//配信データではないのでスルー
    //GFL_PRINT("DeliveryWirelessBeaconBinder::Ignore Data[%d][%d][%d][%d]\n",pBeaconCmd->size ,pBeaconCmd->beaconType,pBeaconCmd->beaconFunctionNo,pBeaconCmd->dummy);
    return;
  }

  const WIRELESS_BEACON_DATA*   pData     = reinterpret_cast<const WIRELESS_BEACON_DATA*>(&pBeaconCmd[1]);
  {
    nn::os::CriticalSection::ScopedLock lock( m_criticalSectionForBeadonData );

    s32 beaconIndex = -1;
    for( u32 index = 0 ; index < MAX_BEACON_SLOT_NUM ; ++index )
    {
      if( m_beaconDataBuffer[index].deliveryID == pData->deliveryID )
      {//データIDが一致
        beaconIndex = index;
        break;
      }
    }

    if( beaconIndex == -1 )
    {
      for( u32 index = 0 ; index < MAX_BEACON_SLOT_NUM ; ++index )
      {
        if( m_beaconDataBuffer[index].recvNum == 0 )
        {//新規データ
          beaconIndex = index;
          break;
        }
      }
    }

    if( beaconIndex < 0 || beaconIndex >= MAX_BEACON_SLOT_NUM )
    {//格納するスロットが無い
      GFL_PRINT( "error : beacon slot index over[%d]\n" , pData->deliveryID );
      return;
    }

    if( pData->splitIndex >= MAX_SPLIT_NUM || pData->splitNum > MAX_SPLIT_NUM )
    {//インデックス範囲外
      GFL_PRINT( "error : splitdata index over[%d][%d]\n" , pData->splitIndex , pData->splitNum );
      return;
    }

    u32 bitIndex = pData->splitIndex / 32;
    u32 recvBitIndex = pData->splitIndex % 32;
    bit32 recvBit = m_beaconDataBuffer[beaconIndex].recvBit[bitIndex];
    s32 flag = recvBit & ( 1 << recvBitIndex );

    if( flag == 0 )
    {//まだ受信していないデータ

      //GFL_PRINT("[%x]!=[%x]\n",m_beaconDataBuffer[index].recvBit,( 1 << pData->splitIndex ));

      //受信データを設定
      const bit8* pBody = reinterpret_cast<const bit8*>(&pData[1]);
      std::memcpy( m_beaconDataBuffer[beaconIndex].beaconSlot[ pData->splitIndex ] , pBody , SPLIT_BEACON_BODY_SIZE );

      m_beaconDataBuffer[beaconIndex].recvNum++;
      m_beaconDataBuffer[beaconIndex].recvBit[bitIndex] |= ( 1 << recvBitIndex );//ビットを立てる

      m_beaconDataBuffer[beaconIndex].deliveryID = pData->deliveryID;//毎回上書き
      m_beaconDataBuffer[beaconIndex].splitNum = pData->splitNum;

      //GFL_PRINT("recv split data : slotIndex[%d]recvBit[0x%x]ID[%d]INDEX[%d]\n" , index , m_beaconDataBuffer[index].recvBit , pData->deliveryID , pData->splitIndex );
    }
  }
}


GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)
#endif

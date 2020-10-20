#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWirelessBeaconSplitter.cpp
 * @brief  ワイヤレスビーコン分割クラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================

#include <NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessBeaconSplitter.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

/*  署名  */
#include <zorro.h>
#include "./DeliveryKeys.h"

/*  署名処理は製品に載らないように  */
#if PM_DEBUG
  #define BEACON_SPLITTER_SIGNING_ENABLE
#endif


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)


//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
DeliveryWirelessBeaconSplitter::DeliveryWirelessBeaconSplitter( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam , const u16 deliveryID , const void* pSendData , const size_t dataSize, const bool bDoSign)
: m_pHeap( pHeap )
, m_pInitParam( pInitParam )
//, m_dataSize( dataSize )
, m_deliveryID( deliveryID )
//, m_pSendData( pSendData , dataSize )
{
  /*
    {
      u32 datasize;     //  overhead not included
      u32 compressSize;
      u8  data[];
    }
  */

  //圧縮
//  u32 memorySize = dataSize + ( SPLIT_BEACON_BODY_SIZE - ( SPLIT_BEACON_BODY_SIZE % dataSize ) );//SPLIT_BEACON_BODY_SIZEバイトアライメントにする
  const u32   headerBytes   = sizeof(u32) + sizeof(u32);    /**<  u32 dataSize; u32 compressSize;  */
  const u32   requiredBytes =                               /**<  送信データ格納に必要なBytes  */
                              headerBytes
                              + dataSize
#ifdef  BEACON_SPLITTER_SIGNING_ENABLE
                              + ((bDoSign) ? DeliveryKeys::OverheadBytes : 0)   /*  署名Overhead  */
#endif
                            ;
  const u32   memorySize    = (requiredBytes + (SPLIT_BEACON_BODY_SIZE-1)) / SPLIT_BEACON_BODY_SIZE * SPLIT_BEACON_BODY_SIZE;   //SPLIT_BEACON_BODY_SIZEバイトアライメントにする  /*  gfl2::math::RoundUp()は2^n専用  */


  m_pSendData = GFL_NEW_ARRAY( pHeap ) bit8[ memorySize ];

  u8* pDataPoint = m_pSendData + sizeof(u32) + sizeof(u32);

  bit8* workBuf = GFL_NEW_ARRAY( pHeap ) bit8[ nn::cx::LZ_COMPRESS_WORK_SIZE ];

  u32 compressSize = nn::cx::CompressLZ( (const u8*)pSendData , dataSize , (u8*)pDataPoint , (void*)workBuf );

  GFL_SAFE_DELETE_ARRAY( workBuf );

  std::memcpy( m_pSendData , &dataSize , sizeof(u32) );
  u8* compressSizePoint = m_pSendData+sizeof(u32);
  std::memcpy( compressSizePoint , &compressSize , sizeof(u32));

  GFL_PRINT("DeliveryWirelessBeaconSplitter::DeliveryWirelessBeaconSplitter : compressSize[%d][%d]\n",dataSize,compressSize);

  if( compressSize == 0 )
  {//圧縮したほうがサイズ大きくなってしまった
    GFL_ASSERT((pDataPoint + dataSize) <= (m_pSendData + memorySize));   /*  check buffer overrun  */
    ::std::memcpy( pDataPoint , pSendData , dataSize );
    m_dataSize = dataSize + sizeof(u32) + sizeof(u32);
  }
  else
  {//圧縮できた
    m_dataSize = compressSize + sizeof(u32) + sizeof(u32);
  }

  /*
    2016/07/05  Yu Muto
    署名関連機能の追加
    圧縮済みデータを一時バッファに退避し、signing
    ※デバッグ時専用
  */
#ifdef  BEACON_SPLITTER_SIGNING_ENABLE
  {
    const u8*   pSecretKey      = DeliveryKeys::SecretKey();
    const u32   secretKeyBytes  = DeliveryKeys::SecretKeyBytes();
    const u8*   pPublicKey      = DeliveryKeys::PublicKey();
    const u32   publicKeyBytes  = DeliveryKeys::PublicKeyBytes();
    const u32   overheadBytes   = DeliveryKeys::OverheadBytes;
    const u32   modulesBits     = DeliveryKeys::ModulesBits;
    const u32   tempbufBytes    = DeliveryKeys::TempBufBytes;

    if(bDoSign && pSecretKey)
    {
      const u32 targetDataBytes = m_dataSize - (sizeof(u32) + sizeof(u32));
      u8*       pTempSourceBuf  = GFL_NEW_ARRAY( pHeap ) u8[targetDataBytes];

      memcpy(pTempSourceBuf, pDataPoint, targetDataBytes);

      /*  sign  */
      {
        u8*   pSignature    = pDataPoint;   /**<  署名後データの格納先  */
        u32   signatureLen  = targetDataBytes + overheadBytes;
        u8    tempBuf[tempbufBytes];
        bool  result;

        const bool  isValidEnv  = (pSignature + signatureLen) < (m_pSendData + memorySize);   /*  check overrun  */

        GFL_ASSERT(isValidEnv);
        if(isValidEnv)
        {
          /*!
              本署名スキームのご利用の際、次のようなパラメータをおすすめします。
              | RSA鍵長       | オーバーヘッド     | 安全性　　 | 最小署名長　　　　　　　　 |
              +---------------+-----------------+----------+------------------------+
              | 768 ビット     | 8 バイト         | 64 ビット | 96 バイト               |
              | 1024 ビット    | 10 バイト        | 80 ビット | 128 バイト              |
              | 2048 ビット    | 16 バイト        | 128 ビット| 256 バイト              |

              @param[out] signature      署名へのポインタ
              @param[inout] signatureLen 署名バッファサイズ。(メッセージサイズ)＋(オーバーヘッドサイズ)以下のサイズの場合、関数は失敗します。
                                         そうでなければ署名長に設定されます。
              @param[in] message         署名対象のメッセージへのポインタ
              @param[in] messageLen      メッセージサイズ。(法パラメータの長さ)-(オーバーヘッドサイズ)以上である必要があります。
              @param[in] overhead        オーバーヘッドサイズ（バイト単位）
              @param[in] publicData      公開鍵データへのポインタ
              @param[in] publicDataLen   公開鍵データ長
              @param[in] derKey          署名するために用いられる鍵を含むRSA秘密鍵（DER形式）へのポインタ
              @param[in] derKeyLen       RSA秘密鍵データ長
              @param[in] modulusBitLen   RSAの法パラメータの長さ（ビット単位）
              @param[in] tempBuffer      Temporary buffer address.
              @param[in] tempBufferLen   Temporary buffer length. Must be at least 2 times the length of the
                                         RSA modulus.
              @return                    成功時はtrue、失敗時はfalse
          */
          result  = mw::zorro::OpssrCmcSha1_Sign(
                      pSignature,     signatureLen,
                      pTempSourceBuf, targetDataBytes,
                      overheadBytes,
                      pPublicKey,     publicKeyBytes,
                      pSecretKey,     secretKeyBytes,
                      modulesBits,
                      tempBuf,        tempbufBytes
                    );

          GFL_PRINT("%s: OpssrCmcSha1_Sign", __FUNCTION__);
          GFL_ASSERT_MSG(result, "OpssrCmcSha1_Sign failed\n");

          if(result)
          {
            m_dataSize  += overheadBytes;
          }
        }
      }

      GFL_SAFE_DELETE_ARRAY( pTempSourceBuf );
    }
  }
#endif

  GFL_PRINT("DeliveryWirelessBeaconSplitter : m_dataSize: [%u]\n", m_dataSize);
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
DeliveryWirelessBeaconSplitter::~DeliveryWirelessBeaconSplitter()
{
  m_pHeap = NULL;
  m_pInitParam = NULL;
  GFL_SAFE_DELETE_ARRAY( m_pSendData );
}



//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
void DeliveryWirelessBeaconSplitter::Update()
{

}
//------------------------------------------------------------------
/**
* @brief  分割数取得
*/
//------------------------------------------------------------------
u32 DeliveryWirelessBeaconSplitter::GetSplitNum()
{
  if( m_dataSize == 0 )
  {
    return 0;
  }

  return ( m_dataSize / SPLIT_BEACON_BODY_SIZE ) + 1;
}


//------------------------------------------------------------------
/**
* @brief  分割データ取得
*/
//------------------------------------------------------------------
size_t DeliveryWirelessBeaconSplitter::GetSplitData( u32 index , void* pOutput )
{
  if( pOutput == NULL || index >= GetSplitNum() )
  {//引数が不正
    GFL_PRINT("DeliveryWirelessBeaconSplitter::GetSplitData:Invalid param\n");
    return 0;
  }

  bit8* pData = m_pSendData;
  for( u32 i = 0 ; i < index ; i++ )
  {
    pData += SPLIT_BEACON_BODY_SIZE;
  }

  WIRELESS_BEACON_DATA header = { 0 };
  header.deliveryID = m_deliveryID;
  header.splitNum = GetSplitNum();
  header.splitIndex = index;

  bit8* pOutputData = (bit8*)pOutput;

  memcpy( pOutputData , &header , sizeof( header ) );
  pOutputData += sizeof(header);

  memcpy( pOutputData , pData , SPLIT_BEACON_BODY_SIZE );

  //GFL_PRINT( "DeliveryWirelessBeaconSplitter::GetSplitData[%u][0x%x]\n" , index , pOutput );

  return sizeof( header) + SPLIT_BEACON_BODY_SIZE;//今のところサイズは固定
}

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)
#endif

// ============================================================================
/*
 * @file gfl2_CTR_QREncoder.cpp
 * @brief QRコードのでコード処理を行うクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <qr/source/ctr/gfl2_CTR_QREncoder.h>
#include <qr/source/ctr/gfl2_CTR_QREncodeExecutor.h>
#include <qr/source/ctr/gfl2_CTR_QREncodeThread.h>
#include <qr/include/gfl2_IQREventListener.h>
#include <qr/source/ctr/gfl2_CTR_TextureConverter.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


CTR_QREncoder::CTR_QREncoder( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, int threadPriority ) :
  m_pHeap( pHeap ),
  m_pDeviceHeap( pDeviceHeap ),
  m_pQRRGBImageInfo( NULL ),
  m_pEncodeThread( NULL ),
  m_pThread( NULL ),
  m_pQREventListener( NULL ),
  m_pTextureBuffer( NULL ),
  m_ThreadPriority( threadPriority )
{
  m_pQRRGBImageInfo = GFL_NEW( m_pHeap ) gfl2::qr::QRRGBImageInfo( m_pHeap );
  m_pEncodeThread = GFL_NEW( m_pHeap ) gfl2::qr::ctr::CTR_QREncodeThread( m_pHeap, m_pDeviceHeap, m_pQRRGBImageInfo );
}


CTR_QREncoder::~CTR_QREncoder()
{
  if( m_pTextureBuffer )
  {
    GflHeapFreeMemory( m_pTextureBuffer );
  }
  GFL_SAFE_DELETE( m_pQRRGBImageInfo );
  GFL_SAFE_DELETE( m_pEncodeThread );
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を非同期で行います。
  * @param  pQREventListener エンコード処理が完了された時に呼び出されるイベントリスナー
  * @param  pQREncodeData QRコードのエンコード情報です
  */
// --------------------------------------------------------------------------
void CTR_QREncoder::EncodeAsync( IQREventListener* pQREventListener, const QREncodeData* pQREncodeData )
{
  if( m_pThread == NULL )
  {
    m_pQREventListener = pQREventListener;

    m_pEncodeThread->SetExecuteParameter( pQREncodeData );

    m_pThread = GFL_NEW( m_pHeap ) gfl2::thread::Thread( m_pEncodeThread, m_pHeap, false, STACK_SIZE );
        
    m_pThread->Start( m_ThreadPriority );
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を行います。
  * @param  pQREncodeData QRコードのエンコード情報です
  * @return QRコードのイメージ情報を返します
  */
// --------------------------------------------------------------------------
QRRGBImageInfo* CTR_QREncoder::Encode( const QREncodeData* pQREncodeData )
{
  gfl2::qr::ctr::CTR_QREncodeExecutor* pExecutor = GFL_NEW( m_pHeap ) gfl2::qr::ctr::CTR_QREncodeExecutor( m_pHeap, m_pDeviceHeap );

  bool bIsSuccess = pExecutor->Execute( m_pQRRGBImageInfo, pQREncodeData );

  GFL_SAFE_DELETE( pExecutor );

  if( bIsSuccess )
  {
    return m_pQRRGBImageInfo;
  }
  return NULL;
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を非同期で行う為の更新処理です。
  */
// --------------------------------------------------------------------------
void CTR_QREncoder::Update()
{
  if( m_pThread != NULL )
  {
    bool bIsAlive = m_pThread->IsAlive();
        
    if( !bIsAlive )
    {
      m_pThread->Kill();
          
      GFL_SAFE_DELETE( m_pThread );

      if( m_pEncodeThread->IsSuccess() )
      {
        if( m_pTextureBuffer )
        {
          GflHeapFreeMemory( m_pTextureBuffer );
        }
        m_pTextureBuffer = createNativeTexture( m_pQRRGBImageInfo );
        m_pQREventListener->OnEncodeSuccess( m_pQRRGBImageInfo, m_pTextureBuffer );
      }
      else
      {
        m_pQREventListener->OnEncodeFailure();
      }
    }
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を実行中か調べます。
  * @retval true 実行中している
  * @retval false 実行中していない
  */
// --------------------------------------------------------------------------
bool CTR_QREncoder::IsBusy()
{
  if( m_pThread != NULL )
  {
    return true;
  }
  return false;
}


u8* CTR_QREncoder::createNativeTexture( gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo )
{
  u8* pRGBImageBuffer = pQRRGBImageInfo->GetRGBImageBuffer();
  u16 imageWidth      = pQRRGBImageInfo->GetWidth();
  u16 imageHeight     = pQRRGBImageInfo->GetHeight();
  u16 realWidth       = static_cast<u16>( nn::math::CeilPwr2( static_cast<u32>( imageWidth ) ) );
  u16 realHeight      = static_cast<u16>( nn::math::CeilPwr2( static_cast<u32>( imageHeight ) ) );
  u32 imageBufferSize = realWidth * realHeight * 3;

  u8* pTextureBuffer = static_cast<u8*>( GflHeapAllocMemoryAlign( m_pDeviceHeap, imageBufferSize, 128 ) );
  u8* pReverseTextureBuffer = static_cast<u8*>( GflHeapAllocMemoryAlign( m_pDeviceHeap, imageBufferSize, 128 ) );

  /*  変換元を準備（ConvertGLTextureToNativeで上下が逆になるようなので、ここでも反転）  */
  {
    const u32   qrLineBytes   = imageWidth * 3;                     /*  qr画像のラインサイズ    */
    const u32   texLineBytes  = realWidth * 3;                      /*  テクスチャラインサイズ  */
    const u32   strideBytes   = texLineBytes - qrLineBytes;         /*  余白部分のラインサイズ  */

    const u8*   pSrc  = reinterpret_cast<const u8*>( pRGBImageBuffer );

    /*  有効部分  */
    for( u16 line = 0; line < imageHeight; ++line )
    {
      u8* pDst = pReverseTextureBuffer + texLineBytes * ( realHeight - 1 - line );

      std::memcpy( pDst, pSrc, qrLineBytes );   /*  qr部分  */
      pDst += qrLineBytes;
      pSrc += qrLineBytes;

      std::memset( pDst, 0x44, strideBytes );   /*  右余白  */
      pDst += strideBytes;
    }

    /*  下余白  */
    std::memset( pReverseTextureBuffer, 0x0f, texLineBytes * ( realHeight - imageHeight ) );
  }

  gfl2::qr::ctr::ConvertGLTextureToNative( GL_RGB_NATIVE_DMP, static_cast<u32>( realWidth ), static_cast<u32>( realHeight ), pReverseTextureBuffer, pTextureBuffer );

  GflHeapFreeMemory( pReverseTextureBuffer );

  return pTextureBuffer;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR

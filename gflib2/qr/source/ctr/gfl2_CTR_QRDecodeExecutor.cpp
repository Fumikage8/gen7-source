// ============================================================================
/*
 * @file gfl2_CTR_QRDecodeExecutor.cpp
 * @brief QRコードのデコード処理を実行するクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <qr/source/ctr/gfl2_CTR_QRDecodeExecutor.h>
#include <mw/qrd/qrd.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


CTR_QRDecodeExecutor::CTR_QRDecodeExecutor( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap ) :
  m_pHeap( pHeap ),
  m_pDeviceHeap( pDeviceHeap )
{
}


CTR_QRDecodeExecutor::~CTR_QRDecodeExecutor()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  デコード処理を実行します。
  * @param  pQRDecodeData QRコードのデコード結果です
  * @param  pQRYUVImageInfo デコード対象の画像情報です
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_QRDecodeExecutor::Execute( gfl2::qr::QRDecodeData* pQRDecodeData, const gfl2::qr::QRYUVImageInfo* pQRYUVImageInfo )
{
  u32 decodeBufferSize = mw::qrd::QRDecoder::GetDecodeBufferSize( pQRYUVImageInfo->GetWidth(), pQRYUVImageInfo->GetHeight() );

  u32* pDecodeBuffer = createDecodeBuffer( decodeBufferSize );
  
  if( pDecodeBuffer == NULL )
  {
    return false;
  }
  
  mw::qrd::QRDecoder* pDecoder = GFL_NEW( m_pHeap ) mw::qrd::QRDecoder();

  pDecoder->Initialize( pDecodeBuffer, decodeBufferSize );

  mw::qrd::DecodeData mwDecodeData;
  mw::qrd::ImageInfo mwImageInfo;

  memset( &mwDecodeData, 0, sizeof( mw::qrd::DecodeData ) );
  memset( &mwImageInfo, 0, sizeof( mw::qrd::ImageInfo ) );

  mwImageInfo.width             = pQRYUVImageInfo->GetWidth();
  mwImageInfo.height            = pQRYUVImageInfo->GetHeight();
  mwImageInfo.yuv422Data        = (u32*)pQRYUVImageInfo->GetYUVImageBuffer();
  mwImageInfo.qrPosition.top    = 0;
  mwImageInfo.qrPosition.left   = 0;
  mwImageInfo.qrPosition.right  = 0;
  mwImageInfo.qrPosition.bottom = 0;

  mw::qrd::Result result = pDecoder->ExecuteDecode( &mwDecodeData, &mwImageInfo );
  
  bool bIsSuccess = false;
  if( result == mw::qrd::RESULT_SUCCESS )
  {
    pQRDecodeData->ClearBuffer();
    pQRDecodeData->CreateBuffer( mwDecodeData.size );
    pQRDecodeData->CopyData( mwDecodeData.data, mwDecodeData.size );

    bIsSuccess = true;
  }

  pDecoder->Finalize();

  GFL_SAFE_DELETE( pDecoder );
  
  GflHeapFreeMemory( pDecodeBuffer );
  
  return bIsSuccess;
}


u32* CTR_QRDecodeExecutor::createDecodeBuffer( u32 decodeBufferSize )
{
  u32* pDecodeBuffer = NULL;

  if( decodeBufferSize > 0 )
  {
    pDecodeBuffer = reinterpret_cast<u32*>( GflHeapAllocMemoryAlign( m_pDeviceHeap, decodeBufferSize, 128 ) );
  }

  return pDecodeBuffer;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR

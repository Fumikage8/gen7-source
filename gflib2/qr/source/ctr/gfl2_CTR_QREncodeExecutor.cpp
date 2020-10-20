// ============================================================================
/*
 * @file gfl2_CTR_QREncodeExecutor.cpp
 * @brief QRコードのエンコード処理を実行するクラスです。
 * @date 2015.03.05
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <qr/source/ctr/gfl2_CTR_QREncodeExecutor.h>
#include <mw/qre/qre.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)


static const u32 QR_DIV_COUNT   = 1;


CTR_QREncodeExecutor::CTR_QREncodeExecutor( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap ) :
  m_pHeap( pHeap ),
  m_pDeviceHeap( pDeviceHeap )
{
}


CTR_QREncodeExecutor::~CTR_QREncodeExecutor()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコード処理を実行します。
  * @param  pQRRGBImageInfo QRコードのイメージ情報です
  * @param  pQREncodeData QRコードのエンコード情報です
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_QREncodeExecutor::Execute( gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo, const gfl2::qr::QREncodeData* pQREncodeData )
{
  mw::qre::QREncoder* pEncoder = GFL_NEW( m_pHeap->GetLowerHandle() ) mw::qre::QREncoder();

  u32 encodeBufferSize = mw::qre::QREncoder::GetEncodeBufferSize( pQREncodeData->GetDataSize(), pQREncodeData->GetCellSize(), QR_DIV_COUNT );
  
  u8* pEncodeBuffer = GFL_NEW_ARRAY( m_pDeviceHeap->GetLowerHandle() ) u8[ encodeBufferSize ];

  pEncoder->InitMemory( pEncodeBuffer, encodeBufferSize );

  mw::qre::EncodeData mwEncodeData;
  
  mwEncodeData.size      = pQREncodeData->GetDataSize();
  mwEncodeData.data      = pQREncodeData->GetData();
  mwEncodeData.version   = pQREncodeData->GetVersion();
  mwEncodeData.total     = QR_DIV_COUNT;
  mwEncodeData.cell_size = pQREncodeData->GetCellSize();
  mwEncodeData.ecc_level = mw::qre::ECC_LEVEL_L;

  bool bIsSuccess = pEncoder->Encode( &mwEncodeData );

  if( bIsSuccess )
  {
    if( QR_DIV_COUNT == pEncoder->GetDivCount() )
    {
      u32 qrSize = pEncoder->GetQRSize( 0, false );
      u32 bmpSize = pEncoder->GetQRSize( 0, true );

      pQRRGBImageInfo->ClearBuffer();
      pQRRGBImageInfo->ClearBMPBuffer();

      pQRRGBImageInfo->CreateBuffer( qrSize );
      pQRRGBImageInfo->CreateBMPBuffer( bmpSize );

      mw::qre::ImageInfo mwImageInfo;
      mwImageInfo.rgb888Data = pQRRGBImageInfo->GetRGBImageBuffer();
      pEncoder->GetQRData( &mwImageInfo, 0 );
      pQRRGBImageInfo->SetWidth( mwImageInfo.width );
      pQRRGBImageInfo->SetHeight( mwImageInfo.height );

      pEncoder->GetQRBMPData( pQRRGBImageInfo->GetBMPImageBuffer(), pQRRGBImageInfo->GetBMPImageBufferSize() );
    }
    else
    {
      bIsSuccess = false;
    }
  }

  GFL_SAFE_DELETE_ARRAY( pEncodeBuffer );

  GFL_SAFE_DELETE( pEncoder );

  return bIsSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR

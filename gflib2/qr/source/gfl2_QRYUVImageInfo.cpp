// ============================================================================
/*
 * @file gfl2_QRYUVImageInfo.cpp
 * @brief QRコードのYUV422形式のイメージ情報です。
 * @date 2015.03.05
 */
// ============================================================================
#include <qr/include/gfl2_QRYUVImageInfo.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージデータを格納するバッファを作成します。
  * @param  pDeviceHeap バッファを確保する為のデバイスヒープです。
  * @param  bufferSize バッファのサイズです。byteサイズを指定します。
  */
// --------------------------------------------------------------------------
QRYUVImageInfo::QRYUVImageInfo( gfl2::heap::HeapBase* pDeviceHeap, u32 bufferSize ) :
  m_pBuffer( NULL ),
  m_BufferSize( bufferSize ),
  m_Width( 400 ),     // @noto : sangoと同じ値にしています。
  m_Height( 240 )     // @noto : sangoと同じ値にしています。
{
  m_pBuffer = GFL_NEW_ARRAY( pDeviceHeap ) u8[ bufferSize ];
}


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージデータを格納するバッファを削除します。
  */
// --------------------------------------------------------------------------
QRYUVImageInfo::~QRYUVImageInfo()
{
  GFL_SAFE_DELETE_ARRAY( m_pBuffer );
}


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージデータをバッファにコピーします。
  * @param  pImage YUVイメージデータ
  * @param  imageSize YUVイメージデータのサイズ
  */
// --------------------------------------------------------------------------
void QRYUVImageInfo::CopyImage( const u8* pImage, u32 imageSize )
{
  if( m_pBuffer != NULL && m_BufferSize >= imageSize )
  {
    memcpy( m_pBuffer, pImage, imageSize );
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージの幅を設定します。
  * @param  width YUVイメージの幅
  */
// --------------------------------------------------------------------------
void QRYUVImageInfo::SetWidth( u16 width )
{
  m_Width = width;
}


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージの高さを設定します。
  * @param  height YUVイメージの高さ
  */
// --------------------------------------------------------------------------
void QRYUVImageInfo::SetHeight( u16 height )
{
  m_Height = height;
}


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージデータを取得します。
  * @return YUVイメージデータ
  */
// --------------------------------------------------------------------------
u8* QRYUVImageInfo::GetYUVImageBuffer() const
{
  return m_pBuffer;
}


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージの幅を取得します。
  * @return YUVイメージの幅
  */
// --------------------------------------------------------------------------
u16 QRYUVImageInfo::GetWidth() const
{
  return m_Width;
}


// --------------------------------------------------------------------------
/**
  * @brief  YUVイメージの高さを取得します。
  * @return YUVイメージの高さ
  */
// --------------------------------------------------------------------------
u16 QRYUVImageInfo::GetHeight() const
{
  return m_Height;
}


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

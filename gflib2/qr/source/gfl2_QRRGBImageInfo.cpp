// ============================================================================
/*
 * @file gfl2_QRRGBImageInfo.cpp
 * @brief QRコードのRGB形式のイメージ情報です。
 * @date 2015.03.05
 */
// ============================================================================
#include <qr/include/gfl2_QRRGBImageInfo.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


QRRGBImageInfo::QRRGBImageInfo( gfl2::heap::HeapBase* pDeviceHeap ) :
  m_pDeviceHeap( pDeviceHeap ),
  m_pBuffer( NULL ),
  m_pBMPBuffer( NULL ),
  m_BufferSize( 0 ),
  m_BMPBufferSize( 0 ),
  m_Width( 0 ),
  m_Height( 0 )
{
}


QRRGBImageInfo::~QRRGBImageInfo()
{
  ClearBuffer();
  ClearBMPBuffer();
}


// --------------------------------------------------------------------------
/**
  * @brief  RGBイメージデータを格納するバッファを削除します。
  */
// --------------------------------------------------------------------------
void QRRGBImageInfo::ClearBuffer()
{
  if( m_pBuffer != NULL )
  {
    GflHeapFreeMemory( m_pBuffer );
    m_pBuffer = NULL;
  }
  m_BufferSize = 0;
  m_Width = 0;
  m_Height = 0;
}


void QRRGBImageInfo::ClearBMPBuffer()
{
  if( m_pBMPBuffer != NULL )
  {
    GflHeapFreeMemory( m_pBMPBuffer );
    m_pBMPBuffer = NULL;
  }
  m_BMPBufferSize = 0;
}


// --------------------------------------------------------------------------
/**
  * @brief  RGBイメージデータを格納するバッファを作成します。
  * @param  bufferSize バッファのサイズです。byteサイズを指定します。
  */
// --------------------------------------------------------------------------
void QRRGBImageInfo::CreateBuffer( u32 bufferSize )
{
  if( m_pBuffer == NULL )
  {
    m_pBuffer = reinterpret_cast<u8*>( GflHeapAllocMemoryAlign( m_pDeviceHeap, bufferSize, 128 ) );
    m_BufferSize = bufferSize;
  }
}


void QRRGBImageInfo::CreateBMPBuffer( u32 bufferSize )
{
  if( m_pBMPBuffer == NULL )
  {
    m_pBMPBuffer = reinterpret_cast<u8*>( GflHeapAllocMemoryAlign( m_pDeviceHeap, bufferSize, 128 ) );
    m_BMPBufferSize = bufferSize;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  RGBイメージデータを取得します。
  * @return RGBイメージデータ
  */
// --------------------------------------------------------------------------
u8* QRRGBImageInfo::GetRGBImageBuffer() const
{
  return m_pBuffer;
}


u8* QRRGBImageInfo::GetBMPImageBuffer() const
{
  return m_pBMPBuffer;
}


// --------------------------------------------------------------------------
/**
  * @brief  RGBイメージデータのバッファサイズを取得します。
  * @return RGBイメージデータのバッファサイズ
  */
// --------------------------------------------------------------------------
u32 QRRGBImageInfo::GetRGBImageBufferSize() const
{
  return m_BufferSize;
}


u32 QRRGBImageInfo::GetBMPImageBufferSize() const
{
  return m_BMPBufferSize;
}


// --------------------------------------------------------------------------
/**
  * @brief  イメージの横幅を設定します。
  * @param  width イメージの横幅
  */
// --------------------------------------------------------------------------
void QRRGBImageInfo::SetWidth( u16 width )
{
  m_Width = width;
}


// --------------------------------------------------------------------------
/**
  * @brief  イメージの横幅を取得します。
  * @return イメージの横幅
  */
// --------------------------------------------------------------------------
u16 QRRGBImageInfo::GetWidth() const
{
  return m_Width;
}


// --------------------------------------------------------------------------
/**
  * @brief  イメージの縦幅を設定します。
  * @param  height イメージの縦幅
  */
// --------------------------------------------------------------------------
void QRRGBImageInfo::SetHeight( u16 height )
{
  m_Height = height;
}


// --------------------------------------------------------------------------
/**
  * @brief  イメージの縦幅を取得します。
  * @return イメージの縦幅
  */
// --------------------------------------------------------------------------
u16 QRRGBImageInfo::GetHeight() const
{
  return m_Height;
}

  
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

// ============================================================================
/*
 * @file gfl2_QRDecodeData.cpp
 * @brief QRコードのデコードデータです。
 * @date 2015.03.05
 */
// ============================================================================
#include <qr/include/gfl2_QRDecodeData.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


QRDecodeData::QRDecodeData( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_pBuffer( NULL ),
  m_BufferSize( 0 ),
  m_DataSize( 0 )
{
}


QRDecodeData::~QRDecodeData()
{
  ClearBuffer();
}


// --------------------------------------------------------------------------
/**
  * @brief  デコードデータを格納するバッファを削除します。
  */
// --------------------------------------------------------------------------
void QRDecodeData::ClearBuffer()
{
  GFL_SAFE_DELETE_ARRAY( m_pBuffer );
  m_BufferSize = 0;
  m_DataSize = 0;
}


// --------------------------------------------------------------------------
/**
  * @brief  デコードデータを格納するバッファを作成します。
  * @param  bufferSize バッファのサイズです。byteサイズを指定します。
  */
// --------------------------------------------------------------------------
void QRDecodeData::CreateBuffer( u32 bufferSize )
{
  if( m_pBuffer == NULL )
  {
    m_pBuffer = GFL_NEW( m_pHeap ) u8[ bufferSize ];
    m_BufferSize = bufferSize;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  デコードデータをバッファにコピーします。
  * @param  pData デコード結果のデータ
  * @param  dataSize デコードデータのサイズ
  */
// --------------------------------------------------------------------------
void QRDecodeData::CopyData( const u8* pData, u32 dataSize )
{
  if( m_pBuffer != NULL && m_BufferSize >= dataSize )
  {
    memcpy( m_pBuffer, pData, dataSize );
    m_DataSize = dataSize;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  デコードデータを取得します。
  * @return デコードデータ
  */
// --------------------------------------------------------------------------
u8* QRDecodeData::GetData() const
{
  return m_pBuffer;
}


// --------------------------------------------------------------------------
/**
  * @brief  デコードデータのサイズを取得します。
  * @return デコードデータのサイズ
  */
// --------------------------------------------------------------------------
u32 QRDecodeData::GetDataSize() const
{
  return m_DataSize;
}


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

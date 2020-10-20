// ============================================================================
/*
 * @file gfl2_QREncodeData.cpp
 * @brief QRコードのエンコードデータです。
 * @date 2015.03.05
 */
// ============================================================================
#include <qr/include/gfl2_QREncodeData.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


// --------------------------------------------------------------------------
/**
  * @brief  エンコードデータを格納するバッファを作成します。
  * @param  pDeviceHeap バッファを作成する為のデバイスヒープです。
  * @param  bufferSize バッファのサイズです。byteサイズを指定します。
  */
// --------------------------------------------------------------------------
QREncodeData::QREncodeData( gfl2::heap::HeapBase* pDeviceHeap, u32 bufferSize ) :
  m_pDeviceHeap( pDeviceHeap ),
  m_pBuffer( NULL ),
  m_BufferSize( 0 ),
  m_DataSize( 0 ),
  m_CellSize( 2 ),  // @noto : sangoと同じ値にしています。
  m_Version( 16 )   // @note : sangoと同じ値にしています。
{
  CreateBuffer( bufferSize );
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコードデータを格納するバッファを削除します。
  */
// --------------------------------------------------------------------------
QREncodeData::~QREncodeData()
{
  ClearBuffer();
}


void QREncodeData::ClearBuffer()
{
  GFL_SAFE_DELETE_ARRAY( m_pBuffer );
  m_BufferSize = 0;
  m_DataSize = 0;
}


void QREncodeData::CreateBuffer( u32 bufferSize )
{
  if( m_pBuffer == NULL )
  {
    m_pBuffer = GFL_NEW_ARRAY( m_pDeviceHeap ) u8[ bufferSize ];
    m_BufferSize = bufferSize;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコードデータをバッファにコピーします。
  * @param  pData エンコードデータ
  * @param  dataSize エンコードデータのサイズ
  */
// --------------------------------------------------------------------------
void QREncodeData::CopyData( const u8* pData, u32 dataSize )
{
  if( m_pBuffer != NULL && m_BufferSize >= dataSize )
  {
    memcpy( m_pBuffer, pData, dataSize );
    m_DataSize = dataSize;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  セルのサイズを設定します。
  * @param  cellSize セルのサイズです。１セルのドット数を指定します
  */
// --------------------------------------------------------------------------
void QREncodeData::SetCellSize( u32 cellSize )
{
  m_CellSize = cellSize;
}


// --------------------------------------------------------------------------
/**
  * @brief  フォーマットバージョンを設定します。
  * @param  version フォーマットバージョンです。0～40を指定します
  *                 0を指定するとフォーマットバージョンを自動で設定します。
  */
// --------------------------------------------------------------------------
void QREncodeData::SetVersion( u32 version )
{
  m_Version = version;
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコードデータを取得します。
  * @return エンコードデータ
  */
// --------------------------------------------------------------------------
u8* QREncodeData::GetData() const
{
  return m_pBuffer;
}


// --------------------------------------------------------------------------
/**
  * @brief  エンコードデータのサイズを取得します。
  * @return エンコードデータのサイズ
  */
// --------------------------------------------------------------------------
u32 QREncodeData::GetDataSize() const
{
  return m_DataSize;
}


// --------------------------------------------------------------------------
/**
  * @brief  セルのサイズを取得します。
  * @return セルのサイズ
  */
// --------------------------------------------------------------------------
u32 QREncodeData::GetCellSize() const
{
  return m_CellSize;
}


// --------------------------------------------------------------------------
/**
  * @brief  フォーマットバージョンを取得します。
  * @return フォーマットバージョン
  */
// --------------------------------------------------------------------------
u32 QREncodeData::GetVersion() const
{
  return m_Version;
}


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

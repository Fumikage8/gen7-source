//=============================================================================
/**
 * @file   btl_SendData.cpp
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  サーバー・クライアント間で送受信されるデータ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_SendData.h"

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief 送信データバッファをクリアする
 * @param buf  クリアするバッファ
 */
 //-----------------------------------------------------------------------------
void SEND_DATA_BUFFER_Clear( SEND_DATA_BUFFER* buf )
{
  buf->header.serialNumber = SEND_DATA_SERIAL_NUMBER_NULL;
  buf->header.serverSeq    = SERVER_SEQ_NULL;
  buf->header.cmd          = ACMD_NONE;
  buf->header.size         = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データバッファをコピーする
 * @param[out] dest  コピー先バッファ
 * @param      src   コピー元バッファ
 */
//-----------------------------------------------------------------------------
void SEND_DATA_BUFFER_Copy( SEND_DATA_BUFFER* dest, const SEND_DATA_BUFFER& src )
{
  const SendDataSerialNumber serialNumber = SEND_DATA_BUFFER_GetSerialNumber( &src );
  const ServerSequence       serverSeq    = SEND_DATA_BUFFER_GetServerSequence( &src );
  const AdapterCommand       command      = SEND_DATA_BUFFER_GetAdapterCommand( &src );
  const void*                data         = SEND_DATA_BUFFER_GetData( &src );
  const u32                  dataSize     = SEND_DATA_BUFFER_GetDataSize( &src );

  SEND_DATA_BUFFER_Store( dest, serialNumber, serverSeq, command, data, dataSize );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データをバッファに格納する
 * @param[out] buf             送信データの格納先
 * @param      serialNumber    送信データの通し番号
 * @param      serverSeq       送信時のサーバーシーケンス
 * @param      adapterCommand  送信するコマンド
 * @param      data            送信データ本体
 * @param      dataSize        送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void SEND_DATA_BUFFER_Store( SEND_DATA_BUFFER* buf, SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand adapterCommand, const void* data, u32 dataSize )
{
  GFL_ASSERT_STOP( dataSize < sizeof(buf->data) );

  buf->header.serialNumber = serialNumber;
  buf->header.serverSeq    = serverSeq;
  buf->header.cmd          = adapterCommand;
  buf->header.size         = dataSize;
  gfl2::std::MemCopy( data, buf->data, dataSize );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、通し番号を取得する
 * @param buf  送信データ
 */
//-----------------------------------------------------------------------------
SendDataSerialNumber SEND_DATA_BUFFER_GetSerialNumber( const SEND_DATA_BUFFER* buf )
{
  return static_cast<SendDataSerialNumber>(buf->header.serialNumber);
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、送信時のサーバーのシーケンス番号を取得する
 * @param buf  送信データ
 */
//-----------------------------------------------------------------------------
ServerSequence SEND_DATA_BUFFER_GetServerSequence( const SEND_DATA_BUFFER* buf )
{
  return static_cast<ServerSequence>(buf->header.serverSeq);
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、コマンドを取得する
 * @param buf  送信データ
 */
//-----------------------------------------------------------------------------
AdapterCommand SEND_DATA_BUFFER_GetAdapterCommand( const SEND_DATA_BUFFER* buf )
{
  return static_cast<AdapterCommand>(buf->header.cmd);
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、データ本体を取得する
 */
//-----------------------------------------------------------------------------
const void* SEND_DATA_BUFFER_GetData( const SEND_DATA_BUFFER* buf )
{
  return buf->data;
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、データ本体のサイズを取得する
 */
//-----------------------------------------------------------------------------
u32 SEND_DATA_BUFFER_GetDataSize( const SEND_DATA_BUFFER* buf )
{
  return buf->header.size;
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データ全体のサイズを取得する
 * @param buf  送信データ
 */
//-----------------------------------------------------------------------------
u32 SEND_DATA_BUFFER_GetTotalSize( const SEND_DATA_BUFFER* buf )
{
  return sizeof( buf->header ) + buf->header.size;
}





//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
SendData::SendData( void )
{
  this->Clear();
}

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param src コピー元のデータ
 */
//-----------------------------------------------------------------------------
SendData::SendData( const SEND_DATA_BUFFER& src )
{
  SEND_DATA_BUFFER_Copy( &m_buffer, src );
}

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param src コピー元のデータ
 */
//-----------------------------------------------------------------------------
SendData::SendData( const SendData& src )
{
  SEND_DATA_BUFFER_Copy( &m_buffer, src.GetBuffer() );
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
SendData::~SendData()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データのバッファを取得する
 */
//-----------------------------------------------------------------------------
const SEND_DATA_BUFFER& SendData::GetBuffer( void ) const
{
  return m_buffer;
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データバッファをクリアする
 */
//-----------------------------------------------------------------------------
void SendData::Clear( void )
{
  SEND_DATA_BUFFER_Clear( &m_buffer );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データバッファをコピーする
 * @param src   コピー元のデータ
 */
//-----------------------------------------------------------------------------
void SendData::CopyFrom( const SendData& src )
{
  SEND_DATA_BUFFER_Copy( &m_buffer, src.m_buffer );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データをバッファに格納する
 * @param serialNumber    送信データの通し番号
 * @param serverSeq       送信時のサーバーシーケンス
 * @param adapterCommand  送信するコマンド
 * @param data            送信データ本体
 * @param dataSize        送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void SendData::Store( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand adapterCommand, const void* data, u32 dataSize )
{
  SEND_DATA_BUFFER_Store( &m_buffer, serialNumber, serverSeq, adapterCommand, data, dataSize );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データをバッファに格納する
 * @param buffer  格納するデータ
 */
//-----------------------------------------------------------------------------
void SendData::Store( const SEND_DATA_BUFFER& buffer )
{
  this->Store(
    static_cast<SendDataSerialNumber>( buffer.header.serialNumber ),
    static_cast<ServerSequence>( buffer.header.serverSeq ),
    static_cast<AdapterCommand>( buffer.header.cmd ),
    reinterpret_cast<const void*>( buffer.data ),
    buffer.header.size );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、通し番号を取得する
 */
//-----------------------------------------------------------------------------
SendDataSerialNumber SendData::GetSerialNumber( void ) const
{
  return SEND_DATA_BUFFER_GetSerialNumber( &m_buffer );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、送信時のサーバーのシーケンス番号を取得する
 * @param buf  送信データ
 */
//-----------------------------------------------------------------------------
ServerSequence SendData::GetServerSequence( void ) const
{
  return SEND_DATA_BUFFER_GetServerSequence( &m_buffer );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、コマンドを取得する
 */
//-----------------------------------------------------------------------------
AdapterCommand SendData::GetAdapterCommand( void ) const
{
  return SEND_DATA_BUFFER_GetAdapterCommand( &m_buffer );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、データ本体を取得する
 */
//-----------------------------------------------------------------------------
const void* SendData::GetData( void ) const
{
  return SEND_DATA_BUFFER_GetData( &m_buffer );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データから、データ本体のサイズを取得する
 */
//-----------------------------------------------------------------------------
u32 SendData::GetDataSize( void ) const
{
  return SEND_DATA_BUFFER_GetDataSize( &m_buffer );
}

//-----------------------------------------------------------------------------
/**
 * @brief 送信データ全体のサイズを取得する
 */
//-----------------------------------------------------------------------------
u32 SendData::GetTotalSize( void ) const
{
  return SEND_DATA_BUFFER_GetTotalSize( &m_buffer );
}


GFL_NAMESPACE_END( btl )
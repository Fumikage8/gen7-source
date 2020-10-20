//=============================================================================
/**
 * @file   btl_SendDataContainer.cpp
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  サーバー・クライアント間で送受信されたデータを保持するコンテナ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_SendDataContainer.h"

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heapForBuffer  バッファの確保に使用するヒープ
 * @param bufferNum      バッファの数
 */
//-----------------------------------------------------------------------------
SendDataContainer::SendDataContainer( HeapHandle heapForBuffer, u32 bufferNum ) : 
  m_buffer( NULL ),
  m_bufferNum( bufferNum ),
  m_addIndex( 0 )
{
  m_buffer = GFL_NEW( heapForBuffer ) SendData[ bufferNum ];
  this->ClearBuffer();
}

/**
 * @brief バッファをクリアする
 */
void SendDataContainer::ClearBuffer( void )
{
  m_addIndex = 0;

  for( u32 i=0; i<m_bufferNum; ++i )
  {
    m_buffer[i].Clear();
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
SendDataContainer::~SendDataContainer()
{
  GFL_SAFE_DELETE_ARRAY( m_buffer );
}

//-------------------------------------------------------------------------
/**
 * @brief データをコピーする
 * @param srcContainer  コピー元データのコンテナ
 */
//-------------------------------------------------------------------------
void SendDataContainer::CopyFrom( const SendDataContainer& srcContainer )
{
  // バッファの数が一致しないとダメ
  if( this->m_bufferNum != srcContainer.m_bufferNum )
  {
    GFL_ASSERT(0);
    return;
  }

  this->m_addIndex = srcContainer.m_addIndex;

  for( u32 i=0; i<m_bufferNum; ++i )
  {
    this->m_buffer[i].CopyFrom( srcContainer.m_buffer[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief データを追加する
 * @param dataBuffer  追加するデータ
 */
//-----------------------------------------------------------------------------
void SendDataContainer::AddData( const SendData& dataBuffer )
{
  // 既に登録済みなら、登録しない
  if( this->IsDataExist( dataBuffer.GetSerialNumber() ) )
  {
    GFL_ASSERT(0);
    return;
  }

  m_buffer[ m_addIndex ].CopyFrom( dataBuffer );
  this->IncAddIndex();
}

/**
 * @brief データの追加位置をインクリメントする
 */
void SendDataContainer::IncAddIndex( void )
{
  m_addIndex = ( m_addIndex + 1 ) % m_bufferNum;
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定したデータが存在するか
 * @param dataSerialNumber  データの通し番号
 * @retval true   指定したデータが存在する
 * @retval false  指定したデータが存在しない
 */
//-----------------------------------------------------------------------------
bool SendDataContainer::IsDataExist( u32 dataSerialNumber ) const
{
  const SendData* data = this->GetData( dataSerialNumber );
  return ( data != NULL );
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定したデータを取得する
 * @param dataSerialNumber  データの通し番号
 * @retval NULL  指定したデータが存在しない場合
 */
//-----------------------------------------------------------------------------
const SendData* SendDataContainer::GetData( u32 dataSerialNumber ) const
{
  for( u32 i=0; i<m_bufferNum; ++i )
  {
    if( ( m_buffer[i].GetSerialNumber() != SEND_DATA_SERIAL_NUMBER_NULL ) &&
        ( m_buffer[i].GetSerialNumber() == dataSerialNumber ) ) 
    {
      return &m_buffer[i];
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief 通し番号が最も大きいデータを取得する
 * @retval NULL  データが存在しない場合
 */
//-----------------------------------------------------------------------------
const SendData* SendDataContainer::GetLatestData( void ) const
{
  u32 latestDataIndex = 0;
  SendDataSerialNumber maxSerialNo = SEND_DATA_SERIAL_NUMBER_NULL;

  for( u32 i=0; i<m_bufferNum; ++i )
  {
    const SendDataSerialNumber serialNo = m_buffer[i].GetSerialNumber();
    if( ( maxSerialNo == SEND_DATA_SERIAL_NUMBER_NULL ) ||
        ( ( serialNo != SEND_DATA_SERIAL_NUMBER_NULL ) &&
          ( serialNo >= maxSerialNo ) ) )
    {
      maxSerialNo = serialNo;
      latestDataIndex = i;
    }
  }

  if( maxSerialNo == SEND_DATA_SERIAL_NUMBER_NULL )
  {
    return NULL;
  }

  return &m_buffer[ latestDataIndex ];
}



GFL_NAMESPACE_END( btl )
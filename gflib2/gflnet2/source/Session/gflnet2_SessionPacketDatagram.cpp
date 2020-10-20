//=============================================================================
/**
 * @file   gflnet2_SessionPacketDatagram.cpp
 * @brief  セッション内パケットデータグラム
 * @author shin kosaka
 */
//=============================================================================
#include <gflnet2/source/Session/gflnet2_SessionPacketDatagram.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)

SessionPacketDatagram::SessionPacketDatagram()
: m_totalSize(0)
, m_pDatagramBuffer(NULL)
, m_dataSizeArray()
{
}

SessionPacketDatagram::~SessionPacketDatagram()
{
  GflHeapFreeMemory( m_pDatagramBuffer );
}

void SessionPacketDatagram::AddDataSize( u16 dataSize )
{
  m_dataSizeArray.push_back( dataSize );
  m_totalSize += dataSize;
}
void SessionPacketDatagram::CreateBuffer( gfl2::heap::HeapBase* pHeap )
{
  if( m_totalSize > MAX_PACKET_SIZE )
  {
    GFL_ASSERT_MSG(0 ,"size limit over");
    return;
  }

  m_pDatagramBuffer = GflHeapAllocMemoryAlign( pHeap, m_totalSize, DATAGRAM_ALIGN );
}

void SessionPacketDatagram::DeleteBuffer( void )
{
  GflHeapFreeMemory( m_pDatagramBuffer );
  m_totalSize = 0;
  m_dataSizeArray.clear();
}

bool SessionPacketDatagram::SetData( u32 messageID , const u8* pMessage , u16 messageSize )
{
  if( m_pDatagramBuffer == NULL )
  {
    GFL_ASSERT_MSG(0,"m_pDatagramBuffer is NULL");
    return false;
  }

  if( m_dataSizeArray.size() <= messageID )
  {
    GFL_ASSERT_MSG(0,"invalid messageID");
    return false;
  }

  if( m_dataSizeArray[messageID] != messageSize )
  {
    GFL_ASSERT_MSG(0,"invalid dataSize");
    return false;
  }

  u8* pDataAddr = static_cast<u8*>(m_pDatagramBuffer);
  for( u32 index = 0 ; index < messageID ; ++index )
  {
    pDataAddr += m_dataSizeArray[index];
  }

  gfl2::std::MemCopy( pMessage , pDataAddr , messageSize );

  return true;
}

u16  SessionPacketDatagram::GetTotalDataSize( void )
{
  return m_totalSize;
}

u16  SessionPacketDatagram::GetDataSize( u16 dataSizeIndex )
{
  if( m_dataSizeArray.size() <= dataSizeIndex )
  {
    GFL_ASSERT_MSG(0,"invalid index");
    return 0;
  }

  return m_dataSizeArray[dataSizeIndex];
}

u8*  SessionPacketDatagram::GetDataAddr( void )
{
  return static_cast<u8*>( m_pDatagramBuffer );
}

u8*  SessionPacketDatagram::GetRecvDataAddr( u8* pPacket , u16 dataIndex )
{
  if( m_pDatagramBuffer == NULL )
  {
    GFL_ASSERT_MSG(0,"m_pDatagramBuffer is NULL");
    return NULL;
  }

  if( m_dataSizeArray.size() <= dataIndex )
  {
    GFL_ASSERT_MSG(0,"invalid messageID");
    return NULL;
  }

  u8* pDataAddr = static_cast<u8*>(m_pDatagramBuffer);
  for( u32 index = 0 ; index < dataIndex ; ++index )
  {
    pDataAddr += m_dataSizeArray[index];
  }

  return pDataAddr;
}

u16  SessionPacketDatagram::GetDataSizeNum( void )
{
  return m_dataSizeArray.size();
}

GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



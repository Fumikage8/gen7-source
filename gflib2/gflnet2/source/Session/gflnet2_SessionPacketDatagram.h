//=============================================================================
/**
 * @file   gflnet2_SessionPacketDatagram.h
 * @brief  セッション内パケットデータグラム
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_SESSION_PACKET_DATAGRAM_H__
#define __GFLNET2_SESSION_PACKET_DATAGRAM_H__
#pragma once

#include <gflnet2/include/Session/gflnet2_SessionPacketInitParam.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)


/***************************************************************************!
 * @brief  セッション内パケットデータグラム
 ***************************************************************************/
class SessionPacketDatagram
{
public:
  enum
  {
    DATAGRAM_ALIGN = 4,
    MAX_PACKET_SIZE = 512,
  };

public:

  SessionPacketDatagram();
  ~SessionPacketDatagram();

  void AddDataSize( u16 dataSize );
  void CreateBuffer( gfl2::heap::HeapBase* pHeap );
  void DeleteBuffer( void );
  bool SetData( u32 messageID , const u8* pMessage , u16 messageSize );
  u16  GetTotalDataSize( void );
  u16  GetDataSize( u16 dataSizeIndex );
  u8*  GetDataAddr( void );
  u8*  GetRecvDataAddr( u8* pPacket , u16 dataIndex );
  u16  GetDataSizeNum( void );
  //void* GetData

private:

  u16 m_totalSize;
  void*  m_pDatagramBuffer;
  gfl2::util::FixedSizeContainer<u16,MAX_UDP_MESSAGE_NUM> m_dataSizeArray;
};


GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET2_SESSION_PACKET_DATAGRAM_H__



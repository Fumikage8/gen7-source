// ============================================================================
/*
 * @file P2pPacketTransfer.cpp
 * @brief 大きいサイズのデータを分割して送受信を行うクラスです。
 * @date 2015.08.24
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetLib/include/P2P/P2pPacketTransfer.h"
#include <util/include/gfl2_std_string.h>
#include <gflnet2/source/p2p/gflnet2_NetGameBase.h>


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)


P2pPacketTransfer::P2pPacketTransfer( gfl2::heap::HeapBase* pLowerHeap, NetLib::P2P::P2pPacketTransferListener* pP2pPacketTransferListener ) :
#if defined( GF_PLATFORM_CTR )
  m_pNetGame( NULL ),
#endif // defiend( GF_PLATFORM_CTR )
  m_pHeap( pLowerHeap ),
  m_pP2pPacketTransferListener( pP2pPacketTransferListener ),
  m_SendInfo(),
  m_Temp(),
  m_RecvPacketInfoList(),
  m_RecvPacketDatasList(),
  m_Sequence( SEQUENCE_IDLE ),
  m_bNetGameInitialized( false )
{
}


P2pPacketTransfer::~P2pPacketTransfer()
{
  clearList();

#if defined( GF_PLATFORM_CTR )

  if( m_pNetGame )
  {
    m_pNetGame->Finalize();
    GFL_SAFE_DELETE( m_pNetGame );
  }

#endif // defiend( GF_PLATFORM_CTR )

}


void P2pPacketTransfer::clearList()
{
  if( m_RecvPacketDatasList.MaxSize() > 0 )
  {
    for( ItPacketDatasList it = m_RecvPacketDatasList.Begin(); it != m_RecvPacketDatasList.End(); ++it )
    {
      it->second->Clear();
      GFL_SAFE_DELETE( it->second );
    }
  }
  m_RecvPacketDatasList.Clear();

  m_RecvPacketInfoList.Clear();
}


void P2pPacketTransfer::Update()
{
#if defined( GF_PLATFORM_CTR )

  if( m_bNetGameInitialized )
  {
    recvData();
  }

  switch( m_Sequence )
  {
    // --------------------------------------------------------------------------------
    case SEQUENCE_INITIALIZE:
    {
      m_pNetGame = GFL_NEW( m_pHeap ) gflnet2::p2p::NetGame();
      m_pNetGame->Initialize();
      m_Sequence = SEQUENCE_INITIALIZE_WAIT;
    }
    break;

    // --------------------------------------------------------------------------------
    case SEQUENCE_INITIALIZE_WAIT:
    {
      if( m_pNetGame->IsInitialize() )
      {
        clearList();

        // 自分を除く為 -1 してます
        int size = m_pNetGame->GetConnectionCount() - 1;

        GFL_PRINT( "-------------------------- P2pPacketTransfer --------------------------\n" );
        GFL_PRINT( "Connection count = %d \n", m_pNetGame->GetConnectionCount() );
        GFL_PRINT( "Received buffer size = %d \n", size );

        m_RecvPacketInfoList.CreateBuffer( m_pHeap, size );
        m_RecvPacketDatasList.CreateBuffer( m_pHeap, size );

        m_bNetGameInitialized = true;

        m_Sequence = SEQUENCE_SEND_PACKET_INFO;
      }
    }
    break;

    // --------------------------------------------------------------------------------
    case SEQUENCE_SEND_PACKET_INFO:
    {
      m_Temp.info.sendSize  = m_SendInfo.sendSize;
      m_Temp.info.max       = m_SendInfo.max;
      m_Temp.info.commandID = m_SendInfo.commandID;

      bool bResult = m_pNetGame->SendDataCommand( reinterpret_cast<char*>( &m_Temp.info ), static_cast<int>( sizeof( PacketInfo ) ), COMMAND_ID_SEND_PACKET_INFO );
      
      if( bResult )
      {
        GFL_PRINT( "-------------------------- P2pPacketTransfer --------------------------\n" );
        GFL_PRINT( "Send packet info. \n" );
        GFL_PRINT( "NetID        = %d \n", m_pNetGame->GetNetID() );
        GFL_PRINT( "Send size    = %d \n", m_Temp.info.sendSize );
        GFL_PRINT( "Packet count = %d \n", m_Temp.info.max );
        GFL_PRINT( "CommandID    = %d \n", m_Temp.info.commandID );

        m_Sequence = SEQUENCE_RECV_PACKET_INFO;
      }
    }
    break;

    // --------------------------------------------------------------------------------
    case SEQUENCE_RECV_PACKET_INFO:
    {
      if( m_RecvPacketInfoList.Size() == m_RecvPacketInfoList.MaxSize() )
      {
        GFL_PRINT( "-------------------------- P2pPacketTransfer --------------------------\n" );
        GFL_PRINT( "Recv packet info.\n" );
        for( ItPacketInfoList it = m_RecvPacketInfoList.Begin(); it != m_RecvPacketInfoList.End(); ++it )
        {
          GFL_PRINT( "NetID        = %d \n", it->first );
          GFL_PRINT( "Send size    = %d \n", it->second.sendSize );
          GFL_PRINT( "Packet count = %d \n", it->second.max );
          GFL_PRINT( "CommandID    = %d \n", it->second.commandID );
        }

        m_Sequence = SEQUENCE_SEND_PACKET_DATA;
      }
    }
    break;

    // --------------------------------------------------------------------------------
    case SEQUENCE_SEND_PACKET_DATA:
    {
      m_Temp.data.size = PACKET_DATA_SIZE;

      if( ( m_SendInfo.count + 1 ) >= m_SendInfo.max )
      {
        m_Temp.data.size = m_SendInfo.sendSize % PACKET_DATA_SIZE;
      }

      m_Temp.data.packetID = m_SendInfo.count;
      gfl2::std::MemCopy( &m_SendInfo.pSendData[ m_SendInfo.count * PACKET_DATA_SIZE ], m_Temp.data.data, PACKET_DATA_SIZE );

      bool bResult = m_pNetGame->SendDataCommand( reinterpret_cast<char*>( &m_Temp.data ), static_cast<int>( sizeof( PacketData ) ), COMMAND_ID_SEND_PACKET_DATA );
      
      if( bResult )
      {
        GFL_PRINT( "-------------------------- P2pPacketTransfer --------------------------\n" );
        GFL_PRINT( "Send packet data. \n" );
        GFL_PRINT( "NetID        = %d \n", m_pNetGame->GetNetID() );
        GFL_PRINT( "Packet size  = %d \n", m_Temp.data.size );
        GFL_PRINT( "PacketID     = %d \n", m_Temp.data.packetID );

        m_SendInfo.count++;

        if( m_SendInfo.count >= m_SendInfo.max )
        {
          m_Sequence = SEQUENCE_RECV_PACKET_DATA;
        }
      }
      
    }
    break;

    // --------------------------------------------------------------------------------
    case SEQUENCE_RECV_PACKET_DATA:
    {
      if( m_RecvPacketDatasList.Size() == m_RecvPacketDatasList.MaxSize() )
      {
        bool bReceivedAll = true;

        // 全部受信したかチェック
        {
          for( ItPacketDatasList packetDatas = m_RecvPacketDatasList.Begin(); packetDatas != m_RecvPacketDatasList.End(); ++packetDatas )
          {
            if( packetDatas->second->Size() != packetDatas->second->MaxSize() )
            {
              bReceivedAll = false;
              break;
            }
          }
        }

        if( bReceivedAll )
        {
          GFL_PRINT( "-------------------------- P2pPacketTransfer --------------------------\n" );
          GFL_PRINT( "Recv packet data. \n" );
          for( ItPacketDatasList packetDatas = m_RecvPacketDatasList.Begin(); packetDatas != m_RecvPacketDatasList.End(); ++packetDatas )
          {
            GFL_PRINT( "NetID        = %d \n", packetDatas->first );
            for( ItPacketDatas packet = packetDatas->second->Begin(); packet != packetDatas->second->End(); ++packet )
            {
              GFL_PRINT( "Packet size  = %d \n", packet->second.size );
              GFL_PRINT( "PacketID     = %d(%d) \n", packet->first, packet->second.packetID );
            }
          }

          m_Sequence = SEQUENCE_TERMINATE;
        }
      }
    }
    break;

    // --------------------------------------------------------------------------------
    case SEQUENCE_TERMINATE:
    {
      m_bNetGameInitialized = false;

      if( m_pNetGame )
      {
        m_pNetGame->Finalize();
        GFL_SAFE_DELETE( m_pNetGame );
      }

      if( m_pP2pPacketTransferListener )
      {
        m_pP2pPacketTransferListener->OnPacketTransferSuccess( this );
      }

      m_Sequence = SEQUENCE_IDLE;
    }
    break;
  }

#elif defined( GF_PLATFORM_WIN32 )

  switch( m_Sequence )
  {
    case SEQUENCE_TERMINATE:
    {
      if( m_pP2pPacketTransferListener )
      {
        m_pP2pPacketTransferListener->OnPacketTransferSuccess( this );
      }

      m_Sequence = SEQUENCE_IDLE;
    }
    break;
  }

#endif

}


void P2pPacketTransfer::SendDataCommand( void* pSendData, u32 sendSize, u8 commandID )
{
#if defined( GF_PLATFORM_CTR )

  if( m_Sequence == SEQUENCE_IDLE )
  {
    m_SendInfo.pSendData = reinterpret_cast<u8*>( pSendData );
    m_SendInfo.sendSize  = sendSize;
    m_SendInfo.commandID = commandID;
    m_SendInfo.count     = 0;
    m_SendInfo.max       = 1;
  
    if( sendSize > PACKET_DATA_SIZE )
    {
      m_SendInfo.max = sendSize / PACKET_DATA_SIZE;
    
      if( sendSize % PACKET_DATA_SIZE > 0 )
      {
        m_SendInfo.max += 1;
      }
    }

    m_Sequence = SEQUENCE_INITIALIZE;
  }

#elif defined( GF_PLATFORM_WIN32 )

  if( m_Sequence == SEQUENCE_IDLE )
  {
    m_Sequence = SEQUENCE_TERMINATE;
  }
#endif

}


u32 P2pPacketTransfer::GetReceivedDataCount()
{
  return m_RecvPacketDatasList.Size();
}


bool P2pPacketTransfer::GetReceivedData( u32 index, u8 commandID, int& netID, void* pRecvData, u32 recvSize )
{
#if defined( GF_PLATFORM_CTR )

  bool bResult = false;

  netID = gflnet2::p2p::NetGameBase::INVALID_NET_ID;

  u32 count = 0;

  for( ItPacketDatasList packetDatas = m_RecvPacketDatasList.Begin(); packetDatas != m_RecvPacketDatasList.End(); ++packetDatas )
  {
    if( count == index )
    {
      ItPacketInfoList packetInfo = m_RecvPacketInfoList.Find( packetDatas->first );

      if( commandID == packetInfo->second.commandID && recvSize == packetInfo->second.sendSize )
      {
        u32 packetsSize = 0;
        
        for( ItPacketDatas packet = packetDatas->second->Begin(); packet != packetDatas->second->End(); ++packet )
        {
          packetsSize += packet->second.size;
        }

        if( recvSize == packetsSize )
        {
          u32 copyAddress = 0;
        
          u8* pTemp = reinterpret_cast<u8*>( pRecvData );

          for( ItPacketDatas packet = packetDatas->second->Begin(); packet != packetDatas->second->End(); ++packet )
          {
            gfl2::std::MemCopy( packet->second.data, &pTemp[ copyAddress ], packet->second.size );

            copyAddress += packet->second.size;
          }

          netID = packetDatas->first;

          bResult = true;
        }
      }

      if( !bResult )
      {
        GFL_PRINT( "-------------------------- P2pPacketTransfer --------------------------\n" );
        GFL_PRINT( "Error !! : Received data size = %d, Received command ID = %d, [commandID] = %d, [recvSize] = %d, \nFunction : P2pPacketTransfer::GetReceivedData \n", packetInfo->second.sendSize, packetInfo->second.commandID, commandID, recvSize );
      }

      break;
    }

    count++;
  }
  
  return bResult;

#elif defined( GF_PLATFORM_WIN32 )
  
  return false;

#endif

}


void P2pPacketTransfer::recvData()
{
#if defined( GF_PLATFORM_CTR )

  if( m_pNetGame )
  {
    int netID;
    u8 commandID;

    while( m_pNetGame->RecvDataCommand( &netID, reinterpret_cast<char*>( &m_Temp ), static_cast<int>( sizeof( Packet ) ), &commandID ) )
    {
      if( netID == m_pNetGame->GetNetID() )
      {
        continue;
      }

      switch( commandID )
      {
        case COMMAND_ID_SEND_PACKET_INFO:
        {
          ItPacketInfoList it = m_RecvPacketInfoList.Find( netID );
          if( it == m_RecvPacketInfoList.End() )
          {
            m_RecvPacketInfoList.Insert( gfl2::util::Pair<NetID, PacketInfo>( netID, m_Temp.info ) );
          }
        }
        break;

        case COMMAND_ID_SEND_PACKET_DATA:
        {
          ItPacketDatasList it = m_RecvPacketDatasList.Find( netID );
          if( it == m_RecvPacketDatasList.End() )
          {
            ItPacketInfoList itInfo = m_RecvPacketInfoList.Find( netID );
            if( itInfo != m_RecvPacketInfoList.End() )
            {
              PacketDatas* pPacketDatas = GFL_NEW( m_pHeap ) PacketDatas( m_pHeap, itInfo->second.max );
              pPacketDatas->Insert( gfl2::util::Pair<PacketID, PacketData>( m_Temp.data.packetID, m_Temp.data ) );
              m_RecvPacketDatasList.Insert( gfl2::util::Pair<NetID, PacketDatas*>( netID, pPacketDatas ) );
            }
          }
          else
          {
            it->second->Insert( gfl2::util::Pair<PacketID, PacketData>( m_Temp.data.packetID, m_Temp.data ) );
          }
        }
        break;
      }
    }
  }

#endif // defiend( GF_PLATFORM_CTR )
}


GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

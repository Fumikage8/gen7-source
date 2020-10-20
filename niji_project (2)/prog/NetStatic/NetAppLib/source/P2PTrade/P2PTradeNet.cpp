// ============================================================================
/*
 * @file P2PTradeNet.cpp
 * @brief P2P交換アプリで使用する通信モジュールです。
 * @date 2015.09.25
 * @author Akira endo
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeNet.h"
#include <util/include/gfl2_std_string.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


P2PTradeNet::P2PTradeNet( P2PTradeNetListener& listener ) :
  m_Listener( listener ),
#if defined( GF_PLATFORM_CTR )
  m_NetGame(),
#endif // defiend( GF_PLATFORM_CTR )
  m_TempDataSize(0),
  m_SendDataSize(0),
  m_TempCommandID(0),
  m_SendCommandID(0),
  m_TempBuffer(),
  m_SendBuffer(),
  m_RecvBuffer(),
  m_bInitialize( false )
{
}


P2PTradeNet::~P2PTradeNet()
{
}


bool P2PTradeNet::Initialize()
{
  bool bResult = true;
#if defined( GF_PLATFORM_CTR )
  if( !m_bInitialize )
  {
    m_NetGame.Initialize();
    m_bInitialize = true;
  }
  bResult = m_NetGame.IsInitialize();
#endif // defiend( GF_PLATFORM_CTR )
  return bResult;
}


void P2PTradeNet::Finalize()
{
#if defined( GF_PLATFORM_CTR )
  m_NetGame.Finalize();
#endif // defiend( GF_PLATFORM_CTR )
}


void P2PTradeNet::Update()
{
#if defined( GF_PLATFORM_CTR )
  // 送信処理
  {
    if( m_TempDataSize > 0 )
    {
      m_SendDataSize = m_TempDataSize;
      m_SendCommandID = m_TempCommandID;
      gfl2::std::MemCopy( m_TempBuffer, m_SendBuffer, sizeof( m_SendBuffer ) );
      m_TempDataSize = 0;
      m_TempCommandID = 0;
    }

    if( m_SendDataSize > 0 )
    {
      bool bResult = m_NetGame.SendDataCommand( reinterpret_cast<char*>( m_SendBuffer ), static_cast<int>( m_SendDataSize ), m_SendCommandID );
      
      if( bResult )
      {
        GFL_PRINT( "NetGame::SendDataCommand() : CommandID = %d \n", m_SendCommandID );

        m_SendDataSize = 0;
        m_SendCommandID = 0;
      }
    }
  }

  // 受信処理
  {
    int  netID;
    u8   commandID;

    while( m_NetGame.RecvDataCommand( &netID, reinterpret_cast<char*>( m_RecvBuffer ), static_cast<int>( sizeof( m_RecvBuffer ) ), &commandID ) )
    {
      if( netID == m_NetGame.GetNetID() )
      {
        continue;
      }

      GFL_PRINT( "NetGame::RecvDataCommand() : CommandID = %d \n", commandID );

      m_Listener.OnRecvDataCommand( m_RecvBuffer, commandID );
    }
  }
#endif // defiend( GF_PLATFORM_CTR )
}


bool P2PTradeNet::SendDataCommand( void* pSendData, u32 sendSize, u8 commandID )
{
  bool bResult = false;

  if( 0 < sendSize && sendSize < BUFFER_SIZE )
  {
    m_TempDataSize = sendSize;
    m_TempCommandID = commandID;
    gfl2::std::MemClear( m_TempBuffer, sizeof( m_TempBuffer ) );
    gfl2::std::MemCopy( pSendData, m_TempBuffer, sendSize );

    bResult = true;
  }

  return bResult;
}


bool P2PTradeNet::SecureSendDataCommand( void* pSendData, u32 sendSize, u8 commandID )
{
  bool bResult = false;
  
  if( m_TempDataSize == 0 && m_SendDataSize == 0 )
  {
    bResult = SendDataCommand( pSendData, sendSize, commandID );
  }
  
  return bResult;
}


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)


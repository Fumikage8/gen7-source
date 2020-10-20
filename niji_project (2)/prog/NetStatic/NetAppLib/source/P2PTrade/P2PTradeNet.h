// ============================================================================
/*
 * @file P2PTradeNet.h
 * @brief P2P交換アプリで使用する通信モジュールです。
 * @date 2015.09.25
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADENET_H_INCLUDE )
#define P2PTRADENET_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


class P2PTradeNetListener
{
public:
  virtual void OnRecvDataCommand( void* pRecvData, u8 commandID ){};
};


class P2PTradeNet
{

public:

  P2PTradeNet( P2PTradeNetListener& listener );
  virtual~P2PTradeNet();

  bool Initialize();
  void Finalize();
  void Update();

  bool SendDataCommand( void* pSendData, u32 sendSize, u8 commandID );
  bool SecureSendDataCommand( void* pSendData, u32 sendSize, u8 commandID );

#if defined( GF_PLATFORM_CTR )
  gflnet2::p2p::NetGame* GetNetGame(){ return &m_NetGame; }
#endif // defiend( GF_PLATFORM_CTR )

private:

  static const u32    BUFFER_SIZE = 1024;

  P2PTradeNetListener&      m_Listener;

#if defined( GF_PLATFORM_CTR )
  gflnet2::p2p::NetGame     m_NetGame;
#endif // defiend( GF_PLATFORM_CTR )

  u32                       m_TempDataSize;
  u32                       m_SendDataSize;
  u8                        m_TempCommandID;
  u8                        m_SendCommandID;
  u8                        m_TempBuffer[ BUFFER_SIZE ];
  u8                        m_SendBuffer[ BUFFER_SIZE ];
  u8                        m_RecvBuffer[ BUFFER_SIZE ];
  bool                      m_bInitialize;

};



GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADENET_H_INCLUDE

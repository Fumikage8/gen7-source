// ============================================================================
/*
 * @file P2pPacketTransfer.h
 * @brief 大きいサイズのデータを分割して送受信を行うクラスです。
 * @date 2015.08.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2P_PACKET_TRANSFER_H_INCLUDE )
#define P2P_PACKET_TRANSFER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_Map.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include "NetStatic/NetLib/include/P2P/P2pPacketTransferListener.h"



GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)


class P2pPacketTransfer
{
  GFL_FORBID_COPY_AND_ASSIGN( P2pPacketTransfer );

public:

  //---------------------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   * @param  pLowerHeap 内部でメモリを確保する為のヒープ
   * @param  pP2pPacketTransferListener 送受信の完了を受け取る為のリスナー
   *
   * @note   ※GetLowerHandle で取得したヒープを渡して使用してください
   */
  //---------------------------------------------------------------------------------
  P2pPacketTransfer( gfl2::heap::HeapBase* pLowerHeap, NetLib::P2P::P2pPacketTransferListener* pP2pPacketTransferListener );
  virtual~P2pPacketTransfer();

  void Update();

  //---------------------------------------------------------------------------------
  /**
   * @brief  コマンド付きデータ送信
   * @param  pSendData   送るデータ[ In ]
   * @param  sendSize    送信サイズ[ In ]
   * @param  commandID   コマンドID[ In ]
   *
   * @note   ※送信するデータは保持しておく必要があります
   */
  //---------------------------------------------------------------------------------
  void SendDataCommand( void* pSendData, u32 sendSize, u8 commandID );

  //---------------------------------------------------------------------------------
  /**
   * @brief  受信データの取得
   * @param  index       受信データの参照インデックス[ In ]
   * @param  commandID   コマンドID[ In ]
   * @param  netID       通信ID[ Out ]
   * @param  pRecvData   受信データ[ Out ]
   * @param  recvSize    受信データのバッファサイズ[ In ]
   * @retval true 取得に成功
   * @retval false 取得に失敗
   *
   * @note   ※GetReceivedDataCount で取得した数分 for ループで回して取得できます
   */
  //---------------------------------------------------------------------------------
  bool GetReceivedData( u32 index, u8 commandID, int& netID, void* pRecvData, u32 recvSize );

  u32 GetReceivedDataCount();

private:

  void clearList();
  void recvData();

  enum Sequence
  {
    SEQUENCE_IDLE,
    SEQUENCE_INITIALIZE,
    SEQUENCE_INITIALIZE_WAIT,
    SEQUENCE_SEND_PACKET_INFO,
    SEQUENCE_RECV_PACKET_INFO,
    SEQUENCE_SEND_PACKET_DATA,
    SEQUENCE_RECV_PACKET_DATA,
    SEQUENCE_TERMINATE
  };

  enum CommandID
  {
    COMMAND_ID_SEND_PACKET_INFO,
    COMMAND_ID_SEND_PACKET_DATA
  };

  static const u32 PACKET_DATA_SIZE = 2000;

  typedef struct
  {
    u8*       pSendData;
    u32       sendSize;
    u8        commandID;
    u8        count;
    u8        max;
  } SendInfo;

  typedef struct
  {
    u32 sendSize;
    u8  max;
    u8  commandID;
  } PacketInfo;

  typedef struct
  {
    u32 size;
    u8  packetID;
    u8  data[ PACKET_DATA_SIZE ];
  } PacketData;

  typedef union
  {
    struct
    {
      PacketInfo info;
    };
    struct
    {
      PacketData data;
    };
  } Packet;

  typedef u8 PacketID;
  typedef int NetID;

  typedef gfl2::util::Map<PacketID, PacketData>            PacketDatas;
  typedef gfl2::util::Map<PacketID, PacketData>::Iterator  ItPacketDatas;
  typedef gfl2::util::Map<NetID, PacketInfo>               PacketInfoList;
  typedef gfl2::util::Map<NetID, PacketInfo>::Iterator     ItPacketInfoList;
  typedef gfl2::util::Map<NetID, PacketDatas*>             PacketDatasList;
  typedef gfl2::util::Map<NetID, PacketDatas*>::Iterator   ItPacketDatasList;

#if defined( GF_PLATFORM_CTR )
  gflnet2::p2p::NetGame*                    m_pNetGame;
#endif // defiend( GF_PLATFORM_CTR )

  gfl2::heap::HeapBase*                     m_pHeap;
  NetLib::P2P::P2pPacketTransferListener*   m_pP2pPacketTransferListener;
  SendInfo                                  m_SendInfo;
  Packet                                    m_Temp;
  PacketInfoList                            m_RecvPacketInfoList;
  PacketDatasList                           m_RecvPacketDatasList;
  Sequence                                  m_Sequence;
  bool                                      m_bNetGameInitialized;

};



GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif // P2P_PACKET_TRANSFER_H_INCLUDE

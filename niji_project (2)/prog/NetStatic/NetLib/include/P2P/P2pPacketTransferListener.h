// ============================================================================
/*
 * @file P2pPacketTransferListener.h
 * @brief 大きいサイズのデータを分割して送受信を行うクラスのリスナーです。
 * @date 2015.08.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2P_PACKET_TRANSFER_LISTENER_H_INCLUDE )
#define P2P_PACKET_TRANSFER_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)


class P2pPacketTransfer;


class P2pPacketTransferListener
{
public:

  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer ) = 0;


  virtual ~P2pPacketTransferListener(){}
};



GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif // P2P_PACKET_TRANSFER_LISTENER_H_INCLUDE

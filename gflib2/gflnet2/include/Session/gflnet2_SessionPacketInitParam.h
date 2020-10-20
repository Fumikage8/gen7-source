//=============================================================================
/**
 * @file   gflnet2_SessionPacketInitParam.h
 * @brief  セッションパケット初期化パラメータ
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_SESSION_PACKET_INIT_PARAM_H__
#define __GFLNET2_SESSION_PACKET_INIT_PARAM_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)

enum
{
  MAX_TCP_MESSAGE_NUM = 10,
  MAX_UDP_MESSAGE_NUM = 10,
};

/***************************************************************************!
 * @brief  セッションパケット初期化パラメータ
 ***************************************************************************/
struct SessionPacketInitParam
{



public:

  gfl2::util::FixedSizeContainer<u16,MAX_TCP_MESSAGE_NUM> TCPMessageSizeArray;
  gfl2::util::FixedSizeContainer<u16,MAX_UDP_MESSAGE_NUM> UDPMessageSizeArray;
};


GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET2_SESSION_PACKET_INIT_PARAM_H__



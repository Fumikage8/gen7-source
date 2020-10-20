﻿#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_RECVDATALISTENER_H__
#define __GFLNET_RECVDATALISTENER_H__

//=============================================================================
/**
 * @file    gflnet_RecvDataListener.h
 * @brief   通信受信用クラス
 * @author  k.ohno
 * @note    
 */
//=============================================================================
#pragma once
#include <nn.h>
#include <nex.h>



#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2) 
GFL_NAMESPACE_BEGIN(p2p)

class RecvDataListener
{
public:

  //---------------------------------------------------------------------------------
  /**
   * @brief  レシーブ関数と同じ型のものをリスナーにして受け取れるようにする
   * @param  srcCID    送ってきた人の通信ID
   * @param  buffer    うけとったバッファ
   * @param  size      受信バッファサイズ
   */
  //---------------------------------------------------------------------------------
  virtual bool ReceiveCallback( nn::nex::ConnectionID srcCID, qByte* buffer, qUnsignedInt32 size ) = 0;

};


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_RECVDATALISTENER_H__
#endif

﻿#ifdef  GF_PLATFORM_CTR
#ifndef __GFL_NETPLAYER_H__
#define __GFL_NETPLAYER_H__
//=============================================================================
/**
 * @file	 gfl_NetPlayer.h
 * @brief	 通信プレーヤー情報を管理するクラス
 * @author k.ohno
 * @date	 2011.5.13
 */
//=============================================================================
#pragma once


#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  
GFL_NAMESPACE_BEGIN(p2p)

class NetPlayer {
  GFL_FORBID_COPY_AND_ASSIGN(NetPlayer);

private:
  bit64 mTransferedID;

public:
  //コンストラクタ
  NetPlayer(void);
  //デストラクタ
  virtual ~NetPlayer();


  //------------------------------------------------------------------
  /**
   * @brief	  transferedID記録
   * @param   transferedID
   */
  //------------------------------------------------------------------
  void SetTransferedID(const bit64 transferedID);
  //------------------------------------------------------------------
  /**
   * @brief	  transferedIDを得る
   * @return  transferedID
   */
  //------------------------------------------------------------------
  bit64 GetTransferedID(void);

};


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFL_NETPLAYER_H__
#endif

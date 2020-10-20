﻿#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_WIRELESS_H__
#define __GFLNET_WIRELESS_H__
//=============================================================================
/**
 * @file	 gflnet_Wireless.h
 * @brief	 無線通信 ローカル通信のクラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================
#pragma once


#include "gflnet2/include/base/gflnet2_Base.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  


class Wireless : public gflnet2::Base {
  GFL_FORBID_COPY_AND_ASSIGN(Wireless);

private:

  //      void* ReceiveBuffer;  //受信バッファは4096アライメントのデバイスバッファ


  //    nn::os::Event maStatusUpdateEvent;  //イベント


public:
  //------------------------------------------------------------------
  /**
   * @brief	  NetBaseコンストラクタ＆デストラクタ
   * @return
   */
  //------------------------------------------------------------------
  Wireless(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix);
  virtual ~Wireless();

  //  inline nn::os::Event* GetOsEvent(void){ return &maStatusUpdateEvent; };

  //    inline void* GetUdsReceiveBuffer(void){ return ReceiveBuffer; };

  //      inline u32 GetUdsReceiveBufferSize(void) const { return UDSRECEIVE_BUFFER; };


  //NetBaseからのバーチャル関数
public:
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンをセットする
   * @param   pData  ビーコンデータ
   * @param   size   ビーコンサイズ
   * @return  ビーコンをセットしたらTRUE
   */
  //------------------------------------------------------------------
  virtual b32 SetBeacon(const u8* pData, const u32 size);

  //------------------------------------------------------------------
  /**
   * @brief	  接続人数をかえす
   * @return  接続人数
   */
  //------------------------------------------------------------------
  virtual u8 GetConnectNum(void) const;
  //------------------------------------------------------------------
  /**
   * @brief	   ビーコンを返す   未実装
   * @param    index データ
   * @param    ビーコンのサイズ
   * @return   ビーコンデータ
   */
  //------------------------------------------------------------------
  virtual void* GetRecvBeacon(const int index,int size){ return NULL; };


};


GFL_NAMESPACE_END(gflnet2)


#endif // __GFLNET_WIRELESS_H__
#endif

﻿#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_IR_H__
#define __GFLNET_IR_H__
//=============================================================================
/**
 * @file	 gflnet_Ir.h
 * @brief	 無線通信 赤外線通信のクラス
 * @author k.ohno
 * @date	 2012.3.20
 */
//=============================================================================
#pragma once

#include "gflnet2/include/base/gflnet2_Base.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  


class Ir : public gflnet2::Base {
  GFL_FORBID_COPY_AND_ASSIGN(Ir);

private:

  static const int IRSEND_BUFFER = 4096;  //送信用バッファ
  void* mpSendBuffer;
  void* mpRecvBuffer;
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  NetBaseコンストラクタ＆デストラクタ
   * @return
   */
  //------------------------------------------------------------------
  Ir(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix);
  virtual ~Ir();



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
   * @brief	   ビーコンを返す
   * @param    index データ
   * @param    ビーコンのサイズ
   * @return   ビーコンデータ
   */
  //------------------------------------------------------------------
  virtual void* GetRecvBeacon(const int index, int size);
  //------------------------------------------------------------------
  /**
   * @brief	  接続人数をかえす
   * @return  接続人数
   */
  //------------------------------------------------------------------
  virtual u8 GetConnectNum(void) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief	  赤外線イベントの調査
   */
  //------------------------------------------------------------------
  void Eventcheck(void);

};


GFL_NAMESPACE_END(gflnet2)


#endif // __GFLNET_IR_H__
#endif

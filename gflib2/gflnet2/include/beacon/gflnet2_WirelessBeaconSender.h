#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_WIRELESSBEACONSENDER_H__
#define __GFLNET_WIRELESSBEACONSENDER_H__
//=============================================================================
/**
 * @file	 gflnet_WirelessBeaconSender.h
 * @brief	 無線通信 ローカル通信のクラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================
#pragma once


#include "gflnet2/include/beacon/gflnet2_BeaconSender.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
/**
* @namespace gflnet2::wireless
* @brief     ビーコンのラッパー baseに統一予定
*/
GFL_NAMESPACE_BEGIN(wireless)


class WirelessBeaconSender : public gflnet2::base::BeaconSender {
  GFL_FORBID_COPY_AND_ASSIGN(WirelessBeaconSender);

public:
  WirelessBeaconSender(void){};
  ~WirelessBeaconSender(){};
  //------------------------------------------------------------------
  /**
   * @brief  ビーコンセット関数
   * @param  pData   送るデータ
   * @param  size    サイズ
   * @return セットできたらTRUE
   */
  //------------------------------------------------------------------
  virtual bool SetBeacon(const void* pData, const u32 size);

};


GFL_NAMESPACE_END(wireless)
GFL_NAMESPACE_END(gflnet2)


#endif // __GFLNET_WIRELESSBEACONSENDER_H__
#endif

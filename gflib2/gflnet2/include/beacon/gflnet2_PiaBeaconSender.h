#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_PIABEACONSENDER_H__
#define __GFLNET_PIABEACONSENDER_H__
//=============================================================================
/**
 * @file	 gflnet_PiaBeaconSender.h
 * @brief	 Pia通信 ローカル通信のクラス
 * @author k.ohno
 * @date	 2012.5.29
 */
//=============================================================================
#pragma once



#include "gflnet2/include/beacon/gflnet2_BeaconSender.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(wireless)


class PiaBeaconSender : public gflnet2::base::BeaconSender {
  GFL_FORBID_COPY_AND_ASSIGN(PiaBeaconSender);

public:
  PiaBeaconSender(void){};
  ~PiaBeaconSender(){};
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


#endif // __GFLNET_PIABEACONSENDER_H__
#endif

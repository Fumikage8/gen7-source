#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_IRBEACONSENDER_H__
#define __GFLNET_IRBEACONSENDER_H__
//=============================================================================
/**
 * @file	 gflnet_IrBeaconSender.h
 * @brief	 無線通信 赤外線通信のクラス
 * @author k.ohno
 * @date	 2012.8.25
 */
//=============================================================================
#pragma once



#include "gflnet2/include/beacon/gflnet2_BeaconSender.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
/**
* @namespace gflnet2::ir
* @brief     削除予定
*/
GFL_NAMESPACE_BEGIN(ir)


class IrBeaconSender : public gflnet2::base::BeaconSender {
  GFL_FORBID_COPY_AND_ASSIGN(IrBeaconSender);

public:
  IrBeaconSender(void){};
  ~IrBeaconSender(){};
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


GFL_NAMESPACE_END(ir)
GFL_NAMESPACE_END(gflnet2)


#endif // __GFLNET_IRBEACONSENDER_H__
#endif

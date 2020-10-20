#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BEACONSENDER_H__
#define __GFLNET_BEACONSENDER_H__
//=============================================================================
/**
 * @file   gflnet_BeaconSender.h
 * @brief  ビーコンを送る関数をつくるためのクラス
 * @author k.ohno
 * @date   2012.5.13
 * @note  仮想関数を実装してください。
 */
//=============================================================================
#pragma once
#include <nn.h>



#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)

class BeaconSender
{
  //GFL_FORBID_COPY_AND_ASSIGN(BeaconListener); //コピーコンストラクタ＋代入禁止
public:

  virtual ~BeaconSender(){};
  //------------------------------------------------------------------
  /**
   * @brief  ビーコンセット関数
   * @param  pData   送るデータ
   * @param  size    サイズ
   * @return セットできたらTRUE
   */
  //------------------------------------------------------------------
  virtual bool SetBeacon(const void* pData, const u32 size)=0;
};


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_BEACONSENDER_H__
#endif

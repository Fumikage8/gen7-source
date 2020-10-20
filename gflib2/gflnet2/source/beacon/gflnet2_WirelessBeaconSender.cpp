#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_WirelessBeaconSender.cpp
 * @brief	 無線通信 ローカル通信のクラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <nn/uds.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <gflnet2/include/beacon/gflnet2_WirelessBeaconSender.h>
#include "gflnet2/include/scan/gflnet2_Wireless.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(wireless)


  //------------------------------------------------------------------
  /**
   * @brief  ビーコンセット関数
   * @param  pData   送るデータ
   * @param  size    サイズ
   * @return セットできたらTRUE
   */
  //------------------------------------------------------------------

bool WirelessBeaconSender::SetBeacon(const void* pData, const u32 size)
{
  nn::Result result;

  result = nn::uds::SetApplicationDataToBeacon( pData, sizeof(gflnet2::base::BeaconData));
  if(result.IsFailure()){
    if(result == nn::uds::ResultNotInitialized()){
//      //OHNO_PRINT("xxx初期化されていない\n");
    }
    if(result == nn::uds::ResultInvalidState()){
      //OHNO_PRINT("実行可能なステートではありません\n");
    }
    if(result == nn::uds::ResultTooLarge 	()){
      //OHNO_PRINT("サイズオーバー\n");
    }
    if(result == nn::uds::ResultWirelessOff()){
      //OHNO_PRINT("再度初期化\n");
    }
    return false;
  }
  return true;
}

GFL_NAMESPACE_END(wireless)
GFL_NAMESPACE_END(gflnet2)
#endif

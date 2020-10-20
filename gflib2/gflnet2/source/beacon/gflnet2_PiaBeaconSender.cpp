#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_PiaBeaconSender.cpp
 * @brief	 Pia通信 ローカル通信のクラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <nn/uds.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"
#include "gflnet2/include/beacon/gflnet2_PiaBeaconSender.h"

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

bool PiaBeaconSender::SetBeacon(const void* pData, const u32 size)
{
  nn::Result result;

  const u8* pDatac = (const u8*)pData;
  return gflnet2::dev::PiaAdapter::GetInstance()->SetDataToBeacon((const char*)pDatac, size);
}

GFL_NAMESPACE_END(wireless)
GFL_NAMESPACE_END(gflnet2)
#endif

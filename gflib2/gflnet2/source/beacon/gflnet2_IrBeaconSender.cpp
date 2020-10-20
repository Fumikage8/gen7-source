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


#include <gflnet2/include/beacon/gflnet2_IrBeaconSender.h>
#include <gflnet2/source/dev/gflnet2_IrAdapter.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ir)


//------------------------------------------------------------------
/**
 * @brief  ビーコンセット関数
 * @param  pData   送るデータ
 * @param  size    サイズ
 * @return セットできたらTRUE
 */
//------------------------------------------------------------------

bool IrBeaconSender::SetBeacon(const void* pData, const u32 size)
{
  nn::Result result;
  nn::uds::ConnectionStatus cs;

  const u8* pDatac = (const u8*)pData;

  if(gflnet2::dev::IrAdapter::GetInstance()){
    return gflnet2::dev::IrAdapter::GetInstance()->SetBeacon(pDatac, size);
  }
  //result = nn::uds::SetApplicationDataToBeacon( pData, sizeof(gflnet2::base::BeaconData));
  return false;
}

GFL_NAMESPACE_END(ir)
GFL_NAMESPACE_END(gflnet2)
#endif

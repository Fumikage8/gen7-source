#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_Wireless.cpp
 * @brief	 無線通信 ローカル通信のクラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <nn/uds.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "gflnet2/include/scan/gflnet2_Wireless.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------
/**
 * @brief	  NetWirelessコンストラクタ
 */
//------------------------------------------------------------------

Wireless::Wireless(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix)
     : Base(pIParam,pIParamFix)
{
}


Wireless::~Wireless()
{
}


//------------------------------------------------------------------
/**
 * @brief	  ビーコンをセットする
 * @param   pData  ビーコンデータ
 * @param   size   ビーコンサイズ
 * @return  成功したらtrue
 */
//------------------------------------------------------------------

b32 Wireless::SetBeacon(const u8* pData, const u32 size)
{
  GFL_ASSERT(0);  //この関数使用禁止  @fix
  return true;
}


//------------------------------------------------------------------
/**
 * @brief	  接続人数をかえす
 * @return  接続人数
 */
//------------------------------------------------------------------
u8 Wireless::GetConnectNum(void) const
{
  nn::Result result;
  nn::uds::CTR::ConnectionStatus cs;

  result = GetConnectionStatus( &cs );
  if(!result.IsFailure()){
    return cs.nowEntry;
  }
  return 0;
}

GFL_NAMESPACE_END(gflnet2)
#endif

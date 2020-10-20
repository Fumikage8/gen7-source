#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_Ir.cpp
 * @brief	 無線通信 赤外線通信のクラス
 * @author k.ohno
 * @date	 2012.3.20
 */
//=============================================================================

#include <nn/ir.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "gflnet2/include/scan/gflnet2_Ir.h"
#include "gflnet2/source/dev/gflnet2_IrAdapter.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------
/**
 * @brief	  Irコンストラクタ
 */
//------------------------------------------------------------------

Ir::Ir(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix)
     : Base(pIParam,pIParamFix)
     , mpSendBuffer(NULL)
     , mpRecvBuffer(NULL)
{
  
  
  gflnet2::dev::IrAdapter::CreateInstance(pIParam->pHeapBase,pIParam,pIParamFix);
}


Ir::~Ir()
{
  gflnet2::dev::IrAdapter::DestroyInstance();
}

//------------------------------------------------------------------
/**
 * @brief	  ビーコンをセットする
 * @param   pData  ビーコンデータ
 * @param   size   ビーコンサイズ
 * @return  成功したらtrue
 */
//------------------------------------------------------------------

b32 Ir::SetBeacon(const u8* pData, const u32 size)
{
  return gflnet2::dev::IrAdapter::GetInstance()->SetBeacon(pData,size);
}

//------------------------------------------------------------------
/**
 * @brief	  データ受信
 * @param   pData     受信データ
 * @param   bufferSize  受信データのサイズ
 * @return  受信完了=true
 */
//------------------------------------------------------------------

void* Ir::GetRecvBeacon(const int index, int size)
{
  return gflnet2::dev::IrAdapter::GetInstance()->GetBeacon(index,size);
}


//------------------------------------------------------------------
/**
 * @brief	  接続人数をかえす
 * @return  接続人数
 */
//------------------------------------------------------------------
u8 Ir::GetConnectNum(void) const
{
  return 0;  //実装していない
}


void Ir::Eventcheck(void)
{
  gflnet2::dev::IrAdapter::GetInstance()->Update();
}


GFL_NAMESPACE_END(gflnet2)
#endif

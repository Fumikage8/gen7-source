//=============================================================================
/**
 * @file	 gflnet_InitParam.cpp
 * @brief	 通信初期化構造体アクセサクラス
 * @author k.ohno
 * @date	 2012.3.21
 */
//=============================================================================
#include <gflnet2/include/base/gflnet2_InitParam.h>

#ifdef  GF_PLATFORM_CTR
#include <nn.h>
#include <nn/uds.h>
#include <nex_P2p.h>
#endif

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "gflnet2/include/base/gflnet2_Base.h"
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gflnet2)



//------------------------------------------------------------------
/**
 * @brief	  初期化
 * @param   pData  ビーコンデータ
 * @param   size   ビーコンサイズ
 * @return  成功したらtrue
 */
  //------------------------------------------------------------------

void InitParam::Initialize(const InitParameter* pIParam, const InitParameterFixed* pIParamFix)
{
  gfl2::std::MemCopy(pIParam, &mParam, sizeof(InitParameter));
  gfl2::std::MemCopy(pIParamFix, &mParamFixed, sizeof(InitParameterFixed));
	mIsMigration=false;
  mIsMigrationCallBackEnable=false;
#ifdef  GF_PLATFORM_CTR
  //LocalCommunicationId生成
  mLocalCommID = nn::uds::CreateLocalCommunicationId(GetProductID());
#endif // GF_PLATFORM_CTR

#if GFL_DEBUG
  //kujira専用のASSERT ガイドライン2.5.1.検査
  GFL_PRINT("InitParam::Initialize: ProductID[0x%x]\n",GetProductID());   //@fix
#endif

#if GFL_DEBUG
  mDebugMatchmakeNo = 0;  ///< デバッグ用：通信混線防止ID
#endif
}


//------------------------------------------------------------------
/**
 * @brief	  赤外線かどうか
 * @param   pData  ビーコンデータ
 * @param   size   ビーコンサイズ
 * @return  成功したらtrue
 */
  //------------------------------------------------------------------

bool InitParam::IsIr(void)
{
  if(TYPE_P2P_IR==GetType()){
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief	  Wifiかどうか
 * @param   pData  ビーコンデータ
 * @param   size   ビーコンサイズ
 * @return  成功したらtrue
 */
  //------------------------------------------------------------------

bool InitParam::IsWifi(void)
{
  if(TYPE_P2P_WIFI==GetType()){
    return true;
  }
  return false;
}



GFL_NAMESPACE_END(gflnet2)


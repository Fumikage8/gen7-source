#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_Base.cpp
 * @brief	 通信基底クラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "gflnet2/include/base/gflnet2_Base.h"


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)



//------------------------------------------------------------------
/**
 * @brief	  NetBaseコンストラクタ
 */
//------------------------------------------------------------------

Base::Base(const InitParameter* pIParam, const InitParameterFixed* pIParamFix)
{
  mInitParam.Initialize(pIParam,pIParamFix);
  mpBeaconAddress=NULL;  ///< ビーコンのデータのポインタ
  mBeaconSize=0;  ///< ビーコンデータサイズ
  mpRecvBeaconAddress=NULL;   ///< 受信用ビーコンアドレス
  mLocalCommunicationId = 0;
  bBeaconSetSuccess = false;
  ReceiveBuffer = NULL;
  mpBeaconManager = NULL;

  GFL_ASSERT(ReceiveBuffer==NULL);   //@fix
  ReceiveBuffer = GflHeapAllocMemoryAlign4096(mInitParam.GetHeapBase() , UDSRECEIVE_BUFFER);
}

Base::~Base()
{
  GflHeapFreeMemory(ReceiveBuffer);
  ReceiveBuffer=NULL;
}

//------------------------------------------------------------------
/**
 * @brief	  初期化パラメータクラスを得る
 * @return  初期化パラメータクラス
 */
  //------------------------------------------------------------------
InitParam* Base::GetInitParam(void)
{
  return &mInitParam;
}


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)
#endif

#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_GlobalAccess.cpp
 * @brief	 通信にてどうしても静的管理の方で解決しないと難しい物の為のクラス
           影響範囲を考えながら実装する事
           名前のセンスが悪いのは使わないようにして欲しいため
 * @author k.ohno
 * @date	 2012.1.27
 */
//=============================================================================


#include "gflnet2/include/base/gflnet2_GlobalAccess.h"

#if GFL_RO_ENABLE
//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(gflnet2::GlobalAccess)
template class gfl2::base::SingletonAccessor<gflnet2::GlobalAccess>;
#endif


GFL_NAMESPACE_BEGIN(gflnet2)

//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
GlobalAccess::GlobalAccess(void)
{
  mPauseBIT = 0;
#if GFL_DEBUG
  mbConnectWifi = false;
#endif
}

//------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------

GlobalAccess::~GlobalAccess()
{
}


GFL_NAMESPACE_END(gflnet2)

#endif

#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gfl_NetWirelessInitThread.cpp
 * @brief	 無線通信 初期管理クラス
 * @author k.ohno
 * @date	 2011.1.13
 */
//=============================================================================


#include <nn.h>
#include <nn/Result.h>
#include <nn/hid.h>
#include <nn/uds.h>

#include <base/include/gfl2_Singleton.h>

#include "gflnet2_WirelessInitThread.h"


GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------------------------

NetWirlessInitThread::NetWirlessInitThread( gflnet2::Base* pnetbase )
     : gflnet2::base::NetThread( pnetbase->GetHeapBase() )
{
  mpNetBase = pnetbase;
}

  
//------------------------------------------------------------------
/**
 * @brief	  通信初期化
 */
//------------------------------------------------------------------

void NetWirlessInitThread::Function( void )
{
  nn::Result result;
  
  result = nn::uds::Initialize( mpNetBase->GetConnectEvent(),
                                mpNetBase->GetReceiveBuffer(),
                                mpNetBase->GetReceiveBufferSize() );

  m_pThread->Kill();  //自分を終了させる
}




GFL_NAMESPACE_END(gflnet2)
#endif

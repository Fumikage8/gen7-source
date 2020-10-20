//==============================================================================
/**
 * @file	gflnet2_P2pConnectionInterface.cpp
 * @brief	P2Pプロセス：接続インターフェイス
 * @author	ichiraku_katsuhiko
 * @date	2015/04/17 17:43:34
 */
// =============================================================================
#ifdef  GF_PLATFORM_CTR

#include <nex.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>

#include "gflnet2/include/p2p/gflnet2_P2pConnectionInterface.h"

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

//------------------------------------------------------------------
/**
 * @brief   Nexの送受信機能を有効化
 */
//------------------------------------------------------------------
void P2pConnectionInterface::DirectStreamEnable( void )
{
  // nexの送受信機能を有効化
  nn::nex::DirectStream* directStream =gflnet2::dev::NetZAdapter::GetDirectStreamInstance();
  if( directStream )
  {
    directStream->Enable();
    GFL_PRINT("nex::DirectStream Enable\n");
  }
}

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // GF_PLATFORM_CTR
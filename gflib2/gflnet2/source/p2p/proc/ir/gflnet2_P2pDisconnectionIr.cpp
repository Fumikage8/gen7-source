#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pDisconnectionIr.cpp
 * @brief	P2Pプロセス：IR切断
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:25:57
 */
// =============================================================================

#include <gflnet2/source/dev/gflnet2_IrAdapter.h>

#include "gflnet2/source/p2p/proc/ir/gflnet2_P2pDisconnectionIr.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//---------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pDisconnectionIr::InitFunc(gfl2::proc::Manager* )
{
  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------------
/**
 * @brief   実行処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pDisconnectionIr::UpdateFunc(gfl2::proc::Manager*)
{
  gflnet2::dev::IrAdapter::GetInstance()->Disconnect();
  gflnet2::dev::IrAdapter::GetInstance()->Finalize();
  gflnet2::dev::IrAdapter::GetInstance()->DestroyInstance();

  SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------------
/**
 * @brief   終了処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pDisconnectionIr::EndFunc(gfl2::proc::Manager*)
{
  return gfl2::proc::RES_FINISH;
}

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR


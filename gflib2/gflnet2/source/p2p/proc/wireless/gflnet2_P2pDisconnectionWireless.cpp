#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pDisconnectionWireless.cpp
 * @brief	P2Pプロセス：Wireless切断
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:32:07
 */
// =============================================================================

#include <nex.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"
#include "gflnet2/source/p2p/gflnet2_P2pStaticManager.h"
#include "gflnet2/source/p2p/proc/wireless/gflnet2_P2pDisconnectionWireless.h"

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
gfl2::proc::Result P2pDisconnectionWireless::InitFunc(gfl2::proc::Manager* )
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
gfl2::proc::Result P2pDisconnectionWireless::UpdateFunc(gfl2::proc::Manager*)
{

  switch( GetSubSeq() ){
  case 0:
    {
      // Session離脱
      DisconnectSession();
      AddSubSeq();
    }
    break;

  case 1:
    {
      // Session終了チェック
      if( IsDisconnectSession() )
      {
        // Nexの送受信機能を無効化
        DirectStreamDisable();

        ICHI_PRINT("ICHI> CheckGameOver\n");
        SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
        return gfl2::proc::RES_FINISH;
      }
    }
    break;
  }

  return gfl2::proc::RES_CONTINUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief   終了処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pDisconnectionWireless::EndFunc(gfl2::proc::Manager*)
{
  switch( GetSubSeq() ){
  case 0:
    if(gflnet2::dev::PiaAdapter::GetInstance()){
      gflnet2::dev::PiaAdapter::GetInstance()->Terminate();
    }
    AddSubSeq();
    break;
  case 1:
    p2p::StaticManager::GetNetZ()->Terminate( &m_context );
    AddSubSeq();
    break;
  case 2:
    if( m_context.GetState() == nn::nex::CallContext::CallInProgress )
    {
      return gfl2::proc::RES_CONTINUE;
    }

    
    p2p::StaticManager::DestroyNetZ();
    gflnet2::dev::PiaAdapter::DestroyInstance();
    GFL_PRINT("P2pDisconnectionWireless EndFunc\n");
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}


GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR



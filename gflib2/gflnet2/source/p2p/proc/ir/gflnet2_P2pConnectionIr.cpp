#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pConnectionIr.cpp
 * @brief	P2Pプロセス：IR接続
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:22:13
 */
// =============================================================================

#include <nn/ir.h>
#include <gflnet2/source/dev/gflnet2_IrAdapter.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <gflnet2/source/p2p/gflnet2_NetGameBase.h>

#include "gflnet2/source/p2p/proc/ir/gflnet2_P2pConnectionIr.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
P2pConnectionIr::P2pConnectionIr( void )
{
}

//---------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pConnectionIr::InitFunc(gfl2::proc::Manager* )
{
  gflnet2::dev::IrAdapter::CreateInstance(  GetInitParam()->GetHeapBase(),
                                            GetInitParam()->GetInitParam(),
                                            GetInitParam()->GetInitParamFixed());

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------------
/**
 * @brief   実行処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pConnectionIr::UpdateFunc(gfl2::proc::Manager*)
{
  // 外からの終了リクエスト
  if( mbEnd )
  {
    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
    return gfl2::proc::RES_FINISH;
  }

  switch(GetSubSeq()){
  case 0:
    {
      if( gflnet2::dev::IrAdapter::GetInstance()->Initialize(GetInitParam()->GetHeapBase()) )
      {
        AddSubSeq();
      }
      else
      {
        // IRモジュール故障（エラーアプレットはInitialize内部でリクエスト済み）
        SetProcReturn( P2pManager::E_PROC_RETURN_ERROR );
        return gfl2::proc::RES_FINISH;
      }
    }
    break;

  case 1:
    {
      int ret = gflnet2::dev::IrAdapter::GetInstance()->ConnectNonBlock(GetInitParam()->GetGameID());

      if( ret == 1 )
      {// 成功
        if(gflnet2::dev::IrAdapter::GetInstance()->IsMaster()){
          gflnet2::dev::IrAdapter::GetInstance()->CreateMaster();
          NetGame::GetNetGameBase()->SetMaster();
        }
        else{
          gflnet2::dev::IrAdapter::GetInstance()->Matching(0);
          NetGame::GetNetGameBase()->SetMaster(false);
        }
        NetGame::GetNetGameBase()->GameStationEventCallbackIr(1);

        AddSubSeq();
      }
    }
    break;

  case 2:
    // 接続チェック
    if( gflnet2::dev::IrAdapter::GetInstance()->IsConnected() ){
      SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
      return gfl2::proc::RES_FINISH;
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
gfl2::proc::Result P2pConnectionIr::EndFunc(gfl2::proc::Manager*)
{
  return gfl2::proc::RES_FINISH;
}



GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR

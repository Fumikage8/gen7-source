#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pMainWireless.cpp
 * @brief	P2Pプロセス：Wirelessメイン
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:34:52
 */
// =============================================================================

#include <nex.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"

#include "gflnet2/source/p2p/proc/wireless/gflnet2_P2pMainWireless.h"

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
gfl2::proc::Result P2pMainWireless::InitFunc(gfl2::proc::Manager* )
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
gfl2::proc::Result P2pMainWireless::UpdateFunc(gfl2::proc::Manager*)
{
  // P2PManagerの中でエラーチェックはしない。
  // P2PManagerは指示を受けるのみ。（内部のエラーはもちろんチェックする）

  // 外からの終了リクエスト（切断以外ないはず）
  if( mbEnd )
  {
    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
    return gfl2::proc::RES_FINISH;
  }

  // session終了チェック
  {
    nn::nex::Session* pSession = nn::nex::Session::GetInstance();
    if( pSession )
    {
      if( pSession->CheckGameIsOver() )
      {
        ICHI_PRINT("ICHI> CheckGameOver\n");
        // P2P切断発生
        GetP2pManager()->SessionLeftOccur();
        // エラー処理へ
        SetProcReturn( P2pManager::E_PROC_RETURN_ERROR );
        return gfl2::proc::RES_FINISH;
      }
    }
  }

  // piaLocalエラーチェック
  if(gflnet2::dev::PiaAdapter::GetInstance())
  {
    gflnet2::dev::ERROR_CHECK_RESULT result = gflnet2::dev::PiaAdapter::GetInstance()->ErrorCheck();
    if( result != gflnet2::dev::RESULT_NONE )
    {//切断かエラー発生
      if( result ==  gflnet2::dev::RESULT_SESSION_LEFT )
      {//P2P切断
        // P2P切断発生
        GetP2pManager()->SessionLeftOccur();
      }
      else if( result == gflnet2::dev::RESULT_ERROR )
      {//エラー
        // エラー発生
        GetP2pManager()->ErrorOccur();
      }

      // エラー処理へ
      SetProcReturn( P2pManager::E_PROC_RETURN_ERROR );
      return gfl2::proc::RES_FINISH;
    }
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
gfl2::proc::Result P2pMainWireless::EndFunc(gfl2::proc::Manager*)
{
  return gfl2::proc::RES_FINISH;
}



GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR

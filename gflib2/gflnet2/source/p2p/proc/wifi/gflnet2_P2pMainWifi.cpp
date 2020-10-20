#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pMainWifi.cpp
 * @brief	P2Pプロセス：Wifiメイン
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:29:07
 */
// =============================================================================

#include <nex.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include "gflnet2/source/p2p/proc/wifi/gflnet2_P2pMainWifi.h"
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"

#if GFL_DEBUG
#include  <debug/include/gfl2_DebugLogWindow.h>
#endif

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
gfl2::proc::Result P2pMainWifi::InitFunc(gfl2::proc::Manager* )
{
  m_isKickMe = false;

  gflnet2::nex::NexMatchMakingClient* pNexMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  if( pNexMatchMakeClient )
  {
    pNexMatchMakeClient->SetMatchmakeListener( this );
  }

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------------
/**
 * @brief   実行処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pMainWifi::UpdateFunc(gfl2::proc::Manager*)
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
        OHNO_PRINT("OHNO> CheckGameOver\n");
        // P2P切断発生
        GetP2pManager()->SessionLeftOccur();
        // エラー処理へ
        SetProcReturn( P2pManager::E_PROC_RETURN_ERROR );
        return gfl2::proc::RES_FINISH;
      }
    }
  }

  // アクセスポイント切断
  if( !nn::ac::CTR::IsConnected() )
  {
    nn::nex::Station *pStasion = gflnet2::dev::NetZAdapter::GetStationInstance();
    if( pStasion != NULL && !m_isKickMe )
    {
  // GFBTS[2657] KickMe失敗時にもう一度送るように書いた
  // 検証を行った際 falseが返ってくることは観測できなかった
  // なので現状ではOFFの方が適切と思われます。  k.ohno
/*
      if(pStasion->KickMe()){
        m_isKickMe=true;
      }
      GFL_PRINT("!!!KickMe2!!!\n");
*/
      pStasion->KickMe();
      m_isKickMe = true;
      GFL_PRINT("!!!KickMe1!!!\n");
    }
  }

  // P2Pセッションが孤立状態になってないかチェック
  // 典型的には、自身がセッションマスターではないときに自分自身の接続状況が悪くてP2Pセッションから切断された場合に孤立状態になります
  // P2P通信中(P2Pセッション確立後、NetZ::Terminate()まで)のみチェックします
  if( GFL_SINGLETON_INSTANCE( gflnet2::nex::NexMatchMakingClient ) )
  {
    bool isOrphan = GFL_SINGLETON_INSTANCE( gflnet2::nex::NexMatchMakingClient )->SessionHostIsOrphan();
    if( isOrphan )
    {
      GFL_PRINT("I'm an orphan.\n");
      // P2P切断発生
      GetP2pManager()->SessionLeftOccur();
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
gfl2::proc::Result P2pMainWifi::EndFunc(gfl2::proc::Manager*)
{
  gflnet2::nex::NexMatchMakingClient* pNexMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  if( pNexMatchMakeClient )
  {
    pNexMatchMakeClient->SetMatchmakeListener( NULL );
  }

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
* @brief マッチメイクセッションのエラー終了通知
*
* @param[in] result エラー理由
* @param[in] result エラーアプレットに渡されるエラーコード
*
* @par  下記関数のエラー時に発行される。
*       - NexMatchMakingClient::CreateSession()
*       - NexMatchMakingClient::SearchSession()
*       - NexMatchMakingClient::AutoMatchmake()
*
* @note 以下のエラーをハンドリングする必要がある
*        QERROR(RendezVous, SessionVoid) ギャザリングが存在しない。
*        QERROR(RendezVous, SessionFull) すでに最大参加人数に達している。
*        QERROR(RendezVous, SessionClosed) ギャザリングが締め切られている。
*/
//------------------------------------------------------------------
void P2pMainWifi::OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode )
{
  //このタイミングでは発生しないはず
  GFL_ASSERT(0);//@fix
  GetP2pManager()->SessionLeftOccur();
}

//------------------------------------------------------------------
/**
* @brief マッチメイクセッションのオーナー変更通知
*
* @param[in] newOwnerId 新たにオーナーとなったユーザーのプリンシバルID
*/
//------------------------------------------------------------------
void P2pMainWifi::OnMatchmakingOwnerChange( nn::nex::PrincipalID newOwnerId )
{
  GFL_PRINT("P2pMainWifi::OnMatchmakingOwnerChange\n");
  gflnet2::friends::PrincipalId pid = gflnet2::friends::FriendManager::GetMyPrincipalId();
  if( newOwnerId == pid )
  {
    GFL_PRINT("P2pMainWifi::OnMatchmakingOwnerChange new owner is me\n");
    NetGame::GetNetGameBase()->SetMaster(true);

#if GFL_DEBUG
    gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"I'm P2P Owner\n");
    GFL_RELEASE_PRINT("I'm P2P Owner\n");
#endif
  }
//  NetGame::GetNetGameBase()->SetDummySend();
  
  GetP2pManager()->OnMigrationSuccess();
}


GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR


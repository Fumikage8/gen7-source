#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pDisconnectionWifi.cpp
 * @brief	P2Pプロセス：Wifi切断
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:30:39
 */
// =============================================================================

#include <nex.h>

#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"
#include "gflnet2/source/p2p/proc/wifi/gflnet2_P2pDisconnectionWifi.h"

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
gfl2::proc::Result P2pDisconnectionWifi::InitFunc(gfl2::proc::Manager* )
{
  m_nexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  m_nexMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  m_isOnMatchmakingEndSuccess = false;

  if( m_nexMatchMakeClient )
  {
    m_nexMatchMakeClient->SetMatchmakeListener( this );
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
gfl2::proc::Result P2pDisconnectionWifi::UpdateFunc(gfl2::proc::Manager*)
{
  //-------------------------------------
  /// シーケンス定義
  //=====================================
  enum{
    SEQ_DISCONNECT_SESSION = 0,   ///< P2Pセッション切断
    SEQ_DISCONNECT_SESSION_WAIT,  ///< P2Pセッション切断待ち
    SEQ_CANCEL_MATCHMAKE,         ///< マッチメイクのキャンセルリクエスト
    SEQ_CANCEL_MATCHMAKE_WAIT,    ///< マッチメイクのキャンセルリクエスト待ち
    SEQ_END_MATCHMAKE,            ///< マッチメイクの切断
    SEQ_END_MATCHMAKE_WAIT,       ///< マッチメイクの切断待ち
    SEQ_DELETE_MATCHMAKE,         ///< マッチメイクの削除
    SEQ_DELETE_NETZ,              ///< NetZの削除
    SEQ_END_NAT_TRAVERSAL,        ///< NATトラバーサルの終了
    SEQ_UNBIND_NAT_TRAVERSAL,     ///< NATトラバーサルのUnbind
    SEQ_END                       ///< 終了
  };

#if GFL_DEBUG
  // シーケンスが変わったら出力
  {
    static int debugSeq;
    if( debugSeq != GetSubSeq() )
    {
      debugSeq = GetSubSeq();
      ICHI_PRINT("P2pDisonnectionWifi::UpdateFunc Seq:%d\n", debugSeq );
    }
  }
#endif // GFL_DEBUG

  switch( GetSubSeq() ){
  case SEQ_DISCONNECT_SESSION:       ///< P2Pセッション切断
    {
      // Session離脱
      DisconnectSession();
      SetSubSeq( SEQ_DISCONNECT_SESSION_WAIT );
    }
    break;
  case SEQ_DISCONNECT_SESSION_WAIT:  ///< P2Pセッション切断待ち
    {
      // Session終了チェック
      if( IsDisconnectSession() )
      {
        // Nexの送受信機能を無効化
        DirectStreamDisable();

        SetSubSeq( SEQ_CANCEL_MATCHMAKE );
      }
    }
    break;

  case SEQ_CANCEL_MATCHMAKE:         ///< マッチメイクのキャンセルリクエスト
    {
      CancelMatchmake();
      SetSubSeq( SEQ_CANCEL_MATCHMAKE_WAIT );
    }
    break;
  case SEQ_CANCEL_MATCHMAKE_WAIT:    ///< マッチメイクのキャンセルリクエスト待ち
    {
      if( !IsCancelMatchmake() )
      {
        SetSubSeq( SEQ_END_MATCHMAKE );
      }
    }
    break;

  case SEQ_END_MATCHMAKE:         ///< マッチメイクの切断
    {
      EndMatchmake();
      SetSubSeq( SEQ_END_MATCHMAKE_WAIT );
    }
    break;
  case SEQ_END_MATCHMAKE_WAIT:    ///< マッチメイクの切断待ち
    {
      if( IsEndMatchmake() )
      {
        SetSubSeq( SEQ_DELETE_MATCHMAKE );
      }
    }
    break;
  case SEQ_DELETE_MATCHMAKE:         ///< マッチメイクの削除
    {
      DeleteMatchmake();
      SetSubSeq( SEQ_DELETE_NETZ );
    }
    break;
  case SEQ_DELETE_NETZ:              ///< NetZの削除
    {
      DeleteNetZ();
      SetSubSeq( SEQ_END_NAT_TRAVERSAL );
    }
    break;
  case SEQ_END_NAT_TRAVERSAL:        ///< NATトラバーサルの終了
    {
      EndNatTraversal();
      SetSubSeq( SEQ_UNBIND_NAT_TRAVERSAL );
    }
    break;
  case SEQ_UNBIND_NAT_TRAVERSAL:     ///< NATトラバーサルのUnbind
    {
      UnbindNatTraversal();
      SetSubSeq( SEQ_END );
    }
    break;
  case SEQ_END:                      ///< 終了
    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
    return gfl2::proc::RES_FINISH;
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
gfl2::proc::Result P2pDisconnectionWifi::EndFunc(gfl2::proc::Manager*)
{
  if( m_nexMatchMakeClient )
  {
    m_nexMatchMakeClient->SetMatchmakeListener( NULL );
  }
    
  GFL_PRINT("P2pDisconnectionWifi EndFunc\n");
  return gfl2::proc::RES_FINISH;
}

// NexMatchmakeListenerの実装
//------------------------------------------------------------------
/**
* @brief マッチメイクセッションの正常終了通知。
*        自身がマッチメイクセッションから正常離脱した旨を検知できる。
*/
//------------------------------------------------------------------
void P2pDisconnectionWifi::OnMatchmakingEndSuccess( void )
{
  GFL_PRINT("P2pDisconnectionWifi::OnMatchmakingEndSuccess\n");
  m_isOnMatchmakingEndSuccess = true;
}

//------------------------------------------------------------------
/**
* @brief マッチメイクセッションのエラー終了通知
*
* @param[in] result エラー理由
* @param[in] result エラーアプレットに渡されるエラーコード
*
* @par  下記関数のエラー時に発行される。
*       - NexMatchMakingClient::DeleteParticipationAsync()
*       - NexMatchMakingClient::EndParticipationAsync()
*/
//------------------------------------------------------------------
void P2pDisconnectionWifi::OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("P2pDisconnectionWifi::OnNexMatchmakeError %d\n", errorCode );
  m_isOnMatchmakingEndSuccess = true;
}


//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
* @brief   NATトラバーサルのUnbind
*/
//------------------------------------------------------------------
void P2pDisconnectionWifi::UnbindNatTraversal( void )
{
  if( m_nexManager )
  {
    m_nexManager->UnbindNATTraversalClient();
  }
  GFL_PRINT("UnbindNatTraversal() Success\n");
}

//------------------------------------------------------------------
/**
* @brief   NATトラバーサルの終了
* @return  実行に成功したならtrueが返却される
*/
//------------------------------------------------------------------
bool P2pDisconnectionWifi::EndNatTraversal( void )
{
  if( m_nexManager )
  {
    if( !m_nexManager->StopNATSession() )
    {
      return false;
    }
  }

  nn::nex::RootTransport::SetEnabledRelay( false );

  GFL_PRINT("EndNatTraversal() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
* @brief   NetZの削除
* @return  実行に成功したならtrueが返却される
*/
//------------------------------------------------------------------
bool P2pDisconnectionWifi::DeleteNetZ( void )
{
  if( m_nexManager )
  {
    if( !m_nexManager->DeleteNetZ() )
    {
      return false;
    }
  }

  GFL_PRINT("DeleteNetZ() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクのキャンセルリクエスト発行
 */
//------------------------------------------------------------------
void P2pDisconnectionWifi::CancelMatchmake( void )
{
  if( m_nexMatchMakeClient )
  {
    if( GetInitParam()->IsMigrationPowerOff() == false )
    {
      m_nexMatchMakeClient->CancelConnecting();
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクのキャンセル待ち
 * @reutrn  通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool P2pDisconnectionWifi::IsCancelMatchmake( void )
{
  if( GetInitParam()->IsMigrationPowerOff() )
  {
    return false;
  }
  if( m_nexMatchMakeClient )
  {
    return m_nexMatchMakeClient->IsConnecting();
  }
  return false;
}

//------------------------------------------------------------------
/**
* @brief   マッチメイクの終了
*/
//------------------------------------------------------------------
void P2pDisconnectionWifi::EndMatchmake( void )
{
  if( !nn::ac::CTR::IsConnected() )
  {
    if( nn::nex::Core::GetInstance() )
    {
      // 強制終了モードを指定
      nn::nex::Core::GetInstance()->SetTerminateImmediately( true );
    }
  }
  if( GetInitParam()->IsMigrationPowerOff() )
  {
    m_nexMatchMakeClient->EndParticipationAsync();    // 子の場合マッチメイク離脱
    return;
  }

  
  if( m_nexMatchMakeClient && m_nexMatchMakeClient->GetMatchmakeExtensionClient() )
  {
    GFL_PRINT("EndMatchmake() Call\n");
    m_nexMatchMakeClient->DeleteParticipationAsync(); // 親の場合マッチメイク削除
    m_nexMatchMakeClient->EndParticipationAsync();    // 子の場合マッチメイク離脱
  }
  else
  {//未作成
    GFL_PRINT("EndMatchmake() Skip\n");
    m_isOnMatchmakingEndSuccess = true;
  }

  
}

//------------------------------------------------------------------
/**
* @brief   マッチメイクの終了完了待ち
*/
//------------------------------------------------------------------
bool P2pDisconnectionWifi::IsEndMatchmake( void )
{
  if( GetInitParam()->IsMigrationPowerOff() )
  {
    return true;
  }

  return m_isOnMatchmakingEndSuccess;
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクの削除
 */
//------------------------------------------------------------------
void P2pDisconnectionWifi::DeleteMatchmake( void )
{
  if( m_nexMatchMakeClient )
  {
    m_nexMatchMakeClient->Finalize();
  }
}


GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR


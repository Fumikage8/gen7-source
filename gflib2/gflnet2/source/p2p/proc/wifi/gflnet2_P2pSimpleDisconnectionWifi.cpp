#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pSimpleDisconnectionWifi.cpp
 * @brief	P2Pプロセス：WIfi切断（シンプルマッチメイク版）ミラクル交換専用
 * @author	ichiraku_katsuhiko
 * @date	2016/01/20　23:07
 */
// =============================================================================

#include <nex.h>

#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"
#include "gflnet2/source/p2p/proc/wifi/gflnet2_P2pSimpleDisconnectionWifi.h"

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
gfl2::proc::Result P2pSimpleDisconnectionWifi::InitFunc(gfl2::proc::Manager* )
{
  m_nexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  m_nexMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------------
/**
 * @brief   実行処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pSimpleDisconnectionWifi::UpdateFunc(gfl2::proc::Manager*)
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
gfl2::proc::Result P2pSimpleDisconnectionWifi::EndFunc(gfl2::proc::Manager*)
{
  GFL_PRINT("P2pSimpleDisconnectionWifi EndFunc\n");
  return gfl2::proc::RES_FINISH;
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
void P2pSimpleDisconnectionWifi::UnbindNatTraversal( void )
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
bool P2pSimpleDisconnectionWifi::EndNatTraversal( void )
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
bool P2pSimpleDisconnectionWifi::DeleteNetZ( void )
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
void P2pSimpleDisconnectionWifi::CancelMatchmake( void )
{
  if( m_nexMatchMakeClient )
  {
    m_nexMatchMakeClient->CancelConnecting();
  }
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクのキャンセル待ち
 * @reutrn  通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool P2pSimpleDisconnectionWifi::IsCancelMatchmake( void )
{
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
void P2pSimpleDisconnectionWifi::EndMatchmake( void )
{
  if( !nn::ac::CTR::IsConnected() )
  {
    if( nn::nex::Core::GetInstance() )
    {
      // 強制終了モードを指定
      nn::nex::Core::GetInstance()->SetTerminateImmediately( true );
    }
  }

  GFL_PRINT("EndMatchmake() Call\n");
}

//------------------------------------------------------------------
/**
* @brief   マッチメイクの終了完了待ち
*/
//------------------------------------------------------------------
bool P2pSimpleDisconnectionWifi::IsEndMatchmake( void )
{
  // マッチメイクがない。
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクの削除
 */
//------------------------------------------------------------------
void P2pSimpleDisconnectionWifi::DeleteMatchmake( void )
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


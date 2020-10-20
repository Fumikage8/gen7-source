#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pSimpleConnectionWifi.cpp
 * @brief	P2Pプロセス：WIfi接続（シンプルマッチメイク版）ミラクル交換専用
 * @author	ichiraku_katsuhiko
 * @date	2016/01/20　23:05
 */
// =============================================================================

#include <nex.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include "gflnet2/include/p2p/gflnet2_NetGame.h"
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include "gflnet2/source/p2p/proc/wifi/gflnet2_P2pSimpleConnectionWifi.h"

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
gfl2::proc::Result P2pSimpleConnectionWifi::InitFunc(gfl2::proc::Manager* )
{
  m_nexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  m_nexMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  m_isOnMatchmakingSimpleMatchmakeSuccess = false;
  m_isOnMatchmakingEntrySimpleMatchmakeSuccess = false;
  m_isOnMatchmakingCancelSimpleMatchmakeSuccess = false;
  m_isMatchMakeFailed = false;
  m_isCancelSimpleMatchmake = false;

  m_nexMatchMakeClient->SetMatchmakeListener( this );

  gflnet2::p2p::NetGame::GetNetGameBase()->ConnectSettingWifi();

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------------
/**
 * @brief   実行処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pSimpleConnectionWifi::UpdateFunc(gfl2::proc::Manager*)
{
  //-------------------------------------
  /// シーケンス定義
  //=====================================
  enum{
    SEQ_CREATE_MATCHMAKE,     ///< MatchmakingClient初期化

    SEQ_SIMPLE_MATCHMAKE,     ///< シンプルマッチメイク開始
    SEQ_SIMPLE_MATCHMAKE_WAIT,///< シンプルマッチメイク待ち
    // 
    SEQ_BIND_NAT_TRAVERSAL,   ///< NATトラバーサルのBind
    SEQ_START_NAT_TRAVERSAL,  ///< NATトラバーサルの開始
    SEQ_WAIT_NAT_TRAVERSAL,   ///< NATトラバーサルの開始待ち
    SEQ_SET_P2PSESSION,       ///< P2Pセッションキーのセット
    SEQ_CREATE_NETZ,          ///< NetZの作成
    SEQ_DIRECT_STREAM_ENABLE, ///< P2P送信許可
    // 
    SEQ_CREATE_SESSION,        ///< P2Pセッション作成（親）
    SEQ_SIMPLE_MATCHMAKE_ENTRY,///< シンプルマッチメイクエントリー（親）
    SEQ_SIMPLE_MATCHMAKE_ENTRY_WAIT,///< シンプルマッチメイクエントリー待ち（親）
    // 
    SEQ_JOIN_SESSION,         ///< P2Pセッションに参加する（子）
    SEQ_WAIT_JOIN_SESSION,    ///< P2Pセッションに参加待ち（子）
    //
    //
    SEQ_WAIT_CONNECTNUM,      ///< 接続人数待ち
    //
  };

#if GFL_DEBUG
  // シーケンスが変わったら出力
  {
    static int debugSeq;
    if( debugSeq != GetSubSeq() )
    {
      debugSeq = GetSubSeq();
      ICHI_PRINT("P2pSimpleConnectionWifi::UpdateFunc Seq:%d\n", debugSeq );
    }
  }
#endif // GFL_DEBUG

  // 外からの終了リクエスト
  if( mbEnd )
  {
    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );

    // マッチメイクのエントリーをしていた場合は、解除する必要がある。
    if( m_isOnMatchmakingEntrySimpleMatchmakeSuccess )
    {
      m_isCancelSimpleMatchmake = true;
      ResetSubSeq();
      ICHI_PRINT(">ICHI シンプルマッチメイクキャンセルへ\n");
    }
    return gfl2::proc::RES_FINISH;
  }

  // エラー発生フラグ
  bool isError = false;

  switch( GetSubSeq() ){
  case SEQ_CREATE_MATCHMAKE:     ///< MatchmakingClient初期化
    {
      isError = !CreateMatchmake();
      SetSubSeq( SEQ_SIMPLE_MATCHMAKE );
    }
    break;

  case SEQ_SIMPLE_MATCHMAKE:     ///< シンプルマッチメイク開始
    {
      isError = !SimpleMatchmake();
      SetSubSeq( SEQ_SIMPLE_MATCHMAKE_WAIT );
    }
    break;

  case SEQ_SIMPLE_MATCHMAKE_WAIT:///< シンプルマッチメイク待ち
    {
      if( m_isOnMatchmakingSimpleMatchmakeSuccess == true )
      {
        SetSubSeq( SEQ_BIND_NAT_TRAVERSAL );
      }
    }
    break;

  case SEQ_BIND_NAT_TRAVERSAL:   ///< NATトラバーサルのBind
    {
      isError = !BindNatTraversal();
      SetSubSeq( SEQ_START_NAT_TRAVERSAL );
    }
    break;
  case SEQ_START_NAT_TRAVERSAL:  ///< NATトラバーサルの開始
    {
      isError = !StartNatTraversal();
      SetSubSeq( SEQ_WAIT_NAT_TRAVERSAL );
    }
    break;
  case SEQ_WAIT_NAT_TRAVERSAL:   ///< NATトラバーサルの開始待ち
    {
      E_STATUS status = WaitNatTraversal();

      switch( status ){
      case E_STATUS_FAILURE: isError = true; break;
      case E_STATUS_SUCCESS: SetSubSeq( SEQ_SET_P2PSESSION ); break;
      }
    }
    break;

  case SEQ_SET_P2PSESSION:       ///< P2Pセッションキーのセット
    {
      // ↓サンプルより引用
      //
      // サーバーで生成されたP2P通信用の暗号鍵をセット
      // AutoMatchmake のサンプルと呼び出しタイミングが異なりますが、
      // 中継サーバ機能を有効にする場合、nn::nex::ConnectivityManager::StartNATSession の後に暗号鍵をセットする必要がある為、
      // こちらに移動させています。
      m_nexMatchMakeClient->SetSimpleMatchmakeP2pSessionKey();
      SetSubSeq( SEQ_CREATE_NETZ );
    }
    /* break through */

  case SEQ_CREATE_NETZ:          ///< NetZの作成
    {
      isError = !CreateNetZ();
      SetSubSeq( SEQ_DIRECT_STREAM_ENABLE );
    }
    /* break through */

  case SEQ_DIRECT_STREAM_ENABLE: ///< P2P送信許可
    {
      // nexの送受信機能を有効化
      DirectStreamEnable();

      // 親子フラグ
      NetGame::GetNetGameBase()->SetMaster( m_nexMatchMakeClient->IsOwner() );
      if( m_nexMatchMakeClient->IsOwner() )
      {
        GFL_PRINT("親\n");
        SetSubSeq( SEQ_CREATE_SESSION ); // 親
      }
      else
      {
        GFL_PRINT("子\n");
        SetSubSeq( SEQ_JOIN_SESSION ); // 子
      }
    }
    break;

  case SEQ_CREATE_SESSION:        ///< P2Pセッション作成（親）
    {
      isError = !CreateSession();
      SetSubSeq( SEQ_SIMPLE_MATCHMAKE_ENTRY );
    }
    break;

  case SEQ_SIMPLE_MATCHMAKE_ENTRY:///< シンプルマッチメイクエントリー（親）
    {
      isError = !EntrySimpleMatchmake();
      SetSubSeq( SEQ_SIMPLE_MATCHMAKE_ENTRY_WAIT );
    }
    break;

  case SEQ_SIMPLE_MATCHMAKE_ENTRY_WAIT:///< シンプルマッチメイクエントリー待ち（親）
    {
      if( m_isOnMatchmakingEntrySimpleMatchmakeSuccess == true )
      {
        SetSubSeq( SEQ_WAIT_CONNECTNUM );
      }
    }
    break;

  case SEQ_JOIN_SESSION:         ///< P2Pセッションに参加する（子）
    {
      isError = !JoinSession();
      SetSubSeq( SEQ_WAIT_JOIN_SESSION );
    }
    break;
  case SEQ_WAIT_JOIN_SESSION:    ///< P2Pセッションに参加待ち（子）
    {
      E_STATUS status = IsJoinSession();

      switch( status ){
      case E_STATUS_FAILURE: isError = true; break;
      case E_STATUS_SUCCESS: SetSubSeq( SEQ_WAIT_CONNECTNUM ); break;
      }
    }
    break;

  case SEQ_WAIT_CONNECTNUM:      ///< 接続人数待ち
    {
      // 接続人数
      int connectMax = GetInitParam()->GetLimitNum();
      if( connectMax == NetGame::GetNetGameBase()->GetConnectionCount() )
      {
        ICHI_PRINT("P2pSimpleConnectionWifi::UpdateFunc 接続人数 %d \n", connectMax );
        SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
        return gfl2::proc::RES_FINISH;
      }
    }
    break;
  
  default:
    GFL_ASSERT(0); // 上でタイムアウト
    break;
  }

  // エラー発生フラグチェック
  if( isError || m_isMatchMakeFailed )
  {
    ICHI_PRINT("%d %d \n",isError,m_isMatchMakeFailed);
    GFL_PRINT("P2pSimpleConnectionWifi::UpdateFunc Error SubSeq[%d]\n", GetSubSeq());
    SetProcReturn( P2pManager::E_PROC_RETURN_ERROR );
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
gfl2::proc::Result P2pSimpleConnectionWifi::EndFunc(gfl2::proc::Manager*)
{
  //-------------------------------------
  /// シーケンス定義
  //=====================================
  enum{
    // P2Pキャンセル
    SEQ_SIMPLE_MATCHMAKE_CANCEL,///< シンプルマッチメイクエントリーキャンセル（親）
    SEQ_SIMPLE_MATCHMAKE_CANCEL_WAIT,///< シンプルマッチメイクエントリーキャンセル待ち（親）
    SEQ_SIMPLE_MATCHMAKE_CANCEL_END,
  };

  // 親でEntrySimpleMatchmakeしていた場合は、Cancelする必要がある
  if( m_isCancelSimpleMatchmake )
  {
    switch( GetSubSeq() ){
    case SEQ_SIMPLE_MATCHMAKE_CANCEL:///< シンプルマッチメイクエントリーキャンセル（親）
      {
        if( CancelSimpleMatchmakeEntry() )
        {
          SetSubSeq( SEQ_SIMPLE_MATCHMAKE_CANCEL_WAIT );
        }
        else
        {//リクエスト発行に失敗したら終わる
          SetSubSeq( SEQ_SIMPLE_MATCHMAKE_CANCEL_END );
        }
      }
      return gfl2::proc::RES_CONTINUE;

    case SEQ_SIMPLE_MATCHMAKE_CANCEL_WAIT:///< シンプルマッチメイクエントリーキャンセル待ち（親）
      {
        // @fix GFNMCat[857]：キャンセルAPI中にエラーになるとはまる対処
        if( m_isOnMatchmakingCancelSimpleMatchmakeSuccess || m_isMatchmakeError )
        {
          SetSubSeq( SEQ_SIMPLE_MATCHMAKE_CANCEL_END );
        }
      }
      return gfl2::proc::RES_CONTINUE;

    case SEQ_SIMPLE_MATCHMAKE_CANCEL_END:
      break;
    }
  }


  if( m_nexMatchMakeClient )
  {
    m_nexMatchMakeClient->SetMatchmakeListener( NULL );
  }

  // 実行中だったらキャンセルする
  if( m_context.GetState() == nn::nex::CallContext::CallInProgress )
  {
    // エラー以外はないはず
    m_context.Cancel();
    GFL_PRINT("m_context Cancel待ち\n");
    return gfl2::proc::RES_CONTINUE;
  }

  //NATのコンテキストはキャンセルせずに完了を待つ
  if( m_natContext.GetState() == nn::nex::CallContext::CallInProgress )
  {
    GFL_PRINT("m_natContext 完了待ち\n");
    return gfl2::proc::RES_CONTINUE;
  }

  return gfl2::proc::RES_FINISH;
}

// NexMatchmakeListenerの実装
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
*/
//------------------------------------------------------------------
void P2pSimpleConnectionWifi::OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "OnMatchmakingMatchingFailed( %u )\n", errorCode );

  // 島岡様指摘　[ml-kujira-net:896] NEXのエラーハンドリングに関するお願い]
  // 以下のエラーは、アプリ側で独自にエラーメッセージ出力するなどしてハンドリングする。
  if(result == QRESULT_ERROR(RendezVous, SessionVoid)){  //マッチセッションが存在しない
    m_isMatchMakeFailed = true;
    GetP2pManager()->MatchMakeFailedOccur();
  }
  else if(result == QRESULT_ERROR(RendezVous, SessionFull)){ //すでに最大参加人数に達している。
    m_isMatchMakeFailed = true;
    GetP2pManager()->MatchMakeFailedOccur();
  }
  else if(result == QRESULT_ERROR(RendezVous, SessionClosed)){ //ギャザリングが締め切られている
    m_isMatchMakeFailed = true;
    GetP2pManager()->MatchMakeFailedOccur();
  }
  else{
    GetP2pManager()->ErrorOccur( result, errorCode );
  }

  m_isMatchmakeError = true;
}

//------------------------------------------------------------------
/**
* @brief SimpleMatchmake処理の成功通知
*/
//------------------------------------------------------------------
void P2pSimpleConnectionWifi::OnMatchmakingSimpleMatchmakeSuccess( void )
{
  GFL_PRINT("OnMatchmakingSimpleMatchmakeSuccess\n");
  m_isOnMatchmakingSimpleMatchmakeSuccess = true;
}

//------------------------------------------------------------------
/**
* @brief EntrySimpleMatchmake処理の成功通知
*/
//------------------------------------------------------------------
void P2pSimpleConnectionWifi::OnMatchmakingEntrySimpleMatchmakeSuccess( void )
{
  GFL_PRINT("OnMatchmakingEntrySimpleMatchmakeSuccess\n");
  m_isOnMatchmakingEntrySimpleMatchmakeSuccess = true;
}

//------------------------------------------------------------------
/**
* @brief CancelSimpleMatchmakeEntry処理の成功通知
*/
//------------------------------------------------------------------
void P2pSimpleConnectionWifi::OnMatchmakingCancelSimpleMatchmakeEntrySuccess( void )
{
  GFL_PRINT("OnMatchmakingCancelSimpleMatchmakeEntrySuccess\n");
  m_isOnMatchmakingCancelSimpleMatchmakeSuccess = true;
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   SimpleMatchmake実行
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::SimpleMatchmake( void )
{
  // リスナーでセットしている変数の初期化
  m_isOnMatchmakingSimpleMatchmakeSuccess = false;

  GFL_PRINT("SimpleMatchmake()\n");
  bool ret = m_nexMatchMakeClient->SimpleMatchmakeAsync();

  if( ret )
  {
    GFL_PRINT("SimpleMatchmake() Success\n");
    return true;
  }

  GetP2pManager()->ErrorOccur();
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   EntrySimpleMatchmake実行
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::EntrySimpleMatchmake( void )
{
  // リスナーでセットしている変数の初期化
  m_isOnMatchmakingEntrySimpleMatchmakeSuccess = false;

  GFL_PRINT("EntrySimpleMatchmake()\n");
  bool ret = m_nexMatchMakeClient->EntrySimpleMatchmakeAsync();

  if( ret )
  {
    GFL_PRINT("EntrySimpleMatchmake() Success\n");
    return true;
  }

  GetP2pManager()->ErrorOccur();
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   CancelSimpleMatchmakeEntry実行
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::CancelSimpleMatchmakeEntry( void )
{
  // リスナーでセットしている変数の初期化
  m_isOnMatchmakingCancelSimpleMatchmakeSuccess = false;

  GFL_PRINT("CancelSimpleMatchmakeEntry()\n");
  bool ret = m_nexMatchMakeClient->CancelSimpleMatchmakeEntryAsync();

  if( ret )
  {
    GFL_PRINT("CancelSimpleMatchmakeEntry() Success\n");
    return true;
  }

  GetP2pManager()->ErrorOccur();
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   NATトラバーサルのBind
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::BindNatTraversal( void )
{
  if( !m_nexManager->BindNATTraversalClient() )
  {
    GetP2pManager()->ErrorOccur();
    return false;
  }

  GFL_PRINT("BindNATTraversal() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   NATトラバーサルの開始
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::StartNatTraversal( void )
{
  // 接続人数
  int joinNum = GetInitParam()->GetLimitNum();

  //中継サーバ使用
  nn::nex::RootTransport::SetEnabledRelay(true);
  //タイムアウト
  nn::nex::RootTransport::SetNATTraversalTimeout(10000 * joinNum);
  //ステーション不一致検出閾値
  nn::nex::Session::SetDetectStationInconsitencyThresholdCount(joinNum*2+2);

  m_natContext.Reset();

  if( !nn::nex::ConnectivityManager::GetInstance()->StartNATSession( &m_natContext ) )
  {
    nn::nex::qResult result(false);
    GetP2pManager()->ErrorOccur( result, nn::nex::SystemError::GetLast() );
    ICHI_PRINT("natError\n");
    return false;
  }

  GFL_PRINT("StartNATTraversal() Success\n");

  return true;
}

//------------------------------------------------------------------
/**
 * @brief   Natトラバーサルの開始待ち
 * @return  E_STATUS定義
 */
//------------------------------------------------------------------
P2pSimpleConnectionWifi::E_STATUS P2pSimpleConnectionWifi::WaitNatTraversal( void )
{
  // コンテキストの状態
  switch( m_natContext.GetState() ){
  case nn::nex::CallContext::Available:
  case nn::nex::CallContext::CallSuccess:
    return E_STATUS_SUCCESS;
  case nn::nex::CallContext::CallInProgress:
    return E_STATUS_PROGRESS;
  case nn::nex::CallContext::CallFailure:
    {
      nn::nex::qResult result(false);
      GetP2pManager()->ErrorOccur( result, nn::nex::SystemError::GetLast() );
      ICHI_PRINT("WaitNATTraversal::Failed\n");
    }
    return E_STATUS_FAILURE;  //失敗の場合
  case nn::nex::CallContext::CallCancelled:
    ICHI_PRINT("WaitNATTraversal::CallCancelled\n");
    GetP2pManager()->ErrorOccur();
    return E_STATUS_FAILURE;  //失敗の場合
  default:
    GFL_ASSERT(0);
    GetP2pManager()->ErrorOccur();
    return E_STATUS_FAILURE;  //失敗の場合
  }
}

//------------------------------------------------------------------
/**
 * @brief   NetZの作成
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::CreateNetZ( void )
{
  if( !m_nexManager->CreateNetZ() )
  {
    GetP2pManager()->ErrorOccur();
    return false;
  }

  // ホストマイグレーション有効化
  // ※NetZとNexMatchmakeClientが作成されている必要がある！
  m_nexMatchMakeClient->EnableHostMigration();

  GFL_PRINT("CreateNetZ() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクの作成
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::CreateMatchmake( void )
{
  if( !m_nexMatchMakeClient->Initialize( m_nexManager->GetNgsFacade() ) )
  {
    GetP2pManager()->ErrorOccur();
    return false;
  }

  GFL_PRINT("CreateMatchmake() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   P2Pセッションを作成する（親）
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::CreateSession( void )
{
  if( !nn::nex::Session::CreateSession( NULL ) )
  {
    GetP2pManager()->ErrorOccur();
    return false;
  }
  
  GFL_PRINT("CreateSession() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   P2Pセッションに参加する（子）
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pSimpleConnectionWifi::JoinSession( void )
{
  nn::nex::qList<nn::nex::StationURL> listURLs = m_nexMatchMakeClient->GetSimpleMatchmakeHostInfo()->GetStationUrls();
#if GFL_DEBUG
    ICHI_PRINT("successfly obtained a list of URLs to connect to the game host\n");
    nn::nex::qList<nn::nex::StationURL>::iterator it = listURLs.begin();
    while (it!=listURLs.end()) {
      ICHI_PRINT("URL = %ls\n", (*it).CStr());
      ++it;
    }
#endif //GFL_DEBUG

  m_context.Reset();
  if( !nn::nex::Session::JoinSession( &m_context, listURLs) )
  {
    GFL_PRINT("JoinSession Call Failed\n");
    GetP2pManager()->ErrorOccur();
    return false;
  }

  GFL_PRINT("JoinSession Call Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   P2Pセッションに参加待ち
 * @return  E_STATUS_SUCCESSでセッションに参加成功した
 */
//------------------------------------------------------------------
P2pSimpleConnectionWifi::E_STATUS P2pSimpleConnectionWifi::IsJoinSession( void )
{
  // @attention  サーバからGatheringの削除を通知された場合はJoinSessionをキャンセルする必要あり
  //             NgsFacadeとの連携プログラムが必要  現在非対応
  //    if(m_pNotificationHandler->GetUnregisteredGatheringID() == m_gatheringID) {
  //    oContext.Cancel();
  //   }
  /*nn::nex::Scheduler* pSch = nn::nex::Scheduler::GetInstance();
  if(pSch){
    pSch->Dispatch(SAMPLE_DISPATCH_TIMEOUT);
  }*/

  // コンテキストの状態
  switch( m_context.GetState() ){
  case nn::nex::CallContext::Available:
  case nn::nex::CallContext::CallSuccess:
    ICHI_PRINT("Net-Z session on successfly joined\n");
    return E_STATUS_SUCCESS;
  case nn::nex::CallContext::CallInProgress:
    return E_STATUS_PROGRESS;
  case nn::nex::CallContext::CallFailure:
    {
      ICHI_PRINT("IsJoinSession::Failed\n");
      nn::nex::qResult result(false);
      GetP2pManager()->ErrorOccur();
    }
    return E_STATUS_FAILURE;
  case nn::nex::CallContext::CallCancelled:
    ICHI_PRINT("IsJoinSession::CallCancelled\n");
    GetP2pManager()->ErrorOccur();
    return E_STATUS_FAILURE;
  default:
    GFL_ASSERT(0);
    GetP2pManager()->ErrorOccur();
    return E_STATUS_FAILURE;
  }
}

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR

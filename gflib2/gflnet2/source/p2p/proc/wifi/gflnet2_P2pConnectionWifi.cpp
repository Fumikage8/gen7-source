#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pConnectionWifi.cpp
 * @brief	P2Pプロセス：Wifi接続
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:28:21
 */
// =============================================================================

#include <nex.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
#include "gflnet2/include/p2p/gflnet2_NetGame.h"
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include "gflnet2/source/p2p/proc/wifi/gflnet2_P2pConnectionWifi.h"

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
gfl2::proc::Result P2pConnectionWifi::InitFunc(gfl2::proc::Manager* )
{
  m_nexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  m_nexMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  m_gatheringHolder = NULL;
  m_gatheringListSize = 0;
  m_isOnMatchmakingCreateSuccess = false;
  m_isOnMatchmakingSearchSuccess = false;
  m_isOnMatchmakingJoinSuccess = false;
  m_isOnMatchmakingOpenSessionSuccess = false;
  m_isOnMatchmakingUpdateHostSuccess = false;
  m_isMatchMakeFailed = false;

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
gfl2::proc::Result P2pConnectionWifi::UpdateFunc(gfl2::proc::Manager*)
{
  //-------------------------------------
  /// シーケンス定義
  //=====================================
  enum{
    SEQ_BIND_NAT_TRAVERSAL,   ///< NATトラバーサルのBind
    SEQ_START_NAT_TRAVERSAL,  ///< NATトラバーサルの開始
    SEQ_WAIT_NAT_TRAVERSAL,   ///< NATトラバーサルの開始待ち
    SEQ_CREATE_NETZ,          ///< NetZの作成
    SEQ_CREATE_MATCHMAKE,     ///< マッチメイクの作成
    SEQ_START_MATCHMAKE,      ///< マッチメイクの開始
    SEQ_START_MATCHMAKE_WAIT, ///< マッチメイクの開始 リスナー待ち
    SEQ_JOIN_MATCHMAKE,       ///< マッチメイクへの参加
    SEQ_CREATE_SESSION,       ///< P2Pセッションを作成する（親）
    SEQ_UPDATE_SESSION_HOST,  ///< セッションホストのアップデート
    SEQ_JOIN_SESSION,         ///< P2Pセッションに参加する（子）
    SEQ_WAIT_JOIN_SESSION,    ///< P2Pセッションに参加待ち（子）
    SEQ_DIRECT_STREAM_ENABLE, ///< P2P送信許可
    SEQ_WAIT_CONNECTNUM,      ///< 接続人数待ち
  };

#if GFL_DEBUG
  // シーケンスが変わったら出力
  {
    static int debugSeq;
    if( debugSeq != GetSubSeq() )
    {
      debugSeq = GetSubSeq();
      ICHI_PRINT("P2pConnectionWifi::UpdateFunc Seq:%d\n", debugSeq );
    }
  }
#endif // GFL_DEBUG

  // 外からの終了リクエスト
  if( mbEnd )
  {
    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
    return gfl2::proc::RES_FINISH;
  }

  // エラー発生フラグ
  bool isError = false;

  switch( GetSubSeq() ){
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
      case E_STATUS_SUCCESS: SetSubSeq( SEQ_CREATE_NETZ ); break;
      }
    }
    break;
  case SEQ_CREATE_NETZ:          ///< NetZの作成
    {
      isError = !CreateNetZ();
      SetSubSeq( SEQ_CREATE_MATCHMAKE );
    }
    break;
  case SEQ_CREATE_MATCHMAKE:     ///< マッチメイクの作成
    {
      isError = !CreateMatchmake();
      SetSubSeq( SEQ_START_MATCHMAKE );
    }
    break;
  case SEQ_START_MATCHMAKE:      ///< マッチメイクの開始
    {
      isError = !StartMatchmake();

      // 後はリスナー待ち
      SetSubSeq( SEQ_START_MATCHMAKE_WAIT );
    }
    break;

  case SEQ_START_MATCHMAKE_WAIT: ///< マッチメイクの開始 リスナー待ち
    // リスナー待ち
    if( m_isOnMatchmakingCreateSuccess || m_isOnMatchmakingSearchSuccess  )
    {
      // 子でギャザリングが見つからなかった場合は再検索
      if( NetGame::GetNetGameBase()->IsMaster() == false && m_gatheringListSize == 0 )
      {
        SetSubSeq( SEQ_START_MATCHMAKE );
      }
      else
      {
        SetSubSeq( SEQ_JOIN_MATCHMAKE );
      }
    }
    break;

  case SEQ_JOIN_MATCHMAKE:       ///< マッチメイクへの参加
    {
      isError = !JoinMatchmake();

      // 接続種類ごとに次シーケンスが違う
      int nextSeq;
      switch( GetConnectType() ){
      case AUTO:    nextSeq = NetGame::GetNetGameBase()->IsMaster() ? SEQ_CREATE_SESSION : SEQ_JOIN_SESSION; break;
      case PARENT:  nextSeq = SEQ_CREATE_SESSION; break;
      case CHILD:   nextSeq = SEQ_JOIN_SESSION; break;
      }
      SetSubSeq( nextSeq );
    }
    break;
  case SEQ_CREATE_SESSION:       ///< P2Pセッションを作成する（親）
    {
      // リスナー待ち
      if( m_isOnMatchmakingOpenSessionSuccess == true )
      {
        isError = !CreateSession();

        if( isError == false )
        {//セッションオーナーのアップデート
          isError = !m_nexMatchMakeClient->UpdateSessionHost();
        }

        SetSubSeq( SEQ_UPDATE_SESSION_HOST );
      }
    }
    break;
  case SEQ_UPDATE_SESSION_HOST:///< セッションホストのアップデート
    {      
      if( m_isOnMatchmakingUpdateHostSuccess == true )
      {
        //@fix [#4127 インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】]
        if( GetInitParam()->IsMigrationCallBackEnable() )
        {
          m_nexMatchMakeClient->RegisterHostMigrationCallback();
        }
        
        SetSubSeq( SEQ_DIRECT_STREAM_ENABLE );//子の接続待ちへ
      }
    }
    break;
  case SEQ_JOIN_SESSION:         ///< P2Pセッションに参加する（子）
    {
      // リスナー待ち
      if( m_isOnMatchmakingJoinSuccess == true )
      {
        isError = !JoinSession();
        SetSubSeq( SEQ_WAIT_JOIN_SESSION );
      }
    }
    break;
  case SEQ_WAIT_JOIN_SESSION:    ///< P2Pセッションに参加待ち（子）
    {
      E_STATUS status = IsJoinSession();

      switch( status ){
      case E_STATUS_FAILURE: isError = true; break;
      case E_STATUS_SUCCESS:
        {
          //@fix [#4127 インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】]
          if( GetInitParam()->IsMigrationCallBackEnable() )
          {
            m_nexMatchMakeClient->RegisterHostMigrationCallback();
          }
          SetSubSeq( SEQ_DIRECT_STREAM_ENABLE ); 
        }
        break;
      }
    }
    break;
  case SEQ_DIRECT_STREAM_ENABLE: ///< P2P送信許可
    {
      // nexの送受信機能を有効化
      DirectStreamEnable();
      SetSubSeq( SEQ_WAIT_CONNECTNUM );
    }
    break;

  case SEQ_WAIT_CONNECTNUM:      ///< 接続人数待ち
    {
      // 接続人数
      int connectMax = GetInitParam()->GetLimitNum();
      if( connectMax == NetGame::GetNetGameBase()->GetConnectionCount() )
      {
        ICHI_PRINT("P2pConnectionWifi::UpdateFunc 接続人数 %d \n", connectMax );
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
    GFL_PRINT("P2pConnectionWifi::UpdateFunc Error SubSeq[%d]\n", GetSubSeq());
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
gfl2::proc::Result P2pConnectionWifi::EndFunc(gfl2::proc::Manager*)
{
  if( m_nexMatchMakeClient )
  {
    // 通信中かチェック
    if( m_nexMatchMakeClient->IsConnecting() )
    {
      m_nexMatchMakeClient->CancelConnecting();
      GFL_PRINT("m_nexMatchMakeClient Cancel待ち\n");
      return gfl2::proc::RES_CONTINUE;
    }
    m_nexMatchMakeClient->SetMatchmakeListener( NULL );
    m_nexMatchMakeClient = NULL;
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

//------------------------------------------------------------------
/**
 * @brief   マッチング条件を設定
 */
//------------------------------------------------------------------
void P2pConnectionWifi::SetMatchRuleList( gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList* ruleList )
{
  m_wifiMatchRuleList.clear();

  // コピー
  int i = 0;
  for( gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it )
  {
    m_rule[i].Copy( (*it) );
    m_wifiMatchRuleList.push_back( &m_rule[i] );
    i++;
    ICHI_PRINT("MatchRule %d[%d] \n", m_rule[i].GetIndex(), m_rule[i].GetBaseValue());
  }
}

// NexMatchmakeListenerの実装
//------------------------------------------------------------------
/**
* @brief マッチメイクセッションの作成完了通知
*
* @param[in] gatheringId 作成されたセッションのギャザリングID
*/
//------------------------------------------------------------------
void P2pConnectionWifi::OnMatchmakingCreateSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT( "OnMatchmakingCreateSuccess( %u )\n", gatheringId );
  m_isOnMatchmakingCreateSuccess = true;

  // ライブラリ外にも伝えたいのでリスナーで通知
  GetP2pManager()->MatchmakingCreateSuccess( gatheringId );

  // 親子フラグ
  NetGame::GetNetGameBase()->SetMaster( m_nexMatchMakeClient->IsOwner() );
  if( m_nexMatchMakeClient->IsOwner() ) { GFL_PRINT("親\n"); }
  else                                  { GFL_PRINT("子\n"); }
}

//------------------------------------------------------------------
/**
* @brief マッチメイクセッションの検索完了通知
*
* @param[in] gatheringList 検索されたセッションのギャザリングIDリスト
*/
//------------------------------------------------------------------
void P2pConnectionWifi::OnMatchmakingSearchSuccess( nn::nex::qList<nn::nex::GatheringHolder>* gatheringList )
{
  nn::nex::qList<nn::nex::GatheringHolder>::iterator it(gatheringList->begin());

  m_gatheringHolder = &(*it);
  m_gatheringListSize = gatheringList->size(); // これがゼロならもう１回
  m_isOnMatchmakingSearchSuccess = true;
  GFL_PRINT( "OnMatchmakingSearchSuccess( %u )\n", gatheringList->size() );
  if( m_gatheringListSize > 0 ){ GFL_PRINT(" GathreingID:[%u]\n", it->Get()->GetID() ); }
  // セッション検索時は子である
  NetGame::GetNetGameBase()->SetMaster( false );
}

//------------------------------------------------------------------
/**
* @brief マッチメイクセッションへの参加完了通知
*
* @param[in] gatheringId 参加したギャザリングID
*/
//------------------------------------------------------------------
void P2pConnectionWifi::OnMatchmakingJoinSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT("OnMatchmakingJoinSuccess( %u )\n", gatheringId );
  m_isOnMatchmakingJoinSuccess = true;

  // 親子フラグ
  NetGame::GetNetGameBase()->SetMaster( m_nexMatchMakeClient->IsOwner() );
  if( m_nexMatchMakeClient->IsOwner() ) { GFL_PRINT("親\n"); }
  else                                  { GFL_PRINT("子\n"); }
}

//------------------------------------------------------------------
/**
* @brief マッチメイクセッションのオープン正常終了通知
*/
//------------------------------------------------------------------
void P2pConnectionWifi::OnMatchmakingOpenSessionSuccess( void )
{
  GFL_PRINT("OnMatchmakingOpenSessionSuccess\n");
  m_isOnMatchmakingOpenSessionSuccess = true;
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
*/
//------------------------------------------------------------------
void P2pConnectionWifi::OnNexMatchmakeError( const nn::nex::qResult& result, u32 errorCode )
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
}

//------------------------------------------------------------------
/**
* @brief ホスト更新処理の成功通知
*/
//------------------------------------------------------------------
void P2pConnectionWifi::OnMatchmakingUpdateSessionHostSuccess( void )
{
  GFL_PRINT("P2pConnectionWifi::OnMatchmakingUpdateSessionHostSuccess \n");
  m_isOnMatchmakingUpdateHostSuccess = true;
}

//------------------------------------------------------------------
/**
* @brief マッチメイクセッションのオーナー変更通知
*
* @param[in] newOwnerId 新たにオーナーとなったユーザーのプリンシバルID
*/
//------------------------------------------------------------------
void P2pConnectionWifi::OnMatchmakingOwnerChange( nn::nex::PrincipalID newOwnerId )
{
  GFL_PRINT("P2pConnectionWifi::OnMatchmakingOwnerChange \n");
  //接続中のマイグレーションはエラーとして扱う
  GetP2pManager()->ErrorOccur();
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   NATトラバーサルのBind
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pConnectionWifi::BindNatTraversal( void )
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
bool P2pConnectionWifi::StartNatTraversal( void )
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
P2pConnectionWifi::E_STATUS P2pConnectionWifi::WaitNatTraversal( void )
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
bool P2pConnectionWifi::CreateNetZ( void )
{
  if( !m_nexManager->CreateNetZ() )
  {
    GetP2pManager()->ErrorOccur();
    return false;
  }

  GFL_PRINT("CreateNetZ() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクの作成
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pConnectionWifi::CreateMatchmake( void )
{
  if( !m_nexMatchMakeClient->Initialize( m_nexManager->GetNgsFacade() ) )
  {
    GetP2pManager()->ErrorOccur();
    return false;
  }

  // ホストマイグレーション有効化
  // ※NetZとNexMatchmakeClientが作成されている必要がある！
  m_nexMatchMakeClient->EnableHostMigration();

  GFL_PRINT("CreateMatchmake() Success\n");
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクの開始
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pConnectionWifi::StartMatchmake( void )
{
  u32 joinNum = GetInitParam()->GetLimitNum();
  u32 gameMode= GetInitParam()->GetGameID();
  bool ret = false;

  // リスナーでセットしている変数の初期化
  m_gatheringHolder = NULL;
  m_gatheringListSize = 0;
  m_isOnMatchmakingCreateSuccess = false;
  m_isOnMatchmakingSearchSuccess = false;

  // 接続種類ごとに開始方法が違う
  switch( GetConnectType() ){
  case AUTO:
    GFL_PRINT("StartMatchmake() AUTO\n");
    ret = m_nexMatchMakeClient->AutoMatchmakeAsync( gflnet2::nex::NexMatchMakingClient::MATCHMAKE_TYPE_ANYBODY,
                                                    joinNum, gameMode, &m_wifiMatchRuleList, true, false );
    break;
  case PARENT:
    GFL_PRINT("StartMatchmake() PARENT\n");
    ret = m_nexMatchMakeClient->CreateSessionAsync( gflnet2::nex::NexMatchMakingClient::MATCHMAKE_TYPE_ANYBODY,
                                                    joinNum, gameMode, &m_wifiMatchRuleList, true, false );
    break;
  case CHILD:
    GFL_PRINT("StartMatchmake() CHILD");
    ret = m_nexMatchMakeClient->SearchSessionAsync( gflnet2::nex::NexMatchMakingClient::MATCHMAKE_TYPE_ANYBODY,
                                                    joinNum, gameMode, &m_wifiMatchRuleList );
    break;
  }

  if( ret )
  {
    GFL_PRINT("StartMatchmake() Success\n");
    return true;
  }
  GetP2pManager()->ErrorOccur();
  return false;
}


//------------------------------------------------------------------
/**
 * @brief   マッチメイクへの参加
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pConnectionWifi::JoinMatchmake( void )
{
  nn::nex::qResult ret(false);

  // リスナーでセットしている変数の初期化
  m_isOnMatchmakingOpenSessionSuccess = false;
  m_isOnMatchmakingJoinSuccess = false;

  // 接続種類ごとに参加方法が違う
  switch( GetConnectType() ){
  case AUTO:
    if( NetGame::GetNetGameBase()->IsMaster() )
    { // 親の場合はSessionが閉じているので開く
      // セッション開き、そこに参加しオーナーになる
      ret = m_nexMatchMakeClient->OpenSessionAsync( m_nexMatchMakeClient->GetGatheringID() );
    }
    else
    { // 子の場合はセッションに参加済み
      m_isOnMatchmakingJoinSuccess = true;
      ret = nn::nex::qResult(true);
    }
    break;
  case PARENT:
    // セッション開き、そこに参加しオーナーになる
    ret = m_nexMatchMakeClient->OpenSessionAsync( m_nexMatchMakeClient->GetGatheringID() );
    break;
  case CHILD:
    // セッションに参加する
    ret = m_nexMatchMakeClient->JoinSessionAsync( *m_gatheringHolder );
    break;
  }

  if( ret.IsError() )
  {
    u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( ret );
    GetP2pManager()->ErrorOccur( ret, errorCode );
    return false;
  }

  GFL_PRINT("JoinMatchmake() Success\n");
  return true;
}


//------------------------------------------------------------------
/**
 * @brief   P2Pセッションを作成する（親）
 * @return  実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool P2pConnectionWifi::CreateSession( void )
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
bool P2pConnectionWifi::JoinSession( void )
{
  nn::nex::qList<nn::nex::StationURL> listURLs;

  if( !GetSessionURLs(listURLs) )
  {
    GFL_PRINT("GetSessionURLs Failed\n");
    GetP2pManager()->ErrorOccur();
    return false;
  }

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
P2pConnectionWifi::E_STATUS P2pConnectionWifi::IsJoinSession( void )
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

//------------------------------------------------------------------
/**
 * @brief   マッチメイクセッションの参加者のURLを取得（サンプルから移植）
 * @return  trueで成功
 */
//------------------------------------------------------------------
bool P2pConnectionWifi::GetSessionURLs( nn::nex::qList<nn::nex::StationURL>& listURL )
{
  nn::nex::GatheringID gatheringID = m_nexMatchMakeClient->GetGatheringID();

  // セッションのURLをリストアップし、それらに参加を試みます。
  nn::nex::ProtocolCallContext oContext;
  if( m_nexMatchMakeClient->GetMatchmakeExtensionClient()->GetSessionURLs( &oContext, gatheringID, &listURL ) )
  {
    oContext.Wait();
    if (oContext.GetState() != nn::nex::CallContext::CallSuccess)
    {
      // Join したマッチメイクセッションが削除されていることもありえます。
      if(oContext.GetOutcome() == QERROR(RendezVous, SessionVoid)) {
        GFL_PRINT("matchmake session is already unregistered.\n");
      }
      return false;
    }
#if GFL_DEBUG
    ICHI_PRINT("successfly obtained a list of URLs to connect to the game host\n");
    nn::nex::qList<nn::nex::StationURL>::iterator it = listURL.begin();
    while (it!=listURL.end()) {
      ICHI_PRINT("URL = %ls\n", (*it).CStr());
      ++it;
    }
#endif //GFL_DEBUG

    // 成功
    return true;
  }

  return false;
}

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR

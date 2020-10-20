#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMatchMakingClient.cpp
 *
 */
//=============================================================================

#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include "gflnet2_NexMatchMakingNotificationListener.h"
#include "gflnet2/source/p2p/gflnet2_P2pStaticManager.h"

#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

//-----------------------------------------------------------------------------
/**
 *    Singletonインスタンスの実体を定義
 */
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gflnet2::nex::NexMatchMakingClient)
template class gfl2::base::SingletonAccessor<gflnet2::nex::NexMatchMakingClient>;
#endif


static gflnet2::nex::NexMatchMakingNotificationListener  s_notifyListener; //!< NEX通知検知用リスナ

// マッチメイクグループ
#if GFL_DEBUG
nn::nex::qUnsignedInt32 gflnet2::nex::NexMatchMakingClient::MATCHMAKE_GROUP_RANDOM_TRADE = 64;
#endif

//-----------------------------------------------------------------------------
/**
 * @brief セッション参加要求受付時のコールバック関数
 *
 * @param pOperation セッション操作情報
 */
//-----------------------------------------------------------------------------
static void CheckJoiningProcesses(nn::nex::JoinSessionOperation * pOperation)
{
  gflnet2::nex::NexMatchMakingClient*  matchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);

  if( (matchMakeClient != NULL) && matchMakeClient->IsSimpleMatchmake() )
  {//シンプルマッチメイクは無条件に成功
    pOperation->Approve();
    return;
  }

  // 既に参加しているステーションの数をカウント (数には、参加してきたステーションは含まれない)
  nn::nex::Station::SelectionIterator it;
  qUnsignedInt32                      uiPlayerCount   = it.Count();

  nn::nex::String    password = _T("");
  nn::nex::Station*  station = nn::nex::Station::GetLocalInstance();

  if( station != NULL){
    password = station->GetIdentificationToken();
  }

  // 参加可能人数内でかつ、パスワードが一致している？
  bool isArrow = false;
  if( (matchMakeClient != NULL) &&
      (uiPlayerCount < matchMakeClient->GetJoinMaxNum()) &&
      (pOperation->GetJoiningStation()->GetIdentificationToken() == password) )
  {
    isArrow = true;
  }

  if( isArrow )
  {
    // 参加を許可
    pOperation->Approve();
  }
  else
  {
    // 参加NG！ 接続希望者には「qResult(DOCore, JoinDenied) 」が発行される
    pOperation->Deny();
  }

  GFL_PRINT("CheckJoiningProcesses[%u] Master-Passord:%ls JoiningStation-Passord:%ls\n", isArrow, password.CStr(), pOperation->GetJoiningStation()->GetIdentificationToken().CStr() );
}






GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(nex)


//------------------------------------------------------------------
/**
 * @brief  NexMatchMakingClientコンストラクタ
 */
//------------------------------------------------------------------
NexMatchMakingClient::NexMatchMakingClient( void )
{
  m_isInit            = false;
  m_isAccess          = false;
  m_isEncryption      = false;
  //
  m_gatheringID       = INVALID_GATHERINGID;
  m_commGatheringID   = INVALID_GATHERINGID;
  m_joinMaxNum        = 0;
  //
  m_matchListener     = NULL;
  m_sessionListener   = NULL;
  m_communityListener = NULL;
  //
  m_ownerPID          = 0;
  m_fascade           = 0;
  m_context           = NULL;
  m_matchingClient    = NULL;
  m_gatheringList     = NULL;
  m_criteriaList      = NULL;
  m_sessionKey        = NULL;
  m_criteriaInfo      = NULL;
  m_sessionInfo       = NULL;
  m_autoSessionInfo   = NULL;
  m_communityList     = NULL;
  //
  m_isFound           = false;//!< 相手が見つかったかどうか
  m_pSimpleMatchmakeHostInfo = NULL;
  m_isSucceed         = false;//!< キャンセル成功したか
  m_isSimpleMatchmake = false;
  //
  m_pHostMigrationCallback = NULL;
  //
#if GFL_DEBUG
  m_debugIsClearCache = false;
  m_debugMatchmakeGroup = 0;
#endif // GFL_DEBUG
  //
  SetOwner( INVALID_PRINCIPALID );
}


//------------------------------------------------------------------
/**
 * @brief  デストラクタ
 */
//------------------------------------------------------------------
NexMatchMakingClient::~NexMatchMakingClient()
{
}


//------------------------------------------------------------------
/**
 * @brief  MatchmakeExtensionClientインスタンスの取得
 *
 * @return MatchmakeExtensionClientインスタンス
 */
//------------------------------------------------------------------
nn::nex::MatchmakeExtensionClient* NexMatchMakingClient::GetMatchmakeExtensionClient(void)
{
  return m_matchingClient;
}

//------------------------------------------------------------------
/**
 * @brief  ホストマイグレーション拡張機能の有効化
 */
//------------------------------------------------------------------
void NexMatchMakingClient::EnableHostMigration(void)
{
  // ホストマイグレーション拡張機能を有効にする
  // ホストマイグレーション拡張機能とはP2PのホストがP2Pセッションを抜けたときにサーバーのマッチメイクセッションのホスト情報を自動的に更新する機能です。
  // このサンプルではホストとオーナーを一致させるようにします。
  // StaticUpdateSessionHostWithOwner() を使いホスト情報を更新すると同時にオーナーの変更も行います。
  // P2Pセッションのホストとなったユーザーがマッチメイクセッションのオーナーとなります。
  // HostMigrationExtension は NetZ オブジェクトが破棄されたときに自動で Unregister されます
  nn::nex::HostMigrationExtension::Register();
  m_pHostMigrationCallback = qNew nn::nex::HostMigrationCallback(m_matchingClient, nn::nex::MatchMakingClient::StaticUpdateSessionHost);
  gflnet2::p2p::StaticManager::SetRegisterdMigration();//Register済みフラグ設定
}

//------------------------------------------------------------------
/**
  * @brief  ホストマイグレーションコールバックの登録
  */
//------------------------------------------------------------------
void NexMatchMakingClient::RegisterHostMigrationCallback(void)
{
  // ホストマイグレーションコールバックを登録
  m_pHostMigrationCallback->Register( m_gatheringID );
}


//------------------------------------------------------------------
/**
  * @brief  ホスト孤立状態かどうか
  */
//------------------------------------------------------------------
bool NexMatchMakingClient::SessionHostIsOrphan(void)
{
  bool isOrphan = ( m_pHostMigrationCallback && m_pHostMigrationCallback->SessionHostIsOrphan() );
  return isOrphan;
}


//------------------------------------------------------------------
/**
 * @brief マッチメイク用リスナの登録
 *
 * @param listener マッチメイク用リスナ。NULLを指定することで登録を解除できる。
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetMatchmakeListener( NexMatchmakeListener* listener )
{
  m_matchListener = listener;
}


//------------------------------------------------------------------
/**
 * @brief マッチメイク用リスナの取得
 *
 * @return マッチメイク用リスナへのポインタ
 */
//------------------------------------------------------------------
NexMatchmakeListener* NexMatchMakingClient::GetMatchmakeListener( void ) const
{
  return m_matchListener;
}


//------------------------------------------------------------------
/**
 * @brief セッションイベント用リスナの登録
 *
 * @param listener セッションイベント用リスナ。NULLを指定することで登録を解除できる。
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetSessionEventListener( NexSessionEventListener* listener )
{
  m_sessionListener = listener;
}


//------------------------------------------------------------------
/**
 * @brief セッションイベント用リスナの取得
 *
 * @return セッションイベント用リスナへのポインタ
 */
//------------------------------------------------------------------
NexSessionEventListener* NexMatchMakingClient::GetSessionEventListener( void ) const
{
  return m_sessionListener;
}


//------------------------------------------------------------------
/**
* @brief コミュニティ用リスナの登録
*
* @param[in] listener コミュニティ用リスナ。NULLを指定することで登録を解除できる。
*/
//------------------------------------------------------------------
void NexMatchMakingClient::SetCommunityListener( NexCommunityListener* listener )
{
  m_communityListener = listener;
}


//------------------------------------------------------------------
/**
* @brief コミュニティ用リスナの取得
*
* @return コミュニティ用リスナへのポインタ
*/
//------------------------------------------------------------------
NexCommunityListener* NexMatchMakingClient::GetCommunityListener( void ) const
{
  return m_communityListener;
}


//------------------------------------------------------------------
/**
 * @brief  MatchMakingClientの初期化
 *
 * @param[in] pFascade     ゲームサーバーログイン情報
 *
 * @note マッチングの連続成立防止用履歴のクリア処理はデバッグビルド時のみ有効
 *
 * @return 初期化に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::Initialize( nn::nex::NgsFacade* pFascade )
{
  if(!m_isInit)
  {
    if( (pFascade == NULL) || (pFascade->GetCredentials()==NULL) ){
      return false;
    }
    SetOwner( INVALID_PRINCIPALID );

//    m_matchListener   = NULL;  //この変数は外部から来るためすでにセットされている使い方が可能
    m_sessionInfo     = NULL;
    m_joinMaxNum      = 0;
    m_gatheringID     = INVALID_GATHERINGID;
    m_commGatheringID = INVALID_GATHERINGID;

    m_isEncryption    = true;   // 暗号化は必ずON
    m_isInit          = true;
    m_isAccess        = false;
    m_fascade         = pFascade;
    m_context         = NULL;
    //
    m_criteriaInfo    = qNew nn::nex::MatchmakeSessionSearchCriteria(); // マッチメイクの検索条件
    m_matchingClient  = qNew nn::nex::MatchmakeExtensionClient();       // マッチメイク機能を利用するためのクラス
    m_autoSessionInfo = qNew nn::nex::GatheringHolder();                // MatchmakeSession をゲームサーバに送受信するために使用されます
    m_pSimpleMatchmakeHostInfo = qNew nn::nex::SimpleMatchmakeHostInfo();
    //
    m_criteriaList    = qNew nn::nex::qList<nn::nex::MatchmakeSessionSearchCriteria>;
    m_gatheringList   = qNew nn::nex::qList<nn::nex::GatheringHolder>;
    m_communityList   = qNew nn::nex::qList<nn::nex::Community>;
    m_sessionKey      = qNew nn::nex::qVector<nn::nex::qByte>;
    //

    // セッション参加要求受付時のコールバックを設定
    nn::nex::Session::RegisterJoinApprovalCallback(CheckJoiningProcesses);

    m_fascade->RegisterNotificationEventHandler( &s_notifyListener );

    if( !m_matchingClient->Bind(m_fascade->GetCredentials()) ){
      return false;
    }

#if GFL_DEBUG
    // 拡張機能。連続してマッチングしない為の履歴情報をクリア
    if( m_debugIsClearCache )
    {
      nn::nex::ProtocolCallContext context;
      m_matchingClient->ClearMyPreviouslyMatchedUserCache( &context );
      context.Wait();
      GFL_PRINT("!!!MatchedUserCache Clear!!!\n");
    }
#endif
  }
  return true;
}


//------------------------------------------------------------------
/**
 * @brief  MatchMakingClientの終了
 *
 * @attention 必ず事前にIsConnecting()がfalseであることを確認してください
 */
//------------------------------------------------------------------
void NexMatchMakingClient::Finalize( void )
{
  if( m_isInit )
  {
    if( m_pHostMigrationCallback )
    {
      m_pHostMigrationCallback->Unregister( m_gatheringID );
      qDelete m_pHostMigrationCallback;
      m_pHostMigrationCallback = NULL;
    }

    nn::nex::Session::RegisterJoinApprovalCallback(NULL);

    m_fascade->UnregisterNotificationEventHandler( &s_notifyListener );

    NexFreeMemory( &m_pSimpleMatchmakeHostInfo );
    NexFreeMemory( &m_autoSessionInfo );
    NexFreeMemory( &m_sessionInfo );
    NexFreeMemory( &m_criteriaInfo );

    NexFreeCollection( &m_sessionKey );
    NexFreeCollection( &m_communityList );
    NexFreeCollection( &m_gatheringList );
    NexFreeCollection( &m_criteriaList );

    m_matchListener   = NULL;
    if( m_matchingClient != NULL )
    {
      if( m_matchingClient->GetCredentials() != NULL ){
        m_matchingClient->Unbind();
      }
      NexFreeMemory( &m_matchingClient );
    }
    m_isInit = false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  現在接続しているセッションのギャザリングIDを取得する
 *
 * @attention CreateSession()またはJoinSession()にてセッションへ
 *            接続しておく必要がある。
 *
 * @return ギャザリングID
 */
//------------------------------------------------------------------
NexMatchMakingClient::GatheringID NexMatchMakingClient::GetGatheringID( void ) const
{
  return m_gatheringID;
}


//------------------------------------------------------------------
/**
 * @brief  参加予定のセッションの最大参加人数を取得する
 *
 * @attention 事前にCreateSession(), SearchSession(), AutoMatchmake()を事前に実行する必要がある。
 *
 * @return 最大参加人数
 */
//------------------------------------------------------------------
u32 NexMatchMakingClient::GetJoinMaxNum( void ) const
{
  return m_joinMaxNum;
}


//------------------------------------------------------------------
/**
 * @brief  自身がマッチメイクセッションのオーナーであるか判定
 *
 * @attention セッションに接続していない場合は正しく動作しない
 *
 * @return 自身がマッチメイクセッションのオーナーであればtrueを返却する
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::IsOwner( void ) const
{
  if( m_matchingClient && m_matchingClient->GetCredentials() )
  {
    return m_matchingClient->GetCredentials()->GetPrincipalID() == m_ownerPID;
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief  オーナーのプリンシバルIDを設定する
 *
 * @attention コールバック関数用に準備している為、ユーザー操作禁止！
 *
 * @return none
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetOwner( nn::nex::PrincipalID pid )
{
  m_ownerPID = pid;
}


//------------------------------------------------------------------
/**
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::IsConnecting( void ) const
{
  return m_isAccess;
}


//------------------------------------------------------------------
/**
 * @brief 通信キャンセルリクエストを発行する
 * @brief キャンセルが実施された場合、完了時にOnNexRatingClientCanceled()がコールされる。
 *
 * @note  通信状態にかかわらず、コールしても良い
 *
 * @return キャンセル処理が実施されたならtrueを返却する
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::CancelConnecting( void )
{
  if( m_isAccess && (m_context != NULL) )
  {
    if( m_context->GetState() == nn::nex::CallContext::CallInProgress ){
      return m_context->Cancel();
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief  通信アクセスロック状態化を判定
 *
 * @return アクセスロック状態にあるならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::LockConnection( void )
{
  if( m_isAccess || (m_matchingClient==NULL) ){
    return true;
  }
  m_isAccess = true;

  return false;
}


//------------------------------------------------------------------
/**
 * @brief  通信アクセスロックを解除する
 */
//------------------------------------------------------------------
void NexMatchMakingClient::UnlockConnection( void )
{
  m_isAccess = false;
  m_context  = NULL;
}


//------------------------------------------------------------------
/**
 * @brief  マッチメイクセッション検索条件を設定する
 *
 * @param type         参加タイプ
 * @param joinNum      マッチメイクに参加できる最大人数
 * @param gameMode     ゲームモード（任意に設定可能）
 * @param ruleList     抽出条件として使用するルールリスト
 *
 * @return none
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetupCriteria( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode, NexMatchMakingRuleList* ruleList )
{
  m_criteriaInfo->Reset();

  GFL_PRINT("----------------------------------\n");
  GFL_PRINT("MatchmakeSession検索条件リスト\n");

  // 参加人数
  m_criteriaInfo->SetMinParticipants( 1 );       // 最低人数
  m_criteriaInfo->SetMaxParticipants( joinNum ); // 最大人数
  GFL_PRINT(" Max人数:%d\n", joinNum );

  // モード
  m_criteriaInfo->SetGameMode( gameMode );
  m_criteriaInfo->SetMatchmakeSystemType( static_cast<nn::nex::MatchmakeSystemType>(type) );
  GFL_PRINT(" GameMode:%d\n", gameMode );

  // 抽出条件
  if( ruleList != NULL )
  {
    for(NexMatchMakingRuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it )
    {
      //作成時のみの条件だったらスキップ
      if( it->GetValueType() == gflnet2::nex::NexMatchMakingRule::VALUE_TYPE_CREATE_ONLY )
        continue;

      if( it->IsRangeValue() ){
        m_criteriaInfo->SetAttributeWithRange( it->GetIndex(), it->GetRangeMinValue(), it->GetRangeMaxValue() );
        GFL_PRINT(" 抽出条件(%d):%d-%d\n", it->GetIndex(), it->GetRangeMinValue(), it->GetRangeMaxValue() );
      }
      else{
        m_criteriaInfo->SetAttribute( it->GetIndex(), it->GetBaseValue() );
        GFL_PRINT(" 抽出条件(%d):%d\n", it->GetIndex(), it->GetBaseValue() );
      }
    }
  }
#if GFL_DEBUG
  if( m_debugMatchmakeGroup != 0 )
  {
    m_criteriaInfo->SetAttribute( nn::nex::NUM_MATCHMAKE_SESSION_ATTRIBUTES-1, m_debugMatchmakeGroup );
    GFL_PRINT(" デバッグ用条件(%d):%d\n", nn::nex::NUM_MATCHMAKE_SESSION_ATTRIBUTES-1, m_debugMatchmakeGroup );
  }
#endif
  GFL_PRINT("----------------------------------\n");

  // 満員は除外
  m_criteriaInfo->SetVacantOnly( true );

  // 参加可能なセッションのみ検索
  m_criteriaInfo->SetExcludeLocked(true);

  // ホストが見つからない場合は除外
  m_criteriaInfo->SetExcludeNonHostPid( true );

  // リストに登録
  m_criteriaList->clear();
  m_criteriaList->push_back( *m_criteriaInfo );
}


//------------------------------------------------------------------
/**
 * @brief  マッチメイクセッション作成条件を設定する
 *
 * @param type             参加タイプ
 * @param joinNum          マッチメイクに参加できる最大人数
 * @param gameMode         ゲームモード（任意に設定可能）
 * @param ruleList         抽出条件として使用するルール
 * @param isMigrateOwner   ホストがログアウトしたときに他のユーザーにOwnerの委譲を行うならtrueを指定
 * @param isUsingBlackList ブラックリストを適応するならtrueを指定
 *
 * @return none
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetupMatchmakeSession( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode,
                                                  NexMatchMakingRuleList* ruleList,
                                                  bool isMigrateOwner, bool isUsingBlackList )
{
  nn::nex::MatchmakeSession* ruleInfo = qNew nn::nex::MatchmakeSession();

  GFL_PRINT("----------------------------------\n");
  GFL_PRINT("MatchmakeSession作成条件リスト\n");
  ruleInfo->Reset();

  // 本セッションへの参加は可能か？
  ruleInfo->SetOpenParticipation( false ); // P2P接続を考慮して閉じておく

  // 参加人数
  ruleInfo->SetMinParticipants( 1 );       // 最低人数
  ruleInfo->SetMaxParticipants( joinNum ); // 最大人数
  GFL_PRINT(" Max人数:%d\n", joinNum );

  // モード
  ruleInfo->SetGameMode( gameMode );
  ruleInfo->SetMatchmakeSystemType( static_cast<nn::nex::MatchmakeSystemType>(type) );
  GFL_PRINT(" GameMode:%d\n", gameMode );

  // 抽出条件
  if( ruleList != NULL )
  {
    for(NexMatchMakingRuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it )
    {
      //検索時のみの条件だったらスキップ
      if( it->GetValueType() == gflnet2::nex::NexMatchMakingRule::VALUE_TYPE_SEARCH_ONRY )
        continue;

      ruleInfo->SetAttribute( it->GetIndex(), it->GetBaseValue() );
      GFL_PRINT(" 抽出条件(%d):%d\n", it->GetIndex(), it->GetBaseValue() );
    }
  }
#if GFL_DEBUG
  if( m_debugMatchmakeGroup != 0 )
  {
    ruleInfo->SetAttribute( nn::nex::NUM_MATCHMAKE_SESSION_ATTRIBUTES-1, m_debugMatchmakeGroup );
    GFL_PRINT(" デバッグ用条件(%d):%d\n", nn::nex::NUM_MATCHMAKE_SESSION_ATTRIBUTES-1, m_debugMatchmakeGroup );
  }
#endif
  GFL_PRINT("----------------------------------\n");

  // ホストがログアウトしたときにOwnerの委譲を行う
  if( isMigrateOwner ){
    ruleInfo->SetFlag(nn::nex::GatheringFlags::MigrateOwner);
    // 他人がオーナー権限を奪えるか。MatchmakingClient::StaticUpdateSessionHostWithOwner() を使うため必要。
    //@fix NMCat[#4132 バトルロイヤルでバトル開始時の画面がホワイトアウトした直後に切断するとゲームが進行しなくなる　【ホストマイグレーション】]
    //ruleInfo->SetFlag(nn::nex::GatheringFlags::ChangeOwnerByOtherHost);
  }

  // マッチメイクにブラックリストを適用する
  //if( isUsingBlackList ){
  //  ruleInfo->SetFlag(nn::nex::GatheringFlags::UseBlackList);
  //}

  // [ruleInfo]は「m_sessionInfo」インスタンス解放時に合わせて解放される
  NexFreeMemory( &m_sessionInfo );
  m_sessionInfo = qNew nn::nex::GatheringHolder(ruleInfo);
}


//------------------------------------------------------------------
/**
 * @brief 自身をオーナーとしてマッチメイクセッションを作成し、参加する(非同期関数)
 *
 * @attention 本関数実行後、必ずOpenSession()をコールしてください。
 *
 * @note  非同期通信の結果
 *        正常終了時は「NexMatchmakeListener::OnMatchmakingCreateSuccess()」が
 *        エラー時は  「NexMatchmakeListener::OnMatchmakingMatchingFailed()」が
 *        コールされる。
 *        本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @param[in] type             参加タイプ
 * @param[in] joinNum          マッチメイクに参加できる最大人数
 * @param[in] gameMode         ゲームモード（任意に設定可能）
 * @param[in] ruleList         抽出条件として使用するルールリスト(NULL指定も可能）
 * @param[in] isMigrateOwner   オーナーがログアウトしたときに他のユーザーにOwnerの委譲を行うならtrueを指定
 * @param[in] isUsingBlackList ブラックリストを適応するならtrueを指定
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::CreateSessionAsync( E_MATCHMAKE_TYPE type,
                                               u32 joinNum, u32 gameMode,
                                               NexMatchMakingRuleList* ruleList,
                                               bool isMigrateOwner, bool isUsingBlackList )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_joinMaxNum = joinNum;
  SetupMatchmakeSession( type, joinNum, gameMode, ruleList, isMigrateOwner, isUsingBlackList );

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  nn::nex::qBool result = m_matchingClient->CreateMatchmakeSession( pContext, *m_sessionInfo, &m_gatheringID, m_sessionKey );

  if( result )
  {
    m_isSimpleMatchmake = false;
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnCreateSessionCompletion, pContext );
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
  }

  return result;
}

//------------------------------------------------------------------
/**
 * @brief マッチメイクセッションのホストを自分自身として更新する
 *
 * @note  非同期通信の結果
 *        正常終了時は「NexMatchmakeListener::OnMatchmakingUpdateHostSuccess()」が
 *        コールされる。
 *        本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::UpdateSessionHost( void )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  nn::nex::qBool result = m_matchingClient->UpdateSessionHost( pContext, m_gatheringID );
  if( result )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnUpdateSessionHostCompletion, pContext );
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
  }

  return result;
}


//------------------------------------------------------------------
/**
 * @brief  セッションを再開する(非同期通信関数)
 *
 * @note   作成成功時は「NexMatchmakeListener::OnMatchmakingOpenSessionSuccess()」がコールされます。
 *
 * @param  gatheringID 再会対象となるセッションのギャザリングID
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::OpenSessionAsync( nn::nex::GatheringID gatheringID )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_matchingClient->OpenParticipation( pContext, m_gatheringID ) )
  {
    m_isSimpleMatchmake = false;
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnOpenSessionCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief 条件に合うマッチメイクセッションを検索・作成・参加する（非同期関数）
*
* @note  自身がオーナーとなった場合は「NexMatchmakeListener::OnMatchmakingCreateSuccess()」がコールされる。
* @note  一般参加者の場合は「NexMatchmakeListener::OnMatchmakingSearchSuccess()」がコールされる。
* @note  エラー時は「NexMatchmakeListener::OnMatchmakingMatchingFailed()」がコールされる。
* @note  本関数がfalseを返却した際は、上記イベントは通知されない。
*
* @attention 本関数を使用してマッチメイクセッションに参加した場合は
*            CreateSession(), JoinSession()をコールしてはならない。
*
* @param[in] type             参加タイプ
* @param[in] joinNum          マッチメイクに参加できる最大人数
* @param[in] gameMode         ゲームモード（任意に設定可能）
* @param[in] ruleList         抽出条件として使用するルールリスト
* @param[in] isMigrateOwner   オーナーがログアウトしたときに他のユーザーにOwnerの委譲を行うならtrueを指定
* @param[in] isUsingBlackList ブラックリストを適応するならtrueを指定
*
* @return 非同期処理の開始に成功したならtrueを返却
*/
//------------------------------------------------------------------
bool NexMatchMakingClient::AutoMatchmakeAsync( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode,
                                               NexMatchMakingRuleList* ruleList,
                                               bool isMigrateOwner, bool isUsingBlackList )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_joinMaxNum = joinNum;

  SetupMatchmakeSession( type, joinNum, gameMode, ruleList, isMigrateOwner, isUsingBlackList );
  SetupCriteria( type, joinNum, gameMode, ruleList );

  if( m_matchingClient->AutoMatchmake( pContext, *m_criteriaList, *m_sessionInfo, m_autoSessionInfo ) )
  {
    m_isSimpleMatchmake = false;
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnAutoMatchmakeCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  マッチメイクセッションを検索する(非同期関数)
 *
 * @attention オーナーユーザーは「CreateSession()」を使用してください
 *
 * @note      非同期通信の結果
 *            正常成功時は「NexMatchmakeListener::OnMatchmakingSearchSuccess()」が
 *            エラー時は  「NexMatchmakeListener::OnMatchmakingMatchingFailed()」が
 *            コールされます。
 *            本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @param[in] type          参加タイプ
 * @param[in] joinNum       マッチメイクに参加できる最大人数
 * @param[in] gameMode      ゲームモード（任意に設定可能）
 * @param[in] ruleList       抽出条件として使用するルールリスト(NULL指定も可能）
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::SearchSessionAsync( E_MATCHMAKE_TYPE type, u32 joinNum, u32 gameMode, NexMatchMakingRuleList* ruleList )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ResultRange          range;  // 検索範囲 デフォルトでオフセット0, 最大20件
  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_gatheringList->clear();
  m_gatheringID    = INVALID_GATHERINGID;
  m_joinMaxNum     = joinNum;

  SetupCriteria( type, joinNum, gameMode, ruleList );

  if( m_matchingClient->BrowseMatchmakeSession( pContext, *m_criteriaInfo, range, m_gatheringList) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnSearchSessionCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  マッチメイクセッションへ参加する(非同期関数)
 *
 * @attention  事前にSearchSession()を実行してください。
 *             ホストユーザーは「CreateSession()」を使用してください。
 *
 * @note       非同期通信の結果
 *             正常成功時は「NexMatchmakeListener::OnMatchmakingJoinSuccess()」が
 *             エラー時は  「NexMatchmakeListener::OnMatchmakingMatchingFailed()」が
 *             コールされる。
 *             本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @param[in]  gathering   参加対象となるギャザリングホルダー
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::JoinSessionAsync( const nn::nex::GatheringHolder& gathering )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  SetPasswordWithGatheringID( gathering.Get()->GetID() );

  if( m_matchingClient->JoinMatchmakeSession( pContext, gathering.Get()->GetID(), m_sessionKey) )
  {
    m_isSimpleMatchmake = false;
    m_gatheringID = gathering.Get()->GetID();
    SetOwner( gathering.Get()->GetOwnerPID() );

    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnJoinSessionCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  マッチメイクセッションから離脱する
 *
 * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
 *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
 *
 * @note   自身がホストの場合は離脱は実行されない。
 *         別途「DeleteParticipation()」をコールしてください。
 *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingEndSuccess()」
 *         がコールされます。
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::EndParticipationAsync( void )
{
  // ホストは離脱しない
  if( IsOwner() )
  {
    GFL_PRINT( "I'm owner. skip EndParticipation()\n" );
    return true;
  }

  if(m_gatheringID==INVALID_GATHERINGID){
    //マッチしようとしたが相手がその瞬間消えて見つからなかった場合
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingEndSuccess();    // 切断通知
    }
    return true;
  }

  if( LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( pContext==NULL )
  {
    UnlockConnection();
    return false;
  }

  //マイグレーションコールバックの登録を解除しておく
  if( m_pHostMigrationCallback )
  {
    m_pHostMigrationCallback->Unregister( m_gatheringID );
  }

  if( m_matchingClient->EndParticipation(pContext, m_gatheringID) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnEndParticipationCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }


}


//------------------------------------------------------------------
/**
 * @brief  マッチメイクセッションを削除する(非同期関数)
 *
 * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
 *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
 *
 * @note   自身がホストのときのみ実行できます。
 *         参加ユーザーについては「EndParticipationAsync()」をコールしてください。
 *         セッション削除成功時は「NexMatchmakeListener::OnMatchmakingEndSuccess()」
 *         がコールされます。
 *
 * @return 実行に成功したならtrueを返却する
 */
//------------------------------------------------------------------
bool  NexMatchMakingClient::DeleteParticipationAsync( void )
{
  // ホスト以外は削除できない
  if( !IsOwner() ){
    GFL_PRINT( "I'm not owner. skip DeleteParticipationAsync()\n" );
    return true;
  }
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( pContext==NULL )
  {
    UnlockConnection();
    return false;
  }

  if( m_matchingClient->UnregisterGathering(pContext, m_gatheringID) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnDeleteParticipationCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}




//------------------------------------------------------------------
/**
 * @brief  コミュニティを作成・参加する(非同期関数)
 *
 * @attention  ユーザーはコミュニティーのオーナーとなる。
 * @attention  ユーザーが作成可能なコミュニティ数は4まで。
 * @attention  コミュニティは参加者が 0 になる、またはコミュニティのオーナーが明示的に
 * @attention  DeleteCommunity() によって削除しない限り、永続的にゲームサーバに残る。
 *
 * @note       非同期通信の結果
 *             正常成功時は「NexCommunityListener::OnCommunityCreateSuccess()」が
 *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
 *             コールされる。
 *             本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @param[in]  community      作成するコミュニティー情報
 * @param[in]  strMessage     参加者が固有に登録できるメッセージ。最大 nn::nex::MATCHMAKE_STRING_MAX_LENGTH 文字まで。
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::CreateCommunity( const nn::nex::Community& community, const nn::nex::String& strMessage )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_commGatheringID = INVALID_GATHERINGID;

  if( m_matchingClient->CreatePersistentGathering( pContext, community, &m_commGatheringID, strMessage ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnCreateCommunityCompletion, pContext );
    return true;
  }
  else
  {
    ReportCommunityFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  コミュニティに参加する(非同期関数)
 *
 * @attention  ユーザーが参加可能なコミュニティ数は16まで。
 * @attention  EndCommunity() で明示的に退出しない限りコミュニティへの参加状態は残り続ける
 *
 * @note       非同期通信の結果
 *             正常成功時は「NexCommunityListener::OnCommunityJoinSuccess()」が
 *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
 *             コールされる。
 *             本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @param[in]  gatheringId    参加対象となるコミュニティ用ギャザリングID。
 * @param[in]  strMessage     参加者が固有に登録できるメッセージ。最大 nn::nex::MATCHMAKE_STRING_MAX_LENGTH 文字まで。
 * @param[in]  strPassword    コミュニティのパスワード。コミュニティタイプがPasswordLockの場合のみ有効。
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::JoinCommunity( nn::nex::GatheringID gatheringId, const nn::nex::String& strMessage, const nn::nex::String& strPassword )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_commGatheringID = gatheringId;

  if( m_matchingClient->JoinPersistentGathering( pContext, gatheringId, strMessage, strPassword ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnJoinCommunityCompletion, pContext );
    return true;
  }
  else
  {
    ReportCommunityFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  コミュニティを参加者のPrincipalIdで検索する(非同期関数)
 *
 * @attention  FindCommunityByGatheringId(), FindOfficialCommunity()とは排他。
 *
 * @note       非同期通信の結果
 *             正常成功時は「NexCommunityListener::OnCommunitySearchSuccess()」が
 *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
 *             コールされる。
 *             本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @param[in]  principalId    検索対象となるプリンシバルID
 * @param[in]  resultRange    検索範囲条件
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::FindCommunityByParticipant( nn::nex::PrincipalID principalId, const nn::nex::ResultRange& resultRange )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_communityList->clear();

  if( m_matchingClient->FindPersistentGatheringByParticipant( pContext, principalId, resultRange, m_communityList ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnFindCommunityCompletion, pContext );
    return true;
  }
  else
  {
    ReportCommunityFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  コミュニティをコミュニティ用ギャザリングIDで検索する(非同期関数)
 *
 * @attention  FindCommunityByParticipant(), FindOfficialCommunity()とは排他。
 *
 * @note       非同期通信の結果
 *             正常成功時は「NexCommunityListener::OnCommunitySearchSuccess()」が
 *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
 *             コールされる。
 *             本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @attention  指定できるコミュニティ用ギャザリングID数は100件まで
 *
 * @param[in]  gatheringIdList 検索対象となるコミュニティ用ギャザリングID
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::FindCommunityByGatheringId( const nn::nex::qList<nn::nex::GatheringID>& gatheringIdList )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_communityList->clear();

  if( m_matchingClient->FindPersistentGatheringByGatheringId( pContext, gatheringIdList, m_communityList ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnFindCommunityCompletion, pContext );
    return true;
  }
  else
  {
    ReportCommunityFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  公式コミュニティを検索する(非同期関数)
 *
 * @attention  FindCommunityByParticipant(), FindCommunityByGatheringId()とは排他。
 *
 * @note       非同期通信の結果
 *             正常成功時は「NexCommunityListener::OnCommunitySearchSuccess()」が
 *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
 *             コールされる。
 *             本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @param[in]  isAvailableOnly 参加可能なコミュニティのみ検索対象に含めるならtrueを指定
 * @param[in]  resultRange     検索範囲条件
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::FindOfficialCommunity( bool isAvailableOnly, const nn::nex::ResultRange& resultRange )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_communityList->clear();

  if( m_matchingClient->FindOfficialPersistentGathering( pContext, isAvailableOnly, resultRange, m_communityList ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnFindCommunityCompletion, pContext );
    return true;
  }
  else
  {
    ReportCommunityFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  コミュニティから退出する(非同期通信）
 *
 * @param  gateringId 退出対象のコミュニティギャザリングID
 *
 * @note       非同期通信の結果
 *             正常成功時は「NexCommunityListener::OnCommunityLeaveSuccess()」が
 *             エラー時は  「NexCommunityListener::OnNexCommunityError()」が
 *             コールされる。
 *             本関数がfalseを返却した際は、上記イベントは通知されない。
 *
 * @return 非同期処理の開始に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::LeaveCommunity( nn::nex::GatheringID gateringId )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_commGatheringID = gateringId;

  if( m_matchingClient->EndParticipation(pContext, gateringId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnLeaveCommunityCompletion, pContext );
    return true;
  }
  else
  {
    ReportCommunityFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  コミュニティ用ギャザリングIDをコミュニティコードへ変換する
 *
 * @param gatheringId    変換元となるコミュニティ用ギャザリングID
 * @param communityCode  コミュニティコード格納先へのポインタ
 *
 * @return 変換に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::GatheringIdToPersistentGatheringCode( nn::nex::GatheringID gatheringId, u64 *communityCode )
{
  u64  code    = nn::nex::MatchmakeExtensionClient::GatheringIdToPersistentGatheringCode(gatheringId);
  bool isValid = nn::nex::MatchmakeExtensionClient::IsValidPersistentGatheringCode(code);

  if( isValid ){
    *communityCode = code;
  }
  return isValid;
}


//------------------------------------------------------------------
/**
 * @brief  コミュニティコードをコミュニティ用ギャザリングIDへ変換する
 *
 * @param communityCode  変換元となるコミュニティコード
 * @param gatheringId    コミュニティ用ギャザリングID格納先へのポインタ
 *
 * @return 変換に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::PersistentGatheringCodeToGatheringId( u64 communityCode, nn::nex::GatheringID *gatheringId )
{
  bool isValid = nn::nex::MatchmakeExtensionClient::IsValidPersistentGatheringCode(communityCode);

  if( isValid ){
    *gatheringId = nn::nex::MatchmakeExtensionClient::PersistentGatheringCodeToGatheringId(communityCode);
  }

  return isValid;
}


//------------------------------------------------------------------
/**
 * @brief  ギャザリングIDをもとに、セッション参加判定用パスワードを設定する
 *
 * @note   誤って別のマッチメイクセッションに参加しているクライアントが P2P通信に参加してしまわないよう、
 *         SetLocalIdentificationToken() で参加したギャザリングIDをセットする。
 *         ここでセットした値を CheckJoiningProcesses() でチェックしている
 *
 * @param  gatheringId  参加対象のギャザリングID
 *
 * @return none
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetPasswordWithGatheringID( GatheringID gatheringId )
{
  nn::nex::StringStream    password;

  // 別のマッチメイクセッションに参加しているクライアントが P2P通信に参加してしまわないよう
  // 参加したギャザリングIDをセット。
  password << gatheringId;
  nn::nex::Station::SetLocalIdentificationToken(password.CStr());
}

//------------------------------------------------------------------
/**
 * @brief  ランダムな1人のユーザーとマッチメイクする（非同期関数）
 *
 * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
 *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
 *
 * @note   ミラクル交換専用です。AutoMatchmakeの代わりに使います。
 * 
 *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingEndSuccess()」
 *         がコールされます。
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::SimpleMatchmakeAsync( void )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_matchingClient->SimpleMatchmake( pContext, MATCHMAKE_GROUP_RANDOM_TRADE, &m_isFound, m_pSimpleMatchmakeHostInfo ) )
  {
    m_isSimpleMatchmake = true;
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnSimpleMatchmakeCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}

//------------------------------------------------------------------
/**
 * @brief SimpleMatchmake用のP2Pセッション署名キーを登録する
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetSimpleMatchmakeP2pSessionKey( void )
{
  SetSessionSignatureKey( m_pSimpleMatchmakeHostInfo->GetSessionKey() );
}

//------------------------------------------------------------------
/**
 * @brief  ランダムな1人のユーザーとのマッチメイクにエントリーする（非同期関数）
 *
 * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
 *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
 *
 * @note   ミラクル交換専用です。AutoMatchmakeの代わりに使います。
 * 
 *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingEntrySimpleMatchmakeSuccess()」
 *         がコールされます。
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::EntrySimpleMatchmakeAsync( void )
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_matchingClient->EntrySimpleMatchmake( pContext, MATCHMAKE_GROUP_RANDOM_TRADE, m_pSimpleMatchmakeHostInfo->GetSessionKey() ) )
  {
    m_isSimpleMatchmake = true;
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnEntrySimpleMatchmakeCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}

//------------------------------------------------------------------
/**
 * @brief  ランダムな1人のユーザーとのマッチメイクにエントリーをキャンセルする（非同期関数）
 *
 * @attention 本関数は2つのスレッドで同時に呼び出すことは出来ません。
 *            また、NexManager::Dispatch() と同時に呼ぶこともできません。
 *
 * @note   ミラクル交換専用です。AutoMatchmakeの代わりに使います。
 * 
 *         セッション離脱成功時は「NexMatchmakeListener::OnMatchmakingCancelSimpleMatchmakeEntrySuccess()」
 *         がコールされます。
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
bool NexMatchMakingClient::CancelSimpleMatchmakeEntryAsync()
{
  if( LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_matchingClient->CancelSimpleMatchmakeEntry( pContext, MATCHMAKE_GROUP_RANDOM_TRADE, &m_isSucceed ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMatchMakingClient>(this, &NexMatchMakingClient::OnCancelSimpleMatchmakeEntryCompletion, pContext );
    return true;
  }
  else
  {
    ReportMatchmakeFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}

//------------------------------------------------------------------
/**
 * @brief  P2Pセッションの署名キーを登録する
 *
 * @note   マッチメイクセッションを確立した後にコールすること
 *
 * @param  sessionKey セッションキー
 */
//------------------------------------------------------------------
void NexMatchMakingClient::SetSessionSignatureKey( const nn::nex::qVector<qByte>& sessionKey )
{
  if( m_sessionKey != NULL )
  {
    nn::nex::StreamSettings& setting = nn::nex::Stream::GetSettings();

    nn::nex::Network* net = nn::nex::Network::GetInstance();

    if( net != NULL )
    {
      net->SetP2PDataPacketSessionSignatureKey(sessionKey);

      // P2Pセッションの暗号化が有効の場合は、ゲームサーバから通知されたキーを暗号化キーとして設定する
      if( m_isEncryption )
      {
        setting.SetIsEncryptionRequired(true);
        setting.SetEncryptionKey( sessionKey );
      }
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief コミュニティーNEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void NexMatchMakingClient::ReportMatchmakeError( const nn::nex::qResult& result )
{
  if( m_matchListener != NULL ){
    u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( result );
    m_matchListener->OnNexMatchmakeError( result, errorCode );
  }
}


//------------------------------------------------------------------
/**
 * @brief コミュニティーNEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void NexMatchMakingClient::ReportMatchmakeFunctionError( void )
{
  if( m_matchListener != NULL )
  {
    u32              errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
    nn::nex::qResult result(false);

    m_matchListener->OnNexMatchmakeError( result, errorCode );
  }
}


//------------------------------------------------------------------
/**
 * @brief コミュニティーNEX関数キャンセル報告処理
 */
//------------------------------------------------------------------
void NexMatchMakingClient::ReportMatchmakeCanceled( void )
{
  if( m_matchListener != NULL )
  {
    m_matchListener->OnMatchmakingCanceled();
  }
}


//------------------------------------------------------------------
/**
 * @brief  コミュニティー関連のエラー報告処理
 *
 * @param[in] result 実行結果
 */
//------------------------------------------------------------------
void NexMatchMakingClient::ReportCommunityError( const nn::nex::qResult& result )
{
  if( m_communityListener != NULL )
  {
    u32 errorCode = NexError::GetNetworkErrorCode( result );
    m_communityListener->OnNexCommunityError( result, errorCode );
  }
}


//------------------------------------------------------------------
/**
 * @brief コミュニティーNEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void NexMatchMakingClient::ReportCommunityFunctionError( void )
{
  if( m_communityListener != NULL )
  {
    u32              errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
    nn::nex::qResult result(false);

    m_communityListener->OnNexCommunityError( result, errorCode );
  }
}


//------------------------------------------------------------------
/**
 * @brief コミュニティーNEX関数キャンセル報告処理
 */
//------------------------------------------------------------------
void NexMatchMakingClient::ReportCommunityCanceled( void )
{
  if( m_communityListener != NULL )
  {
    m_communityListener->OnNexCommunityCanceled();
  }
}


//------------------------------------------------------------------
/**
 * @brief  CreateSessionAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnCreateSessionCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    SetPasswordWithGatheringID( m_gatheringID );
    SetSessionSignatureKey( *m_sessionKey );
    SetOwner( m_matchingClient->GetCredentials()->GetPrincipalID() );

    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingCreateSuccess( m_gatheringID );
    }
  }
  else
  {
    ReportMatchmakeError(result);
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  OpenSessionAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnOpenSessionCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingOpenSessionSuccess();
    }
  }
  else{
    ReportMatchmakeError( result );
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  SearchSessionAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnSearchSessionCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult ret = pContext->GetOutcome();

  if( (nn::nex::qBool)ret )
  {
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingSearchSuccess(m_gatheringList);
    }
  }
  else
  {
    ReportMatchmakeError(ret);
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  JoinSessionAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnJoinSessionCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult ret = pContext->GetOutcome();

  if( (nn::nex::qBool)ret )
  {
    SetSessionSignatureKey( *m_sessionKey );

    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingJoinSuccess(m_gatheringID);
    }
  }
  else
  {
    ReportMatchmakeError(ret);
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  AutoMatchmakeAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnAutoMatchmakeCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    nn::nex::Gathering*        gathering = m_autoSessionInfo->Get();
    nn::nex::MatchmakeSession* session   = reinterpret_cast<nn::nex::MatchmakeSession*>(gathering);

    m_gatheringID = gathering->GetID();
    SetPasswordWithGatheringID( m_gatheringID );
    SetSessionSignatureKey( session->GetSessionKey() );
    SetOwner( gathering->GetOwnerPID() );

    // 完了通知
    if( IsOwner() )
    {
      if( m_matchListener != NULL ){
        m_matchListener->OnMatchmakingCreateSuccess( m_gatheringID );
      }
    }
    else
    {
      m_gatheringList->clear();
      m_gatheringList->push_back( *m_autoSessionInfo );

      if( m_matchListener != NULL ){
        m_matchListener->OnMatchmakingSearchSuccess(m_gatheringList);
      }
    }
  }
  else
  {
    ReportMatchmakeError(result);
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  EndParticipation()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnEndParticipationCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  // 「すでにセッションが削除されている」以外は成功とみなす
  if( (result == QRESULT_ERROR(RendezVous, SessionVoid)) || ((nn::nex::qBool)result) )
  {
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingEndSuccess();    // 切断通知
    }
  }
  else
  {
    ReportMatchmakeError( result );
  }
  m_gatheringID = INVALID_GATHERINGID;
  m_isAccess    = false;
}


//------------------------------------------------------------------
/**
 * @brief  DeleteParticipation()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnDeleteParticipationCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingEndSuccess();    // 切断通知
    }
    m_gatheringID     = INVALID_GATHERINGID;
  }
  else
  {
    ReportMatchmakeError( result );
    m_gatheringID     = INVALID_GATHERINGID;
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  CreateCommunity()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnCreateCommunityCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCommunityCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_communityListener != NULL ){
      m_communityListener->OnCommunityCreateSuccess( m_commGatheringID );
    }
  }
  else
  {
    m_commGatheringID = INVALID_GATHERINGID;
    ReportCommunityError( result );
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  JoinCommunity()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnJoinCommunityCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCommunityCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_communityListener != NULL ){
      m_communityListener->OnCommunityJoinSuccess( m_commGatheringID );
    }
  }
  else
  {
    m_commGatheringID = INVALID_GATHERINGID;
    ReportCommunityError( result );
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  FindCommunity...()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnFindCommunityCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCommunityCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_communityListener != NULL ){
      m_communityListener->OnCommunitySearchSuccess( m_communityList );
    }
  }
  else
  {
    ReportCommunityError( result );
  }
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
 * @brief  LeaveCommunity()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnLeaveCommunityCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCommunityCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_communityListener != NULL ){
      m_communityListener->OnCommunityLeaveSuccess( m_commGatheringID );
    }
  }
  else
  {
    ReportCommunityError( result );
    m_commGatheringID = INVALID_GATHERINGID;
  }
  m_isAccess = false;
}



//------------------------------------------------------------------
/**
 * @brief  UpdateSessionHost()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnUpdateSessionHostCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingUpdateSessionHostSuccess();
    }
  }
  else{
    ReportMatchmakeError( result );
  }
  m_isAccess = false;
}

//------------------------------------------------------------------
/**
 * @brief  SimpleMatchmakeAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnSimpleMatchmakeCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    GFL_PRINT("*****SimpleMatchmake HotInfo*****\n");
    GFL_PRINT("[PID Info >> Host:%s OwnerPID:%d MyPID:%d]\n",
      m_isFound ? "found" : "notfound", 
      m_pSimpleMatchmakeHostInfo->GetPrincipalID(),
      m_matchingClient->GetCredentials()->GetPrincipalID() );


    SetOwner( m_pSimpleMatchmakeHostInfo->GetPrincipalID() );
    // ここではまだP2Pセッションキーをセットしない。サンプルではNATトラバーサル後です。

    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingSimpleMatchmakeSuccess();
    }
  }
  else
  {
    ReportMatchmakeError(result);
  }
  m_isAccess = false;
}

//------------------------------------------------------------------
/**
 * @brief  EntrySimpleMatchmakeAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnEntrySimpleMatchmakeCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingEntrySimpleMatchmakeSuccess();
    }
  }
  else
  {
    ReportMatchmakeError(result);
  }
  m_isAccess = false;
}

//------------------------------------------------------------------
/**
 * @brief  CancelSimpleMatchmakeEntryAsync()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMatchMakingClient::OnCancelSimpleMatchmakeEntryCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportMatchmakeCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    if( m_matchListener != NULL ){
      m_matchListener->OnMatchmakingCancelSimpleMatchmakeEntrySuccess();
    }
  }
  else
  {
    ReportMatchmakeError(result);
  }
  m_isAccess = false;
}


GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(gflnet2)

#endif

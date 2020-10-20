//=============================================================================
/**
 * @file   gflnet_NexRatingClient.cpp
 *
 * @brief  nn::nex::RatingClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================
#if defined(GF_PLATFORM_CTR)


#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
//#include "debug/DebugLogWindow.h"
#include "NexRatingClient.h" // 非公開ヘッダ

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)


//------------------------------------------------------------------
/**
 * @brief  NexRatingClientコンストラクタ
 */
//------------------------------------------------------------------
NexRatingClient::NexRatingClient( void ) :
  m_listener()
{
  m_ratingClient       = NULL;
  m_ratingSessionToken = NULL;
  m_isInit             = false;
  m_isAccess           = false;
  m_statusList         = NULL;
  m_context            = NULL;

  std::memset( m_reportStats, 0, sizeof(m_reportStats) );
}


//------------------------------------------------------------------
/**
 * @brief  NexRatingClientデストラクタ
 */
//------------------------------------------------------------------
NexRatingClient::~NexRatingClient()
{
  Finalize();
}


//------------------------------------------------------------------
/**
* @brief  NexRatingClientの初期化
*
* @param[in]  credentials     ゲームサーバーログイン情報
*
* @return 初期化に成功したならtrueを返却
*/
//------------------------------------------------------------------
bool NexRatingClient::Initialize( nn::nex::Credentials* credentials )
{
  if( !m_isInit )
  {
    m_isInit   = true;
    m_isAccess = false;
    m_context  = NULL;

    m_ratingClient       = qNew nn::nex::RatingClient();
    m_ratingSessionToken = qNew nn::nex::RatingSessionToken();
    m_statusList         = qNew nn::nex::qList<nn::nex::RatingStats>;

    for( u32 i = 0; i < (u32)E_SLOT_MAX; i++ )
    {
      if( m_reportStats[i] == NULL )
      {
        m_reportStats[i] = qNew nn::nex::RatingStats();
        if( m_reportStats[i] == NULL ){
          return false;
        }
      }
    }

    if( !m_ratingClient->Bind(credentials) ){
      return false;
    }
  }
  return true;
}


//------------------------------------------------------------------
/**
* @brief  NexRatingClientの終了
*
* @caution 必ず事前にIsConnecting()がfalseであることを確認してください
*/
//------------------------------------------------------------------
void NexRatingClient::Finalize( void )
{
  if( m_isInit )
  {
    m_isInit   = false;
    m_isAccess = false;
    m_context  = NULL;

    if( m_ratingClient != NULL )
    {
      if( m_ratingClient->GetCredentials() != NULL ){
        m_ratingClient->Unbind();
      }
    }

    for( u32 i = 0; i < (u32)E_SLOT_MAX; i++ )
    {
      gflnet2::nex::NexFreeMemory(&m_reportStats[i]);
    }
    gflnet2::nex::NexFreeCollection( &m_statusList );
    gflnet2::nex::NexFreeMemory(&m_ratingSessionToken);
    gflnet2::nex::NexFreeMemory(&m_ratingClient);
  }
}


//------------------------------------------------------------------
/**
* @brief レーティング用リスナの登録
*
* @param[in] listener 登録対象のメッセージ用リスナ。
*
* @return 登録に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexRatingClient::RegistRatingClientListener( NexRatingClientListener* listener )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] == listener ){
      return false; // 登録済み
    }
  }
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] == NULL )
    {
      m_listener[i] = listener;
      return true; // 登録成功
    }
  }

  return false;
}


//------------------------------------------------------------------
/**
* @brief レーティング用リスナの解除
*
* @param[in] listener 削除対象のメッセージ用リスナ。
*/
//------------------------------------------------------------------
void NexRatingClient::UnregistRatingClientListener( NexRatingClientListener* listener )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] == listener )
    {
      m_listener[i] = NULL;
      return;
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief 通信ロック状態化判定
 *
 * @return 通信ロック状態ならtrueを返却
 */
//------------------------------------------------------------------
bool NexRatingClient::IsAccessLock( void )
{
  if( m_isAccess || !m_isInit ){
    return true;   // 通信中, 未初期化
  }
  m_isAccess = true;
  return false;
}


//------------------------------------------------------------------
/**
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool NexRatingClient::IsConnecting( void ) const
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
bool NexRatingClient::CancelConnecting( void )
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
* @brief レーティングセッションを生成する（非同期通信）
*
* @note  通信成功時は「NexRatingClientListener::OnCreateRatingSessionSuccess()」がコールされる。
* @note  エラー時は「NexRatingClientListener::OnNexRatingClientError()」がコールされる。
* @note  本関数が失敗した場合はNexRatingClientListenerイベントはコールされない。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexRatingClient::CreateRatingSession( void )
{
  if( IsAccessLock() ){
    return false;   // 通信中, 未初期化
  }
  m_context = NULL;

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"RatingClient::CreateSession() \n" );
  #endif

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_ratingClient->CreateSession(pContext, m_ratingSessionToken) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexRatingClient>(this, &NexRatingClient::OnCreateSessionCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief レーティングセッションに参加する（非同期通信）
*
* @note  通信終了時は「NexRatingClientListener::OnJoinRatingSessionSuccess()」がコールされる。
* @note  エラー時は「NexRatingClientListener::OnNexRatingClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexRatingClient::JoinRatingSession( const nn::nex::qVector<qByte>& sessionKey )
{
  if( IsAccessLock() ){
    return false;   // 通信中, 未初期化
  }
  m_context = NULL;

  #if PM_DEBUG
  {
    GFL_PRINT("||||||||||||||||||||||||||||\n");
    GFL_PRINT("||||||||||||||||||||||||||||\n");
    GFL_PRINT("||||||||||||||||||||||||||||\n");
    GFL_PRINT("session token\n");
    GFL_PRINT("size = %d\n", sessionKey.size());

    nn::nex::qVector<qByte>::const_iterator it;
    int i=0;
    for( it=sessionKey.begin(); it != sessionKey.end(); it++ ){
      GFL_PRINT("index %d = %d\n", i++,*it );
    }
  }
  #endif

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"RatingClient::JoinSession() \n" );
  #endif

  m_ratingSessionToken->SetKey( sessionKey );

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_ratingClient->JoinSession(pContext, *m_ratingSessionToken) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexRatingClient>(this, &NexRatingClient::OnJoinSessionCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief 統計データをレーティングサーバに送信する（非同期通信）
*
* @attention 本関数を実行する前に必ずSetReportStatusInfo()にて2名分の結果情報をセットすること！
*
* @note  通信終了時は「NexRatingClientListener::OnRatingReportSuccess()」がコールされる。
* @note  エラー時は「NexRatingClientListener::OnNexRatingClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexRatingClient::ReportRatingStats( void )
{
  if( IsAccessLock() ){
    return false;   // 通信中, 未初期化
  }
  m_context = NULL;

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"RatingClient::ReportStats() \n" );
  #endif

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_statusList->clear();
  for( u32 i = 0; i < (u32)E_SLOT_MAX; i++ ){
    m_statusList->push_back( *m_reportStats[i] );
  }

  if( m_ratingClient->ReportStats(pContext, *m_ratingSessionToken, *m_statusList) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexRatingClient>(this, &NexRatingClient::OnReportRatingCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief 対戦結果データをセットする
*
* @attention ReportRatingStats()を実行する前に必ず本関数にて2名分の結果情報をセットすること！
*
* @param slot            設定するスロットID
* @param pid             プリンシバルID
* @param nexUniqueId     NEXユニークID
* @param categoryId      カテゴリId
* @param dissconnectFlag 通信切断フラグ
* @param rate            対戦結果レート
* @param battleLog       バトルログ情報
* @param battleLogSize   バトルログ情報サイズ
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexRatingClient::SetReportStatusInfo( REPORT_SLOT_ID slot,
                                           gflnet2::friends::PrincipalId pid,
                                           u64 nexUniqueId,
                                           u32 categoryId,
                                           nn::nex::RatingStatsFlags::RatingStatsFlags dissconnectFlag,
                                           REPORT_RATE rate,
                                           const void* battleLog,
                                           u32 battleLogSize ,
                                           BtlRecordResult2 result2 
                                           )

{
  if( m_isAccess || !m_isInit ){
    return false;   // 通信中, 未初期化
  }
  nn::nex::qVector<float> point;

  if( rate == E_RATE_WIN ){
    point.push_back( 1.0f );
  }
  else if( rate == E_RATE_DRAW ){
    point.push_back( 0.5f );
  }
  else{
    point.push_back( 0.0f );
  }

  if( result2 == BTL_RECORD_RESULT_2_SURRENDER )
  {//降参
    point.push_back( 2.0f );
  }
  else if( result2 == BTL_RECORD_RESULT_2_TIMEOVER )
  {//時間切れ
    point.push_back( 1.0f );
  }
  else
  {//瀕死
    point.push_back( 0.0f );
  }

  m_reportStats[slot]->SetCategory( categoryId );
  m_reportStats[slot]->SetPrincipalID( pid );
  m_reportStats[slot]->SetFlag( dissconnectFlag );
  m_reportStats[slot]->SetUniqueID( nexUniqueId );
  m_reportStats[slot]->SetListValues( point );

  // バトルログを設定
  if( (battleLog!=NULL) &&(battleLogSize!=0) )
  {
    nn::nex::qVector<qByte> logBuf;
    const qByte*            bytePtr = static_cast<const nn::nex::qByte*>(battleLog);
    logBuf.assign( bytePtr, bytePtr+battleLogSize );
    m_reportStats[slot]->SetBuffer( logBuf );
  }

  return true;
}


//--------------------------------------------------------------------------
/**
 *	@brief  レーティングセッショントークンを取得
 *  @param  key セッショントークンの格納先
 */
//--------------------------------------------------------------------------
void NexRatingClient::GetRatingSessionToken( nn::nex::qVector<qByte>& key )
{
  if( m_ratingSessionToken )
  {
    m_ratingSessionToken->GetKey(key);
  }
}


//------------------------------------------------------------------
/**
 * @brief エラー通知処理
 *
 * @param result 実行結果
 */
//------------------------------------------------------------------
void NexRatingClient::ReportError( const nn::nex::qResult& result )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexRatingClientError( result, gflnet2::nex::NexError::GetNetworkErrorCode(result) );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief キャンセル通知処理
 */
//------------------------------------------------------------------
void NexRatingClient::ReporCanceled( void )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexRatingClientCanceled();
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief 関数呼び出しエラー通知処理
 *
 * @param result
 */
//------------------------------------------------------------------
void NexRatingClient::ReportFunctionError( void )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
  nn::nex::qResult result(false);

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexRatingClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  レーティングセッション生成の完了通知検知用関数
 *
 * @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRatingClient::OnCreateSessionCompletion(nn::nex::CallContext *pContext)
{
  m_isAccess = false;
  m_context  = NULL;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReporCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    nn::nex::qVector<qByte> key;
    m_ratingSessionToken->GetKey(key);

    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnCreateRatingSessionSuccess( key );
      }
    }
  }
  else
  {
    ReportError( result );
  }
}


//------------------------------------------------------------------
/**
 * @brief  レーティングセッション参加の完了通知検知用関数
 *
 * @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRatingClient::OnJoinSessionCompletion(nn::nex::CallContext *pContext)
{
  m_isAccess = false;
  m_context  = NULL;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReporCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnJoinRatingSessionSuccess();
      }
    }
  }
  else
  {
    ReportError( result );
  }
}


//------------------------------------------------------------------
/**
 * @brief  レーティング報告の完了通知検知用関数
 *
 * @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRatingClient::OnReportRatingCompletion(nn::nex::CallContext *pContext)
{
  m_isAccess = false;
  m_context  = NULL;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReporCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnRatingReportSuccess();
      }
    }
  }
  else
  {
    ReportError( result );
  }
}


GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)


#endif // GF_PLATFORM_CTR

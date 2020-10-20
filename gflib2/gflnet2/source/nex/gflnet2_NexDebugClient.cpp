#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexDebugClient.cpp
 *
 * @brief  nn::nex::DebugClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================

#ifdef ENABLE_NEX_DEBUG_CLIENT

#include <gflnet2/include/nex/gflnet2_NexDebugClient.h>


namespace gflnet2{
namespace nex{



//------------------------------------------------------------------
/**
 * @brief  NexDebugClientコンストラクタ
 */
//------------------------------------------------------------------
NexDebugClient::NexDebugClient( void )
{
  m_isInit                   = false;
  m_isSending                = false;
  m_isEnableApiRecorder      = false;
  m_isAvoidPreviousGathering = false;
  //
  m_debugClient              = NULL;
  m_context                  = NULL;
  //
  m_apiCallData              = NULL;
  m_apiCallSummary           = NULL;
  //
  ::std::memset( m_listener, 0, sizeof(m_listener) );
}


//------------------------------------------------------------------
/**
 * @brief  NexDebugClientデストラクタ
 */
//------------------------------------------------------------------
NexDebugClient::~NexDebugClient()
{
  Finalize();
}


//------------------------------------------------------------------
/**
* @brief  MessageClientの初期化
*
* @param[in]  credentials     ゲームサーバーログイン情報
*
* @return 初期化に成功したならtrueを返却
*/
//------------------------------------------------------------------
bool NexDebugClient::Initialize( nn::nex::Credentials* credentials )
{
  if( !m_isInit )
  {
    m_isInit      = true;
    m_isSending   = false;

    m_debugClient = qNew nn::nex::DebugClient();

    if( !m_debugClient->Bind(credentials) ){
      return false;
    }
    m_apiCallData    = qNew nn::nex::qList< nn::nex::ApiCallData>;
    m_apiCallSummary = qNew nn::nex::qList< nn::nex::ApiCallSummary>;
  }
  return true;
}


//------------------------------------------------------------------
/**
* @brief  MessageClientの終了
*
* @attention 必ず事前にIsConnecting()がfalseであることを確認してください
*/
//------------------------------------------------------------------
void NexDebugClient::Finalize( void )
{
  if( m_isInit )
  {
    m_isInit      = false;
    m_isSending   = false;

    std::memset( m_listener, 0, sizeof(m_listener) );

    if( m_debugClient != NULL )
    {
      if( m_debugClient->GetCredentials() != NULL ){
        m_debugClient->Unbind();
      }
    }

    NexFreeCollection(&m_apiCallData);
    NexFreeCollection(&m_apiCallSummary);
  }
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバデバッグ補助機能用リスナの登録
*
* @param[in] listener 登録対象のゲームサーバデバッグ補助機能用リスナ。
*
* @return 登録に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexDebugClient::RegistDebugClientListener( NexDebugClientListener* listener )
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
* @brief ゲームサーバデバッグ補助機能用リスナの解除
*
* @param[in] listener 削除対象のゲームサーバデバッグ補助機能用リスナ。
*/
//------------------------------------------------------------------
void NexDebugClient::UnregistDebugClientListener( NexDebugClientListener* listener )
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
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool NexDebugClient::IsConnecting( void ) const
{
  return m_isSending;
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
bool NexDebugClient::CancelConnecting( void )
{
  if( m_isSending && (m_context != NULL) )
  {
    if( m_context->GetState() == nn::nex::CallContext::CallInProgress ){
      return m_context->Cancel();
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
* @brief 通信ロックを行う
*
* @return すでに通信接続ロック中であればfalseを返却
*/
//------------------------------------------------------------------
bool NexDebugClient::LockConnection( void )
{
  if( m_isSending || !m_isInit )
  {
    return false;   // 通信中, 未初期化
  }
  m_isSending = true;

  return true;
}


//------------------------------------------------------------------
/**
* @brief 通信ロックを解除する
*/
//------------------------------------------------------------------
void NexDebugClient::UnlockConnection( void )
{
  m_isSending = false;
  m_context   = NULL;
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバのAPIレコード機能設定を変更する(非同期通信）
*
* @param[in] isEnable APIレコード機能を有効化するならtrueを指定
*
* @note  正常終了時は「NexDebugClientListener::OnNexDebugClientChangeStatusSuccess()」がコールされる。
* @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDebugClient::ChangeApiRecorderStatus( bool isEnable )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  bool isSuccess;

  nn::nex::ProtocolCallContext *pContext = qNew nn::nex::ProtocolCallContext();

  if( isEnable ){
    isSuccess = m_debugClient->EnableApiRecorder(pContext);
  }
  else{
    isSuccess = m_debugClient->DisableApiRecorder(pContext);
  }

  if( isSuccess )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDebugClient>(this, &NexDebugClient::ChangeApiRecorderStatusCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバのAPIレコード機能設定値を取得する(非同期通信）
*
* @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetStatusSuccess()」がコールされる。
* @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDebugClient::GetApiRecorderStatus( void )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = qNew nn::nex::ProtocolCallContext();

  if( m_debugClient->IsApiRecorderEnabled(pContext, &m_isEnableApiRecorder) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDebugClient>(this, &NexDebugClient::OnGetApiRecorderStatusCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief オートマッチの連続マッチ有効設定を変更する(非同期通信）
*
* @param[in] isEnable オートマッチで過去参加したマッチメイクセッションを参加対象外とするかどうか。
* @param[in] isEnable trueの場合、過去に参加したマッチメイクセッションに参加しない(ゲームサーバのデフォルト設定)。
* @param[in] isEnable false の場合、過去に参加したマッチメイクセッションに参加する。
*
* @note  正常終了時は「NexDebugClientListener::OnNexDebugClientChangeExecludeJoinedMatchmakeSuccess()」がコールされる。
* @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDebugClient::ChangeExcludeJoinedMatchmake( bool isEnable )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = qNew nn::nex::ProtocolCallContext();

  if( m_debugClient->SetExcludeJoinedMatchmakeSession(pContext, isEnable) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDebugClient>(this, &NexDebugClient::OnChangeExcludeJoinedMatchmakeCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief オートマッチの連続マッチ有効設定値を取得する
*
* @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetExecludeJoinedMatchmakeSuccess()」がコールされる。
* @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDebugClient::GetExecludeJoinedMatchmake( void )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = qNew nn::nex::ProtocolCallContext();

  if( m_debugClient->GetExcludeJoinedMatchmakeSession(pContext, &m_isAvoidPreviousGathering) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDebugClient>(this, &NexDebugClient::OnGetExecludeJoinedMatchmakeCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief API呼び出し情報を取得する(非同期通信）
*
* @param[in] principalId 取得対象のプリンシパルID
* @param[in] sinceTime   取得開始時刻
* @param[in] untilTime   取得終了時刻
*
* @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetApiCallsSuccess()」がコールされる。
* @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDebugClient::GetApiCalls( nn::nex::PrincipalID principalId,
                                  nn::nex::DateTime sinceTime, nn::nex::DateTime untilTime )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::qList< nn::nex::PrincipalID > pidList;
  pidList.push_back(principalId);

  nn::nex::ProtocolCallContext *pContext = qNew nn::nex::ProtocolCallContext();

  if( m_debugClient->GetApiCalls(pContext, m_apiCallData, pidList, sinceTime, untilTime ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDebugClient>(this, &NexDebugClient::OnGetApiCallsCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief APIコール詳細情報を取得する(非同期通信）
*
* @param[in] principalId     取得対象のプリンシパルID
* @param[in] sinceTime       取得開始時刻
* @param[in] untilTime       取得終了時刻
* @param[in] isLimitOverOnly 呼び出し頻度制限の目安を超えているAPIのみ取得するならtrueを指定
*
* @note  正常終了時は「NexDebugClientListener::OnNexDebugClientGetApiCallSummarySuccess()」がコールされる。
* @note  エラー時は「NexDebugClientListener::OnNexDebugClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDebugClient::GetApiCallSummary( nn::nex::PrincipalID principalId,
                                        nn::nex::DateTime sinceTime, nn::nex::DateTime untilTime,
                                        bool isLimitOverOnly )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = qNew nn::nex::ProtocolCallContext();

  if( m_debugClient->GetApiCallSummary(pContext, m_apiCallSummary, principalId, sinceTime, untilTime, isLimitOverOnly ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDebugClient>(this, &NexDebugClient::OnGetApiCallSummaryCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionError();
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief エラー報告処理
 *
 * @param[in] result 実行結果
 */
//------------------------------------------------------------------
void NexDebugClient::ReportError( const nn::nex::qResult& result )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( result );

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDebugClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief NEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void NexDebugClient::ReportFunctionError( void )
{
  u32              errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
  nn::nex::qResult result(false);

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDebugClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief キャンセル報告処理
 */
//------------------------------------------------------------------
void NexDebugClient::ReportCanceled( void )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDebugClientCanceled();
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  ChangeApiRecorderStatusの完了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDebugClient::ChangeApiRecorderStatusCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexDebugClientChangeStatusSuccess();
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
 * @brief  GetApiRecorderStatusの完了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDebugClient::OnGetApiRecorderStatusCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexDebugClientGetStatusSuccess(m_isEnableApiRecorder);
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
 * @brief  ChangeExcludeJoinedMatchmakeの完了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDebugClient::OnChangeExcludeJoinedMatchmakeCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexDebugClientChangeExecludeJoinedMatchmakeSuccess();
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
 * @brief  GetExecludeJoinedMatchmakeの完了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDebugClient::OnGetExecludeJoinedMatchmakeCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexDebugClientGetExecludeJoinedMatchmakeSuccess(m_isAvoidPreviousGathering);
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
 * @brief  GetApiCallsの完了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDebugClient::OnGetApiCallsCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexDebugClientGetApiCallsSuccess(m_apiCallData);
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
 * @brief  GetApiCallSummaryの完了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDebugClient::OnGetApiCallSummaryCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexDebugClientGetApiCallSummarySuccess(m_apiCallSummary);
      }
    }
  }
  else
  {
    ReportError( result );
  }
}


} //nex
} //gflnet

#endif
#endif

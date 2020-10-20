#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMessageClient.cpp
 *
 * @brief  nn::nex::MessageClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================


#include <gflnet2/include/nex/gflnet2_NexMessageClient.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(nex)



//------------------------------------------------------------------
/**
 * @brief  NexMessageClientコンストラクタ
 */
//------------------------------------------------------------------
NexMessageClient::NexMessageClient( void )
{
  m_isInit         = false;
  m_isSending      = false;
  //
  m_messageClient  = NULL;
  m_context        = NULL;
  m_messageHolder  = NULL;
  m_messageSubject = NULL;
  m_messageBody    = NULL;
  //
  std::memset( m_listener, 0, sizeof(m_listener) );
}


//------------------------------------------------------------------
/**
 * @brief  NexMessageClientデストラクタ
 */
//------------------------------------------------------------------
NexMessageClient::~NexMessageClient()
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
bool NexMessageClient::Initialize( nn::nex::Credentials* credentials )
{
  if( !m_isInit )
  {
    m_isInit      = true;
    m_isSending   = false;

    m_messageClient = qNew nn::nex::MessagingClient();

    if( !m_messageClient->Bind(credentials) ){
      return false;
    }
    m_messageClient->RegisterMessagingNotificationHandler( this );
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
void NexMessageClient::Finalize( void )
{
  if( m_isInit )
  {
    m_isInit      = false;
    m_isSending   = false;

    std::memset( m_listener, 0, sizeof(m_listener) );

    if( m_messageClient != NULL )
    {
      m_messageClient->UnregisterMessagingNotificationHandler();
      if( m_messageClient->GetCredentials() != NULL ){
        m_messageClient->Unbind();
      }
    }

    NexFreeMemory(&m_messageBody);
    NexFreeMemory(&m_messageSubject);
    NexFreeMemory(&m_messageHolder);
    NexFreeMemory(&m_messageClient);
  }
}


//------------------------------------------------------------------
/**
* @brief メッセージ用リスナの登録
*
* @param[in] listener 登録対象のメッセージ用リスナ。
*
* @return 登録に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexMessageClient::RegistMessageClientListener( NexMessageClientListener* listener )
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
* @brief メッセージ用リスナの解除
*
* @param[in] listener 削除対象のメッセージ用リスナ。
*/
//------------------------------------------------------------------
void NexMessageClient::UnregistMessageClientListener( NexMessageClientListener* listener )
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
bool NexMessageClient::IsConnecting( void ) const
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
bool NexMessageClient::CancelConnecting( void )
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
* @brief 通信が可能か判定
*
* @return 通信可能ならtrueを返却する
*/
//------------------------------------------------------------------
bool NexMessageClient::IsEnableSending( void ) const
{
  return (!m_isSending && m_isInit);
}


//------------------------------------------------------------------
/**
* @brief 通信ロックを行う
*
* @return すでに通信接続ロック中であればfalseを返却
*/
//------------------------------------------------------------------
bool NexMessageClient::LockConnection( void )
{
  if( !IsEnableSending() )
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
void NexMessageClient::UnlockConnection( void )
{
  m_context   = NULL;
  m_isSending = false;
}


//------------------------------------------------------------------
/**
* @brief 指定ユーザーにメッセージを送信する
*
* @param[in] pid         送信対象ユーザーのプリンシバルID
* @param[in] subject     タイトル文字列。文字列は内部ワークにコピーされる。
* @param[in] messageBody 送信するメッセージ文字列。文字列は内部ワークにコピーされる。
*
* @note  正常終了時は「NexMessageClientListener::OnNexMessageSendingSuccess()」がコールされる。
* @note  エラー時は「NexMessageClientListener::OnNexMessageClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexMessageClient::SendInstant( const nn::nex::PrincipalID pid, const gfl2::str::STRCODE* subject, const gfl2::str::STRCODE* messageBody )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  SetupMessageHolder( subject, messageBody );

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_messageClient->SendInstant(pContext , pid, *m_messageHolder) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMessageClient>(this, &NexMessageClient::OnSendingCompletion, pContext );
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
* @brief 複数指定ユーザーにメッセージを送信する
*/
//------------------------------------------------------------------
bool NexMessageClient::SendInstantMultiTarget( const nn::nex::PrincipalID* pidArray, const gfl2::str::STRCODE* subject, const gfl2::str::STRCODE* messageBody )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  SetupMessageHolder( subject, messageBody );

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  nn::nex::qVector<nn::nex::PrincipalID> pids;
  for( u32 index = 0 ; index < 100 ; ++index )
  {
    if( pidArray[index] == 0 )
      break;

    pids.push_back( pidArray[index] );
  }

  if( m_messageClient->SendInstant(pContext , pids, *m_messageHolder) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMessageClient>(this, &NexMessageClient::OnSendingCompletion, pContext );
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
* @brief マッチメイク参加ユーザー（自分も含む）にメッセージを送信する
*
* @param[in] gid         送信対象マッチメイクのギャザリングID
* @param[in] subject     タイトル文字列。文字列は内部ワークにコピーされる。
* @param[in] messageBody 送信するメッセージ文字列。文字列は内部ワークにコピーされる。
*
* @note  正常終了時は「NexMessageClientListener::OnNexMessageSendingSuccess()」がコールされる。
* @note  エラー時は「NexMessageClientListener::OnNexMessageClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexMessageClient::SendChat( const nn::nex::GatheringID gid, const gfl2::str::STRCODE* subject, const gfl2::str::STRCODE* messageBody )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  SetupMessageHolder( subject, messageBody );

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_messageClient->SendChat(pContext, gid, *m_messageHolder) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexMessageClient>(this, &NexMessageClient::OnSendingCompletion, pContext );
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
* @brief MessageHolderのセットアップ
*
* @param[in] subject     タイトル文字列。文字列は内部ワークにコピーされる。
* @param[in] messageBody 送信するメッセージ文字列。文字列は内部ワークにコピーされる。
*/
//------------------------------------------------------------------
void NexMessageClient::SetupMessageHolder( const gfl2::str::STRCODE* subject, const gfl2::str::STRCODE* messageBody )
{
  NexFreeMemory(&m_messageHolder);
  NexFreeMemory(&m_messageBody);
  NexFreeMemory(&m_messageSubject);

  m_messageSubject = qNew nn::nex::String();
  m_messageBody    = qNew nn::nex::String();

  // オペレータを利用した代入
  if( subject != NULL ){
    *m_messageSubject = subject;
  }
  if( messageBody != NULL ){
    *m_messageBody = messageBody;
  }
  nn::nex::TextMessage *pTextMessage = qNew nn::nex::TextMessage();

  pTextMessage->SetSubject( *m_messageSubject );
  pTextMessage->SetTextBody( *m_messageBody );

  m_messageHolder = qNew nn::nex::UserMessageDataHolder( pTextMessage );
}


//------------------------------------------------------------------
/**
 * @brief エラー報告処理
 *
 * @param[in] result 実行結果
 */
//------------------------------------------------------------------
void NexMessageClient::ReportError( const nn::nex::qResult& result )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( result );

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexMessageClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief NEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void NexMessageClient::ReportFunctionError( void )
{
  u32              errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
  nn::nex::qResult result(false);

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexMessageClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief キャンセル報告処理
 */
//------------------------------------------------------------------
void NexMessageClient::ReportCanceled( void )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexMessageCanceled();
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  メッセージ送信非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexMessageClient::OnSendingCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }

  nn::nex::TextMessage* pTextMsg = NULL;
  nn::nex::qResult     result = pContext->GetOutcome();

  if( m_messageHolder != NULL ){
    pTextMsg = static_cast<nn::nex::TextMessage*>(m_messageHolder->Get());
  }

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexMessageSendingSuccess( pTextMsg );
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
 * @brief  MessagingNotificationHandler用Message受信イベント
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in]  hMsg 受信メッセージ
 */
//------------------------------------------------------------------
void NexMessageClient::DeliverInstantMessage( const nn::nex::DataHolder &hMsg )
{
  nn::nex::TextMessage* pTextMsg = static_cast<nn::nex::TextMessage*>(hMsg.Get());

  if( pTextMsg != NULL )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL )
      {
        m_listener[i]->OnNexMessageReceived( pTextMsg );
      }
    }
  }
}


GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(gflnet2)
#endif

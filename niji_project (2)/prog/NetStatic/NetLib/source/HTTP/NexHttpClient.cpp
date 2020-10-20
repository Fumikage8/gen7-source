//======================================================================
/**
 * @file  NexHttpClient.cpp
 *
 * @brief NEX拡張HTTPライブラリ用ラッパークラス
 *
 * @author  suzuki
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#include <NetStatic/NetLib/include/HTTP/NexHttpClient.h>
#include "NexHttpConnection.h"
//#include "debug/DebugLogWindow.h"

#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(HTTP)

//------------------------------------------------------------------
/**
* @brief コンストラクタ
*/
//------------------------------------------------------------------
NexHttpClient::NexHttpClient( void )
{
  m_isInit      = false;
  m_isAccess    = false;
  //
  m_context     = NULL;
  m_httpClient  = NULL;
  m_connection  = NULL;
  //
  std::memset( m_listener, 0, sizeof(m_listener) );
}


//------------------------------------------------------------------
/**
* @brief デストラクタ
*/
//------------------------------------------------------------------
NexHttpClient::~NexHttpClient()
{
  Finalize();
}


//------------------------------------------------------------------
/**
* @brief 初期化
*
* @return 初期化に成功したならtrueを返却
*/
//------------------------------------------------------------------
bool NexHttpClient::Initialize( void )
{
  if( m_isInit ){
    Finalize();
  }
  m_isInit   = true;
  m_isAccess = false;
  m_context  = NULL;

  m_httpClient = qNew nn::nex::SimpleHttpClient();
  m_connection = qNew NetLib::HTTP::NexHttpConnection();

  return (m_httpClient!=NULL) && (m_connection != NULL);
}


//------------------------------------------------------------------
/**
* @brief 終了処理
*
* @
*/
//------------------------------------------------------------------
void NexHttpClient::Finalize( void )
{
  if( m_isInit )
  {
    gflnet2::nex::NexFreeMemory(&m_httpClient);
    gflnet2::nex::NexFreeMemory(&m_connection);
    m_isInit   = false;
    m_isAccess = false;
  }
}


//------------------------------------------------------------------
/**
* @brief NexHttpClient用リスナの登録
*
* @param[in] listener 登録対象のNexHttpClient用リスナ。
*
* @return 登録に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexHttpClient::RegistNexHttpClientListener( NexHttpClientListener* listener )
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
* @brief NexHttpClient用リスナの解除
*
* @param[in] listener 削除対象のNexHttpClient用リスナ。
*/
//------------------------------------------------------------------
void NexHttpClient::UnregistNexHttpClientListener( NexHttpClientListener* listener )
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
* @brief Post通信の開始
*
* @param[in] setting         アクセス設定情報
* @param[in] relayBufferSize 一度にバッファに読み書きする最大サイズ。 この関数に渡される最大バッファサイズを指定します。
*/
//------------------------------------------------------------------
bool NexHttpClient::StartPost( const HttpConnectSetting &setting, qUnsignedInt32 relayBufferSize )
{
  if( (m_httpClient == NULL) || m_isAccess ){
    return false;
  }
  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  nn::nex::String               url( setting.url );

  if( pContext == NULL ){
    return false;
  }
  m_isAccess = true;
  m_context  = NULL;

  m_connection->ResetReceiveBuffer();
  m_connection->SetSetting( setting );

  pContext->SetTimeout( setting.timeoutMillSeconds );

  GFL_PRINT( "StartPost[%s]\n",setting.url );

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"HttpClient::Post() \n" );
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"[%s] \n", setting.url );
  #endif

  if( m_httpClient->Post( pContext, url, m_connection, relayBufferSize ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexHttpClient>(this, &NexHttpClient::OnConnectCompletion, pContext );
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
* @brief Get通信の開始
*
* @param[in] url             アクセス設定情報
* @param[in] relayBufferSize 一度にバッファに読み書きする最大サイズ。 この関数に渡される最大バッファサイズを指定します。
*/
//------------------------------------------------------------------
bool NexHttpClient::StartGet( const HttpConnectSetting &setting, qUnsignedInt32 relayBufferSize )
{
  if( (m_httpClient == NULL) || m_isAccess ){
    return false;
  }
  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  nn::nex::String               url( setting.url );

  if( pContext == NULL ){
    return false;
  }

  m_isAccess = true;
  m_context  = NULL;

  m_connection->ResetReceiveBuffer();
  m_connection->SetSetting( setting );

  pContext->SetTimeout( setting.timeoutMillSeconds );

  GFL_PRINT( "StartGet[%s]\n",setting.url );

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"HttpClient::Get() \n" );
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"[%s] \n", setting.url );
  #endif

  if( m_httpClient->Get( pContext, url, m_connection, relayBufferSize ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexHttpClient>(this, &NexHttpClient::OnConnectCompletion, pContext );
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
 * @brief 通信キャンセルリクエストを発行する。
 * @brief キャンセルが実施された場合、完了時にOnNexHttpConnectionCanceled()がコールされる。
 *
 * @note  通信状態にかかわらず、コールしても良い
 *
 * @return キャンセル処理が実施されたならtrueを返却する
 */
//------------------------------------------------------------------
bool NexHttpClient::CancelConnecting( void )
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
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool NexHttpClient::IsConnecting( void ) const
{
  return m_isAccess;
}


//------------------------------------------------------------------
/**
 * @brief キャンセルイベントリスナ通知
 */
//------------------------------------------------------------------
void NexHttpClient::ReportCanceled( void )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexHttpConnectionCanceled();
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief エラーのイベントリスナ通知
 *
 * @param result 実行結果
 */
//------------------------------------------------------------------
void NexHttpClient::ReportError( const nn::nex::qResult& result )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( result );

   for( u32 i = 0; i < LISTENER_NUM; i++ )
   {
     if( m_listener[i] != NULL ){
       m_listener[i]->OnNexHttpConnectionError( result, errorCode );
     }
   }
}


//------------------------------------------------------------------
/**
 * @brief NEX関数呼び出しエラーのイベントリスナ通知
 */
//------------------------------------------------------------------
void NexHttpClient::ReportFunctionError( void )
{
  u32              errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
  nn::nex::qResult result(false);

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexHttpConnectionError( result, errorCode );
    }
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
void NexHttpClient::OnConnectCompletion(nn::nex::CallContext *pContext)
{
  m_isAccess = false;
  m_context  = NULL;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  // 成功
  if( (qBool)result )
  {
    if( m_connection != NULL )
    {
      for( u32 i = 0; i < LISTENER_NUM; i++ )
      {
        if( m_listener[i] != NULL ){
          m_listener[i]->OnNexHttpConnectionSuccess( m_connection->GetHttpResponse() );
        }
      }
    }
  }
  // エラー
  else
  {
    ReportError( result );
  }
}


GFL_NAMESPACE_END()
GFL_NAMESPACE_END()
#endif

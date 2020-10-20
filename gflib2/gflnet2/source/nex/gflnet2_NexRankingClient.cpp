#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexRankingClient.cpp
 *
 * @brief  nn::nex::RankingClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================

#include <gflnet2/include/nex/gflnet2_NexRankingClient.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>



namespace gflnet2{
namespace nex{



//------------------------------------------------------------------
/**
 * @brief  NexRankingClientコンストラクタ
 */
//------------------------------------------------------------------
NexRankingClient::NexRankingClient( void )
{
  m_isInit       = false;
  m_isAccess     = false;
  m_appproxOrder = 0;
  m_categoryId   = 0;
  //
  m_rankingClient = NULL;
  m_ratingClient = NULL;
  m_context       = NULL;
  m_commonData    = NULL;
  m_rankResultInf = NULL;
  m_rankData = NULL;
  m_rankStats     = NULL;
  //
  ::std::memset( m_listener, 0, sizeof(m_listener) );
}


//------------------------------------------------------------------
/**
 * @brief  NexMessageClientデストラクタ
 */
//------------------------------------------------------------------
NexRankingClient::~NexRankingClient()
{
  Finalize();
}


//------------------------------------------------------------------
/**
* @brief  RankingClientの初期化
*
* @param[in]  credentials     ゲームサーバーログイン情報
*
* @return 初期化に成功したならtrueを返却
*/
//------------------------------------------------------------------
bool NexRankingClient::Initialize( nn::nex::Credentials* credentials )
{
  if( !m_isInit )
  {
    m_isInit         = true;
    m_isAccess       = false;

    m_rankingClient = qNew nn::nex::RankingClient();
    m_ratingClient = qNew nn::nex::RatingClient();
    m_commonData    = qNew nn::nex::qVector<qByte>;
    m_rankResultInf = qNew nn::nex::RankingResult();
    m_rankData = qNew nn::nex::RatingRankData();
    m_rankStats     = qNew nn::nex::RankingStats();

    if( !m_rankingClient->Bind(credentials) )
    {
      return false;
    }

    if( !m_ratingClient->Bind(credentials) )
    {
      return false;
    }
  }
  return true;
}


//------------------------------------------------------------------
/**
* @brief  RankingClientの終了
*
* @attention 必ず事前にIsConnecting()がfalseであることを確認してください
*/
//------------------------------------------------------------------
void NexRankingClient::Finalize( void )
{
  if( m_isInit )
  {
    m_isInit      = false;
    m_isAccess    = false;

    std::memset( m_listener, 0, sizeof(m_listener) );

    if( m_rankingClient != NULL )
    {
      if( m_rankingClient->GetCredentials() != NULL ){
        m_rankingClient->Unbind();
      }
    }

    if( m_ratingClient != NULL )
    {
      if( m_ratingClient->GetCredentials() != NULL ){
        m_ratingClient->Unbind();
      }
    }

    NexFreeCollection(&m_commonData);
    NexFreeMemory(&m_rankStats);
    NexFreeMemory(&m_rankData);
    NexFreeMemory(&m_rankResultInf);
    NexFreeMemory(&m_ratingClient);
    NexFreeMemory(&m_rankingClient);
  }
}


//------------------------------------------------------------------
/**
* @brief  RankingClientインスタンスの取得
*/
//------------------------------------------------------------------
nn::nex::RankingClient* NexRankingClient::GetRankingClient( void )
{
  return m_rankingClient;
}


//------------------------------------------------------------------
/**
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool NexRankingClient::IsConnecting( void ) const
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
bool NexRankingClient::CancelConnecting( void )
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
* @brief 通信ロックを行う
*
* @param none
*
* @return すでに通信接続ロック中であればfalseを返却
*/
//------------------------------------------------------------------
bool NexRankingClient::LockConnection( void )
{
  if( m_isAccess || !m_isInit )
  {
    return false;   // 通信中, 未初期化
  }
  m_isAccess = true;

  return true;
}


//------------------------------------------------------------------
/**
* @brief 通信ロックを解除する
*/
//------------------------------------------------------------------
void NexRankingClient::UnlockConnection( void )
{
  m_context  = NULL;
  m_isAccess = false;
}


//------------------------------------------------------------------
/**
* @brief ランキング用リスナの登録
*
* @param[in] listener 登録対象のランキング用リスナ。
*
* @return 登録に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexRankingClient::RegistRankingClientListener( NexRankingClientListener* listener )
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
* @brief ランキング用リスナの解除
*
* @param[in] listener 削除対象のランキング用リスナ。
*/
//------------------------------------------------------------------
void NexRankingClient::UnregistRankingClientListener( NexRankingClientListener* listener )
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
* @brief スコア情報を登録する(非同期関数)
*
* @param[in] scoreData    スコア情報
* @param[in] nexUniqueId  NEXユニークID（省略可能）
*
* @attention 自分と同じプリンシパルIDのスコア情報のみ登録可能。
*
* @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingDeleteScoreSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::UploadScore( const nn::nex::RankingScoreData& scoreData, nn::nex::UniqueId nexUniqueId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_categoryId = scoreData.GetCategory();

  if( m_rankingClient->UploadScore(pContext, scoreData, nexUniqueId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnUploadScoreCompletion, pContext );
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
* @brief 共通データを登録する(非同期関数)
*
* @param[in] pCommonData    共通情報へのポインタ
* @param[in] commonDataSize 共通情報データサイズ(最大255バイト）
* @param[in] nexUniqueId    NEXユニークID（省略可能）
*
* @attention 自分と同じプリンシパルIDの共通データのみ登録可能。
*
* @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingUploadCommonDataSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::UploadCommonData( const void* pCommonData, qByte commonDataSize, nn::nex::UniqueId nexUniqueId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_ratingClient->UploadCommonData(pContext , pCommonData, commonDataSize, nexUniqueId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnUploadCommonDataCompletion, pContext );
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
* @brief スコア情報を削除する(非同期関数)
*
* @param[in] categoryId   削除対象カテゴリ
* @param[in] nexUniqueId  NEXユニークID（省略可能）
*
* @attention 自分と同じプリンシパルIDのスコア情報のみ削除可能。
*
* @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingDeleteScoreSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::DeleteScore( u32 categoryId, nn::nex::UniqueId nexUniqueId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_categoryId = categoryId;

  if( m_ratingClient->DeleteScore(pContext, categoryId, nexUniqueId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnDeleteScoreCompletion, pContext );
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
* @brief 共通データを削除する(非同期関数)
*
* @param[in] nexUniqueId  NEXユニークID（省略可能）
*
* @attention 自分と同じプリンシパルIDのスコア情報のみ削除可能。
*
* @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingDeleteScoreSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::DeleteCommonData( nn::nex::UniqueId nexUniqueId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_rankingClient->DeleteCommonData(pContext, nexUniqueId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnDeleteCommonDataCompletion, pContext );
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
* @brief スコアの属性値を変更する(非同期関数)
*
* @param[in] categorId    変更するカテゴリID
* @param[in] changeParam  変更詳細情報
* @param[in] nexUniqueId  NEXユニークID（省略可能）
*
* @attention 自分と同じプリンシパルIDのスコアの属性値のみ変更可能。
*
* @note  非同期通信終了時に「NexRankingClientListener::OnNexRankingChageAttributesSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::ChangeAttributes( u32 categorId,
                                         const nn::nex::RankingChangeAttributesParam& changeParam,
                                         nn::nex::UniqueId nexUniqueId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_categoryId = categorId;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_rankingClient->ChangeAttributes(pContext, categorId, changeParam, nexUniqueId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnChangeAttributesCompletion, pContext );
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
* @brief 共通データを取得する(非同期関数)
*
* @param[in] nexUniqueId  NEXユニークID（省略可能）
*
* @attention 自分と同じプリンシパルIDのスコアの属性値のみ変更可能。
*
* @note  正常終了時は「NexRankingClientListener::OnNexRankingGetCommonDataSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::GetCommonData( nn::nex::UniqueId nexUniqueId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_commonData->clear();

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_ratingClient->GetCommonData(pContext, m_commonData, nexUniqueId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnGetCommonDataCompletion, pContext );
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
* @brief ランキングリストを取得する(非同期関数)
*
* @param[in] rankingMode  ランキングモード
* @param[in] categoryId   取得対象カテゴリID
* @param[in] orderParam   順位計算用パラメータ
* @param[in] nexUniqueId  NEXユニークID（省略可能）
* @param[in] principalId  プリンシバルID（省略可能）
*
* @note  正常終了時は「NexRankingClientListener::OnNexRankingGetListDataSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @attention 引数設定が複雑なので、nn::nex::nn::nex::RankingClient::GetRanking()のAPIリファレンスを
* @attention 必ず確認しておくこと！
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::GetRankingList( nn::nex::RankingConstants::RankingMode rankingMode,
                                       u32 categoryId, const nn::nex::RankingOrderParam& orderParam,
                                       nn::nex::UniqueId nexUniqueId,
                                       gflnet2::friends::PrincipalId principalId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  //if( m_ratingClient->GetRanking(pContext, rankingMode, categoryId, orderParam, m_rankResultInf, nexUniqueId, principalId) )
  if( m_ratingClient->GetRanking(pContext, categoryId, m_rankData, nexUniqueId, principalId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnGetRankingListCompletion, pContext );
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
* @brief 指定したスコアを元にした予測順位を取得する(非同期関数)
*
* @param[in] categoryId   取得対象カテゴリID
* @param[in] orderParam   順位計算用パラメータ
* @param[in] score        スコア
* @param[in] nexUniqueId  NEXユニークID（省略可能）
* @param[in] principalId  プリンシバルID（省略可能）
*
* @note  正常終了時は「NexRankingClientListener::OnNexRankingGetApproxOrderSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @attention 引数設定が複雑なので、nn::nex::nn::nex::RankingClient::GetApproxOrder()のAPIリファレンスを
* @attention 必ず確認しておくこと！
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::GetApproxOrder( u32 categoryId, const nn::nex::RankingOrderParam& orderParam, u32 score,
                                       nn::nex::UniqueId nexUniqueId,
                                       gflnet2::friends::PrincipalId principalId )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_rankingClient->GetApproxOrder(pContext, categoryId, orderParam, score, &m_appproxOrder, nexUniqueId, principalId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnGetApproxOrderCompletion, pContext );
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
* @brief スコア統計情報を取得する(非同期関数)
*
* @param[in] categoryId   取得対象カテゴリID
* @param[in] orderParam   順位計算用パラメータ
* @param[in] flags        取得する統計値を指定するフラグ。「nn::nex::RankingConstants::StatsFlag」の論理和
*
* @note  正常終了時は「NexRankingClientListener::OnNexRankingGetStatsSuccess()」がコールされる。
* @note  エラー時は「NexRankingClientListener::OnNexRankingClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexRankingClient::GetStats( u32 categoryId, const nn::nex::RankingOrderParam& orderParam, u32 flags )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_rankingClient->GetStats(pContext, categoryId, orderParam, flags, m_rankStats) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexRankingClient>(this, &NexRankingClient::OnGetStatsCompletion, pContext );
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
 * @param result
 */
//------------------------------------------------------------------
void NexRankingClient::ReportError( const nn::nex::qResult& result )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( result );

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexRankingClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief NEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void NexRankingClient::ReportFunctionError( void )
{
  u32              errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
  nn::nex::qResult result(false);

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexRankingClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief キャンセル報告処理
 */
//------------------------------------------------------------------
void NexRankingClient::ReportCanceled( void )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexRankingCanceled();
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  UploadScore()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnUploadScoreCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingUploadScoreSuccess( m_categoryId );
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
 * @brief  UploadCommonData()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnUploadCommonDataCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingUploadCommonDataSuccess();
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
 * @brief  DeleteScore()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnDeleteScoreCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingDeleteScoreSuccess( m_categoryId );
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
 * @brief  DeleteCommonData()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnDeleteCommonDataCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingDeleteCommonDataSuccess();
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
 * @brief  ChangeAttributes()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnChangeAttributesCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingChageAttributesSuccess( m_categoryId );
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
 * @brief  GetCommonData()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnGetCommonDataCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingGetCommonDataSuccess( *m_commonData );
      }
    }
    m_commonData->clear();
  }
  else
  {
    ReportError( result );
  }
}


//------------------------------------------------------------------
/**
 * @brief  GetRankingList()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnGetRankingListCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingGetListDataSuccess( *m_rankData );
      }
    }
    m_commonData->clear();
  }
  else
  {
    ReportError( result );
  }
}


//------------------------------------------------------------------
/**
 * @brief  GetApproxOrder()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnGetApproxOrderCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingGetApproxOrderSuccess( m_appproxOrder );
      }
    }
    m_commonData->clear();
  }
  else
  {
    ReportError( result );
  }
}


//------------------------------------------------------------------
/**
 * @brief  GetStats()非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexRankingClient::OnGetStatsCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexRankingGetStatsSuccess( *m_rankStats );
      }
    }
  }
  else
  {
    ReportError( result );
  }
}



} // nex
} //gflnet

#endif

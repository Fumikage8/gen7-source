#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexDataStoreClient.cpp
 *
 * @brief  nn::nex::DataStoreClientのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================


#include <gflnet2/include/nex/gflnet2_NexDataStoreClient.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
namespace gflnet2{
namespace nex{



//------------------------------------------------------------------
/**
 * @brief  NexDataStoreClientコンストラクタ
 */
//------------------------------------------------------------------
NexDataStoreClient::NexDataStoreClient( void )
{
  m_isInit          = false;
  m_isAccess        = false;
  m_writeSize       = 0;
  //
  m_dataStoreClient = NULL;
  m_context         = NULL;
  //
  std::memset( m_listener, 0, sizeof(m_listener) );
  //
  m_searchInf       = NULL;
  m_dataId          = 0L;
  m_results         = NULL;
  m_metas           = NULL;
  //
  m_ratingList      = NULL;
  m_ratingInf       = NULL;
  m_rateSlotId      = 0;
  //
  m_persistenceInfoList = NULL;
}


//------------------------------------------------------------------
/**
 * @brief  NexDataStoreClientデストラクタ
 */
//------------------------------------------------------------------
NexDataStoreClient::~NexDataStoreClient()
{
  Finalize();
}


//------------------------------------------------------------------
/**
* @brief  DataStoreClientインスタンスの取得
*/
//------------------------------------------------------------------
nn::nex::DataStoreClient* NexDataStoreClient::GetDataStoreClient( void )
{
  return m_dataStoreClient;
}


//------------------------------------------------------------------
/**
* @brief  DataStoreClientの初期化
*
* @param[in]  credentials     ゲームサーバーログイン情報
*
* @return 初期化に成功したならtrueを返却
*/
//------------------------------------------------------------------
bool NexDataStoreClient::Initialize( nn::nex::Credentials* credentials )
{
  if( !m_isInit )
  {
    m_isInit    = true;
    m_isAccess  = false;
    m_context   = NULL;

    m_dataStoreClient = qNew nn::nex::DataStoreClient();
    //
    m_results    = qNew nn::nex::qVector<nn::nex::qResult>;
    m_metas      = qNew nn::nex::qVector<nn::nex::DataStoreMetaInfo>;
    m_ratingList = qNew nn::nex::qVector< nn::nex::qMap<nn::nex::qInt8,nn::nex::DataStoreRatingInfo> >;
    //
    m_searchInf  = qNew nn::nex::DataStoreSearchResult();
    m_ratingInf  = qNew nn::nex::DataStoreRatingInfo();
    //
    m_persistenceInfoList = qNew nn::nex::qVector<nn::nex::DataStorePersistenceInfo>;

    if( !m_dataStoreClient->Bind(credentials) ){
      return false;
    }
  }
  return true;
}


//------------------------------------------------------------------
/**
* @brief  DataStoreClientの終了
*
* @attention 必ず事前にIsConnecting()がfalseであることを確認してください
*/
//------------------------------------------------------------------
void NexDataStoreClient::Finalize( void )
{
  if( m_isInit )
  {
    m_isInit     = false;
    m_isAccess   = false;

    std::memset( m_listener, 0, sizeof(m_listener) );

    if( m_dataStoreClient != NULL )
    {
      if( m_dataStoreClient->GetCredentials() != NULL ){
        m_dataStoreClient->Unbind();
      }
    }

    NexFreeCollection(&m_persistenceInfoList);
    NexFreeCollection(&m_results);
    NexFreeCollection(&m_metas);
    NexFreeCollection(&m_ratingList);
    NexFreeMemory(&m_ratingInf);
    NexFreeMemory(&m_searchInf);
    NexFreeMemory(&m_dataStoreClient);
  }
}


//------------------------------------------------------------------
/**
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool NexDataStoreClient::IsConnecting( void ) const
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
bool NexDataStoreClient::CancelConnecting( void )
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
* @return すでに通信接続ロック中であればfalseを返却
*/
//------------------------------------------------------------------
bool NexDataStoreClient::LockConnection( void )
{
  if( m_isAccess || !m_isInit ){
    return false;   // 通信中, 未初期化
  }
  m_isAccess = true;

  return true;
}


//------------------------------------------------------------------
/**
 * @brief 通信ロック解除
 */
//------------------------------------------------------------------
void NexDataStoreClient::UnlockConnection( void )
{
  m_isAccess = false;
  m_context  = NULL;
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
bool NexDataStoreClient::RegistDataStoreClientListener( NexDataStoreClientListener* listener )
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
void NexDataStoreClient::UnregistDataStoreClientListener( NexDataStoreClientListener* listener )
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
* @brief データをストレージサーバへアップロードする(非同期通信）
*
* @attention アップロードできるサイズは10MBバイトまで。
* @attention 永続化情報を設定しない場合、データは有効期限経過後にサーバから削除される。
*
* @param[in]  param  アップロード詳細情報
* @param[in]  buffer アップロードするデータ。DataStorePreparePostParam::SetSize()で指定したサイズがアップロードされる。
* @param[in]  useInternalBuffer trueを指定すると、bufferの内容がNEX用メモリにコピーされる。false指定時はbufferポインタを参照し続ける。
*
* @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreUploadSuccess()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::UploadData( const nn::nex::DataStorePostParam& param, const void* buffer, bool useInternalBuffer )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->PostObject(pContext, param, &m_dataId, buffer, useInternalBuffer) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnUploadCompletion, pContext );
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
* @brief 非永続データをストレージサーバからダウンロードする(非同期通信）
*
* @attention ダウンロードできるサイズは10MBバイトまで。
* @attention 参照権限を持っている人のみダウンロードが可能。
*
* @param[in]  dataId     ダウンロードしたいデータID
* @param[in]  buffer     ダウンロードしたデータ格納先バッファへ
* @param[in]  bufferSize ダウンロードしたいデータサイズ。
*
* @note  終了時に「NexDataStoreClientListener::OnNexDataStoreDownloadDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用するため低速。
* @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::DownloadData( const nn::nex::qUnsignedInt64 dataId, void* buffer, nn::nex::qUnsignedInt32 bufferSize )
{
  nn::nex::DataStoreGetParam param;

  param.SetDataId( dataId );

  return GetData( param, buffer, bufferSize );
}


//------------------------------------------------------------------
/**
* @brief 永続データをストレージサーバからダウンロードする(非同期通信）
*
* @attention ダウンロードできるサイズは10MBバイトまで。
* @attention 参照権限を持っている人のみダウンロードが可能。
*
* @param[in]  pid        ダウンロード対象データの所有者のプリンシバルID
* @param[in]  slotId     ダウンロード対象データのスロットID
* @param[in]  bufferSize ダウンロードしたいデータサイズ。
*
* @note  終了時に「NexDataStoreClientListener::OnNexDataStoreDownloadDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用するため低速。
* @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::DownloadPersistenceData( const nn::friends::PrincipalId& pid, const nn::nex::qInt8 slotId,
                                                  void* buffer, nn::nex::qUnsignedInt32 bufferSize )
{
  nn::nex::DataStoreGetParam          param;
  nn::nex::DataStorePersistenceTarget target;

  target.SetOwnerId( pid );
  target.SetPersistenceSlotId( slotId );

  param.SetPersistenceTarget( target );

  return GetData( param, buffer, bufferSize );
}


//------------------------------------------------------------------
/**
* @brief データをストレージサーバからダウンロードする(非同期通信）
*
* @attention ダウンロードできるサイズは10MBバイトまで。
* @attention 参照権限を持っている人のみダウンロードが可能。
*
* @param[in]  param      ダウンロード詳細情報
* @param[in]  buffer     ダウンロードしたデータ格納先バッファへ
* @param[in]  bufferSize ダウンロードしたいデータサイズ。
*
* @note  終了時に「NexDataStoreClientListener::OnNexDataStoreDownloadDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用するため低速。
* @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::GetData( const nn::nex::DataStoreGetParam& param, void* buffer, nn::nex::qUnsignedInt32 bufferSize )
{
  if( !LockConnection() ){
    return false;
  }
  m_context   = NULL;
  m_writeSize = 0;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->GetObject(pContext, param, buffer, bufferSize, &m_writeSize ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnDownloadDataCompletion, pContext );
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
* @brief ストレージサーバのデータを更新する(非同期通信）
*
* @attention 更新できるサイズは10MBバイトまで。
*
* @param[in]  param  更新詳細情報
* @param[in]  buffer 更新に使用するデータ。DataStoreUpdateParam::SetSize()で指定したサイズがアップロードされる。
* @param[in]  useInternalBuffer trueを指定すると、bufferの内容がNEX用メモリにコピーされる。false指定時はbufferポインタを参照し続ける。
*
* @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreUpdateSuccess()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用するため低速。
* @note  NEXのスレッドモードにかかわらず通信用スレッドが一つ作成される。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::UpdateData( const nn::nex::DataStoreUpdateParam& param, const void* buffer, bool useInternalBuffer )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_dataId = param.GetDataId();

  if( m_dataStoreClient->UpdateObject(pContext, param, buffer, useInternalBuffer) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnUpdateCompletion, pContext );
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
* @brief データをストレージサーバから削除する(非同期通信）
*
* @attention １度に削除できるデータ数は100件まで。
*
* @param[in]  dataIds  削除対象データIDリスト
*
* @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreDeleteDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::DeleteData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_results->clear();

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->DeleteObject(pContext, dataIds, false, m_results) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnDeleteCompletion, pContext );
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
* @brief ストレージサーバからデータを検索する(非同期通信）
*
* @attention 参照権限を持っているデータのみを検索する。
*
* @param[in]  param  検索詳細情報
*
* @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreSearchSuccess()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用するため低速。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::SearchData( const nn::nex::DataStoreSearchParam& param )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->SearchObject(pContext, param, m_searchInf) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnSearchCompletion, pContext );
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
* @brief ストレージサーバのデータ参照カウンタ値を更新する(非同期通信）
*
* @attention 参照権限を持っているデータのみ更新可能。
*
* @param[in]  param  更新詳細情報
*
* @note  終了時は「NexDataStoreClientListener::OnNexDataStoreTouchFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用するため低速。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::TouchData( const nn::nex::DataStoreTouchObjectParam& param )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->TouchObject(pContext, param) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnTouchCompletion, pContext );
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
* @brief メタデータをアップロードする(非同期通信）
*
* @attention アップロードできるサイズは1024バイトまで。
* @attention 永続化情報を設定しない場合、データは有効期限経過後にサーバから削除される。
*
* @param[in]  param  アップロード詳細情報
*
* @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreUploadSuccess()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用しないため高速動作が可能。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::UploadMetaData( const nn::nex::DataStorePostParam& param )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->PostObject(pContext, param, &m_dataId) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnUploadCompletion, pContext );
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
* @brief メタデータを一括ダウンロードする(非同期通信）
*
* @attention １度にダウンロードできるメタデータ数は100件まで。
*
* @param[in]  dataIds  ダウンロード対象データIDリスト
* @param[in]  param    ダウンロード詳細情報
*
* @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreDownloadMetaDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用しないため高速動作が可能。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::GetMetaData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds,
                                      const nn::nex::DataStoreGetMetaParam& param )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_results->clear();

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->GetMeta(pContext, dataIds, param, m_metas, m_results) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnDownloadMetaCompletion, pContext );
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
* @brief 非永続メタデータを一括ダウンロードする(非同期通信）
*
* @attention １度にダウンロードできるメタデータ数は100件まで。
*
* @param[in]  dataIds   ダウンロード対象データIDリスト
* @paran[in]  targetBit 拡張情報の取得指定用フラグ。「DOWNLOAD_TARGET」の論理和で指定する。
*
* @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreDownloadMetaDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用しないため高速動作が可能。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::DownloadMetaData( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds,
                                           const nn::nex::qUnsignedInt32 targetBit )
{
  nn::nex::DataStoreGetMetaParam param;
  param.SetResultOption( targetBit );

  return GetMetaData( dataIds, param );
}


//------------------------------------------------------------------
/**
* @brief 永続メタデータをダウンロードする(非同期通信）
*
* @param[in]  pid       ダウンロード対象データの所有者のプリンシバルID
* @param[in]  slotId    ダウンロード対象データのスロットID
* @paran[in]  targetBit 拡張情報の指定用フラグ。「DOWNLOAD_TARGET」の論理和で指定する。
*
* @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreDownloadMetaDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用しないため高速動作が可能。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::DownloadPersistenceMetaData( const nn::friends::PrincipalId& pid,
                                                      const nn::nex::qInt8 slotId,
                                                      const nn::nex::qUnsignedInt32 targetBit )
{
  nn::nex::DataStoreGetMetaParam            param;
  nn::nex::DataStorePersistenceTarget       target;
  nn::nex::qVector<nn::nex::qUnsignedInt64> dataIds;

  target.SetOwnerId( pid );
  target.SetPersistenceSlotId( slotId );

  param.SetPersistenceTarget( target );
  param.SetResultOption( targetBit );

  // 取得数を１件として登録。指定値に意味なし。
  dataIds.push_back( 0 );

  return GetMetaData( dataIds, param );
}


//------------------------------------------------------------------
/**
* @brief メタデータを一括更新する(非同期通信）
*
* @attention １度に更新できるメタデータ数は100件まで。
*
* @param[in]  param    更新詳細情報リスト
*
* @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreChangeMetaDataFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
* @note  ストレージサーバを使用しないため高速動作が可能。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::CheageMetaData( const nn::nex::qVector<nn::nex::DataStoreChangeMetaParam> & params )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_results->clear();

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->ChangeMeta(pContext, params, false, m_results) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnChangeMetaCompletion, pContext );
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
* @brief データへの評価をリセットする(非同期通信）
*
* @attention １度にリセットできるデータ数は100件まで。
*
* @param[in]  dataIds  評価リセット対象データIDリスト
*
* @note  通信終了時は「NexDataStoreClientListener::OnNexDataStoreResetRatingFinish()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::ResetRating( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_results->clear();

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->ResetRating(pContext, dataIds, false, m_results) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnResetRatingCompletion, pContext );
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
* @brief データへ評価をセットする(非同期通信）
*
* @attention 評価を行うには、予め該当のデータスロットが初期化されている必要がある。
* @attention 初期化は DataStoreClient::UploadData()する際に、DataStorePreparePostParam::SetRatingSetting()で行う。
*
* @param[in]  target  評価設定対象データ
* @param[in]  param   評価設定パラメータ
*
* @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreSetRatingSuccess()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::SetRating( const nn::nex::DataStoreRatingTarget& target, const nn::nex::DataStoreRateObjectParam& param )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  m_rateSlotId = target.GetSlot();

  if( m_dataStoreClient->RateObject(pContext, target, param, m_ratingInf) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnSetRatingCompletion, pContext );
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
* @brief データへの評価を取得する(非同期通信）
*
* @attention １度に取得できるデータ数は100件まで。
*
* @param[in]  dataIds  評価取得対象データIDリスト
*
* @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreGetRatingSuccess()」がコールされる。
* @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
*
* @return 実行結果。成功時はtrueが返却される。
*/
//------------------------------------------------------------------
bool NexDataStoreClient::GetRating( const nn::nex::qVector<nn::nex::qUnsignedInt64>& dataIds )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_ratingList->clear();

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->GetRating(pContext, dataIds, m_ratingList) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnGetRatingCompletion, pContext );
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
 * @brief 永続化情報を一括取得する（非同期通信）
 * @param pid   永続化情報を取得したい人のプリンシパルID
 * @param slotIds 永続化スロットIDのリスト
 *
 * @note  正常終了時は「NexDataStoreClientListener::OnNexDataStoreGetPersistenceInfo()」がコールされる。
 * @note  エラー時は「NexDataStoreClientListener::OnNexDataStoreClientError()」がコールされる。
 * @return 実行結果。成功時はtrueが返却される。
 */
//------------------------------------------------------------------
bool NexDataStoreClient::GetPersistenceInfo( const nn::friends::PrincipalId& pid, const nn::nex::qVector<nn::nex::qUnsignedInt16>& slotIds )
{
  if( !LockConnection() ){
    return false;
  }
  m_context = NULL;
  m_persistenceInfoList->clear();
  m_results->clear();

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->GetPersistenceInfo(pContext, pid, slotIds, m_persistenceInfoList, m_results) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexDataStoreClient>(this, &NexDataStoreClient::OnGetPersistenceInfo, pContext );
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
 * @param result 実行結果
 */
//------------------------------------------------------------------
void NexDataStoreClient::ReportError( const nn::nex::qResult& result )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( result );

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDataStoreClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief キャンセル報告処理
 */
//------------------------------------------------------------------
void NexDataStoreClient::ReportCanceled( void )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDataStoreClientCanceled();
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief NEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void NexDataStoreClient::ReportFunctionError( void )
{
  u32              errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
  nn::nex::qResult result(false);

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDataStoreClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  アップロード非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnUploadCompletion(nn::nex::CallContext *pContext)
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
        m_listener[i]->OnNexDataStoreUploadSuccess( m_dataId );
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
 * @brief  更新非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnUpdateCompletion(nn::nex::CallContext *pContext)
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
        m_listener[i]->OnNexDataStoreUpdateSuccess( m_dataId );
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
 * @brief  データ検索非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnSearchCompletion(nn::nex::CallContext *pContext)
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
        m_listener[i]->OnNexDataStoreSearchSuccess( *m_searchInf );
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
 * @brief  データ参照カウンタ値更新非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnTouchCompletion(nn::nex::CallContext *pContext)
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
        m_listener[i]->OnNexDataStoreTouchSuccess();
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
 * @brief  データ削除非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnDeleteCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  // 通信エラー時
  else if( pContext->GetState() == nn::nex::CallContext::CallFailure )
  {
    if( m_results->size() == 0 ){
      m_results->push_back( pContext->GetOutcome() );
    }
  }

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDataStoreDeleteDataFinish( *m_results );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  ストレージサーバからのデータダウンロード非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnDownloadDataCompletion(nn::nex::CallContext *pContext)
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
        m_listener[i]->OnNexDataStoreDownloadDataSuccess( m_writeSize );
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
 * @brief  メタデータダウンロード非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnDownloadMetaCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  // 通信エラー時
  else if( pContext->GetState() == nn::nex::CallContext::CallFailure )
  {
    if( m_results->size() == 0 ){
      m_results->push_back( pContext->GetOutcome() );
    }
  }

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDataStoreDownloadMetaDataFinish( *m_results, *m_metas );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  メタデータ更新非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnChangeMetaCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  // 通信エラー時
  else if( pContext->GetState() == nn::nex::CallContext::CallFailure )
  {
    if( m_results->size() == 0 ){
      m_results->push_back( pContext->GetOutcome() );
    }
  }

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDataStoreChangeMetaDataFinish( *m_results );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  データ評価リセット更新非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnResetRatingCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  // 通信エラー時
  else if( pContext->GetState() == nn::nex::CallContext::CallFailure )
  {
    if( m_results->size() == 0 ){
      m_results->push_back( pContext->GetOutcome() );
    }
  }

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnNexDataStoreResetRatingFinish( *m_results );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  データ評価設定非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnSetRatingCompletion(nn::nex::CallContext *pContext)
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
        m_listener[i]->OnNexDataStoreSetRatingSuccess( m_rateSlotId, *m_ratingInf );
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
 * @brief  データ評価取得非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnGetRatingCompletion(nn::nex::CallContext *pContext)
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
        m_listener[i]->OnNexDataStoreGetRatingSuccess( *m_ratingList );
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
 * @brief  永続化情報取得非同期処理の完了通知検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexDataStoreClient::OnGetPersistenceInfo(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceled();
    return;
  }
  // 通信エラー時
  else if( pContext->GetState() == nn::nex::CallContext::CallFailure )
  {
    if( m_results->size() == 0 ){
      m_results->push_back( pContext->GetOutcome() );
    }
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (nn::nex::qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnNexDataStoreGetPersistenceInfo( *m_persistenceInfoList, *m_results );
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
 * @brief  メタデータのログ出力（デバッグ用）
 *
 * @param[in]  metaInfo ログ出力対象となるメタデータ
 */
//------------------------------------------------------------------
void NexDataStoreClient::PrintGetMetaResult( const nn::nex::DataStoreMetaInfo& metaInfo )
{
   GFL_PRINT("+---------+----------+--------------+----------+-----------+\n");
   GFL_PRINT("| data_id | owner_id |     name     |   size   | data_type |\n");
   GFL_PRINT("+---------+----------+--------------+----------+-----------+\n");
   GFL_PRINT("|%9llu|%10u|%ls|%10u|%11u|\n",
       metaInfo.GetDataId(),
       metaInfo.GetOwnerId(),
       metaInfo.GetName().GetBufferPtr(),
       metaInfo.GetSize(),
       metaInfo.GetDataType()
   );
   GFL_PRINT("+---------+----------+--------------+----------+-----------+\n");
   // Releaseビルドでのwarning回避
   (void)metaInfo;
}



} // nex
} // gflnet
#endif

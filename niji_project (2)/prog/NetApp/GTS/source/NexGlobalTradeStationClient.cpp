//=============================================================================
/**
 * @file   NexGlobalTradeStationClient.cpp
 *
 * @brief  DataStoreClinentのGlobalTradeStation機能用ラッパークラス
 *
 * @author h.suzuki
 */
//=============================================================================
#ifdef GF_PLATFORM_CTR

#include <nex.h>
#include "NetApp/GTS/source/NexGlobalTradeStationClient.h"
//#include "debug/DebugLogWindow.h"
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "util/include/gfl2_std_string.h"
#include  <debug/include/gfl2_DebugLogWindow.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ
*/
//------------------------------------------------------------------
NexGlobalTradeStationClient::NexGlobalTradeStationClient( void )
{
  m_isInit          = false;
  m_isAccess        = false;
  m_context         = NULL;
  m_dataStoreClient = NULL;
  m_myDownloadInf   = NULL;
  m_preTradeInf     = NULL;
  m_tradeInf        = NULL;
  m_searchInf       = NULL;
  m_recordKey       = NULL;
}


//------------------------------------------------------------------
/**
* @brief  デストラクタ
*/
//------------------------------------------------------------------
NexGlobalTradeStationClient::~NexGlobalTradeStationClient()
{
  Finalize();
}


//------------------------------------------------------------------
/**
* @brief  初期化処理
*
* @param[in]  client DataStoreClientのインスタンス
*
* @return 初期化に成功したならtrueを返却
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::Initialize( nn::nex::DataStoreClient* client )
{
  if( m_isInit ){
    return false;
  }
  m_isInit          = true;
  m_dataStoreClient = client;
  m_context         = NULL;

  std::memset(m_listener, 0, sizeof(m_listener));

  m_myDownloadInf = qNew nn::nex::GlobalTradeStationDownloadMyPokemonResult();
  m_preTradeInf   = qNew nn::nex::GlobalTradeStationPrepareTradePokemonResult();
  m_tradeInf      = qNew nn::nex::GlobalTradeStationTradePokemonResult();
  m_searchInf     = qNew nn::nex::GlobalTradeStationSearchPokemonResult();
  m_recordKey     = qNew nn::nex::GlobalTradeStationRecordKey();

  return true;
}

//------------------------------------------------------------------
/**
* @brief  終了処理
*
* @caution 必ず事前にIsConnecting()がfalseであることを確認してください
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::Finalize( void )
{
  if( m_isInit )
  {
    std::memset(m_listener, 0, sizeof(m_listener));

    gflnet2::nex::NexFreeMemory( &m_myDownloadInf );
    gflnet2::nex::NexFreeMemory( &m_preTradeInf );
    gflnet2::nex::NexFreeMemory( &m_tradeInf );
    gflnet2::nex::NexFreeMemory( &m_searchInf );
    gflnet2::nex::NexFreeMemory( &m_recordKey );

    m_isInit = false;
  }
}


//------------------------------------------------------------------
/**
 * @brief 通信キャンセルリクエストを発行する。
 * @brief キャンセルが実施された場合、完了時にOnGlobalTradeStationClientCanceled()がコールされる。
 *
 * @note  通信状態にかかわらず、コールしても良い。
 *
 * @return キャンセル処理が実施されたならtrueを返却する
 */
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::CancelConnecting( void )
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
* @brief  通信中か判定
*
* @return 通信中ならtrueを返却
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::IsConnecting( void ) const
{
  return m_isAccess;
}


//------------------------------------------------------------------
/**
* @brief リスナの登録
*
* @param[in] listener 登録対象のNexGlobalTradeStationClient用リスナ。
*
* @return 登録に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::RegistGlobalTradeStationClientListener( NexGlobalTradeStationClientListener* listener )
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
* @brief リスナの解除
*
* @param[in] listener 削除対象のNexGlobalTradeStationClient用リスナ。
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::UnregistGlobalTradeStationClientListener( NexGlobalTradeStationClientListener* listener )
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
 * @brief 通信ロック設定
 *
 * @return すでに通信ロックされているならfalseを返却
 */
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::SetAccessLock( void )
{
  if( m_isAccess || !m_isInit ){
    return false;   // 通信中, 未初期化
  }

  // ロックする
  m_isAccess = true;

  return true;
}


//------------------------------------------------------------------
/**
* @brief  自分のポケモンを預ける準備をする（非同期通信）
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnPrepareUploadPokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::PrepareUploadPokemon( void )
{
  if( !SetAccessLock() ){
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::PrepareUploadPokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext *pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  if( m_dataStoreClient->PrepareUploadPokemon(pContext, m_recordKey) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnPrepareUploadCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief  自分のポケモンを預ける（非同期通信）
*
* @caution 事前にPrepareUploadPokemon()をコールすること。
*
* @param  prepareUploadKey 自分が預けたポケモンを特定するキー
* @param  indexData        インデックスデータ
* @param  pokemonData      ポケモンデータ
* @param  signature        署名データ
* @param  signatureSize    署名データサイズ
* @param  period           預けるポケモンを参照できる期限(日数)
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnUploadPokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::UploadPokemon( const GtsSerializeRecordKey& prepareUploadKey,
                                                 const NetApp::GTS::GtsIndexData* indexData,
                                                 const NetApp::GTS::GtsSerializePokemonParam* pokemonData,
                                                 const void* signature,
                                                 u32 signatureSize,
                                                 u16 period )
{
  if( !SetAccessLock() ){
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::UploadPokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext*                 pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  nn::nex::GlobalTradeStationUploadPokemonParam param;
  //
  nn::nex::GlobalTradeStationRecordKey nexPrepareUploadKey;

  nexPrepareUploadKey.Deserialize( prepareUploadKey.data, prepareUploadKey.size );

  param.SetPeriod( period );
  param.SetIndexData( indexData, sizeof(NetApp::GTS::GtsIndexData) );
  param.SetPokemonData( pokemonData, sizeof(NetApp::GTS::GtsSerializePokemonParam) );
  param.SetPrepareUploadKey( nexPrepareUploadKey );
  param.SetSignature( signature, signatureSize );

  if( m_dataStoreClient->UploadPokemon(pContext, param) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnUploadPokemonCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief  交換可能なポケモンを検索する（非同期通信）
*
* @param  searchParam 検索設定情報。
* @param              設定内容については、NexSDKのAPIリファレンス[nn::nex::DataStoreClient::SearchPokemon]を参照。
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnSearchPokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::SearchPokemon( const nn::nex::GlobalTradeStationSearchPokemonParam& searchParam )
{
  if( !SetAccessLock() ){
    GFL_PRINT("GTS CLIENT AccessLock\n");
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::SearchPokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext*  pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  // momiji変更：互換対応としてmomijiから検索のパラメータを指定する。
  if( m_dataStoreClient->SearchPokemon(pContext, searchParam, m_searchInf, nn::nex::GlobalTradeStationConstants::POKEMON_DATA_VERSION_A2A_DEFAULT) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnSearchPokemonCompletion, pContext );
    GFL_PRINT("SearchPokemon呼び出し成功\n");
    return true;
  }
  else
  {
    GFL_PRINT("GTS CLIENT Error\n");
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief  ポケモン交換の準備をする（非同期通信）
*
* @param  prepareUploadKey 自分が預けたポケモンを特定するキー。
* @param  tradeKey         交換相手のポケモンを特定するキー。
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnPrepareTradePokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::PrepareTradePokemon( const GtsSerializeRecordKey& prepareUploadKey,
                                                       const GtsSerializeTradeKey&  tradeKey )
{
  if( !SetAccessLock() ){
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::PrepareTradePokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  //
  nn::nex::GlobalTradeStationRecordKey nexPrepareUploadKey;
  nexPrepareUploadKey.Deserialize( prepareUploadKey.data, prepareUploadKey.size );
  //
  nn::nex::GlobalTradeStationTradeKey nexTradeKey;
  nexTradeKey.Deserialize( tradeKey.data, tradeKey.size );
  //
  nn::nex::GlobalTradeStationPrepareTradePokemonParam param;
  param.SetPrepareUploadKey(nexPrepareUploadKey);
  param.SetTradeKey(nexTradeKey);

  if( m_dataStoreClient->PrepareTradePokemon(pContext, param, m_preTradeInf) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnPrepareTradePokemonCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief  自分のポケモンを交換する（非同期通信）
*
* @caution 事前にPrepareTradePokemon()をコールすること。
*
* @param  tradeKey         交換相手のポケモンを特定するキー(SearchPokemon()で取得した値)
* @param  prepareTradeKey  自分が交換に出すポケモンを特定するキー(PrepareTradePokemon()で取得した値)
* @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
* @param  indexData        インデックスデータ
* @param  pokemonData      ポケモンデータ
* @param  signature        署名データ
* @param  signatureSize    署名データサイズ
* @param  period           預けるポケモンを参照できる期限(日数)
* @param  isNeedData       インデクスデータ、ポケモンデータを取得するならtrueを指定
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnTradePokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::TradePokemon( const GtsSerializeTradeKey& tradeKey,
                                                const GtsSerializeRecordKey& prepareTradeKey,
                                                const GtsSerializeRecordKey& prepareUploadKey,
                                                const NetApp::GTS::GtsIndexData* indexData,
                                                const NetApp::GTS::GtsSerializePokemonParam*  pokemonData,
                                                const void* signature,
                                                u32 signatureSize,
                                                u16 period,
                                                bool isNeedData )
{
  if( !SetAccessLock() ){
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::TradePokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  nn::nex::GlobalTradeStationTradePokemonParam param;
  //
  if( prepareUploadKey.size > 0 )
  {
    nn::nex::GlobalTradeStationRecordKey nexPrepareUploadKey;
    nexPrepareUploadKey.Deserialize( prepareUploadKey.data, prepareUploadKey.size );
    param.SetPrepareUploadKey(nexPrepareUploadKey);
    GFL_PRINT( "nexPrepareUploadKey[%llu][%llu]\n", nexPrepareUploadKey.GetDataId(), nexPrepareUploadKey.GetPassword() );
  }
  //
  if( prepareTradeKey.size > 0 )
  {
    nn::nex::GlobalTradeStationRecordKey nexPrepareTradeKey;
    nexPrepareTradeKey.Deserialize( prepareTradeKey.data, prepareTradeKey.size );
    param.SetPrepareTradeKey(nexPrepareTradeKey);
    GFL_PRINT( "nexPrepareTradeKey[%llu][%llu]\n", nexPrepareTradeKey.GetDataId(), nexPrepareTradeKey.GetPassword() );
  }
  //
  if( tradeKey.size > 0 )
  {
    nn::nex::GlobalTradeStationTradeKey nexTradeKey;
    nexTradeKey.Deserialize( tradeKey.data, tradeKey.size );
    param.SetTradeKey(nexTradeKey);
    GFL_PRINT( "nexTradeKey[%llu][%u]\n", nexTradeKey.GetDataId(), nexTradeKey.GetVersion() );
  }
  //
  param.SetPeriod( period );
  param.SetIndexData( indexData, sizeof(NetApp::GTS::GtsIndexData) );
  param.SetPokemonData( pokemonData, sizeof(NetApp::GTS::GtsSerializePokemonParam) );
  param.SetSignature( signature, signatureSize );
  param.SetNeedData( isNeedData );

  if( m_dataStoreClient->TradePokemon(pContext, param, m_tradeInf) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnTradePokemonCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }

}


//------------------------------------------------------------------
/**
* @brief  自分のポケモンを削除する（非同期通信）
*
* @param  prepareUploadKey 自分が預けたポケモンを特定するキー
* @param  deleteFlag       削除挙動設定フラグ
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnDeletePokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::DeletePokemon( const GtsSerializeRecordKey& prepareUploadKey,
                                                 const nn::nex::GlobalTradeStationConstants::DeleteFlag deleteFlag )
{
  if( !SetAccessLock() ){
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::DeletePokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext*  pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  //
  nn::nex::GlobalTradeStationDeletePokemonParam param;
  //
  nn::nex::GlobalTradeStationRecordKey nexPrepareUploadKey;
  nexPrepareUploadKey.Deserialize( prepareUploadKey.data, prepareUploadKey.size );
  //
  param.SetPrepareUploadKey(nexPrepareUploadKey);
  param.SetDeleteFlag(deleteFlag);

  if( m_dataStoreClient->DeletePokemon(pContext, param) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnDeletePokemonCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief  自分のポケモンを取得する（非同期通信）
*
* @param  prepareUploadKey 自分のポケモンを取得するためのパラメータ
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnDownloadMyPokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::DownloadMyPokemon( const GtsSerializeRecordKey& prepareUploadKey )
{
  if( !SetAccessLock() ){
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::DownloadMyPokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext*  pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  //
  nn::nex::GlobalTradeStationRecordKey nexPrepareUploadKey;
  nexPrepareUploadKey.Deserialize( prepareUploadKey.data, prepareUploadKey.size );
  //
  nn::nex::GlobalTradeStationDownloadMyPokemonParam param;
  param.SetPrepareUploadKey(nexPrepareUploadKey);

  if( m_dataStoreClient->DownloadMyPokemon(pContext, param, m_myDownloadInf) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnDownloadMyPokemonCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
* @brief  交換相手のポケモンを取得する（非同期通信）
*
  * @param  prepareUploadKey 自分が預けたポケモンを特定するキー(PrepareUploadPokemon()で取得した値)
*
* @note   通信終了時は「NexGlobalTradeStationClientListener::OnDownloadOtherPokemonSuccess()」がコールされる。
* @note   エラー時は「NexGlobalTradeStationClientListener::OnGlobalTradeStationClientError()」がコールされる。
*
* @return 実行に成功したならtrueを返却する
*/
//------------------------------------------------------------------
bool NexGlobalTradeStationClient::DownloadOtherPokemon( const GtsSerializeRecordKey& prepareUploadKey )
{
  if( !SetAccessLock() ){
    return false;
  }
  m_context = NULL;

//  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"DataStoreClient::DownloadOtherPokemon() \n" );
//  #endif

  nn::nex::ProtocolCallContext*  pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  //
  nn::nex::GlobalTradeStationRecordKey nexPrepareUploadKey;
  nexPrepareUploadKey.Deserialize( prepareUploadKey.data, prepareUploadKey.size );
  //
  nn::nex::GlobalTradeStationDownloadOtherPokemonParam param;
  param.SetPrepareUploadKey(nexPrepareUploadKey);

  if( m_dataStoreClient->DownloadOtherPokemon(pContext, param, m_tradeInf) )
  {
    m_context = NULL;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<NexGlobalTradeStationClient>(this, &NexGlobalTradeStationClient::OnDownloadOtherPokemonCompletion, pContext );
    return true;
  }
  else
  {
    ReportFunctionErrorCodeToListener();
    qDelete pContext;
    m_isAccess = false;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief エラー通知処理
 *
 * @param result 実行結果
 */
//------------------------------------------------------------------
void NexGlobalTradeStationClient::ReportErrorCodeToListener( const nn::nex::qResult& result )
{
  GFL_PRINT( "ReportErrorCodeToListener[%u]\n", gflnet2::nex::NexError::GetNetworkErrorCode(result) );

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnGlobalTradeStationClientError( result, gflnet2::nex::NexError::GetNetworkErrorCode(result) );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief キャンセル通知処理
 */
//------------------------------------------------------------------
void NexGlobalTradeStationClient::ReportCanceledListener( void )
{
  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnGlobalTradeStationClientCanceled();
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
void NexGlobalTradeStationClient::ReportFunctionErrorCodeToListener( void )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkFunctionErrorCode();
  nn::nex::qResult result(false);

  GFL_PRINT( "ReportFunctionErrorCodeToListener[%u]\n", errorCode );

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_listener[i] != NULL ){
      m_listener[i]->OnGlobalTradeStationClientError( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
* @brief  PrepareUploadPokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnPrepareUploadCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    GFL_PRINT( "OnPrepareUploadCompletion[%llu][%llu]\n", m_recordKey->GetDataId(), m_recordKey->GetPassword() );

    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL )
      {
        GtsSerializeRecordKey serializeRecordKey;
        ConvertNexRecordKeyToSerializeRecordKey( *m_recordKey, serializeRecordKey );
        m_listener[i]->OnPrepareUploadPokemonSuccess( serializeRecordKey );
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
* @brief  UploadPokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnUploadPokemonCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnUploadPokemonSuccess();
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
* @brief  SearchPokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnSearchPokemonCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnSearchPokemonSuccess( *m_searchInf );
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
* @brief  PrepareTradePokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnPrepareTradePokemonCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    GFL_PRINT( "OnPrepareTradePokemonCompletion[%llu][%llu]\n", m_preTradeInf->GetPrepareTradeKey().GetDataId(), m_preTradeInf->GetPrepareTradeKey().GetPassword() );

    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL )
      {
        GtsSerializeRecordKey tradeRecord;
        ConvertNexRecordKeyToSerializeRecordKey( m_preTradeInf->GetPrepareTradeKey(), tradeRecord );

        m_listener[i]->OnPrepareTradePokemonSuccess( m_preTradeInf->GetResult(), tradeRecord );
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
* @brief  TradePokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnTradePokemonCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    GFL_PRINT( "OnTradePokemonCompletion[%llu][%llu]\n", m_tradeInf->GetMyDataId(), m_tradeInf->GetResult().GetDataId() );

    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnTradePokemonSuccess( *m_tradeInf );
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
* @brief  DeletePokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnDeletePokemonCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnDeletePokemonSuccess();
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
* @brief  DownloadMyPokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnDownloadMyPokemonCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnDownloadMyPokemonSuccess( *m_myDownloadInf );
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
* @brief  DownloadMyPokemon完了通知検知用関数
*
* @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
*
* @param[in,out]  pContext 完了通知対象となるコンテキスト
*/
//------------------------------------------------------------------
void NexGlobalTradeStationClient::OnDownloadOtherPokemonCompletion(nn::nex::CallContext *pContext)
{
  m_context  = NULL;
  m_isAccess = false;

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    ReportCanceledListener();
    return;
  }
  nn::nex::qResult result = pContext->GetOutcome();

  if( (qBool)result )
  {
    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_listener[i] != NULL ){
        m_listener[i]->OnDownloadOtherPokemonSuccess( *m_tradeInf );
      }
    }
  }
  else
  {
    ReportErrorCodeToListener( result );
  }
}


//------------------------------------------------------------------
/**
  * @brief NexのキーをシリアライズしてserializeRecodeKeyに出力します
  *
  * @param nexKey 入力パラメータのNexのキー
  * @param serializeRecordKey 出力パラメータのserializeRecodeKey
  */
//------------------------------------------------------------------
void NexGlobalTradeStationClient::ConvertNexRecordKeyToSerializeRecordKey( const nn::nex::GlobalTradeStationRecordKey& nexKey, GtsSerializeRecordKey& serializeRecordKey )
{
  nn::nex::qVector<nn::nex::qByte> serializeKey;
  nexKey.Serialize( serializeKey );
  gfl2::std::MemClear( serializeRecordKey.data, sizeof( serializeRecordKey.data ) );
  GFL_ASSERT_MSG( ( serializeKey.size() <= sizeof( serializeRecordKey.data ) ), "Error : NexGlobalTradeStationClient::ConvertNexRecordKeyToSerializeRecordKey() memory of the serialize recode key buffer is not enough.\n" );
  gfl2::std::MemCopy( reinterpret_cast<const u8*>(&serializeKey[0]), serializeRecordKey.data, serializeKey.size() );
  serializeRecordKey.size = serializeKey.size();
}


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GF_PLATFORM_CTR

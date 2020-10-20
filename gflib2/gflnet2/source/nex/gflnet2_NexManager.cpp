#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexManager.cpp
 *
 * @brief  nn::nexのラッパークラス。NEXライブラリを管理する
 *         なお、事前にfs, ac, socketライブラリを初期化しておく必要がある。
 *
 * @author h.suzuki
 */
//=============================================================================


#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClient.h>
#include <gflnet2/source/nex/gflnet2_NexDispatchThread.h>
#include <RendezVous/Services/JugemPackages.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>

#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gflnet2::nex::NexManager)
template class gfl2::base::SingletonAccessor<gflnet2::nex::NexManager>;
#endif

// SetDOClassInitializationCallback() のための宣言
namespace nn { namespace nex {
  extern void InitDOClasses();
}}

namespace gflnet2{
namespace nex{


//------------------------------------------------------------------
/**
 * @brief  nn::nex::MemoryManager::SetBasicMemoryFunctions()用メモリ確保コールバック関数
 */
//------------------------------------------------------------------
static void* _NexMallocFunction( size_t stSize )
{
  gflnet2::nex::NexManager* nexSys = GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager );
  return nexSys->AllocFunction( stSize );
}


//------------------------------------------------------------------
/**
 * @brief  nn::nex::MemoryManager::SetBasicMemoryFunctions()用メモリ解放コールバック関数
 */
//------------------------------------------------------------------
static void _NexFreeFunction( void *pMemory )
{
  gflnet2::nex::NexManager* nexSys = GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager );
  nexSys->FreeFunction( pMemory );
}



//------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 */
//------------------------------------------------------------------
NexManager::NexManager( void ) : m_nexHeap()
{
  m_isInit             = false;
  m_gameServerLogin    = false;
  m_isConnecting       = false;
  m_isDisconnectHandle = false;
  //
  m_mainBuffer         = NULL;
  m_reservedBuffer     = NULL;
  m_mainBufferSize     = 0;
  m_reserveBufferSize  = 0;
  m_currentUsage       = 0;
  m_maxUsage           = 0;
  m_uniqueId           = 0;
  //
  ::std::memset( m_authToken, 0, sizeof(m_authToken) );
  //
  m_currentContext     = NULL;
  m_natClient          = NULL;
  m_netZ               = NULL;
  m_utilityClient      = NULL;
  m_ngsFacade          = NULL;
  //
  m_dataStoreClient    = NULL;
  m_dispatchThread     = NULL;
  //
  m_gameServerListener = NULL;
  m_errorListener      = NULL;
  m_utilityListener    = NULL;
  //
  m_threadMode         = ThreadModeUndefined;
  m_fillType           = FILL_TYPE_NONE;
  m_isLockDispatch     = false;
  //
  ::std::memset( m_dispatchListener, 0, sizeof(m_dispatchListener) );
  //
  m_criticalSection.Initialize();
  //
  m_isKickMe = false;  // sessionをキックしたフラグ
}


//------------------------------------------------------------------
/**
 * @brief  デストラクタ
 */
//------------------------------------------------------------------
NexManager::~NexManager()
{
  Finalize();
  m_criticalSection.Finalize();
}


//------------------------------------------------------------------
/**
 * @brief  NEXライブラリの初期化
 *         事前にfs, ac, socketを初期化する必要があります！
 *         スレッドモードは「nn::nex::Core::ThreadModeUnsafeTransportBuffer」に設定されます
 *
 * @param  heap              内部用バッファ割り当てに使用されるヒープメモリ
 * @param  mainBufferSize    NEX用に確保されるメインメモリサイズ
 * @param  reserveBufferSize メインメモリが不足した際に割り当てられる緊急時メモリサイズ
 * @param  threadMode        動作スレッドモード
 * @param  isUsingAutoStack  AutoStackManagerを使用するならtrueを指定
 *
 * @result 初期化結果。
 *         成功時には「nn::ResultSuccess()」、
 *         メモリ確保失敗時には「nn::nex::ResultOutOfMemory()」が返却される
 */
//------------------------------------------------------------------
nn::Result NexManager::Initialize( gfl2::heap::HeapBase* heap, u32 mainBufferSize, u32 reserveBufferSize, ThreadMode threadMode, bool isUsingAutoStack ,FREE_MEMORY_FILL_TYPE fillType )
{
  if( !m_isInit )
  {
    u32 totalMemorySize = mainBufferSize+reserveBufferSize;

    m_mainBufferSize    = mainBufferSize;
    m_reserveBufferSize = reserveBufferSize;
    m_threadMode        = threadMode;

    // ワークメモリを確保する
    m_mainBuffer = GflHeapAllocMemoryLowAlign(heap,totalMemorySize,4);
    if( m_mainBuffer == NULL ){
      return nn::nex::ResultOutOfMemory();
    }

    // ヒープを初期化する
    uptr heapBase = NN_ANY_TO_UPTR(m_mainBuffer);
    m_nexHeap.Initialize( heapBase, totalMemorySize);
    GFL_PRINT("[NexManager Memory %d]\n",totalMemorySize);
    m_nexHeap.Dump();       // リークしているメモリを表示する

    // メモリ関数を登録する
    nn::nex::MemoryManager::SetBasicMemoryFunctions( _NexMallocFunction, _NexFreeFunction );

    // 緊急用メモリを確保する
    AllocateReservedMemory();

    // NEXのグローバル変数の確保
    nn::nex::GlobalVariables::AcquireInstance();
    // パケットバッファ機能は今回使わない
    nn::nex::PacketBufferManager::EnablePacketBufferPool(false);

    // スレッドモードを設定
    nn::nex::Core::SetThreadMode( static_cast<nn::nex::Core::ThreadMode>(threadMode) );

    // AutoStackの使用設定
    nn::nex::Core::UseThreadAutoStack( isUsingAutoStack );

    m_ngsFacade         = qNew nn::nex::NgsFacade();
    m_isInit            = true;
    m_fillType = fillType;
  }

  return nn::ResultSuccess();
}


//------------------------------------------------------------------
/**
 * @brief  NEXライブラリの終了
 */
//------------------------------------------------------------------
void NexManager::Finalize()
{
  if( m_isInit )
  {
    // ゲームサーバ情報クラスの終了
    NexFreeMemory( &m_dataStoreClient );
    NexFreeMemory( &m_natClient );
    NexFreeMemory( &m_utilityClient );
    NexFreeMemory( &m_ngsFacade );

    //m_nexHeap.Dump();       // メモリの状態を表示する

    // NEXのグローバル変数の解放
    u32 freeCount = nn::nex::GlobalVariables::GetCount();
    for( u32 i = 0; i < freeCount; i++ )
    {
      nn::nex::GlobalVariables::ReleaseInstance();
    }

    // 緊急用メモリが残っていれば解放する
    ReleaseReservedMemory();
    PrintMemoryUsage();

    // メモリリークしている
    if( m_currentUsage > 0 )
    {
      GFL_PRINT("[NexManager is Memory Leak !!]\n");
      m_nexHeap.Dump();       // リークしているメモリを表示する
      m_nexHeap.Invalidate(); // リークしているメモリを無効化する
      GFL_PRINT("---------------------------------------\n");
    }
    nn::nex::MemoryManager::SetBasicMemoryFunctions( NULL, NULL );

    m_nexHeap.Finalize();

    if( m_mainBuffer != NULL )
    {
      GflHeapFreeMemory( m_mainBuffer );
      m_mainBuffer = NULL;
    }

    m_currentUsage       = 0;
    m_maxUsage           = 0;
    m_gameServerListener = NULL;
    m_threadMode         = ThreadModeUndefined;

    m_isInit = false;
  }
}


//------------------------------------------------------------------
/**
 * @brief NetZインスタンスの作成
 *
 * return 作成に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::CreateNetZ( void )
{
  // DOやホストマイグレーション拡張を使う場合に必要な処理
  nn::nex::NetZ::SetDOClassInitializationCallback(&nn::nex::InitDOClasses);

  m_netZ = qNew nn::nex::NetZ;

  //OHNO_PRINT("qNew nn::nex::NetZ  NexManager \n");
  if( !m_netZ->IsValid() )
  {
    NexFreeMemory( &m_netZ );
    return false;
  }

  // P2P始まる前にクリアする
  m_isKickMe = false;  // sessionをキックしたフラグ
  return true;
}



//------------------------------------------------------------------
/**
 * @brief NetZインスタンスの削除
 *
 * return 実行結果
 */
//------------------------------------------------------------------
nn::nex::qResult NexManager::DeleteNetZ( void )
{
  nn::nex::CallContext oContext;
  nn::nex::qResult     ret;

  if(m_netZ){
    m_netZ->Terminate(&oContext);

    if( !oContext.Wait() ){
      ret = QRESULT_ERROR(Core,Timeout);
    }
    else{
      ret = oContext.GetOutcome();
    }
    NexFreeMemory( &m_netZ );
    //OHNO_PRINT("qDelete nn::nex::NetZ  NexManager \n");
    return ret;
  }
  return nn::nex::qResult(true);
}

//------------------------------------------------------------------
/**
 * @brief ProtocolCallContextの生成
 *
 * @note  タイムアウトの設定も内部で行います（ProtocolCallContextへのタイムアウトの設定は一部関数（Login、Logout）を除いて必須）
 * @return ProtocolCallContext
 */
//------------------------------------------------------------------
nn::nex::ProtocolCallContext* NexManager::CreateProtocolCallContext( bool isEnableTimeout , u32 timeoutMSec )
{
  nn::nex::ProtocolCallContext* pContext        = qNew nn::nex::ProtocolCallContext();
  if( pContext && isEnableTimeout )
  {
    pContext->SetTimeout( timeoutMSec );
  }

  return pContext;
}


//------------------------------------------------------------------
/**
 * @brief NATトラバーサルをBind
 *
 * @note ゲームサーバにログイン後に実行してください
 *
 * return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::BindNATTraversalClient( void )
{
  nn::nex::Credentials* credential = m_ngsFacade->GetCredentials();

  if( credential == NULL ){
    return false;
  }

  // 未確保であれば確保
  if( m_natClient==NULL )
  {
    m_natClient = qNew nn::nex::NATTraversalClient::NATTraversalClient();
    if( m_natClient==NULL ){
      return false;
    }
  }
  if( m_natClient->GetCredentials() != NULL ){
    return false; // 既にバインド済み
  }

  return m_natClient->Bind(credential);
}


//------------------------------------------------------------------
/**
 * @brief NATトラバーサルをUnbind
 *
 * @note ゲームサーバからログアウトする前に実行してください
 *
 * return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::UnbindNATTraversalClient( void )
{
  bool ret = false;
  if( m_natClient )
  {
    ret = m_natClient->Unbind();
    NexFreeMemory( &m_natClient );
  }

  return ret;
}


//------------------------------------------------------------------
/**
 * @brief データストアクライアントをBind
 *
 * @note ゲームサーバにログイン後に実行してください
 *
 * return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::BindDataStoreClient( void )
{
  nn::nex::Credentials* credential = m_ngsFacade->GetCredentials();

  if( credential == NULL ){
    return false;
  }

  // 未確保であれば確保
  if( m_dataStoreClient==NULL )
  {
    m_dataStoreClient = qNew gflnet2::nex::NexDataStoreClient();
    if( m_dataStoreClient==NULL ){
      return false;
    }
  }

  return m_dataStoreClient->Initialize(credential);
}


//------------------------------------------------------------------
/**
 * @brief データストアクライアントのインスタンスを取得
 *
 * @note  事前に「BindDataStoreClient」をコールしてください
 *
 * return データストアクライアントのインスタンス
 */
//------------------------------------------------------------------
NexDataStoreClient* NexManager::GetDataStoreClient( void )
{
  return m_dataStoreClient;
}


//------------------------------------------------------------------
/**
 * @brief データストアクライアントをUnbind
 *
 * @note ゲームサーバからログアウトする前に実行してください
 *
 * return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::UnbindDataStoreClient( void )
{
  if( m_dataStoreClient==NULL ){
    return false;
  }
  m_dataStoreClient->Finalize();
  NexFreeMemory( &m_dataStoreClient );

  return true;
}


//------------------------------------------------------------------
/**
 * @brief NEXユニークIDユーティリティをBind
 *
 * @note ゲームサーバにログイン後に実行してください
 *
 * return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::BindUtilityClient( void )
{
  nn::nex::Credentials* credential = m_ngsFacade->GetCredentials();

  if( credential == NULL ){
    return false;
  }

  // 未確保であれば確保
  if( m_utilityClient==NULL )
  {
    m_utilityClient = qNew nn::nex::UtilityClient();
    if( m_utilityClient==NULL ){
      return false;
    }
  }
  if( m_utilityClient->GetCredentials() != NULL ){
    return false; // 既にバインド済み
  }

  return m_utilityClient->Bind(credential);

}


//------------------------------------------------------------------
/**
 * @brief NEXユニークIDユーティリティをUnbind
 *
 * @note ゲームサーバからログアウトする前に実行してください
 *
 * return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::UnbindUtilityClient( void )
{
  if( m_utilityClient==NULL ){
    return false;
  }
  if( m_utilityClient->GetCredentials() == NULL ){
    return false; // not bind!
  }

  bool ret = m_utilityClient->Unbind();

  NexFreeMemory( &m_utilityClient );

  return ret;
}


//------------------------------------------------------------------
/**
 * @brief NAT セッションを開始し、P2P通信に必要な前処理を行います
 *
 * @attention NetZを作成する前に必ず本関数を実行してください
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
nn::nex::qResult NexManager::StartNATSession( void )
{
  nn::nex::CallContext oContext;

  if( !nn::nex::ConnectivityManager::GetInstance()->StartNATSession( &oContext ) ){
    return nn::nex::qResult(false);
  }
  if( !oContext.Wait() ){
    return QRESULT_ERROR(Core,Timeout);
  }

  return oContext.GetOutcome();
}


//------------------------------------------------------------------
/**
 * @brief NAT セッションを終了します
 *
 * @attention NetZ::Terminate() 完了後、本関数を呼び出してください
 *
 * return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::StopNATSession( void )
{
  return nn::nex::ConnectivityManager::GetInstance()->StopNATSession();
}


//------------------------------------------------------------------
/**
 * @brief NEXのログ動作モードを設定する
 *
 * @param logLevel  出力レベル
 *
 * @param traceMode トレースモード
 */
//------------------------------------------------------------------
void NexManager::SetLogMode( LogLevel logLevel, TraceMode traceMode )
{
  nn::nex::EventLog::GetInstance()->SetLogLevel( static_cast<nn::nex::EventLog::LogLevel>(logLevel) );
  nn::nex::TraceLog::GetInstance()->SetFlag(traceMode);
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバへのログイン＆セッショントークンの取得(非同期関数）
 *
 * @attention  事前に[gflnet_FriendManager]にてフレンドサーバへのログインを完了してください
 * @attention  完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
 *
 * @note  非同期通信終了時に「NexGameServerListener::OnGameServerLoginSuccess()」がコールされる。
 * @note  エラー時は「NexGameServerListener::OnGameServerLoginFailed()」がコールされる。
 *
 * @param[in]  gameId      ゲームＩＤ
 * @param[in]  pAccessKey  アクセスキー
 * @param[in]  uiTimeout   通信タイムアウト時間
 * @param[in]  principalId 本体プリンシバルID
 *
 * @return ログイン開始処理の実行結果
 */
//------------------------------------------------------------------
nn::nex::qResult NexManager::LoginAndRequestAuthenticationToken( qUnsignedInt32 gameId, const qChar* pAccessKey,
                                                                 const char* pAuthToken, nn::nex::TimeInterval uiTimeout,
                                                                 nn::nex::PrincipalID principalId , bool isCheckParentalLock )
{
  if( !LockConnection() ){
    return nn::nex::qResult(false);
  }
  m_currentContext = NULL;


  ::std::memset(m_authToken, 0, sizeof(m_authToken));

  GFL_PRINT("NexManager::LoginAndRequestAuthenticationToken[%d]\n",isCheckParentalLock);

  //ペアレンタルスキップ設定
  m_ngsFacade->SetIgnoreNFSLoginCheck( !isCheckParentalLock );

  nn::nex::ProtocolCallContext* pContext        = CreateProtocolCallContext( false );
  nn::nex::qResult              loginFuncResult = m_ngsFacade->LoginAndRequestAuthenticationToken(pContext, gameId, pAccessKey, pAuthToken,
                                                                                                  m_authToken, uiTimeout, principalId );

  if( loginFuncResult == QRESULT_SUCCESS(Core,Success) )
  {
    nn::nex::MatchmakeExtensionClient::SetPersistentGatheringCodeKey( pAccessKey );
    m_currentContext = pContext;

    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexManager>(this, &NexManager::OnGameServerLoginCompletion, pContext );
  }
  else
  {
    if( m_gameServerListener != NULL ){
      m_gameServerListener->OnGameServerLoginFailed( loginFuncResult, NexError::GetNetworkErrorCode(loginFuncResult) );
    }
    UnlockConnection();
    qDelete pContext;
  }

  return loginFuncResult;
}




//------------------------------------------------------------------
/**
 * @brief  ゲームサーバへのログイン処理(非同期関数）
 *
 * @attention  事前に[gflnet_FriendManager]にてフレンドサーバへのログインを完了してください
 * @attention  完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
 *
 * @note  非同期通信終了時に「NexGameServerListener::OnGameServerLoginSuccess()」がコールされる。
 * @note  エラー時は「NexGameServerListener::OnGameServerLoginFailed()」がコールされる。
 *
 * @param[in]  gameId     ゲームＩＤ
 * @param[in]  pAccessKey アクセスキー
 * @param[in]  uiTimeout  通信タイムアウト時間
 *
 * @return ログイン開始処理の実行結果
 */
//------------------------------------------------------------------
nn::nex::qResult NexManager::LoginGameServerAsync( qUnsignedInt32 gameId, const qChar* pAccessKey, nn::nex::TimeInterval uiTimeout)
{
  if( !LockConnection() ){
    return nn::nex::qResult(false);
  }
  m_currentContext = NULL;

  nn::nex::ProtocolCallContext *pContext       = CreateProtocolCallContext( false );
  nn::nex::qResult             loginFuncResult = m_ngsFacade->Login(pContext, gameId, pAccessKey, uiTimeout);

  if( loginFuncResult == QRESULT_SUCCESS(Core,Success) )
  {
    nn::nex::MatchmakeExtensionClient::SetPersistentGatheringCodeKey( pAccessKey );
    m_currentContext = pContext;

    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexManager>(this, &NexManager::OnGameServerLoginCompletion, pContext );
  }
  else
  {
    if( m_gameServerListener != NULL ){
      m_gameServerListener->OnGameServerLoginFailed( loginFuncResult, NexError::GetNetworkErrorCode(loginFuncResult) );
    }
    UnlockConnection();
    qDelete pContext;
  }

  return loginFuncResult;
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバからのログアウト処理(非同期関数)
 *
 * @attention 完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
 *
 * @note  非同期通信終了時に「NexGameServerListener::OnGameServerLogoutSuccess()」がコールされる。
 * @note  エラー時は「NexGameServerListener::OnGameServerLogoutFailed()」がコールされる。
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
nn::nex::qResult NexManager::LogoutGameServerAsync( void )
{
  // WIFI未接続時は強制終了モードに設定する
  SetTerminateImmediately();

  // コミュニティーコードキーをクリア
  nn::nex::MatchmakeExtensionClient::ClearPersistentGatheringCodeKey();

  m_gameServerLogin    = false;
  m_isDisconnectHandle = false;

  // インスタンス未生成、ゲームサーバ未接続、AP切断
  if( (m_ngsFacade == NULL) || !m_ngsFacade->IsConnected() )
  {
    GFL_PRINT("NexManager::Logout Skip\n");
    // インスタンス未生成時は成功として扱う
    if(m_gameServerListener!=NULL){
      
      m_gameServerListener->OnGameServerLogoutSuccess();
    }
    return nn::nex::qResult(true);
  }

  // ゲームサーバからログアウト
  nn::nex::ProtocolCallContext* pContext         = CreateProtocolCallContext( false );
  nn::nex::qResult              logoutFuncResult = m_ngsFacade->Logout(pContext);

  if( logoutFuncResult == QRESULT_SUCCESS(Core,Success) )
  {
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexManager>(this, &NexManager::OnGameServerLogoutCompletion, pContext );
  }
  else
  {
    if( m_gameServerListener != NULL )
    {
      nn::nex::qUnsignedInt32 errCode = NexError::GetNetworkErrorCode( logoutFuncResult );
      m_gameServerListener->OnGameServerLogoutFailed( logoutFuncResult, errCode );
    }
    qDelete pContext;
  }

  return logoutFuncResult;
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバからのログアウト処理(同期関数)
 *
 * @attention 完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
 *
 * @note  同期通信終了時に「NexGameServerListener::OnGameServerLogoutSuccess()」がコールされる。
 * @note  エラー時は「NexGameServerListener::OnGameServerLogoutFailed()」がコールされる。
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
nn::nex::qResult NexManager::LogoutGameServer( void )
{
  // WIFI未接続時は強制終了モードに設定する
  SetTerminateImmediately();

  // コミュニティーコードキーをクリア
  nn::nex::MatchmakeExtensionClient::ClearPersistentGatheringCodeKey();

  m_gameServerLogin    = false;
  m_isDisconnectHandle = false;

  // インスタンス未生成、ゲームサーバ未接続、AP切断
  if( (m_ngsFacade == NULL) || !m_ngsFacade->IsConnected() )
  {
    GFL_PRINT("NexManager::Logout Skip\n");

    if(m_gameServerListener!=NULL){
      m_gameServerListener->OnGameServerLogoutSuccess();
    }
    return nn::nex::qResult(true);
  }

  // ゲームサーバからログアウト
  nn::nex::ProtocolCallContext* pContext         = CreateProtocolCallContext( false );
  nn::nex::qResult              logoutFuncResult = m_ngsFacade->Logout(pContext);

  if( logoutFuncResult == QRESULT_SUCCESS(Core,Success) )
  {
    pContext->Wait();
    this->OnGameServerLogoutCompletion( pContext );
  }
  else
  {
    if( m_gameServerListener != NULL )
    {
      nn::nex::qUnsignedInt32 errCode = NexError::GetNetworkErrorCode( logoutFuncResult );
      m_gameServerListener->OnGameServerLogoutFailed( logoutFuncResult, errCode );
    }
  }
  qDelete pContext;

  return logoutFuncResult;
}


//------------------------------------------------------------------
/**
 * @brief  独自サーバアクセス用認証トークンを取得する
 *
 * @attention   事前にRequestAuthenticationToken()をコールしてください
 *
 * @return 独自サーバアクセス用認証トークン
 */
//------------------------------------------------------------------
const char* NexManager::GetAuthenticationToken( void ) const
{
  return m_authToken;
}


//------------------------------------------------------------------
/**
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool NexManager::IsConnecting( void ) const
{
  return m_isConnecting;
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
bool NexManager::CancelConnecting( void )
{
  if( m_isConnecting && (m_currentContext != NULL) )
  {
    if( m_currentContext->GetState() == nn::nex::CallContext::CallInProgress ){
      return m_currentContext->Cancel();
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief  NEXユニークIDを発行する(非同期関数）
 *
 * @attention   完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
 * @attention   事前にBindUtilityClient()をコールしてください
 *
 * @note   本関数が失敗した場合は、終了通知リスナはコールされない。
 *
 * @return 実行に成功したならtrueが返却される
 */
//------------------------------------------------------------------
bool NexManager::AcquireNexUniqueIdASync( void )
{
  if( m_utilityClient==NULL ){
    return false;
  }
  if( !LockConnection() ){
    return false;
  }
  m_currentContext = NULL;

  nn::nex::ProtocolCallContext *pContext = CreateProtocolCallContext();

  if( m_utilityClient->AcquireNexUniqueId(pContext, &m_uniqueId) )
  {
    m_currentContext = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew CallContextCallback<NexManager>(this, &NexManager::OnAcquireNexUniqueIdCompletion, pContext );
    return true;
  }
  else
  {
    UnlockConnection();
    qDelete pContext;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバからのイベント通知先リスナの登録
 *
 * @param listener 通知先のリスナ。NULLを指定することで登録を解除できる。
 */
//------------------------------------------------------------------
void NexManager::SetGameServerListener( NexGameServerListener* listener )
{
  m_gameServerListener = listener;
}


//------------------------------------------------------------------
/**
 * @brief NEXエラー通知先リスナの登録
 *
 * @param listener 通知先のリスナ。NULLを指定することで登録を解除できる。
 */
//------------------------------------------------------------------
void NexManager::SetNexErrorListener( NexErrorListener* listener )
{
  m_errorListener = listener;
}

//------------------------------------------------------------------
/**
 * @brief  Utilityイベント通知先リスナの登録
 *
 *    * @param[in,out] listener 通知先のリスナ。NULLを指定することで登録を解除できる。
 */
//------------------------------------------------------------------
void NexManager::SetUtilityListener( NexUtilityListener* listener )
{
  m_utilityListener = listener;
}

//------------------------------------------------------------------
/**
 * @brief NEXゲームサーバ用イベントの通知先リスナを登録
 *
 * @param event 登録する通知イベントハンドラー
 *
 * @return 成功ならtrue、失敗ならfalseを返却
 */
//------------------------------------------------------------------
qBool NexManager::RegisterNotificationEventHandler( nn::nex::NotificationEventHandler *event )
{
  if( m_ngsFacade != NULL )
  {
    return m_ngsFacade->RegisterNotificationEventHandler( event );
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief NEXゲームサーバ用イベントの通知先リスナを登録
 *
 * @param event 登録する通知イベントハンドラー
 *
 * @return 成功ならtrue、失敗ならfalseを返却
 */
//------------------------------------------------------------------
qBool NexManager::UnregisterNotificationEventHandler( nn::nex::NotificationEventHandler *event )
{
  if( m_ngsFacade != NULL )
  {
    return m_ngsFacade->UnregisterNotificationEventHandler( event );
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief Dispatchスレッドポーリング処理用イベントの通知先リスナを登録
 *
 * @param[in,out] event 登録する通知イベントハンドラー
 *
 * @return 成功ならtrue、失敗ならfalseを返却
 */
//------------------------------------------------------------------
qBool NexManager::RegisterDispatchThreadEventHandler( NexDispatchThreadListener *event )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_dispatchListener[i] == event ){
      return true;  // 登録済み
    }
  }

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_dispatchListener[i] == NULL )
    {
      m_dispatchListener[i] = event;
      return true;
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief Dispatchスレッドポーリング処理用イベントの通知先リスナを解除
 *
 * @param[in,out] event 解除する通知イベントハンドラー
 *
 * @return 成功ならtrue、失敗ならfalseを返却
 */
//------------------------------------------------------------------
qBool NexManager::UnregisterDispatchThreadEventHandler( NexDispatchThreadListener *event )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < LISTENER_NUM; i++ )
  {
    if( m_dispatchListener[i] == event )
    {
      m_dispatchListener[i] = NULL;
      return true;
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief Dispatchスレッドポーリングイベント用リスナ関数の呼び出し
 */
//------------------------------------------------------------------
void NexManager::CallDispatchThreadEventListener( void )
{
  // 本来は下記関数も排他制御対象だがメインスレッドと
  // Dispatchスレッドは並列実行されないことを前提に排他対象外とする
  {
    //gflnet2::err::ErrManager* errMgr = GFL_SINGLETON_INSTANCE(gflnet2::err::ErrManager);
    //if( (errMgr != NULL) && errMgr->IsSystemReloadError() ){
     // return; // 継続不可能なエラー状態にある
    //}
    if( !IsGameServerConnecting() ){
      return; // ゲームサーバから切断された
    }
  }

  // 以下は排他制御対象
  {
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);

    for( u32 i = 0; i < LISTENER_NUM; i++ )
    {
      if( m_dispatchListener[i] != NULL ){
        m_dispatchListener[i]->OnNexDispatchPollingEvent();
      }
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  緊急用メモリの確保処理
 */
//------------------------------------------------------------------
void NexManager::AllocateReservedMemory()
{
  if( m_reservedBuffer == NULL )
  {
    GFL_PRINT("call AllocateReservedMemory()\n");

    u32 nowUsage    = m_currentUsage;
    u32 nowMaxUsage = m_maxUsage;

    m_reservedBuffer = AllocFunction(m_reserveBufferSize);

    m_currentUsage = nowUsage;
    m_maxUsage     = nowMaxUsage;
  }
}


//------------------------------------------------------------------
/**
 * @brief  緊急用メモリの解放処理
 */
//------------------------------------------------------------------
void NexManager::ReleaseReservedMemory()
{
  if( m_reservedBuffer != NULL )
  {
    GFL_PRINT("call ReleaseReservedMemory()\n");

    u32 nowUsage    = m_currentUsage;
    u32 nowMaxUsage = m_maxUsage;

    FreeFunction(m_reservedBuffer);

    m_reservedBuffer    = NULL;
    m_reserveBufferSize = 0;

    m_currentUsage  = nowUsage;
    m_maxUsage      = nowMaxUsage;
  }
}


//------------------------------------------------------------------
/**
 * @brief  確保した緊急メモリが使用されている状況か判定
 *
 * @reutrn 緊急メモリが使用されている状況ならtrueを返却
 */
//------------------------------------------------------------------
qBool NexManager::IsLowMemory() const
{
  return (m_reservedBuffer == NULL);
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバログイン時に発行された証明書を取得
 *
 * @reutrn 証明書へのポインタ。ログアウト時はNULLが返却される。
 */
//------------------------------------------------------------------
nn::nex::Credentials* NexManager::GetCredentials( void )
{
  if( m_ngsFacade != NULL ){
    return m_ngsFacade->GetCredentials();
  }
  return NULL;
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバログイン情報を取得
 *
 * @return ゲームサーバログイン情報へのポインタ。ログアウト時はNULLが返却される。
 */
//------------------------------------------------------------------
nn::nex::NgsFacade* NexManager::GetNgsFacade( void )
{
  return m_ngsFacade;
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバへの接続状況を取得
 *
 * @return ゲームサーバに接続しているならtrueを返却する
 */
//------------------------------------------------------------------
bool NexManager::IsGameServerConnecting( void ) const
{
  return (m_ngsFacade!=NULL) && m_ngsFacade->IsConnected();
}


//------------------------------------------------------------------
/**
 * @brief ゲームサーバへ同期通信を行い疎通テストを行う（同期関数）
 *
 * @param timeout タイムアウトに設定する時間（ミリ秒）
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
bool NexManager::TestConnectivity( u32 timeout )
{
  if( !IsGameServerConnecting() ){
    return false;
  }

  if( !LockConnection() ){
    return false;
  }
  m_currentContext = NULL;

  nn::nex::ProtocolCallContext *pContext = CreateProtocolCallContext( true , timeout );

  // 疎通テストAPI
  if( !m_ngsFacade->TestConnectivity( pContext ) ){
    // 呼び出し失敗
    UnlockConnection();
    qDelete pContext;
    return false;
  }

  m_currentContext = pContext;
  // [pContext]コンテキストは終了コールバックで解放される
  qNew CallContextCallback<NexManager>(this, &NexManager::OnTestConnectivity, pContext );

  return true;
}


//------------------------------------------------------------------
/**
 * @brief  ＮＥＸエンジンの処理を実行する。
 *         内部スレッドモード以外のスレッドモードを指定した場合のみ実行すること
 *
 * @param[in]  time          タイムアウト時間。指定した時間内に処理が終わらない場合は次回コール時まで処理を中断する
 * @param[in]  isDispatchAll 全てのディスパッチコマンドを昇華させるならtrueを指定する
 */
//------------------------------------------------------------------
void NexManager::Dispatch( u32 time, bool isDispatchAll )
{
  if( GetIsLockDispatch() == false )
  {
    nn::nex::Scheduler* scheduler = nn::nex::Scheduler::GetInstance();
    if( scheduler != NULL )
    {
      if( isDispatchAll ){
        scheduler->DispatchAll();
      }
      else{
        scheduler->Dispatch( time );
      }
    }
  }

  if( m_isInit )
  {

    // ゲームサーバの予期せぬ切断
    if( m_gameServerLogin )
    {
      if( !m_isDisconnectHandle )
      {
        if( !IsGameServerConnecting() )
        {
          if( m_errorListener != NULL ){
            ICHI_PRINT("NexManager::Dispatch OnNexErrorGameServerShutdown\n");
            m_errorListener->OnNexErrorGameServerShutdown();
          }
          m_isDisconnectHandle = true;
        }
      }
    }

    // NetZエラーチェック
    nn::nex::NetZ* pNetZ = nn::nex::NetZ::GetInstance();
    if( pNetZ != NULL )
    {
      nn::nex::qResult fatal = pNetZ->GetFatalError();

      if ( QFAILED(fatal.GetReturnCode()) )
      {
        // @fix NMCat[4155]：NetZFatal発生時に、エラーアプレット送るまで相手をはめれるのでKickMeで離脱する
        nn::nex::Station *pStasion = gflnet2::dev::NetZAdapter::GetStationInstance();
        if( pStasion != NULL && !m_isKickMe )
        {
          pStasion->KickMe();
          m_isKickMe = true;
          GFL_RELEASE_PRINT("!!!KickMe(NetZFatal)!!!\n");
        }


        if( m_errorListener != NULL ){
          ICHI_PRINT("NexManager::Dispatch OnNexErrorHandling\n");
          m_errorListener->OnNexErrorHandling( fatal );
        }
      }
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  非同期ディスパッチ処理を開始する
 *
 * @attention 終了時に必ずEndDispatchAsync()をコールしてください。
 * @attention Dispatch()とは必ず排他させてください
 *
 * @param  heap              スレッド用に使用するヒープメモリ
 *
 * @return スレッドの開始に成功したならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
bool NexManager::StartDispatchAsync( gfl2::heap::HeapBase* heap )
{
  if( m_dispatchThread == NULL )
  {
    m_dispatchThread = GFL_NEW_LOW( heap ) NexDispatchThread( this, heap, THREAD_STACK_SIZE );

    m_dispatchThread->Start();
  }

  return true;
}


//------------------------------------------------------------------
/**
 * @brief 非同期ディスパッチ処理用スレッドを破棄する
 */
//------------------------------------------------------------------
void NexManager::EndDispatchAsync( void )
{
  if( m_dispatchThread != NULL )
  {
    m_dispatchThread->StopDispatch();

    while( ( m_dispatchThread && m_dispatchThread->IsAlive() ) ) {
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
    }

    GFL_SAFE_DELETE( m_dispatchThread );
  }
}


//------------------------------------------------------------------
/**
* @brief 通信ロックを行う
*
* @return すでに通信接続ロック中であればfalseを返却
*/
//------------------------------------------------------------------
bool NexManager::LockConnection( void )
{
  if( !m_isInit || m_isConnecting )
  {
    return false;   // 通信中, 未初期化
  }
  m_isConnecting = true;

  return true;
}


//------------------------------------------------------------------
/**
* @brief 通信ロックを解除する
*/
//------------------------------------------------------------------
void NexManager::UnlockConnection( void )
{
  m_currentContext = NULL;
  m_isConnecting   = false;
}


//------------------------------------------------------------------
/**
 * @brief  NEXライブラリで内部使用されるメモリ確保処理
 *         コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  size  確保するメモリサイズ
 *
 * @return 確保されたヒープメモリアドレス
 */
//------------------------------------------------------------------
void* NexManager::AllocFunction(size_t size)
{
  void* allocatedPtr = InnerAlloc(size);

  // 緊急用メモリを使用し、エラー通知
  if( allocatedPtr == NULL )
  {
    #if GFL_DEBUG
    GFL_PRINT("[NexManager is Memory FULL !!] %d\n",size);
    m_nexHeap.Dump();       // メモリを表示する
    #endif

    ReleaseReservedMemory();
    allocatedPtr = InnerAlloc(size);

    if( m_errorListener != NULL ){
      m_errorListener->OnNexErrorOutOfMemory();
    }
  }
  return allocatedPtr;
}


//------------------------------------------------------------------
/**
 * @brief  NEXライブラリで内部使用されるメモリ解放処理
 *         コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pointer 解放対象となるポインタ
 */
//------------------------------------------------------------------
void NexManager::FreeFunction(void *pointer)
{
  InnerFree(pointer);
}


//------------------------------------------------------------------
/**
 * @brief  AllocFunction から呼ばれるメモリ確保処理
 *
 * @param  size  確保するメモリサイズ
 *
 * @return 確保されたヒープメモリアドレス
 */
//------------------------------------------------------------------
void* NexManager::InnerAlloc( size_t size )
{
  void* pBuffer = m_nexHeap.Allocate(size);

  if( pBuffer != NULL )
  {
    m_currentUsage += m_nexHeap.GetSizeOf(pBuffer);

    if( m_currentUsage > m_maxUsage ){
      m_maxUsage = m_currentUsage;
    }
  }

  return pBuffer;
}


//------------------------------------------------------------------
/**
 * @brief  FreeFunction から呼ばれるメモリ解放処理
 *
 * @param  ptr 解放対象となるポインタ
 */
//------------------------------------------------------------------
void NexManager::InnerFree( void* ptr )
{
  u32 memSize = m_nexHeap.GetSizeOf(ptr);

#if GFL_DEBUG

  if( m_fillType == FILL_TYPE_0xFF )
  {
    std::memset( ptr , 0xFF , memSize );
  }
  else if( m_fillType == FILL_TYPE_0x00 )
  {
    std::memset( ptr , 0x00 , memSize );
  }

#endif

  m_nexHeap.Free(ptr);
  m_currentUsage -= memSize;
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバ非同期ログイン検知用関数
 *         コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void  NexManager::OnGameServerLoginCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    if( m_gameServerListener != NULL ){
      m_gameServerListener->OnGameServerLoginCanceled();
    }
    return;
  }

  nn::nex::qResult result = pContext->GetOutcome();

  if( result == QRESULT_SUCCESS(Core,Success) )
  {
    if( m_ngsFacade->IsConnected() )
    {
      if( m_gameServerListener != NULL ){
        m_gameServerListener->OnGameServerLoginSuccess( m_ngsFacade->GetCredentials() );
      }
      m_gameServerLogin    = true;
      m_isDisconnectHandle = false;
      return;
    }
  }

  qInt32 errorCode = m_ngsFacade->GetLastLoginErrorCode();
  if( m_gameServerListener != NULL ){
    m_gameServerListener->OnGameServerLoginFailed( result, errorCode );
  }
}


//------------------------------------------------------------------
/**
 * @brief  ゲームサーバ非同期ログアウト検知用関数
 *         コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void  NexManager::OnGameServerLogoutCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  if( m_gameServerListener == NULL ){
    return;
  }

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    m_gameServerListener->OnGameServerLogoutCanceled();
  }
  else
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if( (qBool)result )
    {
      m_gameServerListener->OnGameServerLogoutSuccess();
    }
    else
    {
      u32 errorCode = NexError::GetNetworkErrorCode(result);
      m_gameServerListener->OnGameServerLogoutFailed( result, errorCode );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  NEXユニークID発行の非同期終了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void  NexManager::OnAcquireNexUniqueIdCompletion(nn::nex::CallContext *pContext)
{
  UnlockConnection();

  if( m_gameServerListener == NULL ){
    return;
  }

  // 通信キャンセル時
  if( pContext->GetState() == nn::nex::CallContext::CallCancelled )
  {
    m_gameServerListener->OnGameServerLogoutCanceled();
  }
  else
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if(result == QRESULT_SUCCESS(Core,Success) )
    {
      m_gameServerListener->OnAcquireNexUniqueIdSuccess(m_uniqueId);
    }
    else
    {
      qInt32 errorCode = NexError::GetNetworkErrorCode(result);
      m_gameServerListener->OnAcquireNexUniqueIdFailed( result, errorCode );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  ゲームサーバ非同期疎通テスト検知用関数
 *         コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void  NexManager::OnTestConnectivity(nn::nex::CallContext *pContext)
{
  nn::nex::qResult result;

  UnlockConnection();

  result = pContext->GetOutcome();
  // リスナー通知
  if( m_utilityListener != NULL ){
    m_utilityListener->OnTestConnectivityResult( result );
  }
}

//------------------------------------------------------------------
/**
 * @brief  ＮＥＸログレベルをＤＥＢＵＧモードに設定（デバッグ用）
 */
//------------------------------------------------------------------
void NexManager::EnableDebugLog( void )
{
#if GFL_DEBUG
  nn::nex::EventLog::GetInstance()->SetLogLevel(nn::nex::EventLog::Debug);
  nn::nex::TraceLog::GetInstance()->SetFlag(TRACE_ALWAYS);
#endif
}


//------------------------------------------------------------------
/**
 * @brief  使用されているメモリをコンソール出力する（デバッグ用）
 */
//------------------------------------------------------------------
void NexManager::PrintMemoryUsage() const
{
#if GFL_DEBUG
  GFL_PRINT("---------------------------------------\n");
  GFL_PRINT("Now memory usage = %d\n", m_currentUsage);
  GFL_PRINT("Max memory usage = %d\n", m_maxUsage);
  GFL_PRINT("---------------------------------------\n");
#endif
}


//------------------------------------------------------------------
/**
 * @brief 使用されているメモリをダンプ表示する（デバッグ用）
 */
//------------------------------------------------------------------
void NexManager::PrintMemoryDump( void )
{
#if GFL_DEBUG
  m_nexHeap.Dump();       // リークしているメモリを表示する
#endif
}







//------------------------------------------------------------------
/**
 * @brief  TerminateFacadeの非同期終了検知用関数
 *
 * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param[in,out]  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void NexManager::OnTerminateNgsFacadeCompletion(nn::nex::CallContext *pContext)
{
  if( m_gameServerListener == NULL ){
    return;
  }
  // 実行結果に関係なく終了通知
  m_gameServerListener->OnGameServerTerminated();
}

//------------------------------------------------------------------
/**
 * @brief 強制終了モード設定
 */
//------------------------------------------------------------------
void NexManager::SetTerminateImmediately( bool isForceExec )
{
  // WIFI未接続時は強制終了モードに設定する
  nn::nex::Core* nexCore = nn::nex::Core::GetInstance();
  if( nexCore != NULL )
  {
    bool                      isConnectAP;
    gflnet2::wifi::WifiSystem* wifiSystem = GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem);

    if( wifiSystem != NULL ){
      isConnectAP = wifiSystem->IsConnect();
    }
    else{
      isConnectAP = nn::ac::CTR::IsConnected();
    }
    // APとの接続が未確立であれば強制終了モード
    nexCore->SetTerminateImmediately( !isConnectAP||isForceExec );
  }
}

//------------------------------------------------------------------
/**
 * @brief  ゲームサーバ通信の終了手続き(非同期処理）
 *
 * @attention 完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
 *
 * @return 実行結果
 */
//------------------------------------------------------------------
nn::nex::qResult NexManager::TerminateFacade( void )
{
  SetTerminateImmediately(true);

  if( m_ngsFacade == NULL)
  {
    // インスタンス未生成時は成功として扱う
    if(m_gameServerListener!=NULL){
      m_gameServerListener->OnGameServerTerminated();
    }
  }
  else
  {
    // ゲームサーバからログアウト
    nn::nex::ProtocolCallContext* pContext = CreateProtocolCallContext();

    if( m_ngsFacade->Terminate(pContext) )
    {
      // [pContext]コンテキストは終了コールバックで解放される
      qNew CallContextCallback<NexManager>(this, &NexManager::OnTerminateNgsFacadeCompletion, pContext );
    }
    else
    {
      // 失敗時は強制終了モードに設定して成功として扱う
      if( m_gameServerListener != NULL ){
        m_gameServerListener->OnGameServerTerminated();
      }
      qDelete pContext;
    }
  }

  return nn::nex::qResult(true);
}
























} // nex
} // gflnet

#endif

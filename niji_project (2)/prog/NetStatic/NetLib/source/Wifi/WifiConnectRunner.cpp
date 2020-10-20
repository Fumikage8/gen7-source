#if defined(GF_PLATFORM_CTR)
//============================================================================================
/**
 * @file WifiConnectRunner.cpp
 *
 * @author suzuki_hisanao
 */
//============================================================================================
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>

#include <NetStatic/NetLib/include/NetDefine.h>
#include <gflnet2/include/http/gflnet2_HttpSystem.h>
#include <gflnet2/include/socket/gflnet2_SocketSystem.h>
#include <System/include/HeapDefine.h>
#include <System/include/ThreadPriority.h>
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>

#include  <debug/include/gfl2_DebugLogWindow.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)



//------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 *
 * @param  execMode 実行モード
 */
//------------------------------------------------------------------
WifiConnectRunner::WifiConnectRunner( E_EXEC_MODE execMode , bool isSkipParentalLock ) :
    gflnet2::friends::FriendLoginListener(),
    gflnet2::nex::NexGameServerListener(),
    m_execMode(execMode),
    m_isError(false),
    m_isErrorReserved(false),
    m_isFinish(false),
    m_isFriendLoginFinish(false),
    m_isGameServerLoginFinish(false),
    m_isGameServerLogoutFinish(false),
    m_isWifiConnectFinish(false),
    m_isWifiDisconnectFinish(false),
    m_step(0),
    m_errorCode(0),
    m_nexUniqueId(0),
    m_isSkipParentalLock(isSkipParentalLock),
    m_isGameServerTerminateFinish(false)
{
  m_friendMgr       = GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager);
  m_nexMgr          = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  m_wifiSystem      = GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem);
  //m_errorMgr        = GFL_SINGLETON_INSTANCE(gflnet2::err::ErrManager);
  m_subscrptionMgr  = GFL_SINGLETON_INSTANCE(NetLib::Wifi::SubscriptionManager);

  m_criticalSection.Initialize();

  m_nexMgr->SetGameServerListener( this );
}


//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
WifiConnectRunner::~WifiConnectRunner()
{
  Finalize();
}


//------------------------------------------------------------------
/**
 * @brief   終了処理。必ずコールすること
 */
//------------------------------------------------------------------
void WifiConnectRunner::Finalize( void )
{
  if( m_wifiSystem != NULL ){
    m_wifiSystem->DeleteAsync();
  }
  if( m_friendMgr != NULL ){
    m_friendMgr->DeleteAsync();
  }

  if( m_nexMgr->IsConnecting() ){
    m_nexMgr->CancelConnecting();
  }
  m_nexMgr->SetGameServerListener( NULL );

  m_criticalSection.Finalize();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   発行されたNEXユニークIDを取得
 *
 * @note    返却値が0の場合は未取得状態を指す
 *
 * @return  発行されたNEXユニークID
 */
//--------------------------------------------------------------------------------------------
nn::nex::UniqueId WifiConnectRunner::GetNexUniqueId( void ) const
{
  return m_nexUniqueId;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   エラー時に切断の手続きが必要か判断する
 *
 * @return  切断の手続きが必要であればtrueを返却する
 */
//--------------------------------------------------------------------------------------------
bool WifiConnectRunner::IsNeedDisconnectProcess( void ) const
{
  return m_isWifiConnectFinish;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief アクセスステップを取得する
 *
 * @return アクセスステップ
 */
//--------------------------------------------------------------------------------------------
u32 WifiConnectRunner::GetActionStep( void ) const
{
  return m_step;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   更新処理
 *
 * @return  実行ステータスコード
 */
//--------------------------------------------------------------------------------------------
WifiConnectRunner::E_RET_CODE WifiConnectRunner::Update( void )
{
  //ビーコンが動いていたら停止まで待つ
  if( NetLib::NijiNetworkSystem::UpdateStopBeaconSystem() == false )
  {
    return E_RET_CONTINUE;
  }

  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( E_MODE_DISCONNECT == m_execMode ){
    Disconnect( true );
  }
  else if( E_MODE_DISCONNECT_SYNC == m_execMode ){
    Disconnect( false );
  }
  else
  {
    Connect();
  }

  if( m_isError )
  {
    if( m_errorCode != PATCH_ERROR || m_isSkipParentalLock == false )//ふしぎなおくりもの以外は必ずこっち
    {
      NetLib::Error::NijiNetworkError error;
      error.SetResultWifiDisconnect( m_errorCode );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
      return E_RET_FAILED;
    }
    else
    {//パッチエラーだったらアプレット出さない（ふしぎなおくりもの限定）
      return E_RET_FAILED_PATCH;
    }
  }
  else if( IsFinished() )
  {
    if( m_isErrorReserved ){
      return E_RET_FAILED;
    }
    return E_RET_SUCCESS;
  }

  return E_RET_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief   HTTPシステムの利用を開始する
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::StartupHttpSystem( void )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
  nn::Result result = gflnet2::http::HttpSystem::Initialize( heap, NetLib::Validation::PokemonValidation::POST_BUFF_SIZE );

  if( result.IsFailure() )
  {
    if( result == nn::http::ResultClientProcessMaxErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem::Initialize() failed  / ResultClientProcessMaxErr! \n"  );
    }
    else if( result == nn::http::ResultIpcSessionMaxErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem::Initialize() failed  / ResultIpcSessionMaxErr! \n"  );
    }
    else if( result == nn::http::ResultAlreadyInitializedErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem::Initialize() failed  / ResultAlreadyInitializedErr! \n"  );
    }
    else{
      GFL_PRINT("gflnet2::http::HttpSystem::Initialize() failed  / Reason Unknown[%d] \n", result.GetDescription()  );
    }
    return true;
  }
  GFL_PRINT( "  StartupHttpSystem success\n" );

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   HTTPシステムを終了する
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::EndHttpSystem( void )
{
  nn::Result result = gflnet2::http::HttpSystem::Finalize();

  if( result.IsFailure() )
  {
    return true;
  }
  GFL_PRINT( "  EndHttpSystem success\n" );

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   Ｗｉｆｉアクセスポイントの開始
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::StartupWifiSystem( void )
{
  m_errorCode = 0;
  return !m_wifiSystem->StartWifiASync( this, System::ThreadPriority::WIFI_CONNECT_THREAD_PRIORITY );
}


//------------------------------------------------------------------
/**
 * @brief   アクセスポイントとの接続を終了する
 *
 * @param   isAsync 同期処理を行うならtrueを指定
 *
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::EndWifiSystem( bool isAsync )
{
  //m_errorMgr->ResetWifiDisconnectListener(m_wifiSystem);

  if( isAsync )
  {
    m_errorCode = 0;
    return !m_wifiSystem->EndWifiASync( this, System::ThreadPriority::WIFI_CONNECT_THREAD_PRIORITY );
  }
  else
  {
    m_wifiSystem->Close();
    m_wifiSystem->FinalizeAC();
    m_isWifiDisconnectFinish = true;
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief   Ｓｏｃｋｅｔシステムの開始
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::StartupSocketSystem( void )
{
  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"nn::socket::Initialize\n");
  #endif

  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
  nn::Result result = gflnet2::socket::SocketSystem::Initialize( heap );

  if( result.IsFailure() )
  {
    m_errorCode = 0;
    return true;
  }

  gflnet2::socket::SocketSystem::DumpInfo();

  GFL_PRINT( "StartupSocketSystem success\n" );

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   Ｓｏｃｋｅｔシステムの終了
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::EndSocketSystem( void )
{
  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"nn::socket::Finalize\n");
  #endif

  nn::Result result = gflnet2::socket::SocketSystem::Finalize();

  if( result.IsFailure() )
  {
    m_errorCode = 0;
    return true;
  }
  GFL_PRINT( "EndSocketSystem success\n" );

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   NEXユニークIDユーティリティのBind
 *
 * @return  BINDに失敗したならtrueを返却
 */
//------------------------------------------------------------------
bool WifiConnectRunner::BindUtilityClient( void )
{
  if( !m_nexMgr->BindUtilityClient() )
  {
    return true;
  }
  GFL_PRINT( "BindUtilityClient() Success\n" );
  return false;
}


//------------------------------------------------------------------
/**
 * @brief   NEXユニークIDの発行
 *
 * @return  発行処理に失敗したならtrueを返却
 */
//------------------------------------------------------------------
bool WifiConnectRunner::AcquireNexUniqueId( void )
{
  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"UtilityClient::AcquireNexUniqueId() \n");
  #endif

  if( !m_nexMgr->AcquireNexUniqueIdASync() )
  {
    return true;
  }
  GFL_PRINT( "AcquireNexUniqueIdASync() Success\n" );
  return false;
}


//------------------------------------------------------------------
/**
 * @brief   NEXユニークIDユーティリティのUnbind
 */
//------------------------------------------------------------------
void WifiConnectRunner::UnbindUtilityClient( void )
{
  m_nexMgr->UnbindUtilityClient();

  GFL_PRINT( "UnbindUtilityClient() Success\n" );
}


//------------------------------------------------------------------
/**
 * @brief   フレンドサーバへのログインを開始する
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::StartupFriendService( void )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"nn::friends::CTR::Login\n");
  #endif

  if( m_friendMgr->LoginASync( heap, this, nn::fnd::TimeSpan::FromSeconds(30), System::ThreadPriority::WIFI_CONNECT_THREAD_PRIORITY) == nn::ResultSuccess() )
  {
    GFL_PRINT( "friendMgr->Login() Success\n" );
    return false;
  }
  else
  {
    GFL_PRINT( "StartupFriendService failed\n" );
    return true;
  }
}


//------------------------------------------------------------------
/**
 * @brief  CTRのフレンドキーリストをマネージャにストア
 *
 * @return エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::StoreFriendList( void )
{
  nn::Result ret = m_friendMgr->StoreFriendKeyList();

  if( ret.IsFailure() )
  {
    GFL_PRINT( "friendMgr::StoreFriendKeyList() failed\n" );
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   フレンドサーバからのログアウトを開始する
 */
//------------------------------------------------------------------
void WifiConnectRunner::EndFriendService( void )
{
  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"nn::friends::CTR::Logout\n");
  #endif

  m_friendMgr->Logout();
  GFL_PRINT( "Friend Server Logout\n" );
}


//------------------------------------------------------------------
/**
 * @brief   NEXサービスを開始する
 * @return  エラーが発生したならtrueを返却する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::StartupNexService( void )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );


#if GFL_HEAP_DEBUG
  gflnet2::nex::NexManager::FREE_MEMORY_FILL_TYPE fillType = gflnet2::nex::NexManager::FILL_TYPE_NONE;
  if( gfl2::heap::HeapBase::GetDebugFreeMemClear() == gfl2::heap::HeapBase::DEBUG_FREE_MEMC_ZERO )//0クリア
  {
    fillType = gflnet2::nex::NexManager::FILL_TYPE_0x00;
  }
  else if( gfl2::heap::HeapBase::GetDebugFreeMemClear() == gfl2::heap::HeapBase::DEBUG_FREE_MEMC_FF )//FFクリア
  {
    fillType = gflnet2::nex::NexManager::FILL_TYPE_0xFF;
  }
  if( m_nexMgr->Initialize( heap, FOR_NEX_LIB_HEAP_SIZE, FOR_NEX_LIB_SPARE_HEAP_SIZE, gflnet2::nex::NexManager::ThreadModeUnsafeTransportBuffer ,false ,fillType) == nn::ResultSuccess() )
#else
  if( m_nexMgr->Initialize( heap, FOR_NEX_LIB_HEAP_SIZE, FOR_NEX_LIB_SPARE_HEAP_SIZE, gflnet2::nex::NexManager::ThreadModeUnsafeTransportBuffer ) == nn::ResultSuccess() )
#endif // GFL_HEAP_DEBUG
  {
    GFL_PRINT( "NexManager::Initialize() Success\n" );

    m_nexMgr->SetLogMode( gflnet2::nex::NexManager::Debug, gflnet2::nex::NexManager::E_TRACE_ALWAYS );
    return false;
  }
  else
  {
    GFL_PRINT( "StartupNexService failed\n" );
    return true;
  }
}


//------------------------------------------------------------------
/**
 * @brief   NEXサービスを終了する
 */
//------------------------------------------------------------------
void WifiConnectRunner::EndNexService( void )
{
  m_nexMgr->Finalize();
  GFL_PRINT( "NexManager::Finalize()\n" );
}


//------------------------------------------------------------------
/**
 * @brief   ゲームサーバーへのログインを開始する
 */
//------------------------------------------------------------------
bool WifiConnectRunner::LoginGameServer( void )
{
  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"NgsFacade::LoginAndRequestAuthenticationToken() \n");
  #endif

  if( !m_nexMgr->LoginAndRequestAuthenticationToken( NEX_GAME_ID, NEX_ACCESS_KEY, INDEPENDENT_HASH, D_LOGIN_TIMEOUT , INVALID_PRINCIPALID , !m_isSkipParentalLock ) )
  //if( !m_nexMgr->LoginGameServerAsync( NEX_GAME_ID , NEX_ACCESS_KEY , D_LOGIN_TIMEOUT ) )
  {
    GFL_PRINT( "LoginGameServer failed\n" );
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   ゲームサーバーからログアウトする
 */
//------------------------------------------------------------------
bool WifiConnectRunner::LogoutGameServer( void )
{
  //m_errorMgr->ResetNexErrorManager(m_nexMgr);

  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"NgsFacade::Logout() \n");
  #endif

  m_isGameServerLogoutFinish = false;
  GFL_PRINT("WifiConnectRunner::LogoutGameServer\n");
  if( !m_nexMgr->LogoutGameServerAsync() )
  {
    GFL_PRINT( "LogoutGameServer failed\n" );
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   Subscriptionの開始
 */
//------------------------------------------------------------------
void WifiConnectRunner::StartSubscription( void )
{
  if( m_subscrptionMgr->Initialize( m_nexMgr ) )
  {
    GFL_PRINT( "StartSubscription success\n" );
  }
  else
  {
    GFL_PRINT( "StartSubscription failed\n" );
  }
}


//------------------------------------------------------------------
/**
 * @brief   Subscriptionの終了
 *
 * @param   isAsync 終了処理を非同期で実行するならtrueを指定
 *
 * @return  終了処理が完了したならtrueを返却
 */
//------------------------------------------------------------------
bool WifiConnectRunner::EndSubscription( bool isAsync )
{
  //@fix [#5694 ローカルアトラクション参加中にハング]
  if( m_subscrptionMgr->CancelMessagingConnecting() )
  {//キャンセル処理中
    return false;
  }

  if( isAsync && m_subscrptionMgr->IsConnecting() )
  {
    m_subscrptionMgr->CancelConnecting();
    return false; // 処理中の通信が存在する
  }
  m_subscrptionMgr->Finalize();
  GFL_PRINT( "EndSubscription success\n" );

  return true;
}


//------------------------------------------------------------------
/**
 * @brief   接続処理
 */
//------------------------------------------------------------------
void WifiConnectRunner::Connect()
{
  bool isError = false;

  switch( m_step )
  {
    // Wifi接続
    case 0:
      isError = StartupWifiSystem();
      break;
    case 1:
      if( !m_isWifiConnectFinish ){
        return;
      }
      //m_errorMgr->SetWifiDisconnectListener(m_wifiSystem);
      m_wifiSystem->DeleteAsync();
      break;

    // Socketの確保
    case 2:
      isError = StartupSocketSystem();
      break;

    // フレンドサーバログイン
    case 3:
      isError = StartupFriendService();
      break;
    case 4:
      if( !m_isFriendLoginFinish ){
        return;
      }
      m_friendMgr->DeleteAsync();
      isError = StoreFriendList();
      break;

    // NEX開始
    case 5:
      isError = StartupNexService();
      break;

    // ゲームサーバログイン
    case 6:
      isError = LoginGameServer();
      break;
    case 7:
      if( !m_isGameServerLoginFinish ){
        return;
      }
      //m_errorMgr->SetNexErrorManager(m_nexMgr);
      break;

    // Subscription
    case 8:
      StartSubscription();
      break;

    // Http
    case 9:
      isError = StartupHttpSystem();
      break;

    // UtilityClient
    case 10:
      isError = BindUtilityClient();
      break;

    // CreateNexUniqueId
    case 11:
      if( E_MODE_CONNECT_AND_CREATE_NEXID == m_execMode ){
        isError = AcquireNexUniqueId();
      }
      break;
    case 12:
      if( (E_MODE_CONNECT_AND_CREATE_NEXID == m_execMode) && (m_nexUniqueId==0) ){
        return;
      }
      UnbindUtilityClient(); //不要になったので終了させる
      break;

    default:
      m_isFinish = true;
      return;
  }

  if( isError ){
    m_isError = true;
  }

  m_step++;
}


//------------------------------------------------------------------
/**
 * @brief   切断処理
 *
 * @param isAsync 通信を非同期で実行するならtrueを指定
 */
//------------------------------------------------------------------
void WifiConnectRunner::Disconnect( bool isAsync )
{
  switch( m_step )
  {
    // UtilityClient
    case 0:
      UnbindUtilityClient();
      break;

    case 1:
      if( !EndSubscription( true ) ){
        return;
      }
      break;

    // NEX通信キャンセル
    case 2:
      if( m_nexMgr->IsConnecting() ){
        m_nexMgr->CancelConnecting();
        return;
      }
      break;

    // ゲームサーバログアウト
    case 3:
      if( LogoutGameServer() ){
        m_isGameServerLogoutFinish = true; // エラー時はログアウト終了扱いとする
        GFL_PRINT("Logout Failed. Next Terminate.\n");
      }
      break;
    case 4:
      if( m_isGameServerLogoutFinish == false || m_nexMgr->IsGameServerConnecting() ){
        return;
      }
      break;
    // ゲームサーバの終了処理
    case 5:
      TerminateGameServer();
      break;
    case 6:
      if(!m_isGameServerTerminateFinish){
        return;
      }
      break;
    // NEX終了
    case 7:
      EndNexService();
      break;

    // フレンドサーバログアウト
    case 8:
      EndFriendService();
      break;

    case 9:
      EndHttpSystem();
      break;

    case 10:
      EndSocketSystem();
      break;

    // WIFI切断
    case 11:
      if( EndWifiSystem( isAsync ) == true )
      {//すでに切断済み
        m_isWifiDisconnectFinish = true;
      }
      break;
    case 12:
      if( !m_isWifiDisconnectFinish ){
        return;
      }
      break;

    default:
      m_isFinish = true;
      return;
  }
  m_step++;
}


//------------------------------------------------------------------
/**
 * @brief   終了判定
 * @return  セクションを終了させるならtrueを返却
 */
//------------------------------------------------------------------
bool WifiConnectRunner::IsFinished( void ) const
{
  return m_isFinish;
}


//------------------------------------------------------------------
/**
* @brief   フレンドサーバへのログイン通知ハンドラ
*
* @note    イベントはスレッド内からコールされるため
* @note    リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnFriendServerLoginFinish( const nn::Result &result )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( result.IsFailure() )
  {
    if( m_isSkipParentalLock && result == nn::friends::ResultP2pInternetProhibited() )
    {//ペアレンタルを無視するばあい
      m_isFriendLoginFinish = true;
    }
    else
    {
      m_errorCode = gflnet2::friends::FriendManager::GetNetworkErrorCode( result );
      m_isError   = true;
    }
  }
  else
  {
    m_isFriendLoginFinish = true;
  }
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバのログイン成功通知
*
* @param credential ログイン時に発行された認証情報
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnGameServerLoginSuccess( nn::nex::Credentials* credential )
{
  GFL_PRINT( "OnGameServerLoginSuccess()\n" );
#if PM_DEBUG
  u8 MacAddress[nn::socket::ETH_ALEN];
  nn::socket::GetMacAddress(MacAddress);
  NN_LOG( "MAC ADDRESS %x-%x-%x-%x-%x-%x\n",MacAddress[0],MacAddress[1],MacAddress[2],MacAddress[3],MacAddress[4],MacAddress[5] );
#endif
  m_isGameServerLoginFinish = true;
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバのログイン失敗通知
*
* @param loginFuncResult nn::nex::NgsFacade::Login()の実行結果
* @param lastLoginErrorCode nn::nex::NgsFacade::GetLastLoginErrorCode()の値
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnGameServerLoginFailed( const nn::nex::qResult& loginFuncResult, qInt32 lastLoginErrorCode )
{
  GFL_PRINT( "OnGameServerLoginFailed()[%d]\n",lastLoginErrorCode );
  m_isError   = true;
  m_errorCode = lastLoginErrorCode;
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバからのログアウト成功通知
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnGameServerLogoutSuccess( void )
{
  GFL_PRINT( "OnGameServerLogoutSuccess()\n" );
  m_isGameServerLogoutFinish = true;
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバのログアウト失敗通知
*
* @param logoutFuncResult nn::nex::NgsFacade::Logout()の実行結果
* @param errorCode        logoutFuncResultからエラーコードに変換した値
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnGameServerLogoutFailed( const nn::nex::qResult& logoutFuncResult, qInt32 errorCode )
{
  GFL_PRINT( "OnGameServerLogoutFailed()\n" );
  m_isError   = true;
  m_errorCode = errorCode;
}


//------------------------------------------------------------------
/**
* @brief WIFI接続の成功通知
*
* @note  イベントはスレッド内からコールされるため
* @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
*
* @note  イベントはスレッド内からコールされるため
* @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnWifiConnectSuccess( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  GFL_PRINT( "OnWifiConnectSuccess()\n" );
  m_isWifiConnectFinish = true;
  //netapp::wifi::WifiAccessPointRecorder::RecordAccessPoint();//アクセスポイント接続履歴保存
}


//------------------------------------------------------------------
/**
* @brief WIFI接続の失敗通知
*
* @note  イベントはスレッド内からコールされるため
* @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
*
* @param result    実行結果
* @param errorCode 発生した通信エラーコード
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnWifiConnectFailed( const nn::Result& result, u32 errorCode )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  GFL_PRINT( "OnWifiConnectFailed()\n" );
  m_isError   = true;
  m_errorCode = errorCode;
  //netapp::wifi::WifiAccessPointRecorder::RecordAccessPointConnectFailed();//アクセスポイント接続失敗通知
}


//------------------------------------------------------------------
/**
* @brief WIFI切断の成功通知
*
* @note  イベントはスレッド内からコールされるため
* @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnWifiDisconnectSuccess( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  GFL_PRINT( "OnWifiDisconnectSuccess()\n" );
  m_isWifiDisconnectFinish = true;
}


//------------------------------------------------------------------
/**
* @brief WIFI切断の失敗通知
*
* @note  イベントはスレッド内からコールされるため
* @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
*
* @param result    実行結果
* @param errorCode 発生した通信エラーコード
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnWifiDisconnecFailed( const nn::Result& result, u32 errorCode )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  GFL_PRINT( "OnWifiDisconnecFailed()\n" );
  // 本来はエラーだが、正常終了としてふるまう
  m_isWifiDisconnectFinish = true;
}


//------------------------------------------------------------------
/**
* @brief NEXユニークID発行の成功通知
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnAcquireNexUniqueIdSuccess( nn::nex::UniqueId uniqueId )
{
  m_nexUniqueId = uniqueId;
  GFL_PRINT( "OnAcquireNexUniqueIdSuccess(%llu)\n", uniqueId );
}


//------------------------------------------------------------------
/**
* @brief NEXユニークID発行の失敗通知
*
* @param[in] result     NEXユニークID発行処理の実行結果
* @param[in] errorCode  resultからエラーコードに変換した値
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnAcquireNexUniqueIdFailed( const nn::nex::qResult& result, qInt32 errorCode )
{
  GFL_PRINT( "OnAcquireNexUniqueIdFailed( %u )\n", errorCode );
  m_isError   = true;
  m_errorCode = errorCode;
}


//------------------------------------------------------------------
/**
 * @brief スリープ遷移用WIFI切断処理
 *
 * @note 同期通信にてWIFI切断を実行する
 *
 * @return WIFIを切断したならtrueを返却する
 */
//------------------------------------------------------------------
bool ForceDisconnectWifi( void )
{
  GFL_PRINT("WifiConnectRunner::ForceDisconnectWifi\n");
  gflnet2::nex::NexManager*  nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);

  //if( nexMgr->IsGameServerConnecting() )
  {
    GFL_PRINT("WifiConnectRunner::ForceDisconnectWifi Exec\n");
    gfl2::heap::HeapBase* heap   = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
    WifiConnectRunner*   runner = GFL_NEW_LOW(heap) WifiConnectRunner( WifiConnectRunner::E_MODE_DISCONNECT_SYNC );

    while(true)
    {
      nexMgr->Dispatch(50);
      WifiConnectRunner::E_RET_CODE ret = runner->Update();

      if( WifiConnectRunner::E_RET_CONTINUE != ret ){
        break;
      }
    }
    runner->Finalize();

    GFL_DELETE( runner );

    // 既存ユーザー全員をオフラインに設定
    NetAppLib::JoinFesta::JoinFestaPacketUtil::SetOfflineJoinFestaPersonalAll();

    return true;
  }

  //return false;
}






//------------------------------------------------------------------
/**
 * @brief   ゲームサーバーの終了処理
 */
//------------------------------------------------------------------
void WifiConnectRunner::TerminateGameServer( void )
{
  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"NgsFacade::Terminate() \n");
  #endif

  GFL_PRINT( "Start TerminateGameServer()\n" );

  if( m_nexMgr->GetNgsFacade() == NULL ){
    m_isGameServerTerminateFinish = true;
  }
  else
  {
    m_isGameServerTerminateFinish = false;
    m_nexMgr->TerminateFacade();
  }
}


//------------------------------------------------------------------
/**
* @brief ゲームサーバの終了処理成功通知
*/
//------------------------------------------------------------------
// @override
void WifiConnectRunner::OnGameServerTerminated( void )
{
  GFL_PRINT( "OnGameServerTerminated()\n" );
  m_isGameServerTerminateFinish = true;
}



GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)

#endif
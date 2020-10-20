//==============================================================================
/**
 * @file	P2pConnectionManager.cpp
 * @brief	P2P通信接続マネージャー
 * @author	ichiraku_katsuhiko
 * @date	2015/04/01 19:54:01
 */
// =============================================================================

// HEAPID
#include <heap/include/gfl2_heap_manager.h>
// NetGame
#include <gflnet2/source/p2p/gflnet2_NetGameBase.h>
// NexManager
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>
// List
#include <util/include/gfl2_List.h>
#include "system/include/HeapDefine.h"
#include "system/include/PokemonVersion.h"
#include "system/include/PokemonLanguage.h"
#include "GameSys/include/GameManager.h"

// P2pConnectionErrorListener
#include "NetStatic/NetLib/include/P2P/P2pConnectionErrorListener.h"
// P2pConnectionMigrationListener
#include "NetStatic/NetLib/include/P2P/P2pConnectionMigrationListener.h"
// ErrorManager
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
// BeaconManager
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>

// mine
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
// NetworkSystem
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include "System/include/HomeSleepAndPowerSystem.h"

#include  <debug/include/gfl2_DebugLogWindow.h>

SINGLETON_INSTANCE_DEFINITION(NetLib::P2P::P2pConnectionManager)
template class gfl2::base::SingletonAccessor<NetLib::P2P::P2pConnectionManager>;

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)


// デバッグ機能
#if PM_DEBUG
bool P2pConnectionManager::m_debugConnectError = false; //! 必ず切断失敗フラグ
#endif // PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
P2pConnectionManager::P2pConnectionManager() :
  m_SysHeap(NULL)
  ,m_DevHeap(NULL)
  ,m_seq(E_SEQ_NONE)
  ,m_P2pManager(NULL)
  ,m_BeaconManager(NULL)
  ,m_NetGame(NULL)
  ,m_isInitialize(false)
  ,m_isConnectStartCall(false)
  ,m_p2pId(NIJI_P2P_MAX)
#ifdef  GF_PLATFORM_CTR
  ,m_NexRatingClient(NULL)
  ,m_ratingSessionKeySize(0)
  ,m_ratingSessionKey(NULL)
  ,m_isRatingListenerSuccess(false)
  ,m_ratingSeq(0)
#endif // GF_PLATFORM_CTR 
  ,m_pP2pCommonData(NULL)
  ,m_errorListener(NULL)
  ,m_migrationListener(NULL)
  ,m_disconnectTimingTimeOut()
  //m_localP2PMasterBeaconBuffer[BEACON_BUFFER_SIZE];
  ,m_localP2PMasterBeaconSize(0)
  ,m_isMatchmakingCreateSuccess(false)
  //@fix #5671   「ミラクル交換」交換同期セーブ中に通信切断し電源ボタンを押すとハマリおよび致命的エラー発生する
  ,m_isForceDisconnectSuccessFlag(false)
{
#ifdef  GF_PLATFORM_CTR
  gfl2::std::MemClear( m_ratingSessionKeyBuffer, sizeof(m_ratingSessionKeyBuffer) );
#endif // GF_PLATFORM_CTR 
  m_timeout.RegistErrorListener( this );
  ::std::memset( m_localP2PMasterBeaconBuffer , 0 , sizeof(m_localP2PMasterBeaconBuffer) );
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
P2pConnectionManager::~P2pConnectionManager()
{
  m_timeout.UnregistErrorListener();
#ifdef  GF_PLATFORM_CTR
  qDelete m_ratingSessionKey;
  GFL_SAFE_DELETE( m_NexRatingClient );
#endif // GF_PLATFORM_CTR 

  GFL_SAFE_DELETE( m_pP2pCommonData );
  GFL_SAFE_DELETE( m_NetGame );
  GFL_SAFE_DELETE( m_P2pManager );
#ifdef  GF_PLATFORM_CTR
  GFL_SAFE_DELETE( m_BeaconManager );
#endif // GF_PLATFORM_CTR 
}

//------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------
void P2pConnectionManager::Initialize( ENUM_NIJI_P2P_ID p2pId )
{
  if( m_isInitialize == false )
  {
    // ヒープ
    if( p2pId == NIJI_P2P_JF_QUICK_2 || p2pId == NIJI_P2P_BATTLE_TREE_2 )
    {
      m_SysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
      m_DevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
    }else{
      m_SysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
      m_DevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    }

    gflnet2::EN_NET_TYPE netType; // ネットワーク種別
    u8  connectMax; // 最大接続人数

    // 通信サービス別初期化
    switch( p2pId){
    case NIJI_P2P_JF_QUICK_2:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
    case NIJI_P2P_JF_QUICK_4:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
    case NIJI_P2P_BATTLE_2_L:    ///< バトル（2人接続）（ローカル）
    case NIJI_P2P_BATTLE_4_L:    ///< バトル（4人接続）（ローカル）
    case NIJI_P2P_TRADE_L:    ///< 交換（ローカル)
    case NIJI_P2P_BATTLE_TREE_2:  ///< バトルハウス（マルチ）
    case NIJI_P2P_BATTLE_TREE_4:  ///< バトルハウス（バトルロイヤル）
      netType = gflnet2::TYPE_P2P_WIRELESS;
      break;
    case NIJI_P2P_BATTLE_2_W:    ///< バトル（2人接続）（WiFi）
    case NIJI_P2P_BATTLE_4_W:    ///< バトル（4人接続）（WiFi）
    case NIJI_P2P_TRADE_W:    ///< 交換（WiFi）
    case NIJI_P2P_RANDOM_TRADE:    ///< ランダム交換
    case NIJI_P2P_RNDRATE_S:    ///< レーティング:シングル
    case NIJI_P2P_RNDRATE_D:    ///< レーティング:ダブル
    case NIJI_P2P_RNDRATE_SP:    ///< レーティング:スペシャル
    case NIJI_P2P_RNDRATE_WCS:  ///<  レーティング：ＷＣＳ
    case NIJI_P2P_RNDFREE_S:    ///< フリー：シングル
    case NIJI_P2P_RNDFREE_D:    ///< フリー：ダブル
    case NIJI_P2P_RNDFREE_B:    ///< フリー：バトルロイヤル
    case NIJI_P2P_RNDFREE_S_LEGEND:    ///< フリー：シングル(伝説あり)
    case NIJI_P2P_RNDFREE_D_LEGEND:    ///< フリー：ダブル(伝説あり)
    case NIJI_P2P_RNDFREE_B_LEGEND:    ///< フリー：バトルロイヤル(伝説あり)
    case NIJI_P2P_WIFI_CUP:    ///< インターネット大会
    case NIJI_P2P_GROUP_CUP:    ///< 仲間大会
      netType = gflnet2::TYPE_P2P_WIFI;
      break;
    case NIJI_P2P_LIVE_CUP:    ///< LIVE大会
    case NIJI_P2P_GIFT:        ///< ふしぎなおくりもの
      netType = gflnet2::TYPE_P2P_IR;
      break;
    default:
      GFL_ASSERT(0);
      netType = gflnet2::TYPE_P2P_WIRELESS;
      break;
    }

    // 最大接続人数
    switch( p2pId){
    case NIJI_P2P_JF_QUICK_4:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
    case NIJI_P2P_BATTLE_4_L:    ///< バトル（4人接続）（ローカル）
    case NIJI_P2P_BATTLE_TREE_4:  ///< バトルハウス（バトルロイヤル）
    case NIJI_P2P_BATTLE_4_W:    ///< バトル（4人接続）（WiFi）
    case NIJI_P2P_RNDFREE_B:    ///< フリー：バトルロイヤル
    case NIJI_P2P_RNDFREE_B_LEGEND: ///< フリー：バトルロイヤル(伝説あり)
      connectMax = gflnet2::p2p::CONNECT_MAX_FOUR;
      break;
    default:
      connectMax = gflnet2::p2p::CONNECT_MAX_TWO;
      break;
    }

#ifdef  GF_PLATFORM_CTR
    bit64 TransferedID = nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID); // 自分のTransferdID
#else
    bit64 TransferedID = 0;
#endif

#if PM_DEBUG
    // デバッグで繋がらないようにする
    if( m_debugConnectError )
    {
      TransferedID = 0;
    }
#endif

    //-------------------------------------
    /// 通信初期化パラメータ（アプリ固有）
    //=====================================
    gflnet2::InitParameter netInitParam = {
      TransferedID, // 自分のTransferdID
      m_SysHeap,// システムメモリ
      m_DevHeap,// デバイスメモリ
      netType,  // ネットワーク種別
      p2pId,    // 通信別のID
      connectMax,// 最大接続人数
      true,//マイグレーション有効無効(常に有効にする。マイグレーションの発生をエラーとするかどうかは、モードによって判断する)
    };


    //-------------------------------------
    /// 通信初期化パラメータ（アプリ共通）
    //=====================================
    gflnet2::InitParameterFixed netInitParamFixed;
    netInitParamFixed.ProductID = NIJI_PRODUCT_CODE;    ///< 任天堂からソフトごとに割り当ててくれるID
    netInitParamFixed.LangCode  = System::GetLang();    ///< 言語番号
    netInitParamFixed.PMVer     = System::GetVersion(); ///< ＲＯＭバージョン番号
    netInitParamFixed.Version   = NIJI_NET_VERSION;     ///< 通信バージョン

    // ビーコンマネージャが必要なので作る
#ifdef  GF_PLATFORM_CTR
    m_BeaconManager = GFL_NEW( m_DevHeap ) gflnet2::base::BeaconManager();
#endif // GF_PLATFORM_CTR 

    // P2P管理クラス
    m_P2pManager = GFL_NEW( m_DevHeap ) gflnet2::p2p::P2pManager( m_SysHeap, m_DevHeap );
    m_P2pManager->Initialize( &netInitParam, &netInitParamFixed );
    m_P2pManager->RegistListener( this );
    m_isMatchmakingCreateSuccess = false;

    // 通信サービス種類
    m_p2pId = p2pId;

    // ミラクル交換専用設定
    if( m_p2pId == NIJI_P2P_RANDOM_TRADE )
    {
      m_P2pManager->UseSimpleMatchmake();
    }

    // RatingClient作成
    if( IsRating() )
    {
      RatingClientCreate();
    }

#if PM_DEBUG
    // 通信混線防止策
    m_P2pManager->GetInitParam()->SetDebugMatchmakeNo( NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_VERSION );
#endif

    // 初期化完了
    m_isInitialize = true;

    //@fix #5671   「ミラクル交換」交換同期セーブ中に通信切断し電源ボタンを押すとハマリおよび致命的エラー発生する
    m_isForceDisconnectSuccessFlag = false;

    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_P2P_ACC );
  }
}

//------------------------------------------------------------------
/**
  * @brief   ビーコン設定（ローカルの親のみ）
  */
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
void P2pConnectionManager::SetLocalP2PMasterBeacon( const void* pBeaconData , u32 beaconSize )
{
  if( beaconSize <= sizeof(m_localP2PMasterBeaconBuffer) )
  {
    ::std::memset( m_localP2PMasterBeaconBuffer , 0 , sizeof(m_localP2PMasterBeaconBuffer) );
    ::std::memcpy( m_localP2PMasterBeaconBuffer , pBeaconData , beaconSize );
    m_localP2PMasterBeaconSize = beaconSize;
  }
  else
  {
    GFL_ASSERT( 0 );//@fix
  }
}
#endif // GF_PLATFORM_CTR 

//------------------------------------------------------------------
/**
 * @brief   接続開始
 *
 * @param   connectType     接続種類（自動、親、子）
 * @param   transferdIdList 子の場合のみ親のTransferdIDリスト（親、自動なら0）
 * @return  trueで開始成功
 */
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
bool P2pConnectionManager::ConnectStart( const gflnet2::p2p::CONNECT_TYPE connectType, bit64 transferdId )
{
  if( m_isInitialize == false )
  {
    GFL_ASSERT_MSG(0,"not Initialize\n");
    return false;
  }
  if( m_isConnectStartCall == true )
  {
    GFL_ASSERT_MSG(0,"already Call\n");
    return false;
  }
  // 初期状態か、切断完了していないと接続は出来ない
  if( !(m_seq == E_SEQ_NONE || m_seq == E_SEQ_DISCONNECT_LOOP) )
  {
    GFL_ASSERT_MSG(0,"SEQ Error\n");
    return false;
  }

  // リストに一旦格納
  gfl2::util::List<bit64> transferdIdList( m_SysHeap, 1 );
  transferdIdList.PushBack( transferdId );

  // 接続開始
  m_P2pManager->SetLocalP2PMasterBeacon( m_localP2PMasterBeaconBuffer , m_localP2PMasterBeaconSize , NIJI_BEACON_TYPE_ID_JF , 0 );
  bool ret = m_P2pManager->ConnectStart( connectType, &transferdIdList, m_BeaconManager );

  // バッファ削除
  transferdIdList.Clear();

  // 送受信管理クラス（ないとNetGameBaseがないので、P2PManagerが動かない）
  // 注：BaseのNetGameになるので初期化終了待ちは存在しない。
  m_NetGame = GFL_NEW( m_DevHeap ) gflnet2::p2p::NetGame( m_P2pManager->GetInitParam() );
  m_NetGame->Initialize();

  // NetGameの親子フラグ
  switch( connectType ){
  case gflnet2::p2p::CHILD:   gflnet2::p2p::NetGame::GetNetGameBase()->SetMaster( false ); break;
  case gflnet2::p2p::PARENT:  gflnet2::p2p::NetGame::GetNetGameBase()->SetMaster( true ); break;
  }

  // 呼び出し成功した場合に接続待ちシーケンスに進める
  if( ret == true )
  {
    // 赤外線接続以外はタイムアウトが必要
    // クイックマッチ、ミラクル交換もタイムアウトいらない
    if( !m_P2pManager->GetInitParam()->IsIr() && 
         m_P2pManager->GetInitParam()->GetGameID() != NIJI_P2P_JF_QUICK_2 && 
         m_P2pManager->GetInitParam()->GetGameID() != NIJI_P2P_JF_QUICK_4 &&
         m_P2pManager->GetInitParam()->GetGameID() != NIJI_P2P_BATTLE_TREE_2 )
    {
      m_timeout.Start(); // タイムアウトカウント開始
    }
    SetSequence( E_SEQ_CONNECT_WAIT );
    m_isConnectStartCall = true;
  }

  GFL_PRINT("P2pConnectionManager::ConnectStart[%d][%d]\n",m_seq,ret);

  return ret;
}
#endif // GF_PLATFORM_CTR 

//------------------------------------------------------------------
/**
 * @brief   接続開始(Wifi接続時のランダムマッチング専用）
 *
 * @param   connectType     接続種類（自動、親、子）
 * @param   ruleLest        マッチング条件リスト
 * @return  trueで開MAIN始成功
 */
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
bool P2pConnectionManager::ConnectStartWifi( const gflnet2::p2p::CONNECT_TYPE connectType, gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList* ruleList )
{
  if( m_isInitialize == false )
  {
    GFL_ASSERT_MSG(0,"not Initialize\n");
    return false;
  }
  if( m_isConnectStartCall == true )
  {
    GFL_ASSERT_MSG(0,"already Call\n");
    return false;
  }
  // 初期状態か、切断完了していないと接続は出来ない
  if( !(m_seq == E_SEQ_NONE || m_seq == E_SEQ_DISCONNECT_LOOP) )
  {
    GFL_ASSERT_MSG(0,"SEQ Error\n");
    return false;
  }

  // 接続開始
  bool ret = m_P2pManager->ConnectStartWifi( connectType, ruleList );

  // 送受信管理クラス（ないとNetGameBaseがないので、P2PManagerが動かない）
  // 注：BaseのNetGameになるので初期化終了待ちは存在しない。
  m_NetGame = GFL_NEW( m_DevHeap ) gflnet2::p2p::NetGame( m_P2pManager->GetInitParam() );
  m_NetGame->Initialize();

  // 呼び出し成功した場合に接続待ちシーケンスに進める
  if( ret == true )
  {
    // ミラクル交換はタイムアウトいらない
    if( m_P2pManager->GetInitParam()->GetGameID() != NIJI_P2P_RANDOM_TRADE &&
        m_P2pManager->GetInitParam()->GetGameID() != NIJI_P2P_WIFI_CUP &&
        m_P2pManager->GetInitParam()->GetGameID() != NIJI_P2P_GROUP_CUP &&
        GetCommRouteTypeForP2pID() != JoinFestaScript::COMM_ROUTE_RANDOM_BATTLE )
    {
      m_timeout.Start(); // タイムアウトカウント開始
    }
    SetSequence( E_SEQ_CONNECT_WAIT );
    m_isConnectStartCall = true;
    m_isMatchmakingCreateSuccess = false;
  }

  return ret;
}
#endif // GF_PLATFORM_CTR 


//------------------------------------------------------------------
/**
 * @brief   接続成功取得
 * @return  trueで接続成功している
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsConnectSuccess()
{
  return ( m_seq == E_SEQ_CONNECT_LOOP );
}

//------------------------------------------------------------------
/**
 * @brief   切断開始
 * @param   isSkipDisconnectTiming     切断同期をスキップするか（デフォルトfalse）
 */
//------------------------------------------------------------------
void P2pConnectionManager::DisconnectStart( bool isSkipDisconnectTiming )
{
  // 既に切断処理中・切断完了していたら何もしない
  if( E_SEQ_DISCONNECT_TIMING <= m_seq && m_seq <= E_SEQ_DISCONNECT_LOOP )
  {
    //GFL_PRINT("切断多重呼び出し \n");
    return;
  }

  // 接続開始を呼んでいない → 即時切断で終了
  if( m_isConnectStartCall == false )
  {
    //ビーコン再開
    SetSequence( E_SEQ_DISCONNECT_LOOP );
    return;
  }

  // 通信エラーが発生している　→　切断前同期をスキップして切断処理
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() ||
      isSkipDisconnectTiming )//スキップフラグ設定時も同期スキップ
  {
    SetSequence( E_SEQ_DISCONNECT_EXE );
    GFL_PRINT("!!!P2P Force Disconnect\n");

#ifdef  GF_PLATFORM_CTR
    // @fix GFNMCat[100] 通信エラーでの切断ではKickMeを呼ばないとIsCheckGameOverがtrueにならない
    if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
    {
      nn::nex::Station* pStation = gflnet2::dev::NetZAdapter::GetStationInstance();
      if( pStation != NULL && m_P2pManager != NULL )
      {
        if( m_P2pManager->GetInitParam()->IsMigrationPowerOff() == false )
        {
          pStation->KickMe();
          GFL_PRINT("!!!KickMe !!!\n");
        }
      }
    }
#endif // GF_PLATFORM_CTR 

  }
  else
  {
    // 接続成功している　→　フルの切断処理
    // 切断成功していない → 切断前同期をスキップして切断処理
    if( IsConnectSuccess() )
    {
      SetSequence( E_SEQ_DISCONNECT_TIMING );
    }
    else
    {
      SetSequence( E_SEQ_DISCONNECT_EXE );
    }
  }
  m_isConnectStartCall = false;

  //タイムアウト管理を止める
  m_timeout.Stop();

  // エラーダイアログは抑制する
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->P2pDisconnectSpanOn();

  GFL_PRINT("P2pConnectionManager::DisconnectStart[%d]\n",m_seq);
}

//------------------------------------------------------------------
/**
 * @brief   切断成功取得
 * @return  trueで切断成功している
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsDisconnectSuccess()
{
  //@fix #5671   「ミラクル交換」交換同期セーブ中に通信切断し電源ボタンを押すとハマリおよび致命的エラー発生する
  if( m_isForceDisconnectSuccessFlag == true )
  {//E_SEQ_NONEのとき、強制的に成功にする.E_SEQ_NONEになっている状態は、インスタンス生成時かTerminate後のみ
    return true;
  }

  return ( m_seq == E_SEQ_DISCONNECT_LOOP );
}


//------------------------------------------------------------------
/**
 * @brief   終了
 */
//------------------------------------------------------------------
void P2pConnectionManager::Terminate()
{
  if( m_isInitialize == true )
  {
    // RatingClient破棄
    if( IsRating() )
    {
      RatingClientDelete();
    }
    GFL_SAFE_DELETE( m_P2pManager );
#ifdef  GF_PLATFORM_CTR
    GFL_SAFE_DELETE( m_BeaconManager );
#endif // GF_PLATFORM_CTR 

    m_SysHeap = NULL;
    m_DevHeap = NULL;
    SetSequence( E_SEQ_NONE );
    m_isInitialize = false;

    System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_P2P_ACC );
  }
}


//------------------------------------------------------------------
/**
 * @brief   同期開始
 * @param   timingUniqueNo    同期番号 0以外
 * @param   timeou            タイムアウト有り無し  trueであり
 */
//------------------------------------------------------------------
void P2pConnectionManager::TimingStart( int timingUniqueNo, bool timeout )
{
  if( m_NetGame != NULL )
  {
    m_NetGame->TimingStart( timingUniqueNo );
  }

  if( timeout )
  {
    // タイムアウトカウント開始
    m_timeout.Start();
  }
}

//------------------------------------------------------------------
/**
 * @brief   同期完了チェック
 * @param   timingUniqueNo    同期番号 0以外
 * @return  trueで完了
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsTimingEnd( int timingUniqueNo )
{
  if( m_NetGame == NULL )
  {
    return false;
  }
  bool ret = m_NetGame->IsTimingEnd( timingUniqueNo );

  if( ret )
  {
    m_timeout.Stop();
    return true;
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief   接続相手のMyStatus取得（P2P接続成功後に取得可能）
 * @param[in] netId   通信ID（NetGame::GetNetID)
 * @return  MyStatus
 */
//------------------------------------------------------------------
Savedata::MyStatus* P2pConnectionManager::GetMyStatus( const int netId ) const
{
  if( m_pP2pCommonData != NULL )
  {
    return m_pP2pCommonData->GetMyStatus( netId );
  }
  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief PokePartyデータ送受信開始
 * @param[in] pPokeParty    ポケパーティ
 * @param[in] pCerdData     署名データ（必要ないなら省略可）
 * @return  送信準備完了したらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pConnectionManager::SendPokeParty( const pml::PokeParty* pPokeParty, const void* pCerdData )
{
  if( m_pP2pCommonData != NULL )
  {
    return m_pP2pCommonData->SendPokeParty( pPokeParty, pCerdData );
  }
  GFL_ASSERT(0);
  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief PokeParty送受信終了したか
 * @return 終了していたらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pConnectionManager::IsRecvPokePartyEnd()
{
  if( m_pP2pCommonData != NULL )
  {
    m_pP2pCommonData->Update();
    return m_pP2pCommonData->IsRecvEnd();
  }
  GFL_ASSERT(0);
  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ポケモン改ざんチェックの結果取得
 * @param[in] netId   通信ID（NetGame::GetNetID)
 * @return  改ざんが行われていないならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pConnectionManager::IsValidationCheckOk( const int netId )
{
  if( m_pP2pCommonData != NULL )
  {
    return m_pP2pCommonData->IsValidationCheckOk( netId );
  }
  GFL_ASSERT(0);
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   接続相手のPokeParty取得（IsRecvPokePartyEnd成功後に取得可能）
 * @param[in] netId   通信ID（NetGame::GetNetID)
 * @return  PokeParty
 */
/* ----------------------------------------------------------------------------------------- */
pml::PokeParty* P2pConnectionManager::GetPokeParty( const int netId ) const
{
  if( m_pP2pCommonData != NULL )
  {
    return m_pP2pCommonData->GetPokeParty( netId );
  }
  return NULL;
}

//------------------------------------------------------------------
/**
  * @brief   SetThreadPriority設定
  * @param   SetThreadPriority    
  */
//------------------------------------------------------------------
void P2pConnectionManager::SetThreadPriority( int threadPriority )
{
  if( m_P2pManager )
  {
    m_P2pManager->SetThreadPriority( threadPriority );
  }
}

//------------------------------------------------------------------
/**
 * @brief   接続順番 = NetIDをかえす
 * @return  NetID
 */
//------------------------------------------------------------------
int P2pConnectionManager::GetNetID() const
{
  if( m_NetGame != NULL )
  {
    return m_NetGame->GetNetID();
  }
  return 0;
}

 //------------------------------------------------------------------
/**
 * @brief   インターネット接続かどうか
 * @return  trueでインターネット接続
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsWifi() const
{
  GFL_ASSERT(m_P2pManager);
  return m_P2pManager->GetInitParam()->IsWifi();
}

//------------------------------------------------------------------
/**
 * @brief   赤外線接続かどうか
 * @return  trueで赤外線接続
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsIr() const
{
  GFL_ASSERT(m_P2pManager);
  return m_P2pManager->GetInitParam()->IsIr();
}

//------------------------------------------------------------------
/**
  * @brief   マイグレーション時電源OFF切断設定
  */
//------------------------------------------------------------------
void P2pConnectionManager::SetupMigrationPowerOff()
{ 
  if( m_migrationListener )
  { 
    if( m_P2pManager )
    {
      m_P2pManager->GetInitParam()->SetMigrationPowerOff( true ); 
    }
  } 
}

//@fix #4127   インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】
//------------------------------------------------------------------
/**
  * @brief   マイグレーションコールバック有効化
  */
//------------------------------------------------------------------
void P2pConnectionManager::SetupMigrationCallBackEnable()
{
  if( m_P2pManager )
  {
    m_P2pManager->GetInitParam()->SetMigrationCallBackEnable( true ); 
  }
}

//------------------------------------------------------------------
/**
 * @brief   接続相手にnijiが混ざっているかどうか
 * @return  trueでnijiが混ざっている
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsNijiConnect()
{
  GFL_ASSERT( IsConnectSuccess() );

  for( u8 netId = 0; netId < gflnet2::p2p::CONNECT_MAX_FOUR; netId++ )
  {
    // 自分以外をチェック
    if( GetNetID() != netId )
    {
      Savedata::MyStatus* pMyStatus = GetMyStatus( netId );

      if( pMyStatus != NULL )
      {
        if( pMyStatus->GetRomCode() == VERSION_SUN ||
            pMyStatus->GetRomCode() == VERSION_MOON )
        {
          GFL_PRINT("接続相手にnijiが混ざっている!!!\n");
          return true;
        }
      }
    }
  }

  GFL_PRINT( "接続相手はmomijiのみ!!!\n" );
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------
void P2pConnectionManager::Update()
{
  if( m_P2pManager != NULL )
  {
    //ビーコンが動いていたら停止まで待つ
    if( NetLib::NijiNetworkSystem::UpdateStopBeaconSystem() == false )
    {
      return;
    }
    m_P2pManager->NetMain();
  }

  // タイムアウト更新
  m_timeout.Update();

  // シーケンス制御
  switch(m_seq){
  case E_SEQ_NONE:             ///< なし
    break;
  case E_SEQ_CONNECT_WAIT:     ///< P2P接続待ち
    {
      // ビーコンを収集
#ifdef  GF_PLATFORM_CTR
      m_BeaconManager->MainCheckRecvBeacon();
#endif // GF_PLATFORM_CTR 
      // リスナー待ち
    }
    break;
  case E_SEQ_RATING_INITIALIZE:///< レーティングクラス初期化
    {
      if( !IsRating() )
      {
        // レーティングセッションはSkip
        SetSequence( E_SEQ_CONNECT_TIMING );
      }
      else if( RatingClientInitialize() )
      {
        SetSequence( E_SEQ_RATING_SESSION );
      }
    }
    break;
  case E_SEQ_RATING_SESSION:   ///< レーティングセッション確立処理
    {
      if( RatingSessionSequence() )
      {
        SetSequence( E_SEQ_CONNECT_TIMING );
      }
    }
    break;
  case E_SEQ_CONNECT_TIMING:   ///< P2P接続完了同期
    {
      // 赤外線はタイムアウトなし
      const bool isTimeout = ( IsIr() == true ) ? false : true;
      TimingStart( CONNECT_TIMING_NO, isTimeout );
      SetSequence( E_SEQ_CONNECT_TIMING_WAIT );
    }
    break;
  case E_SEQ_CONNECT_TIMING_WAIT:///< P2P接続完了同期待ち
    {
      if( IsTimingEnd( CONNECT_TIMING_NO ) )
      {
        // @fix GFNMcat[1353] 受信バッファ空にするとこれまでに受信していて必要なデータまで消されてしまう。
        // 受信バッファは空にしておく
        //RecvAllToEmpty();
        SetSequence( E_SEQ_COMMON_DATA_CREATE );
      }
    }
    break;
  case E_SEQ_COMMON_DATA_CREATE:///< 共通のデータモジュール作成
    {
      m_pP2pCommonData = GFL_NEW( m_DevHeap ) P2pCommonData( m_DevHeap, m_NetGame, GetCommRouteTypeForP2pID() );
      SetSequence( E_SEQ_COMMON_DATA_START1 );
    }
    break;
  case E_SEQ_COMMON_DATA_START1:///< 共通のデータ送信
    {
      if( m_pP2pCommonData->SendMyStatus() )
      {
        SetSequence( E_SEQ_COMMON_DATA_WAIT1 );
      }
    }
    break;
  case E_SEQ_COMMON_DATA_WAIT1: ///< 共通のデータ受信待ち
    {
      m_pP2pCommonData->Update();
      if( m_pP2pCommonData->IsRecvEnd() )
      {
        SetSequence( E_SEQ_COMMON_DATA_START2 );
      }
    }
    break;
  case E_SEQ_COMMON_DATA_START2:///< 共通のデータ送信
    {
      if( m_pP2pCommonData->SendJoinFestaPersonalData() )
      {
        SetSequence( E_SEQ_COMMON_DATA_WAIT2 );
      }
    }
    break;
  case E_SEQ_COMMON_DATA_WAIT2: ///< 共通のデータ受信待ち
    {
      m_pP2pCommonData->Update();
      if( m_pP2pCommonData->IsRecvEnd() )
      {
        // @fix GFNMcat[1353] 受信バッファ空にするとこれまでに受信していて必要なデータまで消されてしまう。
        // 受信バッファは空にしておく
        //RecvAllToEmpty();
        SetSequence( E_SEQ_CONNECT_LOOP );

        // 通信の親表示
#if PM_DEBUG
#if defined( GF_PLATFORM_CTR )
        if( GetNetID() == 0 )
        {
          gfl2::debug::LogWindow_AddMsg( gfl2::debug::LOGWIN_ID_NEX, L"I'm P2P Master\n");
          GFL_RELEASE_PRINT( "I'm P2P Master\n" );
        }
#endif
#endif

      }
    }
    break;
  case E_SEQ_CONNECT_LOOP:     ///< P2P接続完了（空ループ）
    // 空ループ
    break;
  case E_SEQ_DISCONNECT_TIMING:///< P2P切断同期
    {
      // TimingStartはタイムアウトエラーを発生させるので使えない。自前でやる。
      DissconnetTimingStart();
      SetSequence( E_SEQ_DISCONNECT_TIMING_WAIT );
    }
    break;
  case E_SEQ_DISCONNECT_TIMING_WAIT:///< P2P切断同期待ち
    {
      // 同期終了かタイムアウト
      if( DissconnetIsTimingEnd() )
      {
        SetSequence( E_SEQ_DISCONNECT_EXE );
      }
    }
    break;
  case E_SEQ_DISCONNECT_EXE:   ///< P2P切断実行
    {
      // このクラスで作ったもの系の切断処理
      if( DisconnectExe() == true )
      {
        SetSequence( E_SEQ_DISCONNECT_START );
      }
    }
    break;
  case E_SEQ_DISCONNECT_START: ///< P2P切断処理
    {
      if( m_P2pManager )
      {
        if( m_P2pManager->DisconnectStart() )
        {
          SetSequence( E_SEQ_DISCONNECT_WAIT );
        }
      }
    }
    break;
  case E_SEQ_DISCONNECT_WAIT:  ///< P2P切断待ち
    // リスナー待ち
    break;
  case E_SEQ_DISCONNECT_LOOP:  ///< P2P切断完了(空ループ)
    // 空ループ
    break;
  }
}

//------------------------------------------------------------------
/**
 * @brief   エラ－リスナー登録
 * @param   listener   通知を受け取るP2pConnectionErrorListener
 */
//------------------------------------------------------------------
void P2pConnectionManager::RegistErrorListener( NetLib::P2P::P2pConnectionErrorListener* listener )
{
  m_errorListener = listener;
}

//------------------------------------------------------------------
/**
 * @brief   エラーリスナー解除
 */
//------------------------------------------------------------------
void P2pConnectionManager::UnregistErrorListener( void )
{
  m_errorListener = NULL;
}

//------------------------------------------------------------------
/**
  * @brief   マイグレーションリスナー登録
  * @param   listener   通知を受け取るP2pConnectionMigrationListener
  */
//------------------------------------------------------------------
void P2pConnectionManager::RegistMigrationListener( NetLib::P2P::P2pConnectionMigrationListener* listener )
{
  m_migrationListener = listener;
}

//------------------------------------------------------------------
/**
  * @brief   マイグレーションリスナー解除
  */
//------------------------------------------------------------------
void P2pConnectionManager::UnregistMigrationListener( void )
{
  m_migrationListener = NULL;
}

//------------------------------------------------------------------
/**
 * @brief   クイックマッチ専用のP2P接続成功
 * @return  P2P接続成功したらtrue
 *
 * @note    クイックマッチはP2P接続直後のバックグラウンドのデータやり取りがいらないので、
 *          P2Pが繋がっただけを知る必要があった。
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsQuickMatchSuccess()
{
  // 接続後のシーケンスならばP2P接続は完了
  if( E_SEQ_RATING_INITIALIZE <= m_seq && m_seq <= E_SEQ_CONNECT_LOOP )
  {
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   マッチメイクセッション作成成功
 * @return  成功してたらtrue
 *
 * @note    NCL三木さん要望：ml-pokenet:397
 *          子の再検索を止めて欲しいとのことで、
 *          親がマッチメイクセッション作成してから子が検索しにいく必要があった。
 *          その為に、マッチメイクセッション作成したどうかを取得する必要がある。
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsMatchmakingCreateSuccess()
{
  return m_isMatchmakingCreateSuccess;
}

//------------------------------------------------------------------
/**
  * @brief   シーケンス番号設定
  */
//------------------------------------------------------------------
void P2pConnectionManager::SetSequence( NetLib::P2P::P2pConnectionManager::E_SEQ seq )
{
  m_seq = seq;
  #if PM_DEBUG
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"P2pConnectionManager : seq[%d]\n",m_seq);
  #endif
}


#if PM_DEBUG
//! エラー対処正常終了チェック
void P2pConnectionManager::DEBUG_CheckErrorHandling()
{
  // 初期化したのにTerminate呼んでいない
  if( m_isInitialize == true )
  {
    GFL_ASSERT_MSG(0,"P2pConnectionManager Terminate忘れ\n");
  }
  // 初期状態 or 切断終了　になっていない
  if( !(m_seq == E_SEQ_NONE || m_seq == E_SEQ_DISCONNECT_LOOP) )
  {
    GFL_ASSERT_MSG(0,"P2pConnectionManager シーケンス不正 %d \n", m_seq);
  }
}
#endif // PM_DEBUG




//=============================================================================
/**
*  P2pManagerリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief   P2P接続成功通知
*/
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pManagerConnectSuccess()
{
  GFL_PRINT("P2pConnectionManager::OnP2pManagerConnectSuccess\n");

  // 既に切断処理中・切断完了していたら何もしない
  if( E_SEQ_DISCONNECT_TIMING <= m_seq && m_seq <= E_SEQ_DISCONNECT_LOOP )
  {
    GFL_PRINT("切断中なので接続成功通知は無視\n");
    return;
  }

  SetSequence( E_SEQ_RATING_INITIALIZE );

  // もうタイムアウトは必要ない
  m_timeout.Stop();
}

//------------------------------------------------------------------
/**
* @brief   P2P切断成功通知
*/
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pManagerDisconnectSuccess()
{
  GFL_PRINT("P2pConnectionManager::OnP2pManagerDisconnectSuccess\n");
  SetSequence( E_SEQ_DISCONNECT_LOOP );
  // エラーダイアログは抑制を解除する
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->P2pDisconnectSpanOff();
}

//------------------------------------------------------------------
/**
* @brief P2pManager通信のエラー通知
*/
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pManagerError()
{
  GFL_PRINT("P2pConnectionManager::OnP2pManagerError\n");

  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    m_errorListener->OnP2pConnectionErrorSystem();
  }
}

//------------------------------------------------------------------
/**
* @brief P2pManager通信のエラー通知
*
* @param[in] result     実行結果
* @param[in] errorCode  エラーアプレット用エラーコード
*/
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
void P2pConnectionManager::OnP2pManagerError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("P2pConnectionManager::OnP2pManagerError\n");
  GFL_PRINT("result:%d\n", result.IsError() );
  GFL_PRINT("errorCode:%d\n", errorCode );

  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    m_errorListener->OnP2pConnectionErrorNex( errorCode );
  }
}
#endif // GF_PLATFORM_CTR 

//------------------------------------------------------------------
/**
* @brief P2P切断通知
*/
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pManagerSessionLeft()
{
  GFL_PRINT("P2pConnectionManager::OnP2pManagerSessionLeft\n");

  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    m_errorListener->OnP2pConnectionErrorSessionLeft();
  }
}

//------------------------------------------------------------------
/**
 * @brief マッチメイクエラー通知（独自ハンドリング版）
 */
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pManagerMatchMakeFailed()
{
  GFL_PRINT("P2pConnectionManager::OnP2pManagerMatchMakeFailed\n");

  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    m_errorListener->OnP2pConnectionErrorMatchMakeFailed();
  }
}


//------------------------------------------------------------------
/**
* @brief 内部で確保されたヒープメモリが枯渇したことを通知
*/
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pManagerErrorOutOfMemory()
{
  GFL_PRINT("P2pConnectionManager::OnP2pMangerErrorOutOfMemory\n");

  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    m_errorListener->OnP2pConnectionErrorOutOfMemory();
  }
}

//------------------------------------------------------------------
/**
  * @brief マイグレーション成功
  */
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pManagerMigrationSuccess()
{
  GFL_PRINT("P2pConnectionManager::OnP2pManagerMigrationSuccess\n");

  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    if( m_migrationListener != NULL )
    {
      m_migrationListener->OnP2PConnectionMigrationSuccess();
    }
    else
    {
      m_errorListener->OnP2pConnectionErrorSessionLeft();
    }
  }
}

#ifdef  GF_PLATFORM_CTR
//------------------------------------------------------------------
/**
 * @brief マッチメイクセッションの作成完了通知
 *
 * @param[in] gatheringId 作成されたセッションのギャザリングID
 */
//------------------------------------------------------------------
void P2pConnectionManager::OnMatchmakingCreateSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT("P2pConnectionManager::OnMatchmakingCreateSuccess\n");
  m_isMatchmakingCreateSuccess = true;
}
#endif // GF_PLATFORM_CTR

//=============================================================================
/**
*  NexRatingClientリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief Ratingセッション作成の成功通知
*
* @param[in] sessionKey 生成されたセッションキー
*/
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
void P2pConnectionManager::OnCreateRatingSessionSuccess( const nn::nex::qVector<qByte>& sessionKey )
{
  GFL_PRINT("P2pConnectionManager::OnCreateRatingSessionSuccess\n");
  gfl2::std::MemClear( m_ratingSessionKeyBuffer, sizeof(m_ratingSessionKeyBuffer) );

  m_ratingSessionKeySize = sessionKey.size();

  // セッションキーのコピー
  if( m_ratingSessionKeySize <= RATING_SESSION_KEY_SIZE )
  {
    for( int i=0; i<m_ratingSessionKeySize; i++ )
    {
      m_ratingSessionKeyBuffer[i] = sessionKey.at(i);
    }
    ICHI_PRINT("レーティングセッション作成 size=%d %d %d %d \n", m_ratingSessionKeySize,m_ratingSessionKeyBuffer[0], m_ratingSessionKeyBuffer[1], m_ratingSessionKeyBuffer[2] );
  }
  else
  {
    GFL_ASSERT(0);
  }
  m_isRatingListenerSuccess = true;
}
#endif // GF_PLATFORM_CTR 

//------------------------------------------------------------------
/**
* @brief Ratingセッション参加終了通知
*/
//------------------------------------------------------------------
void P2pConnectionManager::OnJoinRatingSessionSuccess( void )
{
  GFL_PRINT("P2pConnectionManager::OnJoinRatingSessionSuccess\n");
#ifdef  GF_PLATFORM_CTR
  m_isRatingListenerSuccess = true;
#endif // GF_PLATFORM_CTR 
}

//------------------------------------------------------------------
/**
* @brief Ratingサーバ通信のエラー通知
*
* @param[in] result    実行結果
* @param[in] errorCode 通信エラーコード
*/
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
void P2pConnectionManager::OnNexRatingClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("P2pConnectionManager::OnNexRatingClientError\n");
  GFL_PRINT("result:%d\n", result.IsError() );
  GFL_PRINT("errorCode:%d\n", errorCode );

  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    m_errorListener->OnP2pConnectionErrorNex( errorCode );
  }
}
#endif // GF_PLATFORM_CTR 

//------------------------------------------------------------------
/**
* @brief Ratingサーバ通信のキャンセル通知
*/
//------------------------------------------------------------------
void P2pConnectionManager::OnNexRatingClientCanceled( void )
{
  GFL_PRINT("P2pConnectionManager::OnNexRatingClientCanceled\n");
  // キャンセルは終了時のみなので検知する必要はない
}
//=============================================================================
/**
*  P2pTimeoutリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief タイムアウト発生通知
 */
//------------------------------------------------------------------
void P2pConnectionManager::OnP2pTimeoutError()
{
  GFL_PRINT("P2pConnectionManager::OnP2pTimeoutError\n");
  GFL_ASSERT(m_errorListener);
  if( m_errorListener != NULL )
  {
    m_errorListener->OnP2pConnectionErrorTimeout();
  }
}





//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   切断実行
 */
//------------------------------------------------------------------
bool P2pConnectionManager::DisconnectExe()
{
  if( IsRating() )
  {
    if( RatingClientTerminate() == false )
    {
      return false;
    }
  }

  if( m_NetGame != NULL )
  {
    m_NetGame->Finalize();
    GFL_SAFE_DELETE( m_NetGame );
  }
  GFL_SAFE_DELETE( m_pP2pCommonData );

  return true;
}

//------------------------------------------------------------------
/**
 * @brief   切断前同期開始
 */
//------------------------------------------------------------------
void P2pConnectionManager::DissconnetTimingStart()
{
  if( m_NetGame != NULL )
  {
    m_NetGame->TimingStart( DISCONNECT_TIMING_NO );
  }

  // タイムアウトカウント開始(短め）
  m_disconnectTimingTimeOut.TimeOutStart( 8000 ); // 8秒
}

//------------------------------------------------------------------
/**
 * @brief   切断前同期完了チェック
 * @return   trueで完了
 */
//------------------------------------------------------------------
bool P2pConnectionManager::DissconnetIsTimingEnd()
{
  // 同期完了かタイムアウト
  if( ( m_NetGame && m_NetGame->IsTimingEnd( DISCONNECT_TIMING_NO ) ) ||
       m_disconnectTimingTimeOut.IsTimeOut() )
  {
    return true;
  }

  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief P2P通信IDから通信経路定義を取得
 * @return  通信経路定義
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaScript::CommRouteType P2pConnectionManager::GetCommRouteTypeForP2pID() const
{
  switch( m_p2pId ){
  case NIJI_P2P_JF_QUICK_2:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
  case NIJI_P2P_JF_QUICK_4:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
    return JoinFestaScript::COMM_ROUTE_QUICK;           //!< クイック通信（GFNMCat[1628]対処）
  case NIJI_P2P_BATTLE_2_L:    ///< バトル（2人接続）（ローカル）
  case NIJI_P2P_BATTLE_2_W:    ///< バトル（2人接続）（WiFi）
  case NIJI_P2P_BATTLE_4_L:    ///< バトル（4人接続）（ローカル）
  case NIJI_P2P_BATTLE_4_W:    ///< バトル（4人接続）（WiFi）
  case NIJI_P2P_BATTLE_TREE_2: ///< バトルツリー
    return JoinFestaScript::COMM_ROUTE_P2P_BATTLE;      //!< 通信対戦
  case NIJI_P2P_TRADE_L:    ///< 交換（ローカル)
  case NIJI_P2P_TRADE_W:    ///< 交換（WiFi）
    return JoinFestaScript::COMM_ROUTE_P2P_TRADE;       //!< 通信交換
  case NIJI_P2P_RNDRATE_S:    ///< レーティング:シングル
  case NIJI_P2P_RNDRATE_D:    ///< レーティング:ダブル
  case NIJI_P2P_RNDRATE_SP:    ///< レーティング:スペシャル
  case NIJI_P2P_RNDRATE_WCS:  ///<  レーティング：WCS
  case NIJI_P2P_RNDFREE_S:    ///< フリー：シングル
  case NIJI_P2P_RNDFREE_D:    ///< フリー：ダブル
  case NIJI_P2P_RNDFREE_B:    ///< フリー：バトルロイヤル
  case NIJI_P2P_RNDFREE_S_LEGEND:    ///< フリー：シングル(伝説あり)
  case NIJI_P2P_RNDFREE_D_LEGEND:    ///< フリー：ダブル(伝説あり)
  case NIJI_P2P_RNDFREE_B_LEGEND:    ///< フリー：バトルロイヤル(伝説あり)
  case NIJI_P2P_WIFI_CUP:    ///< インターネット大会
  case NIJI_P2P_GROUP_CUP:    ///< 仲間大会
    return JoinFestaScript::COMM_ROUTE_RANDOM_BATTLE;   //!< ランダムマッチ
  case NIJI_P2P_RANDOM_TRADE:    ///< ランダム交換
    return JoinFestaScript::COMM_ROUTE_MIRACLE_TRADE;   //!< ミラクル交換
  case NIJI_P2P_LIVE_CUP:    ///< LIVE大会
  case NIJI_P2P_GIFT:    ///< ふしぎなおくりもの
    // 通信経路がない場合は無視
    return JoinFestaScript::COMM_ROUTE_MAX;
  }

  return JoinFestaScript::COMM_ROUTE_MAX;
}

//------------------------------------------------------------------
/**
 * @brief   レーティング接続かチェック
 * @return  trueでレーティングである
 */
//------------------------------------------------------------------
bool P2pConnectionManager::IsRating()
{
  switch( m_p2pId ){
  case NIJI_P2P_RNDRATE_S:    ///< レーティング:シングル
  case NIJI_P2P_RNDRATE_D:    ///< レーティング:ダブル
  case NIJI_P2P_RNDRATE_SP:   ///< レーティング:スペシャル
  case NIJI_P2P_RNDRATE_WCS:  ///< レーティング：WCS
  case NIJI_P2P_WIFI_CUP:     ///< インターネット大会
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   レーティングClientの作成
 */
//------------------------------------------------------------------
void P2pConnectionManager::RatingClientCreate()
{
#ifdef  GF_PLATFORM_CTR
  GFL_ASSERT(m_NexRatingClient==NULL);//@fix
  GFL_ASSERT(m_ratingSessionKey==NULL);//@fix

  m_NexRatingClient = GFL_NEW( m_DevHeap ) NexRatingClient();
  m_NexRatingClient->RegistRatingClientListener( this );
  m_ratingSessionKey = qNew nn::nex::qVector<qByte>;
#endif // GF_PLATFORM_CTR 
}
//------------------------------------------------------------------
/**
 * @brief   レーティングClientの初期化
 * @return  初期化に成功したならtrueを返却する
 */
//------------------------------------------------------------------
bool P2pConnectionManager::RatingClientInitialize()
{
#ifdef  GF_PLATFORM_CTR
  if( m_NexRatingClient )
  {
    if( m_ratingSessionKey )
    {
      m_ratingSessionKey->clear();
    }
    m_ratingSeq = 0;
    gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
    return m_NexRatingClient->Initialize( pNexManager->GetNgsFacade()->GetCredentials() );
  }
#endif // GF_PLATFORM_CTR 
  // 既に作ってある場合はOK
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   レーティングセッションを確立する
 * @return  完了でtrueを返す
 */
//------------------------------------------------------------------
bool P2pConnectionManager::RatingSessionSequence()
{
#ifdef  GF_PLATFORM_CTR
  // ちょっと強引なので変えたい・・・
  if( gflnet2::p2p::NetGame::GetNetGameBase()->IsMaster() )
  {
    //==========================
    // 親の流れ
    // 1.レートセッション作成
    // 2.セッションキーを子に送信
    //==========================
    switch( m_ratingSeq ){
    case 0:
      if( RatingCreateSession() )
      {
        m_ratingSeq++;
      }
      break;
    case 1:
      // リスナー待ち
      if( m_isRatingListenerSuccess )
      {
        m_ratingSeq++;
      }
      break;
    case 2:
      // セッションキー送信
      ICHI_PRINT("レーティングセッションキー送信 size=%d %d %d %d \n", m_ratingSessionKeySize,m_ratingSessionKeyBuffer[0], m_ratingSessionKeyBuffer[1], m_ratingSessionKeyBuffer[2] );
      if( m_NetGame->SendData( (char*)m_ratingSessionKeyBuffer, m_ratingSessionKeySize ) )
      {
        m_ratingSeq++;
        
      }
      break;
    case 3:
      {
        // 自分のデータを受信しておく
        for( int i=0; i<m_P2pManager->GetInitParam()->GetLimitNum(); i++ )
        {
          if( m_NetGame->RecvData( i, (char*)m_ratingSessionKeyBuffer, RATING_SESSION_KEY_SIZE, &m_ratingSessionKeySize ) )
          {
            // レーティングセッションキーを受け取ったら次に進む
            ICHI_PRINT("レーティングセッションキー受信 size=%d %d %d %d \n", m_ratingSessionKeySize,m_ratingSessionKeyBuffer[0], m_ratingSessionKeyBuffer[1], m_ratingSessionKeyBuffer[2] );
            if( m_ratingSessionKeySize != 0 )
            {
              return true;
            }
          }
        }
      }
    break;
    }
  }
  else
  {
    //==========================
    // 子の流れ
    // 1.セッションキーを親から受信
    // 2.レートセッションに参加
    //==========================
    switch( m_ratingSeq ){
    case 0:
      {
        // 受信
        for( int i=0; i<m_P2pManager->GetInitParam()->GetLimitNum(); i++ )
        {
          if( m_NetGame->RecvData( i, (char*)m_ratingSessionKeyBuffer, RATING_SESSION_KEY_SIZE, &m_ratingSessionKeySize ) )
          {
            // レーティングセッションキーを受け取ったら次に進む
            ICHI_PRINT("レーティングセッションキー受信 size=%d %d %d %d \n", m_ratingSessionKeySize,m_ratingSessionKeyBuffer[0], m_ratingSessionKeyBuffer[1], m_ratingSessionKeyBuffer[2] );
            if( m_ratingSessionKeySize != 0 )
            {
              m_ratingSeq++;
              break;
            }
          }
        }
      }
      break;
    case 1:
      {
        // レートセッションに参加
        if( RatingJoinSession() )
        {
          m_ratingSeq++;
        }
      }
      break;
    case 2:
      {
        // リスナー待ち
        if( m_isRatingListenerSuccess )
        {
          return true;
        }
      }
      break;
    }
  }
#endif // GF_PLATFORM_CTR 

  return false;
}

//------------------------------------------------------------------
/**
 * @brief   レーティングセッションを生成する
 * @return  実行に成功したならtrueを返却する
 */
//------------------------------------------------------------------
bool P2pConnectionManager::RatingCreateSession()
{
#ifdef  GF_PLATFORM_CTR
  if( m_NexRatingClient )
  {
    m_isRatingListenerSuccess = false;
    return m_NexRatingClient->CreateRatingSession();
  }
#endif // GF_PLATFORM_CTR 
  return false;
}
//------------------------------------------------------------------
/**
 * @brief   レーティングセッションに参加する
 * @return  実行に成功したならtrueを返却する
 */
//------------------------------------------------------------------
bool P2pConnectionManager::RatingJoinSession()
{
#ifdef  GF_PLATFORM_CTR
  if( m_NexRatingClient )
  {
    if( m_ratingSessionKeySize <= RATING_SESSION_KEY_SIZE )
    {
      m_ratingSessionKey->clear();
      // セッションキーを作成
      for( int i=0; i<m_ratingSessionKeySize; i++ )
      {
        m_ratingSessionKey->push_back( (qByte)m_ratingSessionKeyBuffer[i] );
      }
      m_isRatingListenerSuccess = false;
      return m_NexRatingClient->JoinRatingSession( *m_ratingSessionKey );
    }
  }
#endif // GF_PLATFORM_CTR 
  return false;
}
//------------------------------------------------------------------
/**
 * @brief   レーティングClientの終了
 */
//------------------------------------------------------------------
bool P2pConnectionManager::RatingClientTerminate()
{
#ifdef  GF_PLATFORM_CTR
  if( m_NexRatingClient )
  {
    m_ratingSessionKey->clear();
    m_NexRatingClient->CancelConnecting();
    if( m_NexRatingClient->IsConnecting() )
    {
      return false;
    }

    m_NexRatingClient->Finalize();
  }
#endif // GF_PLATFORM_CTR 

  return true;
}
//------------------------------------------------------------------
/**
 * @brief   レーティングClientの破棄
 */
//------------------------------------------------------------------
void P2pConnectionManager::RatingClientDelete()
{
#ifdef  GF_PLATFORM_CTR
  if( m_NexRatingClient )
  {
    m_NexRatingClient->UnregistRatingClientListener( this );
    GFL_SAFE_DELETE( m_NexRatingClient );
  }

  if( m_ratingSessionKey )
  {
    qDelete m_ratingSessionKey;
    m_ratingSessionKey = NULL;
  }
#endif // GF_PLATFORM_CTR 
}

GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

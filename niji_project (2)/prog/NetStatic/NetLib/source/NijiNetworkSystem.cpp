//============================================================================================
/**
 * @file NijiNetworkSystem.cpp
 *
 * @author kousaka_shin
 */
//============================================================================================
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>
#include <gflnet2/source/p2p/gflnet2_NetGameBase.h>
#include <gflnet2/source/dev/gflnet2_IrAdapter.h>
#include <gflnet2/include/ndm/gflnet2_NdmManager.h>
#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include <gflnet2/include/ndm/gflnet2_NdmManager.h>
#include <heap/include/gfl2_heap_manager.h>
#include "system/include/HeapDefine.h"
#include "system/include/HeapSize.h"
#include <GameSys/include/GameManager.h>
#include "ExtSavedata/include/NijiExtSaveSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/source/Error/NijiNetworkErrorDialog.h" // 非公開ヘッダ
#include "NetStatic/NetLib/source/Error/Detect/NijiNetworkErrorDetectDefault.h" // 非公開ヘッダ
#include "NetStatic/NetLib/source/Error/Detect/NijiNetworkErrorDetectBattleSpot.h" // 非公開ヘッダ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketEventList.h"
#include "NetStatic/NetLib/source/Beacon/LocalBeaconTrader.h"
#include "NetStatic/NetLib/include/Wifi/SubscriptionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaInformationMessage.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"
#include  <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include  <System/include/ThreadPriority.h>

GFL_NAMESPACE_BEGIN(NetLib)

Error::NijiNetworkErrorDetectDefault*     NijiNetworkSystem::m_errorDetectDefault = NULL;
Error::NijiNetworkErrorDetectBattleSpot*  NijiNetworkSystem::m_errorDetectBattleSpot = NULL;
NijiNetworkSystem::NETWORK_MODE NijiNetworkSystem::m_networkMode = NijiNetworkSystem::NETWORK_MODE_OFFLINE;
NetAppLib::P2PTrade::P2PTradeRequestClient* NijiNetworkSystem::m_pP2PTradeRequestClient = NULL;
LiveRecSender* NijiNetworkSystem::m_pLiveRecSender = NULL;

#if defined(GF_PLATFORM_CTR)
Beacon::LocalBeaconTrader* NijiNetworkSystem::m_pBeaconTrader = NULL;
BattleSpotNetworkMonitor* NijiNetworkSystem::m_pBSNetworkMonitor = NULL;
#endif

//------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 */
//------------------------------------------------------------------
NijiNetworkSystem::NijiNetworkSystem( gfl2::heap::HeapBase* pHeap )
: m_networkErrorDialog(NULL)
{
  InitializeInternal( pHeap );
}


//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
NijiNetworkSystem::~NijiNetworkSystem()
{
  TerminateInternal();
}

//------------------------------------------------------------------
/**
  * @brief   初期化
  */
//------------------------------------------------------------------
void NijiNetworkSystem::InitializeInternal( gfl2::heap::HeapBase* pHeap )
{

  // エラーダイアログ管理クラス作成
  m_networkErrorDialog = GFL_NEW( pHeap ) NetLib::Error::NijiNetworkErrorDialog();

  // ストリーミングクラス作成
  m_pInfomationMessage = GFL_NEW( pHeap ) NetAppLib::JoinFesta::JoinFestaInformationMessage( pHeap );

  //=============================================================================
  /**
   *  ライブラリのシングルトン
   */
  //=============================================================================
#if defined(GF_PLATFORM_CTR)
  //フレンドマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager) == NULL )
  {
    gflnet2::friends::FriendManager* pFriendManager = GFL_NEW( pHeap ) gflnet2::friends::FriendManager();
    nn::Result ret = pFriendManager->Initialize();
    GFL_ASSERT( ret.IsSuccess() );
    pFriendManager->StoreFriendKeyList();
  }
  //Wifiシステム
  if( GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem) == NULL )
  {
    gflnet2::wifi::WifiSystem* pWifiSystem = GFL_NEW( pHeap ) gflnet2::wifi::WifiSystem();
  }
  //Nexマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager) == NULL ) 
  {
    gflnet2::nex::NexManager* pNexManager = GFL_NEW( pHeap ) gflnet2::nex::NexManager();
  }
  // NexMatchMakingClient
  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient) == NULL ) 
  {
    gflnet2::nex::NexMatchMakingClient* pNexMatchmakingClient = GFL_NEW( pHeap ) gflnet2::nex::NexMatchMakingClient();
  }
#endif // GF_PLATFORM_CTR
  //ServerClientRequestマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) == NULL ) 
  {
    gflnet2::ServerClient::ServerClientRequestManager* pServerClientRequestManager = GFL_NEW( pHeap ) gflnet2::ServerClient::ServerClientRequestManager();
  }

  //=============================================================================
  /**
   *  プロジェクトのシングルトン
   */
  //=============================================================================
  // P2pConnectionManager
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager) == NULL ) 
  {
    NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_NEW( pHeap ) NetLib::P2P::P2pConnectionManager();
  }
  // 通信エラーマネージャ
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) == NULL ) 
  {
    NetLib::Error::NijiNetworkErrorManager* networkErrorManager = GFL_NEW( pHeap ) NetLib::Error::NijiNetworkErrorManager( m_networkErrorDialog );
  }
  // JoinFestaパーソナルマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager) == NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* jfPersonalManager = GFL_NEW( pHeap ) NetAppLib::JoinFesta::JoinFestaPersonalDataManager( pHeap );
    jfPersonalManager->Initialize();
  }
  // JoinFestaイベントリスト
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketEventList) == NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaPacketEventList* jfPacketEventList = GFL_NEW( pHeap ) NetAppLib::JoinFesta::JoinFestaPacketEventList( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager), pHeap );
  }
  // JoinFestaアトラクションマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager) == NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaAttractionManager* jfAttractionManager = GFL_NEW( pHeap ) NetAppLib::JoinFesta::JoinFestaAttractionManager( pHeap );
  }
#if defined(GF_PLATFORM_CTR)
  // サブスクリプションマネージャ
  if( GFL_SINGLETON_INSTANCE(NetLib::Wifi::SubscriptionManager) == NULL ) 
  {
    NetLib::Wifi::SubscriptionManager* subscriptionManager = GFL_NEW( pHeap ) NetLib::Wifi::SubscriptionManager();
  }

  // JoinFestaパケットマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager) == NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaPacketManager* jfPacketManager = GFL_NEW( pHeap ) NetAppLib::JoinFesta::JoinFestaPacketManager();
    jfPacketManager->SetupNetApp( m_pInfomationMessage , 
                                  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager) ,
                                  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketEventList) ,
                                  GFL_SINGLETON_INSTANCE(NetLib::Wifi::SubscriptionManager) );
    jfPacketManager->Initialize();
  }

  //ローカルビーコンクラス
  if( m_pBeaconTrader == NULL )
  {
    m_pBeaconTrader = GFL_NEW( pHeap ) NetLib::Beacon::LocalBeaconTrader();
    m_pBeaconTrader->SetListener( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager) );
  }

  //ndm
  if( GFL_SINGLETON_INSTANCE(gflnet2::ndm::NdmManager) == NULL )
  {
    gflnet2::ndm::NdmManager* pNdmManager = GFL_NEW( pHeap ) gflnet2::ndm::NdmManager( pHeap );
  }

  //boss
  if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager) == NULL )
  {
    gflnet2::boss::BossManager* pBossManager = GFL_NEW( pHeap ) gflnet2::boss::BossManager();
    gflnet2::boss::BossManager::BOSS_INIT_PARAM bossInitParam;
    std::memset( &bossInitParam , 0 , sizeof(bossInitParam));
    bossInitParam.entryCount = NIJI_BOSS_ENTRY_COUNT;
    bossInitParam.pBossCode = NIJI_BOSS_CODE;
    bossInitParam.pMountName = gfl2::fs::ExtSaveSystem::GetMountName();
    bossInitParam.storageId = ExtSavedata::NijiExtSaveData::GetExtSaveDataId();
    bossInitParam.storageSize = NIJI_BOSS_STORAGE_SIZE;

    nn::Result result = pBossManager->Initialize( bossInitParam );
    GFL_ASSERT( result.IsSuccess() );
  }
#endif
}

//------------------------------------------------------------------
/**
  * @brief   終了
  */
//------------------------------------------------------------------
void NijiNetworkSystem::TerminateInternal( void )
{
  GFL_SAFE_DELETE( m_pLiveRecSender );
  // ストリーミングクラス削除
  GFL_SAFE_DELETE( m_pInfomationMessage );
  // エラーダイアログ管理クラス削除
  GFL_SAFE_DELETE( m_networkErrorDialog );

#if defined(GF_PLATFORM_CTR)
  if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager) != NULL )
  {
    gflnet2::boss::BossManager* pBossManager = GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager);
    pBossManager->Finalize();
    GFL_SAFE_DELETE( pBossManager );
  }

  if( GFL_SINGLETON_INSTANCE(gflnet2::ndm::NdmManager) != NULL )
  {
    gflnet2::ndm::NdmManager* pNdmManager = GFL_SINGLETON_INSTANCE(gflnet2::ndm::NdmManager);
    GFL_SAFE_DELETE( pNdmManager );
  }

  GFL_SAFE_DELETE( m_pBeaconTrader );

  // JoinFestaパケットマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager) != NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaPacketManager* jfPacketManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager);
    jfPacketManager->Finalize();
    GFL_SAFE_DELETE( jfPacketManager );
  }

  // サブスクリプションマネージャ
  if( GFL_SINGLETON_INSTANCE(NetLib::Wifi::SubscriptionManager) != NULL ) 
  {
    NetLib::Wifi::SubscriptionManager* subscriptionManager = GFL_SINGLETON_INSTANCE(NetLib::Wifi::SubscriptionManager);
    GFL_SAFE_DELETE( subscriptionManager );
  }

  //フレンドマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager) != NULL )
  {
    gflnet2::friends::FriendManager* pFriendManager = GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager);
    nn::Result ret = pFriendManager->Finalize();
    GFL_ASSERT( ret.IsSuccess() );
    GFL_SAFE_DELETE( pFriendManager );
  }
  //Wifiシステム
  if( GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem) != NULL )
  {
    gflnet2::wifi::WifiSystem* pWifiSystem = GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem);
    pWifiSystem->DeleteAsync();
    pWifiSystem->Terminate();
    GFL_SAFE_DELETE( pWifiSystem );
  }
  //Nexマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager) != NULL ) 
  {
    gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
    pNexManager->Finalize();
    GFL_SAFE_DELETE( pNexManager );
  }
  // NexMatchMakingClient
  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient) != NULL ) 
  {
    gflnet2::nex::NexMatchMakingClient* pNexMatchmakingClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
    GFL_SAFE_DELETE( pNexMatchmakingClient );
  }
#endif // GF_PLATFORM_CTR
  //ServerClientRequestマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) != NULL ) 
  {
    gflnet2::ServerClient::ServerClientRequestManager* pServerClientRequestManager = GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager);
    GFL_SAFE_DELETE( pServerClientRequestManager );
  }
  // P2pConnectionManager
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager) != NULL ) 
  {
    NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
    pP2pConnectionManager->Terminate();
    GFL_SAFE_DELETE( pP2pConnectionManager );
  }
  // 通信エラーマネージャ
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) != NULL ) 
  {
    NetLib::Error::NijiNetworkErrorManager* networkErrorManager = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager);
    GFL_SAFE_DELETE( networkErrorManager );
  }
  // JoinFestaパーソナルマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager) != NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* jfPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    jfPersonalManager->Finalize();
    GFL_SAFE_DELETE( jfPersonalManager );
  }
  // JoinFestaイベントリスト
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketEventList) != NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaPacketEventList* jfPacketEventList = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketEventList);
    GFL_SAFE_DELETE( jfPacketEventList );
  }
  // JoinFestaアトラクションマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager) != NULL ) 
  {
    NetAppLib::JoinFesta::JoinFestaAttractionManager* jfAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
    GFL_SAFE_DELETE( jfAttractionManager );
  }
#if defined(GF_PLATFORM_CTR)
  GFL_SAFE_DELETE( m_pBSNetworkMonitor );
#endif
}

//------------------------------------------------------------------
/**
  * @brief   前更新
  */
//------------------------------------------------------------------
void NijiNetworkSystem::UpdateBefore( void )
{
#if defined(GF_PLATFORM_CTR)
  //boss
  if( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager) != NULL )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->Update();
  }

  //フレンドマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager) != NULL )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager)->Update();
  }
  //Wifiシステム
  if( GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem) != NULL )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->Update();
  }
  //Nexマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
  }

  // JoinFestaパケットマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->Update();
  }

#endif // GF_PLATFORM_CTR
  //ServerClientRequestマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Update();
  }
  // 通信エラーマネージャ
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->Update();
  }
  // P2pConnectionManager
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->Update();
  }
  // JoinFestaアトラクションマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->Update();
  }

#if defined(GF_PLATFORM_CTR)
  if( m_pBeaconTrader )
  {
    if( IsWLANSwitchEnable() )
    {//WLANスイッチON
      m_pBeaconTrader->UpdateRestartBeacon();
    }
    else
    {//WLANスイッチOFF
      m_pBeaconTrader->UpdateStopBeacon();
    }
    m_pBeaconTrader->UpdateBefore();
  }
#endif

  if( m_pP2PTradeRequestClient )
  {
    m_pP2PTradeRequestClient->Update();
  }

  if( m_pLiveRecSender )
  {
    m_pLiveRecSender->Update();
  }

  UpdateBattleSpotNetworkMonitor();
}

//------------------------------------------------------------------
/**
  * @brief   後更新
  */
//------------------------------------------------------------------
void NijiNetworkSystem::UpdateAfter( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBeaconTrader )
  {
    m_pBeaconTrader->UpdateAfter();
  }

  //Nexマネージャ
  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
  }
#endif // GF_PLATFORM_CTR

  // P2pConnectionManager
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager) != NULL ) 
  {
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->Update();
  }

  UpdateBattleSpotNetworkMonitor();
}

//------------------------------------------------------------------
/**
  * @brief   アプレット更新
  */
//------------------------------------------------------------------
void NijiNetworkSystem::UpdateErrorApplet( void )
{
  // エラーダイアログ管理クラス更新
  m_networkErrorDialog->Update();
}


//------------------------------------------------------------------
/**
  * @brief   通信の共通の初期化
  */
//------------------------------------------------------------------
void NijiNetworkSystem::InitializeNetworkInternal( void )
{
  // エラー検知作成（ヒープ作成後に呼ぶ事）
  CreateErrorDetect( ERROR_DETECT_TYPE_DEFAULT );
}



//------------------------------------------------------------------
/**
  * @brief   通信エラー検知の作成
  * @param   detectType エラー検知種類
  */
//------------------------------------------------------------------
void NijiNetworkSystem::CreateErrorDetect( ENUM_ERROR_DETECT_TYPE detectType )
{
#if defined(GF_PLATFORM_CTR)
  if( detectType == ERROR_DETECT_TYPE_DEFAULT )
  {// デフォルト
    // エラー検知クラス作成
    m_errorDetectDefault = GFL_NEW( GetNetworkHeap() ) NetLib::Error::NijiNetworkErrorDetectDefault( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) );

    // 各エラーリスナーへのセット
    GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->SetDisconnectingListener( m_errorDetectDefault );
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->SetNexErrorListener( m_errorDetectDefault );
    gflnet2::p2p::NetGameBase::RegistErrorListener( m_errorDetectDefault );
    gflnet2::dev::IrAdapter::RegistErrorListener( m_errorDetectDefault );
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->RegistErrorListener( m_errorDetectDefault );
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem()->SetListener( m_errorDetectDefault );
  }
  else
  {// バトルスポット用
    // エラー検知クラス作成
    m_errorDetectBattleSpot = GFL_NEW( GetNetworkHeap() ) NetLib::Error::NijiNetworkErrorDetectBattleSpot( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) );

    // 各エラーリスナーへのセット
    GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->SetDisconnectingListener( m_errorDetectBattleSpot );
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->SetNexErrorListener( m_errorDetectBattleSpot );
    gflnet2::p2p::NetGameBase::RegistErrorListener( m_errorDetectBattleSpot );
    gflnet2::dev::IrAdapter::RegistErrorListener( m_errorDetectBattleSpot );
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->RegistErrorListener( m_errorDetectBattleSpot );
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem()->SetListener( m_errorDetectBattleSpot );

    CreateBattleSpotNetworkMonitor();
  }
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
  * @brief   通信エラー検知の削除
  */
//------------------------------------------------------------------
void NijiNetworkSystem::DeleteErrorDetect( void )
{
#if defined(GF_PLATFORM_CTR)
  // リスナー解除
  GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->SetDisconnectingListener( NULL );
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->SetNexErrorListener( NULL );
  gflnet2::p2p::NetGameBase::UnregistErrorListener();
  gflnet2::dev::IrAdapter::UnregistErrorListener();
  GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->UnregistErrorListener();
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem()->SetListener( NULL );

  // エラー検知クラス削除
  GFL_SAFE_DELETE( m_errorDetectDefault );
  GFL_SAFE_DELETE( m_errorDetectBattleSpot );

  DeleteBattleSpotNetworkMonitor();
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
  * @brief   Wifi通信の初期化
  */
//------------------------------------------------------------------
void NijiNetworkSystem::InitializeWifiNetwork( void )
{
  GFL_ASSERT( m_networkMode == NETWORK_MODE_OFFLINE );//@fix

  //通信の共通初期化処理
  InitializeNetworkInternal();

#if defined(GF_PLATFORM_CTR)
  //Wifiシステム
  if( GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem) != NULL )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->Initialize( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
  }
#endif // GF_PLATFORM_CTR

  m_networkMode = NETWORK_MODE_WIFI;
}

//------------------------------------------------------------------
/**
  * @brief   ローカル通信の初期化
  */
//------------------------------------------------------------------
void NijiNetworkSystem::InitializeLocalP2PNetwork( void )
{
  GFL_ASSERT( m_networkMode == NETWORK_MODE_OFFLINE );//@fix

  //通信の共通初期化処理
  InitializeNetworkInternal();

  m_networkMode = NETWORK_MODE_LOCAL;
}

//------------------------------------------------------------------
/**
  * @brief   通信の終了(Wifi、ローカル共通)
  */
//------------------------------------------------------------------
void NijiNetworkSystem::TerminateNetwork( void )
{
#if defined(GF_PLATFORM_CTR)
  //Wifiシステム
  if( GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem) != NULL )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->DeleteAsync();
    GFL_SINGLETON_INSTANCE(gflnet2::wifi::WifiSystem)->Terminate();
  }
#endif // GF_PLATFORM_CTR

  DeleteErrorDetect();

  m_networkMode = NETWORK_MODE_OFFLINE;
}

//------------------------------------------------------------------
/**
  * @brief   通信用ヒープの取得(デバイスヒープ)
  */
//------------------------------------------------------------------
gfl2::heap::HeapBase* NijiNetworkSystem::GetNetworkHeap( void )
{
  return gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
}

//------------------------------------------------------------------
/**
 * @brief   通信エラー検知をバトルスポット専用に切り替える
 */
//------------------------------------------------------------------
void NijiNetworkSystem::SwitchErrorDetectToBattleSpot( void )
{
  DeleteErrorDetect();
  CreateErrorDetect( ERROR_DETECT_TYPE_BATTLESPOT );
}

//------------------------------------------------------------------
/**
 * @brief   通信エラー検知をデフォルトに切り替える
 */
//------------------------------------------------------------------
void NijiNetworkSystem::SwitchErrorDetectToDefault( void )
{
  DeleteErrorDetect();
  CreateErrorDetect( ERROR_DETECT_TYPE_DEFAULT );
}

// @fix NMCat[4197]：バトルスポットからの参加選択画面で通信エラーーが発生した時に、通知されない不具合修正用
//------------------------------------------------------------------
/**
 * @brief   通信エラー検知がバトルスポット専用かどうか
 */
//------------------------------------------------------------------
bool NijiNetworkSystem::IsErrorDetectForBattleSpot( void )
{
  return ( m_errorDetectBattleSpot != NULL );
}

//------------------------------------------------------------------
/**
  * @brief   ネットワークモードの取得
  */
//------------------------------------------------------------------
NijiNetworkSystem::NETWORK_MODE NijiNetworkSystem::GetNetworkMode( void )
{
  return m_networkMode;
}

//------------------------------------------------------------------
/**
  * @brief   ビーコンシステムの開始
  */
//------------------------------------------------------------------
void NijiNetworkSystem::StartBeaconSystem( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBeaconTrader )
  {
    m_pBeaconTrader->StartBeacon();
  }
#endif
}

//------------------------------------------------------------------
/**
  * @brief   送信するビーコンデータを設定
  */
//------------------------------------------------------------------
void NijiNetworkSystem::SetBeaconData( const u8* pSendData, u32 size )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBeaconTrader )
  {
    m_pBeaconTrader->SetBeaconData( pSendData , size );
  }
#endif
}

//------------------------------------------------------------------
/**
  * @brief   ビーコンシステムの停止(trueが返るまで呼ぶ)
  *          この関数で停止すると勝手に再開しなくなる
  */
//------------------------------------------------------------------
bool NijiNetworkSystem::UpdateStopBeaconSystem( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBeaconTrader )
  {
    m_pBeaconTrader->DisableBeacon();
    return m_pBeaconTrader->UpdateStopBeacon();
  }
#endif
  return true;
}

//------------------------------------------------------------------
/**
  * @brief   ビーコンシステムの再開
  */
//------------------------------------------------------------------
void NijiNetworkSystem::RestartBeaconSystem( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBeaconTrader )
  {
    m_pBeaconTrader->EnableBeacon();

    // フィールド中のビーコン飛ばしている時のみ更新する（ジョインフェスタはジョインフェスタ内で各所で更新している）
    NetAppLib::JoinFesta::JoinFestaPersonalData* myData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();

    if( myData->GetPlaySituation() == NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_FIELD )
    {
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SetPacketGameStatusWithMe();
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
    }
  }
#endif
}

//------------------------------------------------------------------
/**
* @brief  無線LANスイッチが有効かどうか
*
* @return 有効true : 無効false
*/
//------------------------------------------------------------------
bool NijiNetworkSystem::IsWLANSwitchEnable( void )
{
#if defined(GF_PLATFORM_CTR)
  return gflnet2::ndm::NdmManager::IsWLANSwitchEnable();
#else
  return false; // 常に無効にしておく
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
* @brief  ゲームサーバ接続中かどうか
*
* @return 有効true : 無効false
*/
//------------------------------------------------------------------
bool NijiNetworkSystem::IsGameServerConnecting( void )
{
#if defined(GF_PLATFORM_CTR)
  return GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->IsGameServerConnecting();
#else
  return false; // 常に無効にしておく
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
* @brief  一時的に取得するビーコンのIDを設定
*/
//------------------------------------------------------------------
void NijiNetworkSystem::SetTempBeaconID( u8 tempID )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBeaconTrader )
  {
    m_pBeaconTrader->SetTempBeaconID( tempID );
  }
#endif
}

//------------------------------------------------------------------
/**
* @brief  オンライン中の他プレイヤーの人数を取得
*/
//------------------------------------------------------------------
u32 NijiNetworkSystem::GetOnlinePlayerNum( void )
{
  u32 count = 0;
#if defined(GF_PLATFORM_CTR)
  // JoinFestaパーソナルマネージャ
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager) == NULL ) 
  {
    return count;
  }

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* jfPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  
  for( u32 i = 0; i < NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = jfPersonalManager->GetPersonalDataList( static_cast<NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); //@fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          if( (*it) && (*it)->IsOnline() )
          {
            count++;
          }
        }
      }
    }
  }
#endif
  return count;
}

//------------------------------------------------------------------
/**
* @brief  ペアレンタルモード判定
*/
//------------------------------------------------------------------
bool  NijiNetworkSystem::IsParentalMode( void )
{
#if defined(GF_PLATFORM_CTR)
  return nn::cfg::CTR::IsRestrictP2pCec() || nn::cfg::CTR::IsRestrictP2pInternet();
#else
  return false;
#endif
}

bool  NijiNetworkSystem::IsParentalModeOnlyInternet( void )
{
#if defined(GF_PLATFORM_CTR)
  return nn::cfg::CTR::IsRestrictP2pInternet();
#else
  return false;
#endif
}


//------------------------------------------------------------------
/**
  * @brief   P2P交換通信処理クラスの設定
  * @note    設定すると UpdateBefore で Update が呼ばれる
  */
//------------------------------------------------------------------
void NijiNetworkSystem::SetP2PTradeRequestClient( NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient )
{
  m_pP2PTradeRequestClient = pP2PTradeRequestClient;
}


//------------------------------------------------------------------
/**
  * @brief   P2P交換通信処理クラスの登録解除
  */
//------------------------------------------------------------------
void NijiNetworkSystem::RemoveP2PTradeRequestClient()
{
  m_pP2PTradeRequestClient = NULL;
}

//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦送信機能作成
  */
//------------------------------------------------------------------
void NijiNetworkSystem::CreateLiveRecSender( void )
{
  if( m_pLiveRecSender == NULL )
  {
    gfl2::heap::HeapBase * pEventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    m_pLiveRecSender = GFL_NEW( pEventDeviceHeap ) LiveRecSender();
  }
}

//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦送信機能削除
  */
//------------------------------------------------------------------
void NijiNetworkSystem::DeleteLiveRecSender( void )
{
  GFL_SAFE_DELETE( m_pLiveRecSender );
}

//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦送信機能セットアップ
  */
//------------------------------------------------------------------
void NijiNetworkSystem::SetupLiveRecSender( u32 myNetID , BATTLE_SETUP_PARAM* param )
{
  if( m_pLiveRecSender )
  {
    m_pLiveRecSender->SetMyNetID( myNetID );
    m_pLiveRecSender->SetBattleSetupParam( param );
  }
}

//@fix[fix]NMCat[4203]:[#4203 【観戦機能】対戦側と観戦側で別のBGMが再生される]
//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦送信機能レギュレーションセット
  */
//------------------------------------------------------------------
void NijiNetworkSystem::SetRegulation( Regulation* pRegulation )
{
  if( m_pLiveRecSender )
  {
    m_pLiveRecSender->SetRegulation( pRegulation );
  }
}


//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦送信機能リセット
  */
//------------------------------------------------------------------
void NijiNetworkSystem::ResetLiveRecSender( void )
{
  if( m_pLiveRecSender )
  {
    m_pLiveRecSender->ResetReload();
  }
}

//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦送信機能リロード
  */
//------------------------------------------------------------------
void NijiNetworkSystem::ReloadLiveRecSender( void )
{
  if( m_pLiveRecSender )
  {
    m_pLiveRecSender->Reload();
  }
}


//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦機能が有効か
  */
//------------------------------------------------------------------
bool NijiNetworkSystem::IsLiveRecSenderEnable( void )
{
  return m_pLiveRecSender != NULL;
}

//------------------------------------------------------------------
/**
  * @brief   選出インデックス送信
  */
//------------------------------------------------------------------
void NijiNetworkSystem::SendLiveRecSelectedIndex( s8 join_order[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ] )
{
  if( m_pLiveRecSender )
  {
    m_pLiveRecSender->SendLiveRecSelectedIndex( join_order );
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NijiNetworkSystem::StopLiveRecSender(void)
{
  if( m_pLiveRecSender )
  {
    m_pLiveRecSender->Stop();
  }
}



//------------------------------------------------------------------
/**
  * @brief   バトルスポット通信監視機能作成
  */
//------------------------------------------------------------------
void NijiNetworkSystem::CreateBattleSpotNetworkMonitor( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBSNetworkMonitor == NULL )
  {
    gfl2::heap::HeapBase * pEventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    m_pBSNetworkMonitor = GFL_NEW( pEventDeviceHeap ) BattleSpotNetworkMonitor();
  }
#endif
}

//------------------------------------------------------------------
/**
  * @brief   バトルスポット通信監視機能削除
  */
//------------------------------------------------------------------
void NijiNetworkSystem::DeleteBattleSpotNetworkMonitor( void )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SAFE_DELETE( m_pBSNetworkMonitor );
#endif
}

//------------------------------------------------------------------
/**
  * @brief   バトルスポット通信監視機能更新
  */
//------------------------------------------------------------------
void NijiNetworkSystem::UpdateBattleSpotNetworkMonitor( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBSNetworkMonitor )
  {
    m_pBSNetworkMonitor->Update();
  }
#endif
}

//------------------------------------------------------------------
/**
  * @brief   バトルスポット通信監視機リセット
  */
//------------------------------------------------------------------
void NijiNetworkSystem::ResetBattleSpotNetworkMonitor( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pBSNetworkMonitor )
  {
    m_pBSNetworkMonitor->ResetMonitor();
  }
#endif
}


#if defined(GF_PLATFORM_CTR)

//------------------------------------------------------------------
/**
  * @brief   バトルスポット通信監視機能
  */
//------------------------------------------------------------------
BattleSpotNetworkMonitor::BattleSpotNetworkMonitor()
{
  m_isTestRunning = false;

  gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  pNexManager->SetUtilityListener( this );
}
BattleSpotNetworkMonitor::~BattleSpotNetworkMonitor()
{
  gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
  pNexManager->SetUtilityListener( NULL );
}

//------------------------------------------------------------------
/**
  * @brief   バトルスポット通信監視機能リセット
  */
//------------------------------------------------------------------
void BattleSpotNetworkMonitor::ResetMonitor( void )
{ 
  m_nexP2pStationManager.Reset();
  m_isTestRunning = false;
}

//------------------------------------------------------------------
/**
  * @brief   バトルスポット通信監視機能更新
  */
//------------------------------------------------------------------
void BattleSpotNetworkMonitor::Update( void )
{
  bool isConnected = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsConnectSuccess();
  if( isConnected == false )
  {//接続中のみ監視
    return;
  }

  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->GetP2PID() == NetLib::P2P::NIJI_P2P_RNDFREE_B_LEGEND ||
      GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->GetP2PID() == NetLib::P2P::NIJI_P2P_RNDFREE_B )
  {//ロイヤルはやらない
    return;
  }

  bool result = m_nexP2pStationManager.Update();
  if( result == false && m_isTestRunning == false )
  {//切断とする
    gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
    bool testResult = pNexManager->TestConnectivity( 1000 );
    if( testResult == false )
    {//エラーにする
      NetLib::Error::NijiNetworkError error;
      error.SetResult( NetLib::Error::NijiNetworkError::ERROR_TYPE_LOGINSERVER_DISCONNECT );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
      //@fix [#4199 LAN挿抜を行い通信切断した本体でエラーアプレットを閉じなかった場合に対戦相手が待機状態で進まなくなる事がある]
      if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsConnectSuccess() )
      {
        GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart( true );
      }
    }
    else
    {//疎通テスト待ち
      m_isTestRunning = true;
    }
  }
}

//------------------------------------------------------------------
/**
  *	@brief ゲームサーバ疎通テスト結果通知
  */
//------------------------------------------------------------------
void BattleSpotNetworkMonitor::OnTestConnectivityResult( const bool isSuccess )
{
  GFL_PRINT("NetSystem::OnTestConnectivityResult %d \n", isSuccess );

  // @fix NMCat[4186]：疎通テストの実行フラグをリセットしていなかった為、１回しか疎通テストしないようになっていた。
  m_isTestRunning = false; // フラグリセット

  if( isSuccess == false )
  {//エラーにする
    NetLib::Error::NijiNetworkError error;
    error.SetResult( NetLib::Error::NijiNetworkError::ERROR_TYPE_LOGINSERVER_DISCONNECT );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
    //@fix [#4199 LAN挿抜を行い通信切断した本体でエラーアプレットを閉じなかった場合に対戦相手が待機状態で進まなくなる事がある]
    if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsConnectSuccess() )
    {
      GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart( true );
    }
  }
}

#endif


#if PM_DEBUG
//------------------------------------------------------------------
/**
* @brief  P2Pマッチング履歴クリア
*/
//------------------------------------------------------------------
void NijiNetworkSystem::SetClearCacheFlag( const bool flag )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient)->SetClearCacheFlag( flag );
#endif
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////



//------------------------------------------------------------------
/**
  * @brief   ライブ大会観戦送信機能
  */
//------------------------------------------------------------------
LiveRecSender::LiveRecSender()
{ 
  m_seq = 0; 
  m_pRecorderSaveData = NULL;
  m_myNetID = 0;
  m_pBattleSetupParam = NULL;
  m_startSeq = SEQ_START_FIEST;
  std::memset( m_join_order , 0 , sizeof(m_join_order) );
  m_isSelectIndexRequest = false;
  m_pRegulation = NULL;
};

LiveRecSender::~LiveRecSender()
{
#if defined(GF_PLATFORM_CTR)
  NetLib::Delivery::DeliveryManager* pDeliv = GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager);
  if( pDeliv )
  {
    pDeliv->Finalize();
    GFL_SAFE_DELETE( pDeliv );
  }

  GFL_SAFE_DELETE( m_pRecorderSaveData );
#endif
}

void LiveRecSender::Update()
{
#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase * pEventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  const gflnet2::InitParameter
    initParameter = {
      nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID),   //  自分のTransferdID 
      gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT ),
      gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),
      gflnet2::TYPE_P2P_WIRELESS,    /*  gflnet2::TYPE_P2P_WIRELESS,  // ネットワーク種別  */
      NetLib::P2P::NIJI_P2P_GIFT,   /*  ENUM_NIJI_P2P_ID,    // 通信別のID  */
      2,                          /*  最大接続人数                */
      false                       /*  //マイグレーション有効無効  */
  };

  const gflnet2::InitParameterFixed
    initParameterFixed  = {
      NIJI_PRODUCT_CODE,      ///< 任天堂からソフトごとに割り当ててくれるID 
      System::GetLang(),      ///< 言語番号 
      System::GetVersion(),   ///< ＲＯＭバージョン番号 
      NIJI_NET_VERSION        ///< 通信バージョン              
  };


  //終了時の処理
  //[#5578 【観戦機能】ライブ大会で、観戦機能がONのとき、子機がビーコンを送信していない]
#if 0
  if( m_pBattleSetupParam == NULL )
  {
    if( m_startSeq == SEQ_START_MAIN )
    {
      m_seq = SEQ_RELOAD;
      m_startSeq = SEQ_MAIN;
    }
    else
    if( m_seq == SEQ_INIT )
    {
      return;
    }
  }
#else
  if( m_pBattleSetupParam == NULL )
  {
    if( m_seq == SEQ_INIT )
    {
      return;
    }
  }
#endif

  switch( m_seq )
  {
  case SEQ_INIT:
    {
      m_pRecorderSaveData = GFL_NEW( pEventDeviceHeap )  ExtSavedata::BattleRecorderSaveData( pEventDeviceHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      NetLib::Delivery::DeliveryManager* pDeliv  = GFL_NEW( pEventDeviceHeap )  NetLib::Delivery::DeliveryManager();

      /*  観戦は署名なし  */
      pDeliv->SetUDSSignatureMode(false, false);

      m_seq++;
    }
  case SEQ_START_FIEST:
    {
      //@fix[fix]NMCat[4203]:[#4203 【観戦機能】対戦側と観戦側で別のBGMが再生される]
      if( m_pRegulation == NULL )
      {
        GFL_ASSERT( 0 );//@fix
        break;
      }
      //初回
      {
        m_pRecorderSaveData->CreateSaveData( m_pBattleSetupParam , m_pRegulation );
      }

      m_pBattleSetupParam->recDataSize = 0;

      GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Initialize(&initParameter, &initParameterFixed, NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wireless);

      GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->StartSend(System::ThreadPriority::DELIVERY_THREAD_PRIORITY, 100 + m_myNetID , m_pRecorderSaveData->GetBattleRecData() , 
        sizeof( ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA ) );

      m_seq = SEQ_MAIN;
    }
    break;
  case SEQ_START_SECOND://選出リスト
    GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Initialize(&initParameter, &initParameterFixed, NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wireless);
    GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->StartSend(System::ThreadPriority::DELIVERY_THREAD_PRIORITY, 150 + m_myNetID , m_join_order , 
      sizeof( m_join_order ) );
    m_seq = SEQ_MAIN;
    break;
  case SEQ_START_MAIN://録画送信
    if( m_pBattleSetupParam )
    {
      GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Initialize(&initParameter, &initParameterFixed, NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wireless);

      u32 bufferSize = m_pBattleSetupParam->recDataSize + sizeof(u64);
      void* pTempBuffer = GflHeapAllocMemoryAlign( pEventDeviceHeap->GetLowerHandle() , bufferSize , 4 );

      u32* pSeedPoint = reinterpret_cast<u32*>( pTempBuffer );
      *pSeedPoint = m_pBattleSetupParam->recRandSeed;
      //if( m_pBattleSetupParam->recDataSize > 0 )
      {
        u32* pRecPoint = &pSeedPoint[2];
        std::memcpy( pRecPoint , m_pBattleSetupParam->recBuffer , m_pBattleSetupParam->recDataSize );
      }

      GFL_PRINT( "send buffer size[%u]randSeed[%u]\n" , bufferSize,m_pBattleSetupParam->recRandSeed );
      
      GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->StartSend( System::ThreadPriority::DELIVERY_THREAD_PRIORITY , 200 + m_myNetID , pTempBuffer , bufferSize );

      GflHeapSafeFreeMemory( pTempBuffer );
    }

    m_seq = SEQ_MAIN;
    break;
  case SEQ_MAIN:
    if( GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager) )
    {
      GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Update();
    }
    break;
  case SEQ_RELOAD:
    if( GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager) )
    {
      GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->PrepareFinalize();
      if( GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->IsCompretePrepareFinalize() )
      {
        GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Finalize();
        m_seq = m_startSeq;
      }
    }
    else
    {
      m_seq = m_startSeq;
    }
    break;
  }
#endif
}

void LiveRecSender::ResetReload()
{
  Reload();
  m_startSeq = SEQ_START_FIEST;
}

void LiveRecSender::Reload()
{
  GFL_PRINT("LiveRecSender::Reload[%d]\n",m_startSeq);
  if( m_seq == SEQ_MAIN )
  {
    m_seq = SEQ_RELOAD;
    m_startSeq = SEQ_START_MAIN;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void LiveRecSender::Stop(void)
{
  SetMyNetID( 0xFF );
  SetBattleSetupParam( NULL );

  Reload();
  m_startSeq = SEQ_MAIN;

  m_pRegulation = NULL;
}


//------------------------------------------------------------------
/**
  * @brief   選出インデックス送信
  */
//------------------------------------------------------------------
void LiveRecSender::SendLiveRecSelectedIndex( s8 join_order[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ] )
{
  std::memcpy( m_join_order , join_order , sizeof(m_join_order) );
  
  Reload();
  m_startSeq = SEQ_START_SECOND;
}




GFL_NAMESPACE_END(NetLib)

#ifdef  GF_PLATFORM_CTR
//============================================================================================
/**
 * @file LocalBeaconTrader.cpp
 *
 * @author kousaka_shin
 */
//============================================================================================
#include "NetStatic/NetLib/source/Beacon/LocalBeaconTrader.h"

#include <heap/include/gfl2_heap_manager.h>
#include "system/include/HeapDefine.h"
#include "system/include/HeapSize.h"
#include <gflnet2/source/scan/thread/gflnet2_WirelessCycleThread.h>
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/NetDefine.h"
#include "NetStatic/NetLib/include/Beacon/LocalBeaconListener.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include "GameSys/include/GameManager.h"
#include "system/include/PokemonVersion.h"
#include "system/include/PokemonLanguage.h"
GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Beacon)


//------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 */
//------------------------------------------------------------------
LocalBeaconTrader::LocalBeaconTrader()
: m_isInitialized( false )
, m_pBeaconThread( NULL )
, m_beaconManager()
, m_beaconSender()
, m_initParam()
, m_pListener( NULL )
, m_isEnable( true )
{
  //-------------------------------------
  /// 通信初期化パラメータ（アプリ固有）
  //=====================================
  gflnet2::InitParameter netInitParam = {
    nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID), // 自分のTransferdID
    NetLib::NijiNetworkSystem::GetNetworkHeap()->GetLowerHandle(),// システムメモリ
    NetLib::NijiNetworkSystem::GetNetworkHeap()->GetLowerHandle(),// デバイスメモリ
    gflnet2::TYPE_P2P_WIRELESS,  // ネットワーク種別
    NetLib::P2P::NIJI_BEACON_TRADE,    // 通信別のID
    CONNECT_LIMIT_NUM,// 最大接続人数
    false,//マイグレーション有効無効
  };


  //-------------------------------------
  /// 通信初期化パラメータ（アプリ共通）
  //=====================================
  gflnet2::InitParameterFixed netInitParamFixed;
  netInitParamFixed.ProductID = NIJI_PRODUCT_CODE;    ///< 任天堂からソフトごとに割り当ててくれるID
  netInitParamFixed.LangCode  = System::GetLang();    ///< 言語番号
  netInitParamFixed.PMVer     = System::GetVersion(); ///< ＲＯＭバージョン番号
  netInitParamFixed.Version   = NIJI_NET_VERSION;     ///< 通信バージョン

  m_initParam.Initialize( &netInitParam , &netInitParamFixed );

  m_beaconManager.SetInitParam( &m_initParam );
  m_beaconManager.SetBeaconSender( &m_beaconSender );
  m_beaconManager.SetBeaconListener( this );
}


//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
LocalBeaconTrader::~LocalBeaconTrader()
{

}

//------------------------------------------------------------------
/**
  * @brief   リスナー設定
  */
//------------------------------------------------------------------
void LocalBeaconTrader::SetListener( LocalBeaconListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
  * @brief   有効化
  */
//------------------------------------------------------------------
void LocalBeaconTrader::EnableBeacon( void )
{
  m_isEnable = true;
}

//------------------------------------------------------------------
/**
  * @brief   無効化
  */
//------------------------------------------------------------------
void LocalBeaconTrader::DisableBeacon( void )
{
  m_isEnable = false;
}


//------------------------------------------------------------------
/**
  * @brief   前更新
  */
//------------------------------------------------------------------
void LocalBeaconTrader::UpdateBefore( void )
{
  if( m_isInitialized == false )
    return;

  if( m_pBeaconThread )
  {
    m_pBeaconThread->UpdateBefore();
    m_beaconManager.MainCheckRecvBeacon();
  }

  CheckAndExitThread();
}

//------------------------------------------------------------------
/**
  * @brief   後更新
  */
//------------------------------------------------------------------
void LocalBeaconTrader::UpdateAfter( void )
{
  if( m_isInitialized == false )
    return;

  if( m_pBeaconThread )
  {
    m_pBeaconThread->UpdateAfter();
  }

  CheckAndExitThread();
}

//------------------------------------------------------------------
/**
  * @brief   ビーコンデータ設定
  */
//------------------------------------------------------------------
void LocalBeaconTrader::SetBeaconData( const u8* pSendData, u32 size )
{
  m_beaconManager.CreateBeaconData();
  m_beaconManager.AddBeaconData( NIJI_BEACON_TYPE_ID_JF , 0 , pSendData , size );
}


//------------------------------------------------------------------
/**
* @brief  一時的に取得するビーコンのIDを設定
*/
//------------------------------------------------------------------
void LocalBeaconTrader::SetTempBeaconID( u8 tempID )
{
  m_beaconManager.SetTemporaryGameID( tempID );
}

//------------------------------------------------------------------
/**
  * @brief   ビーコンの開始
  */
//------------------------------------------------------------------
void LocalBeaconTrader::StartBeacon( void )
{
  m_isInitialized = true;
}

//------------------------------------------------------------------
/**
  * @brief   ビーコンの停止(trueが返るまで呼ぶ)
  */
//------------------------------------------------------------------
bool LocalBeaconTrader::UpdateStopBeacon( void )
{
  if( m_isInitialized == false )
    return true;

  if( m_pBeaconThread )
  {
    m_pBeaconThread->KillNetThread();
  }

  CheckAndExitThread();

  return ( m_pBeaconThread == NULL );
}

//------------------------------------------------------------------
/**
  * @brief   ビーコンの再開
  */
//------------------------------------------------------------------
bool LocalBeaconTrader::UpdateRestartBeacon( void )
{
  if( m_isInitialized == false )
    return false;

  if( m_isEnable == false )
    return false;

  if( NetLib::NijiNetworkSystem::GetNetworkMode() != NijiNetworkSystem::NETWORK_MODE_OFFLINE )
    return false;

  if( m_pBeaconThread == NULL )
  {
    //Wifi強制切断(念のため)
    // →必要ないはず　ichiraku 2015/12/25
    //NetLib::Wifi::ForceDisconnectWifi();

    GFL_PRINT("LocalBeaconTrader::UpdateRestartBeacon : beacon start\n");
    m_pBeaconThread = GFL_NEW( NijiNetworkSystem::GetNetworkHeap()->GetLowerHandle() ) gflnet2::NetWirelessCycleThread( &m_beaconManager , CONNECT_LIMIT_NUM , NIJI_PRODUCT_CODE , NijiNetworkSystem::GetNetworkHeap()->GetLowerHandle() );
    m_pBeaconThread->Start( gfl2::thread::Thread::THREAD_PRI_NORMAL - 1 );
    if( m_pListener )
    {
      m_pListener->OnRestartBeacon();
    }
  }

  return true;
}


//------------------------------------------------------------------
/**
  * @brief   スレッドの終了確認＆終了
  */
//------------------------------------------------------------------
void LocalBeaconTrader::CheckAndExitThread( void )
{
  if( m_pBeaconThread )
  {
    if( m_pBeaconThread->IsAlive() )
    {
      return;
    }

    GFL_SAFE_DELETE( m_pBeaconThread );
    m_beaconManager.Clear();

    GFL_PRINT("LocalBeaconTrader::UpdateRestartBeacon : beacon end\n");
  }
}

//------------------------------------------------------------------
/**
* @brief  ビーコン更新イベント
*
* @param  beacon 更新されたビーコン情報
*                ※引き渡されたデータアドレス領域は永続保障されない。
*                  ポインタを保持せず、自前のバッファにコピーすること。
*
* @param  localFriendCode ローカルフレンドコード。
*                         フレンドユーザーのみ有効な値がセットされ、
*                         その他のユーザーからのアクセス時には
*                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
*
* @param  transferdId     送信者のtransferdId
*/
//------------------------------------------------------------------
void LocalBeaconTrader::OnUpdateBeacon( const void* beacon, u64 localFriendCode, u64 transferdId )
{
  if( m_pListener )
  {
    m_pListener->OnUpdateBeacon( beacon , localFriendCode , transferdId );
  }
}

//------------------------------------------------------------------
/**
* @brief  ビーコン削除前イベント
*
* @param  beacon 更新されるビーコン情報
*                ※引き渡されたデータアドレス領域は永続保障されない。
*                  ポインタを保持せず、自前のバッファにコピーすること。
*
*
* @param  localFriendCode ローカルフレンドコード。
*                         フレンドユーザーのみ有効な値がセットされ、
*                         その他のユーザーからのアクセス時には
*                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
*
* @param  transferdId     送信者のtransferdId
*/
//------------------------------------------------------------------
void LocalBeaconTrader::OnDeleteBeacon( const void* beacon, u64 localFriendCode, u64 transferdId )
{
  if( m_pListener )
  {
    m_pListener->OnDeleteBeacon( beacon , localFriendCode , transferdId );
  }
}

GFL_NAMESPACE_END(Beacon)
GFL_NAMESPACE_END(NetLib)
#endif

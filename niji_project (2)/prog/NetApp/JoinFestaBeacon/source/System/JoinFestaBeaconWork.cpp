// ============================================================================
/*
 * @file JoinFestaBeaconWork.cpp
 * @brief ビーコンアプリのワーククラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"


#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


JoinFestaBeaconWork::JoinFestaBeaconWork( NetApp::JoinFestaBeacon::APP_PARAM* pAppParam ) 
: ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
, m_pAppParam( pAppParam )
, m_pTrainerIconUtility( NULL )
, m_pUpperView( NULL )
, m_pLowerView( NULL )
{
}


JoinFestaBeaconWork::~JoinFestaBeaconWork()
{
}


void JoinFestaBeaconWork::SetView( NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* pUpperView , NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView* pLowerView )
{
  GFL_ASSERT( pUpperView ); //@fix
  GFL_ASSERT( pLowerView ); //@fix

  m_pUpperView = pUpperView;
  m_pLowerView = pLowerView;
}

NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* JoinFestaBeaconWork::GetUpperView( void )
{
  return m_pUpperView;
}

NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView* JoinFestaBeaconWork::GetLowerView( void )
{
  return m_pLowerView;
}

Regulation::BATTLE_RULE JoinFestaBeaconWork::GetBattleRule( void )
{
  switch( m_pAppParam->type )
  {
  case TYPE_BATTLE_S:  //シングル
    return Regulation::BATTLE_RULE_SINGLE;
  case TYPE_BATTLE_D:  //ダブル
    return Regulation::BATTLE_RULE_DOUBLE;
  case TYPE_BATTLE_M:  //マルチ
    return Regulation::BATTLE_RULE_MULTI;
  case TYPE_BATTLE_R:  //ロイヤル
    return Regulation::BATTLE_RULE_ROYAL;
  }

  GFL_ASSERT( 0 );//@fix

  return Regulation::BATTLE_RULE_SINGLE;
}

NetLib::P2P::ENUM_NIJI_P2P_ID JoinFestaBeaconWork::GetP2PID( void )
{
  bool isWifi = NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI;
  NetLib::P2P::ENUM_NIJI_P2P_ID p2pID = NetLib::P2P::NIJI_P2P_TRADE_L;

  switch( m_pAppParam->type )
  {
  case TYPE_TRADE://交換
    {
      if( isWifi )
        p2pID = NetLib::P2P::NIJI_P2P_TRADE_W;///< 交換（WiFi）
      else
        p2pID = NetLib::P2P::NIJI_P2P_TRADE_L;///< 交換（ローカル)
    }
  break;
  case TYPE_BATTLE_S:
  case TYPE_BATTLE_D:
    {                    
      if( isWifi )
        p2pID = NetLib::P2P::NIJI_P2P_BATTLE_2_W;///< バトル（2人接続）（WiFi）
      else
        p2pID = NetLib::P2P::NIJI_P2P_BATTLE_2_L;///< バトル（2人接続）（ローカル）
    }
  break;
  case TYPE_BATTLE_M:
  case TYPE_BATTLE_R:
    {
      if( isWifi )
        p2pID = NetLib::P2P::NIJI_P2P_BATTLE_4_W;///< バトル（4人接続）（WiFi）
      else
        p2pID = NetLib::P2P::NIJI_P2P_BATTLE_4_L;///< バトル（4人接続）（ローカル）
    }
  break;
  default:
    GFL_ASSERT( 0 ); //@fix
  break;
  }

  return p2pID;
}

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

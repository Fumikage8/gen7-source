// ============================================================================
/*
 * @file JoinFestaBeaconWork.h
 * @brief ビーコンアプリのワーククラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_WORK_H_INCLUDE )
#define JOINFESTABEACON_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include <NetApp/JoinFestaBeacon/include/JoinFestaBeaconAppParam.h>

#include "NetApp/JoinFestaBeacon/source/View/JoinFestaBeaconUpperView.h"
#include "NetApp/JoinFestaBeacon/source/View/JoinFestaBeaconLowerView.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include "Battle/Regulation/include/Regulation.h" // バトルルール

#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaBeaconWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconWork );

private:

  static const int  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
  static const int  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 10;   // 10MB

public:
  JoinFestaBeaconWork( NetApp::JoinFestaBeacon::APP_PARAM* pAppParam );
  virtual~JoinFestaBeaconWork();



public:

  APP_PARAM* GetAppParam( void ) { return m_pAppParam; }

  void SetView( NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* pUpperView , NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView* pLowerView );

  NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* GetUpperView( void );
  NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView* GetLowerView( void );

  Regulation::BATTLE_RULE GetBattleRule( void );
  NetLib::P2P::ENUM_NIJI_P2P_ID GetP2PID( void );

  NetAppLib::Util::NetAppTrainerIconUtility* GetTrainerIconUtility() { return m_pTrainerIconUtility; }
  void SetTrainerIconUtility( NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility ){ m_pTrainerIconUtility = pTrainerIconUtility; }

private:

  APP_PARAM* m_pAppParam;

  NetAppLib::Util::NetAppTrainerIconUtility*                    m_pTrainerIconUtility;

  NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* m_pUpperView;
  NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView* m_pLowerView;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONWORK_H_INCLUDE

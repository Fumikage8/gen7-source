// ============================================================================
/*
 * @file JoinFestaBeaconFrameFactory.h
 * @brief ビーコンアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTABEACON_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaBeaconFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconFrameFactory );
public:
  JoinFestaBeaconFrameFactory( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork );
  virtual~JoinFestaBeaconFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork*                m_pJoinFestaBeaconWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACON_FRAMEFACTORY_H_INCLUDE

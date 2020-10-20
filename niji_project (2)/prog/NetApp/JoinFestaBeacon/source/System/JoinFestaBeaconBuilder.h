// ============================================================================
/*
 * @file JoinFestaBeaconBuilder.h
 * @brief ビーコンアプリを構築するクラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_BUILDER_H_INCLUDE )
#define JOINFESTABEACON_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaBeaconBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconBuilder );
public:
  JoinFestaBeaconBuilder( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork );
  virtual~JoinFestaBeaconBuilder();

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();

public:

  virtual bool IsCreateNetAppMessageMenu(){ return true; }
  virtual bool IsCreateMessageUtility(){ return true; }
  virtual bool IsCreateAppToolTrainerIconRendering(){ return true; }
  virtual u32  GetTrainerIconNum(){ return 3; }
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType(){ return app::util::G2DUtil::SETUP_UPPER; }

private:
  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork*                m_pJoinFestaBeaconWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

// ============================================================================
/*
 * @file GTSBuilder.h
 * @brief GTSアプリを構築するクラスです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( GTSBUILDER_H_INCLUDE )
#define GTSBUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class GTSViewObjects;


class GTSBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSBuilder );
public:
  GTSBuilder( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork, NetAppLib::System::FrameID startFrameID );
  virtual~GTSBuilder();

  void SetDrawManager( NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) { m_pGTSViewObjects = pGTSViewObjects; }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();
  virtual u32 CreateAppRenderingManagerCount();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescriptions();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();
  virtual bool IsCreateAppToolTrainerIconRendering();
  virtual u32  GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType();

  virtual bool IsCreateAppCountryRegionNameUtility();
  virtual bool IsCreateAppCommonGrpIconData();

  virtual bool IsEnableInputExclusiveKeyMode();

private:
  NetApp::GTS::ApplicationSystem::GTSWork*              m_pGTSWork;
  NetAppLib::System::FrameID                            m_StartFrameID;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*       m_pGTSViewObjects;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSBUILDER_H_INCLUDE

// ============================================================================
/*
 * @file MiracleTradeBuilder.h
 * @brief ミラクル交換アプリを構築するクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEBUILDER_H_INCLUDE )
#define MIRACLETRADEBUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class MiracleTradeViewObjects;


class MiracleTradeBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeBuilder );
public:
  MiracleTradeBuilder(
    NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork,
    NetAppLib::System::FrameID startFrameID );
  virtual~MiracleTradeBuilder();

  void SetDrawManager( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects* pMiracleTradeViewObjects ) { m_pMiracleTradeViewObjects = pMiracleTradeViewObjects; }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();
  virtual bool IsCreateAppToolTrainerIconRendering();
  virtual u32  GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType();

private:
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*              m_pMiracleTradeWork;
  NetAppLib::System::FrameID                                              m_StartFrameID;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects*       m_pMiracleTradeViewObjects;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

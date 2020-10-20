// ============================================================================
/*
 * @file GameSyncBuilder.h
 * @brief ゲームシンクアプリを構築するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_BUILDER_H_INCLUDE )
#define GAMESYNC_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


class GameSyncViewSystemAccessor;


class GameSyncBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncBuilder );
public:
  GameSyncBuilder( NetApp::GameSync::System::GameSyncWork* pGameSyncWork );
  virtual~GameSyncBuilder();

  void SetViewSystemAccessor( NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor )
  {
    m_pGameSyncViewSystemAccessor = pGameSyncViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();


  virtual bool IsCreateMessageUtility();
  virtual bool IsCreateNetAppMessageMenu();
  virtual bool IsCreateAppCommonGrpIconData();
  virtual bool IsCreateAppToolTrainerIconRendering();
  virtual u32  GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType();

private:
  NetApp::GameSync::System::GameSyncWork*                m_pGameSyncWork;
  NetApp::GameSync::System::GameSyncViewSystemAccessor*  m_pGameSyncViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

// ============================================================================
/*
 * @file JoinFestaPlayerListSelectBuilder.h
 * @brief プレイヤー単体選択アプリを構築するクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_BUILDER_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectViewSystemAccessor;


class JoinFestaPlayerListSelectBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectBuilder );
public:
  JoinFestaPlayerListSelectBuilder( NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork );
  virtual~JoinFestaPlayerListSelectBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor* pJoinFestaPlayerListSelectViewSystemAccessor )
  {
    m_pJoinFestaPlayerListSelectViewSystemAccessor = pJoinFestaPlayerListSelectViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();
  virtual bool IsCreateNetAppMessageMenu();
  virtual bool IsCreateAppCountryRegionNameUtility();
  virtual bool IsCreateAppCommonGrpIconData();

  virtual bool IsCreateAppToolTrainerIconRendering();
  virtual u32  GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType();

  virtual bool IsEnableInputExclusiveKeyMode();

private:
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork*                m_pJoinFestaPlayerListSelectWork;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor*  m_pJoinFestaPlayerListSelectViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

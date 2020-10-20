// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptBuilder.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)を構築するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_BUILDER_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptWork.h"




GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
  struct  APP_PARAM;

GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectForScriptViewSystemAccessor;


class JoinFestaPlayerListSelectForScriptBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptBuilder );
public:
  JoinFestaPlayerListSelectForScriptBuilder( NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork, const APP_PARAM& rParam);
  virtual~JoinFestaPlayerListSelectForScriptBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor )
  {
    m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor = pJoinFestaPlayerListSelectForScriptViewSystemAccessor;
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
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork*                 m_pJoinFestaPlayerListSelectForScriptWork;
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor*   m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor;
  NetAppLib::System::FrameID                                                                                  m_StartFrameID;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

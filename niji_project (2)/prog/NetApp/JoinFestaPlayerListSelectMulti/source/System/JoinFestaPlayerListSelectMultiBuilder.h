// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiBuilder.h
 * @brief プレイヤー複数選択アプリを構築するクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_BUILDER_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectMultiViewSystemAccessor;


class JoinFestaPlayerListSelectMultiBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiBuilder );
public:
  JoinFestaPlayerListSelectMultiBuilder( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork );
  virtual~JoinFestaPlayerListSelectMultiBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor* pJoinFestaPlayerListSelectMultiViewSystemAccessor )
  {
    m_pJoinFestaPlayerListSelectMultiViewSystemAccessor = pJoinFestaPlayerListSelectMultiViewSystemAccessor;
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
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork*                m_pJoinFestaPlayerListSelectMultiWork;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor*  m_pJoinFestaPlayerListSelectMultiViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

// ============================================================================
/*
 * @file JoinFestaPlayerDetailBuilder.h
 * @brief 詳細アプリを構築するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_BUILDER_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerDetailViewSystemAccessor;


class JoinFestaPlayerDetailBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailBuilder );
public:
  JoinFestaPlayerDetailBuilder( NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork );
  virtual~JoinFestaPlayerDetailBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor* pJoinFestaPlayerDetailViewSystemAccessor )
  {
    m_pJoinFestaPlayerDetailViewSystemAccessor = pJoinFestaPlayerDetailViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();
  virtual bool IsCreateAppCountryRegionNameUtility();
  virtual bool IsCreateAppCommonGrpIconData();

  virtual bool IsCreateAppToolTrainerIconRendering();
  virtual u32  GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType();

  virtual bool IsEnableInputExclusiveKeyMode();

private:
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork*                m_pJoinFestaPlayerDetailWork;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor*  m_pJoinFestaPlayerDetailViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

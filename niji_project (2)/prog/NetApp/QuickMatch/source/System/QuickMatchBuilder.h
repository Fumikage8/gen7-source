// ============================================================================
/*
 * @file QuickMatchBuilder.h
 * @brief クイックマッチアプリを構築するクラスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_BUILDER_H_INCLUDE )
#define QUICKMATCH_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)


class QuickMatchBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchBuilder );
public:
  QuickMatchBuilder( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork );
  virtual~QuickMatchBuilder();

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
  virtual u32  GetTrainerIconNum(){ return 1; }
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType(){ return app::util::G2DUtil::SETUP_UPPER; }
  

private:
  NetApp::QuickMatch::System::QuickMatchWork*                m_pQuickMatchWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

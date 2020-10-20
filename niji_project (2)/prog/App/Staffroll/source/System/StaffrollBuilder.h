// ============================================================================
/*
 * @file StaffrollBuilder.h
 * @brief スタッフロールアプリを構築するクラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_BUILDER_H_INCLUDE )
#define STAFFROLL_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "App/Staffroll/source/System/StaffrollWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)


class StaffrollBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( StaffrollBuilder );
public:
  StaffrollBuilder( App::Staffroll::System::StaffrollWork* pStaffrollWork );
  virtual~StaffrollBuilder();

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility(){ return true; }

  //  レンダー環境データを作成返す
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescriptions();

private:
  enum
  {
    RENDER_DESCRIPTION_NORMAL = 0,
    RENDER_DESCRIPTION_MAX
  };

  App::Staffroll::System::StaffrollWork*                m_pWork;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

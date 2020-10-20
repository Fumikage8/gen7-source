// ============================================================================
/*
 * @file JoinFestaSubScreenBuilder.h
 * @brief ジョインフェスタ下画面を構築するクラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_BUILDER_H_INCLUDE )
#define JOINFESTASUBSCREEN_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h" // 本番用のパス

// 前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)
class JoinFestaSubScreenParentView;
GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

//------------------------------------------------

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class JoinFestaSubScreenBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenBuilder );
public:
  JoinFestaSubScreenBuilder( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork );
  virtual~JoinFestaSubScreenBuilder();

  void SetParentView( NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) { m_pJoinFestaSubScreenParentView = pJoinFestaSubScreenParentView; }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  // MessageUtilytyを使用する
  virtual bool IsCreateMessageUtility();

  // UIViewマネージャのボタン監視モード
  virtual bool IsEnableInputExclusiveKeyMode(){ return true; }

  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* GetWork();

private:
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork*              m_pJoinFestaSubScreenWork;
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView*     m_pJoinFestaSubScreenParentView;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_BUILDER_H_INCLUDE

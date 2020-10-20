// ============================================================================
/*
 * @file JoinFestaSubScreenFrameFactory.h
 * @brief ジョインフェスタ下画面のフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTASUBSCREEN_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"

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



class JoinFestaSubScreenFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenFrameFactory );
public:
  JoinFestaSubScreenFrameFactory(
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView,
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pWork);
  virtual~JoinFestaSubScreenFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView*       m_pJoinFestaSubScreenParentView;
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork*  m_pWork;       //! ワーク

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_FRAMEFACTORY_H_INCLUDE

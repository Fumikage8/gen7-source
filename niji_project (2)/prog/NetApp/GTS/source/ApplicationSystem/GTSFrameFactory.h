// ============================================================================
/*
 * @file GTSFrameFactory.h
 * @brief GTSアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( GTSFRAMEFACTORY_H_INCLUDE )
#define GTSFRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class GTSViewObjects;


class GTSFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSFrameFactory );
public:
  GTSFrameFactory( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork, NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::GTS::ApplicationSystem::GTSWork*              m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*       m_pGTSViewObjects;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSFRAMEFACTORY_H_INCLUDE

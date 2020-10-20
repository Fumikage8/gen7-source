// ============================================================================
/*
 * @file ConnectionBuilder.h
 * @brief インターネット接続アプリを構築するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_BUILDER_H_INCLUDE )
#define CONNECTION_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


class ConnectionViewSystemAccessor;


class ConnectionBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionBuilder );
public:
  ConnectionBuilder( NetApp::Connection::System::ConnectionWork* pConnectionWork );
  virtual~ConnectionBuilder();

  void SetViewSystemAccessor( NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor )
  {
    m_pConnectionViewSystemAccessor = pConnectionViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();

  // トレーナーアイコンを使う
  virtual bool IsCreateAppToolTrainerIconRendering();
  // トレーナーアイコンを使う個数
  virtual u32  GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType();

private:
  NetApp::Connection::System::ConnectionWork*                m_pConnectionWork;
  NetApp::Connection::System::ConnectionViewSystemAccessor*  m_pConnectionViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE

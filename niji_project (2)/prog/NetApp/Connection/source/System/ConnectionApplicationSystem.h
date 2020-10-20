// ============================================================================
/*
 * @file ConnectionApplicationSystem.h
 * @brief インターネット接続アプリのアプリケーションシステムです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_APPLICATIONSYSTEM_H_INCLUDE )
#define CONNECTION_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)

class ConnectionWork;
class ConnectionBuilder;
class ConnectionViewList;
class ConnectionViewObjects;
class ConnectionViewSystemAccessor;

class ConnectionApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionApplicationSystem );

public:
  ConnectionApplicationSystem();
  virtual~ConnectionApplicationSystem();

  void Build( NetApp::Connection::System::ConnectionWork* pConnectionWork, NetApp::Connection::System::ConnectionBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::Connection::System::ConnectionWork*                  m_pConnectionWork;
  NetApp::Connection::System::ConnectionViewList*              m_pConnectionViewList;
  NetApp::Connection::System::ConnectionViewObjects*           m_pConnectionViewObjects;
  NetApp::Connection::System::ConnectionViewSystemAccessor*    m_pConnectionViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)


#endif // CONNECTION_APPLICATIONSYSTEM_H_INCLUDE

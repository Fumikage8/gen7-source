// ============================================================================
/*
 * @file ConnectionFrameFactory.h
 * @brief インターネット接続アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_FRAMEFACTORY_H_INCLUDE )
#define CONNECTION_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


class ConnectionViewSystemAccessor;


class ConnectionFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionFrameFactory );
public:
  ConnectionFrameFactory(
    NetApp::Connection::System::ConnectionWork* pConnectionWork,
    NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor );
  virtual~ConnectionFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::Connection::System::ConnectionWork*                m_pConnectionWork;
  NetApp::Connection::System::ConnectionViewSystemAccessor*  m_pConnectionViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_FRAMEFACTORY_H_INCLUDE

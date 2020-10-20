// ============================================================================
/*
 * @file ConnectionViewSystemAccessor.h
 * @brief インターネット接続アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define CONNECTION_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)

class ConnectionViewList;
class ConnectionViewObjects;

class ConnectionViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionViewSystemAccessor );

public:

  ConnectionViewSystemAccessor(
    NetApp::Connection::System::ConnectionViewList* pConnectionViewList,
    NetApp::Connection::System::ConnectionViewObjects* pConnectionViewObjects );

  virtual~ConnectionViewSystemAccessor();

  NetApp::Connection::System::ConnectionViewList*  GetConnectionViewList() { return m_pConnectionViewList; }
  NetApp::Connection::System::ConnectionViewObjects*  GetConnectionViewObjects() { return m_pConnectionViewObjects; }

private:

  NetApp::Connection::System::ConnectionViewList*  m_pConnectionViewList;
  NetApp::Connection::System::ConnectionViewObjects*  m_pConnectionViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)


#endif // CONNECTION_VIEWSYSTEMACCESSOR_H_INCLUDE

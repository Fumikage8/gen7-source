// ============================================================================
/*
 * @file ConnectionViewObjects.h
 * @brief インターネット接続アプリのViewクラスを持っているクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_VIEWOBJECTS_H_INCLUDE )
#define CONNECTION_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)

GFL_NAMESPACE_BEGIN(View)
class ConnectionUpperView;
class ConnectionLowerView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class ConnectionWork;

class ConnectionViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionViewObjects );
public:
  ConnectionViewObjects();
  virtual~ConnectionViewObjects();

  void CreateView( NetApp::Connection::System::ConnectionWork* pConnectionWork );

  void InputDisableAll();

  bool IsDrawing();

  NetApp::Connection::View::ConnectionUpperView*            GetConnectionUpperView() { return m_pConnectionUpperView; }
  NetApp::Connection::View::ConnectionLowerView*            GetConnectionLowerView() { return m_pConnectionLowerView; }

private:

  NetApp::Connection::View::ConnectionUpperView*            m_pConnectionUpperView;
  NetApp::Connection::View::ConnectionLowerView*            m_pConnectionLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)


#endif // CONNECTION_VIEWOBJECTS_H_INCLUDE

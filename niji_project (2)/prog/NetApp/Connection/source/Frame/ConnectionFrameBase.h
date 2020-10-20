// ============================================================================
/*
 * @file ConnectionFrameBase.h
 * @brief インターネット接続アプリのフレームの基底クラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_FRAMEBASE_H_INCLUDE )
#define CONNECTION_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"
#include "NetApp/Connection/source/System/ConnectionViewList.h"
#include "NetApp/Connection/source/System/ConnectionViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)

GFL_NAMESPACE_BEGIN(System)
class ConnectionViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class ConnectionFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionFrameBase );
public:
  ConnectionFrameBase(
    NetApp::Connection::System::ConnectionWork* pConnectionWork,
    NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor );
  virtual~ConnectionFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::Connection::System::ConnectionWork*            m_pConnectionWork;
  NetApp::Connection::System::ConnectionViewList*        m_pConnectionViewList;
  NetApp::Connection::System::ConnectionViewObjects*     m_pConnectionViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_FRAMEBASE_H_INCLUDE

// ============================================================================
/*
 * @file ConnectionViewSystemAccessor.cpp
 * @brief インターネット接続アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


ConnectionViewSystemAccessor::ConnectionViewSystemAccessor(
  NetApp::Connection::System::ConnectionViewList* pConnectionViewList,
  NetApp::Connection::System::ConnectionViewObjects* pConnectionViewObjects ) :
  m_pConnectionViewList( pConnectionViewList ),
  m_pConnectionViewObjects( pConnectionViewObjects )
{
}


ConnectionViewSystemAccessor::~ConnectionViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

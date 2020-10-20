// ============================================================================
/*
 * @file ConnectionFrameBase.cpp
 * @brief インターネット接続アプリのフレームの基底クラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/Frame/ConnectionFrameBase.h"
#include "NetApp/Connection/source/System/ConnectionViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


ConnectionFrameBase::ConnectionFrameBase(
  NetApp::Connection::System::ConnectionWork* pConnectionWork,
  NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pConnectionWork ),
  m_pConnectionWork( pConnectionWork ),
  m_pConnectionViewList( pConnectionViewSystemAccessor->GetConnectionViewList() ),
  m_pConnectionViewObjects( pConnectionViewSystemAccessor->GetConnectionViewObjects() )
{
}


ConnectionFrameBase::~ConnectionFrameBase()
{
}


void ConnectionFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pConnectionViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

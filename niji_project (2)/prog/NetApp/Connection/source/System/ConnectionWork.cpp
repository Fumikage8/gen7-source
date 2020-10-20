// ============================================================================
/*
 * @file ConnectionWork.cpp
 * @brief インターネット接続アプリのワーククラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


ConnectionWork::ConnectionWork( gfl2::heap::HEAPID heapId , u32 appHeapSize , u32 deviceHeapSize ) :
  ApplicationWorkBase( heapId, appHeapSize, deviceHeapSize ),
  m_eBootMode( NetApp::Connection::BOOT_MODE_INTERNET_CONNECTION ),
  m_bIsCanceled( false ),
  m_eFatalErrorNo( NetEvent::FatalError::FATAL_ERROR_NO_NET_ERROR ),
  m_bIsConnected( false ),
  m_pTrainerIconUtility(NULL)
{
}


ConnectionWork::~ConnectionWork()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

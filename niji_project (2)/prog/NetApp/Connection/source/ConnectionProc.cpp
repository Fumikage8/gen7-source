// ============================================================================
/*
 * @file ConnectionProc.cpp
 * @brief インターネット接続アプリのプロッククラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/include/ConnectionProc.h"
#include "NetApp/Connection/source/System/ConnectionBuilder.h"
#include "NetApp/Connection/source/System/ConnectionApplicationSystem.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)


ConnectionProc::ConnectionProc( NetApp::Connection::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pAppResult( NULL ),
  m_pConnectionWork( NULL ),
  m_pApplicationSystem( NULL ),
  m_FatalErrorEventParam()
{
}


ConnectionProc::~ConnectionProc()
{
}


gfl2::proc::Result ConnectionProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
      // フィールド中に起動することを考慮して一律 EVENT_DEVICEのヒープを使用する。
      u32 heapID = HEAPID_EVENT_DEVICE;
      u32 appHeapSize = NetApp::Connection::CONNECTION_APP_HEAP_SIZE;
      u32 appDeviceHeapSize = NetApp::Connection::CONNECTION_APP_DEVICE_HEAP_SIZE;
      if( m_AppParam.bootMode == BOOT_MODE_INTERNET_CONNECTION || m_AppParam.bootMode == BOOT_MODE_CHANGE_INTERNET_MODE )
      {//インターネット接続時はAPP_DEVICEヒープ&ヒープサイズ増やす
        heapID = HEAPID_APP_DEVICE;
        appHeapSize = NetApp::Connection::CONNECTION_APP_HEAP_SIZE_CONNECT;
        appDeviceHeapSize = NetApp::Connection::CONNECTION_APP_DEVICE_HEAP_SIZE_CONNECT;
      }

	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( heapID );
      m_pConnectionWork = GFL_NEW( pDevHeap ) NetApp::Connection::System::ConnectionWork( heapID , appHeapSize , appDeviceHeapSize );
      m_pConnectionWork->SetBootMode( static_cast<NetApp::Connection::BootMode>( m_AppParam.bootMode ) );
      m_pConnectionWork->SetFatalErrorNo( m_FatalErrorEventParam.eFatalErrorNo );

      app::util::Heap* pAppHeap = m_pConnectionWork->GetAppHeap();
      NetApp::Connection::System::ConnectionBuilder netAppSkeletonBuilder( m_pConnectionWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::Connection::System::ConnectionApplicationSystem();
      m_pApplicationSystem->Build( m_pConnectionWork, &netAppSkeletonBuilder );
      SetSubSeq( INIT_SEQUENCE_RESOURCE_LOADING );
    }
    break;

    case INIT_SEQUENCE_RESOURCE_LOADING:
    {
      if( !m_pApplicationSystem->Loading() )
      {
        result = gfl2::proc::RES_FINISH;
      }
    }
    break;
  }

  return result;
}


gfl2::proc::Result ConnectionProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pAppResult )
    {
      m_pAppResult->bIsCanceled = m_pConnectionWork->IsCanceled();
    }

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pConnectionWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result ConnectionProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void ConnectionProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void ConnectionProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

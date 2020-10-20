// ============================================================================
/*
 * @file JoinFestaBeaconProc.cpp
 * @brief ビーコンアプリのプロッククラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/include/JoinFestaBeaconProc.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconBuilder.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconApplicationSystem.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)


JoinFestaBeaconProc::JoinFestaBeaconProc( NetApp::JoinFestaBeacon::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pJoinFestaBeaconWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaBeaconProc::~JoinFestaBeaconProc()
{
}


gfl2::proc::Result JoinFestaBeaconProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaBeaconWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork( m_pAppParam );

      app::util::Heap* pAppHeap = m_pJoinFestaBeaconWork->GetAppHeap();
      NetApp::JoinFestaBeacon::System::JoinFestaBeaconBuilder netAppSkeletonBuilder( m_pJoinFestaBeaconWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaBeacon::System::JoinFestaBeaconApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaBeaconWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaBeaconProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaBeaconWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaBeaconProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

void JoinFestaBeaconProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  m_pApplicationSystem->PreDraw();
}

void JoinFestaBeaconProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

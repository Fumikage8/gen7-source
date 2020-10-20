// ============================================================================
/*
 * @file JoinFestaPlayerListShowProc.cpp
 * @brief 閲覧アプリのプロッククラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/include/JoinFestaPlayerListShowProc.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowBuilder.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)


JoinFestaPlayerListShowProc::JoinFestaPlayerListShowProc( NetApp::JoinFestaPlayerListShow::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pJoinFestaPlayerListShowWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaPlayerListShowProc::~JoinFestaPlayerListShowProc()
{
}


gfl2::proc::Result JoinFestaPlayerListShowProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaPlayerListShowWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork();
      m_pJoinFestaPlayerListShowWork->SetAppParam(&m_AppParam);

      app::util::Heap* pAppHeap = m_pJoinFestaPlayerListShowWork->GetAppHeap();
      NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowBuilder netAppSkeletonBuilder( m_pJoinFestaPlayerListShowWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaPlayerListShowWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaPlayerListShowProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );
    GFL_SAFE_DELETE( m_pJoinFestaPlayerListShowWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaPlayerListShowProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaPlayerListShowProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaPlayerListShowProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

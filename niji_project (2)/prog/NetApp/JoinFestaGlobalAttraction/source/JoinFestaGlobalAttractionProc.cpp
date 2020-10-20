// ============================================================================
/*
 * @file JoinFestaGlobalAttractionProc.cpp
 * @brief グローバルアトラクションアプリのプロッククラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/include/JoinFestaGlobalAttractionProc.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionBuilder.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionApplicationSystem.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)


JoinFestaGlobalAttractionProc::JoinFestaGlobalAttractionProc( NetApp::JoinFestaGlobalAttraction::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pJoinFestaGlobalAttractionWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaGlobalAttractionProc::~JoinFestaGlobalAttractionProc()
{
}


gfl2::proc::Result JoinFestaGlobalAttractionProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaGlobalAttractionWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork();

      app::util::Heap* pAppHeap = m_pJoinFestaGlobalAttractionWork->GetAppHeap();
      NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionBuilder netAppSkeletonBuilder( m_pJoinFestaGlobalAttractionWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaGlobalAttractionWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaGlobalAttractionProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaGlobalAttractionWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaGlobalAttractionProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaGlobalAttractionProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaGlobalAttractionProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

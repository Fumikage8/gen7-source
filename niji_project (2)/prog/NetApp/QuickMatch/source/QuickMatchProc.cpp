// ============================================================================
/*
 * @file QuickMatchProc.cpp
 * @brief クイックマッチアプリのプロッククラスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/include/QuickMatchProc.h"
#include "NetApp/QuickMatch/source/System/QuickMatchBuilder.h"
#include "NetApp/QuickMatch/source/System/QuickMatchApplicationSystem.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)


QuickMatchProc::QuickMatchProc() :
  m_pAppParam( NULL ),
  m_pQuickMatchWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


QuickMatchProc::QuickMatchProc( NetApp::QuickMatch::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pQuickMatchWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


QuickMatchProc::~QuickMatchProc()
{
  if( m_pAppParam )
  {
    m_pAppParam->m_isEnd = true;
  }
}


gfl2::proc::Result QuickMatchProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pQuickMatchWork = GFL_NEW( pDevHeap ) NetApp::QuickMatch::System::QuickMatchWork( m_pAppParam );

      app::util::Heap* pAppHeap = m_pQuickMatchWork->GetAppHeap();
      NetApp::QuickMatch::System::QuickMatchBuilder netAppSkeletonBuilder( m_pQuickMatchWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::System::QuickMatchApplicationSystem();
      m_pApplicationSystem->Build( m_pQuickMatchWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result QuickMatchProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pQuickMatchWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result QuickMatchProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

void QuickMatchProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  m_pApplicationSystem->PreDraw();
}

void QuickMatchProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

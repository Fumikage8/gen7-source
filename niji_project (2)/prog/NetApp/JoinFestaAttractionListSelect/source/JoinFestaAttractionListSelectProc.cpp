// ============================================================================
/*
 * @file JoinFestaAttractionListSelectProc.cpp
 * @brief アトラクション開催参加アプリのプロッククラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/include/JoinFestaAttractionListSelectProc.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectBuilder.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectApplicationSystem.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)


JoinFestaAttractionListSelectProc::JoinFestaAttractionListSelectProc( NetApp::JoinFestaAttractionListSelect::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pAppResult( NULL ),
  m_pJoinFestaAttractionListSelectWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaAttractionListSelectProc::~JoinFestaAttractionListSelectProc()
{
}


gfl2::proc::Result JoinFestaAttractionListSelectProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaAttractionListSelectWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork();

      app::util::Heap* pAppHeap = m_pJoinFestaAttractionListSelectWork->GetAppHeap();
      NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectBuilder netAppSkeletonBuilder( m_pJoinFestaAttractionListSelectWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaAttractionListSelectWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaAttractionListSelectProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pAppResult )
    {
      NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_pJoinFestaAttractionListSelectWork->GetSelectAttraction();
      if( pSelectAttraction )
      {
        m_pAppResult->bIsSelectedJoinFestaAttractionData = true;
      }
      else
      {
        m_pAppResult->bIsSelectedJoinFestaAttractionData = false;
      }
    }

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaAttractionListSelectWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaAttractionListSelectProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaAttractionListSelectProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaAttractionListSelectProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file JoinFestaOpenListProc.cpp
 * @brief 開催選択アプリのプロッククラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/include/JoinFestaOpenListProc.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListBuilder.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListApplicationSystem.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


JoinFestaOpenListProc::JoinFestaOpenListProc( NetApp::JoinFestaOpenList::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pAppResult( NULL ),
  m_pJoinFestaOpenListWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaOpenListProc::~JoinFestaOpenListProc()
{
}


gfl2::proc::Result JoinFestaOpenListProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaOpenListWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork();

      app::util::Heap* pAppHeap = m_pJoinFestaOpenListWork->GetAppHeap();
      NetApp::JoinFestaOpenList::System::JoinFestaOpenListBuilder netAppSkeletonBuilder( m_pJoinFestaOpenListWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaOpenList::System::JoinFestaOpenListApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaOpenListWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaOpenListProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pAppResult )
    {
      NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_pJoinFestaOpenListWork->GetSelectAttraction();
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

    GFL_SAFE_DELETE( m_pJoinFestaOpenListWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaOpenListProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaOpenListProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaOpenListProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

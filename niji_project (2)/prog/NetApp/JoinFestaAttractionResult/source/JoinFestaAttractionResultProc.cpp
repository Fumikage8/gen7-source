// ============================================================================
/*
 * @file JoinFestaAttractionResultProc.cpp
 * @brief アトラクション結果アプリのプロッククラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/include/JoinFestaAttractionResultProc.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultBuilder.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultApplicationSystem.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)


JoinFestaAttractionResultProc::JoinFestaAttractionResultProc( NetApp::JoinFestaAttractionResult::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pJoinFestaAttractionResultWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaAttractionResultProc::~JoinFestaAttractionResultProc()
{
}


gfl2::proc::Result JoinFestaAttractionResultProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaAttractionResultWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork();
      m_pJoinFestaAttractionResultWork->SetAttractionData( m_AppParam.attractionData );

      app::util::Heap* pAppHeap = m_pJoinFestaAttractionResultWork->GetAppHeap();
      NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultBuilder netAppSkeletonBuilder( m_pJoinFestaAttractionResultWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaAttractionResultWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaAttractionResultProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaAttractionResultWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaAttractionResultProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaAttractionResultProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaAttractionResultProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

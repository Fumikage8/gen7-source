// ============================================================================
/*
 * @file JoinFestaPlayerDetailProc.cpp
 * @brief 詳細アプリのプロッククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/include/JoinFestaPlayerDetailProc.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailBuilder.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailApplicationSystem.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)


JoinFestaPlayerDetailProc::JoinFestaPlayerDetailProc( NetApp::JoinFestaPlayerDetail::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pJoinFestaPlayerDetailWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaPlayerDetailProc::~JoinFestaPlayerDetailProc()
{
}


gfl2::proc::Result JoinFestaPlayerDetailProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaPlayerDetailWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork();
      m_pJoinFestaPlayerDetailWork->SetSelectPlayer( m_AppParam.playerData );

      app::util::Heap* pAppHeap = m_pJoinFestaPlayerDetailWork->GetAppHeap();
      NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailBuilder netAppSkeletonBuilder( m_pJoinFestaPlayerDetailWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaPlayerDetailWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaPlayerDetailProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaPlayerDetailWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaPlayerDetailProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaPlayerDetailProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaPlayerDetailProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

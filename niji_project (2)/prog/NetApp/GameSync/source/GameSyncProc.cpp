// ============================================================================
/*
 * @file GameSyncProc.cpp
 * @brief ゲームシンクアプリのプロッククラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/include/GameSyncProc.h"
#include "NetApp/GameSync/source/System/GameSyncBuilder.h"
#include "NetApp/GameSync/source/System/GameSyncApplicationSystem.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)


GameSyncProc::GameSyncProc( NetApp::GameSync::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pGameSyncWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


GameSyncProc::~GameSyncProc()
{
}


gfl2::proc::Result GameSyncProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pGameSyncWork = GFL_NEW( pDevHeap ) NetApp::GameSync::System::GameSyncWork();

      app::util::Heap* pAppHeap = m_pGameSyncWork->GetAppHeap();
      NetApp::GameSync::System::GameSyncBuilder netAppSkeletonBuilder( m_pGameSyncWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GameSync::System::GameSyncApplicationSystem();
      m_pApplicationSystem->Build( m_pGameSyncWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result GameSyncProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pGameSyncWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result GameSyncProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void GameSyncProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void GameSyncProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

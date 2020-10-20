// ============================================================================
/*
 * @file JoinFestaSelectWordProc.cpp
 * @brief 挨拶選択アプリのプロッククラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/include/JoinFestaSelectWordProc.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordBuilder.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordApplicationSystem.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)


JoinFestaSelectWordProc::JoinFestaSelectWordProc( NetApp::JoinFestaSelectWord::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pJoinFestaSelectWordWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaSelectWordProc::~JoinFestaSelectWordProc()
{
}


gfl2::proc::Result JoinFestaSelectWordProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaSelectWordWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork();
      m_pJoinFestaSelectWordWork->SetEditMenuID( static_cast<NetApp::JoinFestaSelectWord::EditMenuID>( m_AppParam.editMenuID ) );

      app::util::Heap* pAppHeap = m_pJoinFestaSelectWordWork->GetAppHeap();
      NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordBuilder netAppSkeletonBuilder( m_pJoinFestaSelectWordWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaSelectWordWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaSelectWordProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaSelectWordWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaSelectWordProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaSelectWordProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaSelectWordProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

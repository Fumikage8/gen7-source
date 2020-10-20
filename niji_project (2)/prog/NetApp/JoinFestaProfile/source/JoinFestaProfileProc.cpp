// ============================================================================
/*
 * @file JoinFestaProfileProc.cpp
 * @brief プロフィールアプリのプロッククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/include/JoinFestaProfileProc.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileBuilder.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileApplicationSystem.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)


JoinFestaProfileProc::JoinFestaProfileProc( NetApp::JoinFestaProfile::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pJoinFestaProfileWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaProfileProc::~JoinFestaProfileProc()
{
}


gfl2::proc::Result JoinFestaProfileProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
      app::util::Heap* pAppHeap = m_pJoinFestaProfileWork->GetAppHeap();
      NetApp::JoinFestaProfile::System::JoinFestaProfileBuilder netAppSkeletonBuilder( m_pJoinFestaProfileWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::System::JoinFestaProfileApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaProfileWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaProfileProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaProfileProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaProfileProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaProfileProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

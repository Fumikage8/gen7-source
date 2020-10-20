// ============================================================================
/*
 * @file GTSProc.cpp
 * @brief GTSプロック
 * @date 2015.03.17
 */
// ============================================================================
#include "NetApp/GTS/include/GTSProc.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSBuilder.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSApplicationSystem.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


GTSProc::GTSProc( NetApp::GTS::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pGTSWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


GTSProc::~GTSProc()
{
}


gfl2::proc::Result GTSProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
      app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

      Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();
      
      if( pGTSSaveData->GetDepositFlag() )
      {
        m_pGTSWork->SetUploadGTSIndexData( pGTSSaveData->GetUploadGtsIndexData() );
      }

      NetApp::GTS::ApplicationSystem::GTSBuilder gtsBuilder( m_pGTSWork, m_pAppParam->startFrameID );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::ApplicationSystem::GTSApplicationSystem();
      m_pApplicationSystem->Build( m_pGTSWork, &gtsBuilder );
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


gfl2::proc::Result GTSProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result GTSProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void GTSProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void GTSProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
#if PM_DEBUG
  m_pApplicationSystem->DebugDraw(displayNo);
#endif
}


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

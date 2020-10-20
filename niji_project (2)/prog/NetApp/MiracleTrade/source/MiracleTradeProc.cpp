// ============================================================================
/*
 * @file MiracleTradeProc.cpp
 * @brief ミラクル交換プロック
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/include/MiracleTradeProc.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeBuilder.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeApplicationSystem.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)


MiracleTradeProc::MiracleTradeProc( NetApp::MiracleTrade::APP_PARAM* pAppParam ) :
  m_AppParam(),
  m_pMiracleTradeWork( NULL ),
  m_pApplicationSystem( NULL )
{
  m_AppParam = *pAppParam;
}


MiracleTradeProc::~MiracleTradeProc()
{
}


gfl2::proc::Result MiracleTradeProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
      app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();
      NetApp::MiracleTrade::ApplicationSystem::MiracleTradeBuilder miracleTradeBuilder( m_pMiracleTradeWork, m_AppParam.startFrameID );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::ApplicationSystem::MiracleTradeApplicationSystem();
      m_pApplicationSystem->Build( m_pMiracleTradeWork, &miracleTradeBuilder );
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


gfl2::proc::Result MiracleTradeProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result MiracleTradeProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void MiracleTradeProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void MiracleTradeProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
#if PM_DEBUG
  m_pApplicationSystem->DebugDraw(displayNo);
#endif
}


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

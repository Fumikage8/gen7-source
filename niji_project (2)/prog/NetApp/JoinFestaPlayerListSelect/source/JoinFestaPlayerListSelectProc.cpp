// ============================================================================
/*
 * @file JoinFestaPlayerListSelectProc.cpp
 * @brief プレイヤー単体選択アプリのプロッククラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/include/JoinFestaPlayerListSelectProc.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectBuilder.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)


JoinFestaPlayerListSelectProc::JoinFestaPlayerListSelectProc( NetApp::JoinFestaPlayerListSelect::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pAppResult( NULL ),
  m_pJoinFestaPlayerListSelectWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaPlayerListSelectProc::~JoinFestaPlayerListSelectProc()
{
}


gfl2::proc::Result JoinFestaPlayerListSelectProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaPlayerListSelectWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork();

      app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectWork->GetAppHeap();
      NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectBuilder netAppSkeletonBuilder( m_pJoinFestaPlayerListSelectWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaPlayerListSelectWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaPlayerListSelectProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pAppResult )
    {
      if( m_pAppResult->pSelectJoinFestaPersonalData )
      {
        NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_pJoinFestaPlayerListSelectWork->GetSelectPlayer();
        if( pSelectPlayer )
        {
          m_pAppResult->pSelectJoinFestaPersonalData->Copy( *pSelectPlayer );
          m_pAppResult->bIsSelectedJoinFestaPersonalData = true;
        }
        else
        {
          m_pAppResult->bIsSelectedJoinFestaPersonalData = false;
        }
      }
    }

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaPlayerListSelectProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaPlayerListSelectProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaPlayerListSelectProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

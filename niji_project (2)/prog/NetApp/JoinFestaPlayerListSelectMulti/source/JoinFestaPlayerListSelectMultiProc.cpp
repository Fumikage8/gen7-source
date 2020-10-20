// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiProc.cpp
 * @brief プレイヤー複数選択アプリのプロッククラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/include/JoinFestaPlayerListSelectMultiProc.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)


JoinFestaPlayerListSelectMultiProc::JoinFestaPlayerListSelectMultiProc( NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pAppResult( NULL ),
  m_pJoinFestaPlayerListSelectMultiWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


JoinFestaPlayerListSelectMultiProc::~JoinFestaPlayerListSelectMultiProc()
{
}


gfl2::proc::Result JoinFestaPlayerListSelectMultiProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaPlayerListSelectMultiWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork();

      app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectMultiWork->GetAppHeap();
      NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiBuilder netAppSkeletonBuilder( m_pJoinFestaPlayerListSelectMultiWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaPlayerListSelectMultiWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result JoinFestaPlayerListSelectMultiProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pAppResult )
    {
      NetAppLib::JoinFestaUI::JoinFestPlayerDataList& selectPlayerList = m_pJoinFestaPlayerListSelectMultiWork->GetSelectPlayerList();
      
      if( m_pAppResult->pSelectJoinFestaPersonalData1
       && m_pAppResult->pSelectJoinFestaPersonalData2
       && m_pAppResult->pSelectJoinFestaPersonalData3
       && selectPlayerList.Size() == NetApp::JoinFestaPlayerListSelectMulti::SELECT_PLAYER_MAX
       && !m_pJoinFestaPlayerListSelectMultiWork->IsSelectCancel() )
      {
        NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = selectPlayerList.Begin();
        m_pAppResult->pSelectJoinFestaPersonalData1->Copy( (*it) ); ++it;
        m_pAppResult->pSelectJoinFestaPersonalData2->Copy( (*it) ); ++it;
        m_pAppResult->pSelectJoinFestaPersonalData3->Copy( (*it) );
        m_pAppResult->bIsSelectedJoinFestaPersonalData = true;
      }
      else
      {
        m_pAppResult->bIsSelectedJoinFestaPersonalData = false;
      }
    }

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectMultiWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaPlayerListSelectMultiProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaPlayerListSelectMultiProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaPlayerListSelectMultiProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

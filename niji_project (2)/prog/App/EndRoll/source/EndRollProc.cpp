// ============================================================================
/*
 * @file EndRollProc.cpp
 * @brief エンドロールアプリのプロッククラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/include/EndRollProc.h"
#include "App/EndRoll/source/System/EndRollBuilder.h"
#include "App/EndRoll/source/System/EndRollApplicationSystem.h"
#include "App/EndRoll/source/System/EndRollWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)


EndRollProc::EndRollProc( App::EndRoll::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pEndRollWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


EndRollProc::~EndRollProc()
{
}


gfl2::proc::Result EndRollProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pEndRollWork = GFL_NEW( pDevHeap ) App::EndRoll::System::EndRollWork();

      app::util::Heap* pAppHeap = m_pEndRollWork->GetAppHeap();
      App::EndRoll::System::EndRollBuilder netAppSkeletonBuilder( m_pEndRollWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::System::EndRollApplicationSystem();
      m_pApplicationSystem->Build( m_pEndRollWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result EndRollProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pEndRollWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result EndRollProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void EndRollProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void EndRollProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

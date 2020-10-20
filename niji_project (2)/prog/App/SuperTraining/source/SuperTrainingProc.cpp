// ============================================================================
/*
 * @file SuperTrainingProc.cpp
 * @brief すごい特訓アプリのプロッククラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/include/SuperTrainingProc.h"
#include "App/SuperTraining/source/System/SuperTrainingBuilder.h"
#include "App/SuperTraining/source/System/SuperTrainingApplicationSystem.h"
#include "App/SuperTraining/source/System/SuperTrainingWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)


  SuperTrainingProc::SuperTrainingProc( App::SuperTraining::APP_PARAM* pAppParam ) :
    m_pAppParam( pAppParam ),
    m_pSuperTrainingWork( NULL ),
    m_pApplicationSystem( NULL )
  {
  }


  SuperTrainingProc::~SuperTrainingProc()
  {
  }


  gfl2::proc::Result SuperTrainingProc::InitFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    switch( GetSubSeq() )
    {
      case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
      {
        gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

        m_pSuperTrainingWork = GFL_NEW( pDevHeap ) App::SuperTraining::System::SuperTrainingWork( m_pAppParam );

        app::util::Heap* pAppHeap = m_pSuperTrainingWork->GetAppHeap();
        App::SuperTraining::System::SuperTrainingBuilder netAppSkeletonBuilder( m_pSuperTrainingWork );
        m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::System::SuperTrainingApplicationSystem();
        m_pApplicationSystem->Build( m_pSuperTrainingWork, &netAppSkeletonBuilder );
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


  gfl2::proc::Result SuperTrainingProc::EndFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    if( !m_pApplicationSystem->End() )
    {
      GFL_SAFE_DELETE( m_pApplicationSystem );

      GFL_SAFE_DELETE( m_pSuperTrainingWork );
    
      result = gfl2::proc::RES_FINISH;
    }
    return result;
  }


  gfl2::proc::Result SuperTrainingProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
  {
    bool isExitProc = m_pApplicationSystem->Update();
    if( isExitProc )
    {
      return gfl2::proc::RES_FINISH;
    }
    return gfl2::proc::RES_CONTINUE;
  }


  void SuperTrainingProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
  {
    m_pApplicationSystem->PreDraw();
  }


  void SuperTrainingProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pApplicationSystem->Draw( displayNo );
  }


GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

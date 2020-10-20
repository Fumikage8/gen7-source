// ============================================================================
/*
 * @file BattleTreeProc.cpp
 * @brief バトルツリーアプリのプロッククラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/include/BattleTreeProc.h"
#include "App/BattleTree/source/System/BattleTreeBuilder.h"
#include "App/BattleTree/source/System/BattleTreeApplicationSystem.h"
#include "App/BattleTree/source/System/BattleTreeWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)


  BattleTreeProc::BattleTreeProc( App::BattleTree::APP_PARAM* pAppParam ) :
    m_pAppParam( pAppParam ),
    m_pBattleTreeWork( NULL ),
    m_pApplicationSystem( NULL )
  {
  }


  BattleTreeProc::~BattleTreeProc()
  {
  }


  gfl2::proc::Result BattleTreeProc::InitFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    switch( GetSubSeq() )
    {
      case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
      {
        gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

        m_pBattleTreeWork = GFL_NEW( pDevHeap ) App::BattleTree::System::BattleTreeWork( m_pAppParam );

        app::util::Heap* pAppHeap = m_pBattleTreeWork->GetAppHeap();
        App::BattleTree::System::BattleTreeBuilder BattleTreeBuilder( m_pBattleTreeWork );
        m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::System::BattleTreeApplicationSystem();
        m_pApplicationSystem->Build( m_pBattleTreeWork, &BattleTreeBuilder );

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


  gfl2::proc::Result BattleTreeProc::EndFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    if( !m_pApplicationSystem->End() )
    {
      GFL_SAFE_DELETE( m_pApplicationSystem );

      GFL_SAFE_DELETE( m_pBattleTreeWork );
    
      result = gfl2::proc::RES_FINISH;
    }
    return result;
  }


  gfl2::proc::Result BattleTreeProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
  {
    bool isExitProc = m_pApplicationSystem->Update();
    if( isExitProc )
    {
      return gfl2::proc::RES_FINISH;
    }
    return gfl2::proc::RES_CONTINUE;
  }


  void BattleTreeProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
  {
    m_pApplicationSystem->PreDraw();
  }


  void BattleTreeProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pApplicationSystem->Draw( displayNo );
  }


GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

// ============================================================================
/*
 * @file BattleRoyalResultProc.cpp
 * @brief バトルロイヤルの結果アプリのプロッククラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/include/BattleRoyalResultProc.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultBuilder.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultApplicationSystem.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)


  BattleRoyalResultProc::BattleRoyalResultProc( App::BattleRoyalResult::APP_PARAM* pAppParam ) :
    m_pAppParam( pAppParam ),
    m_pBattleRoyalResultWork( NULL ),
    m_pApplicationSystem( NULL )
  {
  }


  BattleRoyalResultProc::~BattleRoyalResultProc()
  {
  }


  gfl2::proc::Result BattleRoyalResultProc::InitFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    switch( GetSubSeq() )
    {
      case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
      {
	      gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

        m_pBattleRoyalResultWork = GFL_NEW( pDevHeap ) App::BattleRoyalResult::System::BattleRoyalResultWork( m_pAppParam );

        app::util::Heap* pAppHeap = m_pBattleRoyalResultWork->GetAppHeap();
        App::BattleRoyalResult::System::BattleRoyalResultBuilder netAppSkeletonBuilder( m_pBattleRoyalResultWork );
        m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::System::BattleRoyalResultApplicationSystem();
        m_pApplicationSystem->Build( m_pBattleRoyalResultWork, &netAppSkeletonBuilder );
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


  gfl2::proc::Result BattleRoyalResultProc::EndFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    if( !m_pApplicationSystem->End() )
    {
      GFL_SAFE_DELETE( m_pApplicationSystem );

      GFL_SAFE_DELETE( m_pBattleRoyalResultWork );
    
      result = gfl2::proc::RES_FINISH;
    }
    return result;
  }


  gfl2::proc::Result BattleRoyalResultProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
  {
    bool isExitProc = m_pApplicationSystem->Update();
    if( isExitProc )
    {
      return gfl2::proc::RES_FINISH;
    }
    return gfl2::proc::RES_CONTINUE;
  }

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  void BattleRoyalResultProc::PreDrawFunc( gfl2::proc::Manager* pManager )
  {
    GFL_UNUSED( pManager );
    m_pApplicationSystem->PreDraw();
  }


  void BattleRoyalResultProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pApplicationSystem->Draw( displayNo );
  }


GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

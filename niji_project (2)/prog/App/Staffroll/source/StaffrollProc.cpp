// ============================================================================
/*
 * @file StaffrollProc.cpp
 * @brief スタッフロールのプロッククラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/include/StaffrollProc.h"
#include "App/Staffroll/source/System/StaffrollBuilder.h"
#include "App/Staffroll/source/System/StaffrollApplicationSystem.h"
#include "App/Staffroll/source/System/StaffrollWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)


  StaffrollProc::StaffrollProc()
  {
    _Clear();
  }


  StaffrollProc::~StaffrollProc()
  {
  }

  //  初期化パラメータ
  void StaffrollProc::SetupParam( const u8 version, const bool bFirstEvent )
  {
    m_version     = version;
    m_bFirstEvent = bFirstEvent;
  }

  gfl2::proc::Result StaffrollProc::InitFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    switch( GetSubSeq() )
    {
      case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
      {
        gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

        m_pStaffrollWork = GFL_NEW( pDevHeap ) App::Staffroll::System::StaffrollWork( m_version, m_bFirstEvent );

        app::util::Heap* pAppHeap = m_pStaffrollWork->GetAppHeap();
        App::Staffroll::System::StaffrollBuilder netAppSkeletonBuilder( m_pStaffrollWork );
        m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::System::StaffrollApplicationSystem();
        m_pApplicationSystem->Build( m_pStaffrollWork, &netAppSkeletonBuilder );
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


  gfl2::proc::Result StaffrollProc::EndFunc( gfl2::proc::Manager* pProcManager )
  {
    gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

    if( !m_pApplicationSystem->End() )
    {
      GFL_SAFE_DELETE( m_pApplicationSystem );

      GFL_SAFE_DELETE( m_pStaffrollWork );
    
      result = gfl2::proc::RES_FINISH;
    }
    return result;
  }


  gfl2::proc::Result StaffrollProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
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
  void StaffrollProc::PreDrawFunc( gfl2::proc::Manager* pManager )
  {
    GFL_UNUSED( pManager );
    m_pApplicationSystem->PreDraw();
  }


  void StaffrollProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pApplicationSystem->Draw( displayNo );
  }


GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

// ============================================================================
/*
 * @file KisekaeProc.cpp
 * @brief きせかえアプリのプロッククラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/include/KisekaeProc.h"
#include "App/Kisekae/source/System/KisekaeBuilder.h"
#include "App/Kisekae/source/System/KisekaeApplicationSystem.h"
#include "App/Kisekae/source/System/KisekaeWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)


Proc::Proc( App::Kisekae::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


Proc::~Proc()
{
}


gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
      gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pWork = GFL_NEW( pDevHeap ) App::Kisekae::System::Work( m_pAppParam );

      app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
      App::Kisekae::System::Builder netAppSkeletonBuilder( m_pWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::System::ApplicationSystem();
      m_pApplicationSystem->Build( m_pWork, &netAppSkeletonBuilder );
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


gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void Proc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );
  m_pApplicationSystem->PreDraw();
}

gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void Proc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

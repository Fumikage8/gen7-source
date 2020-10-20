#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestProc.cpp
 * @date    2016/10/26 20:03:33
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Test/FStudioDecoTest/include/FStudioDecoTestProc.h>

#include "FStudioDecoTestFrame.h"

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <System/include/HeapDefine.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
Proc::Proc( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pFrameManager( NULL )
  , m_pManagerHeap( NULL )
  , m_pFrame( NULL )
  , m_Seq( 0 )
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* manager )
{
  if( m_Seq == 0 )
  {
    gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    {
      m_pManagerHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap, 0x1000, gfl2::heap::HEAP_TYPE_EXP, false, "FSDecoTestManagerHeap" );

      m_pAppHeap = GFL_NEW( m_pManagerHeap ) app::util::Heap();
      m_pAppHeap->LocalHeapCreate( heap, heap, APP_SYS_HEAP_SIZE, APP_DEV_HEAP_SIZE );

      m_pFrameManager = GFL_NEW( m_pManagerHeap ) applib::frame::Manager( m_pManagerHeap );
      m_pFrameManager->SetAppFrameListener( this );

      m_pAppRenderingManager = GFL_NEW( m_pManagerHeap ) app::util::AppRenderingManager();

      gfl2::fs::AsyncFileManager* afm = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
      app::util::AppRenderingManager::Description desc;
      {
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      }
      m_pAppRenderingManager->StartAsyncCreate( desc, m_pAppHeap, NULL, afm );
    }

    m_Seq++;
  }
  if( m_Seq == 1 )
  {
    m_pAppRenderingManager->Update();

    if( m_pAppRenderingManager->IsCreated() )
    {
      m_Seq++;
    }
  }
  if( m_Seq == 2 )
  {
    CreateFrame();
    m_Seq = 0;
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* manager )
{
  if( m_Seq == 0 )
  {
    if( m_pAppRenderingManager->CanDestroy() )
    {
      GFL_DELETE( m_pAppRenderingManager );
      m_Seq++;
    }
  }
  if( m_Seq == 1 )
  {
    // 終了処理が完了するまで待つ
    if( m_pFrameManager->End() == false )
    {
      GFL_DELETE( m_pFrameManager );
      m_Seq++;
    }
  }
  if( m_Seq == 2 )
  {
    GFL_SAFE_DELETE( m_pAppHeap );
    GFL_DELETE_HEAP( m_pManagerHeap );
    m_Seq++;
  }
  if( m_Seq == 3 )
  {
    m_Seq = 0;
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}



//----------------------------------------------------------------------
// @brief   更新・描画
//----------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* manager )
{
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

void Proc::PreDrawFunc( gfl2::proc::Manager* manager )
{
  m_pAppRenderingManager->PreDraw();
}

void Proc::DrawFunc( gfl2::proc::Manager* manager, gfl2::gfx::CtrDisplayNo no )
{
  m_pAppRenderingManager->Update();

  m_pFrameManager->Draw( no );
  m_pAppRenderingManager->Draw( no );
}



//----------------------------------------------------------------------
// @brief   リスナー
//----------------------------------------------------------------------
void Proc::EndFuncFinish( applib::frame::CellSuper* endCell )
{
  if( endCell == m_pFrame )
  {
    m_pFrame = NULL;
  }
}



//----------------------------------------------------------------------
// @brief   生成関数群
//----------------------------------------------------------------------
void Proc::CreateFrame( void )
{
  m_pFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, Frame>( this->GetFrameManager() );
  {
    m_pFrame->SetAppHeap( m_pAppHeap );
    m_pFrame->SetAppRenderingManager( m_pAppRenderingManager );
  }
}

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG

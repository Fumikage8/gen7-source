#if defined(GF_PLATFORM_CTR)

//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @author ito_isao  -->k.ohno
 *
 */
//====================================================================== 


#include "Layout/include/gfl2_Layout.h"
//#include <gflib.h>
//#include <gfl_Debug.h>
//#include <gfl_Heap.h>
//#include <gfl_UI.h>
//#include <gfl_Grp.h>
//#include <grp/util/gfl_UtilDrawUtil.h>
//#include <ui/ctr/gfl_UI_CTR_DeviceManager.h>
#include "MovieProc.h"
//#include "print/system_font.h"
#include "MobiclipScene.h"
#include "heap/include/gfl2_Heap.h"
#include "System/include/HeapDefine.h"

#include <util/include/gfl2_GLHeapAllocator.h>
#include <System/include/GflUse.h>
#include "renderer/MobiclipRenderingPipeLine.h"

#include <Debug/include/gfl2_DebugTick.h>
#include "DebugMovie.h"


namespace movie{
/*
  // レイアウトシステムのセットアップデータ
  const gfl::grp::g2d::LytSysSetUp LAYOUT_SYSTEM_SETUP = 
  {
    128, // 管理リソース最大数
    10,  // 管理レイアウトワーク最大数
  }; 
	*/
void MovieProc::InitLocal()
{
	// シーンの初期化
	this->CreateScene<MobiclipScene>();

}

void MovieProc::EndLocal()
{
}


GFL_DEBUG_TICK_SETUP(END, 10);

//-------------------------------------------------------------------------------------
/**
 * @brief 初期化処理
 */
//-------------------------------------------------------------------------------------
gfl2::proc::Result MovieProcBase::InitFunc(gfl2::proc::Manager* pProcManager)
{
  enum 
  {
    SEQ_INIT,
    SEQ_WAIT_ASYNC,
  };

  switch( this->GetSubSeq())
  {
  case SEQ_INIT:
    {
#if PM_DEBUG
      ::movie::DebugMovie::InitializeSingleton();
#endif //PM_DEBUG
      GFL_DEBUG_TICK_SET_FLAG(END,true);
      // アロケータ
      gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      gfl2::heap::HeapBase *dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_heap_alloc      = GFL_NEW(heap) gfl2::heap::NwAllocator(heap);
      m_device_alloc    = GFL_NEW(dev_heap) gfl2::heap::NwAllocator(dev_heap);
      m_gl_device_alloc = GFL_NEW(dev_heap) gfl2::util::GLHeapAllocator(dev_heap);

      // UI
      GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      m_deviceManager = game_manager->GetUiDeviceManager();
      m_button = m_deviceManager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STANDARD );

      // グラフィック
      //  m_graphics = game_manager->GetGraphicsSystem();
      // m_graphics->InitializeApplicationMemory(m_device_alloc);

      //  m_layout = GFL_NEW( heap ) gfl2::grp::g2d::LytSys( &LAYOUT_SYSTEM_SETUP, m_heap_alloc, m_device_alloc ); 

      m_renderingPipeLine = GFL_NEW(dev_heap) movie::renderer::MobiclipRenderingPipeLine();
      movie::renderer::MobiclipRenderPath* renderPath = m_renderingPipeLine->GetMobiclipRenderPath();

      movie::renderer::MobiclipRenderPath::StartAsyncCreateFileOpne(dev_heap,GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager());
      for( int i = 0; i < m_renderingPipeLine->NumMobiclipRenderPath(); ++i ) {
        renderPath[i].StartAsyncCreate( dev_heap, m_gl_device_alloc, GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager() );
      }
      movie::renderer::MobiclipRenderPath::StartAsyncCreateFileClose(dev_heap,GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager());

      this->AddSubSeq();
    }
    return gfl2::proc::RES_CONTINUE;
  case SEQ_WAIT_ASYNC:
    {
      bool isEnd = true;
      movie::renderer::MobiclipRenderPath* renderPath = m_renderingPipeLine->GetMobiclipRenderPath();
      for( int i = 0; i < m_renderingPipeLine->NumMobiclipRenderPath(); ++i ) {
        isEnd &= renderPath[i].UpdateAsyncCreate();
      }
      isEnd &= movie::renderer::MobiclipRenderPath::UpdateAsyncCreateFile(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager());
      if(isEnd)
      {
        this->InitLocal();
        m_next = RET_NONE;
        return gfl2::proc::RES_FINISH;
      }
    }
  }

	return gfl2::proc::RES_CONTINUE;
}
//-------------------------------------------------------------------------------------
/**
 * @brief 終了処理
 */
//-------------------------------------------------------------------------------------
gfl2::proc::Result MovieProcBase::EndFunc(gfl2::proc::Manager* pProcManager)
{
	GFL_UNUSED(pProcManager);

  GFL_DEBUG_TICK_START(END, 0, "END");
	if( m_scene.Exit() ) {

    if( m_returnCode )
    {
      *m_returnCode = m_next;
    }

    this->EndLocal();

    GFL_SAFE_DELETE(m_renderingPipeLine);

    //	GFL_DELETE(m_layout);

    //  m_graphics->FinalizeApplicationMemory();
    //	m_graphics = NULL;


    GFL_SAFE_DELETE(m_gl_device_alloc);
    GFL_SAFE_DELETE(m_device_alloc);
    GFL_SAFE_DELETE(m_heap_alloc);

    GFL_DEBUG_TICK_END(END, 0);
    GFL_DEBUG_TICK_DUMP_ALL(END);
#if PM_DEBUG
    ::movie::DebugMovie::FinalizeSingleton();
#endif //PM_DEBUG
    return gfl2::proc::RES_FINISH;
  }

  GFL_DEBUG_TICK_END(END, 0);
  GFL_DEBUG_TICK_DUMP_ALL(END);
  return gfl2::proc::RES_CONTINUE;
}

//-------------------------------------------------------------------------------------
/**
 * @brief メイン処理
 */
//-------------------------------------------------------------------------------------
gfl2::proc::Result MovieProcBase::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  int code = m_scene.Update(mMovieNo);
	switch(code)
  {
  case RET_NONE:
    break;
  case RET_DECIDE:
    m_next = RET_DECIDE;
    if( m_isDecide )
    {
      *m_isDecide = true;
    }

    return gfl2::proc::RES_FINISH;
  default:
    m_next = code;
    return gfl2::proc::RES_FINISH;
  }
	//this->Draw();

  m_renderingPipeLine->Update();

	return gfl2::proc::RES_CONTINUE;
}


void MovieProcBase::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
	GFL_UNUSED(pManager);
/*	
	gfl::grp::RenderTarget* upper_frame_buffer[3];

	upper_frame_buffer[0] = m_graphics->GetCurrentFrameBuffer(gfl::grp::DISPLAY_UPPER);
	upper_frame_buffer[1] = m_graphics->GetCurrentFrameBuffer(gfl::grp::DISPLAY_UPPER_RIGHT);
	
	for(int i = 0; i < 2; ++i)
	{
//		if( m_graphics->BindCurrentCommandList(upper_frame_buffer[i]) )    ///どの画面に書き込むかを指示している>>>
    {
      gfl::grp::util::DrawUtil::BeginRender(upper_frame_buffer[i]);

      // ここで描画する
      m_scene.DrawTop(m_graphics);

      gfl::grp::util::DrawUtil::EndRender();
    }
	}
	
	upper_frame_buffer[2] = m_graphics->GetCurrentFrameBuffer(gfl::grp::DISPLAY_LOWER);
	
//	if( m_graphics->BindCurrentCommandList(upper_frame_buffer[2]) )
  {
    gfl::grp::util::DrawUtil::BeginRender(upper_frame_buffer[2]);

    // ここで描画する
    m_scene.DrawBottom(m_graphics);

    gfl::grp::util::DrawUtil::EndRender();
  }
*/

  bool ret = false;
  {
    if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
    {
      ret = m_scene.DrawTop();
    }
    else if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      ret = m_scene.DrawBottom();
    }
  }

  {
    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[displayNo];
 
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );

    gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetIdentity();
    gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetIdentity();
    m_renderingPipeLine->SetProjectionMatrix(projMatrix);
    m_renderingPipeLine->SetViewMatrix(viewMatrix);
    
    m_renderingPipeLine->SetDrawBuffer( displayNo );
    m_renderingPipeLine->SetRenderTarget( pBackBuffer );
    m_renderingPipeLine->SetDepthStencil( pDepthBuffer );
    m_renderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

    m_renderingPipeLine->Execute();

    if( ret )
    {
      if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
      {
        m_scene.DrawEnd();
      }
    }
  }
}


}

#endif //defined(GF_PLATFORM_CTR)

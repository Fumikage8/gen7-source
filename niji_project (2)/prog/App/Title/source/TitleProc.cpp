//==============================================================================
/**
 @file    TitleProc.h
 @date    2015.05.08
 @note
          タイトルプロセス
 */
//==============================================================================
#include "App/Title/include/TitleProc.h"

// niji
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"
#include "Applib/include/Frame/AppFrameManager.h"
#include "Applib/include/Util/AppRenderingManager.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(title)

  TitleProc::TitleProc() :
  GameProc()
  {
    _Clear();
  }

  TitleProc::~TitleProc()
  {}

  gfl2::proc::Result TitleProc::InitFunc( gfl2::proc::Manager* pManager )
  {
    static const s32 HEAP_MEM_SIZE = 0xf0000;
    static const s32 DEV_MEM_SIZE  = 1024 * 1024 * 30;

    static const s32 LOCAL_HEAP_SIZE = 1024 * 600;
    static const s32 LOCAL_DEVICE_HEAP_SIZE  = 1024 * 1024 * 20;

    enum
    {
      SEQ_RENDER_CREATE = 0,
      SEQ_RENDER_CREATE_WAIT,
      SEQ_FRAME_CREATE
    };

    s32 seq = GameSys::GameProc::GetSubSeq();

    if( seq == SEQ_RENDER_CREATE )
    {
      //  タイトル全体が使用するメモリを確保
      {
        gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pHeapMem = GFL_CREATE_LOCAL_HEAP( parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  
        gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pDevMem = GFL_CREATE_LOCAL_HEAP( parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
      }

      // ローカルヒープ生成
      {
        m_pUtilheap = GFL_NEW( m_pDevMem ) app::util::Heap();
        m_pUtilheap->LocalHeapCreate( m_pHeapMem, m_pDevMem, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
      }

      // パイプライン生成
      {
        m_pNijiAllocatorForEdgeMapTexture = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) System::nijiAllocator( m_pUtilheap->GetDeviceHeap() );

        app::util::AppRenderingManager::Description appRenderingManagerDesc;
        //  下画面はレイアウトー＞ポケモンモデルを描画
        appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;

        //  上画面はレイアウト描画
        appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

        m_pRenderingManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppRenderingManager();

        m_pRenderingManager->StartAsyncCreate(
            appRenderingManagerDesc,
            m_pUtilheap,
            m_pNijiAllocatorForEdgeMapTexture,
            GameSys::GameProc::GetGameManager()->GetAsyncFileManager()
        );
      }

      ++seq;
    }
    else
    if( seq == SEQ_RENDER_CREATE_WAIT )
    {
      m_pRenderingManager->Update();
      if( m_pRenderingManager->IsCreated() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_FRAME_CREATE )
    {
      //  フレーム管理生成
      {
        m_pFrameManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) applib::frame::Manager( m_pUtilheap->GetDeviceHeap() );
      }

      // フレーム生成
      {
        m_pTitleFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, TitleFrame >( m_pFrameManager );

        {
          TitleFrame::APP_PARAM::_tag_in_param* pInParam  = &m_titleFrameAppParam.in_param;
          pInParam->p_parent_app_heap         = m_pUtilheap->GetSystemHeap();
          pInParam->p_parent_dev_heap         = m_pUtilheap->GetDeviceHeap();
          pInParam->p_rendering_manager       = m_pRenderingManager;
        }

        m_pTitleFrame->Initialize( &m_titleFrameAppParam );
      }

      m_pFrameManager->SetAppFrameListener( this );

      return gfl2::proc::RES_FINISH;
    }

    GameSys::GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TitleProc::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
  }

  gfl2::proc::Result TitleProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
    {
      return gfl2::proc::RES_FINISH;
    }

    m_pRenderingManager->Update();

    return gfl2::proc::RES_CONTINUE;
  }

  void TitleProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    m_pRenderingManager->PreDraw();
  }

  void TitleProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pFrameManager->Draw( displayNo );
    m_pRenderingManager->Draw( displayNo );
  }

  gfl2::proc::Result TitleProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    enum
    {
      SEQ_FRAME_END = 0,
      SEQ_RENDERING_DESTROY,
      SEQ_END
    };

    s32 seq = GameSys::GameProc::GetSubSeq();
    if( seq == SEQ_FRAME_END )
    {
      // 終了処理が完了するまで待つ
      if( m_pFrameManager->End() == false )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_RENDERING_DESTROY )
    {
      if( m_pRenderingManager->CanDestroy() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_END )
    {
      GFL_SAFE_DELETE( m_pFrameManager );

      GFL_SAFE_DELETE( m_pRenderingManager );
      GFL_SAFE_DELETE( m_pNijiAllocatorForEdgeMapTexture );
      GFL_SAFE_DELETE( m_pUtilheap );

      if( m_pDevMem )
      {
        GFL_DELETE_HEAP(m_pDevMem);
        m_pDevMem = NULL;
      }

      if( m_pHeapMem )
      {
        GFL_DELETE_HEAP(m_pHeapMem);
        m_pHeapMem = NULL;
      }

      return gfl2::proc::RES_FINISH;
    }

    GameSys::GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  EndFuncが完了した直後に呼び出される関数
    */
  //------------------------------------------------------------------
  //  override
  void TitleProc::EndFuncFinish( applib::frame::CellSuper* pEndCell )
  {
    if( pEndCell == m_pTitleFrame )
    {
      //  タイトルフレーム終了

      //  結果を受け取る
      if( m_pAppParam != NULL )
      {
        if( m_titleFrameAppParam.out_param.b_ret_debug_seq == true )
        {
          //  デバッグシーケンスに戻れとの結果を設定
          m_pAppParam->out_param.ret  = PROC_RET_DEBUG_SEQ;
        }
        else
        if( m_titleFrameAppParam.out_param.b_ret_del_save_seq == true )
        {
          //  セーブ破棄に遷移
          m_pAppParam->out_param.ret  = PROC_RET_DELETE_SAVE;
        }
        else
        {
          m_pAppParam->out_param.ret  = PROC_RET_GAME_SEQ;
        }
      }

      m_pTitleFrame = NULL;
    }
  }

GFL_NAMESPACE_END(title)
GFL_NAMESPACE_END(app)

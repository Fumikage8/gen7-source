//======================================================================
/**
* @file DendouDemoProc.cpp
* @date 2016/02/26 19:17:46
* @author nagihashi_toru(移植), Hiroyuki Nakamura
* @brief 殿堂デモProc
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// niji
#include <System/include/HeapDefine.h>
#include <GameSys/include/GameManager.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <System/include/nijiAllocator.h>

// DendouDemo
#include "App/DendouDemo/include/DendouDemoProc.h"
#include "DendouDemoFrame.h"
#include "DendouDemoDebug.h"


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
* @namespace	app::dendou_demo
* @brief			殿堂入りデモ
*/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
  namespace dendou_demo {

    //============================================================================================
    //	定数定義
    //============================================================================================

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		コンストラクタ
    *
    * @param		prm		外部設定パラメータ
    */
    //--------------------------------------------------------------------------------------------
    Proc::Proc( APP_PARAM * prm ) :
      GameSys::GameProc(),
      pParam(prm),
      appHeap(),
      pFrameManagerHeap(NULL),
      pNijiAllocator(NULL),
      pAppRenderingManager(NULL),
      pFrameManager(NULL),
      pFrame(NULL)
    {
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		デストラクタ
    */
    //--------------------------------------------------------------------------------------------
    Proc::~Proc()
    {
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		Proc初期化処理
    *
    * @param		proc_manager		gfl2::proc::Manager
    *
    * @return	gfl2::proc::Result
    */
    //--------------------------------------------------------------------------------------------
    gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager * proc_manager )
    {
      enum
      {
        SEQ_CREATE,
        SEQ_WAIT,
      };

      switch(GetSubSeq())
      {
      case SEQ_CREATE:
        CreateHeap();
        CreateRenderingManager();
        AddSubSeq();
        // fallthrogh
      case SEQ_WAIT:
        if( WaitRenderingManager() )
        {
          CreateFrame();
          CallMainFrame();
#if PM_DEBUG
          Debug::InitializeSingleton();
#endif
          return gfl2::proc::RES_FINISH;
        }
        return gfl2::proc::RES_CONTINUE;
      default:
        GFL_ASSERT(0);
        return gfl2::proc::RES_FINISH;
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		Proc終了処理
    *
    * @param		proc_manager		gfl2::proc::Manager
    *
    * @return	gfl2::proc::Result
    */
    //--------------------------------------------------------------------------------------------
    gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager * )
    {
      if( DeleteFrame() == false ) 
      {
        return gfl2::proc::RES_CONTINUE;
      }
      
      if( DeleteRenderingManager() == false )
      {
        return gfl2::proc::RES_CONTINUE;
      }

#if PM_DEBUG
      Debug::FinalizeSingleton();
#endif
      DeleteHeap();
      return gfl2::proc::RES_FINISH;
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		Procアップデート処理
    *
    * @param		proc_manager		gfl2::proc::Manager
    *
    * @return	gfl2::proc::Result
    */
    //--------------------------------------------------------------------------------------------
    gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager * )
    {
      if( pFrameManager )
      {
        if( pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
        {
          return gfl2::proc::RES_FINISH;
        }
      }

      if( pAppRenderingManager )
      {
        pAppRenderingManager->Update();
      }

      return gfl2::proc::RES_CONTINUE;
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		ProcPre描画処理
    *
    * @param		proc_manager		gfl2::proc::Manager
    *
    * @return	none
    */
    //--------------------------------------------------------------------------------------------
    void Proc::PreDrawFunc( gfl2::proc::Manager* proc_manaer )
    {
      if( pFrame )
      {
        if( pFrame->CanDraw() ==false )
        {
          return;
        }
      }
      if( pAppRenderingManager )
      {
          pAppRenderingManager->PreDraw();
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		Proc描画処理
    *
    * @param		proc_manager		gfl2::proc::Manager
    * @param		display_no  		gfl2:gfx:CtrDisplayNo
    *
    * @return	none
    */
    //--------------------------------------------------------------------------------------------
    void Proc::DrawFunc( gfl2::proc::Manager* proc_manager, gfl2::gfx::CtrDisplayNo display_no )
    {
      if( pFrame )
      {
        if( pFrame->CanDraw() ==false )
        {
          return;
        }
      }

      if( pFrameManager )
      {
        pFrameManager->Draw(display_no);
      }

      if( pAppRenderingManager )
      {
        pAppRenderingManager->Draw(display_no);
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		システムヒープ作成
    *
    * @param		none
    *
    * @return	none
    */
    //--------------------------------------------------------------------------------------------
    void Proc::CreateHeap(void)
    {
      //UtilHeap作成
      {
        static const u32 HEAP_MEM_SIZE = 0x5000;//0xf0000;
        static const u32 DEV_MEM_SIZE  = 0x2500000;//0x1E00000;//1024 * 1024 * 30;//0x2000000
        gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
        gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        NAGI_PRINT("totalAllocatableSize : %d\n", parentHeapMem->GetTotalAllocatableSize());//0x9D20
        NAGI_PRINT("sysTotalAllocatableSize : %d\n", parentDevMem->GetTotalAllocatableSize());//0x1783FD20
        appHeap.LocalHeapCreate( parentDevMem, parentDevMem, HEAP_MEM_SIZE, DEV_MEM_SIZE ); 
      }

      //FrameManager専用の作成
      {
        static const u32 FRAME_MANAGER_HEAP_SIZE = 0x1000;
        pFrameManagerHeap = GFL_CREATE_LOCAL_HEAP( appHeap.GetSystemHeap(), FRAME_MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if PM_DEBUG
#if CTR_TARGET
#if GFL_HIO_ENABLE
        //appHeap.GetSystemHeap()->StartHioDump();
        //appHeap.GetDeviceHeap()->StartHioDump();
#endif
#endif
#endif
      }

      // NIJIアロケータの生成
      {
        pNijiAllocator = GFL_NEW( appHeap.GetDeviceHeap() ) System::nijiAllocator( appHeap.GetDeviceHeap() );
      }

    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief		システムヒープ削除
    *
    * @param		none
    *
    * @return	none
    */
    //--------------------------------------------------------------------------------------------
    void Proc::DeleteHeap(void)
    {
      GFL_SAFE_DELETE( pNijiAllocator );
      GFL_DELETE_HEAP( pFrameManagerHeap );
      appHeap.Delete();
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief	  レンダリングパイプライン作成
    */
    //--------------------------------------------------------------------------------------------
    void Proc::CreateRenderingManager(void)
    {
      if( pAppRenderingManager != NULL )
      {
        GFL_ASSERT(0);
        return;
      }

      // レンダリングマネージャ
      pAppRenderingManager = GFL_NEW(appHeap.GetDeviceHeap()) app::util::AppRenderingManager();

      app::util::AppRenderingManager::Description desc;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].pokeModelNumMax = 4;

      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::NONE;

      pAppRenderingManager->StartAsyncCreate(
        desc,
        &appHeap,
        pNijiAllocator,
        this->GetGameManager()->GetAsyncFileManager()
        );
    }
    //--------------------------------------------------------------------------------------------
    /**
    * @brief	  レンダリングパイプライン作成待ち
    */
    //--------------------------------------------------------------------------------------------
    bool Proc::WaitRenderingManager(void)
    {
      if( pAppRenderingManager == NULL )
      {
        GFL_ASSERT(0);
        return true;
      }
      pAppRenderingManager->Update();
      return pAppRenderingManager->IsCreated();
    }
    //--------------------------------------------------------------------------------------------
    /**
    * @brief	  レンダリングパイプライン破棄
    * @return	true 破棄完了 false 破棄できなかった
    */
    //--------------------------------------------------------------------------------------------
    bool Proc::DeleteRenderingManager(void)
    {
      if( pAppRenderingManager == NULL )
      {
        //削除済み
        return true;
      }

      if( !pAppRenderingManager->CanDestroy() )
      {
        return false;
      }

      GFL_SAFE_DELETE( pAppRenderingManager );
      return true;
    }
    //--------------------------------------------------------------------------------------------
    /**
    * @brief	  Frameの作成
    */
    //--------------------------------------------------------------------------------------------
    void Proc::CreateFrame(void)
    {
      if( pFrameManager != NULL )
      {
        GFL_ASSERT(0);
        return;
      }

      // フレーム
      pFrameManager = GFL_NEW(appHeap.GetSystemHeap()) applib::frame::Manager(pFrameManagerHeap);
    }
    //--------------------------------------------------------------------------------------------
    /**
    * @brief	  Frameの破棄
    * @return	true Frameの破棄 false 破棄できなかった
    */
    //--------------------------------------------------------------------------------------------
    bool Proc::DeleteFrame(void)
    {
      if(pFrameManager == NULL )
      {
        return true;
      }

      if(pFrameManager->End() == false)
      {
        GFL_SAFE_DELETE(pFrameManager);
        return true;
      }

      return false;
    }
    //--------------------------------------------------------------------------------------------
    /**
    * @brief	  MainFrame実行
    */
    //--------------------------------------------------------------------------------------------
    void Proc::CallMainFrame(void)
    {
      //最初のフレーム登録
      pFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, app::dendou_demo::Frame>(pFrameManager);
      pFrame->Setup( pParam, &appHeap, this->GetGameManager(), pAppRenderingManager );
    }

  }	// dendou_demo
}	// app

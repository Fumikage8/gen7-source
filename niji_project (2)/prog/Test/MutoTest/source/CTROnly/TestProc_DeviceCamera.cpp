
#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
//==============================================================================
/**
 * @file   TestProc_DeviceCamera.cpp
 * @date   2015/07/28 Tue. 19:09:06
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <heap/include/gfl2_Heap.h>
#include  <ui/include/gfl2_UI.h>
#include  <str/include/gfl2_Str.h>
#include  <System/include/HeapDefine.h>
#include  <System/include/ThreadPriority.h>
#include  <system/include/GflUse.h>
#include  "AppLib/include/Util/app_util_FileAccessor.h"
#include  <print/include/SystemFont.h>

/*  resource  */
#include  <arc_def_index/arc_def.h>
#include  <arc_index/message.gaix>
#include  <arc_index/MutoTest.gaix>
#include  <niji_conv_header/app/test/muto_test/MutoTest.h>
#include  <niji_conv_header/app/test/muto_test/MutoTest_anim_list.h>
#include  <niji_conv_header/app/test/muto_test/MutoTest_pane.h>

#include  <debug/include/gfl2_DebugLogWindow.h>
#include  <DeviceCamera/include/gfl2_DeviceCamera.h>

#include  <NetStatic/NetAppLib/include/Debug/NetAppDebugPokeToolSimpleParam.h>


#include  "../../include/CTROnly/TestProc_DeviceCamera.h"


// =============================================================================
/**/
//==============================================================================
#define TESTPROC_DEVICECAMERA_DEBUGLOGWINDOW_TEST


namespace {

  enum
  {
    UPDATESEQ_Main,


    UPDATESEQ_Exit,

    UPDATESEQ_
  };
}




namespace Test      {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================



CameraImageView::CameraImageView(app::util::Heap * pHeap, app::util::AppRenderingManager* pRenderManager, void* pLayoutBuffer)
  : app::ui::UIView(pHeap)
  , m_pRenderManager(pRenderManager)
{
  /*  setupLayoutSystem  */
  {
    const app::util::G2DUtil::LytResData  lytResDataTable[] = {
      { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_NONE },
    };

    const app::util::G2DUtil::LytwkData   layoutWorkTable[] =
    {
      {
        0,                                        //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_MUTOTEST_DEVICECAMERATEST_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_MUTOTEST_DEVICECAMERATEST___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
        pLayoutBuffer,                            //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                     //!< 非表示ペインに行列計算を行うか
        app::util::G2DUtil::SETUP_UPPER,          //!< ディスプレイ
        true                                      //!< 表示設定
      },
    };


    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( pHeap, false );

    Create2D(
      pHeap,                            /*  app::util::Heap*    */
      NULL,                             /*  gfl2::lyt::LytSys                 (NULLなら内部で新規に生成する。既存のものを使用する場合にはここで指定する)  */
      1,                                /*  u32                               layoutを扱う数（capacity）。内部ワークの数  */
      lytResDataTable,                  /*  app::util::G2DUtil::LytResData*   リソーステーブル        */
      GFL_NELEMS(lytResDataTable),      /*  u32                               リソーステーブル用素数  */
      layoutWorkTable,                  /*  app::util::G2DUtil::LytwkData*    ワークテーブル          */
      GFL_NELEMS(layoutWorkTable),      /*  u32                               ワークテーブル用素数    */
      app::ui::UIView::UNUSE_MESSAGE_DATA
    );
  }
}


CameraImageView::~CameraImageView()
{
  Delete2D();
}


void CameraImageView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pRenderManager, displayNo, 0);
}











//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_DeviceCamera::TestProc_DeviceCamera(void)
  : GameSys::GameProc()
  , m_Heap()
  , m_TextDrawer()
  , m_UpdateSeq(UPDATESEQ_Main)
  , m_pLayoutBuffer(NULL)
  , m_pRenderManager(NULL)
  , m_pCameraImageView(NULL)
{
  
  {
    PokeTool::SimpleParam   simpleParam;
    u32                     count  = 1;


    NetAppLib::Debug::PokeToolSimpleParamDebugger::SetFirst(simpleParam);
    while(NetAppLib::Debug::PokeToolSimpleParamDebugger::SetNext(simpleParam))
    {
      ++count;
    }
  }
  
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_DeviceCamera::~TestProc_DeviceCamera()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result  TestProc_DeviceCamera::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;

  CreateHeap();
  m_TextDrawer.Initialize(&m_Heap);


  /*  rendermanager  */
  {
    app::util::AppRenderingManager::Description  desc;


    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder  = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

    m_pRenderManager  = GFL_NEW(m_Heap.GetDeviceHeap()) app::util::AppRenderingManager();
    
    
    m_pRenderManager->SyncCreate(
        desc,                             // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
        &m_Heap,                          // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
        NULL,                             // VRAM限定。RenderOrderTypeにPOKEMODELを書いているときは必須、そうでないときはNULLでよい。
        NULL                              // ファイルマネージャ
    );
  }


  /* layout */
  {
    app::util::FileAccessor::FileOpenSync( ARCID_MUTO_TEST, m_Heap.GetDeviceHeap() );
    app::util::FileAccessor::FileLoadSync(
      ARCID_MUTO_TEST,
      GARC_MutoTest_MutoTest_applyt_COMP,
      &m_pLayoutBuffer,
      m_Heap.GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT
    );
    app::util::FileAccessor::FileCloseSync( ARCID_MUTO_TEST );
  }

  /*  view  */
  {
    m_pCameraImageView  = GFL_NEW(m_Heap.GetDeviceHeap()) CameraImageView(&m_Heap, m_pRenderManager, m_pLayoutBuffer);
  }


  /*  DeviceCamera  */
  {
    using namespace gfl2::devicecamera;

    DeviceCamera::Config  config;


//    config.pAppHeap           = m_Heap.GetDeviceHeap();
    config.pAppHeap           = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
    config.pDevHeap           = m_Heap.GetDeviceHeap();

    config.camType            = CAM_OUT_R;

    config.resolution.Set(512, 384);
    config.resolution.SetCropCenter(640, 480);

    config.photoMode.type     = MODE_LETTER;

    config.trimming.isEnable  = false;


    config.exposure.isEnable    = true;
    config.exposure.value       = 2;
    config.exposure.rectUnit[0] = 0;
    config.exposure.rectUnit[1] = 0;
    config.exposure.rectUnit[2] = 16;
    config.exposure.rectUnit[3] = 16;


    config.whiteBalance.type        = WB_AUTO;
    config.whiteBalance.rectUnit[0] = 0;
    config.whiteBalance.rectUnit[1] = 0;
    config.whiteBalance.rectUnit[2] = 16;
    config.whiteBalance.rectUnit[3] = 16;

    config.sharpness.value      = 2;
    config.flip.type            = FLIP_NONE;
    config.frameRate.type       = FPS_30;
    config.effect.type          = EFFECT_NONE;
    config.contrast.type        = CONTRAST_HIGH;
    config.lensCorrection.type  = LC_NORMAL;
    config.noiseFilter.isEnable = false ;
    config.outImageSize[0]      = 512;
    config.outImageSize[1]      = 512;
    config.outputFormat         = Y2RThread::OUTPUT_RGB_24;
    config.alpha                = 0xff;


    GFL_NEW(m_Heap.GetDeviceHeap()) DeviceCamera();

    DeviceCamera* pDeviceCamera = GFL_SINGLETON_INSTANCE(DeviceCamera);

    pDeviceCamera->Setup(config);
    pDeviceCamera->Start();
  }


  /*  debugwin  */
#ifdef  TESTPROC_DEVICECAMERA_DEBUGLOGWINDOW_TEST
  {
    //gfl2::debug::InitDebugLogWindow(m_Heap.GetDeviceHeap(), m_Heap.GetGLHeapAllocator(), );
    gfl2::debug::LogWindow_SetFont(print::SystemFont_GetFont());
    gfl2::debug::LogWindow_SetEnable(true, gfl2::debug::LOGWIN_ID_POKE_MEMORY);
    gfl2::debug::LogWindow_SetDispFlg(true, gfl2::debug::LOGWIN_ID_POKE_MEMORY);
  }
#endif

  result	= gfl2::proc::RES_FINISH;
  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result  TestProc_DeviceCamera::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  if(m_pCameraImageView->IsDrawing()) return gfl2::proc::RES_CONTINUE;


  /*  DeviceCamera  */
  {
    using namespace gfl2::devicecamera;

    DeviceCamera*   pDeviceCamera  = GFL_SINGLETON_INSTANCE(DeviceCamera);

    pDeviceCamera->Cleanup();
    GFL_SAFE_DELETE(pDeviceCamera);
  }


  /*
    view
  */
  {
    GFL_SAFE_DELETE(m_pCameraImageView);
  }


  /*
    rendermanager
  */
  {
    GFL_SAFE_DELETE(m_pRenderManager);
  }


  /*  layout  */
  {
    GflHeapFreeMemory( m_pLayoutBuffer );
  }


#ifdef  TESTPROC_DEVICECAMERA_DEBUGLOGWINDOW_TEST
  /*    */
#endif


  m_TextDrawer.Finalize();
  DeleteHeap();
  result	= gfl2::proc::RES_FINISH;


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result  TestProc_DeviceCamera::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	      result	        = gfl2::proc::RES_CONTINUE;
  gfl2::ui::DeviceManager*  pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*         pButton         = pDeviceManager->GetButton(0);


  /*  devicecamera  */
  {
    using namespace gfl2::devicecamera;

    DeviceCamera*   pDeviceCamera  = GFL_SINGLETON_INSTANCE(DeviceCamera);

    pDeviceCamera->Update();
    pDeviceCamera->SetPicturePaneTexture(
      m_pCameraImageView->GetG2DUtil()->GetLayoutWork(0)->GetPicturePane(PANENAME_DEVICECAMERATEST_PANE_CAMDUMMYTEX),
      Y2RThread::CH_1
    );
  }

#ifdef  TESTPROC_DEVICECAMERA_DEBUGLOGWINDOW_TEST
  {
    gfl2::debug::DebugLogWindow*  pLogWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugLogWindow); 
    pLogWin->Update();

    {
      static u32        cnt       =  0;
      static const u32  breakcnt  = 150;
      
      if(!((cnt/breakcnt)%2))
      {
        gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_POKE_MEMORY, L"cnt: %4d", cnt);
      }
      else if(!(cnt%breakcnt))
      {
        gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_POKE_MEMORY, L"break %d", breakcnt);
      }
      
      
      gfl2::debug::DebugLogWindow*  pLogWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugLogWindow); 

      
      ++cnt;
      *(pLogWin->GetDispPosPointer()) = ((cnt/breakcnt/2)%2);
    }
  }
#endif


  

  switch(m_UpdateSeq)
  {
    case  UPDATESEQ_Main:
      if(false)
      {
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_UP))
      {
        gfl2::devicecamera::DeviceCamera::SleepRequest();
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))
      {
        gfl2::devicecamera::DeviceCamera::AwakeRequest();
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_X))     {
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_A))     {
        using namespace gfl2::devicecamera;

        static int  index = 0;
        
        const EffectType  effects[]  = 
        {
          EFFECT_NONE,
          EFFECT_MONO,
          EFFECT_SEPIA,
          EFFECT_NEGATIVE,
          EFFECT_NEGAFILM,
          EFFECT_SEPIA01,
        };

        DeviceCamera*   pDeviceCamera  = GFL_SINGLETON_INSTANCE(DeviceCamera);

        Effect  effect;
        effect.type = effects[index];
        pDeviceCamera->SetEffect(effect);



        pDeviceCamera->ApplyContext();
        ++index;
        index %= GFL_NELEMS(effects);
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))     m_UpdateSeq   = UPDATESEQ_Exit;

      break;

    case  UPDATESEQ_Exit:
    default:
    {
      result	= gfl2::proc::RES_FINISH;
    }
      break;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  TestProc_DeviceCamera::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  m_pRenderManager->PreDraw();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  TestProc_DeviceCamera::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pCameraImageView->DrawTree(displayNo);
  m_pRenderManager->Draw(displayNo);





  m_TextDrawer.CLS();
  m_TextDrawer.LOC(0,0);


  m_TextDrawer.PrintLn(L"[DeviceCamera]");

  m_TextDrawer.Draw(displayNo);


  /*  */
#ifdef  TESTPROC_DEVICECAMERA_DEBUGLOGWINDOW_TEST
  {
    gfl2::debug::DebugLogWindow*  pLogWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugLogWindow); 

    pLogWin->Draw(displayNo, System::GflUse::m_HandleList[ displayNo ]);
  }
#endif

}



/*
*/
void TestProc_DeviceCamera::CreateHeap( void )
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  
  const size_t            appHeapSize   = 0x00100000;
  const size_t            devHeapSize   = 0x00400000;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


/*
*/
void TestProc_DeviceCamera::DeleteHeap( void )
{
  m_Heap.Delete();
}











} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if defined(GF_PLATFORM_CTR)  */

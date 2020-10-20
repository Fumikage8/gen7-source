//==============================================================================
/**
 * @file   MutoTest_TestProc_SurfUI.cpp
 * @date   2016/11/14 Mon. 12:22:32
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
#if PM_DEBUG

// ============================================================================
/*	includes	*/
// ============================================================================
#include  <heap/include/gfl2_Heap.h>
#include  <System/include/HeapDefine.h>
#include  <System/include/GflUse.h>

#include  <App/MantainSurf/source/UI/SurfUI.h>
#include  "../include/Mutotest_TestProc_SurfUI.h"



// ============================================================================
/*  defs  */
// ============================================================================


// ============================================================================
/*  local prms  */
// ============================================================================
namespace	{
  static const int    frameMax  = 30;


  enum
  {
    STATE_Init_Initial,
    STATE_Init_Loading,
    STATE_Running,
  };

  static const char* const moduleTable[] =
  {
    "MantainSurf.cro",
  };
  static const u32         moduleTableItems  = GFL_NELEMS(moduleTable);
}


/*
  Test::MutoTest
*/
namespace Test  {
namespace MutoTest  {
// ============================================================================
/*  funcs */
// ============================================================================

//-----------------------------------------------------------------------------
/**
 *  @func   TestProc_SurfUI
 *  @brief  ctor
 */
//-----------------------------------------------------------------------------
TestProc_SurfUI::TestProc_SurfUI(void)
  : GameSys::GameProc()
  , m_Heap()
  , m_pRenderManager(NULL)
  , m_ppROModules(NULL)
  , m_State(STATE_Init_Initial)
  , m_TextDrawer()
  , m_pSurfUI(NULL)
  , m_bTrickListVisibility(false)
  , m_bExit(false)
  , m_TutorialID(0)
{
}


//-----------------------------------------------------------------------------
/**
 *  @func   ~TestProc_SurfUI
 *  @brief  dtor
 */
//-----------------------------------------------------------------------------
TestProc_SurfUI::~TestProc_SurfUI()
{
}


//-----------------------------------------------------------------------------
/**
 *  @func     InitFunc
 *  @brief    PROCの初期化を行う関数
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result	TestProc_SurfUI::InitFunc(gfl2::proc::Manager* pManager)
{
  /*
    RES_CONTINUE,		///<動作継続中
    RES_FINISH,				///<動作終了
  */
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  switch(m_State)
  {
    case STATE_Init_Initial:
    {
      CreateHeap();
      m_TextDrawer.Initialize(&m_Heap);


      /*  rendermanager  */
      {
        app::util::AppRenderingManager::Description  desc;


        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

        m_pRenderManager  = GFL_NEW(m_Heap.GetDeviceHeap()) app::util::AppRenderingManager();
        
        m_pRenderManager->SyncCreate(
            desc,                             // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
            &m_Heap,                          // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
            NULL,                             // VRAM限定。RenderOrderTypeにPOKEMODELを書いているときは必須、そうでないときはNULLでよい。
            NULL                              // ファイルマネージャ
        );
      }

      /*  surfRO  */
      {
        gfl2::ro::RoManager*          pROManager    = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager); 
        gfl2::fs::AsyncFileManager*   pFileManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

        m_ppROModules = GFL_NEW(m_Heap.GetDeviceHeap()) gfl2::ro::Module*[moduleTableItems];

        for(u32 index = 0; index < moduleTableItems; ++index)
        {
          m_ppROModules[index] = pROManager->LoadModule(pFileManager, moduleTable[index]);
          pROManager->StartModule(m_ppROModules[index]);
        }
      }

      /*  surfUI  */
      {
        gfl2::heap::HeapBase* pHeapBase = m_Heap.GetDeviceHeap();
        
        m_pSurfUI = GFL_NEW(pHeapBase)  App::MantainSurf::UI::SurfUI(pHeapBase, m_pRenderManager);
        m_pSurfUI->SetEventHandler(this);
      }

      m_State = STATE_Init_Loading;
    }
      break;

    case STATE_Init_Loading:
    {
      if(m_pSurfUI->ObserveSetup())
      {
        m_pSurfUI->SetBestScore(5000 + System::GflUse::GetPublicRand(5000));
        m_pSurfUI->Debug_EnableSE(false);
        m_State = STATE_Running;
        result  = gfl2::proc::RES_FINISH;
      }
    }
      break;

    default:
      break;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_SurfUI::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  if(m_pSurfUI && m_pSurfUI->IsDrawing())
  {
    return gfl2::proc::RES_CONTINUE;
  }



  /*  surfUI  */
  {
    GFL_SAFE_DELETE(m_pSurfUI);
  }

  /*  modules */
  {
    gfl2::ro::RoManager*  pROManager  = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager); 

    for(u32 index = 0; index < moduleTableItems; ++index)
    {
      pROManager->DisposeModule(m_ppROModules[index]);
    }
    GFL_SAFE_DELETE_ARRAY(m_ppROModules);
  }

  /*
    rendermanager
  */
  {
    GFL_SAFE_DELETE(m_pRenderManager);
  }

  m_TextDrawer.Finalize();
  DeleteHeap();

  result	= gfl2::proc::RES_FINISH;
  return result;
}


//-----------------------------------------------------------------------------
/**
  * @func UpdateFunc
  * @brief	PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//-----------------------------------------------------------------------------
gfl2::proc::Result  TestProc_SurfUI::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result        result          = gfl2::proc::RES_CONTINUE;
  gfl2::ui::DeviceManager*  pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*         pButton         = pDeviceManager->GetButton(0);

  if(m_bExit) return gfl2::proc::RES_FINISH;


  if(pButton->IsTrigger(gfl2::ui::BUTTON_R))
  {
//    const u32 trickID = System::GflUse::GetPublicRand(10);
    const u32 trickID = 11;
    const u32 score   = 1+System::GflUse::GetPublicRand(99998);
//    m_pSurfUI->ShowTrickName(static_cast<App::MantainSurf::MantainSurfAppData::SkillId>(trickID));
    m_pSurfUI->RegisterTrickResult(static_cast<App::MantainSurf::MantainSurfAppData::SkillId>(trickID), score);
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_X))
  {
    const u32 trickID = System::GflUse::GetPublicRand(10);
    const u32 score   = 0;
//    m_pSurfUI->ShowTrickName(static_cast<App::MantainSurf::MantainSurfAppData::SkillId>(trickID));
    m_pSurfUI->RegisterTrickResult(static_cast<App::MantainSurf::MantainSurfAppData::SkillId>(trickID), score);
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_Y))
  {
    const u32 trickID = System::GflUse::GetPublicRand(10);
    m_pSurfUI->ShowTrickName(static_cast<App::MantainSurf::MantainSurfAppData::SkillId>(trickID));
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    m_bTrickListVisibility  = !m_bTrickListVisibility;
    if(m_bTrickListVisibility)  m_pSurfUI->ShowTrickList();
    else                        m_pSurfUI->HideTrickList();
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))   result = gfl2::proc::RES_FINISH;
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_L))   m_pSurfUI->ShowResult();
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_UP))   m_pSurfUI->StartTutorial(static_cast<App::MantainSurf::MantainSurfAppData::TutorialID>(((m_TutorialID++) % App::MantainSurf::MantainSurfAppData::SKILL_ID_MAX)));

//  else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))   m_pSurfUI->Result_ScrollDown();

  /*  surfUI  */
  {
    m_pSurfUI->Update();
  }


  m_TextDrawer.CLS();
  m_TextDrawer.LOC(0,0);
  m_TextDrawer.Print(L"Test:SurfUI");




  return result;
}


//-----------------------------------------------------------------------------
/**
  * @func   PreDrawFunc
  * @brief  PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
//-----------------------------------------------------------------------------
void  TestProc_SurfUI::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  m_pRenderManager->PreDraw();
}


//------------------------------------------------------------------
/**
  * @func   DrawFunc
  * @brief  PROCの描画処理
  * @param  pManager   プロセス管理マネージャー
  * @param  displayNo  描画対象画面
  */
//------------------------------------------------------------------
void  TestProc_SurfUI::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
#if 0
  ../../../../../gflib2/gfx/include/gfl2_enums.h

  //! @brief CTRのディスプレイ番号
  DECLARE_ENUM_BEGIN( CtrDisplayNo )

    LEFT,           //!< 3DSの左画面 400x240
    RIGHT,          //!< 3DSの右画面 400x240
    DOWN,           //!< 3DSの下画面 320x240
    
  	NumberOf,

  DECLARE_ENUM_END( CtrDisplayNo )
#endif


  /*  surfUI  */
  {
    m_pSurfUI->Draw(displayNo);
  }
  m_pRenderManager->Draw(displayNo);

  m_TextDrawer.Draw(displayNo);
}





/*
*/
void TestProc_SurfUI::CreateHeap( void )
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  const size_t            appHeapSize   = 0x00080000;
  const size_t            devHeapSize   = 0x01000000;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


/*
*/
void TestProc_SurfUI::DeleteHeap( void )
{
  m_Heap.Delete();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TestProc_SurfUI::SurfUI_OnEvent(App::MantainSurf::UI::SurfUI* pSender, const App::MantainSurf::UI::SurfUI::IEventHandler::EventCode eventCode)
{
  if(eventCode == App::MantainSurf::UI::SurfUI::IEventHandler::EVENT_ResultEnd)
  {
    m_bExit = true;
  }
}




} /*  namespace MutoTest  */
} /*  namespace Test      */

#endif // PM_DEBUG

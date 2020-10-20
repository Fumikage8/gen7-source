//==============================================================================
/**
 * @file   MutoTest_TestProc_LunaSolDiveUI.cpp
 * @date   2016/11/29 Tue. 17:31:01
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

//#include  <App/LunaSolDive/source/UI/DiveUI.h>
#include  "./UI/DiveUI.h"
#include  "../include/Mutotest_TestProc_LunaSolDiveUI.h"



// ============================================================================
/*  defs  */
// ============================================================================


// ============================================================================
/*  local prms  */
// ============================================================================
namespace	{
  enum
  {
    STATE_Init_Initial,
    STATE_Init_Loading,
    STATE_Running,
  };
/*
  static const char* const moduleTable[] =
  {
    "MantainSurf.cro",
  };
  static const u32         moduleTableItems  = GFL_NELEMS(moduleTable);
*/
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
 *  @func   TestProc_LunaSolDiveUI
 *  @brief  ctor
 */
//-----------------------------------------------------------------------------
TestProc_LunaSolDiveUI::TestProc_LunaSolDiveUI(void)
  : GameSys::GameProc()
  , m_Heap()
  , m_pRenderManager(NULL)
  , m_ppROModules(NULL)
  , m_State(STATE_Init_Initial)
  , m_TextDrawer()
  , m_pLunaSolDiveUI(NULL)
{
}


//-----------------------------------------------------------------------------
/**
 *  @func   ~TestProc_LunaSolDiveUI
 *  @brief  dtor
 */
//-----------------------------------------------------------------------------
TestProc_LunaSolDiveUI::~TestProc_LunaSolDiveUI()
{
}


//-----------------------------------------------------------------------------
/**
 *  @func     InitFunc
 *  @brief    PROCの初期化を行う関数
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result	TestProc_LunaSolDiveUI::InitFunc(gfl2::proc::Manager* pManager)
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


        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_EFFECT;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_EFFECT;

        m_pRenderManager  = GFL_NEW(m_Heap.GetDeviceHeap()) app::util::AppRenderingManager();
        
        m_pRenderManager->SyncCreate(
            desc,                             // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
            &m_Heap,                          // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
            NULL,                             // VRAM限定。RenderOrderTypeにPOKEMODELを書いているときは必須、そうでないときはNULLでよい。
            NULL                              // ファイルマネージャ
        );
      }

      /*  RO  */
/*
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
*/
      /*  LunaSolDiveUI  */
      {
        gfl2::heap::HeapBase* pHeapBase = m_Heap.GetDeviceHeap();
        
        m_pLunaSolDiveUI = GFL_NEW(pHeapBase)  DiveUI(pHeapBase, m_pRenderManager);
      }

      m_State = STATE_Init_Loading;
    }
      break;

    case STATE_Init_Loading:
    {
      if(m_pLunaSolDiveUI->ObserveSetup())
      {
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
gfl2::proc::Result TestProc_LunaSolDiveUI::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  if(m_pLunaSolDiveUI && m_pLunaSolDiveUI->IsDrawing())
  {
    return gfl2::proc::RES_CONTINUE;
  }



  /*  LunaSolDiveUI  */
  {
    GFL_SAFE_DELETE(m_pLunaSolDiveUI);
  }

  /*  modules */
/*
  {
    gfl2::ro::RoManager*  pROManager  = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager); 

    for(u32 index = 0; index < moduleTableItems; ++index)
    {
      pROManager->DisposeModule(m_ppROModules[index]);
    }
    GFL_SAFE_DELETE_ARRAY(m_ppROModules);
  }
*/

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
gfl2::proc::Result  TestProc_LunaSolDiveUI::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result        result          = gfl2::proc::RES_CONTINUE;
  gfl2::ui::DeviceManager*  pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*         pButton         = pDeviceManager->GetButton(0);


  if(pButton->IsTrigger(gfl2::ui::BUTTON_B))     result = gfl2::proc::RES_FINISH;
//  else if(pButton->IsTrigger(gfl2::ui::BUTTON_R))     m_pLunaSolDiveUI->ShowTrick(static_cast<App::MantainSurf::MantainSurfAppData::SkillId>(System::GflUse::GetPublicRand(10)), System::GflUse::GetPublicRand(999));
//  else if(pButton->IsTrigger(gfl2::ui::BUTTON_L))     m_pLunaSolDiveUI->ShowResult();
//  else if(pButton->IsTrigger(gfl2::ui::BUTTON_Y))     m_pLunaSolDiveUI->HideResult();
//  else if(pButton->IsTrigger(gfl2::ui::BUTTON_R))     m_pLunaSolDiveUI->SetResultTotalValue(System::GflUse::GetPublicRand(65535));
//  else if(pButton->IsRepeat(gfl2::ui::BUTTON_UP))     m_pLunaSolDiveUI->Result_ScrollUp();
//  else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))   m_pLunaSolDiveUI->Result_ScrollDown();

  /*  LunaSolDiveUI  */
  {
    m_pLunaSolDiveUI->Update();
  }


  m_TextDrawer.CLS();
  m_TextDrawer.LOC(0,0);
  m_TextDrawer.Print(L"Test:LunaSolDiveUI");




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
void  TestProc_LunaSolDiveUI::PreDrawFunc( gfl2::proc::Manager* pManager )
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
void  TestProc_LunaSolDiveUI::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
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


  /*  LunaSolDiveUI  */
  {
    m_pLunaSolDiveUI->Draw(displayNo);
  }
  m_pRenderManager->Draw(displayNo);

  m_TextDrawer.Draw(displayNo);
}





/*
*/
void TestProc_LunaSolDiveUI::CreateHeap( void )
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  const size_t            appHeapSize   = 0x00080000;
  const size_t            devHeapSize   = 0x01000000;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


/*
*/
void TestProc_LunaSolDiveUI::DeleteHeap( void )
{
  m_Heap.Delete();
}




} /*  namespace MutoTest  */
} /*  namespace Test      */

#endif // PM_DEBUG

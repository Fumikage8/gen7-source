
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   TestProc_HIOView.cpp
 * @date   2016/09/06 Tue. 15:28:58
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
#include  <print/include/SystemFont.h>

/*  resource  */
#include  <arc_def_index/arc_def.h>
#include  <arc_index/message.gaix>
#include  <arc_index/MutoTest.gaix>

#include  <debug/include/gfl2_DebugLogWindow.h>

#include  "../Common/Scene3D.h"

#include  "../../include/CTROnly/TestProc_HIOView.h"


// =============================================================================
/**/
//==============================================================================

namespace {

  enum
  {
    UPDATESEQ_Main,


    UPDATESEQ_Quit,
    UPDATESEQ_Exit,

    UPDATESEQ_
  };


  static const u32  poolCapacity  = 32;


  struct  ModelGroupWork
  {
    Test::MutoTest::Scene3D*              pScene3D;
    Test::MutoTest::HIOViewModule_Model*  pModelModule;
    Test::MutoTest::EasyModel*            pEasyModel;
  };
  static const u32  ModelGroupWorkBytes = sizeof(ModelGroupWork);


  bool IsTrig(const u32 buttonID)
  {
    gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    return (pButton->IsTrigger(buttonID));
  }

  bool IsTrigA(void)      {return IsTrig(gfl2::ui::BUTTON_A);}
  bool IsTrigX(void)      {return IsTrig(gfl2::ui::BUTTON_X);}
  bool IsTrigRight(void)  {return IsTrig(gfl2::ui::BUTTON_RIGHT);}
  bool IsTrigLeft(void)   {return IsTrig(gfl2::ui::BUTTON_LEFT);}


  void  SE_OK(gfl2::debug::DebugWinItem* pItem) {pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );}

  void  Func_RemoveModel(void* _pGroupWork, gfl2::debug::DebugWinItem* pItem)
  {
    if(IsTrigA())
    {
      ModelGroupWork* pGroupWork  = static_cast<ModelGroupWork*>(_pGroupWork);

      pGroupWork->pScene3D->RemoveModel_U(*(pGroupWork->pEasyModel));
      pGroupWork->pModelModule->DisposeModel(pGroupWork->pEasyModel);
      pGroupWork->pEasyModel->RemoveDebug();    /*  workが消えるので最後に  */
    }
  }
}




namespace Test      {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_HIOView::TestProc_HIOView(void)
  : GameSys::GameProc()
  , m_Heap()
  , m_Scene3D()
  , m_TextDrawer()
  , m_UpdateSeq(UPDATESEQ_Main)
    /*  HIOUtil */
  , m_HIOUtil_Camera()
  , m_HIOUtil_Light()
  , m_HIOUtil_Model(32)
  , m_HIOUtil_Motion()
  /*    */
  , m_FrameCount(0)
  /*  debug  */
  , m_pGroup_Root(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_HIOView::~TestProc_HIOView()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result  TestProc_HIOView::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;

  CreateHeap();

  /*  scene  */
  m_Scene3D.Initialize(&m_Heap);
  m_TextDrawer.Initialize(&m_Heap);

  /*  HIO  */
  {
    gfl2::heap::HeapBase* pHeapBase = m_Heap.GetDeviceHeap();

    /*  camera  */
    m_HIOUtil_Camera.Initialize(pHeapBase);
    m_HIOUtil_Camera.GetEasyCamera().Create(&m_Heap);

    /*  light  */
    m_HIOUtil_Light.Initialize(pHeapBase);
    m_HIOUtil_Light.GetEasyLight().Create(&m_Heap);

    /*  model  */
    m_HIOUtil_Model.Initialize(pHeapBase);
    m_HIOUtil_Model.SetHandler(this);
    m_HIOUtil_Model.SetUtilHeap(&m_Heap);

    /*  motion  */
    m_HIOUtil_Motion.Initialize(pHeapBase);
    m_HIOUtil_Motion.SetHandler(this);
    m_HIOUtil_Motion.SetUtilHeap(&m_Heap);
  }

  
  m_Scene3D.SetCamera(m_HIOUtil_Camera.GetEasyCamera());
  m_Scene3D.SetLight(m_HIOUtil_Light.GetEasyLight());

  /*  debug */
  {
    m_pGroup_Root  = gfl2::debug::DebugWin_AddGroup(m_Heap.GetDeviceHeap(), L"HIOModelView", NULL);
  }


  result	= gfl2::proc::RES_FINISH;
  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result  TestProc_HIOView::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  m_Scene3D.UnsetCamera();
  m_Scene3D.UnsetLight();

  /*  debug  */
  gfl2::debug::DebugWin_RemoveGroup(m_pGroup_Root);


  /*  HIO  */
  {
    m_HIOUtil_Camera.GetEasyCamera().Release();
    m_HIOUtil_Light.GetEasyLight().Release();

    m_HIOUtil_Camera.Finalize();
    m_HIOUtil_Light.Finalize();
    m_HIOUtil_Model.Finalize();
    m_HIOUtil_Motion.Finalize();
  }

  /*  scene  */
  m_Scene3D.Finalize();
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
gfl2::proc::Result  TestProc_HIOView::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	      result	        = gfl2::proc::RES_CONTINUE;
  gfl2::ui::DeviceManager*  pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*         pButton         = pDeviceManager->GetButton(0);


  ++m_FrameCount;

  /*  modules/scene  */
  m_HIOUtil_Camera.Update();
  m_HIOUtil_Light.Update();
  m_HIOUtil_Model.Update();
  m_HIOUtil_Motion.Update();
  m_Scene3D.OnUpdate();

  switch(m_UpdateSeq)
  {
    case  UPDATESEQ_Main:
      if(gfl2::debug::DebugWin_IsOpen())
      {
        /*  nop  */
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_UP))
      {
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))
      {
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_X))     {
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_A))     {
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))     {
        m_Scene3D.Quit();
        m_UpdateSeq   = UPDATESEQ_Quit;
      }

      break;

    case  UPDATESEQ_Quit:
      if(m_Scene3D.IsStable())
      {
        m_UpdateSeq = UPDATESEQ_Exit;
      }
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
void  TestProc_HIOView::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  m_Scene3D.OnPreDraw();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  TestProc_HIOView::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  if(m_UpdateSeq < UPDATESEQ_Quit)
  {
    m_Scene3D.OnDraw(displayNo);
  }


  {
    m_TextDrawer.CLS();
    m_TextDrawer.LOC(0,0);

    {
      u32 count = (m_FrameCount / 30) % 2;
      
      m_TextDrawer.PrintLn(L"[HIOView]%s", count ? L"." : L"");
    }


    {
      const wchar_t* pwStr  = L"";

      switch(m_HIOUtil_Model.GetHIOUtilState())
      {
        case  0:  pwStr = L"initial";   break;
        case  1:  pwStr = L"waitHost";  break;
        case  2:  pwStr = L"waitData";  break;
        case  3:  pwStr = L"reading";   break;
        case  4:  pwStr = L"readEnd";   break;

        default:  pwStr = L"unknown";   break;
      }

      m_TextDrawer.PrintLn(L"  HIO: %ls", pwStr);
    }



    m_TextDrawer.Draw(displayNo);
  }
}


/*
*/
void TestProc_HIOView::CreateHeap( void )
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  
  const size_t            appHeapSize   = 0x00100000;
  const size_t            devHeapSize   = 0x01400000;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


/*
*/
void TestProc_HIOView::DeleteHeap( void )
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
void  TestProc_HIOView::OnEvent_AddModel(HIOViewModule_Model* pSender, EasyModel& rEasyModel)
{
  m_Scene3D.AddModel_U(rEasyModel);

  /*  debug  */
  {
    
    const NetAppLib::Debug::HIOUtil::DataInfo&      rDataInfo   = pSender->GetHIODataInfo();
    
    gfl2::heap::HeapBase*         pHeapBase   = m_Heap.GetDeviceHeap();
    gfl2::debug::DebugWinGroup*   pGroup      = gfl2::debug::DebugWin_AddGroup(pHeapBase, rDataInfo.description, m_pGroup_Root);
    ModelGroupWork*               pGroupWork  = static_cast<ModelGroupWork*>(pGroup->CreateGroupWork(ModelGroupWorkBytes));


    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeapBase, L"<remove>", pGroupWork, Func_RemoveModel);
    rEasyModel.AddDebug(pGroup, pHeapBase);
    
    pGroupWork->pScene3D      = &m_Scene3D;
    pGroupWork->pModelModule  = &m_HIOUtil_Model;
    pGroupWork->pEasyModel    = &rEasyModel;
  }

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TestProc_HIOView::OnEvent_RemoveModel(HIOViewModule_Model* pSender, EasyModel& rEasyModel)
{
  m_Scene3D.RemoveModel_U(rEasyModel);
  pSender->DisposeModel(&rEasyModel);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TestProc_HIOView::OnEvent_AddMotion(HIOViewModule_Motion* pSender, const void* pData, const u32 modelHash, const u32 motionHash)
{
  EasyModel*  pModel  = m_HIOUtil_Model.FindModel(modelHash);

  if(pModel)
  {
    pModel->SetMotion(0, &m_Heap, pData, motionHash);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TestProc_HIOView::OnEvent_RemoveMotion(HIOViewModule_Motion* pSender, const u32 modelHash, const u32 motionHash)
{
  EasyModel*  pModel  = m_HIOUtil_Model.FindModel(modelHash);

  if(pModel)
  {
    pModel->UnsetMotion(motionHash);
  }
}







} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if NN_SWITCH_ENABLE_HOST_IO  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */

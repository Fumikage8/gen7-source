//==============================================================================
/**
 * @file   EggHatchingViewContainer.cpp
 * @date   2015/11/24 Tue. 12:45:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/EggHatchingWork.h"

#include  "../EggHatchingResource.h"
#include  "../Views/EggHatchingViewContainer.h"


namespace NetApp        {
namespace EggHatching   {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EggHatchingViewContainer::EggHatchingViewContainer(EggHatchingWork* pWork)
  : m_DemoHelper()
  , m_pDemoUtil(NULL)
{
  gfl2::heap::HeapBase* pHeapBase = pWork->GetAppHeap()->GetDeviceHeap();

  m_pUpperView_Demo       = GFL_NEW(pHeapBase)  UpperView_Demo(pWork);
  m_pLowerView_Selection  = GFL_NEW(pHeapBase)  LowerView_Selection(pWork);

  SetScene(SCENE_Void);

  /*  demo  */
  {
    gfl2::heap::HeapBase* pDemoParentHeapBase = gfl2::heap::Manager::GetHeapByHeapId(DEMO_PARENT_HEAP_ID);

    m_DemoHelper.Begin(pHeapBase);
    m_pDemoUtil = GFL_NEW(pHeapBase) App::AppDemo::AppDemoUtility(pDemoParentHeapBase, App::AppDemo::AppDemoUtility::HEAPMODEL_Half);

    for(u32 index = 0; index < GFL_NELEMS(m_pDemoPokemonParam); ++index)
    {
      m_pDemoPokemonParam[index]  = NULL;
    }
    /*  demo用pokepara(clone)  */
    {
      m_pDemoPokemonParam[DEMO_POKEMONPARAM_Before] = pWork->m_pAppParam->m_pTargetEgg;
      m_pDemoPokemonParam[DEMO_POKEMONPARAM_After]  = pWork->m_pAppParam->m_pTargetMons;
    }
  }

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EggHatchingViewContainer::~EggHatchingViewContainer()
{
  /*  demo  */
  GFL_SAFE_DELETE(m_pDemoUtil);
  m_DemoHelper.End();


  GFL_SAFE_DELETE(m_pLowerView_Selection);
  GFL_SAFE_DELETE(m_pUpperView_Demo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  EggHatchingViewContainer::IsDrawingAny(void) const
{
  if(
    false
    ||  m_pUpperView_Demo->IsDrawing()
    ||  m_pLowerView_Selection->IsDrawing()
  )
  {
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EggHatchingViewContainer::SetScene(const SceneID sceneID)
{
  ClearViewArray();
  switch(sceneID)
  {
    case  SCENE_Demo:
      m_pViewArray[0] = m_pUpperView_Demo;
      m_pViewArray[1] = m_pLowerView_Selection;


    case  SCENE_Void: /*  through  */
    default:
      /*  nop  */
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EggHatchingViewContainer::Update(void)
{
  if(m_pDemoUtil)
  {
    m_pDemoUtil->Update();
  }

  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->UpdateTree();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EggHatchingViewContainer::Draw(const gfl2::gfx::CtrDisplayNo dispNo)
{
  if(m_pDemoUtil)
  {
    m_pDemoUtil->PreDraw();
    m_pDemoUtil->Draw(dispNo);
  }


  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->DrawTree(dispNo);
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
void  EggHatchingViewContainer::ClearViewArray(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    m_pViewArray[index] = NULL;
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
void  EggHatchingViewContainer::SetupDemoParam(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  pViewSystem->SetPokemonParam(0, m_pDemoPokemonParam[DEMO_POKEMONPARAM_After]);
}


} /*  namespace EggHatching    */
} /*  namespace NetApp  */

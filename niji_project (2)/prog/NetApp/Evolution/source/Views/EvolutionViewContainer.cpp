
//==============================================================================
/**
 * @file   EvolutionViewContainer.cpp
 * @date   2015/08/28 Fri. 18:27:44
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/EvolutionWork.h"

#include  "../EvolutionResource.h"
#include  "../Views/EvolutionViewContainer.h"


namespace NetApp      {
namespace Evolution   {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EvolutionViewContainer::EvolutionViewContainer(EvolutionWork* pWork)
  : m_pWork(pWork)
  , m_DemoHelper()
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
    m_pDemoPokemonParam[DEMO_POKEMONPARAM_Before] = GFL_NEW(pHeapBase)  pml::pokepara::PokemonParam(pHeapBase, *(pWork->m_pAppParam->m_pTargetPokemon));
    m_pDemoPokemonParam[DEMO_POKEMONPARAM_After]  = GFL_NEW(pHeapBase)  pml::pokepara::PokemonParam(pHeapBase, *m_pDemoPokemonParam[DEMO_POKEMONPARAM_Before]);
    if(pWork->m_pAppParam->m_MonsNoEvolve != m_pDemoPokemonParam[DEMO_POKEMONPARAM_Before]->GetMonsNo())
    {
      m_pDemoPokemonParam[DEMO_POKEMONPARAM_After]->Evolve(pWork->m_pAppParam->m_MonsNoEvolve, pWork->m_pAppParam->m_EvolveRootNum);
    }
  }

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EvolutionViewContainer::~EvolutionViewContainer()
{
  /*  demo  */
  for(u32 index = 0; index < GFL_NELEMS(m_pDemoPokemonParam); ++index)
  {
    GFL_SAFE_DELETE(m_pDemoPokemonParam[index])
  }
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
bool  EvolutionViewContainer::IsDrawingAny(void) const
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
void  EvolutionViewContainer::SetScene(const SceneID sceneID)
{
  ClearViewArray();
  switch(sceneID)
  {
    case  SCENE_EvolutionDemo:
      m_pViewArray[0] = m_pUpperView_Demo;
      m_pViewArray[1] = m_pLowerView_Selection;
      break;

    case  SCENE_CheckZukan:
      break;

    case  SCENE_CheckWaza:
      m_pViewArray[0] = m_pUpperView_Demo;
      m_pViewArray[1] = m_pLowerView_Selection;
      break;


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
void  EvolutionViewContainer::Update(void)
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
void  EvolutionViewContainer::Draw(const gfl2::gfx::CtrDisplayNo dispNo)
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
void  EvolutionViewContainer::ClearViewArray(void)
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
void  EvolutionViewContainer::SetupDemoParam(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  pViewSystem->SetPokemonParam(0, m_pDemoPokemonParam[DEMO_POKEMONPARAM_Before]);
  pViewSystem->SetPokemonParam(1, m_pDemoPokemonParam[DEMO_POKEMONPARAM_After]);
//  pViewSystem->SetPokemonParam(2, m_pDemoPokemonParam[DEMO_POKEMONPARAM_Before]);
}


//------------------------------------------------------------------
/**
  *  @brief    進化後のポケモンがAlola種であるか
  */
//------------------------------------------------------------------
bool  EvolutionViewContainer::CheckEvolvedPokemonIsAlolaSpecies(void) const
{
  return APP_PARAM::IsAlolaSpecies(*m_pDemoPokemonParam[DEMO_POKEMONPARAM_After]);
}


} /*  namespace Evolution    */
} /*  namespace NetApp  */

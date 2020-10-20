//==============================================================================
/**
 * @file   LiveViewContainer.cpp
 * @date   2015/07/17 Fri. 16:20:47
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../ApplicationSystem/LiveWork.h"

#include  "./LiveViewContainer.h"


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LiveViewContainer::LiveViewContainer(LiveWork* pWork)
  : m_EffectBG()
{
  gfl2::heap::HeapBase* pHeapBase = pWork->GetAppHeap()->GetDeviceHeap();


  m_pUpperView_Message    = GFL_NEW(pHeapBase)  UpperView_Message(pWork);
  m_pUpperView_Main       = GFL_NEW(pHeapBase)  UpperView_Main(pWork);
  m_pLowerView_Selection  = GFL_NEW(pHeapBase)  LowerView_Selection(pWork);

  {
    u32 resourceIDArray[1] = { RESOURCE_BG };

    m_EffectBG.Setup(pWork, resourceIDArray, 1);
    m_EffectBG.SetUpperCamera(
      0.0f, 0.0f,  20.0f,
      0.0f, 0.0f,   0.0f
    );
    m_EffectBG.SetLowerCamera(
      0.0f, 0.0f,  20.0f,
      0.0f, 0.0f,   0.0f
    );
    
    m_EffectBG.PushClearColor(0,0,0,1);
  }

  SetScene(SCENE_Void);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LiveViewContainer::~LiveViewContainer()
{
  m_EffectBG.PopClearColor();
  m_EffectBG.Cleanup();
  GFL_SAFE_DELETE(m_pLowerView_Selection);
  GFL_SAFE_DELETE(m_pUpperView_Main);
  GFL_SAFE_DELETE(m_pUpperView_Message);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveViewContainer::SetAllInputEnable(const bool isEnable)
{
  m_pLowerView_Selection  ->SetInputEnabled(isEnable);
  m_pUpperView_Main       ->SetInputEnabled(isEnable);
  m_pUpperView_Message    ->SetInputEnabled(isEnable);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  LiveViewContainer::IsDrawingAny(void) const
{
  if(
    m_pUpperView_Message->IsDrawing()
    ||  m_pUpperView_Main->IsDrawing()
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
void  LiveViewContainer::SetScene(const SceneID sceneID)
{
  ClearViewArray();

  switch(sceneID)
  {
    case  SCENE_MainMenu:
      m_pViewArray[0] = m_pUpperView_Message;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_ReceivePlayerPass:
      m_pViewArray[0] = m_pUpperView_Message;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_Registration:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(true);
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_CheckRemainingMatches:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(true);
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_MatchMenu:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(true);
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_ReplayMatch:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(true);
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_Deregistration:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(true);
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_Matching:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(true);
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
      break;

    case  SCENE_MatchResult:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Selection;
      m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(true);
      m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(true);
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
void  LiveViewContainer::Update(void)
{
  m_EffectBG.Update();
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->UpdateTree();
  }

  if(m_pUpperView_Main)       m_pUpperView_Main     ->ApplyTrainerIconVisibility();
  if(m_pLowerView_Selection)  m_pLowerView_Selection->ApplyTrainerIconVisibility();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveViewContainer::Draw(const gfl2::gfx::CtrDisplayNo dispNo)
{
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
void  LiveViewContainer::ClearViewArray(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    m_pViewArray[index] = NULL;
  }

  m_pUpperView_Main     ->SetAllTrainerIconVisibilityExternal(false);
  m_pLowerView_Selection->SetAllTrainerIconVisibilityExternal(false);
}



} /*  namespace Live    */
} /*  namespace NetApp  */

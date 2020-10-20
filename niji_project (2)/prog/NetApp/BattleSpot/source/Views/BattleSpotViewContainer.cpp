
//==============================================================================
/**
 * @file   BattleSpotViewContainer.cpp
 * @date   2015/10/02 Fri. 13:09:21
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/BattleSpotWork.h"

#include  "../BattleSpotResource.h"
#include  "../Views/BattleSpotViewContainer.h"


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
BattleSpotViewContainer::BattleSpotViewContainer(BattleSpotWork* pWork)
  : m_EffectBG()
{
  gfl2::heap::HeapBase* pHeapBase = pWork->GetAppHeap()->GetDeviceHeap();

  /*  views  */
  m_pUpperView_Information  = GFL_NEW(pHeapBase)  UpperView_Information(pWork);
  m_pLowerView_Selection    = GFL_NEW(pHeapBase)  LowerView_Selection(pWork);
  m_pLowerView_Card         = GFL_NEW(pHeapBase)  LowerView_Card(pWork);

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
BattleSpotViewContainer::~BattleSpotViewContainer()
{
  m_EffectBG.PopClearColor();
  m_EffectBG.Cleanup();
  GFL_SAFE_DELETE(m_pLowerView_Card);
  GFL_SAFE_DELETE(m_pLowerView_Selection);
  GFL_SAFE_DELETE(m_pUpperView_Information);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  BattleSpotViewContainer::SetAllInputEnabled( bool enable )
{
  m_pLowerView_Card->SetInputEnabled( enable );
  m_pLowerView_Selection->SetInputEnabled( enable );
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  BattleSpotViewContainer::IsDrawingAny(void) const
{
  if(
    false
    ||  m_pUpperView_Information->IsDrawing()
    ||  m_pLowerView_Selection->IsDrawing()
    ||  m_pLowerView_Card->IsDrawing()
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
void  BattleSpotViewContainer::SetScene(const SceneID sceneID)
{
  ClearViewArray();
  switch(sceneID)
  {
    case  SCENE_Welcome:
      m_pViewArray[0] = m_pUpperView_Information;
      m_pViewArray[1] = m_pLowerView_Selection;
      break;

    /*  freebattle  */
    case  SCENE_FreeBattleEntry:
      m_pViewArray[0] = m_pUpperView_Information;
      m_pViewArray[1] = m_pLowerView_Selection;
      break;

    /*  ratingBattle  */
    case  SCENE_RatingBattleEntry:
      m_pViewArray[0] = m_pUpperView_Information;
      m_pViewArray[1] = m_pLowerView_Selection;
      break;

    /*  internetMatch  */
    case  SCENE_InternetMatchEntry:
      m_pViewArray[0] = m_pUpperView_Information;
      m_pViewArray[1] = m_pLowerView_Selection;
      break;

    case  SCENE_InternetMatch_Matching:
      m_pViewArray[0] = m_pUpperView_Information;
      m_pViewArray[1] = m_pLowerView_Card;
      m_pLowerView_Card->SetAllTrainerIconVisibilityExternal(true);
      break;

    /*  friendMatch  */
    case  SCENE_FriendMatchEntry:
      m_pViewArray[0] = m_pUpperView_Information;
      m_pViewArray[1] = m_pLowerView_Selection;
      break;

    case  SCENE_FriendMatch_Matching:
      m_pViewArray[0] = m_pUpperView_Information;
      m_pViewArray[1] = m_pLowerView_Card;
      m_pLowerView_Card->SetAllTrainerIconVisibilityExternal(true);
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
void  BattleSpotViewContainer::Update(void)
{
  m_EffectBG.Update();
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->UpdateTree();
  }

  if(m_pLowerView_Card) m_pLowerView_Card->ApplyTrainerIconVisibility();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  BattleSpotViewContainer::Draw(const gfl2::gfx::CtrDisplayNo dispNo)
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
void  BattleSpotViewContainer::ClearViewArray(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    m_pViewArray[index] = NULL;
  }

  m_pLowerView_Card->SetAllTrainerIconVisibilityExternal(false);
}


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */

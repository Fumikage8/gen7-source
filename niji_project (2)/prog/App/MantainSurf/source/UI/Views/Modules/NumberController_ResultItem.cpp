//==============================================================================
/**
 * @file   NumberController_ResultItem.cpp
 * @date   2016/11/18 Fri. 16:53:39
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./NumberController_ResultItem.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_NumberController_ResultItem
namespace FILELOCAL
{
  static const u32  InterpolateFrame  = 3;
}


// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController_ResultItem::Update(void)
{
  this->NumberController::Update(); /*  super  */
  if(m_bTargetValueSuspended)
  {
    if(m_pG2DUtil && (m_AppearAnimID != UINT_MAX))
    {
      if(m_pG2DUtil->IsAnimeEnd(m_LayoutID, m_AppearAnimID))
      {
        m_bTargetValueSuspended = false;
      }
    }
    else
    {
        m_bTargetValueSuspended = false;
    }
    
    /*  on->offになったら数値演出開始  */
    if(!m_bTargetValueSuspended)
    {
      SetInterpolateFrame(FILELOCAL::InterpolateFrame);
      SetTargetValue(m_SuspendedTargetValue);
    }
  }

  /*  登場演出後は数値表示  */
  if(m_pNumberGroupPane)
  {
    if(!m_bTargetValueSuspended)
    {
      m_pNumberGroupPane->SetVisible(true);
    }
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
void  NumberController_ResultItem::SetTargetValueAfterAnim(const u32 targetValue)
{
  m_SuspendedTargetValue  = targetValue;
  m_bTargetValueSuspended = true;
  SetTargetValue(0, true);

  if(m_pG2DUtil && (m_AppearAnimID != UINT_MAX))
  {
    m_pG2DUtil->StartAnime(m_LayoutID, m_AppearAnimID);
  }
  /*  登場演出中は数値非表示  */
  if(m_pNumberGroupPane)
  {
    m_pNumberGroupPane->SetVisible(false);
  }
}



} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

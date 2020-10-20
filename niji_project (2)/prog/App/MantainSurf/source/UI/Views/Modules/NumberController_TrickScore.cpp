//==============================================================================
/**
 * @file   NumberController_TrickScore.cpp
 * @date   2017/01/18 Wed. 12:59:51
 * @author muto_yu
 * @brief  上画面技名表示に付随するスコア表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./NumberController_TrickScore.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_NumberController_TrickScore
namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController_TrickScore::Update(void)
{
  this->NumberController::Update(); /*  super  */
  UpdatePlusSign();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController_TrickScore::UpdatePlusSign(void)
{
  if(!m_pG2DUtil) return;

  const u32 layoutID  = 0;
  const u32 anim      = m_AnimID[ANIMID_PlusSign];

  if(anim != ANIMID_INVALID)
  {
    if(m_pG2DUtil->IsAnimeEnd(layoutID, anim)) m_pG2DUtil->StartAnime(layoutID, anim, false);
    
    const u32 currentValue  = GetValue();
    const u32 currentDigits = static_cast<u32>(log10f(currentValue));

    m_pG2DUtil->SetAnimeFrame(layoutID, anim, currentDigits);
  }
}


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

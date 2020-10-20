//==============================================================================
/**
 * @file   NumberController_ResultTotal.cpp
 * @date   2016/11/16 Wed. 17:10:09
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./NumberController_ResultTotal.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_NumberController_ResultTotal
namespace FILELOCAL
{
  static const u32  InterpolateFrame = 30;
}


// =============================================================================
/**/
//==============================================================================
NumberController_ResultTotal::NumberController_ResultTotal(const u32 digits)
  : NumberController(digits)
{
  SetInterpolateFrame(FILELOCAL::InterpolateFrame);
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController_ResultTotal::Update(void)
{
  this->NumberController::Update(); /*  super  */
}


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

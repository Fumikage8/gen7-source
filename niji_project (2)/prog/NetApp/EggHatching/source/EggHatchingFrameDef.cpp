//==============================================================================
/**
 * @file   EggHatchingFrameDef.cpp
 * @date   2015/11/24 Tue. 13:50:09
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "./System/EggHatchingWork.h"

#include  "./EggHatchingFrameDef.h"


namespace NetApp    {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(EggHatchingWork* pWork, const NetAppLib::System::FrameResult frameResult)
{
  switch(frameResult)
  {
    /*
      demo
    */
    case  RESULT_DEMO_Hatched:      return  FRAME_INVALID;
  }
  return  FRAME_INVALID; 
}




} /*  namespace Evolution */
} /*  namespace NetApp    */

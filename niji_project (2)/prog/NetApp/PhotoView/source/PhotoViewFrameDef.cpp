

//==============================================================================
/**
 * @file   PhotoViewFrameDef.cpp
 * @date   2017/02/07 Tue. 12:52:57
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./PhotoViewFrameDef.h"


namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewFrameDef
namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(const NetAppLib::System::FrameResult frameResult)
{
  switch(frameResult)
  {
    /*  main  */
    case  RESULT_MAIN_Exit:     return  FRAME_INVALID;
    case  RESULT_MAIN_Cancel:   return  FRAME_INVALID;


    default:
      return  FRAME_INVALID;
  }
}


}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */


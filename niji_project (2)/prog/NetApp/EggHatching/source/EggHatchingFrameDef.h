#if !defined(EGGHATCHINGFRAMEDEF_H_INCLUDED)
#define EGGHATCHINGFRAMEDEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingFrameDef.h
 * @date   2015/11/24 Tue. 13:47:13
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"


namespace NetApp      {
namespace EggHatching {
  class EggHatchingWork;
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
enum  FrameID
{
  FRAME_Demo    = NetAppLib::System::FRAME_ID_START,

  FRAME_INVALID = NetAppLib::System::FRAME_ID_INVALID,
  FRAME_STARTUP = FRAME_Demo
};


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
enum  FrameResult
{
  /*  evolutionDemo  */
  RESULT_DEMO_Hatched,


  RESULT_INVALID  = NetAppLib::System::FRAME_RESULT_INVALID,
  RESULT_
};




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(EggHatchingWork* pWork, const NetAppLib::System::FrameResult frameResult);



} /*  namespace EggHatching */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EGGHATCHINGFRAMEDEF_H_INCLUDED)  */

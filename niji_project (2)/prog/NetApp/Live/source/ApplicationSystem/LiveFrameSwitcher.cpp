//==============================================================================
/**
 * @file   LiveFrameSwitcher.cpp
 * @date   2015/07/09 Thu. 14:44:01
 * @author muto_yu
 * @brief  Live大会 FrameSwitcher
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../LiveFrameDef.h"
#include  "./LiveFrameSwitcher.h"


// =============================================================================
/**/
//==============================================================================


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     FrameSwitcher
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameSwitcher::FrameSwitcher(void)
{
}


//------------------------------------------------------------------
/**
  *  @func     ~FrameSwitcher
  *  @brief    dtor
  */
//------------------------------------------------------------------
FrameSwitcher::~FrameSwitcher()
{
}


/*
  implement IApplicationFrameSwitcher I/F
*/
//------------------------------------------------------------------
/**
  *  @func     GetNextFrameID
  *  @brief    遷移先FrameのIDを返す
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID
  FrameSwitcher::GetNextFrameID(
    NetAppLib::System::FrameID      executeFrameID,
    NetAppLib::System::FrameResult  frameResult
  )
{
  return GetNextFrameIDCore(frameResult);
}

/*
  end IApplicationFrameSwitcher I/F
*/


} /*  namespace Live */
} /*  namespace NetApp      */

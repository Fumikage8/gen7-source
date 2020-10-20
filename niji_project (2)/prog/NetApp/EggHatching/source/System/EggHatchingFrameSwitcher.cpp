
//==============================================================================
/**
 * @file   EggHatchingFrameSwitcher.cpp
 * @date   2015/11/24 Tue. 12:40:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../EggHatchingFrameDef.h"
#include  "./EggHatchingWork.h"

#include  "./EggHatchingFrameSwitcher.h"


namespace NetApp      {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     FrameSwitcher
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameSwitcher::FrameSwitcher(EggHatchingWork* pWork)
  : m_pWork(pWork)
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
  return GetNextFrameIDCore(m_pWork, frameResult);
}

/*
  end IApplicationFrameSwitcher I/F
*/


} /*  namespace EggHatching */
} /*  namespace NetApp      */

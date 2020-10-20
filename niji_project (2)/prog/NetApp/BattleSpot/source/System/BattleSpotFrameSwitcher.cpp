//==============================================================================
/**
 * @file   BattleSpotFrameSwitcher.cpp
 * @date   2015/10/02 Fri. 12:50:14
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "../BattleSpotFrameDef.h"
#include  "./BattleSpotWork.h"

#include  "./BattleSpotFrameSwitcher.h"


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     FrameSwitcher
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameSwitcher::FrameSwitcher(BattleSpotWork* pWork)
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


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */



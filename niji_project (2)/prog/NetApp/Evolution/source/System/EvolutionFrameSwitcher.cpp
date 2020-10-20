//==============================================================================
/**
 * @file   EvolutionFrameSwitcher.cpp
 * @date   2015/08/28 Fri. 17:50:13
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../EvolutionFrameDef.h"
#include  "./EvolutionWork.h"

#include  "./EvolutionFrameSwitcher.h"


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     FrameSwitcher
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameSwitcher::FrameSwitcher(EvolutionWork* pWork)
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


} /*  namespace Evolution */
} /*  namespace NetApp    */



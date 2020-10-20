//==============================================================================
/**
 * @file   GiftFrameSwitcher.cpp
 * @date   2015/07/31 Fri. 15:48:33
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../GiftFrameDef.h"
#include  "./GiftWork.h"
#include  "./GiftFrameSwitcher.h"


// =============================================================================
/**/
//==============================================================================


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     FrameSwitcher
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameSwitcher::FrameSwitcher(GiftWork* pWork)
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
  if(m_pWork->NetworkErrorHandling())
  {
    /*  内部Fatalへ  */
    m_pWork->m_pAppParam->procResult  = APP_PARAM::PROCRESULT_InternalNetworkErrorOccured;
    return FRAME_INVALID;
  }

  return GetNextFrameIDCore(frameResult);
}

/*
  end IApplicationFrameSwitcher I/F
*/


} /*  namespace Gift */
} /*  namespace NetApp      */

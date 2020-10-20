
//==============================================================================
/**
 * @file   QRReaderFrameSwitcher.cpp
 * @date   2016/01/14 Thu. 17:42:08
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================



// =============================================================================
/**/
//==============================================================================
#include  "../QRReaderFrameDef.h"
#include  "./QRReaderWork.h"

#include  "./QRReaderFrameSwitcher.h"


namespace NetApp   {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     FrameSwitcher
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameSwitcher::FrameSwitcher(QRReaderWork* pWork)
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


} /*  namespace QRReader */
} /*  namespace NetApp      */

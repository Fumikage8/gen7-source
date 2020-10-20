
//==============================================================================
/**
 * @file   QRReaderFrameDef.cpp
 * @date   2016/01/14 Thu. 11:46:33
 * @author muto_yu
 * @brief  QR読み取り
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./System/QRReaderWork.h"

#include  "./QRReaderFrameDef.h"


namespace NetApp    {
namespace QRReader  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(QRReaderWork* pWork, const NetAppLib::System::FrameResult frameResult)
{
/*
  FRAME_Tutorial,
  FRAME_QRRead,
  FRAME_FieldScan,
*/
  switch(frameResult)
  {
    case  RESULT_Tutorial_Exit:               return  FRAME_QRRead;
    case  RESULT_QRRead_FieldScan:            return  FRAME_FieldScan;
    case  RESULT_FieldScan_QRRead:            return  FRAME_QRRead;
    case  RESULT_FieldScan_FieldScanRunning:  return  FRAME_FieldScanRunning;
    case  RESULT_FieldScanRunning_QRRead:     return  FRAME_QRRead;


    case  RESULT_QRRead_Exit:
    case  RESULT_QRSimpleRead_Exit:
    case  RESULT_QRSimpleRead_OK:
    case  RESULT_FieldScan_Exit:
    default:
      Sound::PlaySE(SEQ_SE_SYS_NJ_023);
      return  FRAME_INVALID;
  }
//  return  FRAME_INVALID; 
}




} /*  namespace QRReader  */
} /*  namespace NetApp    */

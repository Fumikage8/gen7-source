#if !defined(QRREADERFRAMEDEF_H_INCLUDED)
#define QRREADERFRAMEDEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderFrameDef.h
 * @date   2016/01/14 Thu. 11:49:56
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


namespace NetApp   {
namespace QRReader {
  class QRReaderWork;
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
  FRAME_Tutorial,
  FRAME_QRRead,
  FRAME_FieldScan,
  FRAME_FieldScanRunning,

  FRAME_QRSimpleRead,

  FRAME_INVALID = NetAppLib::System::FRAME_ID_INVALID
};


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
enum  FrameResult
{
  /*  tutorial  */
  RESULT_Tutorial_Exit,

  /*  QRRead    */
  RESULT_QRRead_Exit,
  RESULT_QRRead_FieldScan,

  /*  FieldScan */
  RESULT_FieldScan_Exit,
  RESULT_FieldScan_QRRead,
  RESULT_FieldScan_FieldScanRunning,

  /*  FieldScanRunning  */
  RESULT_FieldScanRunning_Exit,
  RESULT_FieldScanRunning_QRRead,

  /*  Simple  */
  RESULT_QRSimpleRead_Exit,
  RESULT_QRSimpleRead_OK,

  RESULT_INVALID  = NetAppLib::System::FRAME_RESULT_INVALID,
  RESULT_
};




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(QRReaderWork* pWork, const NetAppLib::System::FrameResult frameResult);



} /*  namespace QRReader */
} /*  namespace NetApp    */
#endif  /*  #if !defined(QRREADERFRAMEDEF_H_INCLUDED)  */


#if !defined(PHOTOVIEWFRAMEDEF_H_INCLUDED)
#define PHOTOVIEWFRAMEDEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewFrameDef.h
 * @date   2017/02/07 Tue. 12:45:48
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h>


namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    各フレームのID
  *  @note    NetAppLib::System::FrameIDとの互換を取る
  */
//------------------------------------------------------------------
enum  FrameID
{
  FRAME_Main    = NetAppLib::System::FRAME_ID_START,

  FRAME_INVALID = NetAppLib::System::FRAME_ID_INVALID,
  FRAME_STARTUP = FRAME_Main
};



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief   各フレームの結果定義
  *  @note    NetAppLib::System::FrameResultとの互換を取る
  */
//------------------------------------------------------------------
enum  FrameResult
{
  /*  main  */
  RESULT_MAIN_Exit,
  RESULT_MAIN_Cancel,

  RESULT_INVALID  = NetAppLib::System::FRAME_RESULT_INVALID,
  RESULT_
};


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(const NetAppLib::System::FrameResult frameResult);




}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWFRAMEDEF_H_INCLUDED)  */


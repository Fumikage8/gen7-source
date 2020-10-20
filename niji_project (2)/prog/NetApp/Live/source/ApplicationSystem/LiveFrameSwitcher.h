#if !defined(LIVEFRAMESWITCHER_H_INCLUDED)
#define LIVEFRAMESWITCHER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrameSwitcher.h
 * @date   2015/07/09 Thu. 15:13:23
 * @author muto_yu
 * @brief  Live大会FrameSwitcher
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  FrameSwitcher
  *  @brief  Frameの切り替えを行う
  *  @date   2015.05.14 Thu.
  */
//==============================================================================
class FrameSwitcher : public NetAppLib::System::IApplicationFrameSwitcher
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameSwitcher );

public:
  //------------------------------------------------------------------
  /**
    *  @func     FrameSwitcher
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  FrameSwitcher(void);


  //------------------------------------------------------------------
  /**
    *  @func     ~FrameSwitcher
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~FrameSwitcher();


  /*
    implement IApplicationFrameSwitcher I/F
  */
  //------------------------------------------------------------------
  /**
    *  @func     GetNextFrameID
    *  @brief    繊維先FrameのIDを返す
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::FrameID
            GetNextFrameID(
              NetAppLib::System::FrameID      executeFrameID,
              NetAppLib::System::FrameResult  frameResult
            );


private:
};


} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEFRAMESWITCHER_H_INCLUDED)  */

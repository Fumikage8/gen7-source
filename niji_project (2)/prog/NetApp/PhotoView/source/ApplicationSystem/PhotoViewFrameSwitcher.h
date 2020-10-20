
#if !defined(PHOTOVIEWFRAMESWITCHER_H_INCLUDED)
#define PHOTOVIEWFRAMESWITCHER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewFrameSwitcher.h
 * @date   2017/02/07 Tue. 13:18:16
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h>


namespace NetApp  {
namespace PhotoView {
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
class FrameSwitcher
  : public  NetAppLib::System::IApplicationFrameSwitcher
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


} /*  namespace PhotoView    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWFRAMESWITCHER_H_INCLUDED)  */

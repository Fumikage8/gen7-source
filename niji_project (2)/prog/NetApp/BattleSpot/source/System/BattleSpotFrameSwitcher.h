
#if !defined(BATTLESPOTFRAMESWITCHER_H_INCLUDED)
#define BATTLESPOTFRAMESWITCHER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrameSwitcher.h
 * @date   2015/10/02 Fri. 12:56:51
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
#include  "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  FrameSwitcher
  *  @brief  Frameの切り替えを行う
  */
//==============================================================================
class FrameSwitcher
  : public NetAppLib::System::IApplicationFrameSwitcher
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameSwitcher );

public:
  //------------------------------------------------------------------
  /**
    *  @func     FrameSwitcher
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  FrameSwitcher(BattleSpotWork* pWork);


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
  BattleSpotWork*  m_pWork;
};



} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAMESWITCHER_H_INCLUDED)  */



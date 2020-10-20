
#if !defined(EVOLUTIONFRAMESWITCHER_H_INCLUDED)
#define EVOLUTIONFRAMESWITCHER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionFrameSwitcher.h
 * @date   2015/08/28 Fri. 18:18:58
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


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  FrameSwitcher
  *  @brief  Frameの切り替えを行う
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
  FrameSwitcher(EvolutionWork* pWork);


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
  EvolutionWork*  m_pWork;
};



} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONFRAMESWITCHER_H_INCLUDED)  */



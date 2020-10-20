#if !defined(BATTLESPOTFRAMEFACTORY_H_INCLUDED)
#define BATTLESPOTFRAMEFACTORY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrameFactory.h
 * @date   2015/10/02 Fri. 12:56:11
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

#include  "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================
class FrameFactory
  : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameFactory );

public:

  //------------------------------------------------------------------
  /**
    *  @func     FrameFactory
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  FrameFactory(BattleSpotWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~FrameFactory
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~FrameFactory();

  /*
    implement IApplciationFrameFactory
  */
  //------------------------------------------------------------------
  /**
    *  @func     CreateFrame
    *  @brief    Frameの生成
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::NetAppFrameBase*   CreateNetAppFrame(NetAppLib::System::FrameID  frameID);

private:
  BattleSpotWork*  m_pWork;
};



} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAMEFACTORY_H_INCLUDED)  */
